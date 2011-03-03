/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_ORIENTATION_HH_ID
#define GSSE_ORIENTATION_HH_ID 

// *** system includes
//
#include <iostream>
#include <set>
#include <vector>

// *** BOOST includes
// 
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

// *** GSSE includes
//
#include "gsse/topology.hpp"
#include "gsse/topology/structured_topology.hpp"


namespace gsse
{


struct orient_triangle_to_triangle
{
   template <typename CellIterator1, typename CellIterator2>
   bool operator()(CellIterator1 c1, CellIterator2 c2)
      {
         if (*(c1+0) == *(c2+0) && *(c1+1) == *(c2+1) && *(c1+2) == *(c2+2)) return true;
         if (*(c1+0) == *(c2+1) && *(c1+1) == *(c2+3) && *(c1+2) == *(c2+2)) return true;
         if (*(c1+0) == *(c2+3) && *(c1+1) == *(c2+0) && *(c1+2) == *(c2+2)) return true;
         if (*(c1+0) == *(c2+0) && *(c1+1) == *(c2+3) && *(c1+2) == *(c2+1)) return true;    
         return false;
      }
};



struct orient_vertex_on_edge 
{

   template <typename VertexIterator1, typename VertexIterator2>
   bool operator() (VertexIterator1 v1, VertexIterator2 v2)
      {
         return ((*v1) == (*v2));
      }
};
    
struct orient_edge_on_tri
{
   template <typename VertexIterator1, typename VertexIterator2>
   bool operator()(VertexIterator1 v1, VertexIterator2 v2)
      {
         if (*(v1+0) == *(v2+0) && *(v1+1) == *(v2+1)) return true;
         if (*(v1+0) == *(v2+1) && *(v1+1) == *(v2+2)) return true;
         if (*(v1+0) == *(v2+2) && *(v1+1) == *(v2+0)) return true;
         return false;
      }
};

struct orient_edge_on_rect
{
   template <typename VertexIterator1, typename VertexIterator2>
   bool operator()(VertexIterator1 v1, VertexIterator2 v2)
      {
         //std::cout << "orient edge on rectangle" << std::endl;

//      std::cout << "rectangle: " << std::endl;
//      std::cout << v1.base() << std::endl;
//      std::cout << *(v1+0) << std::endl;
//      std::cout << *(v1+1) << std::endl;
//      std::cout << *(v1+2) << std::endl;
//      std::cout << *(v1+3) << std::endl;
//
//      std::cout << "edge: " << std::endl;
//      std::cout << v2.base() << std::endl;
//      std::cout << *(v2+0) << std::endl;
//      std::cout << *(v2+1) << std::endl;

         if (*(v2+0) == *(v1+0) && *(v2+1) == *(v1+1)) return true;
         if (*(v2+0) == *(v1+1) && *(v2+1) == *(v1+3)) return true;
         if (*(v2+0) == *(v1+3) && *(v2+1) == *(v1+2)) return true;
         if (*(v2+0) == *(v1+2) && *(v2+1) == *(v1+0)) return true;
         if (*(v2+0) == *(v1+1) && *(v2+1) == *(v1+0)) return false;
         if (*(v2+0) == *(v1+3) && *(v2+1) == *(v1+1)) return false;
         if (*(v2+0) == *(v1+2) && *(v2+1) == *(v1+3)) return false;
         if (*(v2+0) == *(v1+0) && *(v2+1) == *(v1+2)) return false;

         return false;    
      }
};

struct orient_tri_on_tetra
{
   template <typename VertexIterator1, typename VertexIterator2>
   bool operator()(VertexIterator1 v1, VertexIterator2 v2)
      {
         //std::cout << "Orient Triangle on Tetrahedron" << std::endl;

         if (*(v1+0) == *(v2+0) && *(v1+1) == *(v2+1) && *(v1+2) == *(v2+2)) return true;
         if (*(v1+0) == *(v2+1) && *(v1+1) == *(v2+3) && *(v1+2) == *(v2+2)) return true;
         if (*(v1+0) == *(v2+3) && *(v1+1) == *(v2+0) && *(v1+2) == *(v2+2)) return true;
         if (*(v1+0) == *(v2+0) && *(v1+1) == *(v2+3) && *(v1+2) == *(v2+1)) return true;    
         return false;
      }
};

struct orient_rect_on_cube
{
   template <typename VertexIterator1, typename VertexIterator2>
   bool operator()(VertexIterator1 v1, VertexIterator2 v2)
      {
         if ((*(v1+0) == *(v2+0) && *(v1+1) == *(v2+1) && *(v1+2) == *(v2+3) && *(v1+3) == *(v2+2))
             || (*(v1+0) == *(v2+1) && *(v1+1) == *(v2+5) && *(v1+2) == *(v2+7) && *(v1+3) == *(v2+3))
             || (*(v1+0) == *(v2+4) && *(v1+1) == *(v2+6) && *(v1+2) == *(v2+7) && *(v1+3) == *(v2+5)) 
             || (*(v1+0) == *(v2+0) && *(v1+1) == *(v2+4) && *(v1+2) == *(v2+6) && *(v1+3) == *(v2+4)) 
             || (*(v1+0) == *(v2+0) && *(v1+1) == *(v2+4) && *(v1+2) == *(v2+5) && *(v1+3) == *(v2+3)) 
             || (*(v1+0) == *(v2+2) && *(v1+1) == *(v2+3) && *(v1+2) == *(v2+7) && *(v1+3) == *(v2+6)))
            return true;    
         return false;
      }
};

// required for inter-dimensional traversal operations
//
struct orient_false
{
   template <typename VertexIterator1, typename VertexIterator2>
   bool operator()(VertexIterator1 v1, VertexIterator2 v2)
   {
      return false;
   }
};

template <typename Function>
struct commutate
{
   template <typename T1, typename T2>
   bool operator()(T1 t1, T2 t2)
   {
      return Function()(t2, t1);
   }
};

  
template <typename Elem1, typename Elem2>  
bool orient_i(const Elem1& e1, const Elem2& e2)
{
   typedef typename topology_traits<typename Elem1::topology_type>::
      template vertex_on_element_iterator<Elem1>::type iterator1_type;

   typedef typename topology_traits<typename Elem2::topology_type>::
      template vertex_on_element_iterator<Elem2>::type iterator2_type;

   iterator1_type iter1(e1);
   iterator2_type iter2(e2);

   typedef typename 
      topology_traits<typename Elem1::topology_type>::template 
      orientation_func<Elem1, Elem2>::type
      orientation_func;

   return orientation_func()(iter1, iter2);
}


}

#endif
