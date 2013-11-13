#ifndef VIENNAMESH_CORE_DYNAMIC_ALGORITHM_HPP
#define VIENNAMESH_CORE_DYNAMIC_ALGORITHM_HPP


#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/core/parameter.hpp"
#include "viennamesh/core/basic_parameters.hpp"

#include "viennamesh/utils/logger.hpp"
#include "viennamesh/utils/std_capture.hpp"


namespace viennamesh
{
  namespace result_of
  {
    template<typename AlgorithmTagT>
    struct native_input_mesh_wrapper;

    template<typename AlgorithmTagT>
    struct native_output_mesh_wrapper;
  }



  class BaseAlgorithm;

  typedef shared_ptr<BaseAlgorithm> AlgorithmHandle;



  class BaseAlgorithm
  {
  public:
    virtual ~BaseAlgorithm() {}

    template<typename TypeT>
    void set_input( string const & name, TypeT const & value )
    {
      inputs.set( name, value );
    }

    void set_input( string const & name, char const * value )
    {
      inputs.set( name, string(value) );
    }

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

    ParameterHandle get_output( string const & name )
    {
      return outputs.get(name);
    }

    void unset_input( string const & name ) { inputs.unset(name); }
    void unset_output( string const & name ) { outputs.unset(name); }

    bool run()
    {
      outputs.clear();
      return run_impl();
    }

  protected:

    virtual bool run_impl() = 0;

    ConstParameterSet inputs;
    ParameterSet outputs;
  };




  template<typename AlgorithmTagT>
  class Algorithm : public BaseAlgorithm
  {
    typedef typename result_of::native_input_mesh_wrapper<AlgorithmTagT>::type NativeInputMeshWrapperType;
    typedef typename result_of::native_output_mesh_wrapper<AlgorithmTagT>::type NativeOutputMeshWrapperType;

  public:

    Algorithm()
    {
      static_init<NativeInputMeshWrapperType>::init();
      static_init<NativeOutputMeshWrapperType>::init();
    }

    bool run_impl()
    {
      LoggingStack stack( string("Algoritm: ") + result_of::algorithm_info<AlgorithmTagT>::name() );

      ConstParameterHandle input = inputs.get("default");
      if (!input)
      {
        error(1) << "Input Parameter 'default' (type: mesh) is missing" << std::endl;
        return false;
      }

      ParameterHandle & output = outputs.get_create("default");

      typename result_of::const_parameter_handle<NativeInputMeshWrapperType>::type native_input_mesh = dynamic_handle_cast<const NativeInputMeshWrapperType>(input);
      if (!native_input_mesh)
        native_input_mesh = input->get_converted<NativeInputMeshWrapperType>();

      if (!native_input_mesh)
      {
        error(1) << "Input Parameter 'default' is not convertable to native input type" << std::endl;
        Converter::get().print_conversions(input);
        info(10) << "Required Type: [" << &typeid(ParameterWrapper<NativeInputMeshWrapperType>) << "]" << std::endl;
        return false;
      }

      viennautils::StdCapture capture;
      capture.start();

      typename result_of::parameter_handle<NativeOutputMeshWrapperType>::type native_output_mesh = make_parameter<NativeOutputMeshWrapperType>();
      viennamesh::run_algo<AlgorithmTagT>( native_input_mesh->value.mesh, native_output_mesh->value.mesh, inputs );
      output = native_output_mesh;

      capture.finish();

      info(5) << capture.get() << std::endl;

      return true;
    }
  };
}

#endif
