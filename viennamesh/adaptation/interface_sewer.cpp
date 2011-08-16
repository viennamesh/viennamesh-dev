/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at
               Franz Stimpfl


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
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - initiating" << std::endl;
#endif
}
// --------------------------------------------------------------------------
mesh_adaptor<viennamesh::tag::int_sewer>::~mesh_adaptor()
{
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshAdaptor::"+id+" - shutting down" << std::endl;
   #endif
}
// --------------------------------------------------------------------------
boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> >
mesh_adaptor<viennamesh::tag::int_sewer>::operator()(viennagrid::domain<viennagrid::config::triangular_3d> & domain)
{
   // forwarding to main implementation
   return (*this)(boost::make_shared<viennagrid::domain<viennagrid::config::triangular_3d> >(domain));
}
// --------------------------------------------------------------------------
boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> >
mesh_adaptor<viennamesh::tag::int_sewer>::operator()(boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> > domain)
{
#ifdef MESH_ADAPTOR_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - starting up .." << std::endl;
#endif            

   input_type sewed_domain(new domain_type);         

   typedef boost::array<double, CELLSIZE> tempcell_type;

   std::map<std::size_t, std::size_t>     index_map;
   std::map<tempcell_type, bool>          uniquer;
   std::map<tempcell_type, std::size_t>   point_index;   
   
   GeometryContainer geometry = viennagrid::ncells<0>(*domain);
   for(GeometryIterator git = geometry.begin(); git != geometry.end(); git++)
   {
      tempcell_type tempcell;
      for(std::size_t i = 0; i < CELLSIZE; i++)
         tempcell[i] = git->getPoint()[i];
      
      if( !uniquer[tempcell] )
      {
         VertexType vertex;
         vertex.getPoint()       = git->getPoint();
         index_map[git->getID()] = sewed_domain->add(vertex)->getID();
         point_index[tempcell]   = index_map[git->getID()];
         uniquer[tempcell]       = true;
      }
      else
      {
         index_map[git->getID()]    = point_index[tempcell];
      }
      
   }

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

   return sewed_domain;

}
// --------------------------------------------------------------------------
bool mesh_adaptor<viennamesh::tag::int_sewer>::colocal(PointType const& pnt1, PointType const& pnt2)
{
   bool colocal = true;
   for(std::size_t i = 0; i < pnt1.size(); i++)
   {
      if( !(fabs(pnt1[i]-pnt2[i]) < std::numeric_limits<PointType::value_type>::epsilon()) )
         colocal = false;
   }
   return colocal;
}
// --------------------------------------------------------------------------


} // end namespace viennamesh









