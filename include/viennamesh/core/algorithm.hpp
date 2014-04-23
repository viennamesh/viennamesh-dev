#ifndef VIENNAMESH_CORE_ALGORITHM_HPP
#define VIENNAMESH_CORE_ALGORITHM_HPP

#include "viennamesh/core/parameter.hpp"
#include "viennamesh/core/basic_parameters.hpp"

#include "viennamesh/utils/logger.hpp"
#include "viennamesh/core/exceptions.hpp"


namespace viennamesh
{
  class base_algorithm;

#ifdef VIENNAMESH_USES_BOOST
  typedef boost::shared_ptr<base_algorithm> algorithm_handle;
  typedef boost::shared_ptr<const base_algorithm> const_algorithm_handle;
#endif

#ifdef VIENNAMESH_USES_CPP11
  typedef std::shared_ptr<base_algorithm> algorithm_handle;
  typedef std::shared_ptr<const base_algorithm> const_algorithm_handle;
#endif


  class base_parameter_interface;
  class input_parameter_interface;
  class output_parameter_interface;
  class parameter_link;

  parameter_handle_t<parameter_link> make_parameter_link(algorithm_handle const & algorithm, string const & para_name);



  class base_algorithm : public enable_shared_from_this<base_algorithm>
  {
    template<typename ValueT>
    friend class output_parameter_proxy;

    friend class base_parameter_interface;
    friend class input_parameter_interface;
    friend class output_parameter_interface;


  public:
    virtual ~base_algorithm() {}

    algorithm_handle handle() { return shared_from_this(); }
    const_algorithm_handle chandle() const { return shared_from_this(); }
    const_algorithm_handle handle() const { return chandle(); }


    void set_default_source( algorithm_handle const & default_source_ )
    { default_source = default_source_; }

    void unset_default_source()
    { default_source = algorithm_handle(); }


  private:

    void set_input_impl( std::string const & name, const_parameter_handle const & parameter )
    {
      if (!is_input_registered(name))
        warning(1) << "Input parameter \"" << name << "\" is set but was not registered" << std::endl;
      inputs.set(name, parameter);
    }

    void set_input_impl( std::string const & name, parameter_handle const & parameter )
    {
      if (!is_input_registered(name))
        warning(1) << "Input parameter \"" << name << "\" is set but was not registered" << std::endl;
      inputs.set(name, parameter);
    }


    template<typename TypeT>
    void generic_set_input( std::string const & name, TypeT const & value )
    { set_input_impl( name, static_cast<parameter_handle const &>(make_parameter(value)) ); }

    template<typename ValueT>
    void generic_set_input( std::string const & name, parameter_handle_t< parameter_wrapper<ValueT> > const & parameter )
    { set_input_impl(name, static_cast<parameter_handle const &>(parameter)); }

  public:


    // sets an input parameter
    void set_input( std::string const & name, const_parameter_handle const & parameter )
    { set_input_impl(name, parameter); }

    void set_input( std::string const & name, parameter_handle const & parameter )
    { set_input_impl(name, parameter); }

    template<typename TypeT>
    void set_input( std::string const & name, TypeT const & value )
    { generic_set_input(name, value); }

    template<typename ValueT>
    void set_input( std::string const & name, parameter_handle_t< const parameter_wrapper<ValueT> > const & parameter )
    { generic_set_input(name, parameter); }

    template<typename ValueT>
    void set_input( std::string const & name, parameter_handle_t< parameter_wrapper<ValueT> > const & parameter )
    { set_input_impl(name, static_cast<parameter_handle const &>(parameter)); }

    void set_input( std::string const & name, parameter_handle_t<parameter_link> const & parameter )
    { set_input_impl(name, static_cast<parameter_handle const &>(parameter)); }

    void set_input( std::string const & name, char const * value )
    { set_input( name, make_parameter( string(value) ) ); }

    void set_input( std::string const & name, char * value )
    { set_input( name, make_parameter( string(value) ) ); }

    void set_input( std::string const & name, bool value )
    { generic_set_input(name, value); }

    void set_input( std::string const & name, int value )
    { generic_set_input(name, value); }

    void set_input( std::string const & name, double value )
    { generic_set_input(name, value); }


    void link_input( std::string const & name, algorithm_handle const & source_algorithm, string const & source_name )
    { set_input( name, make_parameter_link(source_algorithm, source_name) ); }


    // unsets an input parameter
    void unset_input( std::string const & name )
    {
      if (!is_input_registered(name))
        warning(1) << "Input parameter \"" << name << "\" is unset but was not registered" << std::endl;
      inputs.unset(name);
    }

  private:
    // queries an input parameter
    const_parameter_handle get_input( std::string const & name ) const
    {
      if (!is_input_registered(name))
        warning(1) << "Input parameter \"" << name << "\" is accessed but was not registered" << std::endl;

      const_parameter_handle parameter = inputs.get(name);

      if (parameter)
        return parameter;

      if (default_source)
        return default_source->get_output(name);

      return const_parameter_handle();
    }

    // queries an input parameter of special type
    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get_input( std::string const & name ) const
    {
      const_parameter_handle parameter = get_input(name);
      if (!parameter) return typename result_of::const_parameter_handle<ValueT>::type();

      typename result_of::const_parameter_handle<ValueT>::type result = dynamic_handle_cast<const ValueT>(parameter);

      if (result)
        return result;

      return parameter->template get_converted<ValueT>();
    }

    // queries an input parameter, throws input_parameter_not_found_exception if not found
    const_parameter_handle get_required_input( std::string const & name ) const
    {
      const_parameter_handle param = get_input(name);
      if (!param)
        throw input_parameter_not_found_exception( "Input parameter '" + name + "' is missing or of non-convertable type" );
      return param;
    }

    // queries an input parameter of special type, throws input_parameter_not_found_exception if not found
    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get_required_input( std::string const & name ) const
    {
      typename result_of::const_parameter_handle<ValueT>::type parameter = get_input<ValueT>(name);
      if (!parameter)
        throw input_parameter_not_found_exception( "Input parameter '" + name + "' is missing or of non-convertable type" );
      return parameter;
    }


    // copies input parameter of special type to value, only works if input is present and convertable, returns true if copy was performed
    template<typename ValueT>
    bool copy_input( std::string const & name, ValueT & value ) const
    { return inputs.copy_if_present(name, value); }

  public:

    // references an output parameter to a specific value, doesn't take ownership, remembers pointer to value
    template<typename TypeT>
    void set_output( std::string const & name, TypeT & value )
    { outputs.set( name, make_reference_parameter(value) ); }

    void set_output( std::string const & name, parameter_handle const & handle )
    { outputs.set( name, parameter_handle( new parameter_handle_reference(handle) ) ); }

    void set_output( std::string const & name, parameter_handle & handle )
    { outputs.set( name, parameter_handle( new parameter_handle_reference(handle) ) ); }

  public:

    // queries an output parameter
    const_parameter_handle get_output( std::string const & name ) const
    { return outputs.get(name); }

    parameter_handle get_output( std::string const & name )
    { return outputs.get(name); }

    // queries an output parameter of special type
    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get_output( std::string const & name ) const
    { return outputs.get<ValueT>(name); }

    template<typename ValueT>
    typename result_of::parameter_handle<ValueT>::type get_output( std::string const & name )
    { return outputs.get<ValueT>(name); }

    // unsets an output parameter
    void unset_output( string const & name ) { outputs.unset(name); } // TODO needed?

//     // clears all output parameters
//     void clear_outputs() { outputs.clear(); } // TODO needed?

    // runs the algorithm
    bool run();

    // returns the algorithm name
    virtual string name() const = 0;
    virtual string id() const = 0;
//     virtual string type() const = 0;
//     virtual string category() const = 0;
//     virtual string description() const = 0;

    std::string const & base_path() const { return base_path_; }
    std::string & base_path() { return base_path_; }

  protected:

    virtual bool run_impl() = 0;

  private:

    typedef std::map<std::string, base_parameter_interface *> RegisteredParameterMapType;
    RegisteredParameterMapType input_parameters;
    RegisteredParameterMapType output_parameters;

    bool is_input_registered(std::string const & name) const;
    bool is_output_registered(std::string const & name) const;

    void register_input_parameter( base_parameter_interface & input_parameter );
    void register_output_parameter( base_parameter_interface
    & output_parameter );

  private:

    algorithm_handle default_source;

    const_parameter_set inputs;
    parameter_set outputs;

    std::string base_path_;
  };


  class parameter_link : public base_parameter
  {
  public:

    parameter_link(algorithm_handle const & algorithm, string const & para_name) : algorithm_(algorithm), parameter_name_(para_name) {}

    parameter_handle unpack() { return algorithm_->get_output(parameter_name_); }
    const_parameter_handle unpack() const { return algorithm_->get_output(parameter_name_); }
    bool is_reference() const { return false; }
    string type_name() const { return "parameter_link"; }

  private:

    algorithm_handle algorithm_;
    string parameter_name_;
  };










  class base_parameter_interface
  {
    friend class base_algorithm;

  public:
    base_parameter_interface(base_algorithm & algorithm, std::string const & parameter_name) :
      algorithm_(algorithm), name_(parameter_name) {}

    virtual ~base_parameter_interface() {}

    std::string const & name() const { return name_; }
    base_algorithm & algorithm() { return algorithm_; }
    base_algorithm const & algorithm() const { return algorithm_; }

  private:

    virtual void reset() {}

    base_algorithm & algorithm_;
    std::string name_;
  };


  class output_parameter_interface : public base_parameter_interface
  {
  public:
    output_parameter_interface(base_algorithm & algorithm, std::string const & parameter_name) :
      base_parameter_interface(algorithm, parameter_name)
      {
        algorithm.register_output_parameter(*this);
      }
  };






  class input_parameter_interface : public base_parameter_interface
  {
  public:
    input_parameter_interface(base_algorithm & algorithm, std::string const & parameter_name) :
      base_parameter_interface(algorithm, parameter_name)
      {
        algorithm.register_input_parameter(*this);
      }

  protected:
    const_parameter_handle get() const
    { return algorithm().get_input(name()); }

    const_parameter_handle get_required() const
    { return algorithm().get_required_input(name()); }


    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get() const
    { return algorithm().get_input<ValueT>(name()); }

    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get_required() const
    { return algorithm().get_required_input<ValueT>(name()); }
  };




  class dynamic_required_input_parameter_interface : public input_parameter_interface
  {
  public:

    dynamic_required_input_parameter_interface(base_algorithm & algorithm, std::string const & parameter_name) :
      input_parameter_interface(algorithm, parameter_name) {}

    const_parameter_handle get() const
    { return input_parameter_interface::get_required(); }

    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get() const
    {
      get();
      return input_parameter_interface::get<ValueT>();
    }

  private:
  };



  template<typename ValueT>
  class required_input_parameter_interface : public input_parameter_interface
  {
  public:

    required_input_parameter_interface(base_algorithm & algorithm, std::string const & parameter_name) :
      input_parameter_interface(algorithm, parameter_name), fetched(false) {}

    ValueT const & operator()() const
    {
      fetch();
      return native_handle();
    }

  private:

    void reset()
    {
      fetched = false;
    }

    void fetch() const
    {
      if (!fetched)
      {
        native_handle = input_parameter_interface::get_required<ValueT>();
        fetched = true;
      }
    }

    mutable bool fetched;
    mutable typename result_of::const_parameter_handle<ValueT>::type native_handle;
  };




  class dynamic_optional_input_parameter_interface : public input_parameter_interface
  {
  public:

    dynamic_optional_input_parameter_interface(base_algorithm & algorithm, std::string const & parameter_name) :
      input_parameter_interface(algorithm, parameter_name) {}

    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get() const
    { return input_parameter_interface::get<ValueT>(); }

    bool valid() const
    { return input_parameter_interface::get(); }

  private:
  };


  template<typename ValueT>
  class optional_input_parameter_interface : public input_parameter_interface
  {
  public:

    optional_input_parameter_interface(base_algorithm & algorithm, std::string const & parameter_name) :
      input_parameter_interface(algorithm, parameter_name), fetched(false) {}

    typename result_of::const_parameter_handle<ValueT>::type get() const
    {
      fetch();
      return native_handle;
    }

    ValueT const & operator()() const
//     ValueT operator()() const
    { return get()(); }

    bool valid() const
    { return get(); }

  private:
    void reset()
    {
      fetched = false;
    }

    void fetch() const
    {
      if (!fetched)
      {
        native_handle = input_parameter_interface::get<ValueT>();
        fetched = true;
      }
    }

    mutable bool fetched;
    mutable typename result_of::const_parameter_handle<ValueT>::type native_handle;
  };


  template<typename ValueT>
  class default_input_parameter_interface : public input_parameter_interface
  {
  public:

    default_input_parameter_interface(base_algorithm & algorithm, std::string const & parameter_name, ValueT const & default_value_) :
      input_parameter_interface(algorithm, parameter_name), default_value(default_value_) {}

//     ValueT const & operator()() const
    ValueT operator()() const
    {
      typename result_of::const_parameter_handle<ValueT>::type param = input_parameter_interface::get<ValueT>();
      if (param)
        return param();
      return default_value;
    }

  private:
    ValueT default_value;
  };




  template<typename ValueT>
  class output_parameter_proxy
  {
    friend class base_output_parameter_interface;

  private:
    typedef typename result_of::parameter_handle<ValueT>::type ParameterHandleType;

    void init()
    {
      is_native_ = false;
      used_ = false;

      base_handle = algorithm.outputs.get(name);
      native_handle = dynamic_handle_cast<ValueT>( base_handle );

      if (native_handle)
      {
        is_native_ = true;
      }
      else
      {
        if (base_handle && native_handle && !is_convertable(native_handle, base_handle))
          throw output_not_convertable_to_referenced_value_exception( "Output parameter '" + name + "' is not convertable to referenced value" );

        is_native_ = false;
        native_handle = make_parameter<ValueT>();
      }
    }

    output_parameter_proxy(base_algorithm & algorithm_, string const & name_) : algorithm(algorithm_), name(name_)
    {
      init();
    }

  public:

    output_parameter_proxy( output_parameter_interface & output_interface ) :
      algorithm(output_interface.algorithm()), name(output_interface.name())
    {
      init();
    }

    ~output_parameter_proxy()
    {
      if (used_ && !is_native_)
      {
        if (base_handle)
        {
          if (!convert(native_handle, base_handle))
            error(1) << "output_parameter_proxy::~output_parameter_proxy() - convert failed -> BUG!!" << std::endl;
        }
        else
        {
          algorithm.outputs.set(name, native_handle);
        }
      }
    }

    bool is_native() const { return is_native_; }

    ValueT & operator()() { used_ = true; return native_handle(); }
    ValueT const & operator()() const { return native_handle(); }


    bool operator==( typename viennamesh::result_of::parameter_handle<const ValueT>::type const & ph )
    {
      if (!is_native())
        return false;

      return ph == native_handle;
    }

    bool operator!=( typename viennamesh::result_of::parameter_handle<const ValueT>::type const & ph )
    {
      return !(*this == ph);
    }

  private:
    base_algorithm & algorithm;
    string name;

    bool is_native_;
    bool used_;

    parameter_handle base_handle;
    ParameterHandleType native_handle;
  };


  template<typename ValueT>
  bool operator==( typename viennamesh::result_of::parameter_handle<const ValueT>::type const & ph, output_parameter_proxy<ValueT> const & oop )
  { return oop == ph; }

  template<typename ValueT>
  bool operator!=( typename viennamesh::result_of::parameter_handle<const ValueT>::type const & ph, output_parameter_proxy<ValueT> const & oop )
  { return !(ph == oop); }



//   struct parameter_information
//   {
//     parameter_information() {}
//
//     parameter_information(std::string const & type_,
//                           std::string const & description_,
//                           bool required_) :
//       type(type_), description(description_), default_value(""), required(required_) {}
//
//     parameter_information(std::string const & type_,
//                           std::string const & description_,
//                           std::string const & default_value_,
//                           std::string const & valid_range_,
//                           bool required_) :
//       type(type_), description(description_), default_value(default_value_), valid_range(valid_range_), required(required_) {}
//
//     std::string type;
//     std::string description;
//     std::string default_value;
//     std::string valid_range;
//     bool required;
//   };














}

#endif
