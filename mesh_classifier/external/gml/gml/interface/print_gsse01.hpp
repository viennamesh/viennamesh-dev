/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2007-2009 Franz Stimpfl                          franz@gsse.at    
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_INTERFACE_PRINTGSSE01
#define GML_INTERFACE_PRINTGSSE01
//
// ===================================================================================
//
// *** SYSTEM include
//
#include <fstream>
#include <iostream>
#include <iomanip>

//
// *** GML includes
//
#include "../util/size.hpp"
//
// ===================================================================================
//
//    [JW] TODO replace the preliminary prints ... 
//
// ===================================================================================
//

namespace gml
{

template < typename TopoEnv, typename GeomEnv >
void print_simple_gsse01(TopoEnv & topo_env, GeomEnv & geom_env, std::string const& filename)
{
   std::cout << "printing cell complex only ... " << std::endl;

   static const long DIMG = gml::result_of::size< typename GeomEnv::value_type >::value;
   // -----------------------------------
   std::ofstream  stream; 
   stream.open(filename.c_str());
   // -----------------------------------
   stream << DIMG << std::endl;
   stream << gsse::size(geom_env) << std::endl;
   stream.setf( std::ios::right,std::ios::adjustfield);
   stream << std::setprecision(12)  << std::setiosflags(std::ios::fixed);   
   // -----------------------------------
   for( long i = 0; i < gsse::size( geom_env ); i++ )
   {
      // -----------------------------------
      stream << geom_env[i] << std::endl;
      // -----------------------------------
   }
   // -----------------------------------
   stream << std::endl;
   // -----------------------------------
   stream << 1 << std::endl;
   // -----------------------------------
   stream << "Default-Segment-" + boost::lexical_cast<std::string>(0) << std::endl;
   stream << gsse::size( topo_env ) << std::endl;
   stream << "0" << std::endl;
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
   // -----------------------------------
   stream.close();   
   // -----------------------------------
}
} // end namespace: gml

#endif
