#include "viennamesh/algorithm/affine_transform.hpp"

#include "viennagrid/algorithm/geometric_transform.hpp"

namespace viennamesh
{
  template<typename MeshT, typename SegmentationT>
  bool affine_transform::generic_run( dynamic_point const & matrix, dynamic_point const & base_translate )
  {
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;

    typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type input_mesh = get_input<SegmentedMeshType>("default");
    if (input_mesh)
    {
      output_parameter_proxy<SegmentedMeshType> output_mesh = output_proxy<SegmentedMeshType>( "default" );

      PointType translate;
      std::copy( base_translate.begin(), base_translate.end(), translate.begin() );

      if (output_mesh != input_mesh)
        output_mesh() = input_mesh();

      viennagrid::affine_transform( output_mesh().mesh, &matrix[0], translate );

      return true;
    }

    return false;
  }

  bool affine_transform::run_impl()
  {
    viennamesh::const_parameter_handle mesh = get_input("default");
    if (!mesh)
    {
      error(1) << "Input Parameter 'default' (type: mesh) is missing" << std::endl;
      return false;
    }

    viennamesh::result_of::const_parameter_handle<dynamic_point>::type matrix = get_required_input<dynamic_point>( "matrix" );
    viennamesh::result_of::const_parameter_handle<dynamic_point>::type base_translate = get_input<dynamic_point>("translate");


    unsigned int mesh_geometric_dimension = lexical_cast<unsigned int>( mesh->get_property("geometric_dimension").first );

    if (mesh_geometric_dimension*mesh_geometric_dimension != matrix().size())
    {
      error(1) << "Dimension missmatch, mesh has geometric dimension " << mesh_geometric_dimension << " but matrix has dimension " << matrix().size() << std::endl;
      return false;
    }

    dynamic_point translate(mesh_geometric_dimension, 0.0);
    if (base_translate)
    {
      if (mesh_geometric_dimension != base_translate().size())
      {
        error(1) << "Dimension missmatch, mesh has geometric dimension " << mesh_geometric_dimension << " but translate vector has dimension " << base_translate().size() << std::endl;
        return false;
      }

      translate = base_translate();
    }


    if (generic_run<viennagrid::line_2d_mesh, viennagrid::line_2d_segmentation>(matrix(), translate))
      return true;

    if (generic_run<viennagrid::line_3d_mesh, viennagrid::line_3d_segmentation>(matrix(), translate))
      return true;

    if (generic_run<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation>(matrix(), translate))
      return true;

    if (generic_run<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation>(matrix(), translate))
      return true;

    if (generic_run<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>(matrix(), translate))
      return true;


    error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
    return false;
  }

}
