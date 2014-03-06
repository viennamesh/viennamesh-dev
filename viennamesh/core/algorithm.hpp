#ifndef VIENNAMESH_CORE_ALGORITHM_HPP
#define VIENNAMESH_CORE_ALGORITHM_HPP

#include <exception>
#include "viennagrid/mesh/element_creation.hpp"

#include "viennamesh/core/parameter.hpp"
#include "viennamesh/core/basic_parameters.hpp"

#include "viennamesh/utils/logger.hpp"



namespace viennamesh
{

  class base_algorithm;

  typedef shared_ptr<base_algorithm> algorithm_handle;
  typedef shared_ptr<const base_algorithm> const_algorithm_handle;



  class algorithm_exception : public std::exception
  {
  public:
    virtual ~algorithm_exception() throw() {}
  };


  class input_parameter_not_found_exception : public algorithm_exception
  {
  public:

    input_parameter_not_found_exception(string const & parameter_name) : parameter_name_(parameter_name) {}

    virtual ~input_parameter_not_found_exception() throw() {}
    virtual const char* what() const throw()
    {
      std::stringstream ss;
      ss << "Input parameter '" << parameter_name_ << "' is missing or of non-convertable type";
      return ss.str().c_str();
    }

  private:
    string parameter_name_;
  };


  class output_not_convertable_to_referenced_value_exception : public algorithm_exception
  {
  public:

    output_not_convertable_to_referenced_value_exception(string const & parameter_name) : parameter_name_(parameter_name) {}

    virtual ~output_not_convertable_to_referenced_value_exception() throw() {}
    virtual const char* what() const throw()
    {
      std::stringstream ss;
      ss << "Output parameter '" << parameter_name_ << "' is not convertable to referenced value";
      return ss.str().c_str();
    }

  private:
    string parameter_name_;
  };





  template<typename ValueT>
  class output_parameter_proxy
  {
  public:
    typedef typename result_of::parameter_handle<ValueT>::type ParameterHandleType;

    output_parameter_proxy() : parameters(NULL), is_native_(false), used_(false) {}

    void init(parameter_set & parameters_, string const & name_)
    {
      parameters = &parameters_;
      name = name_;
      used_ = false;

      base_handle = parameters->get(name);
      native_handle = dynamic_handle_cast<ValueT>( base_handle );

      if (native_handle)
      {
        is_native_ = true;
      }
      else
      {
        if (base_handle && native_handle && !is_convertable(native_handle, base_handle))
          throw output_not_convertable_to_referenced_value_exception(name_);

        is_native_ = false;
        native_handle = make_parameter<ValueT>();
      }
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
          parameters->set(name, native_handle);
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
    parameter_set * parameters;
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

  class parameter_link;

  class base_algorithm : public enable_shared_from_this<base_algorithm>
  {
  public:
    virtual ~base_algorithm() {}

    algorithm_handle handle() { return shared_from_this(); }
    const_algorithm_handle chandle() const { return shared_from_this(); }
    const_algorithm_handle handle() const { return chandle(); }


    // sets an input parameter
    template<typename TypeT>
    void set_input( string const & name, TypeT const & value )
    { inputs.set( name, static_cast<parameter_handle const &>(make_parameter(value)) ); }

    template<typename ValueT>
    void set_input( string const & name, parameter_handle_t< const parameter_wrapper<ValueT> > const & parameter )
    { inputs.set(name, static_cast<const_parameter_handle const &>(parameter)); }

    template<typename ValueT>
    void set_input( string const & name, parameter_handle_t< parameter_wrapper<ValueT> > const & parameter )
    { inputs.set(name, static_cast<parameter_handle const &>(parameter)); }

    void set_input( string const & name, parameter_handle_t<parameter_link> const & parameter )
    { inputs.set(name, static_cast<parameter_handle const &>(parameter)); }

    void set_input( string const & name, char const * value )
    { set_input( name, make_parameter( string(value) ) ); }

    void set_input( string const & name, char * value )
    { set_input( name, make_parameter( string(value) ) ); }





    // unsets an input parameter
    void unset_input( string const & name ) { inputs.unset(name); }


    // queries an input parameter
    const_parameter_handle get_input( string const & name ) const
    { return inputs.get(name); }

  protected:

    const_parameter_handle get_required_input( string const & name ) const
    {
      const_parameter_handle param = get_input(name);
      if (!param)
        throw input_parameter_not_found_exception(name);
      return param;
    }

  public:

    // queries an input parameter of special type
    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get_input( string const & name ) const
    { return inputs.get<ValueT>(name); }


  protected:

    // queries an input parameter of special type, throws input_parameter_not_found_exception if not found
    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get_required_input( string const & name ) const
    {
      typename result_of::const_parameter_handle<ValueT>::type param = get_input<ValueT>(name);
      if (!param)
        throw input_parameter_not_found_exception(name);
      return param;
    }



    // copies input parameter of special type to value, only works if input is present and convertable, returns true if copy was performed
    template<typename ValueT>
    bool copy_input( string const & name, ValueT & value ) const
    { return inputs.copy_if_present(name, value); }

  public:

    // references an output parameter to a specific value, doesn't take ownership, remembers pointer to value
    template<typename TypeT>
    void set_output( string const & name, TypeT & value )
    { outputs.set( name, make_reference_parameter(value) ); }

    void set_output( string const & name, parameter_handle const & handle )
    { outputs.set( name, parameter_handle( new parameter_handle_reference(handle) ) ); }

    void set_output( string const & name, parameter_handle & handle )
    { outputs.set( name, parameter_handle( new parameter_handle_reference(handle) ) ); }

  protected:

    // gets a proxy for an output parameter, only way of setting an output parameter, used within an algorithm
    template<typename ValueT>
    output_parameter_proxy<ValueT> output_proxy( string const & name )
    {
      output_parameter_proxy<ValueT> proxy;
      proxy.init(outputs, name);
      return proxy;
    }

  public:

    // queries an output parameter
    const_parameter_handle get_output( string const & name ) const
    { return outputs.get(name); }

    parameter_handle get_output( string const & name )
    { return outputs.get(name); }

    // queries an output parameter of special type
    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get_output( string const & name ) const
    { return outputs.get<ValueT>(name); }

    template<typename ValueT>
    typename result_of::parameter_handle<ValueT>::type get_output( string const & name )
    { return outputs.get<ValueT>(name); }

    // unsets an output parameter
    void unset_output( string const & name ) { outputs.unset(name); } // TODO needed?

    // clears all output parameters
    void clear_outputs() { outputs.clear(); } // TODO needed?

    // runs the algorithm
    bool run()
    {
      LoggingStack stack( string("Algoritm: ") + name() );
      outputs.clear_non_references();

      try
      {
        bool success = run_impl();

        if (!success)
          error(1) << "Algorithm failed" << std::endl;

        return success;
      }
      catch ( algorithm_exception const & ex )
      {
        error(1) << "Algorithm failed!" << std::endl;
        error(1) << ex.what() << std::endl;
        return false;
      }
    }

    // returns the algorithm name
    virtual string name() const = 0;

  protected:

    virtual bool run_impl() = 0;

  private:

    const_parameter_set inputs;
    parameter_set outputs;
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



  inline parameter_handle_t<parameter_link> make_parameter_link( algorithm_handle const & algorithm, string const & para_name )
  {
    return parameter_handle_t<parameter_link>( new parameter_link(algorithm, para_name) );
  }










}

#endif
