/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_INTERFACE_READGSSE01
#define GML_INTERFACE_READGSSE01
//
// ===================================================================================
//
// *** SYSTEM include
//
#include <fstream>
//
// *** GML include
//
#include "../util/common.hpp"



namespace gml
{
template < typename TopoEnv, typename GeomEnv >
void read_simple_gsse01(TopoEnv & topo_env, GeomEnv & geom_env, std::string const& filename)
{
   typedef typename gml::result_of::val< TopoEnv >::type             Cell;

   // -----------------------------------
   std::ifstream file(filename.c_str(), std::ios::binary);
   // -----------------------------------
   std::string tempinput;
   long gsse_dim;
   file >> gsse_dim; // dimension

   long size_vertices;
   file >> size_vertices;

   gsse::resize( size_vertices )( geom_env );

   for (long i =0 ; i < size_vertices; ++i)
   {
      for (long di = 0; di < gsse_dim; ++di)
      {
         file >> geom_env[i][di];
      }
   }
   // -----------------------------------   
   long size_segments;
   file >> size_segments;

   // -----------------------------------  
   std::string name_segments;
   file >> tempinput;
   for (long cnt_seg = 0; cnt_seg < size_segments; ++cnt_seg)
   {
      name_segments = tempinput;
      
      long size_segment_cells;
      file >> size_segment_cells;
      file >> tempinput;  // nb of different vertices
      
      for (long i = 0 ; i < size_segment_cells; ++i)
      {
         Cell cell;
         file >> tempinput; // cell handle/id
         
         for (unsigned int ci = 0; ci < cell.size(); ++ci)
         {
            file >> cell[ci];
         }
         gsse::at(i)(topo_env) = cell;

      }        
   }
        
}


} // end namespace: gml

#endif
