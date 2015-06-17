/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2007-2009 Franz Stimpfl                          franz@gsse.at    
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_SEQUENCE_TAGIF
#define GML_FUNCTORS_TAGIF
//
// ===================================================================================
// *** GSSE includes
//
#include <gsse/util/access.hpp>
//
// ===================================================================================
//
namespace gml 
{
template < typename TagContainer >
struct tag_if 
{ 
   typedef bool        result_type;
   // ------------------------------------------------
   tag_if( TagContainer & tag_cont ) 
            : _tag_cont(tag_cont) { }
   // ------------------------------------------------
   result_type operator() (bool & tag)
   {
      if(tag) 
         gsse::pushback( _tag_cont ) = 1;
      else
         gsse::pushback( _tag_cont ) = 0;      
      return tag;
   }   
   // ------------------------------------------------      
   result_type operator() (bool const& tag)
   {
      if(tag) 
         gsse::pushback( _tag_cont ) = 1;
      else
         gsse::pushback( _tag_cont ) = 0;      
      return tag;
   }   
   // ------------------------------------------------   
   TagContainer & _tag_cont;
   // ------------------------------------------------
};

} // end namespace: gml
#endif















