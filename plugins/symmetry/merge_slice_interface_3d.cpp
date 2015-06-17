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

#include <set>
#include <map>
#include <iterator>

#include "merge_slice_interface_3d.hpp"
#include "geometry.hpp"

#include "viennagridpp/algorithm/distance.hpp"
#include "viennagridpp/algorithm/centroid.hpp"


namespace viennamesh
{





  template<typename MeshT, typename PointT, typename NumericConfigT>
  typename viennagrid::result_of::element<MeshT>::type get_vertex(MeshT const & mesh,
                                                                  PointT const & point,
                                                                  NumericConfigT numeric_config)
  {
    typedef typename viennagrid::result_of::const_element_range<MeshT>::type ConstElementRangeType;
    typedef typename viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

    ConstElementRangeType vertices(mesh, 0);
    for (ConstElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      if ( viennagrid::detail::is_equal( numeric_config, viennagrid::get_point(*vit), point ) )
        return *vit;
    }

    return typename viennagrid::result_of::element<MeshT>::type();
  }










  merge_symmetry_slice_interface_3d::merge_symmetry_slice_interface_3d() {}
  std::string merge_symmetry_slice_interface_3d::name() { return "merge_symmetry_slice_interface_3d"; }

  bool merge_symmetry_slice_interface_3d::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    int geometric_dimension = viennagrid::geometric_dimension( input_mesh() );

    if (geometric_dimension != 3)
      return false;

    double tol = 1e-6;

    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::point<MeshType>::type PointType;
    typedef viennagrid::result_of::element<MeshType>::type ElementType;

    typedef viennagrid::result_of::const_element_range<MeshType>::type ConstElementRangeType;
    typedef viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementRangeIterator;


    PointType centroid(geometric_dimension);
    PointType axis = get_required_input<point_t>("axis")();
    axis.normalize();

    int rotational_frequency = get_required_input<int>("rotational_frequency")();
    double angle = 2*M_PI/rotational_frequency;

    PointType N[2];
    N[0] = viennagrid::make_point(1,0,0);
    if ( std::abs(viennagrid::inner_prod(axis,N[0])) > 1.0-tol )
      N[0] = viennagrid::make_point(0,1,0);
    N[1] = viennagrid::cross_prod( axis, N[0] );
    N[1].normalize();
    N[0] = viennagrid::cross_prod( N[1], axis );
    N[0].normalize();

    N[1] = rotate(N[0], point_t(geometric_dimension), axis, angle);

    info(1) << "Using rotational frequency " << rotational_frequency << std::endl;
    info(1) << "Angle = " << angle << std::endl;
    info(1) << "Axis = " << axis << std::endl;
    info(1) << "Normal[0] = " << N[0] << std::endl;
    info(1) << "Normal[1] = " << N[1] << std::endl;
    info(1) << "Centroid = " << centroid << std::endl;



    mesh_handle output_mesh = make_data<mesh_handle>();

    std::vector<ElementType> elements_not_copied;
    viennagrid::result_of::element_copy_map<>::type copy_map( output_mesh() );

    ConstElementRangeType cells( input_mesh(), viennagrid::cell_dimension(input_mesh()) );
    for (ConstElementRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      if ( !is_on_plane(*cit, centroid, N[1], tol) )
      {
        if ( is_on_plane(*cit, centroid, N[0], tol) )
          elements_not_copied.push_back(*cit);

        copy_map(*cit);
      }
    }

    std::map<ElementType, ElementType> vertex_map;
    ConstElementRangeType vertices( input_mesh(), 0 );
    for (ConstElementRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      if ( is_on_plane(*vit, centroid, N[0], tol) )
      {
        PointType point = viennagrid::get_point(*vit);
        PointType new_point = rotate(point, centroid, axis, angle);

        vertex_map[*vit] = viennagrid::make_unique_vertex( output_mesh(), new_point, tol );
      }
    }

    for (std::size_t i = 0; i != elements_not_copied.size(); ++i)
    {
      std::vector<ElementType> new_vertices;

      typedef viennagrid::result_of::const_element_range<ElementType>::type ConstBoundaryElementRangeType;
      typedef viennagrid::result_of::iterator<ConstBoundaryElementRangeType>::type ConstBoundaryElementIteratorType;

      ConstBoundaryElementRangeType vertices( elements_not_copied[i], 0 );
      for (ConstBoundaryElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
      {
        std::map<ElementType, ElementType>::const_iterator nvit = vertex_map.find(*vit);

        if (nvit == vertex_map.end())
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED, "Error finding vertex on other plane");

        new_vertices.push_back( (*nvit).second );
      }

      viennagrid::make_element( output_mesh(), elements_not_copied[i].tag(), new_vertices.begin(), new_vertices.end() );
    }




    set_output( "mesh", output_mesh );

    return true;
  }

}
