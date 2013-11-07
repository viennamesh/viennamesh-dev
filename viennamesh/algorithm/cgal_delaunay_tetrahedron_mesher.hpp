#ifndef VIENNAMESH_ALGORITHM_CGAL_DELAUNEY_TETRAHEDRON_MESHER_HPP
#define VIENNAMESH_ALGORITHM_CGAL_DELAUNEY_TETRAHEDRON_MESHER_HPP

#include "viennamesh/core/dynamic_algorithm.hpp"
#include "viennamesh/mesh/cgal_delaunay_tetrahedron.hpp"

namespace viennamesh
{
  struct cgal_delaunay_tetrahedron_tag {};


  namespace result_of
  {
    template<>
    struct native_input_mesh_wrapper<cgal_delaunay_tetrahedron_tag>
    {
      typedef ParameterWrapper< MeshWrapper<cgal_mesh_polyhedron_mesh, NoSegmentation> > type;
    };

    template<>
    struct native_output_mesh_wrapper<cgal_delaunay_tetrahedron_tag>
    {
      typedef ParameterWrapper< MeshWrapper<cgal_delaunay_tetrahedron_mesh, NoSegmentation> > type;
    };




    template<>
    struct works_in_place<cgal_delaunay_tetrahedron_tag>
    {
      static const bool value = false;
    };

    template<>
    struct algorithm_info< cgal_delaunay_tetrahedron_tag >
    {
      static const std::string name() { return "CGAL Triangle Hull to Tetrahedron Mesh mesher"; }
    };


    template<typename mesh_type>
    struct best_matching_native_input_mesh<cgal_delaunay_tetrahedron_tag, mesh_type>
    {
      typedef cgal_mesh_polyhedron_mesh type;
    };

    template<typename mesh_type>
    struct best_matching_native_output_mesh<cgal_delaunay_tetrahedron_tag, mesh_type>
    {
      typedef cgal_delaunay_tetrahedron_mesh type;
    };

    template<typename segmentation_type>
    struct best_matching_native_input_segmentation<cgal_delaunay_tetrahedron_tag, segmentation_type>
    {
      typedef NoSegmentation type;
    };

    template<typename segmentation_type>
    struct best_matching_native_output_segmentation<cgal_delaunay_tetrahedron_tag, segmentation_type>
    {
      typedef NoSegmentation type;
    };
  }


//   template<typename FT_, typename Point, typename Index_>
//   struct cgal_sizing_field
//   {
//   public:
//     typedef FT_ FT;
//     typedef Point Point_3;
//     typedef Index_ Index;
//
//     cgal_sizing_field(ScalarField3DFunction field_) : field(field_) {}
//
//     FT operator() ( Point_3 const & p, int dimension, Index const & index ) const
//     {
//         assert(field != 0);
//         return field( p.x(), p.y(), p.z() );
//     }
//
//   private:
//     ScalarField3DFunction field;
//   };





  template<typename mesh_cirteria>
  struct get_triangulation;

  template<typename Tr>
  struct get_triangulation< CGAL::Mesh_criteria_3<Tr> >
  {
    typedef Tr type;
  };




  template<typename cgal_mesh_criteria_type, typename index_type>
  cgal_mesh_criteria_type * get_cirteria( ConstParameterSet const & parameters )
  {
    ConstParameterHandle cell_size = parameters.get( "cell_size" );
//     parameters.get("cell_size");

    double cell_radius_edge_ratio = 0;
    parameters.copy_if_present( "cell_radius_edge_ratio", cell_radius_edge_ratio );
//     parameters.copy_scalar("cell_radius_edge_ratio", cell_radius_edge_ratio);

    double facet_angle = 0;
//     parameters.copy_scalar("facet_angle", facet_angle);
    parameters.copy_if_present( "facet_angle", facet_angle );

    if ( !cell_size )
    {
      return new cgal_mesh_criteria_type(
//                 CGAL::parameters::edge_size = std::numeric_limits<double>::max(),
          CGAL::parameters::cell_radius_edge_ratio = cell_radius_edge_ratio,
          CGAL::parameters::facet_angle = facet_angle,
          CGAL::parameters::facet_topology = CGAL::FACET_VERTICES_ON_SAME_SURFACE_PATCH_WITH_ADJACENCY_CHECK
      );
    }

//     if ( cell_size->is_scalar() )
    ConstDoubleParameterHandle cell_size_double = cell_size->get_converted<double>();
    if ( cell_size_double )
    {
      return new cgal_mesh_criteria_type(
          CGAL::parameters::edge_size = cell_size_double->value,
          CGAL::parameters::cell_radius_edge_ratio = cell_radius_edge_ratio,
          CGAL::parameters::facet_angle = facet_angle,
          CGAL::parameters::facet_size = cell_size_double->value,
          CGAL::parameters::facet_topology = CGAL::FACET_VERTICES_ON_SAME_SURFACE_PATCH_WITH_ADJACENCY_CHECK,
          CGAL::parameters::cell_size = cell_size_double->value
      );
    }

//     if ( cell_size->template cast<ScalarField3DFunction>() )
//     {
//
// //     typedef typename settings_type::field_parameter_type::field_functor_ptr_type field_functor_ptr_type;
//
//       typedef typename get_triangulation<cgal_mesh_criteria_type>::type::Triangulation_3 Triangulation;
//       typedef typename cgal_mesh_criteria_type::Edge_criteria::FT FT;
//       typedef typename Triangulation::Point Point;
//
//       typedef typename cgal_mesh_criteria_type::Edge_criteria::Point_3 EdgePoint;
//
//       typedef index_type Index;
//
//       cgal_sizing_field<
//           FT,
//           Point,
//           Index
//           > cell_and_facet_sizing_field( cell_size->template cast<ScalarField3DFunction>()->get() );
//
//       cgal_sizing_field<
//           FT,
//           EdgePoint,
//           Index
//           > edge_sizing_field( cell_size->template cast<ScalarField3DFunction>()->get() );
//
//       return new cgal_mesh_criteria_type(
//           CGAL::parameters::edge_size = edge_sizing_field,
//           CGAL::parameters::cell_radius_edge_ratio = cell_radius_edge_ratio,
//           CGAL::parameters::facet_angle = facet_angle,
//           CGAL::parameters::facet_size = cell_and_facet_sizing_field,
//           CGAL::parameters::facet_topology = CGAL::FACET_VERTICES_ON_SAME_SURFACE_PATCH_WITH_ADJACENCY_CHECK,
//           CGAL::parameters::cell_size = cell_and_facet_sizing_field
//       );
//     }

    return new cgal_mesh_criteria_type();
  }





  template<>
  struct native_algorithm_impl<cgal_delaunay_tetrahedron_tag>
  {
    typedef cgal_delaunay_tetrahedron_tag algorithm_tag;

    template<typename native_input_mesh_type, typename native_output_mesh_type>
    static algorithm_feedback run( native_input_mesh_type const & native_input_mesh, native_output_mesh_type & native_output_mesh, ConstParameterSet const & parameters )
    {
      algorithm_feedback feedback( result_of::algorithm_info<algorithm_tag>::name() );
      typename native_output_mesh_type::Mesh_mesh mesh_mesh( native_input_mesh.polyhedron );

      typename native_input_mesh_type::feature_lines_type feature_lines(native_input_mesh.feature_lines);
      mesh_mesh.add_features( feature_lines.begin(), feature_lines.end() );


      typedef CGAL::Mesh_criteria_3<typename native_output_mesh_type::Tr> mesh_criteria_type;
      mesh_criteria_type * criteria = get_cirteria<mesh_criteria_type, typename native_input_mesh_type::Mesh_domain::Index>(parameters);


      native_output_mesh.tetrahedron_triangulation = CGAL::make_mesh_3<typename native_output_mesh_type::C3t3>(mesh_mesh, *criteria);

      delete criteria;

      feedback.set_success();
      return feedback;
    }

  };

}

#endif
