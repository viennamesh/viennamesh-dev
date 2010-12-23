/* ============================================================================
   Copyright (c) 2003-2008 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2004-2008 Philipp Schwaha                      philipp@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_ALGORITHM_TOPOLOGICAL_HH)
#define GSSE_ALGORITHM_TOPOLOGICAL_HH

// *** system includes
// *** BOOST includes
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/home/phoenix/container.hpp>
// *** GSSE includes
#include "gsse/exception/exceptions.hpp"
#include "gsse/traversal.hpp"
#include "gsse/util.hpp"

// ############################################################
//
namespace gsse { namespace algorithm { namespace topological {


// ##############################################################################
//
// generic copy by indirection algorithm
//   does not need to know any size
//
template<typename ContainerTo, typename ContainerView, typename ContainerSrc>
void copy_by_view(ContainerTo&   container_to,
                  const ContainerView& container_view,  
                  const ContainerSrc&  container_src)
{
   using namespace boost::phoenix::arg_names;

   gsse::traverse()  // [RH][debug][path][XX3]
   [ 
      boost::phoenix::push_back(boost::phoenix::ref(container_to), 
                                boost::phoenix::at(boost::phoenix::ref(container_src), boost::phoenix::arg_names::_1)) 
   ] (container_view);
}



template<typename Container2Find, typename ContainerSearch>
bool subset(const Container2Find&   container_find, 
            const ContainerSearch&  container_search)
{
   size_t cnt_find = 0;
   for (size_t cs_i = 0; cs_i < gsse::size( container_search ); ++cs_i)
   {
      for (size_t cf_i = 0; cf_i < gsse::size( container_find ); ++cf_i)
      {
         if ( container_find[cf_i] == container_search[cs_i])
            ++cnt_find;
      }
   }
   if (cnt_find == gsse::size( container_find ))
      return true;

   return false;
}



// calculates the intersection of two sets
//   container1 and container2 have to model
//   unique sequences    
//
//     ContainerT:: has to model 
//              push_back
//              operator[]
//              size()
//
template<typename ContainerT1,
         typename ContainerT2>
ContainerT1 
intersection(ContainerT1 const& container1, 
	     ContainerT2 const& container2)
{
  ContainerT1 intersection_set;
  
  for (size_t i = 0; i < gsse::size(container1); ++i)
  {
     for (size_t j = 0; j < gsse::size(container2); ++j)
     {
        if (container1[i] == container2[j])
           intersection_set.push_back(container2[j]);
     }
  }
  
  return intersection_set;
}


}  // namespace topological
} // namespace algorithm

}// namespace gsse


#endif



