/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_ALGORITHMS_UNIQUE
#define GML_ALGORITHMS_UNIQUE

//
// ===================================================================================
//
// *** GML includes
//
#include "../util/debug.hpp"
//
// *** GSSE includes
//
#include <gsse/algorithm/modification.hpp>
//
// *** BOOST includes
//
#include <boost/utility/enable_if.hpp>
#include  <boost/type_traits/is_integral.hpp>
#include  <boost/type_traits/is_array.hpp>
//
// ===================================================================================
//
namespace gml{

template < typename Container >
void unique(Container& container)
{
   std::cout << "ERROR - GML::ALGORITHMS::UNIQUE - there is no unique algorithm implemented for"
   << " this container: "; 
   gml::dumptype< Container >();

}

template<typename T1>
void unique(std::vector<T1>& container)
{
   std::cout << "gsse unique.. "<< std::endl;
   gsse::algorithm::unique( container );
}
/*
template<typename T1>
void unique(std::vector<T1>& container, 
            typename boost::enable_if< boost::is_array< T1 > >::type* dummy = 0)
{
   std::cout << "my unique .. " << std::endl;
   typedef std::map< T1, bool > UniqueSet;

   UniqueSet   unique_set;

   for( long ei = 0; ei < gsse::size( container ); ei++ )
   {
      std::sort( container[ei].begin(), container[ei].end() ); 
      //std::sort( cont[ei].begin(), cont[ei].begin()+2 ); 

      if( !unique_set[container[ei]] )
         unique_set[container[ei]] = true;
   }      
   
   long size_unique = gsse::size( unique_set );
   
   container.clear();
   gsse::resize( size_unique )( container );
   
   long ei = 0;
   for( typename UniqueSet::iterator iter = unique_set.begin(); 
         iter != unique_set.end(); iter++ )
   {      
      gml::copy( iter->first, container[ei] );
      ei++;
   }
}
*/
} // end namespace: gml

#endif
