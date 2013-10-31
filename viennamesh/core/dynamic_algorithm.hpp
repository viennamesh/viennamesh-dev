#ifndef VIENNAMESH_CORE_DYNAMIC_ALGORITHM_HPP
#define VIENNAMESH_CORE_DYNAMIC_ALGORITHM_HPP

#include "viennamesh/core/settings.hpp"
#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/core/dynamic_mesh.hpp"

#include <unistd.h>


namespace viennamesh
{
  namespace result_of
  {
    template<typename AlgorithmTagT>
    struct native_input_mesh;

    template<typename AlgorithmTagT>
    struct native_input_segmentation;

    template<typename AlgorithmTagT>
    struct native_output_mesh;

    template<typename AlgorithmTagT>
    struct native_output_segmentation;
  }




  class BaseAlgorithm
  {
  public:
    virtual ~BaseAlgorithm() {}

    void setInput( ConstMeshHandle const & mesh, string const & usage = "default" ) { inputs[usage] = mesh; }
    void setOutput( MeshHandle const & mesh, string const & usage = "default" ) { outputs[usage] = mesh; }

    ConstMeshHandle getInput( string const & usage = "default" ) const
    {
      InputMeshContainerType::const_iterator it = inputs.find(usage);
      if (it != inputs.end())
        return it->second;
      else
        return ConstMeshHandle();
    }

    MeshHandle getOutput( string const & usage = "default" ) const
    {
      OutputMeshContainerType::const_iterator it = outputs.find(usage);
      if (it != outputs.end())
        return it->second;
      else
        return MeshHandle();
    }

    void unsetInput( string const & usage ) { inputs.erase(usage); }
    void unsetOutput( string const & usage ) { outputs.erase(usage); }

    std::map<string, string> const & getInputDefinition() const { return input_definition; }
    std::map<string, string> const & getOutputDefinition() const { return output_definition; }



    void setParameters( ParameterSet const & parameters_ ) { parameters = parameters_; }
    ParameterSet const & getParameters() const { return parameters; }

    template<typename T>
    void setParameter( string const & name, T const & value ) { parameters.set(name, value); }

    virtual bool run() = 0;
    bool run(ConstMeshHandle const & input_mesh, MeshHandle const & output_mesh)
    {
      setInput(input_mesh);
      setOutput(output_mesh);
      return run();
    }

  protected:
    typedef std::map<string, ConstMeshHandle> InputMeshContainerType;
    typedef std::map<string, MeshHandle> OutputMeshContainerType;

    std::map<string, string> input_definition;
    std::map<string, string> output_definition;
    std::map<string, std::pair<string, string> > parameter_definition;

    InputMeshContainerType inputs;
    OutputMeshContainerType outputs;

    ParameterSet parameters;
  };

  typedef shared_ptr<BaseAlgorithm> AlgorithmHandle;


  template<typename AlgorithmTagT>
  class Algorithm : public BaseAlgorithm
  {
  public:

    Algorithm()
    {
      input_definition["default"] = "Default Input Mesh";
      output_definition["default"] = "Default Output Mesh";
    }

    bool run()
    {
      InputMeshContainerType::iterator input_it = inputs.find("default");
      if (input_it == inputs.end()) return false;

      ConstMeshHandle input = input_it->second;
      MeshHandle & output = outputs["default"];

      typedef typename result_of::native_input_mesh<AlgorithmTagT>::type NativeInputMeshType;
      typedef typename result_of::native_input_segmentation<AlgorithmTagT>::type NativeInputSegmentationType;
      typedef MeshWrapper<NativeInputMeshType, NativeInputSegmentationType> NativeInputMeshWrapperType;

      typedef typename result_of::native_output_mesh<AlgorithmTagT>::type NativeOutputMeshType;
      typedef typename result_of::native_output_segmentation<AlgorithmTagT>::type NativeOutputSegmentationType;
      typedef MeshWrapper<NativeOutputMeshType, NativeOutputSegmentationType> NativeOutputMeshWrapperType;

      shared_ptr<const NativeInputMeshWrapperType> native_input_mesh = dynamic_pointer_cast<const NativeInputMeshWrapperType>(input);
      if (!native_input_mesh)
        native_input_mesh = static_pointer_cast<const NativeInputMeshWrapperType>(input->getConverted<NativeInputMeshWrapperType>());


//       int m_oldStdOut = dup(fileno(stdout));
//       int m_oldStdErr = dup(fileno(stderr));
//       int fds[2];
//       int res;
//       char buf[4096];
//       int so;
//
//       res = pipe(fds);
//       so = fileno(stdout);
//       res = dup2(fds[1],so);

      shared_ptr<NativeOutputMeshWrapperType> native_output_mesh = dynamic_pointer_cast<NativeOutputMeshWrapperType>(output);
      if (!native_output_mesh)
      {
        native_output_mesh = shared_ptr<NativeOutputMeshWrapperType>( new NativeOutputMeshWrapperType );
        viennamesh::run_algo<AlgorithmTagT>( native_input_mesh->mesh, native_output_mesh->mesh, parameters );
        if (output)
          native_output_mesh->convert_to(output);
        else
          output = native_output_mesh;
      }
      else
        viennamesh::run_algo<AlgorithmTagT>( native_input_mesh->mesh, native_output_mesh->mesh, parameters );

//       std::cout << "HAAAAALLLLOOO" << std::endl;
//
//       fflush(stdout);
//
//       res=read(fds[0],buf,sizeof(buf)-1);
//       buf[res]=0;
//
//       dup2(m_oldStdOut, fileno(stdout));
//       dup2(m_oldStdErr, fileno(stderr));
//
//       std::cout << "\033[1;32m HAHAHAHA \033[0m" << std::endl;
//
//       std::cout << string(buf) << std::endl;

      return true;
    }
  };
}

#endif
