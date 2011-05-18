/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_STRUCTURE_HH_ID
#define GSSE_STRUCTURE_HH_ID


// *** system includes
//
#include <algorithm>


// *** GSSE includes
//
#include "gsse/domain.hpp"
#include "gsse/algorithm/interpolation.hpp"
#include "gsse/math/geometric_barycenter.hpp"


//#define DEBUG

namespace gsse 
{

//  create the structural domain information 
//    create all quantity names .. NO actual quantity transfer is made !!!! 
//
template<typename DomainT>
void transfer_domain_information(DomainT& domain_old, DomainT& domain_new)
{
   typedef typename gsse::domain_traits<DomainT>::segment_iterator         segment_iterator;
   typedef typename gsse::domain_traits<DomainT>::vertex_type              vertex_type;
   typedef typename gsse::domain_traits<DomainT>::edge_type                edge_type;
   typedef typename gsse::domain_traits<DomainT>::facet_type               facet_type;
   typedef typename gsse::domain_traits<DomainT>::cell_type                cell_type;
   typedef typename gsse::domain_traits<DomainT>::vertex_handle            vertex_handle;
   typedef typename gsse::domain_traits<DomainT>::vertex_iterator          vertex_iterator;
   typedef typename gsse::domain_traits<DomainT>::cell_iterator            cell_iterator;
   typedef typename gsse::domain_traits<DomainT>::storage_type        storage_type;

   typedef typename gsse::domain_traits<DomainT>::segment_key_iterator     segment_key_iterator;
   typedef typename gsse::domain_traits<DomainT>::segment_skey_iterator    segment_skey_iterator;

   typedef typename gsse::domain_traits<DomainT>::vertex_key_iterator      vertex_key_iterator;
   typedef typename gsse::domain_traits<DomainT>::edge_key_iterator        edge_key_iterator;
   typedef typename gsse::domain_traits<DomainT>::facet_key_iterator       facet_key_iterator;
   typedef typename gsse::domain_traits<DomainT>::cell_key_iterator        cell_key_iterator;



   // [RH][TODO] .. maybe select some quantities which are copied
   //


   // iterate over all domain_old_segments ..
   //
   segment_iterator seg_it_old = domain_old.segment_begin(), 
                    seg_it_new = domain_new.segment_begin();
   for ( ; seg_it_old != domain_old.segment_end(); ++seg_it_old, ++seg_it_new)
   {

      // ================================================
      //
      // copy all segment quantities onto the new domain
      //
//       std::cout << "segname : " << (*seg_it_old).get_name() << std::endl;
      for (segment_key_iterator ski=(*seg_it_old).segment_key_begin(); ski != (*seg_it_old).segment_key_end(); ++ski)
      {
//          std::cout << " quan name: " << *ski << std::endl;
         (*seg_it_new).store_quantity( *ski, (*seg_it_old)(*ski));
      }

      // ================================================
      //
      // copy all string segment quantities onto the new domain
      //
      for (segment_skey_iterator sski=(*seg_it_old).segment_skey_begin(); sski != (*seg_it_old).segment_skey_end(); ++sski)
      {
//          std::cout << " squan name: " << *sski << std::endl;
         std::string tempname;
         (*seg_it_old).retrieve_quantity( *sski , tempname );
         (*seg_it_new).store_quantity( *sski, tempname);
      }


      // ================================================
      //
      // copy all remaining vertex-quantities onto the new domain
      //
      for (vertex_key_iterator object_it=(*seg_it_old).vertex_key_begin(); object_it != (*seg_it_old).vertex_key_end(); ++object_it)
      {
         (*seg_it_new).add_vertex_quantity(*object_it);

         // just to make it debugable
         //
         vertex_type temp_object(*((*seg_it_new).vertex_begin()));
         (domain_new).store_quantity( temp_object , *object_it ,
                                      (domain_old)((*(*seg_it_old).vertex_begin()), *object_it)  );
      }

      //
      // copy all remaining edge-quantities onto the new domain
      //
      for (edge_key_iterator object_it=(*seg_it_old).edge_key_begin(); object_it != (*seg_it_old).edge_key_end(); ++object_it)
      {
         (*seg_it_new).add_edge_quantity(*object_it);

         // just to make it debugable
         //
         edge_type temp_object(*((*seg_it_new).edge_begin()));
         (domain_new).store_quantity( temp_object , *object_it ,
                                       (domain_old)((*(*seg_it_old).edge_begin()), *object_it)  );
      }
      //
      // copy all remaining facet-quantities onto the new domain
      //
      for (facet_key_iterator object_it=(*seg_it_old).facet_key_begin(); object_it != (*seg_it_old).facet_key_end(); ++object_it)
      {
         (*seg_it_new).add_facet_quantity(*object_it);

         // just to make it debugable
         //
         facet_type temp_object(*((*seg_it_new).facet_begin()));
         (domain_new).store_quantity( temp_object , *object_it ,
                                       (domain_old)((*(*seg_it_old).facet_begin()), *object_it)  );
      }
      //
      // copy all remaining cell-quantities onto the new domain
      //
      for (cell_key_iterator object_it=(*seg_it_old).cell_key_begin(); object_it != (*seg_it_old).cell_key_end(); ++object_it)
      {
         (*seg_it_new).add_cell_quantity(*object_it);

         // just to make it debugable
         //
         cell_type temp_object(*((*seg_it_new).cell_begin()));
         (domain_new).store_quantity( temp_object , *object_it ,
                                       (domain_old)((*(*seg_it_old).cell_begin()), *object_it)  );
      }
      
   }
}

//
// this method interpolates the given interpolatio-quanity (quan_name) and uses the information of the old domain -> domain
// thepoint .. is from the newly created domain
//
// this method uses three parts
//  part I)   a fast finding method.. normally this is used if the new point is one of the old-domain points
//  part II)  here can't find a point at the old-domain-location.. extend the search parameter until an old point is found
//  part III) this is a robustness issue.. 
//             one example of this use is, if the mesher adds new points along the surface and these points are not inside the old domain
//             here we just use the nearest match of another point/quantity
//
template<typename DomainT, typename GenericPoint>
double interpolate_point(DomainT& domain, GenericPoint thepoint, std::string quan_name)
{
   typedef typename gsse::domain_traits<DomainT>::segment_iterator            segment_iterator;
   typedef typename gsse::domain_traits<DomainT>::cell_type                   cell_type;
   typedef typename gsse::domain_traits<DomainT>::cell_iterator               cell_iterator;
   typedef typename gsse::domain_traits<DomainT>::segment_on_vertex_iterator  segment_on_vertex_iterator;
   typedef typename gsse::domain_traits<DomainT>::vertex_on_cell_iterator     vertex_on_cell_iterator;
   typedef typename gsse::domain_traits<DomainT>::cell_on_vertex_iterator     cell_on_vertex_iterator;
   typedef typename gsse::domain_traits<DomainT>::point_t                     point_t;
   typedef typename gsse::domain_traits<DomainT>::vertex_handle               vertex_handle;
   typedef typename gsse::domain_traits<DomainT>::storage_type                storage_type;
   typedef typename gsse::domain_traits<DomainT>::vertex_type                 vertex_type;
   typedef typename gsse::domain_traits<DomainT>::segment_vertex_quantity_iterator   vertex_quan_iterator;
   typedef typename point_t::numeric_t                                        numeric_t;

   static const unsigned long DIM = gsse::domain_traits<DomainT>::dimension_tag::dim;

   // -----------------------------------------------------------------
   //
   // create search range
   //
   std::vector<vertex_handle> vh_container;
   point_t radius_p (1e-4, 1e-4);

   point_t point_dl  ;
   point_t point_ur  ;
   
   point_dl = thepoint - radius_p ;
   point_ur = thepoint + radius_p ;
   vh_container = domain.query_pointcontainer(point_dl, point_ur);

   // ===============================================================================
   //   PART I:
   // 
   //  FAST TRACK .. assumption:  old point is the same as the new point
   //    copy the old value..   
   //

   if (vh_container.size() == 1)
   {
#ifdef DEBUG
      std::cout << " RH smart interpolation cache.. on.. " << std::endl;
#endif
      storage_type value(1,1,0);
      try
      {
         domain.retrieve_quantity_direct(vh_container[0], quan_name, value);
      }
      catch(...)
      {
         std::cout << " PI: .. quantity ("<< quan_name << ") and value NOT  found"<< std::endl;
      }
      return value(0,0);
   }
   



   // ===============================================================================
   //   PART II:
   //
   //   SLOW TRACK  .. assumption: point is new.. find the domain_old::cell containing the domain_new::vertex
   // extend the search area until vertices are found..
   //
   double scaling      = 1.0;
   long loop_ctr       = 0;
   long loop_detector  = 100;

   while ( (vh_container.size() == 0) && (++loop_ctr < loop_detector))
   {
      scaling *= 10;
      point_t point_dl = thepoint - radius_p*scaling ;
      point_t point_ur = thepoint + radius_p*scaling ;
      vh_container = domain.query_pointcontainer(point_dl, point_ur);

#ifdef DEBUG
      std::cout << "pdl: " << point_dl << std::endl;
      std::cout << "pur: " << point_ur << std::endl;
      std::cout << "part II:: size of container: "<< vh_container.size() << std::endl;
#endif
   }
   if (loop_ctr == loop_detector)
   {
      std::cout << "### endless loop detection in PART II of interpolate point .. " << std::endl;
		exit(-1);
   }

#ifdef DEBUG
	std::cout << "### proceeding .." << std::endl;
#endif

   //    this is only used to extract the domain information..
   //
   segment_iterator seg_it=domain.segment_begin();
   

   // ======================   check the found area points =================
   // 
   bool point_found;
   for (size_t it= 0; it < vh_container.size(); ++it)
   {
      for (segment_on_vertex_iterator sov_it(domain, vh_container[it]); sov_it.valid(); ++sov_it)
      {
         // check all adjacent cells
         //
         point_found = false;

        	cell_on_vertex_iterator cov_it( vertex_type( (*(*sov_it)).retrieve_topology(), vh_container[it])  );

         for (; cov_it.valid(); ++cov_it)
         {
            boost::array<point_t,DIM+1> simplex_points;
            long cnt=0;
            for(vertex_on_cell_iterator voc_it(*cov_it); voc_it.valid(); ++voc_it, ++cnt) 
            {
               simplex_points[cnt] = domain.get_point(*voc_it);
               //std::cout << "simplex_point: " << simplex_points[cnt] << std::endl;
            }
            
            short retval = orient2D_triangle(simplex_points[0],
                                             simplex_points[1],
                                             simplex_points[2], thepoint);
               //std::cout << "retval: " << retval << std::endl;
                //std::cout << std::endl;
            
            // 2: outside
            //
            if (retval == 0 ||  
                retval == 1 ||
                retval == 3  )
            {
               point_found = true;
                 //std::cout << " point is found.. I have the cell iterator.. " << std::endl;
               break;
            }
         }

     
         
         if (point_found)   // a cell was found that contains the searched vertex
         {
            cell_type found_cell = *cov_it;
            
            // weights and quantity values
            //
            typedef double numeric_quan_t;
            boost::array<point_t,DIM+1>          simplex_points;
            boost::array<numeric_quan_t, DIM+1>  values;
            
				//
				// [JW] I've deactivated this array, as it's of no use --> comp warning ;-)
				//
				//double array[DIM+1];
            
            long cnt = 0;
            for(vertex_on_cell_iterator voc_it(found_cell); voc_it.valid(); ++voc_it, ++cnt) 
            {
               simplex_points[cnt] = domain.get_point(*voc_it);

               storage_type value(1,1,0);
               try
               {
						//std::cout << " trying to access the quantity .." << std::endl;
                  domain.retrieve_quantity(*voc_it, quan_name, value);
                  values[cnt] = value(0,0);
               }
               catch(...)
               {
                  std::cout << "  PII: .. quantity ("<< quan_name << ") and value NOT  found"<< std::endl;
               }
            }
            
            
            std::vector<numeric_t> weights;
            weights = computeInterpolationWeights2d(thepoint, simplex_points);
            
            
            // interpolator .. select between linear / log
            // for NOW .. only log
            // [RH][TODO] .. make it decideable..
            // 
            numeric_quan_t value ;
            value = interpolate_log( weights.begin(), weights.end(),
                                     values.begin(),  values.end()  );
            
            return value;
         }
      }
   }
#ifdef DEBUG
	std::cout << "### part III starting up .." << std::endl;
#endif
   // ===============================================================================
   //   PART III:
   //
   // no cell was found.. possible BUG or inconsistent mesh interface
   // for the sake of robustness -> we use the closest point and use the quantity thereof
   //

//    std::cout << "PART III .. no cell was found.. there are various reasons for that.. " << std::endl;
//    std::cout << "part III:: size of container: "<< vh_container.size() << std::endl;
//    std::cout << "           the point: " << thepoint << std::endl;

   // resize the point one more time  to always get more than one point
   //
   loop_ctr       = 0;
   scaling = 1.0;

   while ( (vh_container.size() < 5) && (++loop_ctr < loop_detector))
   {
      scaling *= 10;
      point_t point_dl = thepoint - radius_p*scaling ;
      point_t point_ur = thepoint + radius_p*scaling ;
      vh_container = domain.query_pointcontainer(point_dl, point_ur);
#ifdef DEBUG
      std::cout << "part III:: size of container: "<< vh_container.size() << std::endl;
#endif
   }
   if (loop_ctr == loop_detector)
   {
      std::cout << "### endless loop detection in PART III of interpolate point .. " << std::endl;
		exit(-1);
   }


   typedef double numeric_quan_t;
   numeric_quan_t backup_value(NAN);   //initialize it with NAN -> to detect problems /// development version
   double distance(1e30);
   for (size_t it= 0; it < vh_container.size(); ++it)
   {
      for (segment_on_vertex_iterator sov_it(domain, vh_container[it]); sov_it.valid(); ++sov_it)
      {
         point_t distance_vec = thepoint - domain.get_point_fromhandle( vh_container[it] );
         if (distance_vec.len() < distance)
         {
            storage_type value(1,1,0);
            try
            {
               domain.retrieve_quantity(vertex_type( (*(*sov_it)).retrieve_topology(), vh_container[it]), quan_name, value);
               backup_value = value(0,0);
            }
            catch(...)
            {
               std::cout << "  PIII: .. quantity ("<< quan_name << ") and value NOT  found"<< std::endl;
            }
            
            distance = distance_vec.len();
         }
      }
   }
   

   return backup_value;
}


// =============================================
//
// main interpolation procedure
//
template<typename DomainT>
void interpolate(DomainT& domain_old, DomainT& domain_new, std::string quan_name)
{
   typedef typename gsse::domain_traits<DomainT>::segment_iterator         segment_iterator;
   typedef typename gsse::domain_traits<DomainT>::vertex_type              vertex_type;
   typedef typename gsse::domain_traits<DomainT>::vertex_handle            vertex_handle;
   typedef typename gsse::domain_traits<DomainT>::vertex_iterator          vertex_iterator;
   typedef typename gsse::domain_traits<DomainT>::cell_iterator            cell_iterator;
   typedef typename gsse::domain_traits<DomainT>::vertex_key_iterator      vertex_key_iterator;
   typedef typename gsse::domain_traits<DomainT>::storage_type        storage_type;
   typedef typename gsse::domain_traits<DomainT>::point_t                  point_t;
   typedef typename gsse::domain_traits<DomainT>::edge_on_cell_iterator    eoc_it;
   typedef typename gsse::domain_traits<DomainT>::cell_on_edge_iterator    coe_it;
   typedef typename gsse::domain_traits<DomainT>::vertex_on_cell_iterator  voc_it;
   typedef typename point_t::numeric_t                                     numeric_t;

   //
   // =================================
   // requirement:: all segment information (quantities) has to be available
   // start the real interpolation
   // =================================
   //
   for (segment_iterator seg_it = domain_new.segment_begin(); seg_it != domain_new.segment_end(); ++seg_it)
   {
      
      // ================================================
      //
      // first, check if the necessary interpolation quantity is on the current segment
      //
      bool quan_on_segment=false;
      for (vertex_key_iterator vki=(*seg_it).vertex_key_begin(); vki != (*seg_it).vertex_key_end(); ++vki)
      {
         if ((*vki) == quan_name) 
         {
            // quantity found on segment
            //
            quan_on_segment = true;
            break;
         }
      }
      if (!quan_on_segment)    //quantity not found on segment
      {
         continue; // next segment...
      }
      
      // ================================================
      //
      // here we have the necessary interpolation quantity on the segment
      //
      std::cout << "  ## Interpolation of segment: " << (*seg_it).get_name() << std::endl;

      // ================================================
      //
      // walk through all geometrical segment points of the new segments and interpolate the 
      //   given interpolation quantity
      //
      for (vertex_iterator vit = (*seg_it).vertex_begin(); vit != (*seg_it).vertex_end(); ++vit)
      {
         point_t testpoint(domain_new.get_point(*vit));
			

         //  ##########################
         //                                                                    xxxxxxxx the interpolation quantity name
         double interpolated_value = interpolate_point(domain_old, testpoint, quan_name);
         //
         //  ##########################


         // here we have to check if there was something wrong with the interpolation
         //
         if (std::isnan(interpolated_value))
         {
            std::cout << "### error..not a number..############# " << std::endl;
            interpolated_value = -1e10;
         }
         
         // finally, create  new quantity on this vertex
         //
         (domain_new).store_quantity( *vit, quan_name, storage_type(1,1,interpolated_value));
//          std::cout << "   val: " << interpolated_value << std::endl;
//          std::cout << std::endl;
      }
   }


}


} // namespace gsse

#endif
