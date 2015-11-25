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

#include "hyperplane_clip.hpp"
#include "viennagrid/algorithm/refine.hpp"

namespace viennamesh
{
  template<typename PointT, typename NumericConfigT>
  bool on_positive_hyperplane_side( PointT const & hyperplane_point, PointT const & hyperplane_normal,
                                    PointT const & to_test,
                                    NumericConfigT numeric_config)
  {
    return viennagrid::inner_prod( hyperplane_normal, to_test-hyperplane_point ) >
      viennagrid::detail::relative_tolerance(numeric_config, viennagrid::norm_2(to_test-hyperplane_point));
  }

  template<typename PointT, typename NumericConfigT>
  struct on_positive_hyperplane_side_functor
  {
    typedef bool result_type;
    typedef typename viennagrid::result_of::coord<PointT>::type ScalarType;

    on_positive_hyperplane_side_functor(PointT const & hyperplane_point_, PointT const & hyperplane_normal_,
                                        NumericConfigT numeric_config_) :
        hyperplane_point(hyperplane_point_),
        hyperplane_normal(hyperplane_normal_),
        numeric_config(numeric_config_) {}

    template<typename ElementT>
    bool operator()(ElementT const & element) const
    {
      PointT centroid = viennagrid::centroid(element);
      bool tmp = on_positive_hyperplane_side(hyperplane_point, hyperplane_normal, centroid, numeric_config);

//       std::cout << "On positive side: " << std::boolalpha << tmp << std::endl;

      return tmp;
    }

    PointT hyperplane_point;
    PointT hyperplane_normal;
    NumericConfigT numeric_config;
  };


  hyperplane_clip::hyperplane_clip() {}
  std::string hyperplane_clip::name() { return "hyperplane_clip"; }


  bool hyperplane_clip::run(viennamesh::algorithm_handle &)
  {
    point_handle input_hyperplane_point = get_required_input<point_handle>("hyperplane_point");
    point_handle input_hyperplane_normal = get_required_input<point_handle>("hyperplane_normal");

    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");

    int point_dimension = viennagrid::geometric_dimension( input_mesh() );

    point hyperplane_point = input_hyperplane_point();
    point hyperplane_normal = input_hyperplane_normal();

    if ( hyperplane_point.size() != hyperplane_normal.size() )
      return false;

    if ( (point_dimension != static_cast<int>(hyperplane_point.size())) ||
         (point_dimension != static_cast<int>(hyperplane_normal.size())) )
      return false;

//     convert( input_hyperplane_point(), hyperplane_point );
//     convert( input_hyperplane_normal(), hyperplane_normal );



    info(1) << "Hyperplane point: " << hyperplane_point << std::endl;
    info(1) << "Hyperplane normal: " << hyperplane_normal << std::endl;

    mesh_handle tmp = make_data<mesh_handle>();
    mesh_handle output_mesh = make_data<mesh_handle>();

    viennagrid::hyperplane_refine(input_mesh(), hyperplane_point, hyperplane_normal, 1e-8, tmp() );
    viennagrid::copy( tmp(), output_mesh(),
                      on_positive_hyperplane_side_functor<point, double>(hyperplane_point, -hyperplane_normal, 1e-8) );

    set_output( "mesh", output_mesh );

    return true;
  }


}
