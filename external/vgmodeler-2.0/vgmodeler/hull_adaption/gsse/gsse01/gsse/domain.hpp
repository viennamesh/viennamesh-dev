/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_DOMAIN_HH_ID
#define GSSE_DOMAIN_HH_ID

// ** BOOST includes
//
#include <boost/mpl/if.hpp>

// *** GSSE includes
//
#include "gsse/topology.hpp"
#include "gsse/segment.hpp"

#include "gsse/topology/unstructured_topology.hpp"
#include "gsse/topology/structured_topology.hpp"
#include "gsse/domain/unstructured_domain.hpp"
#include "gsse/domain/structured_domain.hpp"
 
namespace gsse
{

// ###########################################################################################################
// ###########################################################################################################
//
// *** this class is a get - metafunction to obtain the correct
// *** domain 
//
// ############

template<typename TopologyT, 
         typename NumericTCoordinates, 
         typename NumericTQuantity,
	 unsigned int DIM=topology_traits<TopologyT>::dimension,
         typename IdentifierTQuantity=std::string > 
struct get_domain
{
   typedef typename get_segment <TopologyT, NumericTQuantity>::type                     segment_t;
   typedef typename get_segment <TopologyT, NumericTQuantity>::type                     segment_type;
   
   typedef typename
   boost::mpl::if_
   <
      typename boost::is_same<typename topology_traits<TopologyT>::topology_tag, structured_topology_tag>, 
      detail_domain::structured_domain<NumericTCoordinates, segment_t, NumericTQuantity, IdentifierTQuantity, DIM  >,
      detail_domain::unstructured_domain<NumericTCoordinates, segment_t, NumericTQuantity, IdentifierTQuantity, DIM >  
   > :: type type;  
};


// ###########################################################################################################
//
// *** The domain_traits class as well as the get_domain
// *** metafunction are the only connections to the exterior.
//
// ############


// forward template declarations
//   
template <typename DomainT> struct voronoi_calculation_simplex_2d;
template <typename DomainT> struct voronoi_calculation_simplex_3d;
template <typename DomainT> struct voronoi_calculation_cuboid;
  

template <typename MeshTag, typename DimensionTag, typename DomainT>
struct voronoi_info
{
   typedef voronoi_calculation_cuboid<DomainT>     vornds;     
   typedef voronoi_calculation_simplex_2d<DomainT> vor2du;
   typedef voronoi_calculation_simplex_3d<DomainT> vor3du;

   typedef typename boost::mpl::if_<boost::is_same<DimensionTag, mesh_dim_tag<3> >, vor3du, vor2du>::type unstr_vor;
   typedef typename boost::mpl::if_<boost::is_same<MeshTag, structured_topology_tag >, vornds, unstr_vor>::type type;
};


template<typename DomainT> 
struct domain_traits : public segment_traits<typename DomainT::segment_t>
{
  typedef typename DomainT::segment_t                           segment_t;   
  typedef typename DomainT::segment_t                           segment_type;
  typedef typename DomainT::segment_iterator                    segment_iterator;
  typedef typename DomainT::segment_on_vertex_iterator          segment_on_vertex_iterator;
  typedef typename DomainT::global_point_iterator		global_point_iterator;
  typedef typename DomainT::point_t                             point_t;
  typedef typename DomainT::quan_numeric_t                      quan_numeric_t;
  typedef typename segment_traits<segment_t>::topology_tag      topology_tag;
  typedef typename segment_traits<segment_t>::dimension_tag     dimension_tag;

  typedef typename segment_traits<segment_t>::data_pattern_type   data_pattern_type;

// [RH][TODO] .. decide.. where the key iterators are taken.
//
//    typedef typename DomainT::vertex_key_iterator                vertex_key_iterator;
//    typedef typename DomainT::edge_key_iterator                  edge_key_iterator;
//    typedef typename DomainT::facet_key_iterator                 facet_key_iterator;
//    typedef typename DomainT::cell_key_iterator                  cell_key_iterator;

   typedef typename segment_traits<segment_t>::segment_vertex_quantity_iterator   vertex_key_iterator  ;
   typedef typename segment_traits<segment_t>::segment_edge_quantity_iterator     edge_key_iterator  ;
   typedef typename segment_traits<segment_t>::segment_facet_quantity_iterator    facet_key_iterator  ;
   typedef typename segment_traits<segment_t>::segment_cell_quantity_iterator     cell_key_iterator  ;


   typedef typename DomainT::domain_key_iterator                                  domain_key_iterator;
   typedef typename voronoi_info<topology_tag, dimension_tag, DomainT>::type      voronoi_t;

   
   static const unsigned int dimension_geometry  = DomainT::dimension_geometry;
   
   
};



//  algorithm for colorize the segments of a domain
//
template<typename DomainT>
void colorize_segments(DomainT& domain)
{
   typedef typename gsse::domain_traits<DomainT>::segment_iterator                       segment_iterator;
   typedef typename gsse::domain_traits<DomainT>::cell_iterator                          cell_iterator;
   typedef typename gsse::domain_traits<DomainT>::vertex_iterator                        vertex_iterator;
   typedef typename gsse::domain_traits<DomainT>::storage_type                      storage_type;

   segment_iterator seg_it;
   long counter = 0;
   for (seg_it = domain.segment_begin(); seg_it != domain.segment_end(); ++seg_it, counter ++)
   {
      cell_iterator cit;
      for (cit = (*seg_it).cell_begin(); cit != (*seg_it).cell_end(); ++cit)
      {
         (*seg_it).store_quantity(*cit, "segment_nr", storage_type(1,1,counter));
      }
   }
   domain.write_dx_file_cell("output_test.dx", "segment_nr");
}




// ============================================================================================
//
// *** Filename parser
//
// ############


//enum topology_type_e {unstructured, structured, undefined};

//struct file_information
//{
//   long            dimension;
//   topology_type_e tt;

//   friend std::ostream& operator<<(std::ostream& ostr, const file_information fi)
//   {
//      ostr << "fi dimension: " << fi.dimension << "  topology type: ";
//      if (fi.tt == unstructured)
//         ostr << " unstructured";
//      else if (fi.tt == structured)
//         ostr << " structured";
//      ostr << std::endl;
//      return ostr;
//   }
//};

//file_information get_file_information(std::string filename)
//{
//   file_information fi;
//   fi.dimension=-1;
//   fi.tt = undefined;

//   //  ..............

//   std::string file_ending;
//   std::string::size_type pos = filename.rfind(".")+1;
//   //std::cout << "  filename: " << filename.size() << std::endl;
//   //std::cout << "pos: "<< pos << std::endl;
//   file_ending = filename.substr(pos); 

//   if (file_ending == "ele")
//   {
//      fi.tt = unstructured;
//      fi.dimension=2;
//   }

//   if (file_ending[2] =='u' )
//      fi.tt = unstructured;
//   else if (file_ending[2] =='s' )
//      fi.tt = structured;

//   if (file_ending[3] == '1')
//      fi.dimension=1;
//   else if (file_ending[3] =='2')
//      fi.dimension=2;
//   else if (file_ending[3] =='3')
//      fi.dimension=3;
//   else if (file_ending[3] =='4')
//      fi.dimension=4;

//   return fi;
//}


}



#endif

