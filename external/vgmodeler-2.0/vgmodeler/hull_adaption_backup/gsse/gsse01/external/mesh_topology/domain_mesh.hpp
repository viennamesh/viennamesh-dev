/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_NETGENDOMAIN_HH_ID
#define GSSE_NETGENDOMAIN_HH_ID

// *** GSSE includes
//
#include "external/mesh_topology/topology_interface_netgen.hpp"
#include "gsse/topology.hpp"
#include "external/mesh_topology/unstructured_domain_mesh.hpp"

// ** BOOST includes
//
#include <boost/mpl/if.hpp>




namespace gsse
{
  
// ============================================================================================
// *** this class is a get - metafunction to obtain the correct
// *** domain 
//
// ############

template<typename TopologyT, 
         typename NumericTCoordinates, 
         typename NumericTQuantity ,
	 unsigned int DIM=topology_traits<TopologyT>::dimension> 
struct get_domain_mesh
{
   typedef typename get_segment <TopologyT, NumericTQuantity>::type                              segment_t;
   typedef typename detail_domain::unstructured_domain_mesh<NumericTCoordinates, segment_t,DIM>   type;

//    typedef typename
//    boost::mpl::if_
//    <
//      typename boost::is_same<typename topology_traits<TopologyT>::topology_tag, structured_topology_tag>, 
//      detail_domain::unstructured_domain<NumericTCoordinates, segment_t,DIM>,
//      detail_domain::unstructured_domain_mesh<NumericTCoordinates, segment_t,DIM>
//       > :: type
   
//    type;  


};

// ============================================================================================
// *** The domain_traits class as well as the get_domain
// *** metafunction are the only connections to the exterior.
//
// ############
   
template <typename DomainT> 
struct assign_voronoi_information_structured;
   
template <typename DomainT> 
struct assign_voronoi_information_unstructured_2d;
template <typename DomainT> 
struct assign_voronoi_information_unstructured_3d;
  
template <typename MeshTag, typename DimensionTag, typename DomainT>
struct voronoi_info
{
   typedef assign_voronoi_information_unstructured_2d<DomainT> vor2du;
   typedef assign_voronoi_information_structured<DomainT>      vornds;     
   typedef assign_voronoi_information_unstructured_3d<DomainT> vor3du;

   typedef typename boost::mpl::if_<boost::is_same<DimensionTag, mesh_dim_tag<3> >, vor3du, vor2du>::type unstr_vor;
   typedef typename boost::mpl::if_<boost::is_same<MeshTag, structured_topology_tag >, vornds, unstr_vor>::type type;
};


template<typename DomainT> 
struct domain_traits : public segment_traits<typename DomainT::segment_t>
{
  typedef typename DomainT::segment_t                         segment_t;   
  //typedef typename DomainT::segment_t                         segment_type;
  //typedef typename DomainT::const_segment_iterator           const_segment_iterator;


//   typedef typename DomainT::segment_iterator                  segment_iterator;
  typedef typename DomainT::point_t                           point_t;
  typedef typename segment_traits<segment_t>::topology_tag    topology_tag;
  typedef typename segment_traits<segment_t>::dimension_tag   dimension_tag;
  typedef typename DomainT::global_point_iterator		global_point_iterator;
  

  static const unsigned int dimension_geometry  = DomainT::dimension_geometry;


  typedef typename voronoi_info<topology_tag, dimension_tag, DomainT>::type voronoi_t;
};


// ============================================================================================
// *** Filename parser
//
// ############

enum topology_type_e {unstructured, structured, undefined};

struct file_information
{
   long            dimension;
   topology_type_e tt;

   friend std::ostream& operator<<(std::ostream& ostr, const file_information fi)
   {
      ostr << "fi dimension: " << fi.dimension << "  topology type: ";
      if (fi.tt == unstructured)
         ostr << " unstructured";
      else if (fi.tt == structured)
         ostr << " structured";
      ostr << std::endl;
      return ostr;
   }
};

file_information get_file_information(std::string filename)
{
   file_information fi;
   fi.dimension=-1;
   fi.tt = undefined;

   //  ..............

   std::string file_ending;
   std::string::size_type pos = filename.rfind(".")+1;
   //std::cout << "  filename: " << filename.size() << std::endl;
   //std::cout << "pos: "<< pos << std::endl;
   file_ending = filename.substr(pos); 

   if (file_ending == "ele")
   {
      fi.tt = unstructured;
      fi.dimension=2;
   }

   if (file_ending[2] =='u' )
      fi.tt = unstructured;
   else if (file_ending[2] =='s' )
      fi.tt = structured;

   if (file_ending[3] == '1')
      fi.dimension=1;
   else if (file_ending[3] =='2')
      fi.dimension=2;
   else if (file_ending[3] =='3')
      fi.dimension=3;
   else if (file_ending[3] =='4')
      fi.dimension=4;

   return fi;
}


}



#endif

