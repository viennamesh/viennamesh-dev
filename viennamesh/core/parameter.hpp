#ifndef VIENNAMESH_CORE_PARAMETER_HPP
#define VIENNAMESH_CORE_PARAMETER_HPP

#include <vector>

#include "viennamesh/forwards.hpp"
#include "viennamesh/utils/logger.hpp"


namespace viennamesh
{
  class BaseParameter;

  template<typename T>
  class ParameterWrapper;


  template<typename MeshT, typename SegmentationT>
  struct SegmentedMesh
  {
    typedef MeshT mesh_type;
    typedef SegmentationT segmentation_type;

    mesh_type mesh;
    segmentation_type segmentation;
  };

  typedef shared_ptr<BaseParameter> ParameterHandle;
  typedef shared_ptr<const BaseParameter> ConstParameterHandle;


  namespace result_of
  {
    template<typename ValueT>
    struct parameter_handle
    {
      typedef shared_ptr< ParameterWrapper<ValueT> > type;
    };

    template<typename ValueT>
    struct const_parameter_handle
    {
      typedef shared_ptr< const ParameterWrapper<ValueT> > type;
    };

    template<typename ValueT>
    struct parameter_handle<const ValueT>
    {
      typedef typename const_parameter_handle<ValueT>::type type;
    };
  }

  typedef result_of::parameter_handle<bool>::type BoolParameterHandle;
  typedef result_of::const_parameter_handle<bool>::type ConstBoolParameterHandle;

  typedef result_of::parameter_handle<int>::type IntParameterHandle;
  typedef result_of::const_parameter_handle<int>::type ConstIntParameterHandle;

  typedef result_of::parameter_handle<double>::type DoubleParameterHandle;
  typedef result_of::const_parameter_handle<double>::type ConstDoubleParameterHandle;

  typedef result_of::parameter_handle<string>::type StringParameterHandle;
  typedef result_of::const_parameter_handle<string>::type ConstStringParameterHandle;


  namespace detail
  {
    template<typename ValueT, typename SourceT>
    struct dynamic_handle_cast_impl
    {
      static typename result_of::parameter_handle<ValueT>::type cast( shared_ptr<SourceT> const & ptr )
      {
        return dynamic_pointer_cast< ParameterWrapper<ValueT> >(ptr);
      }
    };

    template<typename ValueT, typename SourceT>
    struct dynamic_handle_cast_impl<const ValueT, SourceT>
    {
      static typename result_of::const_parameter_handle<ValueT>::type cast( shared_ptr<SourceT> const & ptr )
      {
        return dynamic_pointer_cast< const ParameterWrapper<ValueT> >(ptr);
      }
    };
  }


  template<typename ValueT, typename SourceT>
  typename result_of::parameter_handle<ValueT>::type dynamic_handle_cast( shared_ptr<SourceT> const & ptr )
  {
    return detail::dynamic_handle_cast_impl<ValueT, SourceT>::cast(ptr);
  }

  template<typename ValueT>
  typename result_of::parameter_handle<ValueT>::type make_parameter()
  {
    return typename result_of::parameter_handle<ValueT>::type( new ParameterWrapper<ValueT>() );
  }

  template<typename ValueT>
  typename result_of::parameter_handle<ValueT>::type make_parameter( ValueT const & value )
  {
    return typename result_of::parameter_handle<ValueT>::type( new ParameterWrapper<ValueT>(value) );
  }



  class TypeProperties
  {
  public:

    typedef std::map<string, string> PropertyMap;


    PropertyMap & get_map(type_info_wrapper const & ti)
    {
      return type_properties_map[ti];
    }

    std::pair<string, bool> get_property( type_info_wrapper const & ti, string const & name ) const
    {
      std::map<type_info_wrapper, PropertyMap>::const_iterator tpit = type_properties_map.find(ti);
      if (tpit != type_properties_map.end())
      {
        PropertyMap::const_iterator pmit = tpit->second.find(name);
        if (pmit != tpit->second.end())
        {
          return std::pair<string, bool>(pmit->second, true);
        }
      }

      return std::pair<string, bool>("", false);
    }

    void set_property( type_info_wrapper const & ti, string const & key, string const & value )
    {
      get_map(ti)[key] = value;
    }

    template<typename ValueT>
    void set_property( string const & key, string const & value )
    {
      set_property( typeid( ParameterWrapper<ValueT> ), key, value );
    }

    bool match_properties( type_info_wrapper const & ti, PropertyMap const & to_match ) const
    {
      if (to_match.empty())
        return true;

      std::map<type_info_wrapper, PropertyMap>::const_iterator pmit = type_properties_map.find(ti);

      if (pmit == type_properties_map.end())
        return false;

      PropertyMap const & properties = pmit->second;

      for (PropertyMap::const_iterator it = to_match.begin(); it != to_match.end(); ++it)
      {
        PropertyMap::const_iterator pit = properties.find( it->first );
        if ( pit == properties.end() )
          return false;

        if ( it->second != pit->second )
          return false;
      }

      return true;
    }

    bool match_property( type_info_wrapper const & ti, string const & key, string const & value ) const
    {
      std::map<type_info_wrapper, PropertyMap>::const_iterator pmit = type_properties_map.find(ti);

      if (pmit == type_properties_map.end())
        return false;

      PropertyMap const & properties = pmit->second;

      PropertyMap::const_iterator pit = properties.find(key);
      if ( pit == properties.end() )
        return false;

      return pit->second == value;
    }


    void print() const
    {
      for (std::map<type_info_wrapper, PropertyMap>::const_iterator tit = type_properties_map.begin(); tit != type_properties_map.end(); ++tit)
      {
        std::cout << "Type: " << tit->first.name() << std::endl;
        for (PropertyMap::const_iterator pit = tit->second.begin(); pit != tit->second.end(); ++pit)
        {
          std::cout << "  " << pit->first << " - " << pit->second << std::endl;
        }
      }
    }


    static TypeProperties & get()
    {
      static TypeProperties type_properties_;
      return type_properties_;
    }

  private:
    std::map<type_info_wrapper, PropertyMap> type_properties_map;
  };









  struct BaseConversionFunction
  {
    BaseConversionFunction(int function_depth_) : function_depth(function_depth_) {}
    virtual ~BaseConversionFunction() {}

    virtual bool convert( ConstParameterHandle const &, ParameterHandle const & ) const = 0;
    virtual ParameterHandle get_converted( ConstParameterHandle const & ) const = 0;

    virtual type_info_wrapper input_type() const = 0;
    virtual type_info_wrapper output_type() const = 0;

    int function_depth;
  };

  template<typename InputValueT, typename OutputValueT>
  struct ConversionFunction : BaseConversionFunction
  {
    typedef ParameterWrapper<InputValueT> InputParameterType;
    typedef ParameterWrapper<OutputValueT> OutputParameterType;

    ConversionFunction() : BaseConversionFunction(1) {}
    ConversionFunction( function<bool (InputValueT const &, OutputValueT &)> const & f) :
        BaseConversionFunction(1), convert_function(f) {}

    virtual bool convert( ConstParameterHandle const & input, ParameterHandle const & output ) const
    {
#ifdef DEBUG
      return convert_function( dynamic_cast<InputParameterType const &>(*input).value, dynamic_cast<OutputParameterType &>(*output).value );
#else
      return convert_function( static_cast<InputParameterType const &>(*input).value, static_cast<OutputParameterType &>(*output).value );
#endif
    }

    virtual ParameterHandle get_converted( ConstParameterHandle const & input ) const
    {
      shared_ptr<OutputParameterType> result( new OutputParameterType() );
      if (convert(input, result))
        return result;
      return shared_ptr<OutputParameterType>();
    }

    virtual type_info_wrapper input_type() const
    { return type_info_wrapper::make<InputParameterType>(); }

    virtual type_info_wrapper output_type() const
    { return type_info_wrapper::make<OutputParameterType>(); }


    function<bool (InputValueT const &, OutputValueT &)> convert_function;
  };

  struct DualConversionFunction : BaseConversionFunction
  {
    DualConversionFunction( shared_ptr<BaseConversionFunction> const & first_, shared_ptr<BaseConversionFunction> const & second_ ) :
        BaseConversionFunction(first_->function_depth + second_->function_depth), first(first_), second(second_) {}

    virtual bool convert( ConstParameterHandle const & input, ParameterHandle const & output ) const
    {
      return second->convert( first->get_converted(input), output );
    }

    virtual ParameterHandle get_converted( ConstParameterHandle const & input ) const
    {
      return second->get_converted( first->get_converted(input) );
    }

    virtual type_info_wrapper input_type() const
    { return first->input_type(); }

    virtual type_info_wrapper output_type() const
    { return second->output_type(); }

    shared_ptr<BaseConversionFunction> first;
    shared_ptr<BaseConversionFunction> second;
  };





  class Converter
  {
  public:

    typedef std::map<type_info_wrapper, shared_ptr<BaseConversionFunction> > ConversionFunctionMapType;
    typedef std::map<type_info_wrapper, ConversionFunctionMapType> ConversionFunctionMapMapType;

    static type_info_wrapper get_type_id( ConstParameterHandle const & tmp );

    shared_ptr<BaseConversionFunction> convert_function( ConstParameterHandle const & input, ConstParameterHandle const & output );
    template<typename ValueT>
    shared_ptr<BaseConversionFunction> convert_function( ConstParameterHandle const & input );

    shared_ptr<BaseConversionFunction> best_convert_function( ConstParameterHandle const & input, std::map<string, string> const & properties );
    shared_ptr<BaseConversionFunction> best_convert_function( ConstParameterHandle const & input, string const & property_key, string const & property_value );


    bool is_convertable( ConstParameterHandle const & input, ConstParameterHandle const & output )
    { return convert_function(input, output); }
    template<typename ValueT>
    bool is_convertable( ConstParameterHandle const & input )
    { return convert_function<ValueT>(input); }

    bool convert( ConstParameterHandle const & input, ParameterHandle const & output )
    {
      shared_ptr<BaseConversionFunction> cf = convert_function(input, output);
      if (cf && cf->convert(input, output))
      {
        return true;
      }
      else
        return false;
    }
    template<typename ValueT>
    typename result_of::parameter_handle<ValueT>::type get_converted(ConstParameterHandle const & input)
    {
      shared_ptr<BaseConversionFunction> cf = convert_function<ValueT>(input);
      if (cf)
        return static_pointer_cast< ParameterWrapper<ValueT> >(cf->get_converted(input));
      else
        return typename result_of::parameter_handle<ValueT>::type();
    }


    void print_conversions(ConstParameterHandle const & input) const;


    template<typename InputValueT, typename OutputValueT>
    void register_conversion( bool (*fp)(InputValueT const &, OutputValueT &) )
    {
      typedef ParameterWrapper<InputValueT> InputParameterType;
      typedef ParameterWrapper<OutputValueT> OutputParameterType;

      type_info_wrapper input_type_id(typeid(InputParameterType));
      type_info_wrapper output_type_id(typeid(OutputParameterType));

      shared_ptr<BaseConversionFunction> current_conversion(new ConversionFunction<InputValueT, OutputValueT>(fp));
      simple_register_conversion( input_type_id, output_type_id, current_conversion );

      for (ConversionFunctionMapMapType::iterator imtit = conversions.begin();
           imtit != conversions.end();
           ++imtit)
      {
        ConversionFunctionMapType::iterator omtit = imtit->second.find(input_type_id);
        if (omtit != imtit->second.end())
        {
          simple_register_conversion(imtit->first, output_type_id, shared_ptr<BaseConversionFunction>(new DualConversionFunction(omtit->second, current_conversion)));
        }
      }

      ConversionFunctionMapMapType::iterator imtit = conversions.find(output_type_id);
      if (imtit != conversions.end())
      {
        for (ConversionFunctionMapType::iterator omtit = imtit->second.begin();
            omtit != imtit->second.end();
            ++omtit)
        {
          simple_register_conversion(input_type_id, omtit->first, shared_ptr<BaseConversionFunction>(new DualConversionFunction(current_conversion, omtit->second)));
        }
      }
    }

    static Converter & get()
    {
      static Converter converter_;
      return converter_;
    }

  private:

    void simple_register_conversion( type_info_wrapper const & input_type_id, type_info_wrapper const & output_type_id, shared_ptr<BaseConversionFunction> const & conversion )
    {
      shared_ptr<BaseConversionFunction> & entry = conversions[input_type_id][output_type_id];
      if (!entry)
        entry = conversion;
      else
      {
        if (entry->function_depth >= conversion->function_depth)
          entry = conversion;
      }
    }

    ConversionFunctionMapMapType conversions;
  };






  template<typename ParameterT>
  struct static_init_impl
  {
    // default implementation is empty! spezialise this class if you want something special
    static void init() {}
  };


  template<typename ParameterT>
  struct static_init
  {
    static void init()
    {
      static bool to_init = true;
      if (to_init)
      {
        to_init = false;
        info(10) << "static_init< " << typeid(ParameterT).name() << " >::init" << std::endl;
        static_init_impl<ParameterT>::init();
      }
    }

  };


  template<typename T>
  class ParameterWrapper;

  class BaseParameter : public enable_shared_from_this<BaseParameter>
  {
  public:

    virtual ~BaseParameter() {}

    virtual ParameterHandle unpack() = 0;
    virtual ConstParameterHandle unpack() const = 0;


    std::pair<string, bool> get_property( string const & key ) const
    { return TypeProperties::get().get_property( typeid(*this), key ); }

    bool match_property( string const & key, string const & value ) const
    { return TypeProperties::get().match_property( typeid(*this), key, value ); }

    bool match_properties( std::map<string, string> const & properties ) const
    { return TypeProperties::get().match_properties( typeid(*this), properties ); }


    template<typename ValueT>
    bool is_convertable_to() const
    { return Converter::get().is_convertable<ValueT>( shared_from_this() ); }

    template<typename ValueT>
    typename result_of::parameter_handle<ValueT>::type get_converted() const
    { return Converter::get().get_converted<ValueT>( shared_from_this() ); }

  protected:
  };


  bool is_convertable( ConstParameterHandle const & source, ParameterHandle & destination )
  { return Converter::get().is_convertable( source, destination ); }

  bool convert( ConstParameterHandle const & source, ParameterHandle & destination )
  {
    return Converter::get().convert( source, destination );
  }



  template<typename WrappedMeshConfig, typename WrappedSegmentationConfig>
  struct SegmentedMesh< viennagrid::mesh<WrappedMeshConfig>, viennagrid::segmentation<WrappedSegmentationConfig> >
  {
    typedef viennagrid::mesh<WrappedMeshConfig> mesh_type;
    typedef viennagrid::segmentation<WrappedSegmentationConfig> segmentation_type;

    SegmentedMesh() : segmentation(mesh) {}

    mesh_type mesh;
    segmentation_type segmentation;
  };





  template<typename InputValueT, typename OutputValueT>
  bool static_cast_convert( InputValueT const & input, OutputValueT & output )
  {
    output = static_cast<OutputValueT>(input);
    return true;
  }



  template<typename ParameterT>
  struct static_init;

  template<typename ParameterT>
  struct static_init< ParameterWrapper<ParameterT> >
  {
    static void init()
    {
      static_init<ParameterT>::init();
    }
  };



  template<typename ValueT>
  class ParameterWrapper : public BaseParameter
  {
  public:
    typedef ValueT value_type;

    ParameterWrapper() { static_init(); }
    ParameterWrapper(value_type const & value_) :
      value(value_) { static_init(); }

    ParameterHandle unpack()
    { return shared_from_this(); }

    ConstParameterHandle unpack() const
    { return shared_from_this(); }

    static void static_init()
    {
      static bool to_init = true;
      if (to_init)
      {
        viennamesh::static_init<value_type>::init();
        to_init = false;
      }
    }

    ValueT value;
  };



  type_info_wrapper Converter::get_type_id( ConstParameterHandle const & tmp )
  { return typeid(*tmp); }

  shared_ptr<BaseConversionFunction> Converter::convert_function( ConstParameterHandle const & input, ConstParameterHandle const & output_mesh )
  {
    ConversionFunctionMapMapType::iterator ipit = conversions.find(typeid(*input));
    if (ipit != conversions.end())
    {
      ConversionFunctionMapType::iterator opit = ipit->second.find(typeid(*output_mesh));
      if ( opit != ipit->second.end() )
      {
        return opit->second;
      }
    }

    return shared_ptr<BaseConversionFunction>();
  }

  template<typename ValueT>
  shared_ptr<BaseConversionFunction> Converter::convert_function( ConstParameterHandle const & input )
  {
    static_init<ValueT>::init();

    ConversionFunctionMapMapType::iterator ipit = conversions.find(typeid(*input));
    if (ipit != conversions.end())
    {
      ConversionFunctionMapType::iterator opit = ipit->second.find(typeid(ParameterWrapper<ValueT>));
      if ( opit != ipit->second.end() )
      {
        return opit->second;
      }
    }

    return shared_ptr<BaseConversionFunction>();
  }

  shared_ptr<BaseConversionFunction> Converter::best_convert_function( ConstParameterHandle const & input, std::map<string, string> const & properties )
  {
    ConversionFunctionMapMapType::iterator ipit = conversions.find(typeid(*input));
    if (ipit != conversions.end())
    {
      ConversionFunctionMapType::iterator best = ipit->second.end();
      for (ConversionFunctionMapType::iterator opit = ipit->second.begin(); opit != ipit->second.end(); ++opit)
      {
        if ( TypeProperties::get().match_properties(opit->second->output_type(), properties) )
        {
          if (best == ipit->second.end())
            best = opit;
          else
          {
            if (best->second->function_depth > opit->second->function_depth)
              best = opit;
          }
        }
      }

      if (best != ipit->second.end())
        return best->second;
    }

    return shared_ptr<BaseConversionFunction>();
  }

  shared_ptr<BaseConversionFunction> Converter::best_convert_function( ConstParameterHandle const & input, string const & property_key, string const & property_value )
  {
    ConversionFunctionMapMapType::iterator ipit = conversions.find(typeid(*input));
    if (ipit != conversions.end())
    {
      ConversionFunctionMapType::iterator best = ipit->second.end();
      for (ConversionFunctionMapType::iterator opit = ipit->second.begin(); opit != ipit->second.end(); ++opit)
      {
        if ( TypeProperties::get().match_property(opit->second->output_type(), property_key, property_value) )
        {
          if (best == ipit->second.end())
            best = opit;
          else
          {
            if (best->second->function_depth > opit->second->function_depth)
              best = opit;
          }
        }
      }

      if (best != ipit->second.end())
        return best->second;
    }

    return shared_ptr<BaseConversionFunction>();
  }



  void Converter::print_conversions(ConstParameterHandle const & input) const
  {
    LoggingStack ls("Supported conversion functions");
    info(10) << "Source type: [" << &typeid(*input) << "] " << std::endl;
    ConversionFunctionMapMapType::const_iterator ipit = conversions.find(typeid(*input));
    if (ipit != conversions.end())
    {
      for (ConversionFunctionMapType::const_iterator opit = ipit->second.begin(); opit != ipit->second.end(); ++opit)
      {
        info(10) << "Supports conversion to (" << opit->second->function_depth << "): [" << opit->first.get() << "] " << std::endl;
      }
    }
  }



  class ParameterSet;
  class ConstParameterSet;


  class ParameterLink : public BaseParameter
  {
  public:

    ParameterLink(ParameterSet & parameter_set_, string const & parameter_name_) : parameter_set(parameter_set_), parameter_name(parameter_name_)
    {}

    ParameterHandle unpack();
    ConstParameterHandle unpack() const;

  private:

    ParameterSet & parameter_set;
    string parameter_name;
  };

  class ConstParameterLink : public BaseParameter
  {
  public:

    ConstParameterLink(ConstParameterSet const & parameter_set_, string const & parameter_name_) : parameter_set(parameter_set_), parameter_name(parameter_name_) {}

    ParameterHandle unpack();
    ConstParameterHandle unpack() const;

  private:

    ConstParameterSet const & parameter_set;
    string parameter_name;
  };

  typedef shared_ptr<ParameterLink> ParameterLinkHandle;
  typedef shared_ptr<ConstParameterLink> ConstParameterLinkHandle;



  class ParameterSet
  {
  public:

    typedef std::map<string, ParameterHandle> ParameterMapType;

    void set( string const & name, ConstParameterHandle const & parameter ); // not supported
    void set( string const & name, ParameterHandle const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    void set( string const & name, ConstParameterLinkHandle const & parameter ); // not supported
    void set( string const & name, ParameterLinkHandle const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    template<typename ValueT>
    void set( string const & name, shared_ptr< const ParameterWrapper<ValueT> > const & parameter ); // not supported

    template<typename ValueT>
    void set( string const & name, shared_ptr< ParameterWrapper<ValueT> > const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    template<typename ValueT>
    void set( string const & name, ValueT const & value )
    {
      set( name, make_parameter(value) );
    }

    void unset( string const & name )
    {
      parameters.erase(name);
    }

    ConstParameterHandle get( string const & name ) const
    {
      ParameterMapType::const_iterator it = parameters.find(name);
      if (it == parameters.end())
        return ParameterHandle();
      return it->second->unpack();
    }

    ParameterHandle get( string const & name )
    {
      ParameterMapType::iterator it = parameters.find(name);
      if (it == parameters.end())
        return ParameterHandle();
      return it->second->unpack();
    }

    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get( string const & name ) const
    {
      ConstParameterHandle handle = get(name);
      if (!handle) return typename result_of::const_parameter_handle<ValueT>::type();
      typename result_of::const_parameter_handle<ValueT>::type result = dynamic_handle_cast<const ValueT>(handle);

      if (result)
        return result;

      return handle->template get_converted<ValueT>();
    }

    template<typename ValueT>
    typename result_of::parameter_handle<ValueT>::type get( string const & name )
    {
      ParameterHandle handle = get(name);
      if (!handle) return typename result_of::parameter_handle<ValueT>::type();
      typename result_of::parameter_handle<ValueT>::type result = dynamic_handle_cast<ValueT>(handle);

      if (result)
        return result;

      return handle->template get_converted<ValueT>();
    }

    ParameterHandle & get_create( string const & name )
    {
      return parameters[name];
    }

    template<typename ValueT>
    bool copy_if_present( string const & name, ValueT & target ) const
    {
      typename result_of::const_parameter_handle<ValueT>::type ptr = get<ValueT>(name);
      if (ptr)
      {
        target = ptr->value;
        return true;
      }
      return false;
    }

    void clear()
    { parameters.clear(); }

  private:
    ParameterMapType parameters;
  };





  class ConstParameterSet
  {
  public:

    typedef std::map<string, ConstParameterHandle> ParameterMapType;

    void set( string const & name, ConstParameterHandle const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    void set( string const & name, ParameterHandle const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    void set( string const & name, ConstParameterLinkHandle const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    void set( string const & name, ParameterLinkHandle const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    template<typename ValueT>
    void set( string const & name, shared_ptr< const ParameterWrapper<ValueT> > const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    template<typename ValueT>
    void set( string const & name, shared_ptr< ParameterWrapper<ValueT> > const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    template<typename ValueT>
    void set( string const & name, ValueT const & value )
    {
      set( name, make_parameter(value) );
    }

    void unset( string const & name )
    {
      parameters.erase(name);
    }

    ConstParameterHandle get( string const & name ) const
    {
      ParameterMapType::const_iterator it = parameters.find(name);
      if (it == parameters.end())
        return ConstParameterHandle();
      return it->second->unpack();
    }

    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get( string const & name ) const
    {
      ConstParameterHandle handle = get(name);
      if (!handle) return typename result_of::const_parameter_handle<ValueT>::type();
      typename result_of::const_parameter_handle<ValueT>::type result = dynamic_handle_cast<const ValueT>(handle);

      if (result)
        return result;

      return handle->template get_converted<ValueT>();
    }

    ConstParameterHandle & get_create( string const & name )
    {
      return parameters[name];
    }

    template<typename ValueT>
    bool copy_if_present( string const & name, ValueT & target ) const
    {
      typename result_of::const_parameter_handle<ValueT>::type ptr = get<ValueT>(name);
      if (ptr)
      {
        target = ptr->value;
        return true;
      }
      return false;
    }

    void clear()
    { parameters.clear(); }

  private:
    ParameterMapType parameters;
  };


  ParameterHandle ParameterLink::unpack()
  {
    return parameter_set.get(parameter_name);
  }

  ConstParameterHandle ParameterLink::unpack() const
  {
    return parameter_set.get(parameter_name);
  }


  ParameterHandle ConstParameterLink::unpack()
  {
    return ParameterHandle();
  }

  ConstParameterHandle ConstParameterLink::unpack() const
  {
    return parameter_set.get(parameter_name);
  }


}


#endif
