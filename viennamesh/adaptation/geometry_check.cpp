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
#include "viennamesh/adaptation/geometry_check.hpp"
// *** vienna includes
#include "viennautils/messages.hpp"
// *** boost includes
#include <boost/make_shared.hpp>
#include <boost/array.hpp>
#include <boost/progress.hpp>
#ifdef VIENNAMESH_USE_MT
   #include <boost/threadpool.hpp>
#endif
// *** cgal includes
#include "CGAL/Exact_predicates_inexact_constructions_kernel.h"
#include "CGAL/Ray_3.h"
#include "CGAL/intersection_3.h"



namespace viennamesh {

// --------------------------------------------------------------------------
mesh_adaptor<viennamesh::tag::geom_check>::mesh_adaptor()
{
   id = "GeomCheck";      
#ifdef MESH_ADAPTOR_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - initiating" << std::endl;
#endif
}
// --------------------------------------------------------------------------
mesh_adaptor<viennamesh::tag::geom_check>::~mesh_adaptor()
{
   #ifdef MESH_ADAPTOR_DEBUG
      std::cout << "## MeshAdaptor::"+id+" - shutting down" << std::endl;
   #endif
}
// --------------------------------------------------------------------------
boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> >
mesh_adaptor<viennamesh::tag::geom_check>::operator()(viennagrid::domain<viennagrid::config::triangular_3d> & domain)
{
   // forwarding to main implementation
   return (*this)(boost::make_shared<viennagrid::domain<viennagrid::config::triangular_3d> >(domain));
}

// --------------------------------------------------------------------------

template<typename DomainSPT>
struct check_intersections
{
   typedef typename DomainSPT::value_type                                                                      domain_type;
   typedef typename domain_type::segment_type                                                                  SegmentType;   
   typedef typename domain_type::config_type                                                                   DomainConfiguration;
   typedef typename DomainConfiguration::cell_tag                                                              CellTag;   
   typedef typename viennagrid::result_of::ncell_type<DomainConfiguration, CellTag::topology_level>::type      CellType;   
   typedef typename viennagrid::result_of::point_type<DomainConfiguration>::type                               PointType;   

   typedef typename viennagrid::result_of::ncell_container<SegmentType, CellTag::topology_level>::type         CellContainer;      
   typedef typename viennagrid::result_of::iterator<CellContainer>::type                                       CellIterator;      
   typedef typename viennagrid::result_of::ncell_container<CellType, 0>::type                                  VertexOnCellContainer;
   typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type                               VertexOnCellIterator;      

   static const int DIMT = DomainConfiguration::cell_tag::topology_level;   
   static const int CELLSIZE = DIMT+1;      

   typedef CGAL::Exact_predicates_inexact_constructions_kernel    K;
   typedef CGAL::Ray_3<K>                                         cgal_ray_type;
   typedef CGAL::Point_3<K>                                       cgal_point_type;
   typedef CGAL::Direction_3<K>                                   cgal_direction_type;
   typedef CGAL::Triangle_3<K>                                    cgal_triangle_type;

   check_intersections(DomainSPT domain, SegmentType& seg, 
                       CellType& cell, 
                       boost::shared_ptr<std::size_t> intersp, 
                       boost::shared_ptr<boost::progress_display>   pdisp ) : domain(domain), seg(seg), cell(cell), intersp(intersp), pdisp(pdisp) {}
   
   DomainSPT                                    domain;
   SegmentType&                                 seg;
   CellType&                                    cell;
   boost::shared_ptr<std::size_t>               intersp;
   boost::shared_ptr<boost::progress_display>   pdisp;
   
   void run()
   {
      boost::array<PointType,CELLSIZE>     cell_points, cell_points2;      
   
      CellContainer cells = viennagrid::ncells<CellTag::topology_level>(seg);
   
      // extract the cell
      int vi = 0;
      VertexOnCellContainer vertices_for_cell = viennagrid::ncells<0>(cell);
      for (VertexOnCellIterator vocit = vertices_for_cell.begin();
          vocit != vertices_for_cell.end();
          ++vocit)
      {
         cell_points[vi++] = vocit->getPoint();
      }                  
      // transfer to cgal triangle object
      cgal_triangle_type ref_triangle(
         cgal_point_type(cell_points[0][0], cell_points[0][1], cell_points[0][2]),
         cgal_point_type(cell_points[1][0], cell_points[1][1], cell_points[1][2]),
         cgal_point_type(cell_points[2][0], cell_points[2][1], cell_points[2][2])
      );         
      
      // for each cell under test, traverse all other cells and test for intersection
      for (CellIterator cit2 = cells.begin(); cit2 != cells.end(); ++cit2)
      {
         if(cit2->getID() == cell.getID()) continue; // skip the reference cell
         
         int vi2 = 0;
         VertexOnCellContainer vertices_for_cell2 = viennagrid::ncells<0>(*cit2);
         for (VertexOnCellIterator vocit2 = vertices_for_cell2.begin();
             vocit2 != vertices_for_cell2.end();
             ++vocit2)
         {
            cell_points2[vi2++] = vocit2->getPoint();
         }                  
         cgal_triangle_type triangle(
            cgal_point_type(cell_points2[0][0], cell_points2[0][1], cell_points2[0][2]),
            cgal_point_type(cell_points2[1][0], cell_points2[1][1], cell_points2[1][2]),
            cgal_point_type(cell_points2[2][0], cell_points2[2][1], cell_points2[2][2])
         );         
         
         // perform the intersection test
         if(CGAL::do_intersect(ref_triangle, triangle)) (*intersp)++;
      }               
      ++(*pdisp);
   }
};

// --------------------------------------------------------------------------
boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> >
mesh_adaptor<viennamesh::tag::geom_check>::operator()(boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> > domain)
{
#ifdef MESH_ADAPTOR_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - starting up .." << std::endl;
#endif            

   typedef viennagrid::domain<viennagrid::config::triangular_3d>           domain_type;
   typedef domain_type::segment_type                                       SegmentType;
   typedef domain_type::config_type                                        DomainConfiguration;
   typedef DomainConfiguration::cell_tag                                   CellTag;
   typedef viennagrid::result_of::ncell_type<DomainConfiguration, CellTag::topology_level>::type      CellType;   
   typedef viennagrid::result_of::ncell_container<SegmentType, CellTag::topology_level>::type         CellContainer;      
   typedef viennagrid::result_of::iterator<CellContainer>::type                                       CellIterator;      
   typedef viennagrid::result_of::ncell_container<CellType, 0>::type                                  VertexOnCellContainer;
   typedef viennagrid::result_of::iterator<VertexOnCellContainer>::type                               VertexOnCellIterator;      
   typedef viennagrid::result_of::point_type<DomainConfiguration>::type                               PointType;   

   
   static const int DIMT = DomainConfiguration::cell_tag::topology_level;   
   static const int CELLSIZE = DIMT+1;      

   typedef CGAL::Exact_predicates_inexact_constructions_kernel    K;
   typedef CGAL::Ray_3<K>                                         cgal_ray_type;
   typedef CGAL::Point_3<K>                                       cgal_point_type;
   typedef CGAL::Direction_3<K>                                   cgal_direction_type;
   typedef CGAL::Triangle_3<K>                                    cgal_triangle_type;

#ifdef MESH_ADAPTOR_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - looking for 2-simplex intersections .." << std::endl;
#endif       
   
   boost::array<PointType,CELLSIZE>     cell_points, cell_points2;   
   
   std::size_t intersections = 0;
   std::size_t iterations = 0;

   for (std::size_t si = 0; si < domain->segment_size(); ++si)
   {
      SegmentType & seg = domain->segment(si);
      CellContainer cells = viennagrid::ncells<CellTag::topology_level>(seg);
      for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
      {
         iterations++;
      }
   }

   boost::shared_ptr<boost::progress_display>   pdisp(new boost::progress_display(iterations));
   boost::shared_ptr<std::size_t>               intersp(new std::size_t);
#ifdef VIENNAMESH_USE_MT
   std::size_t cores = boost::thread::hardware_concurrency();
   // if it is zero, the function hardware concurrency is not supported 
   // on the current workstation, set it at least to one ..
   if(cores == 0) cores = 1;
   boost::threadpool::pool tp(cores);
#endif

   for (std::size_t si = 0; si < domain->segment_size(); ++si)
   {
      SegmentType & seg = domain->segment(si);
      CellContainer cells = viennagrid::ncells<CellTag::topology_level>(seg);

      for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
      {
         typedef boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> >  domainsp_type;
      #ifdef VIENNAMESH_USE_MT
         boost::shared_ptr< check_intersections<domainsp_type> >  job(new check_intersections<domainsp_type>(domain, seg, *cit, intersp, pdisp));
         boost::threadpool::schedule(tp, boost::bind(&check_intersections<domainsp_type>::run, job));
      #else
         check_intersections<domainsp_type>  job(domain, seg, *cit, intersp, pdisp);
         job.run();
      #endif
         
      }      
   #ifdef VIENNAMESH_USE_MT      
      tp.wait();
   #endif
   }
   
#if MESH_ADAPTOR_DEBUG || MESH_STATISTICS
   std::cout << "   intersections: " << *intersp << std::endl;
#endif          
   
   if(*intersp > 0)
   {
      viennautils::msg::error("## MeshAdaptor::"+id+"mesh contains geometry erros - shutting down ..");
      //throw "mesh contains geometry errors";
   }
   
   return domain;
}
// --------------------------------------------------------------------------
} // end namespace viennamesh


