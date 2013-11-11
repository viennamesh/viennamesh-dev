#ifndef VIENNAMESH_MESH_TETGEN_TETRAHEDRON_HPP
#define VIENNAMESH_MESH_TETGEN_TETRAHEDRON_HPP

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/mesh/segmentation.hpp"
#include "viennamesh/core/convert.hpp"
#include "viennamesh/core/parameter.hpp"


// #define TETLIBRARY
#include "tetgen/tetgen.h"



namespace viennamesh
{
  typedef tetgenio tetgen_tetrahedron_mesh;
}

namespace viennagrid
{
  namespace result_of
  {
    template<>
    struct point<viennamesh::tetgen_tetrahedron_mesh>
    {
      typedef viennagrid::config::point_type_3d type;
    };

    template<>
    struct point<const viennamesh::tetgen_tetrahedron_mesh>
    {
      typedef viennagrid::config::point_type_3d type;
    };
  }
}


namespace viennamesh
{

  template<>
  struct static_init< MeshWrapper<viennamesh::tetgen_tetrahedron_mesh, NoSegmentation> >
  {
    typedef MeshWrapper<viennamesh::tetgen_tetrahedron_mesh, NoSegmentation> SelfT;

    static void init()
    {
      static bool to_init = true;
      if (to_init)
      {
        to_init = false;
        info(10) << "static_init< MeshWrapper<viennamesh::tetgen_tetrahedron_mesh, NoSegmentation> >::init" << std::endl;

        typedef MeshWrapper<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> Triangular3DViennaGridMesh;
        typedef MeshWrapper<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> Tetrahedral3DViennaGridMesh;

        Converter::get().register_conversion<Triangular3DViennaGridMesh, SelfT>( &mesh_convert<Triangular3DViennaGridMesh, SelfT> );
        Converter::get().register_conversion<SelfT, Tetrahedral3DViennaGridMesh>( &mesh_convert<SelfT, Tetrahedral3DViennaGridMesh> );

        TypeProperties::get().set_property<SelfT>( "is_mesh", "true" );
      }
    }
  };





  template<typename PLCT>
  std::vector< std::vector< typename viennagrid::result_of::const_vertex_handle<PLCT>::type > > split_into_polygons( PLCT const & plc )
  {
    typedef typename viennagrid::result_of::const_vertex_range<PLCT>::type ConstVertexRange;
    typedef typename viennagrid::result_of::iterator<ConstVertexRange>::type ConstVertexIterator;

    typedef typename viennagrid::result_of::const_line_range<PLCT>::type ConstLineRange;
    typedef typename viennagrid::result_of::iterator<ConstLineRange>::type ConstLineIterator;

    typedef typename viennagrid::result_of::line<PLCT>::type LineType;
    typedef typename viennagrid::result_of::id<LineType>::type LineIDType;

    typedef typename viennagrid::result_of::vertex<PLCT>::type VertexType;
    typedef typename viennagrid::result_of::id<VertexType>::type VertexIDType;


    typedef typename viennagrid::result_of::const_vertex_handle<PLCT>::type ConstVertexHandle;


    typedef std::vector< std::vector<ConstVertexHandle> > ReturnType;
    ReturnType polygons;



    ConstVertexRange vertices = viennagrid::vertices(plc);
    ConstLineRange lines = viennagrid::lines(plc);

    // first step: all line markers to false
    std::map<LineIDType, bool> line_marker;
    for (ConstLineIterator lit = lines.begin(); lit != lines.end(); ++lit)
    {
//       std::cout << *lit << std::endl;
      line_marker[lit->id()] = false;
    }


    std::vector<ConstLineIterator> loose_lines;

    // second step: find point lines
    for (ConstLineIterator lit = lines.begin(); lit != lines.end(); ++lit)
    {
      if (line_marker[lit->id()]) continue;

      if ( viennagrid::vertices(*lit).handle_at(0) == viennagrid::vertices(*lit).handle_at(1) )
      {
        std::cout << "Found loose point" << std::endl;
        loose_lines.push_back( lit );
        line_marker[lit->id()] = true;
      }
    }

    // third step: calculating the use count of each vertex
    std::map<VertexIDType, int> vertex_used_counter;
    for (ConstVertexIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
//       std::cout << *vit << std::endl;
      vertex_used_counter[vit->id()] = 0;
    }

    for (ConstLineIterator lit = lines.begin(); lit != lines.end(); ++lit)
    {
      if (line_marker[lit->id()]) continue;

      vertex_used_counter[ viennagrid::vertices(*lit)[0].id() ]++;
      vertex_used_counter[ viennagrid::vertices(*lit)[1].id() ]++;
    }

//     for (ConstVertexIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
//     {
//       std::cout << *vit << " " << vertex_used_counter[vit->id()] << std::endl;
//     }

    for (ConstVertexIterator vit = vertices.begin(); vit != vertices.end();)
    {
      if (vertex_used_counter[vit->id()] == 0)
      {
        std::cout << "Some might be wrong?" << std::endl;
        continue;
      }

      if (vertex_used_counter[vit->id()] == 1)
      {
        for (ConstLineIterator lit = lines.begin(); lit != lines.end(); ++lit)
        {
          if (viennagrid::vertices(*lit).handle_at(0) == vit.handle() || viennagrid::vertices(*lit).handle_at(1) == vit.handle())
          {
            std::cout << "Found loose line" << std::endl;
            loose_lines.push_back( lit );

            vertex_used_counter[ viennagrid::vertices(*lit)[0].id() ]--;
            vertex_used_counter[ viennagrid::vertices(*lit)[1].id() ]--;
          }
        }

        vit = vertices.begin();
      }
      else
        ++vit;
    }







    for (ConstVertexIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      if (vertex_used_counter[vit->id()] == 0)
        continue;

      if (vertex_used_counter[vit->id()] != 2)
      {
        std::cout << "Unsupported PLC, point: " << *vit << std::endl;
        return ReturnType();
      }



      std::vector<ConstVertexHandle> polygon;
      polygon.push_back( vit.handle() );

      while (vertex_used_counter[vit->id()] != 0)
      {
        for (ConstLineIterator lit = lines.begin(); lit != lines.end(); ++lit)
        {
          if (line_marker[lit->id()]) continue;

          if (viennagrid::vertices(*lit).handle_at(0) == polygon.back())
          {
            polygon.push_back( viennagrid::vertices(*lit).handle_at(1) );
            vertex_used_counter[ viennagrid::vertices(*lit)[0].id() ]--;
            vertex_used_counter[ viennagrid::vertices(*lit)[1].id() ]--;
            line_marker[lit->id()] = true;

            continue;
          }

          if (viennagrid::vertices(*lit).handle_at(1) == polygon.back())
          {
            polygon.push_back( viennagrid::vertices(*lit).handle_at(0) );
            vertex_used_counter[ viennagrid::vertices(*lit)[0].id() ]--;
            vertex_used_counter[ viennagrid::vertices(*lit)[1].id() ]--;
            line_marker[lit->id()] = true;

            continue;
          }
        }
      }

      polygon.pop_back();
      polygons.push_back(polygon);
    }


    std::cout << "Num polygons: " << polygons.size() << std::endl;


    return polygons;
  }




  template<>
  struct convert_impl<viennagrid::plc_3d_mesh, NoSegmentation, tetgen_tetrahedron_mesh, NoSegmentation >
  {
    typedef viennagrid::plc_3d_mesh vgrid_mesh_type;
    typedef tetgen_tetrahedron_mesh netgen_mesh_type;

    typedef vgrid_mesh_type input_mesh_type;
    typedef NoSegmentation input_segmentation_type;
    typedef netgen_mesh_type output_mesh_type;
    typedef NoSegmentation output_segmentation_type;

    static bool convert( input_mesh_type const & vgrid_mesh, input_segmentation_type const & input_segmentation,
                          output_mesh_type & tetgen_mesh, output_segmentation_type & output_segmentation )
    {
      typedef typename viennagrid::result_of::point<vgrid_mesh_type>::type PointType;

      typedef typename viennagrid::result_of::const_vertex_handle<vgrid_mesh_type>::type ConstVertexHandle;
      typedef typename viennagrid::result_of::cell<vgrid_mesh_type>::type CellType;

      typedef typename viennagrid::result_of::const_vertex_range<vgrid_mesh_type>::type ConstVertexRange;
      typedef typename viennagrid::result_of::iterator<ConstVertexRange>::type ConstVertexIterator;

      typedef typename viennagrid::result_of::const_cell_range<vgrid_mesh_type>::type ConstCellRange;
      typedef typename viennagrid::result_of::iterator<ConstCellRange>::type ConstCellIterator;


      std::map<ConstVertexHandle, int> vertex_handle_to_tetgen_index_map;

      ConstVertexRange vertices(vgrid_mesh);

      tetgen_mesh.firstnumber = 0;
      tetgen_mesh.numberofpoints = vertices.size();
      tetgen_mesh.pointlist = new REAL[ tetgen_mesh.numberofpoints * 3 ];

      int index = 0;
      for (ConstVertexIterator vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
      {

        tetgen_mesh.pointlist[index*3+0] = viennagrid::point(vgrid_mesh, *vit)[0];
        tetgen_mesh.pointlist[index*3+1] = viennagrid::point(vgrid_mesh, *vit)[1];
        tetgen_mesh.pointlist[index*3+2] = viennagrid::point(vgrid_mesh, *vit)[2];

        vertex_handle_to_tetgen_index_map[ vit.handle() ] = index;
      }


      ConstCellRange cells(vgrid_mesh);

      tetgen_mesh.numberoffacets = cells.size();
      tetgen_mesh.facetlist = new tetgenio::facet[tetgen_mesh.numberoffacets];
      tetgen_mesh.facetmarkerlist = new int[tetgen_mesh.numberoffacets];

      index = 0;
      for (ConstCellIterator cit = cells.begin(); cit != cells.end(); ++cit, ++index)
      {
        typedef typename viennagrid::result_of::const_line_range<CellType>::type ConstLineOnCellRange;
        typedef typename viennagrid::result_of::iterator<ConstLineOnCellRange>::type ConstLineOnCellIterator;


        std::vector< std::vector<ConstVertexHandle> > polygons = split_into_polygons(*cit);

        if (polygons.empty())
          return false;

        tetgenio::facet & facet = tetgen_mesh.facetlist[index];

        facet.numberofpolygons = polygons.size();
        facet.polygonlist = new tetgenio::polygon[ polygons.size() ];
        facet.numberofholes = 0;
        facet.holelist = 0;

        std::cout << "Num Polygons: " << polygons.size() << std::endl;

        int polygon_index = 0;
        for (int polygon_index = 0; polygon_index != polygons.size(); ++polygon_index)
        {
          std::cout << "  Num Vertices: " << polygons[polygon_index].size() << std::endl;

          tetgenio::polygon & polygon = facet.polygonlist[polygon_index];
          polygon.numberofvertices = polygons[polygon_index].size();
          polygon.vertexlist = new int[ polygons[polygon_index].size() ];

          for (int vertex_index = 0; vertex_index != polygons[polygon_index].size(); ++vertex_index)
          {
            std::cout << vertex_handle_to_tetgen_index_map[ polygons[polygon_index][vertex_index] ] << std::endl;
            polygon.vertexlist[vertex_index] = vertex_handle_to_tetgen_index_map[ polygons[polygon_index][vertex_index] ];
          }
        }
      }

      return true;
    }
  };







  template<typename triangular_3d_segmentation_type>
  struct convert_impl<viennagrid::triangular_3d_mesh, triangular_3d_segmentation_type, tetgen_tetrahedron_mesh, NoSegmentation >
  {
    typedef viennagrid::triangular_3d_mesh vgrid_mesh_type;
    typedef tetgen_tetrahedron_mesh netgen_mesh_type;

    typedef vgrid_mesh_type input_mesh_type;
    typedef triangular_3d_segmentation_type input_segmentation_type;
    typedef netgen_mesh_type output_mesh_type;
    typedef NoSegmentation output_segmentation_type;

    static bool convert( input_mesh_type const & vgrid_mesh, input_segmentation_type const & input_segmentation,
                          output_mesh_type & tetgen_mesh, output_segmentation_type & output_segmentation )
    {
      typedef typename viennagrid::result_of::point<vgrid_mesh_type>::type point_type;

      typedef typename viennagrid::result_of::element<vgrid_mesh_type, viennagrid::vertex_tag>::type vertex_type;
      typedef typename viennagrid::result_of::const_handle<vgrid_mesh_type, viennagrid::vertex_tag>::type vertex_const_handle_type;
      typedef typename viennagrid::result_of::element<vgrid_mesh_type, viennagrid::triangle_tag>::type triangle_type;

      typedef typename viennagrid::result_of::const_element_range<vgrid_mesh_type, viennagrid::vertex_tag>::type vertex_range_type;
      typedef typename viennagrid::result_of::iterator<vertex_range_type>::type vertex_range_iterator;

      typedef typename viennagrid::result_of::const_element_range<vgrid_mesh_type, viennagrid::triangle_tag>::type triangle_range_type;
      typedef typename viennagrid::result_of::iterator<triangle_range_type>::type triangle_range_iterator;

//         typedef typename triangular_3d_segmentation_type::segment_type SegmentType;


      std::map<vertex_const_handle_type, int> vertex_handle_to_tetgen_index_map;

      vertex_range_type vertices(vgrid_mesh);

      tetgen_mesh.firstnumber = 0;
      tetgen_mesh.numberofpoints = vertices.size();
      tetgen_mesh.pointlist = new REAL[ tetgen_mesh.numberofpoints * 3 ];

      int index = 0;
      for (vertex_range_iterator vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
      {

        tetgen_mesh.pointlist[index*3+0] = viennagrid::point(vgrid_mesh, *vit)[0];
        tetgen_mesh.pointlist[index*3+1] = viennagrid::point(vgrid_mesh, *vit)[1];
        tetgen_mesh.pointlist[index*3+2] = viennagrid::point(vgrid_mesh, *vit)[2];

        vertex_handle_to_tetgen_index_map[ vit.handle() ] = index;
      }


      triangle_range_type triangles(vgrid_mesh);

      tetgen_mesh.numberoffacets = triangles.size();
      tetgen_mesh.facetlist = new tetgenio::facet[tetgen_mesh.numberoffacets];
      tetgen_mesh.facetmarkerlist = new int[tetgen_mesh.numberoffacets];

      index = 0;
      for (triangle_range_iterator tit = triangles.begin(); tit != triangles.end(); ++tit, ++index)
      {
        tetgenio::facet & facet = tetgen_mesh.facetlist[index];

        facet.numberofpolygons = 1;
        facet.polygonlist = new tetgenio::polygon[1];
        facet.numberofholes = 0;
        facet.holelist = 0;

        tetgenio::polygon & polygon = facet.polygonlist[0];
        polygon.numberofvertices = 3;
        polygon.vertexlist = new int[3];
        polygon.vertexlist[0] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*tit).handle_at(0) ];
        polygon.vertexlist[1] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*tit).handle_at(1) ];
        polygon.vertexlist[2] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*tit).handle_at(2) ];

        tetgen_mesh.facetmarkerlist[index] = 0; // TODO Was tut das?????
      }

      return true;
    }
  };





  template<>
  struct convert_impl<tetgen_tetrahedron_mesh, NoSegmentation, viennagrid::tetrahedral_3d_mesh, NoSegmentation>
  {
    typedef tetgen_tetrahedron_mesh netgen_mesh_type;
    typedef viennagrid::tetrahedral_3d_mesh vgrid_mesh_type;


    typedef netgen_mesh_type input_mesh_type;
    typedef NoSegmentation input_segmentation_type;
    typedef vgrid_mesh_type output_mesh_type;
    typedef NoSegmentation output_segmentation_type;
//         typedef viennagrid::tetrahedral_3d_segmentation output_segmentation_type;


    static bool convert( input_mesh_type const & tetgen_mesh, input_segmentation_type const & input_segmentation,
                          output_mesh_type & vgrid_mesh, output_segmentation_type & output_segmentation )
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

//         typedef typename output_segmentation_type::segment_type SegmentType;

      std::vector<vertex_handle_type> vertex_handles(tetgen_mesh.numberofpoints);

      for (int i = 0; i < tetgen_mesh.numberofpoints; ++i)
      {
        point_type point;

        point[0] = tetgen_mesh.pointlist[3*i+0];
        point[1] = tetgen_mesh.pointlist[3*i+1];
        point[2] = tetgen_mesh.pointlist[3*i+2];

        vertex_handles[i] = viennagrid::make_vertex( vgrid_mesh, point );
      }


      for (int i = 0; i < tetgen_mesh.numberoftetrahedra; ++i)
      {
        viennagrid::make_tetrahedron(
          vgrid_mesh,
          vertex_handles[ tetgen_mesh.tetrahedronlist[4*i+0] ],
          vertex_handles[ tetgen_mesh.tetrahedronlist[4*i+1] ],
          vertex_handles[ tetgen_mesh.tetrahedronlist[4*i+2] ],
          vertex_handles[ tetgen_mesh.tetrahedronlist[4*i+3] ]
        );
      }

      return true;
    }
  };




  template<>
  struct convert_impl<tetgen_tetrahedron_mesh, NoSegmentation, viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>
  {
    typedef tetgen_tetrahedron_mesh netgen_mesh_type;
    typedef viennagrid::tetrahedral_3d_mesh vgrid_mesh_type;


    typedef netgen_mesh_type input_mesh_type;
    typedef NoSegmentation input_segmentation_type;
    typedef vgrid_mesh_type output_mesh_type;
    typedef viennagrid::tetrahedral_3d_segmentation output_segmentation_type;


    static bool convert( input_mesh_type const & tetgen_mesh, input_segmentation_type const & input_segmentation,
                          output_mesh_type & vgrid_mesh, output_segmentation_type & output_segmentation )
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

//         typedef typename output_segmentation_type::segment_type SegmentType;

      std::vector<vertex_handle_type> vertex_handles(tetgen_mesh.numberofpoints);

      for (int i = 0; i < tetgen_mesh.numberofpoints; ++i)
      {
        point_type point;

        point[0] = tetgen_mesh.pointlist[3*i+0];
        point[1] = tetgen_mesh.pointlist[3*i+1];
        point[2] = tetgen_mesh.pointlist[3*i+2];

        vertex_handles[i] = viennagrid::make_vertex( vgrid_mesh, point );
      }


      for (int i = 0; i < tetgen_mesh.numberoftetrahedra; ++i)
      {
        if (tetgen_mesh.numberoftetrahedronattributes == 0)
        {
          viennagrid::make_tetrahedron(
            vgrid_mesh,
            vertex_handles[ tetgen_mesh.tetrahedronlist[4*i+0] ],
            vertex_handles[ tetgen_mesh.tetrahedronlist[4*i+1] ],
            vertex_handles[ tetgen_mesh.tetrahedronlist[4*i+2] ],
            vertex_handles[ tetgen_mesh.tetrahedronlist[4*i+3] ]
          );
        }
        else
        {
          output_segmentation_type::segment_id_type segment_id = tetgen_mesh.tetrahedronattributelist[i];

          viennagrid::make_tetrahedron(
            output_segmentation.get_make_segment(segment_id),
            vertex_handles[ tetgen_mesh.tetrahedronlist[4*i+0] ],
            vertex_handles[ tetgen_mesh.tetrahedronlist[4*i+1] ],
            vertex_handles[ tetgen_mesh.tetrahedronlist[4*i+2] ],
            vertex_handles[ tetgen_mesh.tetrahedronlist[4*i+3] ]
          );
        }
      }

      return true;
    }
  };
}

#endif
