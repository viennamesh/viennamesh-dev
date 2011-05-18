/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_QUAN_CHECK
#define GSSE_QUAN_CHECK

// *** system includes
//
#include <string>

// *** GSSE includes
//
#include "gsse/segment.hpp"


namespace gsse
{

template <typename DomainT>
bool quan_on_domain(DomainT & domain, std::string quan_name)
{
   typedef typename gsse::domain_traits<DomainT>::domain_key_iterator               domain_key_iterator; 

   for (domain_key_iterator oq_it = domain.domain_key_begin(); oq_it != domain.domain_key_end(); ++oq_it)
   {
      if ( (*oq_it) == quan_name)
         return true;
   }
   
   return false;
}


template <typename SegmentT>
bool quan_on_segment(SegmentT & seg, std::string quan_name)
{

   typedef typename gsse::segment_traits<SegmentT>::segment_key_iterator               segment_key_iterator; 

   for (segment_key_iterator oq_it = seg.segment_key_begin(); oq_it != seg.segment_key_end(); ++oq_it)
   {
      if ( (*oq_it) == quan_name)
         return true;
   }
   
   return false;
}

template <typename SegmentT>
bool squan_on_segment(SegmentT & seg, std::string quan_name)
{
   typedef typename gsse::segment_traits<SegmentT>::segment_skey_iterator              segment_skey_iterator; 

   for (segment_skey_iterator oq_it = seg.segment_skey_begin(); oq_it != seg.segment_skey_end(); ++oq_it)
   {
      if ( (*oq_it) == quan_name)
         return true;
   }

   return false;
}


// ### domain :: quantities
//
template <typename DomainT>
bool quan_on_vertex(DomainT& domain, std::string quan_name)
{
   typedef typename gsse::domain_traits<DomainT>::segment_iterator                   segment_iterator; 
   typedef typename gsse::domain_traits<DomainT>::segment_vertex_quantity_iterator   vertex_quan_iterator;

   for (segment_iterator seg_it = domain.segment_begin(); seg_it != domain.segment_end(); ++seg_it)
   {      
      for (vertex_quan_iterator oq_it = (*seg_it).vertex_key_begin(); oq_it != (*seg_it).vertex_key_end(); ++oq_it)
      {
//          std::cout << " quan name: " << quan_name << std::endl;
//          std::cout << "    oq_it:  " << *oq_it << std::endl;

         if ( (*oq_it) == quan_name)
            return true;
      }
      
   }

   return false;
}

//
// [JW] added "vertex quantity on specific segment" - check
//
template <typename DomainT>
bool quan_on_vertex_seg(   typename gsse::domain_traits<DomainT>::segment_iterator&    seg_it,
                           std::string                                                 quan_name)
{
   typedef typename gsse::domain_traits<DomainT>::segment_vertex_quantity_iterator   vertex_quan_iterator;

   for (vertex_quan_iterator oq_it = (*seg_it).vertex_key_begin(); oq_it != (*seg_it).vertex_key_end(); ++oq_it)
   {
      if ( (*oq_it) == quan_name)
         return true;
   }
   return false;
}


template <typename DomainT>
bool quan_on_edge(DomainT& domain, std::string quan_name)
{
   typedef typename gsse::domain_traits<DomainT>::segment_iterator                   segment_iterator; 
   typedef typename gsse::domain_traits<DomainT>::segment_edge_quantity_iterator     edge_quan_iterator;

   for (segment_iterator seg_it = domain.segment_begin(); seg_it != domain.segment_end(); ++seg_it)
   {      
      for (edge_quan_iterator oq_it = (*seg_it).edge_key_begin(); oq_it != (*seg_it).edge_key_end(); ++oq_it)
      {
         if ( (*oq_it) == quan_name)
            return true;
      }
   }
   return false;
}

template <typename DomainT>
bool quan_on_facet(DomainT& domain, std::string quan_name)
{
   typedef typename gsse::domain_traits<DomainT>::segment_iterator                   segment_iterator; 
   typedef typename gsse::domain_traits<DomainT>::segment_facet_quantity_iterator    facet_quan_iterator;

   for (segment_iterator seg_it = domain.segment_begin(); seg_it != domain.segment_end(); ++seg_it)
   {      
      for (facet_quan_iterator oq_it = (*seg_it).facet_key_begin(); oq_it != (*seg_it).facet_key_end(); ++oq_it)
      {
         if ( (*oq_it) == quan_name)
            return true;
      }
   }
   return false;
}

template <typename DomainT>
bool quan_on_cell(DomainT& domain, std::string quan_name)
{
   typedef typename gsse::domain_traits<DomainT>::segment_iterator                   segment_iterator; 
   typedef typename gsse::domain_traits<DomainT>::segment_cell_quantity_iterator     cell_quan_iterator;

   for (segment_iterator seg_it = domain.segment_begin(); seg_it != domain.segment_end(); ++seg_it)
   {      
      for (cell_quan_iterator oq_it = (*seg_it).cell_key_begin(); oq_it != (*seg_it).cell_key_end(); ++oq_it)
      {
         if ( (*oq_it) == quan_name)
            return true;
      }
   }
   return false;
}
//
// [JW] added "cell quantity on specific segment" - check
//
template <typename DomainT>
bool quan_on_cell_seg(  typename gsse::domain_traits<DomainT>::segment_iterator&    seg_it,
                        std::string                                                 quan_name)
{
   typedef typename gsse::domain_traits<DomainT>::segment_cell_quantity_iterator     cell_quan_iterator;

   for (cell_quan_iterator oq_it = (*seg_it).cell_key_begin(); oq_it != (*seg_it).cell_key_end(); ++oq_it)
   {
      if ( (*oq_it) == quan_name)
      return true;
   }
   return false;
}



}   // namespace gsse

#endif
