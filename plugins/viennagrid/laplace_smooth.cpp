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

#include "laplace_smooth.hpp"
#include "viennagrid/core.hpp"

namespace viennamesh
{
  // http://en.wikipedia.org/wiki/Laplacian_smoothing
  // http://graphics.stanford.edu/courses/cs468-12-spring/LectureSlides/06_smoothing.pdf
  template<typename NumericT>
  void laplace_smooth_impl( viennagrid::mesh_t const & mesh, NumericT lambda )
  {
    typedef viennagrid::mesh_t MeshType;

    typedef typename viennagrid::result_of::point<MeshType>::type PointType;
    typedef typename viennagrid::result_of::element<MeshType>::type VertexType;

    typedef typename viennagrid::result_of::vertex_range<MeshType>::type VertexRangeType;
    typedef typename viennagrid::result_of::iterator<VertexRangeType>::type VertexIteratorType;

    VertexRangeType vertices(mesh);

    std::vector<PointType> vertex_offset_container(vertices.size(), PointType());
    typename viennagrid::result_of::accessor<std::vector<PointType>, VertexType>::type vertex_offset(vertex_offset_container);

    for (VertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      if (viennagrid::is_any_boundary(mesh, *vit))
        continue;

      typedef typename viennagrid::result_of::coboundary_range<MeshType>::type CoboundaryVertexRangeType;
      typedef typename viennagrid::result_of::iterator<CoboundaryVertexRangeType>::type CoboundaryLineIteratorType;

      PointType sum;
      PointType point = viennagrid::get_point(*vit);

      CoboundaryVertexRangeType coboundary_lines(mesh, *vit, 1);
      for (CoboundaryLineIteratorType lit = coboundary_lines.begin(); lit != coboundary_lines.end(); ++lit)
      {
        PointType neighbor_point = viennagrid::get_point(
                                viennagrid::vertices(*lit)[0] == *vit ?
                                viennagrid::vertices(*lit)[1] :
                                viennagrid::vertices(*lit)[0]);

        sum += neighbor_point-point;
      }

      sum /= coboundary_lines.size();
      sum *= lambda;

      vertex_offset(*vit) = sum;
    }

    for (VertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
      viennagrid::set_point( *vit, viennagrid::get_point(*vit) + vertex_offset(*vit) );
  }


  laplace_smooth::laplace_smooth() {}
  std::string laplace_smooth::name() { return "laplace_smooth"; }


  bool laplace_smooth::run(viennamesh::algorithm_handle &)
  {
    data_handle<double> lambda = get_required_input<double>("lambda");
    data_handle<int> iteration_count = get_required_input<int>("iteration_count");

    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    if (!input_mesh)
      return false;

    if (!lambda)
      return false;

    if (!iteration_count)
      return false;

    mesh_handle output_mesh = make_data<mesh_handle>();

    if (output_mesh != input_mesh)
      viennagrid::copy( input_mesh(), output_mesh() );

    for (int i = 0; i < iteration_count(); ++i)
      laplace_smooth_impl(output_mesh(), lambda());

    set_output( "mesh", output_mesh );

    return true;
  }

}
