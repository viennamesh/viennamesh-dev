/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at                     

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_INTERFACE_GSSE_TOOLBOX
#define GML_INTERFACE_GSSE_TOOLBOX

//
// *** GSSE includes
//
#include <gsse/datastructure/domain_algorithms.hpp>
//
// *** BOOST includes
//
#include <boost/mpl/int.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/less.hpp>
#include <boost/utility/enable_if.hpp>
//
// ===================================================================================
//
namespace gml {
/*
template <  int GeomDIM, 
            int TopoDIM, 
            int QuanDIM           = 0, 
            typename NumericT     = double,
            typename CTArray      = gsse::array< NumericT, GeomDIM >,
            typename CellTopology = gsse::cell_simplex >
struct create_gsse_domain
{
   typedef NumericT  StorageTPosition;
   typedef NumericT  StorageTQuantity;
   typedef CTArray   CoordPosT;
  
   typedef typename gsse::cellcomplex::create_index_dense::template apply<TopoDIM, CellTopology>::type      CellComplex;

   typedef gsse::fiberbundle::collection<
      QuanDIM, gsse::cell_nothing
      , boost::fusion::pair<gsse::access_specifier::AT_vx, gsse::fiberbundle::dforms::FB_Nform>    
      , boost::fusion::pair<gsse::access_specifier::AT_ee, gsse::fiberbundle::dforms::FB_Nform>
      , boost::fusion::pair<gsse::access_specifier::AT_f0, gsse::fiberbundle::dforms::FB_Nform>
      , boost::fusion::pair<gsse::access_specifier::AT_cl, gsse::fiberbundle::dforms::FB_Nform>
      > QuanComplex;

   typedef typename gsse::container::create< CoordPosT >::type  FBPosT;  
   typedef typename gsse::container::create_index_dense< CellComplex, std::vector, std::string >::type   SpaceTopologySegmentsT;
   typedef typename gsse::container::create_index_dense< QuanComplex, std::vector, std::string >::type   SpaceQuantitySegmentsT;
  
   typedef boost::fusion::map<
      boost::fusion::pair<gsse::access_specifier::AP, FBPosT>
    , boost::fusion::pair<gsse::access_specifier::AC, SpaceTopologySegmentsT>
    , boost::fusion::pair<gsse::access_specifier::AQ, SpaceQuantitySegmentsT>
      > type;   
};
*/

//
// ===================================================================================
//
template<typename DomainT>
void move_domain(DomainT & domain_source, 
                 DomainT & domain_target)
{
   using namespace gsse::access_specifier;

   typedef gsse::property_domain<DomainT>                                                 GSSE_Domain;
   typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain>::type   SpaceTopologySegmentsT;
   typedef typename gsse::result_of::property_FBPosT<GSSE_Domain>::type                   FBPosT;

   SpaceTopologySegmentsT& segments_topology_source = gsse::at_dim<AC>(domain_source);
   FBPosT&                 geometry_source          = gsse::at_dim<AP>(domain_source);
      
   SpaceTopologySegmentsT& segments_topology_target = gsse::at_dim<AC>(domain_target);
   FBPosT&                 geometry_target          = gsse::at_dim<AP>(domain_target);
   //
   // RESET TARGET 
   //
   reset_domain(domain_target);
   // 
   // TRANSFER GEOMETRY
   //
   for(long pi = 0; pi < gsse::size(gsse::at_dim<AP>(domain_source)); ++pi)
   {
      gsse::pushback(geometry_target) = gsse::at(pi)(geometry_source); 
   }
   // 
   // TRANSFER TOPOLOGY
   //
   gsse::resize(gsse::size(segments_topology_source))(segments_topology_target);
   for (long si = 0; si < gsse::size(segments_topology_source); ++si)
   {
      gsse::at_index(gsse::at(si)(segments_topology_target)) = gsse::at_index(gsse::at(si)(segments_topology_source));
           
      for ( long ci = 0; ci < gsse::size_cl(gsse::at(si)(segments_topology_source)); ++ci)
      {      
         gsse::pushback(gsse::at_dim<AT_cl>(
          gsse::at_fiber(gsse::at(si)(segments_topology_target)))) = 
            gsse::at_cl(ci)(gsse::at(si)(segments_topology_source));    
      }
   }   
   //
   // FINALIZE TARGET TOPOLOGY
   //
   finalize_topology(domain_target); 
   //
   // RESET SOURCE 
   //
   reset_domain(domain_source);
}                    
//
// ===================================================================================
//
template<typename DomainT>
void reset_domain(DomainT& domain)
{
   using namespace gsse::access_specifier;

   typedef gsse::property_domain<DomainT>                                                 GSSE_Domain;
   typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain>::type   SpaceTopologySegmentsT;

   SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(domain);
   // 
   // RESET GEOMETRY
   //
   gsse::at_dim<AP>(domain).clear();
   //
   // RESET TOPOLOGY
   //
   for (long si = 0; si < gsse::size(segments_topology); ++si)
   {   
      gsse::at_dim<AT_vx>(gsse::at_fiber((gsse::at(si)(segments_topology)))).clear();
      gsse::at_dim<AT_ee>(gsse::at_fiber((gsse::at(si)(segments_topology)))).clear();
      gsse::at_dim<AT_cl>(gsse::at_fiber((gsse::at(si)(segments_topology)))).clear();
   }
}
//
// ===================================================================================
//

template<typename Domain>
void create_faces(Domain& domain,
                  typename boost::enable_if< 
                     typename boost::mpl::equal_to< 
                        boost::mpl::int_< gsse::result_of::at_dim_index< 
                           typename gsse::result_of::at_dim<Domain, gsse::access_specifier::AC>::type>::type::DIM_cc>,
                        boost::mpl::int_<3> 
                     >::type 
                  >::type* dummy = 0)
{

   using namespace gsse::access_specifier;

   typedef typename gsse::result_of::at_dim<Domain, AC>::type                       SpaceTopologySegmentsT;
   typedef typename gsse::result_of::at_dim_index<SpaceTopologySegmentsT>::type     CellComplex;
   typedef typename CellComplex::CellT                                              CellTopology;

   static const long DIM = CellComplex::DIM_cc;
   
   SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(domain);   

   typedef gsse::property_data<DIM, CellTopology>                                   GSSE_Data;
   typedef typename gsse::result_of::facet_on_cell<GSSE_Data>::type                 FacetOnCell;
   typedef typename gsse::result_of::facet_on_cell<GSSE_Data>::result               FacetOnCell_result;
   typedef typename gsse::result_of::facet_on_cell<GSSE_Data>::result::value_type   FacetT;
   FacetOnCell facet_on_cell;  

   typedef std::set< FacetT >             container_facets_t;

   for (long si = 0; si < gsse::size (segments_topology ); ++si)
   {

      container_facets_t                     container_facets;
      typename container_facets_t::iterator  ci_it;
      
      for ( long ci = 0; ci < gsse::size_cl( gsse::at(si) (segments_topology) ); ++ci)
      {
          FacetOnCell_result cont_f0 = 
            facet_on_cell ( gsse::at(ci) (gsse::at_dim<AT_cl>( gsse::at_fiber((gsse::at(si) (segments_topology)))) ) );

         for (long cf_i = 0; cf_i < gsse::size(cont_f0); ++cf_i)
            container_facets.insert ( cont_f0[cf_i] );
      }
      
      for (ci_it = container_facets.begin();
           ci_it != container_facets.end(); 
           ++ci_it)
      {
         gsse::pushback ( gsse::at_dim<AT_f0>( gsse::at_fiber((gsse::at(si) (segments_topology))))) = (*ci_it);
      }
   }

}

template<typename Domain>
void create_faces(Domain& domain,
                  typename boost::enable_if< 
                     typename boost::mpl::less< 
                        boost::mpl::int_< gsse::result_of::at_dim_index< 
                           typename gsse::result_of::at_dim<Domain, gsse::access_specifier::AC>::type>::type::DIM_cc>,
                        boost::mpl::int_<3> 
                     >::type 
                  >::type* dummy = 0)
{ }

//
// ===================================================================================
//
//
// ===================================================================================
//
template <typename DomainT>
void finalize_topology(DomainT& domain)
{
   using namespace gsse::access_specifier;

   typedef gsse::property_domain<DomainT>                                                             GSSE_Domain;   
   typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain>::type               SpaceTopologySegmentsT;
   typedef gsse::property_space<DomainT>                                                              GSSE_Space;   
   typedef typename gsse::result_of::property_CellComplex<GSSE_Space>::type                           CellComplex; 
   typedef typename gsse::result_of::at_dim_isd<CellComplex, gsse::access_specifier::AT_cl>::type     CellT;      
   typedef typename gsse::result_of::at_dim_isd<CellComplex, gsse::access_specifier::AT_vx>::type     VertexT;      

   SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(domain);

   long   segment_size = segments_topology.size();
   // --------------------------------------------
   for (long si = 0; si < segment_size; ++si)
   {   
      //
      // RESET VERTEX && EDGE CONTAINER
      //
      //   brief: in case finalize_topology() has been called on an previously finalized domain
      //          we do not want this function to append on the previous filled container - we want
      //          to create the containers from scratch to ensure consistancy ..
      //
      if(gsse::size(gsse::at_dim<AT_vx>(gsse::at_fiber((gsse::at(si)(segments_topology))))) > 0)
         gsse::at_dim<AT_vx>(gsse::at_fiber((gsse::at(si)(segments_topology)))).clear();
      if(gsse::size(gsse::at_dim<AT_ee>(gsse::at_fiber((gsse::at(si)(segments_topology))))) > 0)
         gsse::at_dim<AT_ee>(gsse::at_fiber((gsse::at(si)(segments_topology)))).clear();   
      //
      // vertex_con_t --> to impose uniqueness on the segment vertices
      //
      typedef std::map<long, bool>      vertex_con_t;
      //
      // vertex_map_t --> to bridge between the dense vertex container elements
      //                  and the sparse real vertex indices
      //
      typedef std::map<long, long>    vertex_map_t;
      vertex_con_t      vertex_con;   
      vertex_map_t      vertex_map;      

      long cellsize = gsse::size_cl( gsse::at(si) (segments_topology) );

      //
      // SETUP UNIQUE VERTEX CONTAINER
      //
      for (long ci = 0 ; ci < cellsize; ++ci)
      {
         CellT cell = gsse::at(ci) (gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology)))   );
      
         for (unsigned int voci = 0; voci < cell.size(); ++voci)    
         {
            if (!vertex_con[ cell[voci] ])
               vertex_con[ cell[voci] ] = true;
         }           
      }
      //
      // RESIZE VERTEX CONTAINER OF THE ACTUAL SEGMENT ACCORDING TO THE UNIQUE VERTEX CONTAINER
      //      
      gsse::resize(gsse::size(vertex_con))(gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(si)(segments_topology))));
      //
      // SETUP TRANSFER UNIQUE VERTEX DATASET TO THE SEGMENT VERTEX CONTAINER
      //        
      long   vindex = 0;
      for(vertex_con_t::iterator viter = vertex_con.begin(); viter != vertex_con.end(); ++viter)
      {
         gsse::at_index(gsse::at(vindex)(gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(si)(segments_topology))))) = (*viter).first; 
         //
         // setup index space link: 
         //
         //       key:     real vertex index
         //       value:   base space index
         //        
         vertex_map[(*viter).first] = vindex;         
         vindex++;
      }
      //
      // SETUP CELL ON VERTEX RELATION
      //
      for (long ci = 0 ; ci < cellsize; ++ci)
      {
         CellT cell = gsse::at(ci) (gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology)))   );      
         for (unsigned int voci = 0; voci < cell.size(); ++voci)    
         {
            gsse::pushback // store on the fiber (vector<long>)
            ( 
               gsse::at_fiber // access the fiber of this vertex
               (
                  //
                  // use index space link to derive the vertex container position(aka base space index) 
                  // for a given real vertex index out of the sparse vertex index space
                  //                |
                  //                v
                  gsse::at(vertex_map[gsse::at(voci)(cell)]) //  access the vertex container element, a certain vertex
                  (gsse::at_dim<AT_vx>( gsse::at_fiber(gsse::at(si)(segments_topology)) ) ) // <- vertex container
               ) 
            ) = ci;
         }        
      }
   }

   //
   // CREATE EDGES
   //
   gsse::create_edges(domain);
   //
   // CREATE FACES
   //
   create_faces(domain);

   for (long si = 0; si < segment_size; ++si)
   {  
   #ifdef DEBUG_TOOLBOX           
      std::cout << "\tsegment #: " << si << std::endl;
      std::cout << "\t\tvertexsize: \033[1;32m"
         << gsse::size(gsse::at_dim<AT_vx>( gsse::at_fiber((gsse::at(si) (segments_topology))) )) 
         << "\033[0m" << std::endl;
      std::cout << "\t\tcellsize: \033[1;32m"  
         << gsse::size(gsse::at_dim<AT_cl>( gsse::at_fiber((gsse::at(si) (segments_topology))) )) 
         << "\033[0m" << std::endl;
      std::cout << "\t\tedgesize: \033[1;32m"  
         << gsse::size(gsse::at_dim<AT_ee>( gsse::at_fiber((gsse::at(si) (segments_topology))) )) 
         << "\033[0m" << std::endl;
   #endif
   }

}

} // end namespace: gml

#endif
