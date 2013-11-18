#ifndef VIENNAMESH_CORE_ALGORITHM_HPP
#define VIENNAMESH_CORE_ALGORITHM_HPP


#include "viennagrid/mesh/element_creation.hpp"

#include "viennamesh/core/parameter.hpp"
#include "viennamesh/core/basic_parameters.hpp"

#include "viennamesh/utils/logger.hpp"
#include "viennamesh/utils/std_capture.hpp"



namespace viennamesh
{

  class BaseAlgorithm;

  typedef shared_ptr<BaseAlgorithm> AlgorithmHandle;



  class BaseAlgorithm : public enable_shared_from_this<BaseAlgorithm>
  {
  public:
    virtual ~BaseAlgorithm() {}

    template<typename TypeT>
    void set_input( string const & name, TypeT const & value )
    {
      inputs.set( name, value );
    }

//     void set_input( string const & name, char const * value )
//     {
//       inputs.set( name, string(value) );
//     }

    void link_input( string const & name, AlgorithmHandle const & algorithm, string const & output_name )
    {
      set_input( name, ParameterLinkHandle(new ParameterLink( algorithm->outputs, output_name )) );
    }


    ConstParameterHandle get_input( string const & name ) const
    {
      return inputs.get(name);
    }


    ConstParameterHandle get_output( string const & name ) const
    {
      return outputs.get(name);
    }


    template<typename TypeT>
    void reference_output( string const & name, TypeT & value )
    {
      outputs.set( name, make_reference_parameter(value) );
    }

    ParameterHandle get_output( string const & name )
    {
      return outputs.get(name);
    }

    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get_output( string const & name ) const
    {
      return outputs.get<ValueT>(name);
    }

    template<typename ValueT>
    typename result_of::parameter_handle<ValueT>::type get_output( string const & name )
    {
      return outputs.get<ValueT>(name);
    }


    void unset_input( string const & name ) { inputs.unset(name); }
    void unset_output( string const & name ) { outputs.unset(name); }

    void clear_outputs() { outputs.clear(); }

    bool run()
    {
      LoggingStack stack( string("Algoritm: ") + name() );
      return run_impl();
    }

    virtual string name() const = 0;

  protected:

    virtual bool run_impl() = 0;

    ConstParameterSet inputs;
    ParameterSet outputs;
  };
}

#endif
