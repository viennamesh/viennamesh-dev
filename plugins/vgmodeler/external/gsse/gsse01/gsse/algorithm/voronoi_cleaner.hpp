/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c)      2007 Josef Weinbub              josef@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */


#ifndef GSSE_VORONOI_CLEANER_HPP
#define GSSE_VORONOI_CLEANER_HPP


// *** GSSE includes
//
#include "gsse/domain.hpp"


template <typename DomainT>
void clean_voronoi(DomainT& domain, std::string edge_area_key = std::string("edge_area"))
{
   typedef typename gsse::domain_traits<DomainT>::segment_iterator         segment_iterator;
   typedef typename gsse::domain_traits<DomainT>::vertex_iterator          vertex_iterator;
   typedef typename gsse::domain_traits<DomainT>::edge_on_vertex_iterator  edge_on_vertex_iterator;

   std::cout << "VORONOI EDGE AREA CLEANER ACTIVE" << std::endl;

   for (segment_iterator segit  = domain.segment_begin();  segit != domain.segment_end();   ++segit)
   {
      for (vertex_iterator vit = (*segit).vertex_begin(); vit != (*segit).vertex_end(); ++vit)
      {
         for (edge_on_vertex_iterator eovit(*vit); eovit.valid(); ++ eovit)
         {
            if( (*segit)(*eovit, edge_area_key)(0,0) < 0.0 )
               (*segit)(*eovit, edge_area_key)(0,0) *= -1.0;
         }
      }
   }
}
#endif
