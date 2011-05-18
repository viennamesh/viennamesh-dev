/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_RUNTIME_HH_ID
#define GSSE_RUNTIME_HH_ID

// *** system includes
//
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <sstream>

// *** GSSE includes
//
#include "gsse/domain.hpp"


// virtual base class for compile time domain to make it
//   runtime changeable
//
struct runtime_domain
{
   void          virtual read_file(std::string file_name) = 0;
   void          virtual write_file(std::string file_name) = 0;
   std::string   virtual get_quantity(std::string object_str, long quan_id) = 0;
   long          virtual get_quantity_number(std::string object_str) = 0 ;

   long          virtual get_object_size(std::string object_str) = 0;
   void          virtual reset_edge_it()  =0 ;
   void          virtual reset_facet_it() =0;
   void          virtual increment_edge_it(long object_id=0) = 0; 
   void          virtual increment_facet_it(long object_id=0)  =0; 

   long 	 virtual get_number_voc() = 0;
   long          virtual voc_iteration(long voc_cnt) = 0;
   long          virtual dimension_topology() = 0;
   long          virtual dimension_geometry() = 0;

   double        virtual get_value(std::string object_str, long object_id, std::string quan_name) = 0;
   void          virtual set_segment_iter() = 0;
	
   std::vector<double>  virtual get_point(long object_id) = 0;
   
   virtual ~runtime_domain()  {}
	
};


template<typename DomainT>
struct runtime_domain_generic : public runtime_domain
{
   typedef typename gsse::domain_traits<DomainT>::storage_type      storage_type;
   typedef typename gsse::domain_traits<DomainT>::vertex_type       vertex_type;
   typedef typename gsse::domain_traits<DomainT>::edge_type         edge_type;
   typedef typename gsse::domain_traits<DomainT>::edge_handle       edge_handle;
   typedef typename gsse::domain_traits<DomainT>::facet_type        facet_type;
   typedef typename gsse::domain_traits<DomainT>::cell_type         cell_type;
   typedef typename gsse::domain_traits<DomainT>::segment_iterator  segment_iterator;
   
   typedef typename gsse::domain_traits<DomainT>::edge_iterator     edge_iterator;
   typedef typename gsse::domain_traits<DomainT>::facet_iterator    facet_iterator;
   
   typedef typename gsse::domain_traits<DomainT>::vertex_on_cell_iterator  vertex_on_cell_iterator;
   
   typedef typename gsse::domain_traits<DomainT>::vertex_key_iterator      vertex_key_iterator;
   typedef typename gsse::domain_traits<DomainT>::edge_key_iterator        edge_key_iterator;
   typedef typename gsse::domain_traits<DomainT>::facet_key_iterator       facet_key_iterator;
   typedef typename gsse::domain_traits<DomainT>::cell_key_iterator        cell_key_iterator;
  
   // geometry
   //
   typedef typename gsse::domain_traits<DomainT>::point_t                   point_type;

   runtime_domain_generic()
      {
	domain_dimension_geom      = DomainT::dimension_topology;
	domain_dimension_topology  = DomainT::dimension_geometry;
      	number_voc                 = -1;
      }
   virtual ~runtime_domain_generic()  {}
   
   void read_file(std::string file_name)
      {
         domain.read_file(file_name);
         for (seg_it = domain.segment_begin(); seg_it != domain.segment_end(); ++seg_it)
         {
            (*seg_it).retrieve_topology().build_up_edge_container();
            (*seg_it).retrieve_topology().build_up_facet_container();
         }
         seg_it = domain.segment_begin();

	 ee_it  = (*seg_it).edge_begin();
	 ft_it  = (*seg_it).facet_begin();
      }	
   void write_file(std::string file_name)
      {
         domain.write_file(file_name);
      }


   long get_object_size(std::string object_str)
      {
         if (object_str == "vertex")
         {
            return (*seg_it).vertex_size();
         }
         else if (object_str == "edge")
         {
            return (*seg_it).edge_size();
         }
         else if (object_str == "facet")
         {
            return (*seg_it).facet_size();
         }
         else if (object_str == "cell")
         {
            return (*seg_it).cell_size();
         }

         // in case of errors
         //
         return -1;
      }

   long get_quantity_number(std::string object_str)
      {
         if (object_str == "vertex")
         {
            return (*seg_it).vertex_key_size();
         }
         else if (object_str == "edge")
         {
            return (*seg_it).edge_key_size();
         }
         else if (object_str == "facet")
         {
            return (*seg_it).facet_key_size();
         }
         else if (object_str == "cell")
         {
            return (*seg_it).cell_key_size();
         }

         // in case of errors
         //
         return -1;
         
      }   

   std::string get_quantity(std::string object_str, long quan_id)
      {

         if (object_str == "vertex")
         {
            long quan_ctr = 0;
            for (vertex_key_iterator 
                    object_it = (*seg_it).vertex_key_begin(); 
                 object_it != (*seg_it).vertex_key_end(); ++object_it, ++quan_ctr)
            {
               if ( quan_ctr == quan_id )
                  return *object_it;
            }
         }
         else if (object_str == "edge")
         {
            long quan_ctr = 0;
            for (edge_key_iterator 
                    object_it = (*seg_it).edge_key_begin(); 
                 object_it != (*seg_it).edge_key_end(); ++object_it, ++quan_ctr)
            {
               if ( quan_ctr == quan_id )
                  return *object_it;
            }

         }
         else if (object_str == "facet")
         {
            long quan_ctr = 0;
            for (facet_key_iterator 
                    object_it = (*seg_it).facet_key_begin(); 
                 object_it != (*seg_it).facet_key_end(); ++object_it, ++quan_ctr)
            {
               if ( quan_ctr == quan_id )
                  return *object_it;
            }
         }
         else if (object_str == "cell")
         {
            long quan_ctr = 0;
            for (cell_key_iterator 
                    object_it = (*seg_it).cell_key_begin(); 
                 object_it != (*seg_it).cell_key_end(); ++object_it, ++quan_ctr)
            {
               if ( quan_ctr == quan_id )
                  return *object_it;
            }
         }

         // in case of errors
         // 
         return std::string("__empty__");
      }
  
   void reset_edge_it()   { ee_it = (*seg_it).edge_begin(); }
   void reset_facet_it()  { ft_it = (*seg_it).facet_begin(); }
   void increment_edge_it(long object_id)   { ++ee_it; }
   void increment_facet_it(long object_id)  { ++ft_it; }

   double get_value(std::string object_str, long object_id, std::string quan_name)
      {
         storage_type return_st(1,1,0);
         
         if (object_str == "vertex")
         {
            vertex_type vertex((*seg_it).retrieve_topology(), object_id );
            domain.retrieve_quantity(vertex, quan_name, return_st);	
         }
         else if (object_str == "edge")
         {
            edge_type edge( *ee_it );
            domain.retrieve_quantity(edge, quan_name, return_st);	
         }
         else if (object_str == "facet")
         {
            facet_type facet( *ft_it );
            domain.retrieve_quantity(facet, quan_name, return_st);	
         }
         else if (object_str == "cell")
         {
            cell_type cell ((*seg_it).retrieve_topology(), object_id );
            domain.retrieve_quantity(cell, quan_name, return_st);	
         }
         return return_st(0,0);
      }
   
   void set_segment_iter()
      {
         seg_it = domain.segment_begin();
      }
   void set_segment(long segment_ctr)
      {
         for (long i = 0; i < segment_ctr ; ++i)
            ++seg_it;

	 ee_it = (*seg_it).edge_begin();
	 ft_it = (*seg_it).facet_begin();
      }
   void next_segment()
      {
         ++seg_it;
      
         ee_it = (*seg_it).edge_begin();
	 ft_it = (*seg_it).facet_begin();
      }

   long get_number_voc()
   {
   	if (number_voc == -1)
	{
	
	 long temp_number_voc = 0;
         for (vertex_on_cell_iterator voc_it(*((*domain.segment_begin()).cell_begin()));
	 	voc_it.valid(); ++voc_it, ++ temp_number_voc);
	 number_voc = temp_number_voc; 

	}
	return number_voc;
   }

   long voc_iteration(long voc_cnt)
   {
	 long temp_number_voc = 0;
         for (vertex_on_cell_iterator voc_it(*((*domain.segment_begin()).cell_begin()));
	 	voc_it.valid(); ++voc_it, ++ temp_number_voc)
		{
			if (temp_number_voc == voc_cnt)
				return (*voc_it).handle();
		}
	
	return -1;
   }



 	long dimension_topology()
	{
		return domain_dimension_topology;
	}
 

 // geometry
 //
 	long dimension_geometry()
	{
		return domain_dimension_geom;
	}
 	
	std::vector<double> get_point(long object_id)
 	{	
		std::vector<double> coord;
		point_type pt = domain.get_point(vertex_type((*seg_it).retrieve_topology(), object_id));
		for (long dim = 0 ; dim < domain_dimension_geom; ++dim)
		{
			coord.push_back(pt[dim]);
		}
		return coord;
	}

protected:
  DomainT            domain;
  segment_iterator   seg_it;
  edge_iterator      ee_it;
  facet_iterator     ft_it;

  long number_voc;
  long domain_dimension_geom;
  long domain_dimension_topology;
};



#endif
