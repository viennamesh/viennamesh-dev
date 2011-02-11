/* ============================================================================
   Copyright (c) 2007-2010 Franz Stimpfl                          franz@gsse.at   
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GSSE_MESH_STRATEGY
#define GSSE_MESH_STRATEGY

namespace gsse {
   namespace mesh {
      namespace strategy {
        
         struct advancing_front      {};               
         struct incremental_delaunay {};               
         struct constrained_delaunay {};               
         struct conforming_delaunay  {};               
         struct quadtree             {};
         struct octtree              {};
                  
      } // end namespace: strategy
   }
}

#endif
