#ifndef VIENNAMESH_ALGORITHM_CGAL_MESH_HPP
#define VIENNAMESH_ALGORITHM_CGAL_MESH_HPP

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
  namespace cgal
  {
    struct input_mesh
    {
        typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

        typedef CGAL::Mesh_polyhedron_3<K>::type MeshPolyhedron;
        typedef CGAL::Polyhedral_mesh_domain_with_features_3<K, MeshPolyhedron> Mesh_domain;

        MeshPolyhedron polyhedron;

        typedef MeshPolyhedron::Point_3 Point;

        typedef std::vector< viennagrid::static_array<Point, 2> > feature_lines_type;
        feature_lines_type feature_lines;
    };

    struct output_mesh
    {
        typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

        typedef CGAL::Mesh_polyhedron_3<K>::type MeshPolyhedron;
        typedef CGAL::Polyhedral_mesh_domain_with_features_3<K, MeshPolyhedron> Mesh_mesh;

        // Triangulation
        typedef CGAL::Mesh_triangulation_3<Mesh_mesh>::type Tr;
        typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr, Mesh_mesh::Corner_index,Mesh_mesh::Curve_segment_index> C3t3;

        C3t3 tetrahedron_triangulation;
    };



    template<typename MeshT>
    void generic_convert( MeshT const & input, cgal::input_mesh & output)
    {
      output = cgal::input_mesh();

      typedef typename viennagrid::result_of::point<MeshT>::type PointType;

      typedef typename viennagrid::result_of::line<MeshT>::type LineType;

      typedef typename viennagrid::result_of::triangle<MeshT>::type TriangleType;
      typedef typename viennagrid::result_of::const_triangle_range<MeshT>::type TriangleRangeType;
      typedef typename viennagrid::result_of::iterator<TriangleRangeType>::type TriangleRangeIterator;

      typedef typename viennagrid::result_of::const_element_range<MeshT, viennagrid::line_tag>::type LineRangeType;
      typedef typename viennagrid::result_of::iterator<LineRangeType>::type LineRangeIterator;


      typedef cgal::input_mesh::MeshPolyhedron::Point_3 CGALPointType;


      TriangleRangeType triangles = viennagrid::elements<viennagrid::triangle_tag>( input );
      for (TriangleRangeIterator it = triangles.begin(); it != triangles.end(); ++it)
      {
        TriangleType const & tri = *it;

        PointType const & p0 = viennagrid::point( viennagrid::vertices(tri)[0] );
        PointType const & p1 = viennagrid::point( viennagrid::vertices(tri)[1] );
        PointType const & p2 = viennagrid::point( viennagrid::vertices(tri)[2] );

        output.polyhedron.make_triangle( CGALPointType(p0[0], p0[1], p0[2]), CGALPointType(p1[0], p1[1], p1[2]), CGALPointType(p2[0], p2[1], p2[2]) );
      }


      LineRangeType lines = viennagrid::elements<viennagrid::line_tag>( input );

      output.feature_lines.resize( lines.size() );

      unsigned int index = 0;
      for (LineRangeIterator it = lines.begin(); it != lines.end(); ++it, ++index)
      {
        LineType const & line = *it;

        PointType const & p0 = viennagrid::point( viennagrid::vertices(line)[0] );
        PointType const & p1 = viennagrid::point( viennagrid::vertices(line)[1] );

        output.feature_lines[index][0] = CGALPointType(p0[0], p0[1], p0[2]);
        output.feature_lines[index][1] = CGALPointType(p1[0], p1[1], p1[2]);
      }
    }

    inline bool convert(viennagrid::triangular_3d_mesh const & input, cgal::input_mesh & output)
    {
      generic_convert(input, output);
      return true;
    }

    inline bool convert(viennagrid::segmented_mesh<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> const & input, cgal::input_mesh & output)
    {
      generic_convert(input.mesh, output);
      return true;
    }







    template<typename MeshT>
    void generic_convert( cgal::output_mesh const & input, MeshT & output )
    {
      viennagrid::clear(output);

      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;
      typedef typename viennagrid::result_of::cell<MeshT>::type CellType;

      typedef cgal::output_mesh::Tr::Vertex_handle CGALVertexHandleType;
      typedef cgal::output_mesh::C3t3::Cell_iterator  CGALCellIterator;

      std::map<CGALVertexHandleType, VertexHandleType> points;

//       int mesh_faces_counter = 0;
      for (CGALCellIterator it = input.tetrahedron_triangulation.cells_in_complex_begin(); it != input.tetrahedron_triangulation.cells_in_complex_end(); ++it)
      {
        cgal::output_mesh::Tr::Cell t = *it;

        VertexHandleType vgrid_vtx[4];

        for (int i = 0; i < 4; ++i)
        {
          typename std::map<CGALVertexHandleType, VertexHandleType>::iterator pit = points.find( t.vertex(i) );
          if (pit == points.end())
          {
            PointType tmp;
            tmp[0] = t.vertex(i)->point().x();
            tmp[1] = t.vertex(i)->point().y();
            tmp[2] = t.vertex(i)->point().z();

            vgrid_vtx[i] = viennagrid::make_vertex( output, tmp );
            points[ t.vertex(i) ] = vgrid_vtx[i];
          }
          else
            vgrid_vtx[i] = pit->second;
        }

        viennagrid::make_element<CellType>( output, vgrid_vtx, vgrid_vtx+4 );
      }
    }

    inline bool convert(cgal::output_mesh const & input, viennagrid::tetrahedral_3d_mesh & output)
    {
      generic_convert(input, output);
      return true;
    }

    inline bool convert(cgal::output_mesh const & input, viennagrid::thin_tetrahedral_3d_mesh & output)
    {
      generic_convert(input, output);
      return true;
    }

    inline bool convert(cgal::output_mesh const & input, viennagrid::segmented_mesh<viennagrid::thin_tetrahedral_3d_mesh, viennagrid::thin_cell_only_tetrahedral_3d_segmentation> & output)
    {
      generic_convert(input, output.mesh);
      return true;
    }
  }





  template<>
  struct type_information<cgal::input_mesh>
  {
    typedef cgal::input_mesh SelfType;

    static void init()
    {
      typedef viennagrid::triangular_3d_mesh Triangular3DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> SegmentedTriangular3DViennaGridMeshType;

      converter::get().register_conversion<Triangular3DViennaGridMeshType, SelfType>( &cgal::convert );
      converter::get().register_conversion<SegmentedTriangular3DViennaGridMeshType, SelfType>( &cgal::convert );

      type_properties::get().set_property<SelfType>( "is_mesh", "true" );
      type_properties::get().set_property<SelfType>( "geometric_dimension", "3" );
      type_properties::get().set_property<SelfType>( "cell_type", "2-simplex" );
      type_properties::get().set_property<SelfType>( "is_segmented", "true" );
    }

    static string name()
    { return "cgal::input_mesh"; }
  };


  template<>
  struct type_information< cgal::output_mesh >
  {
    typedef cgal::output_mesh SelfType;

    static void init()
    {
      typedef viennagrid::tetrahedral_3d_mesh Tetrahedral3DViennaGridMeshType;
      typedef viennagrid::thin_tetrahedral_3d_mesh ThinTetrahedral3DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::thin_tetrahedral_3d_mesh, viennagrid::thin_cell_only_tetrahedral_3d_segmentation> ThinCellOnlySegmentedTetrahedral3DViennaGridMeshType;

      converter::get().register_conversion<SelfType, Tetrahedral3DViennaGridMeshType>( &cgal::convert );
      converter::get().register_conversion<SelfType, ThinTetrahedral3DViennaGridMeshType>( &cgal::convert );
      converter::get().register_conversion<SelfType, ThinCellOnlySegmentedTetrahedral3DViennaGridMeshType>( &cgal::convert );

      type_properties::get().set_property<SelfType>( "is_mesh", "true" );
      type_properties::get().set_property<SelfType>( "geometric_dimension", "3" );
      type_properties::get().set_property<SelfType>( "cell_type", "3-simplex" );
      type_properties::get().set_property<SelfType>( "is_segmented", "true" );
    }

    static string name()
    { return "cgal::output_mesh"; }
  };


}

#endif
