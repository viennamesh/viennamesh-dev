/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at       
   Copyright (c) 2007-2010 Franz Stimpfl                          franz@gsse.at   

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
   
#ifndef GMI_UTILS_ADD
#define GMI_UTILS_ADD

#include <boost/spirit/home/phoenix/function/function.hpp>

namespace gmi {


template<typename Element>
struct add_hole_object_unary         
{
   // ------------------------------------------
   add_hole_object_unary(Element const& ele) : _ele(ele) {}
   add_hole_object_unary(Element      & ele) : _ele(ele) {}
   // ------------------------------------------
   template <typename Target>
   inline void 
   operator()(Target const& target)
   {
      target.add_hole(_ele);
   }
   template <typename Target>
   inline void
   operator()(Target & target)
   {
      target.add_hole(_ele);
   }   
   // ------------------------------------------
private:
   // ------------------------------------------
   Element _ele;
   // ------------------------------------------
};
// ===============================================================
template<typename Element>
inline
add_hole_object_unary<Element>
add_hole(Element const& ele)         
{
    return gmi::add_hole_object_unary<Element>(ele);
}
// ===============================================================
template<typename Element>
inline
add_hole_object_unary<Element>
add_hole(Element & ele)              
{
    return gmi::add_hole_object_unary<Element>(ele);
}



// ===============================================================
// ===============================================================
template<typename Element>
struct add_object_unary         
{
   // ------------------------------------------
   add_object_unary(Element const& ele) : _ele(ele) {}
   add_object_unary(Element      & ele) : _ele(ele) {}
   // ------------------------------------------
   template <typename Target>
   inline void 
   operator()(Target const& target)
   {
      target.add(_ele);
   }
   template <typename Target>
   inline void
   operator()(Target & target)
   {
      target.add(_ele);
   }   
   // ------------------------------------------
private:
   // ------------------------------------------
   Element _ele;
   // ------------------------------------------
};
// ===============================================================
template<typename Element>
inline
add_object_unary<Element>
add(Element const& ele)         
{
    return gmi::add_object_unary<Element>(ele);
}
// ===============================================================
template<typename Element>
inline
add_object_unary<Element>
add(Element & ele)              
{
    return gmi::add_object_unary<Element>(ele);
}
// ===============================================================

namespace functional {

struct add_impl
{    
   template< typename Element, typename Target >
   struct result
   { 
      typedef void  type;
   };
      
   template< typename Element, typename Target >
   typename result< Element, Target >::type 
   operator()( Element & ele, Target & target )  const
   {
      gmi::add( ele )( target );
   }
};
//boost::phoenix::function< gmi::functional::add_impl > const add = gmi::functional::add_impl();
boost::phoenix::function< gmi::functional::add_impl > add;
} // end namespace: functional

} // end namespace: gmi

#endif






















