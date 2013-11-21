#ifndef VIENNAMESH_ALGORITHM_FILE_WRITER_HPP
#define VIENNAMESH_ALGORITHM_FILE_WRITER_HPP

#include "viennamesh/core/algorithm.hpp"

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/vtk_writer.hpp"


namespace viennamesh
{

  template<typename MeshT>
  bool writeToFile( const_parameter_handle const & mesh, string const & filename )
  {
    typename result_of::const_parameter_handle<MeshT>::type tmp = dynamic_handle_cast<const MeshT>( mesh );
    if (!tmp)
      tmp = mesh->get_converted<MeshT>();

    if (!tmp)
      return false;

    info(5) << "Found conversion to ViennaGrid mesh." << std::endl;

    string extension = filename.substr( filename.rfind(".")+1 );

    if (extension == "vtu" || extension == "pvd")
    {
      info(5) << "Found .vtu/.pvd extension, using ViennaGrid VTK Writer" << std::endl;
      viennagrid::io::vtk_writer<MeshT> vtk_writer;
      vtk_writer( tmp->value(), filename.substr(0, filename.rfind(".")) );
      return true;
    }

    error(1) << "Unsupported extension: " << extension << std::endl;

    return false;
  }


  template<typename MeshT, typename SegmentationT>
  bool writeToFile( const_parameter_handle const & mesh, string const & filename )
  {
    typedef viennagrid::segmented_mesh<MeshT, SegmentationT> WrappedMeshType;
    typename result_of::const_parameter_handle<WrappedMeshType>::type tmp = dynamic_handle_cast<const WrappedMeshType>( mesh );
    if (!tmp)
      tmp = mesh->get_converted<WrappedMeshType>();

    if (!tmp)
      return false;

    info(5) << "Found conversion to ViennaGrid mesh." << std::endl;

    string extension = filename.substr( filename.rfind(".")+1 );

    if (extension == "vtu" || extension == "pvd")
    {
      info(5) << "Found .vtu/.pvd extension, using ViennaGrid VTK Writer" << std::endl;
      viennagrid::io::vtk_writer<MeshT, SegmentationT> vtk_writer;
      vtk_writer( tmp->value().mesh, tmp->value().segmentation, filename.substr(0, filename.rfind(".")) );
      return true;
    }

    error(1) << "Unsupported extension: " << extension << std::endl;

    return false;
  }



  class file_writer : public base_algorithm
  {
  public:

    string name() const { return "ViennaGrid FileWriter"; }

    bool run_impl()
    {
      const_parameter_handle mesh = get_input("default");
      if (!mesh)
      {
        error(1) << "Input Parameter 'default' (type: mesh) is missing" << std::endl;
        return false;
      }

      const_string_parameter_handle filename = get_input<string>("filename");
      if (!filename)
      {
        error(1) << "Input Parameter 'filename' (type: string) is missing" << std::endl;
        return false;
      }



      if (writeToFile<viennagrid::line_1d_mesh, viennagrid::line_1d_segmentation>(mesh, filename->value()))
        return true;

      if (writeToFile<viennagrid::line_1d_mesh>(mesh, filename->value()))
        return true;

      if (writeToFile<viennagrid::line_2d_mesh, viennagrid::line_2d_segmentation>(mesh, filename->value()))
        return true;

      if (writeToFile<viennagrid::line_2d_mesh>(mesh, filename->value()))
        return true;

      if (writeToFile<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation>(mesh, filename->value()))
        return true;

      if (writeToFile<viennagrid::triangular_2d_mesh>(mesh, filename->value()))
        return true;


      if (writeToFile<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation>(mesh, filename->value()))
        return true;

      if (writeToFile<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>(mesh, filename->value()))
        return true;

      error(1) << "Input mesh is not convertable to any supported ViennaGrid mesh." << std::endl;

      return false;
    }

  private:

  };



}



#endif
