#ifndef VIENNAMESH_MESH_CGAL_DELAUNAY_TETRAHEDRON_HPP
#define VIENNAMESH_MESH_CGAL_DELAUNAY_TETRAHEDRON_HPP

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"

#include "viennamesh/core/convert.hpp"
#include "viennamesh/core/parameter.hpp"
#include "viennamesh/core/basic_parameters.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Projection_traits_xy_3.h>

#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <CGAL/Mesh_polyhedron_3.h>

#include <CGAL/Polyhedral_mesh_domain_3.h>
#include <CGAL/Polyhedron_copy_3.h>
#include <CGAL/Polyhedral_mesh_domain_with_features_3.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/refine_mesh_3.h>


namespace viennamesh
{
  struct cgal_mesh_polyhedron_mesh
  {
      typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

      typedef CGAL::Mesh_polyhedron_3<K>::type MeshPolyhedron;
      typedef CGAL::Polyhedral_mesh_domain_with_features_3<K, MeshPolyhedron> Mesh_domain;

      MeshPolyhedron polyhedron;

      typedef MeshPolyhedron::Point_3 Point;

      typedef std::vector< viennagrid::static_array<Point, 2> > feature_lines_type;
      feature_lines_type feature_lines;
  };

  struct cgal_delaunay_tetrahedron_mesh
  {
      typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

      typedef CGAL::Mesh_polyhedron_3<K>::type MeshPolyhedron;
      typedef CGAL::Polyhedral_mesh_domain_with_features_3<K, MeshPolyhedron> Mesh_mesh;

      // Triangulation
      typedef CGAL::Mesh_triangulation_3<Mesh_mesh>::type Tr;
      typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr, Mesh_mesh::Corner_index,Mesh_mesh::Curve_segment_index> C3t3;

      C3t3 tetrahedron_triangulation;;
  };


//   namespace result_of
//   {
//     template<typename SegmentationT>
//     struct best_matching_viennagrid_mesh<cgal_delaunay_tetrahedron_mesh, SegmentationT>
//     {
//       typedef viennagrid::tetrahedral_3d_mesh type;
//     };
//
//     template<typename SegmentationT>
//     struct best_matching_viennagrid_segmentation<cgal_delaunay_tetrahedron_mesh, SegmentationT>
//     {
//       typedef NoSegmentation type;
//     };
//   }
}

namespace viennagrid
{
  namespace result_of
  {
    template<>
    struct point<viennamesh::cgal_mesh_polyhedron_mesh>
    {
        typedef viennagrid::config::point_type_3d type;
    };

    template<>
    struct point<const viennamesh::cgal_mesh_polyhedron_mesh>
    {
        typedef viennagrid::config::point_type_3d type;
    };

    template<>
    struct point<viennamesh::cgal_delaunay_tetrahedron_mesh>
    {
        typedef viennagrid::config::point_type_3d type;
    };

    template<>
    struct point<const viennamesh::cgal_delaunay_tetrahedron_mesh>
    {
        typedef viennagrid::config::point_type_3d type;
    };
  }
}


namespace viennamesh
{
  template<>
  struct static_init< MeshWrapper<viennamesh::cgal_mesh_polyhedron_mesh, NoSegmentation> >
  {
    typedef MeshWrapper<viennamesh::cgal_mesh_polyhedron_mesh, NoSegmentation> SelfT;

    static void init()
    {
      static bool to_init = true;
      if (to_init)
      {
        to_init = false;
        info(10) << "static_init< MeshWrapper<viennamesh::cgal_mesh_polyhedron_mesh, NoSegmentation> >::init" << std::endl;

        typedef MeshWrapper<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> Triangular3DViennaGridMesh;

        Converter::get().register_conversion<Triangular3DViennaGridMesh, SelfT>( &mesh_convert<Triangular3DViennaGridMesh, SelfT> );

        TypeProperties::get().set_property<SelfT>( "is_mesh", "true" );
      }
    }
  };



  template<>
  struct static_init< MeshWrapper<viennamesh::cgal_delaunay_tetrahedron_mesh, NoSegmentation> >
  {
    typedef MeshWrapper<viennamesh::cgal_delaunay_tetrahedron_mesh, NoSegmentation> SelfT;

    static void init()
    {
      static bool to_init = true;
      if (to_init)
      {
        to_init = false;
        info(10) << "static_init< MeshWrapper<viennamesh::cgal_delaunay_tetrahedron_mesh, NoSegmentation> >::init" << std::endl;

        typedef MeshWrapper<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> Tetrahedral3DViennaGridMesh;

        Converter::get().register_conversion<SelfT, Tetrahedral3DViennaGridMesh>( &mesh_convert<SelfT, Tetrahedral3DViennaGridMesh> );

        TypeProperties::get().set_property<SelfT>( "is_mesh", "true" );
      }
    }
  };




  template<typename InputSegmentationT, typename OutputSegmentationT>
  struct convert_impl<viennagrid::triangular_3d_mesh, InputSegmentationT, cgal_mesh_polyhedron_mesh, OutputSegmentationT >
  {
    typedef viennagrid::triangular_3d_mesh vgrid_mesh_type;
    typedef cgal_mesh_polyhedron_mesh cgal_mesh_type;


    typedef vgrid_mesh_type input_mesh_type;
    typedef InputSegmentationT input_segmentation_type;
    typedef cgal_mesh_type output_mesh_type;
    typedef OutputSegmentationT output_segmentation_type;

    static bool convert( input_mesh_type const & vgrid_mesh, input_segmentation_type const &, output_mesh_type & cgal_mesh, output_segmentation_type & )
    {
      typedef viennagrid::result_of::point<vgrid_mesh_type>::type point_type;

      typedef viennagrid::result_of::element<vgrid_mesh_type, viennagrid::vertex_tag>::type vertex_type;
      typedef viennagrid::result_of::const_handle<vgrid_mesh_type, viennagrid::vertex_tag>::type vertex_const_handle_type;

      typedef viennagrid::result_of::element<vgrid_mesh_type, viennagrid::line_tag>::type line_type;
      typedef viennagrid::result_of::handle<vgrid_mesh_type, viennagrid::line_tag>::type line_handle_type;

      typedef viennagrid::result_of::element<vgrid_mesh_type, viennagrid::triangle_tag>::type triangle_type;
      typedef viennagrid::result_of::handle<vgrid_mesh_type, viennagrid::triangle_tag>::type triangle_handle_type;


      typedef viennagrid::result_of::const_element_range<vgrid_mesh_type, viennagrid::triangle_tag>::type triangle_range_type;
      typedef viennagrid::result_of::iterator<triangle_range_type>::type triangle_range_iterator;

      typedef viennagrid::result_of::const_element_range<vgrid_mesh_type, viennagrid::line_tag>::type line_range_type;
      typedef viennagrid::result_of::iterator<line_range_type>::type line_range_iterator;


      typedef cgal_mesh_type::MeshPolyhedron::Point_3 Point;


      triangle_range_type triangles = viennagrid::elements<viennagrid::triangle_tag>( vgrid_mesh );
      for (triangle_range_iterator it = triangles.begin(); it != triangles.end(); ++it)
      {
        triangle_type const & tri = *it;

        point_type const & p0 = viennagrid::point( vgrid_mesh, viennagrid::elements<viennagrid::vertex_tag>(tri)[0] );
        point_type const & p1 = viennagrid::point( vgrid_mesh, viennagrid::elements<viennagrid::vertex_tag>(tri)[1] );
        point_type const & p2 = viennagrid::point( vgrid_mesh, viennagrid::elements<viennagrid::vertex_tag>(tri)[2] );

        cgal_mesh.polyhedron.make_triangle( Point(p0[0], p0[1], p0[2]), Point(p1[0], p1[1], p1[2]), Point(p2[0], p2[1], p2[2]) );
      }


      line_range_type lines = viennagrid::elements<viennagrid::line_tag>( vgrid_mesh );

      cgal_mesh.feature_lines.resize( lines.size() );

      unsigned int index = 0;
      for (line_range_iterator it = lines.begin(); it != lines.end(); ++it, ++index)
      {
        line_type const & line = *it;

        point_type const & p0 = viennagrid::point( vgrid_mesh, viennagrid::elements<viennagrid::vertex_tag>(line)[0] );
        point_type const & p1 = viennagrid::point( vgrid_mesh, viennagrid::elements<viennagrid::vertex_tag>(line)[1] );

        cgal_mesh.feature_lines[index][0] = Point(p0[0], p0[1], p0[2]);
        cgal_mesh.feature_lines[index][1] = Point(p1[0], p1[1], p1[2]);
      }

      return true;
    }
  };





  template<typename InputSegmentationT, typename OutputSegmentationT>
  struct convert_impl<cgal_delaunay_tetrahedron_mesh, InputSegmentationT, viennagrid::tetrahedral_3d_mesh, OutputSegmentationT >
  {
    typedef cgal_delaunay_tetrahedron_mesh cgal_mesh_type;
    typedef viennagrid::tetrahedral_3d_mesh vgrid_mesh_type;


    typedef cgal_mesh_type input_mesh_type;
    typedef InputSegmentationT input_segmentation_type;
    typedef vgrid_mesh_type output_mesh_type;
    typedef OutputSegmentationT output_segmentation_type;


    static bool convert( input_mesh_type const & cgal_mesh, input_segmentation_type const &, output_mesh_type & vgrid_mesh, output_segmentation_type & )
    {
      typedef viennagrid::result_of::point<vgrid_mesh_type>::type point_type;

      typedef viennagrid::result_of::element<vgrid_mesh_type, viennagrid::vertex_tag>::type vertex_type;
      typedef viennagrid::result_of::handle<vgrid_mesh_type, viennagrid::vertex_tag>::type vertex_handle_type;

      typedef viennagrid::result_of::element<vgrid_mesh_type, viennagrid::line_tag>::type line_type;
      typedef viennagrid::result_of::handle<vgrid_mesh_type, viennagrid::line_tag>::type line_handle_type;

      typedef viennagrid::result_of::element<vgrid_mesh_type, viennagrid::triangle_tag>::type triangle_type;
      typedef viennagrid::result_of::handle<vgrid_mesh_type, viennagrid::triangle_tag>::type triangle_handle_type;

      typedef viennagrid::result_of::element<vgrid_mesh_type, viennagrid::tetrahedron_tag>::type tetrahedron_type;
      typedef viennagrid::result_of::handle<vgrid_mesh_type, viennagrid::tetrahedron_tag>::type tetrahedron_handle_type;



      typedef cgal_mesh_type::Tr::Vertex_handle Vertex_handle;
      typedef cgal_mesh_type::C3t3::Cell_iterator  Cell_iterator;

      std::map<Vertex_handle, vertex_handle_type> points;

      int mesh_faces_counter = 0;
      for (Cell_iterator it = cgal_mesh.tetrahedron_triangulation.cells_in_complex_begin(); it != cgal_mesh.tetrahedron_triangulation.cells_in_complex_end(); ++it)
      {
        cgal_mesh_type::Tr::Cell t = *it;

        vertex_handle_type vgrid_vtx[4];

        for (int i = 0; i < 4; ++i)
        {
          std::map<Vertex_handle, vertex_handle_type>::iterator pit = points.find( t.vertex(i) );
          if (pit == points.end())
          {
            point_type tmp;
            tmp[0] = t.vertex(i)->point().x();
            tmp[1] = t.vertex(i)->point().y();
            tmp[2] = t.vertex(i)->point().z();

            vgrid_vtx[i] = viennagrid::make_vertex( vgrid_mesh, tmp );
            points[ t.vertex(i) ] = vgrid_vtx[i];
          }
          else
            vgrid_vtx[i] = pit->second;
        }

        viennagrid::make_element<tetrahedron_type>( vgrid_mesh, vgrid_vtx, vgrid_vtx+4 );
      }

      return true;
    }
  };






}

#endif
