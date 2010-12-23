/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_FOREACH_HH
#define GSSE_FOREACH_HH

// *** BOOST includes
//
#include <boost/lambda/lambda.hpp>


// *** GSSE includes
//
#include "gsse/segment.hpp"
#include "gsse/domain.hpp"




// ### [RH] we have to use our for_each
//
namespace gsse
{

template <typename Iter, typename Functor>
void for_each(Iter begin, Iter end, Functor func)
{
   while (begin != end)
   {
      func(boost::lambda::make_const(*begin));
      ++begin;
   }
}

template <typename TraversalObject, typename Functor>
void for_each_segment(TraversalObject traversal_object, Functor func)
{
   typedef typename domain_traits<TraversalObject>::segment_iterator  object_iterator;
   
   for (object_iterator o_it  = traversal_object.segment_begin();
                        o_it != traversal_object.segment_end();
                      ++ o_it)
   {
      func( *o_it );
      //func(boost::lambda::make_const(*o_it));
   }
}



template <typename TraversalObject, typename Functor>
void for_each_vertex(TraversalObject traversal_object, Functor func)
{
   typedef typename segment_traits<TraversalObject>::vertex_iterator  object_iterator;
   
   for (object_iterator o_it  = traversal_object.vertex_begin();
                        o_it != traversal_object.vertex_end();
                      ++ o_it)
   {
      func(boost::lambda::make_const(*o_it));
   }
}


template <typename TraversalObject, typename Functor>
void for_each_cell(TraversalObject traversal_object, Functor func)
{
   typedef typename segment_traits<TraversalObject>::cell_iterator  cell_iterator;
   
   for (cell_iterator c_it  = traversal_object.cell_begin();
                      c_it != traversal_object.cell_end();
                      ++ c_it)
   {
      func(boost::lambda::make_const(*c_it));
   }
}



template <typename TraversalObject, typename Functor>
void for_each_edge(TraversalObject traversal_object, Functor func)
{
   typedef typename segment_traits<TraversalObject>::edge_iterator  object_iterator;
   
   for (object_iterator o_it  = traversal_object.edge_begin();
                        o_it != traversal_object.edge_end();
                      ++ o_it)
   {
      func(boost::lambda::make_const(*o_it));
   }
}

template <typename TraversalObject, typename Functor>
void for_each_facet(TraversalObject traversal_object, Functor func)
{
   typedef typename segment_traits<TraversalObject>::facet_iterator  object_iterator;
   
   for (object_iterator o_it  = traversal_object.facet_begin();
                        o_it != traversal_object.facet_end();
                      ++ o_it)
   {
      func(boost::lambda::make_const(*o_it));
   }
}




}



#endif
