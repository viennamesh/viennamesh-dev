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




  class BaseAlgorithm
  {
  public:
    virtual ~BaseAlgorithm() {}

    template<typename TypeT>
    void set_input( string const & usage, TypeT const & value )
    {
      inputs.set( usage, value );
    }
//     { inputs[usage] = shared_ptr< ParameterWrapper<TypeT> >( new ParameterWrapper<TypeT>(value) ); }

    void set_input( string const & usage, char const * value )
    {
      inputs.set( usage, string(value) );
    }
//     { inputs[usage] = shared_ptr< ParameterWrapper<string> >( new ParameterWrapper<string>(value) ); }

//     void set_input( string const & usage, ParameterHandle const & parameter ) { inputs.set( usage, parameter ); }
//     void set_input( string const & usage, ConstParameterHandle const & parameter ) { inputs.set( usage, parameter ); }


    ConstParameterHandle get_input( string const & usage ) const
    {
      return inputs.get(usage);
    }

    ParameterHandle get_output( string const & usage ) const
    {
      return outputs.get(usage);
    }

    void unset_input( string const & usage ) { inputs.unset(usage); }
    void unset_output( string const & usage ) { outputs.unset(usage); }

    virtual bool run() = 0;

  protected:
    typedef ConstParameterSet InputParameterContainerType;
    typedef ParameterSet OutputParameterContainerType;

    InputParameterContainerType inputs;
    OutputParameterContainerType outputs;
  };

  typedef shared_ptr<BaseAlgorithm> AlgorithmHandle;


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

    bool run()
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
