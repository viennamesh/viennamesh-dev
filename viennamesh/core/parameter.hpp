#ifndef VIENNAMESH_CORE_PARAMETER_HPP
#define VIENNAMESH_CORE_PARAMETER_HPP

#include <vector>

#include "viennamesh/forwards.hpp"
#include "viennamesh/utils/logger.hpp"


namespace viennamesh
{
  class base_parameter;

  template<typename T>
  class parameter_wrapper;




  template<typename ObjectT>
  class parameter_handle_t : public shared_ptr<ObjectT>
  {
  public:
    parameter_handle_t() {}

    template<typename AnotherObjectT>
    parameter_handle_t( parameter_handle_t<AnotherObjectT> const & rhs ) : shared_ptr<ObjectT>(rhs) {}

    parameter_handle_t( shared_ptr<ObjectT> const & ptr ) : shared_ptr<ObjectT>(ptr) {}

    explicit parameter_handle_t( ObjectT * ptr ) : shared_ptr<ObjectT>(ptr) {}

  };

  template<typename ValueT>
  class parameter_handle_t< parameter_wrapper<ValueT> > : public shared_ptr< parameter_wrapper<ValueT> >
  {
    typedef parameter_wrapper<ValueT> ObjectType;
  public:
    parameter_handle_t() {}
    parameter_handle_t( shared_ptr<ObjectType> const & ptr ) : shared_ptr<ObjectType>(ptr) {}

    explicit parameter_handle_t( ObjectType * ptr ) : shared_ptr<ObjectType>(ptr) {}

    ValueT & operator()() { return (*this)->value(); }
    ValueT const & operator()() const { return (*this)->value(); }
  };

  template<typename ValueT>
  class parameter_handle_t< const parameter_wrapper<ValueT> > : public shared_ptr<const parameter_wrapper<ValueT> >
  {
    typedef parameter_wrapper<ValueT> NonConstObjectType;
    typedef const parameter_wrapper<ValueT> ObjectType;
  public:
    parameter_handle_t() {}

    parameter_handle_t( shared_ptr<ObjectType> const & ptr ) : shared_ptr<ObjectType>(ptr) {}
    parameter_handle_t( shared_ptr<NonConstObjectType> const & ptr ) : shared_ptr<ObjectType>(ptr) {}

    explicit parameter_handle_t( ObjectType * ptr ) : shared_ptr<ObjectType>(ptr) {}

    ValueT const & operator()() { return (*this)->value(); }
    ValueT const & operator()() const { return (*this)->value(); }
  };



  typedef parameter_handle_t<base_parameter> parameter_handle;
  typedef parameter_handle_t<const base_parameter> const_parameter_handle;

  namespace result_of
  {
    template<typename ValueT>
    struct parameter_handle
    {
      typedef parameter_handle_t< parameter_wrapper<ValueT> > type;
    };

    template<typename ValueT>
    struct const_parameter_handle
    {
      typedef parameter_handle_t< const parameter_wrapper<ValueT> > type;
    };


    template<typename ValueT>
    struct parameter_handle<const ValueT>
    {
      typedef typename const_parameter_handle<ValueT>::type type;
    };
  }

  typedef result_of::parameter_handle<bool>::type bool_parameter_handle;
  typedef result_of::const_parameter_handle<bool>::type const_bool_parameter_handle;

  typedef result_of::parameter_handle<int>::type int_parameter_handle;
  typedef result_of::const_parameter_handle<int>::type const_int_parameter_handle;

  typedef result_of::parameter_handle<double>::type double_parameter_handle;
  typedef result_of::const_parameter_handle<double>::type const_double_parameter_handle;

  typedef result_of::parameter_handle<string>::type string_parameter_handle;
  typedef result_of::const_parameter_handle<string>::type const_string_parameter_handle;


  namespace detail
  {
    template<typename ValueT, typename SourceT>
    struct dynamic_handle_cast_impl
    {
      static typename result_of::parameter_handle<ValueT>::type cast( shared_ptr<SourceT> const & ptr )
      {
        return dynamic_pointer_cast< parameter_wrapper<ValueT> >(ptr);
      }
    };

    template<typename ValueT, typename SourceT>
    struct dynamic_handle_cast_impl<const ValueT, SourceT>
    {
      static typename result_of::const_parameter_handle<ValueT>::type cast( shared_ptr<SourceT> const & ptr )
      {
        return dynamic_pointer_cast< const parameter_wrapper<ValueT> >(ptr);
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
    return typename result_of::parameter_handle<ValueT>::type( new parameter_wrapper<ValueT>() );
  }

  template<typename ValueT>
  typename result_of::parameter_handle<ValueT>::type make_parameter( ValueT const & value )
  {
    return typename result_of::parameter_handle<ValueT>::type( new parameter_wrapper<ValueT>(value) );
  }

  template<typename ValueT>
  typename result_of::parameter_handle<ValueT>::type make_reference_parameter( ValueT & value )
  {
    return typename result_of::parameter_handle<ValueT>::type( new parameter_wrapper<ValueT>(&value) );
  }



  class type_properties
  {
  public:

    typedef std::map<string, string> PropertyMapType;


    PropertyMapType & get_map(type_info_wrapper const & ti)
    {
      return type_properties_map[ti];
    }

    std::pair<string, bool> get_property( type_info_wrapper const & ti, string const & name ) const
    {
      std::map<type_info_wrapper, PropertyMapType>::const_iterator tpit = type_properties_map.find(ti);
      if (tpit != type_properties_map.end())
      {
        PropertyMapType::const_iterator pmit = tpit->second.find(name);
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
      set_property( typeid( parameter_wrapper<ValueT> ), key, value );
    }

    bool match_properties( type_info_wrapper const & ti, PropertyMapType const & to_match ) const
    {
      if (to_match.empty())
        return true;

      std::map<type_info_wrapper, PropertyMapType>::const_iterator pmit = type_properties_map.find(ti);

      if (pmit == type_properties_map.end())
        return false;

      PropertyMapType const & properties = pmit->second;

      for (PropertyMapType::const_iterator it = to_match.begin(); it != to_match.end(); ++it)
      {
        PropertyMapType::const_iterator pit = properties.find( it->first );
        if ( pit == properties.end() )
          return false;

        if ( it->second != pit->second )
          return false;
      }

      return true;
    }

    bool match_property( type_info_wrapper const & ti, string const & key, string const & value ) const
    {
      std::map<type_info_wrapper, PropertyMapType>::const_iterator pmit = type_properties_map.find(ti);

      if (pmit == type_properties_map.end())
        return false;

      PropertyMapType const & properties = pmit->second;

      PropertyMapType::const_iterator pit = properties.find(key);
      if ( pit == properties.end() )
        return false;

      return pit->second == value;
    }


    void print() const
    {
      for (std::map<type_info_wrapper, PropertyMapType>::const_iterator tit = type_properties_map.begin(); tit != type_properties_map.end(); ++tit)
      {
        std::cout << "Type: " << tit->first.name() << std::endl;
        for (PropertyMapType::const_iterator pit = tit->second.begin(); pit != tit->second.end(); ++pit)
        {
          std::cout << "  " << pit->first << " - " << pit->second << std::endl;
        }
      }
    }


    static type_properties & get()
    {
      static type_properties type_properties_;
      return type_properties_;
    }

  private:
    std::map<type_info_wrapper, PropertyMapType> type_properties_map;
  };









  class base_conversion_function
  {
  public:
    base_conversion_function(int function_depth_) : function_depth(function_depth_) {}
    virtual ~base_conversion_function() {}

    virtual bool convert( const_parameter_handle const &, parameter_handle const & ) const = 0;
    virtual parameter_handle get_converted( const_parameter_handle const & ) const = 0;

    virtual type_info_wrapper input_type() const = 0;
    virtual type_info_wrapper output_type() const = 0;

    int function_depth;
  };

  template<typename InputValueT, typename OutputValueT>
  class conversion_function : public base_conversion_function
  {
  public:
    typedef parameter_wrapper<InputValueT> InputParameterType;
    typedef parameter_wrapper<OutputValueT> OutputParameterType;

    conversion_function() : base_conversion_function(1) {}
    conversion_function( function<bool (InputValueT const &, OutputValueT &)> const & f) :
        base_conversion_function(1), convert_function(f) {}

    virtual bool convert( const_parameter_handle const & input, parameter_handle const & output ) const
    {
#ifdef DEBUG
      return convert_function( dynamic_cast<InputParameterType const &>(*input).value(), dynamic_cast<OutputParameterType &>(*output).value() );
#else
      return convert_function( static_cast<InputParameterType const &>(*input).value(), static_cast<OutputParameterType &>(*output).value() );
#endif
    }

    virtual parameter_handle get_converted( const_parameter_handle const & input ) const
    {
      parameter_handle_t<OutputParameterType> result( new OutputParameterType() );
      if (convert(input, result))
        return result;
      return parameter_handle_t<OutputParameterType>();
    }

    virtual type_info_wrapper input_type() const
    { return type_info_wrapper::make<InputParameterType>(); }

    virtual type_info_wrapper output_type() const
    { return type_info_wrapper::make<OutputParameterType>(); }


    function<bool (InputValueT const &, OutputValueT &)> convert_function;
  };

  class dual_conversion_function : public base_conversion_function
  {
  public:
    dual_conversion_function( parameter_handle_t<base_conversion_function> const & first_, parameter_handle_t<base_conversion_function> const & second_ ) :
        base_conversion_function(first_->function_depth + second_->function_depth), first(first_), second(second_) {}

    virtual bool convert( const_parameter_handle const & input, parameter_handle const & output ) const
    {
      return second->convert( first->get_converted(input), output );
    }

    virtual parameter_handle get_converted( const_parameter_handle const & input ) const
    {
      return second->get_converted( first->get_converted(input) );
    }

    virtual type_info_wrapper input_type() const
    { return first->input_type(); }

    virtual type_info_wrapper output_type() const
    { return second->output_type(); }

    shared_ptr<base_conversion_function> first;
    shared_ptr<base_conversion_function> second;
  };





  class converter
  {
  public:

    typedef std::map<type_info_wrapper, shared_ptr<base_conversion_function> > ConversionFunctionMapType;
    typedef std::map<type_info_wrapper, ConversionFunctionMapType> ConversionFunctionMapMapType;

    static type_info_wrapper get_type_id( const_parameter_handle const & tmp );

    shared_ptr<base_conversion_function> convert_function( const_parameter_handle const & input, const_parameter_handle const & output );
    template<typename ValueT>
    shared_ptr<base_conversion_function> convert_function( const_parameter_handle const & input );

    shared_ptr<base_conversion_function> best_convert_function( const_parameter_handle const & input, std::map<string, string> const & properties );
    shared_ptr<base_conversion_function> best_convert_function( const_parameter_handle const & input, string const & property_key, string const & property_value );


    bool is_convertable( const_parameter_handle const & input, const_parameter_handle const & output )
    { return (get_type_id(input) == get_type_id(output)) || convert_function(input, output); }
    template<typename ValueT>
    bool is_convertable( const_parameter_handle const & input )
    { return (get_type_id(input) == type_info_wrapper::make< parameter_wrapper<ValueT> >()) ||  convert_function<ValueT>(input); }

    bool convert( const_parameter_handle const & input, parameter_handle const & output )
    {
      shared_ptr<base_conversion_function> cf = convert_function(input, output);
      if (cf && cf->convert(input, output))
      {
        return true;
      }
      else
        return false;
    }
    template<typename ValueT>
    typename result_of::parameter_handle<ValueT>::type get_converted(const_parameter_handle const & input)
    {
      shared_ptr<base_conversion_function> cf = convert_function<ValueT>(input);
      if (cf)
        return static_pointer_cast< parameter_wrapper<ValueT> >(cf->get_converted(input));
      else
        return typename result_of::parameter_handle<ValueT>::type();
    }


    void print_conversions(const_parameter_handle const & input) const;


    template<typename InputValueT, typename OutputValueT>
    void register_conversion( bool (*fp)(InputValueT const &, OutputValueT &) )
    {
      typedef parameter_wrapper<InputValueT> InputParameterType;
      typedef parameter_wrapper<OutputValueT> OutputParameterType;

      type_info_wrapper input_type_id(typeid(InputParameterType));
      type_info_wrapper output_type_id(typeid(OutputParameterType));

      shared_ptr<base_conversion_function> current_conversion(new conversion_function<InputValueT, OutputValueT>(fp));
      simple_register_conversion( input_type_id, output_type_id, current_conversion );

      for (ConversionFunctionMapMapType::iterator imtit = conversions.begin();
           imtit != conversions.end();
           ++imtit)
      {
        ConversionFunctionMapType::iterator omtit = imtit->second.find(input_type_id);
        if (omtit != imtit->second.end())
        {
          simple_register_conversion(imtit->first, output_type_id, shared_ptr<base_conversion_function>(new dual_conversion_function(omtit->second, current_conversion)));
        }
      }

      ConversionFunctionMapMapType::iterator imtit = conversions.find(output_type_id);
      if (imtit != conversions.end())
      {
        for (ConversionFunctionMapType::iterator omtit = imtit->second.begin();
            omtit != imtit->second.end();
            ++omtit)
        {
          simple_register_conversion(input_type_id, omtit->first, shared_ptr<base_conversion_function>(new dual_conversion_function(current_conversion, omtit->second)));
        }
      }
    }

    static converter & get()
    {
      static converter converter_;
      return converter_;
    }

  private:

    void simple_register_conversion( type_info_wrapper const & input_type_id, type_info_wrapper const & output_type_id, shared_ptr<base_conversion_function> const & conversion )
    {
      shared_ptr<base_conversion_function> & entry = conversions[input_type_id][output_type_id];
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
  class parameter_wrapper;

  class base_parameter : public enable_shared_from_this<base_parameter>
  {
  public:

    virtual ~base_parameter() {}

    virtual parameter_handle unpack() = 0;
    virtual const_parameter_handle unpack() const = 0;
    virtual bool is_link() const = 0;
    virtual bool is_reference() const = 0;


    std::pair<string, bool> get_property( string const & key ) const
    { return type_properties::get().get_property( typeid(*this), key ); }

    bool match_property( string const & key, string const & value ) const
    { return type_properties::get().match_property( typeid(*this), key, value ); }

    bool match_properties( std::map<string, string> const & properties ) const
    { return type_properties::get().match_properties( typeid(*this), properties ); }


    template<typename ValueT>
    bool is_convertable_to() const
    { return converter::get().is_convertable<ValueT>( shared_from_this() ); }

    template<typename ValueT>
    typename result_of::parameter_handle<ValueT>::type get_converted() const
    { return converter::get().get_converted<ValueT>( shared_from_this() ); }

  protected:
  };



  inline bool is_convertable( const_parameter_handle const & source, parameter_handle & destination )
  { return converter::get().is_convertable( source, destination ); }

  inline bool convert( const_parameter_handle const & source, parameter_handle & destination )
  { return converter::get().convert( source, destination ); }









  template<typename InputValueT, typename OutputValueT>
  bool static_cast_convert( InputValueT const & input, OutputValueT & output )
  {
    output = static_cast<OutputValueT>(input);
    return true;
  }



  template<typename ParameterT>
  struct static_init;

  template<typename ParameterT>
  struct static_init< parameter_wrapper<ParameterT> >
  {
    static void init()
    {
      static_init<ParameterT>::init();
    }
  };



  template<typename ValueT>
  class parameter_wrapper : public base_parameter
  {
  public:
    typedef ValueT value_type;

    parameter_wrapper() : value_ptr_(NULL) { static_init(); }
    parameter_wrapper(value_type const & val) :
      value_(val), value_ptr_(NULL) { static_init(); }
    parameter_wrapper(value_type * val_ptr) :
      value_ptr_(val_ptr) { static_init(); }

    parameter_handle unpack() { return shared_from_this(); }
    const_parameter_handle unpack() const { return shared_from_this(); }
    bool is_link() const { return false; }
    bool is_reference() const { return value_ptr_; }

    static void static_init()
    {
      static bool to_init = true;
      if (to_init)
      {
        viennamesh::static_init<value_type>::init();
        to_init = false;
      }
    }

    void set_ptr( ValueT * val_ptr ) { value_ptr_ = val_ptr; }
    void set( ValueT const & val ) { value_ptr_ = NULL; value_ = val; }

  private:

    template<typename TypeT>
    friend class parameter_handle_t;

    template<typename InputValueT, typename OutputValueT>
    friend class conversion_function;

    ValueT & value() { if (value_ptr_) return *value_ptr_; else return value_; }
    ValueT const & value() const { if (value_ptr_) return *value_ptr_; else return value_; }

    ValueT value_;
    ValueT * value_ptr_;
  };




  inline type_info_wrapper converter::get_type_id( const_parameter_handle const & tmp )
  { return typeid(*tmp); }

  inline shared_ptr<base_conversion_function> converter::convert_function( const_parameter_handle const & input, const_parameter_handle const & output_mesh )
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

    return shared_ptr<base_conversion_function>();
  }

  template<typename ValueT>
  shared_ptr<base_conversion_function> converter::convert_function( const_parameter_handle const & input )
  {
    static_init<ValueT>::init();

    ConversionFunctionMapMapType::iterator ipit = conversions.find(typeid(*input));
    if (ipit != conversions.end())
    {
      ConversionFunctionMapType::iterator opit = ipit->second.find(typeid(parameter_wrapper<ValueT>));
      if ( opit != ipit->second.end() )
      {
        return opit->second;
      }
    }

    return shared_ptr<base_conversion_function>();
  }

  inline shared_ptr<base_conversion_function> converter::best_convert_function( const_parameter_handle const & input, std::map<string, string> const & properties )
  {
    ConversionFunctionMapMapType::iterator ipit = conversions.find(typeid(*input));
    if (ipit != conversions.end())
    {
      ConversionFunctionMapType::iterator best = ipit->second.end();
      for (ConversionFunctionMapType::iterator opit = ipit->second.begin(); opit != ipit->second.end(); ++opit)
      {
        if ( type_properties::get().match_properties(opit->second->output_type(), properties) )
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

    return shared_ptr<base_conversion_function>();
  }

  inline shared_ptr<base_conversion_function> converter::best_convert_function( const_parameter_handle const & input, string const & property_key, string const & property_value )
  {
    ConversionFunctionMapMapType::iterator ipit = conversions.find(typeid(*input));
    if (ipit != conversions.end())
    {
      ConversionFunctionMapType::iterator best = ipit->second.end();
      for (ConversionFunctionMapType::iterator opit = ipit->second.begin(); opit != ipit->second.end(); ++opit)
      {
        if ( type_properties::get().match_property(opit->second->output_type(), property_key, property_value) )
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

    return shared_ptr<base_conversion_function>();
  }



  inline void converter::print_conversions(const_parameter_handle const & input) const
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



  class parameter_set;
  class const_parameter_set;


  class parameter_link : public base_parameter
  {
  public:

    parameter_link(parameter_set & para_set, string const & para_name) : parameter_set_(para_set), parameter_name_(para_name)
    {}

    parameter_handle unpack();
    const_parameter_handle unpack() const;

    bool is_link() const { return true; }
    bool is_reference() const { return false; }

  private:

    parameter_set & parameter_set_;
    string          parameter_name_;
  };

  class const_parameter_link : public base_parameter
  {
  public:

    const_parameter_link(const_parameter_set const & para_set, string const & para_name) : parameter_set_(para_set), parameter_name_(para_name) {}

    parameter_handle unpack();
    const_parameter_handle unpack() const;
    bool is_link() const { return true; }
    bool is_reference() const { return false; }

  private:

    const_parameter_set const & parameter_set_;
    string parameter_name_;
  };

//   typedef shared_ptr<parameter_link> parameter_link_handle;
//   typedef shared_ptr<const_parameter_link> const_parameter_link_handle;

//   typedef result_of::parameter_handle<parameter_link>::type parameter_link_handle;
//   typedef result_of::const_parameter_handle<parameter_link>::type const_parameter_link_handle;

  typedef parameter_handle_t<parameter_link> parameter_link_handle;
  typedef parameter_handle_t<const_parameter_link> const_parameter_link_handle;


  class parameter_set
  {
  public:

    typedef std::map<string, parameter_handle> ParameterMapType;

    void set( string const & name, const_parameter_handle const & parameter ); // not supported
    void set( string const & name, parameter_handle const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    void set( string const & name, const_parameter_link_handle const & parameter ); // not supported
    void set( string const & name, parameter_link_handle const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    template<typename ValueT>
    void set( string const & name, parameter_handle_t< const parameter_wrapper<ValueT> > const & parameter ); // not supported

    template<typename ValueT>
    void set( string const & name, parameter_handle_t< parameter_wrapper<ValueT> > const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    template<typename ValueT>
    void set( string const & name, ValueT const & value )
    {
      set( name, make_parameter(value) );
    }

    void set( string const & name, char const * value )
    {
      set( name, make_parameter( string(value) ) );
    }




    void unset( string const & name )
    {
      parameters.erase(name);
    }

    const_parameter_handle get( string const & name ) const
    {
      ParameterMapType::const_iterator it = parameters.find(name);
      if (it == parameters.end())
        return parameter_handle();
      return it->second->unpack();
    }

    parameter_handle get( string const & name )
    {
      ParameterMapType::iterator it = parameters.find(name);
      if (it == parameters.end())
        return parameter_handle();
      return it->second->unpack();
    }

    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get( string const & name ) const
    {
      const_parameter_handle handle = get(name);
      if (!handle) return typename result_of::const_parameter_handle<ValueT>::type();
      typename result_of::const_parameter_handle<ValueT>::type result = dynamic_handle_cast<const ValueT>(handle);

      if (result)
        return result;

      return handle->template get_converted<ValueT>();
    }

    template<typename ValueT>
    typename result_of::parameter_handle<ValueT>::type get( string const & name )
    {
      parameter_handle handle = get(name);
      if (!handle) return typename result_of::parameter_handle<ValueT>::type();
      typename result_of::parameter_handle<ValueT>::type result = dynamic_handle_cast<ValueT>(handle);

      if (result)
        return result;

      return handle->template get_converted<ValueT>();
    }

    parameter_handle & get_create( string const & name )
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

    void clear_non_references()
    {
      ParameterMapType::iterator it = parameters.begin();
      while (it != parameters.end())
      {
        if (!it->second->is_reference())
          parameters.erase(it++);
        else
          ++it;
      }
    }

  private:
    ParameterMapType parameters;
  };


  class const_parameter_set
  {
  public:

    typedef std::map<string, const_parameter_handle> ParameterMapType;

    void set( string const & name, const_parameter_handle const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    void set( string const & name, parameter_handle const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    void set( string const & name, const_parameter_link_handle const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    void set( string const & name, parameter_link_handle const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    template<typename ValueT>
    void set( string const & name, parameter_handle_t< const parameter_wrapper<ValueT> > const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    template<typename ValueT>
    void set( string const & name, parameter_handle_t< parameter_wrapper<ValueT> > const & parameter )
    {
      if (parameter)
        parameters[name] = parameter;
    }

    template<typename ValueT>
    void set( string const & name, ValueT const & value )
    {
      set( name, make_parameter(value) );
    }

    void set( string const & name, char const * value )
    {
      set( name, make_parameter( string(value) ) );
    }

    void unset( string const & name )
    {
      parameters.erase(name);
    }

    const_parameter_handle get( string const & name ) const
    {
      ParameterMapType::const_iterator it = parameters.find(name);
      if (it == parameters.end())
        return const_parameter_handle();
      return it->second->unpack();
    }

    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get( string const & name ) const
    {
      const_parameter_handle handle = get(name);
      if (!handle) return typename result_of::const_parameter_handle<ValueT>::type();
      typename result_of::const_parameter_handle<ValueT>::type result = dynamic_handle_cast<const ValueT>(handle);

      if (result)
        return result;

      return handle->template get_converted<ValueT>();
    }

    const_parameter_handle & get_create( string const & name )
    {
      return parameters[name];
    }

    template<typename ValueT>
    bool copy_if_present( string const & name, ValueT & target ) const
    {
      typename result_of::const_parameter_handle<ValueT>::type handle = get<ValueT>(name);
      if (handle)
      {
        target = handle();
        return true;
      }
      return false;
    }

    void clear()
    { parameters.clear(); }

  private:
    ParameterMapType parameters;
  };


  inline parameter_handle parameter_link::unpack()
  { return parameter_set_.get(parameter_name_); }

  inline const_parameter_handle parameter_link::unpack() const
  { return parameter_set_.get(parameter_name_); }


  inline parameter_handle const_parameter_link::unpack()
  { return parameter_handle(); }

  inline const_parameter_handle const_parameter_link::unpack() const
  { return parameter_set_.get(parameter_name_); }


}


#endif
