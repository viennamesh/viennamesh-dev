/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at

   license:    see file LICENSE in the base directory
============================================================================= */

// *** local includes
#include "viennamesh/adaptation/interface_sewer.hpp"
// *** vienna includes
#include "viennautils/messages.hpp"
// *** boost includes
#include <boost/make_shared.hpp>
#include <boost/array.hpp>

namespace viennamesh {

// --------------------------------------------------------------------------
mesh_adaptor<viennamesh::tag::int_sewer>::mesh_adaptor()
{
   id = "InterfaceSewer";      
#ifdef MESH_ADAPTOR_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - initiating" << std::endl;
#endif
}
// --------------------------------------------------------------------------
mesh_adaptor<viennamesh::tag::int_sewer>::~mesh_adaptor()
{
#ifdef MESH_ADAPTOR_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - shutting down" << std::endl;
#endif
}
// --------------------------------------------------------------------------
template<typename DomainT>
boost::shared_ptr< DomainT >
mesh_adaptor<viennamesh::tag::int_sewer>::operator()(DomainT& domain)
{
   // forwarding to main implementation
   return (*this)(boost::make_shared<DomainT>(domain));
}
// --------------------------------------------------------------------------
template<typename DomainT>
boost::shared_ptr< DomainT >
mesh_adaptor<viennamesh::tag::int_sewer>::operator()(boost::shared_ptr<DomainT> domain)
{
#ifdef MESH_ADAPTOR_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - starting up .." << std::endl;
#endif            

   typedef typename DomainT::config_type                                                                       DomainConfiguration;   
   typedef typename DomainConfiguration::cell_tag                                                              CellTag;   
   typedef typename DomainT::segment_type                                                                      SegmentType;
   typedef typename viennagrid::result_of::ncell_type<DomainConfiguration, CellTag::topology_level>::type      CellType;   
   typedef typename viennagrid::result_of::ncell_type<DomainConfiguration, 0>::type                            VertexType;   
   typedef typename viennagrid::result_of::ncell_container<DomainT, 0>::type                                   GeometryContainer;      
   typedef typename viennagrid::result_of::iterator<GeometryContainer>::type                                   GeometryIterator;       
   typedef typename viennagrid::result_of::ncell_container<SegmentType, 0>::type                               VertexContainer;      
   typedef typename viennagrid::result_of::iterator<VertexContainer>::type                                     VertexIterator;         
   typedef typename viennagrid::result_of::ncell_container<SegmentType, CellTag::topology_level>::type         CellContainer;      
   typedef typename viennagrid::result_of::iterator<CellContainer>::type                                       CellIterator;         
   typedef typename viennagrid::result_of::ncell_container<CellType, 0>::type                                  VertexOnCellContainer;
   typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type                               VertexOnCellIterator;         
   typedef typename viennagrid::result_of::point_type<DomainConfiguration>::type                               PointType;   

   static const int CELLSIZE = viennagrid::traits::subcell_desc<CellTag, 0>::num_elements;
   static const int DIMG     = DomainConfiguration::dimension_tag::value;


   boost::shared_ptr<DomainT> sewed_domain(new DomainT);         

   typedef boost::array<double, DIMG>     temppnt_type;

   std::map<std::size_t, std::size_t>     index_map;
   std::map<temppnt_type, bool>           uniquer;
   std::map<temppnt_type, std::size_t>    point_index;   
   
#ifdef MESH_ADAPTOR_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - transferring geometry .." << std::endl;
#endif            

   
#ifdef MESH_ADAPTOR_DEBUG 
   std::size_t colocal_points = 0;
#endif
//   GeometryContainer geometry = viennagrid::ncells<0>(*domain);
//   for(GeometryIterator git = geometry.begin(); git != geometry.end(); git++)

   for (std::size_t si = 0; si < domain->segment_size(); ++si)
   {
      SegmentType & seg = domain->segment(si);
      VertexContainer vertices = viennagrid::ncells<0>(seg);
      for(VertexIterator vit = vertices.begin(); vit != vertices.end(); vit++)
      {
         temppnt_type temppnt;
         for(std::size_t i = 0; i < DIMG; i++)
            temppnt[i] = vit->getPoint()[i];
         
         if( !uniquer[temppnt] )
         {
            VertexType vertex;
            vertex.getPoint()       = vit->getPoint();
            index_map[vit->getID()] = sewed_domain->add(vertex)->getID();
            point_index[temppnt]   = index_map[vit->getID()];
            uniquer[temppnt]       = true;
         }
         else
         {
         #ifdef MESH_ADAPTOR_DEBUG 
            colocal_points++;
         #endif      
            index_map[vit->getID()]    = point_index[temppnt];
         }
         
      }
   }

#ifdef MESH_ADAPTOR_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - transferring topology .." << std::endl;
#endif            

   sewed_domain->create_segments(domain->segment_size());

   for (std::size_t si = 0; si < domain->segment_size(); ++si)
   {
      SegmentType & seg = domain->segment(si);
      CellContainer cells = viennagrid::ncells<CellTag::topology_level>(seg);
      for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
      {
         boost::array<std::size_t, CELLSIZE> tempcell;
         std::size_t vi = 0;       
         VertexOnCellContainer vertices_for_cell = viennagrid::ncells<0>(*cit);
         for (VertexOnCellIterator vocit = vertices_for_cell.begin();
             vocit != vertices_for_cell.end();
             ++vocit)
         {
            tempcell[vi++] = vocit->getID();
         }
         
         VertexType *vertices[CELLSIZE];  
         for(std::size_t ci = 0; ci < CELLSIZE; ci++)
            vertices[ci] = &(sewed_domain->vertex(index_map[tempcell[ci]]));
         CellType cell;
         cell.setVertices(vertices);
         sewed_domain->segment(si).add(cell);
      }
   }

#ifdef MESH_ADAPTOR_DEBUG 
   std::cout << "## MeshAdaptor::"+id+" - colocal points removed: " << colocal_points << std::endl;
#endif      

   return sewed_domain;

}
// -----------------------------------------------------------------------------
// 
// explicit declarations for the template functions
// 
template boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_2d> >
mesh_adaptor<viennamesh::tag::int_sewer>::operator()(viennagrid::domain<viennagrid::config::triangular_2d>& domain);
template boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_2d> >
mesh_adaptor<viennamesh::tag::int_sewer>::operator()(boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_2d> > domain);

template boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> >
mesh_adaptor<viennamesh::tag::int_sewer>::operator()(viennagrid::domain<viennagrid::config::triangular_3d>& domain);
template boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> >
mesh_adaptor<viennamesh::tag::int_sewer>::operator()(boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> > domain);

template boost::shared_ptr< viennagrid::domain<viennagrid::config::tetrahedral_3d> >
mesh_adaptor<viennamesh::tag::int_sewer>::operator()(viennagrid::domain<viennagrid::config::tetrahedral_3d>& domain);
template boost::shared_ptr< viennagrid::domain<viennagrid::config::tetrahedral_3d> >
mesh_adaptor<viennamesh::tag::int_sewer>::operator()(boost::shared_ptr< viennagrid::domain<viennagrid::config::tetrahedral_3d> > domain);
// -----------------------------------------------------------------------------

} // end namespace viennamesh









