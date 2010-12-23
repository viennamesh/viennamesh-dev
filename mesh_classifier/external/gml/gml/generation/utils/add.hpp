/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       
   Copyright (c) 2007-2009 Franz Stimpfl                          franz@gsse.at    

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GML_GENERATION_UTILS_ADD
#define GML_GENERATION_UTILS_ADD

namespace gml {


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
    return gml::add_hole_object_unary<Element>(ele);
}
// ===============================================================
template<typename Element>
inline
add_hole_object_unary<Element>
add_hole(Element & ele)              
{
    return gml::add_hole_object_unary<Element>(ele);
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
    return gml::add_object_unary<Element>(ele);
}
// ===============================================================
template<typename Element>
inline
add_object_unary<Element>
add(Element & ele)              
{
    return gml::add_object_unary<Element>(ele);
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
      gml::add( ele )( target );
   }
};
//boost::phoenix::function< gml::functional::add_impl > const add = gml::functional::add_impl();
boost::phoenix::function< gml::functional::add_impl > add;
} // end namespace: functional

} // end namespace: gml

#endif






















