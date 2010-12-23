/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at    

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GML_DATASTRUCTURE_METRICOBJECT
#define GML_DATASTRUCTURE_METRICOBJECT

//
// ===================================================================================
//
// *** GSSE includes
#include <gsse/geometry/metric_object.hpp>
//
// *** GML includes
//
#include "../util/tags.hpp"
//
// ===================================================================================
//

namespace gml{

template< typename NumericT, int DIM >
struct metric_object : gsse::metric_object< NumericT, DIM >
{
   static const int dimension = DIM;
   
   typedef metric_object<NumericT, DIM>    self;
   typedef typename self::iterator         iterator;
   typedef typename self::const_iterator   const_iterator;   
   
   typedef gml::tag_metric_object               tag_type;
   typedef gml::tag_compiletime                 evaluation_type;   
   typedef tag_compiletime_container            container_type; // gsse ..

   typedef gsse::metric_object< NumericT, DIM >    BaseT;

   metric_object() 
      : BaseT() {}
   
   metric_object(NumericT a1) 
      : BaseT(a1) {}

   metric_object(NumericT a1, NumericT a2) 
      : BaseT(a1, a2) {}
   
   metric_object(NumericT a1, NumericT a2, NumericT a3) 
      : BaseT(a1, a2, a3) {}   

   metric_object(NumericT a1, NumericT a2, NumericT a3, NumericT a4) 
      : BaseT(a1, a2, a3, a4) {}   

/*  [JW] TODO test this 
   self 
   operator=( const self& other ) const
   {
      iterator i = (*this).begin(), i_end = (*this).end();      
      const_iterator j = other.begin();
      for ( ; i != i_end; ++i, ++j ) {
         *i = *j;
      }
      return (*this);
   }  
*/   
};

}


#endif
