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

#include "viennamesh/algorithm/laplace_smooth.hpp"

#include "viennagrid/algorithm/boundary.hpp"

namespace viennamesh
{
  // http://en.wikipedia.org/wiki/Laplacian_smoothing
  // http://graphics.stanford.edu/courses/cs468-12-spring/LectureSlides/06_smoothing.pdf
  template<typename MeshT, typename SegmentationT, typename NumericT>
  void laplace_smooth_impl( MeshT & mesh, SegmentationT & segmentation, NumericT lambda )
  {
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::vertex<MeshT>::type VertexType;

    typedef typename viennagrid::result_of::vertex_range<MeshT>::type VertexRangeType;
    typedef typename viennagrid::result_of::iterator<VertexRangeType>::type VertexIteratorType;

    VertexRangeType vertices(mesh);

    std::vector<PointType> vertex_offset_container(vertices.size(), PointType());
    typename viennagrid::result_of::accessor<std::vector<PointType>, VertexType>::type vertex_offset(vertex_offset_container);

    for (VertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      bool is_on_boundary = false;
      for (typename SegmentationT::const_iterator sit = segmentation.begin(); sit != segmentation.end(); ++sit)
      {
        if (viennagrid::is_boundary(*sit, *vit))
        {
          is_on_boundary = true;
          break;
        }
      }
      if (is_on_boundary)
        continue;

      typedef typename viennagrid::result_of::coboundary_range<MeshT, viennagrid::vertex_tag, viennagrid::line_tag>::type CoboundaryVertexRangeType;
      typedef typename viennagrid::result_of::iterator<CoboundaryVertexRangeType>::type CoboundaryLineIteratorType;

      PointType sum;
      PointType point = viennagrid::point(*vit);

      CoboundaryVertexRangeType coboundary_lines(mesh, *vit);
      for (CoboundaryLineIteratorType lit = coboundary_lines.begin(); lit != coboundary_lines.end(); ++lit)
      {
        PointType neighbor_point = viennagrid::point(
                                viennagrid::vertices(*lit).handle_at(0) == vit.handle() ?
                                viennagrid::vertices(*lit)[1] :
                                viennagrid::vertices(*lit)[0]);

        sum += neighbor_point-point;
      }

      sum /= coboundary_lines.size();
      sum *= lambda;

      vertex_offset(*vit) = sum;
    }

    for (VertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
      viennagrid::point(*vit) += vertex_offset(*vit);
  }




  laplace_smooth::laplace_smooth() :
    input_mesh(*this, "mesh"),
    lambda(*this, "lambda", 0.5),
    iteration_count(*this, "iteration_count", 1),
    output_mesh(*this, "mesh") {}

  string laplace_smooth::name() const { return "ViennaGrid Laplace Smoothing"; }
  string laplace_smooth::id() const { return "laplace_smooth"; }

  template<typename MeshT, typename SegmentationT>
  bool laplace_smooth::generic_run()
  {
    typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;
    typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type imp = input_mesh.get<SegmentedMeshType>();
    if (imp)
    {
      output_parameter_proxy<SegmentedMeshType> omp(output_mesh);

      info(5) << "Using lambda             : " << lambda() << std::endl;
      info(5) << "Using iteration count of : " << iteration_count() << std::endl;

      if (omp != imp)
        omp() = imp();

      for (int i = 0; i < iteration_count(); ++i)
        laplace_smooth_impl(omp().mesh, omp().segmentation, lambda());

      return true;
    }

    return false;
  }

  bool laplace_smooth::run_impl()
  {
    if (generic_run<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation>())
      return true;

    if (generic_run<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>())
      return true;


    error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
    return false;
  }

}
