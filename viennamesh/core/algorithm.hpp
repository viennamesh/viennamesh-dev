#ifndef VIENNAMESH_CORE_ALGORITHM_HPP
#define VIENNAMESH_CORE_ALGORITHM_HPP

#include <exception>
#include "viennagrid/mesh/element_creation.hpp"

#include "viennamesh/core/parameter.hpp"
#include "viennamesh/core/basic_parameters.hpp"

#include "viennamesh/utils/logger.hpp"
#include "viennamesh/utils/std_capture.hpp"



namespace viennamesh
{

  class BaseAlgorithm;

  typedef shared_ptr<BaseAlgorithm> AlgorithmHandle;



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
  class OutputParameterProxy
  {
  public:
    typedef typename result_of::parameter_handle<ValueT>::type ParameterHandleType;

    OutputParameterProxy() : parameters(NULL), is_native_(false), used_(false) {}

    void init(ParameterSet & parameters_, string const & name_)
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

    ~OutputParameterProxy()
    {
      if (used_ && !is_native_)
      {
        if (base_handle)
        {
          if (!convert(native_handle, base_handle))
            error(1) << "OutputParameterProxy::~OutputParameterProxy() - convert failed -> BUG!!" << std::endl;
        }
        else
        {
          parameters->set(name, native_handle);
        }
      }
    }

    bool is_native() const { return is_native_; }

    ValueT & operator()() { used_ = true; return native_handle->get(); }
    ValueT const & operator()() const { used_ = true; return native_handle->get(); }

  private:
    ParameterSet * parameters;
    string name;

    bool is_native_;
    bool used_;

    ParameterHandle base_handle;
    ParameterHandleType native_handle;
  };



  class BaseAlgorithm : public enable_shared_from_this<BaseAlgorithm>
  {
  public:
    virtual ~BaseAlgorithm() {}

    // sets an input parameter
    template<typename TypeT>
    void set_input( string const & name, TypeT const & value )
    { inputs.set( name, value ); }

    // links an input parameter to an output parameter of another algorithm
    void link_input( string const & name, AlgorithmHandle const & algorithm, string const & output_name )
    { set_input( name, ParameterLinkHandle(new ParameterLink( algorithm->outputs, output_name )) ); }

    // unsets an input parameter
    void unset_input( string const & name ) { inputs.unset(name); }


    // queries an input parameter
    ConstParameterHandle get_input( string const & name ) const
    { return inputs.get(name); }

    // queries an input parameter of special type
    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get_input( string const & name ) const
    { return inputs.get<ValueT>(name); }

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



    // references an output parameter to a specific value, doesn't take ownership, remembers pointer to value
    template<typename TypeT>
    void reference_output( string const & name, TypeT & value )
    { outputs.set( name, make_reference_parameter(value) ); }

    // gets a proxy for an output parameter, only way of setting an output parameter, used within an algorithm
    template<typename ValueT>
    OutputParameterProxy<ValueT> output_proxy( string const & name )
    {
      OutputParameterProxy<ValueT> proxy;
      proxy.init(outputs, name);
      return proxy;
    }


    // queries an output parameter
    ConstParameterHandle get_output( string const & name ) const
    { return outputs.get(name); }

    ParameterHandle get_output( string const & name )
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
        return run_impl();
      }
      catch ( algorithm_exception const & ex )
      {
        error(1) << ex.what() << std::endl;
        return false;
      }
    }

    // returns the algorithm name
    virtual string name() const = 0;

  protected:

    virtual bool run_impl() = 0;

  private:

    ConstParameterSet inputs;
    ParameterSet outputs;
  };
}

#endif
