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
#include "viennagrid/viennagrid.hpp"

namespace viennamesh
{
  // http://en.wikipedia.org/wiki/Laplacian_smoothing
  // http://graphics.stanford.edu/courses/cs468-12-spring/LectureSlides/06_smoothing.pdf
  void laplace_smooth_impl( viennagrid::mesh const & mesh, viennagrid_numeric lambda )
  {
    typedef viennagrid::mesh                                        MeshType;

    typedef viennagrid::result_of::point<MeshType>::type            PointType;
    typedef viennagrid::result_of::element<MeshType>::type          VertexType;

    typedef viennagrid::result_of::vertex_range<MeshType>::type     VertexRangeType;
    typedef viennagrid::result_of::iterator<VertexRangeType>::type  VertexIteratorType;

    viennagrid_dimension dim = viennagrid::geometric_dimension(mesh);

    VertexRangeType vertices(mesh);

    std::vector<PointType> vertex_offset_container(vertices.size(), PointType(dim));
    viennagrid::result_of::accessor<std::vector<PointType>, VertexType>::type vertex_offset(vertex_offset_container);

    for (VertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      if (viennagrid::is_any_boundary(*vit))
        continue;

      typedef viennagrid::result_of::coboundary_range<MeshType>::type CoboundaryVertexRangeType;
      typedef viennagrid::result_of::iterator<CoboundaryVertexRangeType>::type CoboundaryLineIteratorType;

      PointType sum(dim);
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

      vertex_offset.set(*vit, sum);
    }

    for (VertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
      viennagrid::set_point( *vit, viennagrid::get_point(*vit) + vertex_offset.get(*vit) );
  }








  template<typename RegionRangeT>
  bool is_subset(RegionRangeT const & subset, RegionRangeT const & superset)
  {
    typedef typename viennagrid::result_of::const_iterator<RegionRangeT>::type RegionRangeIterator;

    for (RegionRangeIterator it = subset.begin(); it != subset.end(); ++it)
    {
      RegionRangeIterator jt = superset.begin();
      for (; jt != superset.end(); ++jt)
        if ( (*it).id() == (*jt).id() )
          break;

      if (jt == superset.end())
        return false;
    }

    return true;
  }


  void hull_laplace_smooth_impl( viennagrid::mesh const & mesh, viennagrid_numeric lambda )
  {
    typedef viennagrid::mesh                                            MeshType;

    typedef viennagrid::result_of::point<MeshType>::type                PointType;
    typedef viennagrid::result_of::element<MeshType>::type              VertexType;

    typedef viennagrid::result_of::vertex_range<MeshType>::type         VertexRangeType;
    typedef viennagrid::result_of::iterator<VertexRangeType>::type      VertexIteratorType;

    viennagrid_dimension dim = viennagrid::geometric_dimension(mesh);

    VertexRangeType vertices(mesh);

    std::vector<PointType> vertex_offset_container(vertices.size(), PointType(dim));
    viennagrid::result_of::accessor<std::vector<PointType>, VertexType>::type vertex_offset(vertex_offset_container);

    for (VertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      typedef viennagrid::result_of::region_range<VertexType>::type RegionRangeType;
//       typedef typename viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

      typedef viennagrid::result_of::neighbor_range<MeshType>::type NeighborVertexRangeType;
      typedef viennagrid::result_of::iterator<NeighborVertexRangeType>::type NeighborVertexRangeIterator;

      PointType offset(dim);
      int point_count = 0;

      PointType point = viennagrid::get_point(*vit);

      NeighborVertexRangeType neighbor_vertices(mesh, *vit, 1, 0);

      RegionRangeType regions(*vit);
//       std::cout << regions.size() << std::endl;

      if (regions.size() == 2)
      {
        for (NeighborVertexRangeIterator nvit = neighbor_vertices.begin(); nvit != neighbor_vertices.end(); ++nvit)
        {
          offset += viennagrid::get_point(*nvit) - point;
          ++point_count;
        }
      }
//       else if (regions.size() == 3)
//       {
//         for (NeighborVertexRangeIterator nvit = neighbor_vertices.begin(); nvit != neighbor_vertices.end(); ++nvit)
//         {
//           RegionRangeType neighbor_regions(mesh, *nvit);
//           if ( is_subset(neighbor_regions, regions) )
//           {
//             offset += viennagrid::get_point(*nvit) - point;
//             ++point_count;
//           }
//         }
//       }

      if (point_count != 0)
        offset /= point_count;
      vertex_offset.set(*vit, offset * lambda);
    }

    for (VertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
      viennagrid::set_point( *vit, viennagrid::get_point(*vit) + vertex_offset.get(*vit) );
  }




  laplace_smooth::laplace_smooth() {}
  std::string laplace_smooth::name() { return "laplace_smooth"; }


  bool laplace_smooth::run(viennamesh::algorithm_handle &)
  {
    data_handle<double> lambda = get_required_input<double>("lambda");
    data_handle<int> iteration_count = get_required_input<int>("iteration_count");

    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    if (!input_mesh.valid())
      return false;

    int geometric_dimension = viennagrid::geometric_dimension( input_mesh() );
    int cell_dimension = viennagrid::cell_dimension( input_mesh() );

    if (!lambda.valid())
      return false;

    if (!iteration_count.valid())
      return false;

    mesh_handle output_mesh = make_data<mesh_handle>();

    if (output_mesh != input_mesh)
      viennagrid::copy( input_mesh(), output_mesh() );


    function< void(viennagrid::mesh const &) > smooth_function;
    if (geometric_dimension == 3 && cell_dimension == 2)
    {
      info(1) << "Geometric dimension == 3 and cell dimension == 2 -> using hull laplacian smoothing" << std::endl;
      smooth_function = bind( hull_laplace_smooth_impl, _1, lambda() );
    }
    else if (geometric_dimension == cell_dimension)
    {
      info(1) << "Geometric dimension == cell dimension -> using standard laplacian smoothing" << std::endl;
      smooth_function = bind( laplace_smooth_impl, _1, lambda() );
    }
    else
    {
      info(1) << "Laplacian smoothing not supported for geometric dimension == " << geometric_dimension << " and cell dimension == " << cell_dimension << std::endl;
      return false;
    }

//     typedef void (*hull_laplace_smooth_impl)(viennagrid::mesh_t const &, double) SmoothFunction

    for (int i = 0; i < iteration_count(); ++i)
      smooth_function( output_mesh() );

    set_output( "mesh", output_mesh );

    return true;
  }

}
