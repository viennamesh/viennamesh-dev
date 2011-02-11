/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2007-2010 Franz Stimpfl                          franz@gsse.at    
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GSSE_INTERFACE_PRINTPLY
#define GSSE_INTERFACE_PRINTPLY
//
// ===================================================================================
//
// *** SYSTEM include
//
#include <fstream>
#include <iostream>
#include <iomanip>

//
// *** GSSE includes
//
#include "../util/size.hpp"
//
// ===================================================================================
//
//    [JW] TODO replace the preliminary prints ... 
//
// ===================================================================================
//

namespace gsse
{

template < typename TopoEnv, typename GeomEnv >
void print_ply(TopoEnv & topo_env, GeomEnv & geom_env, GeomEnv & holes, std::string const& filename)
{
   std::cout << "printing cell complex only in ply format ... " << std::endl;

   // -----------------------------------
   std::ofstream  stream; 
   stream.open(filename.c_str());
   // -----------------------------------
   stream << gsse::size(geom_env) << " 2 1 0" << std::endl;
   stream.setf( std::ios::right,std::ios::adjustfield);
   stream << std::setprecision(12)  << std::setiosflags(std::ios::fixed);   
   // -----------------------------------
   for( long i = 0; i < gsse::size( geom_env ); i++ )
   {
      // -----------------------------------
      stream << i << " " << geom_env[i] << std::endl;
      // -----------------------------------
   }
   // -----------------------------------
   stream << std::endl;
   // -----------------------------------
   stream << gsse::size( topo_env ) << " 0" << std::endl;
   // -----------------------------------
   for( long ci = 0; ci < gsse::size( topo_env ); ci++ )
   {
      // -----------------------------------
      stream << ci << " " << topo_env[ci] << std::endl;
      // -----------------------------------
   }
   // -----------------------------------
   
   // -----------------------------------   
   stream << std::endl;
   
   stream << gsse::size(holes) << std::endl;   
   // -----------------------------------
   for( long i = 0; i < gsse::size( holes ); i++ )
   {
      // -----------------------------------
      stream << i+1 << " " << holes[i] << std::endl;
      // -----------------------------------
   }
   
   stream.close();   
   // -----------------------------------
}
} // end namespace: gsse

#endif
