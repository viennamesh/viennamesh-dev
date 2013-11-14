#ifndef VIENNAMESH_UTILS_PLC_TOOLS_HPP
#define VIENNAMESH_UTILS_PLC_TOOLS_HPP

#include "viennagrid/mesh/mesh.hpp"

namespace viennamesh
{

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
        std::cout << "Some might be wrong?" << std::endl;
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
