/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
   
#ifndef GML_DATASTRUCTURE_VIEWELEMENT
#define GML_DATASTRUCTURE_VIEWELEMENT
//
// ===================================================================================
//
// *** BOOST include
//
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/is_reference.hpp>
//
// ===================================================================================
//
namespace gml
{
//
// ===================================================================================
//
template<typename Element>
struct ViewElement
{
   typedef ViewElement < Element >     Self;
   typedef Element*                    ElementPointer;
   typedef Element                     value_type; 

   ViewElement() 
   {
      _ele = NULL;
   }

   ViewElement(int const&) 
   {
      _ele = NULL;
   }   

   ViewElement(Element & ele) : _ele(&ele) { }
   
   operator Element& ()
   {
      return *_ele;
   }

   operator Element const& () const
   {
      return *_ele;
   }

   Self operator=(Element const& other)
   {
      *_ele = other;
      return *this;
   }   


   ElementPointer _ele;   
};
//
// ===================================================================================
//
} // end namespace: gml
#endif










