#ifndef VIENNAMESH_UTILS_PLC_TOOLS_HPP
#define VIENNAMESH_UTILS_PLC_TOOLS_HPP

#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/algorithm/geometry.hpp"

namespace viennamesh
{



//   template<typename PLCT>
//   bool split_plc_into_trivials( PLCT const & plc,
//                                 std::vector<typename viennagrid::result_of::const_vertex_handle<PLCT>::type> & loose_vertices,
//                                 std::vector<std::pair<typename viennagrid::result_of::const_vertex_handle<PLCT>::type, typename viennagrid::result_of::const_vertex_handle<PLCT>::type> > & loose_lines,
//                                 std::vector< std::vector<typename viennagrid::result_of::const_vertex_handle<PLCT>::type> > & polygons
//                               )
//   {
//     typedef typename viennagrid::result_of::point<PLCT>::type PointType;
//     typedef viennagrid::config::point_type_2d Point2DType;
//
//     typedef typename viennagrid::result_of::const_vertex_range<PLCT>::type ConstVertexRangeType;
//     typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;
//
//     typedef typename viennagrid::result_of::const_vertex_handle<PLCT>::type ConstVertexHandleType;
//
//     typedef typename viennagrid::result_of::const_line_range<PLCT>::type ConstLineRangeType;
//     typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineIteratorType;
//
//     typedef typename viennagrid::result_of::line<PLCT>::type LineType;
//     typedef typename viennagrid::result_of::id<LineType>::type LineIDType;
//
//     typedef typename viennagrid::result_of::vertex<PLCT>::type VertexType;
//     typedef typename viennagrid::result_of::id<VertexType>::type VertexIDType;
//
//     loose_vertices.clear();
//     loose_lines.clear();
//     polygons.clear();
//
//
//
//     ConstVertexRangeType vertices(plc);
//
//
//
//     std::map<VertexIDType, int> vertex_index_map;
//     std::vector<ConstVertexHandleType> vertex_handles(vertices.size());
//
//     std::vector<PointType> points(vertices.size());
//     std::vector<int> vertex_use_count( vertices.size(), 0 );
//
//     typedef std::list< std::pair<int, int> > LinesContainerType;
//     typedef typename LinesContainerType::iterator LinesIteratorType;
//     LinesContainerType lines;
//
//     {
//       ConstLineRangeType plc_lines(plc);
//       for (ConstLineIteratorType lit = plc_lines.begin(); lit != plc_lines.end(); ++lit)
//         lines.push_back( std::make_pair( vertex_index_map[viennagrid::vertices(*lit)[0].id()],
//                                          vertex_index_map[viennagrid::vertices(*lit)[1].id()] ) );
//     }
//
// //     std::vector<bool> line_flag( lines.size(), true );
//
//
//     {
//       unsigned int vertex_index = 0;
//       for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++vertex_index)
//       {
//         points[vertex_index] = viennagrid::point(*vit);
//         vertex_index_map[ vit->id() ] = vertex_index;
//         vertex_handles[vertex_index] = vit.handle();
//       }
//     }
//
//     viennagrid::static_array<PointType, 2> projection_matrix;
//     PointType center;
//
//     viennagrid::projection_matrix( points.begin(), points.end(), 1e-6, center, projection_matrix.begin() );
//
//     std::vector<Point2DType> points_2d(points.size());
//     viennagrid::project( points.begin(), points.end(), points_2d.begin(), center, projection_matrix.begin(), projection_matrix.end() );
//
//
//     // determine vertex use count
//     for (LinesIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
//     {
//       ++vertex_use_count[ lit->first ];
//       ++vertex_use_count[ lit->second ];
//     }
//
//     // search for loose vertices
//     for (unsigned int i = 0; i < vertex_use_count.size(); ++i)
//     {
//       if ( vertex_use_count[i] == 0 )
//       {
//         std::cout << "Found loose point: " << points[i] << std::endl;
//         loose_vertices.push_back( vertex_handles[i] );
//       }
//     }
//     std::cout << std::endl;
//
//     // search for degenerate lines
//     for (LinesIteratorType lit = lines.begin(); lit != lines.end();)
//     {
//       if ( lit->first == lit->second )
//       {
//         --vertex_use_count[ lit->first ];
//         --vertex_use_count[ lit->second ];
//
//         std::cout << "Found loose point: " << points[ lit->first ] << std::endl;
//         loose_vertices.push_back( viennagrid::vertices(*lit).handle_at(0) );
//
//         lines.erase(lit++);
//       }
//       else
//         ++lit;
//     }
//
//
//     while (!lines.empty())
//     {
//       // search for lines which are not within a polygonal ring (loose lines)
//       for (unsigned int i = 0; i != vertex_use_count.size();)
//       {
//         if (vertex_use_count[i] == 1)
//         {
//           for (LinesIteratorType lit = lines.begin(); lit != lines.end();)
//           {
//             if ((lit->first == i) || (lit->second == i) )
//             {
//               std::cout << "Found loose line" << std::endl;
//               loose_lines.push_back( std::make_pair( vertex_handles[lit->first], vertex_handles[lit->second] ) );
//
//               --vertex_use_count[ lit->first ];
//               --vertex_use_count[ lit->second ];
//
//               lines.erase(lit++);
//             }
//             else
//               ++lit;
//           }
//
//           // start from beginning
//           i = 0;
//         }
//         else
//           ++i;
//       }
//
//       if (lines.empty())
//         return true;
//
//       // find a point which is surely on the boundary polygon
//       Point2DType lowest_left = viennagrid::bounding_box( points_2d.begin(), points_2d.end() ).second + Point2DType(1,1);
//       int lowest_index = -1;
//
//       for (unsigned int i = 0; i != vertex_use_count.size(); ++i)
//       {
//         if (vertex_use_count[i] == 0)
//           continue;
//
//         if ((points_2d[i][0] <= lowest_index[0]) && (points_2d[i][1] <= lowest_index[1]))
//         {
//           lowest_index = i;
//           lowest_left = points_2d[i];
//         }
//       }
//
//       assert( lowest_index >= 0 );
//
//       int current_index = -1;
//       for (LinesIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
//       {
//         if ((lit->first == lowest_index) || (lit->second == lowest_index))
//         {
//           if (current_index == -1)
//             current_index = (lit->first == lowest_index) ? lit->second : lit->first;
//           else
//           {
//
//           }
//         }
//       }
//
//
// //       while
//
//     }
//
//
//
//
//     for (unsigned int i = 0; i != points.size(); ++i)
//     {
//       std::cout << points[i] << " " << points_2d[i] << std::endl;
//     }
//     std::cout << std::endl;
//
//
//
//
//
//
//
//
//
//     std::vector<Point2DType> projected_points;
//
//   }






  template<typename PLCT>
  std::vector< std::vector< typename viennagrid::result_of::const_vertex_handle<PLCT>::type > > split_plc_into_polygons( PLCT const & plc )
  {
    typedef typename viennagrid::result_of::const_vertex_range<PLCT>::type ConstVertexRangeType;
    typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

    typedef typename viennagrid::result_of::const_line_range<PLCT>::type ConstLineRangeType;
    typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineIteratorType;

    typedef typename viennagrid::result_of::line<PLCT>::type LineType;
    typedef typename viennagrid::result_of::id<LineType>::type LineIDType;

    typedef typename viennagrid::result_of::vertex<PLCT>::type VertexType;
    typedef typename viennagrid::result_of::id<VertexType>::type VertexIDType;


    typedef typename viennagrid::result_of::const_vertex_handle<PLCT>::type ConstVertexHandle;


    typedef std::vector< std::vector<ConstVertexHandle> > ReturnType;
    ReturnType polygons;



    ConstVertexRangeType vertices = viennagrid::vertices(plc);
    ConstLineRangeType lines = viennagrid::lines(plc);

    // first step: all line markers to false
    std::map<LineIDType, bool> line_marker;
    for (ConstLineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
    {
//       std::cout << *lit << std::endl;
      line_marker[lit->id()] = false;
    }


    std::vector<ConstLineIteratorType> loose_lines;

    // second step: find point lines
    for (ConstLineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
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
    for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
//       std::cout << *vit << std::endl;
      vertex_used_counter[vit->id()] = 0;
    }

    for (ConstLineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
    {
      if (line_marker[lit->id()]) continue;

      vertex_used_counter[ viennagrid::vertices(*lit)[0].id() ]++;
      vertex_used_counter[ viennagrid::vertices(*lit)[1].id() ]++;
    }

//     for (ConstVertexIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
//     {
//       std::cout << *vit << " " << vertex_used_counter[vit->id()] << std::endl;
//     }

    for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end();)
    {
      if (vertex_used_counter[vit->id()] == 0)
      {
        std::cout << "Something might be wrong?" << std::endl;
        continue;
      }

      if (vertex_used_counter[vit->id()] == 1)
      {
        for (ConstLineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
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







    for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
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
        for (ConstLineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
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

    return polygons;
  }


}


#endif
