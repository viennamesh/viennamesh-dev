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


#ifndef VIENNAMESH_MESH_HPP
#define VIENNAMESH_MESH_HPP

#include "viennamesh/interfaces.hpp"  
#include "viennamesh/add.hpp"
#include "viennamesh/object.hpp"
#include "viennamesh/generate.hpp"

#include "viennagrid/domain.hpp"

#include "viennautils/dumptype.hpp"

namespace viennamesh {

template<typename MapSequenceT>
struct mesh_generator
{
   typedef viennamesh::triangle<double>  mesher_type;
   typedef viennamesh::add<mesher_type>  add_type;
   
   template<typename DomainT>
   void operator()(DomainT& domain)
   {
      add_type add(mesher);
      
      typedef typename DomainT::config_type              domain_config_type;
      typedef typename domain_config_type::cell_tag      cell_tag_type;
      typedef typename domain_config_type::numeric_type  numeric_type;
      
      static const int DIMT = cell_tag_type::topology_level;
      static const int DIMG = domain_config_type::dimension_tag::value;
      
      typedef typename viennagrid::result_of::ncell_container<DomainT, 0>::type        vertex_container_type;
      typedef typename viennagrid::result_of::iterator<vertex_container_type>::type    vertex_container_iterator_type;

      vertex_container_type vertices = viennagrid::ncells<0>(domain);
      for (vertex_container_iterator_type it = vertices.begin();
            it != vertices.end(); ++it)
      {
         add(viennamesh::object<viennamesh::key::point>(it->getPoint()));
      }   

      typedef typename viennagrid::result_of::ncell_container<DomainT, DIMT>::type     cell_container_type;
      typedef typename viennagrid::result_of::iterator<cell_container_type>::type      cell_container_iterator_type;

      boost::array<long, DIMT+1>   cell;      
      cell_container_type cells = viennagrid::ncells<DIMT>(domain);
      for (cell_container_iterator_type cit = cells.begin();
            cit != cells.end(); ++cit)
      {
         typedef typename viennagrid::result_of::ncell_type<domain_config_type, DIMT>::type        cell_type;
         typedef typename viennagrid::result_of::ncell_container<cell_type, 0>::type               vertex_on_cell_container_type;;
         typedef typename viennagrid::result_of::iterator<vertex_on_cell_container_type>::type     vertex_on_cell_iterator_type;
         
         vertex_on_cell_container_type vertex_on_cell_cont = viennagrid::ncells<0>(*cit);
         
         long i = 0;
         for(vertex_on_cell_iterator_type vocit = vertex_on_cell_cont.begin();
             vocit != vertex_on_cell_cont.end(); vocit++)
         {
            cell[i] = vocit->getID();
            i++;
         }
         add(viennamesh::object<viennamesh::key::constraint>(cell));         
      }   
      add(viennamesh::object<viennamesh::key::method>(viennamesh::method::conforming_delaunay()));
      
      //typedef viennagrid::segment_t<domain_config_type>           segment_type;      
      typedef boost::array< numeric_type, DIMG >     point_type;
      
      point_type pnt_reg;
      
      for(size_t si = 0; si < domain.segment_container()->size(); si++)
      {  
         //add(viennamesh::object<viennamesh::key::region>(pntreg1));
      }
      
      viennamesh::generate(mesher);
   }
   
   // -------------------------------------------------------------------------     
   inline typename mesher_type::geometry_container_type &
   geometry()
   {
      return mesher.geometry();
   }         
   // -------------------------------------------------------------------------     
   inline typename mesher_type::segment_container_type &
   topology()
   {
      return mesher.topology();
   }       
   // -------------------------------------------------------------------------      
   
   mesher_type mesher;   
};
   
   
template<typename T, typename DomainT>
void transfer(typename viennamesh::mesh_generator<T>& mesher, DomainT& domain)
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
   
   typedef typename viennamesh::mesh_generator<T>::mesher_type    mesher_type;
   typedef typename mesher_type::geometry_container_type          mesher_geometry_cont_type;
   typedef typename mesher_type::segment_container_type           mesher_segment_container_type;
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
   
//    long ci = 0;
//    domain.reserve_cells(topo_cont.size());
//    for(typename topology_cont_type::iterator iter = topo_cont.begin(); 
//       iter != topo_cont.end(); iter++)
//    {
//       for(int i = 0; i < CELLSIZE; i++)
//       {
//          vertices[i] = &(domain.vertex((*iter)[i]));
//       }      
//       cell.setVertices(&(vertices[0]));
//       cell.setID(ci);
//       domain.add(cell);         
//    }

}   
   
   
// template<typename MesherT, typename DomainT>   
// void load_mesher(MesherT& mesher, DomainT& domain)
// {
//    typedef typename DomainInT::config_type                  domain_config_in_type;
//    typedef typename domain_config_in_type::cell_tag         cell_tag_in_type;;
//    
//    typedef typename viennagrid::result_of::ncell_container<DomainInT, 0>::type      vertex_container_in_type;
//    typedef typename viennagrid::result_of::iterator<vertex_container_type>::type    vertex_container_iterator_type;   
// 
// }
//    
// template<typename DomainInT>
// struct mesh_generator 
// {
//    mesh_generator(DomainInT& domain_in) : domain_in(domain_in) {}
//    
//    template<typename DomainOutT>
//    void operator()(DomainOutT& domain_out)
//    {
//    }
//    
//    DomainInT & domain_in;
// };
// 
//    
// template<typename DomainInT, typename DomainOutT>   
// void mesh(DomainInT& domainin, DomainOutT& domainout)
// {
//    
// }

} // end namespace viennamesh

#endif
