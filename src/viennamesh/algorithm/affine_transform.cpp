/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include "viennamesh/algorithm/affine_transform.hpp"

#include "viennagrid/algorithm/geometric_transform.hpp"

namespace viennamesh
{
  affine_transform::affine_transform() :
    input_mesh(*this, parameter_information("mesh","mesh","The input mesh, segmented line 2d mesh, segmented line 3d mesh, segmented triangular 2d mesh, segmented triangular 3d mesh and segmented tetrahedral 3d mesh supported")),
    matrix(*this, parameter_information("matrix", "dynamic_point", "The linear transformation matrix")),
    translate(*this, parameter_information("translate", "dynamic_point", "The translation matrix")),
    output_mesh(*this, parameter_information("mesh", "mesh", "The output mesh, same type of mesh as input mesh")) {}

  std::string affine_transform::name() const { return "ViennaGrid Affine Transform"; }
  std::string affine_transform::id() const { return "affine_transform"; }

  template<typename MeshT, typename SegmentationT>
  bool affine_transform::generic_run( dynamic_point const & matrix, dynamic_point const & base_translate )
  {
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;

    typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type imp = input_mesh.get<SegmentedMeshType>();
    if (imp)
    {
      output_parameter_proxy<SegmentedMeshType> omp(output_mesh);

      PointType translate;
      std::copy( base_translate.begin(), base_translate.end(), translate.begin() );

      if (omp != imp)
        omp() = imp();

      viennagrid::affine_transform( omp().mesh, &matrix[0], translate );

      return true;
    }

    return false;
  }

  bool affine_transform::run_impl()
  {
    viennamesh::const_parameter_handle mesh = input_mesh.get();

    unsigned int mesh_geometric_dimension = lexical_cast<unsigned int>( mesh->get_property("geometric_dimension").first );

    if (mesh_geometric_dimension*mesh_geometric_dimension != matrix().size())
    {
      error(1) << "Dimension missmatch, mesh has geometric dimension " << mesh_geometric_dimension << " but matrix has dimension " << matrix().size() << std::endl;
      return false;
    }

    dynamic_point translate_vector(mesh_geometric_dimension, 0.0);
    if (translate.valid())
    {
      if (mesh_geometric_dimension != translate().size())
      {
        error(1) << "Dimension missmatch, mesh has geometric dimension " << mesh_geometric_dimension << " but translate vector has dimension " << translate().size() << std::endl;
        return false;
      }

      translate_vector = translate();
    }


    if (generic_run<viennagrid::line_2d_mesh, viennagrid::line_2d_segmentation>(matrix(), translate_vector))
      return true;

    if (generic_run<viennagrid::line_3d_mesh, viennagrid::line_3d_segmentation>(matrix(), translate_vector))
      return true;

    if (generic_run<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation>(matrix(), translate_vector))
      return true;

    if (generic_run<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation>(matrix(), translate_vector))
      return true;

    if (generic_run<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>(matrix(), translate_vector))
      return true;


    error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
    return false;
  }

}
