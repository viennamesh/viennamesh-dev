/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at
               Karl Rupp                                rupp@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_TRANSFER_VIENNAGRID_HPP
#define VIENNAMESH_TRANSFER_VIENNAGRID_HPP

#include "viennagrid/domain.hpp"

#include "viennamesh/transfer/transfer_base.hpp"

namespace viennamesh {

#ifndef VIENNAMESH_VIENNAGRID_SUPPORT
#define VIENNAMESH_VIENNAGRID_SUPPORT
namespace tag {
struct viennagrid {};
} // end namespace tag   
#endif

   
template<>   
struct transfer <viennamesh::tag::viennagrid>
{
   template<typename MesherT, typename DomainT>
   void operator()(MesherT& mesher, DomainT& domain)
   {
      typedef typename DomainT::config_type                                                  domain_config_type;

      typedef viennagrid::segment_t<domain_config_type>                                      segment_type;
      
      static const int DIMG = domain_config_type::dimension_tag::value;
      static const int DIMT = domain_config_type::cell_tag::topology_level;     
      
      typedef typename viennagrid::result_of::point_type<domain_config_type>::type           point_type;   
      typedef typename viennagrid::result_of::ncell_type<domain_config_type, 0>::type        vertex_type;   
      typedef typename viennagrid::result_of::ncell_type<domain_config_type, DIMT>::type     cell_type;   
      typedef typename domain_config_type::numeric_type                                      numeric_type;
      typedef typename domain_config_type::cell_tag                                          cell_tag;

      static const int CELLSIZE = viennagrid::subcell_traits<cell_tag, 0>::num_elements;   
      
      typedef typename MesherT::geometry_container_type              mesher_geometry_cont_type;
      typedef typename MesherT::segment_container_type               mesher_segment_container_type;
      typedef typename mesher_segment_container_type::value_type     mesher_cell_container_type;
      
      mesher_geometry_cont_type&     geom_cont = mesher.geometry();
      mesher_segment_container_type& seg_cont  = mesher.topology();
      
      domain.reserve_vertices(geom_cont.size());
   //    vertex_type vertex;
   //    numeric_type coords[DIMG]; 
      point_type pnt;
      long vi = 0;
      for(typename mesher_geometry_cont_type::iterator iter = geom_cont.begin(); 
         iter != geom_cont.end(); iter++)
      {
         for(int i = 0; i < DIMG; i++)
         {
            pnt[i] = (*iter)[i];
         }      
         // TODO make domain.add(vertex) support const& so get rid of the temporary here
         vertex_type vertex(pnt,vi++);  
         domain.add(vertex);
   //       for(int i = 0; i < DIMG; i++)
   //       {
   //          coords[i] = (*iter)[i];
   //       }
   //       vertex.getPoint().setCoordinates(coords);
   //       vertex.setID(vi); vi++;
   //       domain.add(vertex);      
      }

      cell_type cell;
      vertex_type *vertices[CELLSIZE];
      
      domain.create_segments(seg_cont.size());
      unsigned int si_cnt = 0;
      
      // first, count the cells of all segments
      // we need this size to allocate the cell container of viennagrid
      //
      std::size_t cell_cnt = 0;
      for(typename mesher_segment_container_type::iterator si = seg_cont.begin();
         si != seg_cont.end(); si++)
      {
         //domain.segment(si_cnt).reserve_cells( si->size() );
         for(typename mesher_cell_container_type::iterator ci = (*si).begin();
            ci != (*si).end(); ci++)
         {
            cell_cnt++;
         }
      }
      domain.reserve_cells(cell_cnt);
      
      // second, add the mesh topology to the domain
      //
      for(typename mesher_segment_container_type::iterator si = seg_cont.begin();
         si != seg_cont.end(); si++)
      {
         for(typename mesher_cell_container_type::iterator ci = (*si).begin();
            ci != (*si).end(); ci++)
         {
            // extract the cells vertices from the cell provided by the mesher
            for(int i = 0; i < CELLSIZE; i++)
            {
               vertices[i] = &(domain.vertex((*ci)[i]));
            }    
            cell.setVertices(vertices);
            domain.segment(si_cnt).add(cell);
         }
         si_cnt++;
      }
   }
};
   
} // end namespace viennamesh

#endif