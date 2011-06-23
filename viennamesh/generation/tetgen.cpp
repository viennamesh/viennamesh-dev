/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */



// *** system includes
#include <vector>
#include <map>
#include <cmath>
#include <cstring>

// *** vienna includes
#include "viennautils/dumptype.hpp"
#include "viennagrid/domain.hpp"

// *** local includes
#include "tetgen.hpp"

// *** boost includes
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/at_key.hpp>

// *** tetgen includes
// note: macros need to be set before including tetgen.h ..
#define REAL double
#define TETLIBRARY
#include "tetgen/tetgen.h"



namespace viennamesh {

// --------------------------------------------------------------------------
mesh_kernel<viennamesh::tag::tetgen>::mesh_kernel()
{
   mesh_kernel_id = "Tetgen";      
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - initiating" << std::endl;
#endif
}
// --------------------------------------------------------------------------
mesh_kernel<viennamesh::tag::tetgen>::~mesh_kernel()
{
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - shutting down" << std::endl;
   #endif
}
// --------------------------------------------------------------------------
mesh_kernel<viennamesh::tag::tetgen>::result_type 
mesh_kernel<viennamesh::tag::tetgen>::operator()(viennamesh::wrapper<viennamesh::tag::bnd, viennautils::io::bnd_reader>& data) // default meshing
{
   return (*this)(data, boost::fusion::make_map<tag::criteria>(tag::constrained_delaunay()));
}   

template<typename ParametersMapT>
mesh_kernel<viennamesh::tag::tetgen>::result_type 
mesh_kernel<viennamesh::tag::tetgen>::operator()(viennamesh::wrapper<viennamesh::tag::bnd, viennautils::io::bnd_reader>& data, ParametersMapT const& paras )
{
   // redirect to reference implementation 
   ParametersMapT paras_new(paras);
   return (*this)(data, paras_new);
}

template<typename ParametersMapT>
mesh_kernel<viennamesh::tag::tetgen>::result_type 
mesh_kernel<viennamesh::tag::tetgen>::operator()(viennamesh::wrapper<viennamesh::tag::bnd, viennautils::io::bnd_reader>& data, ParametersMapT & paras )
{
   typedef viennamesh::wrapper<viennamesh::tag::bnd, viennautils::io::bnd_reader> DatastructureT;
   this->init();      

   typedef typename DatastructureT::segment_iterator  vmesh_segment_iterator;
   typedef typename DatastructureT::cell_type         vmesh_cell_type;
   typedef typename DatastructureT::cell_iterator     vmesh_cell_iterator;   

   this->setOptions(paras);


   size_t segment_size = data.segment_size();
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing segments" << std::endl;
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - detected segments: " << segment_size << std::endl;
#endif  
   if( segment_size > 1 )
   {
     typedef std::vector<point_type>    region_points_type;
     region_points_type region_points;
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - dealing with multi-segment input" << std::endl;
   #endif      
      std::size_t seg_cnt = 0;

      for(vmesh_segment_iterator seg_iter = data.segment_begin();
         seg_iter != data.segment_end(); seg_iter++)
      {
         point_type pnt;
         typename DatastructureT::cell_complex_wrapper_type segment = *seg_iter;
         this->find_point_in_segment(data, segment, pnt, seg_cnt);
         
         region_points.push_back(pnt);
      #ifdef MESH_KERNEL_DEBUG
         std::cout << "## MeshKernel::"+mesh_kernel_id+" - computed point in segment " 
            << seg_cnt << " : " << pnt[0] << " " << pnt[1] << " " << pnt[2] << std::endl;
      #endif
         seg_cnt++;
      }
      for(typename region_points_type::iterator iter = region_points.begin();
          iter != region_points.end(); iter++)
      {
         this->addRegion(*iter);
      }
   }
#ifdef MESH_KERNEL_DEBUG
   else
   {
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - dealing with single-segment input" << std::endl;
   }   
#endif              



   // traverse and add the geometry information
   // aka the set of points
   //
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing geometry" << std::endl;
#endif
   typedef typename DatastructureT::geometry_iterator geometry_iterator;
   for(geometry_iterator iter = data.geometry_begin();
       iter != data.geometry_end(); iter++)
   {
   #ifdef MESH_KERNEL_DEBUG_FULL
       std::cout << "## MeshKernel::"+mesh_kernel_id+" - adding point " << 
          std::distance(data.geometry_begin(), iter) << " : " << *iter << std::endl;
   #endif   
      this->addPoint(*iter);
   }
   
   // traverse and add the topology information
   // aka the set of boundary constraints
   //
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing constraints" << std::endl;
#endif
   //size_t si = 0;

   typedef std::vector<std::size_t>           cell_copy_type;
   std::map<cell_copy_type, bool>        cell_uniquer;      
#ifdef MESH_KERNEL_DEBUG_FULL
   size_t cell_cnt = 0;
#endif
   for(vmesh_segment_iterator seg_iter = data.segment_begin();
      seg_iter != data.segment_end(); seg_iter++)
   {  
      for(vmesh_cell_iterator cit = (*seg_iter).cell_begin();
          cit != (*seg_iter).cell_end(); cit++)
      {  
         vmesh_cell_type cell = *cit;

         // make sure a cell hasn't been already added ..
         // therefore we copy the original cell, and sort it ascendingly
         // due to the ascending order, we can compare it with other cells
         cell_copy_type cell_copy(cell.size());

         for(size_t i = 0; i < cell.size(); i++)  
            cell_copy[i] = cell[i];

         std::sort(cell_copy.begin(), cell_copy.end());
         
         if(!cell_uniquer[cell_copy])
         {
         #ifdef MESH_KERNEL_DEBUG_FULL
            std::cout << "## MeshKernel::"+mesh_kernel_id+" - adding constraint   " << 
               cell_cnt << " : ";
            for(size_t i = 0; i < cell.size(); i++)  
               std::cout << cell[i] << " ";
            std::cout << std::endl;
            cell_cnt++;
         #endif               
            cell_uniquer[cell_copy] = true;
            
            this->addConstraint(cell);
         }
      #ifdef MESH_KERNEL_DEBUG_FULL
         else
         { 
            std::cout << "## MeshKernel::"+mesh_kernel_id+" - skipping constraint " << 
               cell_cnt << " : ";
            for(size_t i = 0; i < cell.size(); i++)  
               std::cout << cell[i] << " ";
            std::cout << std::endl;
         }
      #endif

      }
   }            

   domain_ptr_type domain(new domain_type);               

   this->do_meshing(domain);

   this->clear();   
   
   return domain;

}
// --------------------------------------------------------------------------
mesh_kernel<viennamesh::tag::tetgen>::result_type 
mesh_kernel<viennamesh::tag::tetgen>::operator()(boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> > hull_domain) 
{
   return (*this)(hull_domain, boost::fusion::make_map<tag::criteria>(tag::constrained_delaunay()));
}   


template<typename ParametersMapT>
mesh_kernel<viennamesh::tag::tetgen>::result_type 
mesh_kernel<viennamesh::tag::tetgen>::operator()(boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> > hull_domain, 
                       ParametersMapT const& paras )
{
   ParametersMapT paras_new(paras);
   return (*this)(hull_domain, paras_new);
}

template<typename ParametersMapT>
mesh_kernel<viennamesh::tag::tetgen>::result_type 
mesh_kernel<viennamesh::tag::tetgen>::operator()(boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> > hull_domain,
                       ParametersMapT & paras)
{
   this->init();         
   this->setOptions(paras);

   typedef typename viennagrid::domain<viennagrid::config::triangular_3d>  HullDomain;
   typedef typename HullDomain::config_type                                HullDomainConfiguration;      
   typedef typename HullDomain::segment_type                               HullSegmentType;      
   typedef typename HullDomainConfiguration::cell_tag                      HullCellTag;      
   typedef typename viennagrid::result_of::point_type<HullDomainConfiguration>::type                                 HullPointType;   
   typedef typename viennagrid::result_of::ncell_container<HullDomain, 0>::type                                      HullPointContainer;            
   typedef typename viennagrid::result_of::iterator<HullPointContainer>::type                                        HullPointIterator;            
   typedef typename viennagrid::result_of::ncell_type<HullDomainConfiguration, HullCellTag::topology_level>::type    HullCellType;      
   typedef typename viennagrid::result_of::ncell_container<HullSegmentType, HullCellTag::topology_level>::type       HullCellContainer;      
   typedef typename viennagrid::result_of::iterator<HullCellContainer>::type                                         HullCellIterator;         
   typedef typename viennagrid::result_of::ncell_container<HullCellType, 0>::type                                    HullVertexOnCellContainer;
   typedef typename viennagrid::result_of::iterator<HullVertexOnCellContainer>::type                                 HullVertexOnCellIterator;               

   std::size_t segment_size = hull_domain->segment_size();   
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing segments" << std::endl;
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - detected segments: " << segment_size << std::endl;
#endif  
   if( segment_size > 1 )
   {
     typedef std::vector<HullPointType>    region_points_type;
     region_points_type region_points;
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - dealing with multi-segment input" << std::endl;
   #endif      
      for (std::size_t si = 0; si < hull_domain->segment_size(); ++si)
      {
         HullSegmentType & seg = hull_domain->segment(si); 
         HullPointType pnt;
         this->find_point_in_segment(hull_domain, seg, pnt, si);
         region_points.push_back(pnt);
      #ifdef MESH_KERNEL_DEBUG
         std::cout << "## MeshKernel::"+mesh_kernel_id+" - computed point in segment " 
            << si << " : " << pnt[0] << " " << pnt[1] << " " << pnt[2] << std::endl;
      #endif

      }
      for(typename region_points_type::iterator iter = region_points.begin();
          iter != region_points.end(); iter++)
      {  
         this->addRegion(*iter);
      }
   }
#ifdef MESH_KERNEL_DEBUG
   else
   {
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - dealing with single-segment input" << std::endl;
   }   
#endif              

   // traverse and add the geometry information
   // aka the set of points
   //
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing geometry" << std::endl;
#endif

#ifdef MESH_KERNEL_DEBUG_FULL
   std::size_t point_cnt = 0;
#endif

  HullPointContainer vertices = viennagrid::ncells<0>(*hull_domain);
  for (HullPointIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
  {
   #ifdef MESH_KERNEL_DEBUG_FULL
       std::cout << "## MeshKernel::"+mesh_kernel_id+" - adding point " << 
          point_cnt++ << " : " << vit->getPoint() << std::endl;
   #endif   
      this->addPoint(vit->getPoint());
  }

   // traverse and add the topology information
   // aka the set of boundary constraints
   //
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing constraints" << std::endl;
#endif
   //size_t si = 0;

   typedef std::vector<std::size_t>           cell_copy_type;
   std::map<cell_copy_type, bool>        cell_uniquer;      
#ifdef MESH_KERNEL_DEBUG_FULL
   size_t cell_cnt = 0;
#endif
   for (std::size_t si = 0; si < hull_domain->segment_size(); ++si)
   {
      HullSegmentType & seg = hull_domain->segment(si); 
      HullCellContainer cells = viennagrid::ncells<HullCellTag::topology_level>(seg);      
      
      for (HullCellIterator cit = cells.begin(); cit != cells.end(); ++cit)
      {  

         // make sure a cell hasn't been already added ..
         // therefore we copy the original cell, and sort it ascendingly
         // due to the ascending order, we can compare it with other cells
         cell_copy_type cell_copy;

         HullVertexOnCellContainer vertices_for_cell = viennagrid::ncells<0>(*cit);
         for (HullVertexOnCellIterator vocit = vertices_for_cell.begin();
             vocit != vertices_for_cell.end();
             ++vocit)
         {
            cell_copy.push_back(vocit->getID());
         }
         
         // we will keep a copy of the cell which stores the original
         // orientation. we'll use this oriented cell as a constraint, 
         // just to be sure the input is oriented right.
         // note: tetgen does _not_ require the input cells to be 
         // oriented in any manner - however, maybe TetGen derives 
         // the orientation of the generated tetrahedrons from the input 
         // constraints - therefore it would be wise to not through the 
         // input orientation away.
         //
         cell_copy_type cell_orig(cell_copy.size());
         std::copy(cell_copy.begin(), cell_copy.end(), cell_orig.begin());
         std::sort(cell_copy.begin(), cell_copy.end());
         
         if(!cell_uniquer[cell_copy])
         {
         #ifdef MESH_KERNEL_DEBUG_FULL
            std::cout << "## MeshKernel::"+mesh_kernel_id+" - adding constraint   " << 
               cell_cnt << " : ";
            for(size_t i = 0; i < cell_orig.size(); i++)  
               std::cout << cell_orig[i] << " ";
            std::cout << std::endl;
            cell_cnt++;
         #endif               
            cell_uniquer[cell_copy] = true;
            
            
            this->addConstraint(cell_orig); 
         }
      #ifdef MESH_KERNEL_DEBUG_FULL
         else
         { 
            std::cout << "## MeshKernel::"+mesh_kernel_id+" - skipping constraint " << 
               cell_cnt << " : ";
            for(size_t i = 0; i < cell_orig.size(); i++)  
               std::cout << cell_orig[i] << " ";
            std::cout << std::endl;
         }
      #endif

      }
   }            

   domain_ptr_type domain(new domain_type);               

   this->do_meshing(domain);

   this->clear();   
   
   return domain;   
}
// --------------------------------------------------------------------------
void mesh_kernel<viennamesh::tag::tetgen>::do_meshing(domain_ptr_type domain)
{
   // if there are more than one regions, spread the regional attribute
   if(in->numberofregions > 1) options.append("A");  
   
#ifndef MESH_KERNEL_DEBUG
   options.append("Q");
#endif
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - meshing:" << std::endl;
   std::cout << "  parameter set:     " << options << std::endl;
   std::cout << "  input point size:  " << in->numberofpoints << std::endl;
   std::cout << "  input const size:  " << in->numberoffacets << std::endl;      
   std::cout << "  input region size: " << in->numberofregions << std::endl;            
   std::cout << "## MeshKernel::"+mesh_kernel_id+" starting mesh generation " << std::endl;               
#endif         

   char *buffer;
   buffer = (char *)malloc( options.length() * sizeof(char) );
   std::strcpy(buffer,options.c_str());      

   BOOST_ASSERT(in->numberofpoints != 0);
   //BOOST_ASSERT(in->numberoffacets != 0);      
   //
   // start meshing process
   //
   tetrahedralize(buffer, in, out);

   free(buffer);
   
   if ( (!out->pointlist) || !out->tetrahedronlist )      
      std::cerr << "## MeshKernel::"+mesh_kernel_id+" [ERROR] - no mesh produced due to some input error" << std::endl;      
   
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - finished:" << std::endl;
   std::cout << "  output point size:  " << out->numberofpoints << std::endl;
   std::cout << "  output cell size:   " << out->numberoftetrahedra << std::endl;      
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - extracting geometry" << std::endl;
#endif               
   
   transfer_to_domain(domain, out);
}   


// --------------------------------------------------------------------------
template<typename ParametersMapT>
void mesh_kernel<viennamesh::tag::tetgen>::setOptions(ParametersMapT & paras,
   typename boost::enable_if< typename boost::fusion::result_of::has_key<ParametersMapT, viennamesh::tag::criteria>::type >::type* dummy = 0) 
{
   setOptions_impl(boost::fusion::at_key<viennamesh::tag::criteria>(paras));
}
void mesh_kernel<viennamesh::tag::tetgen>::setOptions_impl(viennamesh::tag::convex const&)               { options = "z";    }   
void mesh_kernel<viennamesh::tag::tetgen>::setOptions_impl(viennamesh::tag::constrained_delaunay const&) { options = "zp";  }
void mesh_kernel<viennamesh::tag::tetgen>::setOptions_impl(viennamesh::tag::conforming_delaunay const&)  { options = "zpD"; }
// --------------------------------------------------------------------------

template<typename PointT>
void mesh_kernel<viennamesh::tag::tetgen>::find_point_in_segment(boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> >       hull_domain,
                           typename viennagrid::domain<viennagrid::config::triangular_3d>::segment_type&    seg, 
                           PointT& pnt, std::size_t segid = 0)
{
   typedef typename viennagrid::domain<viennagrid::config::triangular_3d>  HullDomain;
   typedef typename HullDomain::config_type                                HullDomainConfiguration;      
   typedef typename HullDomain::segment_type                               HullSegmentType;         
   typedef typename HullDomainConfiguration::cell_tag                      HullCellTag;            

   typedef typename viennagrid::result_of::point_type<HullDomainConfiguration>::type                                 HullPointType;   
   typedef typename viennagrid::result_of::ncell_type<HullDomainConfiguration, HullCellTag::topology_level>::type    HullCellType;         
   typedef typename viennagrid::result_of::ncell_container<HullSegmentType, HullCellTag::topology_level>::type       HullCellContainer;      
   typedef typename viennagrid::result_of::iterator<HullCellContainer>::type                                         HullCellIterator;            
   typedef typename viennagrid::result_of::ncell_container<HullCellType, 0>::type                                    HullVertexOnCellContainer;
   typedef typename viennagrid::result_of::iterator<HullVertexOnCellContainer>::type                                 HullVertexOnCellIterator;            

   self_type   temp_mesher;
   temp_mesher.init();   
   
   std::map<std::size_t, bool>  pnt_uniquer;
   std::map<std::size_t, std::size_t> index_map;
   std::size_t point_cnt = 0;      
   
   HullCellContainer cells = viennagrid::ncells<HullCellTag::topology_level>(seg);      
   
   for (HullCellIterator cit = cells.begin(); cit != cells.end(); ++cit)
   {      
      HullVertexOnCellContainer vertices_for_cell = viennagrid::ncells<0>(*cit);      
      for (HullVertexOnCellIterator vocit = vertices_for_cell.begin();
          vocit != vertices_for_cell.end();
          ++vocit)
      {
         std::size_t vindex = vocit->getID();
         if(!pnt_uniquer[ vindex ])
         {  
            temp_mesher.addPoint( vocit->getPoint() ); 
            
            pnt_uniquer[vindex] = true;
            index_map[vindex]   = point_cnt;
            point_cnt++;               
         }         
      }      
      std::vector<std::size_t> mapped_cell;
      for (HullVertexOnCellIterator vocit = vertices_for_cell.begin();
          vocit != vertices_for_cell.end();
          ++vocit)
      {
         std::size_t vindex = vocit->getID();
         mapped_cell.push_back(index_map[ vindex ]);
      }
      //std::cout << "adding cell: " << mapped_cell << std::endl;
      temp_mesher.addConstraint(mapped_cell);         
   }
   
   std::string temp_options = "zpQ";
   char* buffer;
   buffer = (char *)malloc( temp_options.length() * sizeof(char) );
   std::strcpy(buffer,temp_options.c_str());      
   tetrahedralize(buffer, temp_mesher.in, temp_mesher.out);
   free(buffer);

   if ( (!temp_mesher.out->pointlist) || !temp_mesher.out->tetrahedronlist )      
      std::cout << "## MeshKernel::"+mesh_kernel_id+" [ERROR] - point in segment algorithm failed as pre-meshing failed!" << std::endl;

   PointT pnt1 (temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[0]*DIMG)],
                temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[0]*DIMG+1)],
                temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[0]*DIMG+2)]);

   PointT pnt2 (temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[1]*DIMG)],
                temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[1]*DIMG+1)],
                temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[1]*DIMG+2)]);

   PointT pnt3 (temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[2]*DIMG)],
                temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[2]*DIMG+1)],
                temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[2]*DIMG+2)]);

   PointT pnt4 (temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[3]*DIMG)],
                temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[3]*DIMG+1)],
                temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[3]*DIMG+2)]);

   temp_mesher.barycenter(pnt1, pnt2, pnt3, pnt4, pnt);

//   #ifdef MESH_KERNEL_DEBUG_FULL
//      // export the current segment to a vtk file, to investigate it ..
//      domain_ptr_type temp_domain(new domain_type);               
//      transfer_to_domain(temp_domain, temp_mesher.out);
//      viennagrid::io::vtk_writer<domain_type>  my_volume_vtk_writer;         
//      my_volume_vtk_writer.writeDomain(*temp_domain, "premesh_seg_"+boost::lexical_cast<std::string>(segid));
//   #endif

   temp_mesher.clear();      
}


template<typename DatastructureT>
void mesh_kernel<viennamesh::tag::tetgen>::find_point_in_segment(DatastructureT& data, 
                           typename DatastructureT::cell_complex_wrapper_type & cell_complex, 
                           point_type& pnt, std::size_t segid = 0)
{
   self_type   temp_mesher;
   temp_mesher.init();
   typedef typename DatastructureT::geometry_iterator geometry_iterator;
   typedef typename DatastructureT::cell_type         vmesh_cell_type;      
   typedef typename DatastructureT::cell_iterator     vmesh_cell_iterator;         

   std::map<std::size_t, bool>  pnt_uniquer;
   std::map<std::size_t, std::size_t> index_map;
   std::size_t point_cnt = 0;

   for(vmesh_cell_iterator cit = cell_complex.cell_begin();
         cit != cell_complex.cell_end(); cit++)
   {
      vmesh_cell_type cell = *cit;
      for(std::size_t dim = 0; dim < cell.size(); dim++)
      {
         if(!pnt_uniquer[cell[dim]])
         {  
            geometry_iterator geo = data.geometry_begin();
            std::advance(geo, cell[dim]);

//            std::cout << "adding point: " << *geo << std::endl;
            temp_mesher.addPoint( *geo ); 

            pnt_uniquer[cell[dim]] = true;
            index_map[cell[dim]] = point_cnt;
            point_cnt++;
         }
      }
      std::vector<std::size_t> mapped_cell(cell.size());
      for(std::size_t i = 0; i < cell.size(); i++)
      {
        mapped_cell[i] = index_map[cell[i]];
      }
      //std::cout << "adding cell: " << mapped_cell << std::endl;
      temp_mesher.addConstraint(mapped_cell);
   }

   
   std::string temp_options = "zpQ";
   char* buffer;
   buffer = (char *)malloc( temp_options.length() * sizeof(char) );
   std::strcpy(buffer,temp_options.c_str());      
   tetrahedralize(buffer, temp_mesher.in, temp_mesher.out);
   free(buffer);

   if ( (!temp_mesher.out->pointlist) || !temp_mesher.out->tetrahedronlist )      
      std::cout << "## MeshKernel::"+mesh_kernel_id+" [ERROR] - point in segment algorithm failed as pre-meshing failed!" << std::endl;

   point_type pnt1 = {{temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[0]*DIMG)],
                       temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[0]*DIMG+1)],
                       temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[0]*DIMG+2)]}};

   point_type pnt2 = {{temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[1]*DIMG)],
                       temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[1]*DIMG+1)],
                       temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[1]*DIMG+2)]}};

   point_type pnt3 = {{temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[2]*DIMG)],
                       temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[2]*DIMG+1)],
                       temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[2]*DIMG+2)]}};

   point_type pnt4 = {{temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[3]*DIMG)],
                       temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[3]*DIMG+1)],
                       temp_mesher.out->pointlist[(temp_mesher.out->tetrahedronlist[3]*DIMG+2)]}};

   temp_mesher.barycenter(pnt1, pnt2, pnt3, pnt4, pnt);

//   #ifdef MESH_KERNEL_DEBUG_FULL
//      // export the current segment to a vtk file, to investigate it ..
//      domain_ptr_type temp_domain(new domain_type);               
//      transfer_to_domain(temp_domain, temp_mesher.out);
//      viennagrid::io::vtk_writer<domain_type>  my_volume_vtk_writer;         
//      my_volume_vtk_writer.writeDomain(*temp_domain, "premesh_seg_"+boost::lexical_cast<std::string>(segid));
//   #endif

   temp_mesher.clear();
}


void mesh_kernel<viennamesh::tag::tetgen>::transfer_to_domain(domain_ptr_type domain, tetgenio *mesh)
{
   //
   // extracting geometry data
   //
   domain->reserve_vertices(mesh->numberofpoints);             

   std::size_t point_cnt = 0;
   for(long pnt_index = 0; pnt_index < mesh->numberofpoints; ++pnt_index)
   {
      integer_type index = pnt_index * 3;
      
//         numeric_type pnt[DIMG];
//         pnt[0] = mesh->pointlist[index];
//         pnt[1] = mesh->pointlist[index+1];
//         pnt[2] = mesh->pointlist[index+2];         

      vertex_type    vertex;
      vertex.getPoint()[0] = mesh->pointlist[index];
      vertex.getPoint()[1] = mesh->pointlist[index+1];
      vertex.getPoint()[2] = mesh->pointlist[index+2];                  
      vertex.setID(point_cnt++);
      domain->add(vertex);
   }      
   
   std::size_t seg_id = 0;
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - extracting topology" << std::endl;
   std::map<size_t, bool>  seg_check;
#endif          
   //
   // extracting cell complex
   //
   // segment index is only increment in the addRegion method
   // in the case of a single segment, this particular method is never 
   // called, therefore we have to set it manually to one
   if(segment_index == 0) segment_index = 1; 
   
   domain->create_segments(segment_index);
   domain->reserve_cells(mesh->numberoftetrahedra);
   
   for(long tet_index = 0; tet_index < mesh->numberoftetrahedra; ++tet_index)
   {
      integer_type index = tet_index * 4; 

      vertex_type *vertices[CELLSIZE];

      vertices[0] = &(domain->vertex( mesh->tetrahedronlist[index] ));
      vertices[1] = &(domain->vertex( mesh->tetrahedronlist[index+1] ));
      vertices[2] = &(domain->vertex( mesh->tetrahedronlist[index+2] ));
      vertices[3] = &(domain->vertex( mesh->tetrahedronlist[index+3] ));                           

      // only access triangle attributes if there are any
      // otherwise we get ourselves a segfault
      // if, for example, there is only one region, 
      // there is no need to call "add<region>", therefore
      // we have to counter this case
      if(mesh->numberoftetrahedronattributes > 0)
      {
         seg_id = mesh->tetrahedronattributelist[tet_index];
      }

   #ifdef MESH_KERNEL_DEBUG_FULL
      std::cout << "tet: " << tet_index << " : " << vertices[0]->getID() << " " 
                                                 << vertices[1]->getID() << " " 
                                                 << vertices[2]->getID() << " " 
                                                 << vertices[3]->getID() << " " 
                                                 << "segid: " << mesh->tetrahedronattributelist[tet_index] 
                                                 << std::endl;
   #endif
      
   #ifdef MESH_KERNEL_DEBUG
      seg_check[seg_id] = true;
   #endif
      cell_type cell;
      cell.setVertices(vertices);
      domain->segment(seg_id).add(cell);
   }
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - extracted segments: ";
   for( std::map<size_t, bool>::iterator iter = seg_check.begin();
        iter != seg_check.end(); iter++)
   {
      std::cout << iter->first << " ";
   }
   std::cout << std::endl;
#endif   
}
// --------------------------------------------------------------------------
template<typename PointT>
void mesh_kernel<viennamesh::tag::tetgen>::barycenter(PointT const& p1, PointT const& p2, PointT const& p3, PointT const& p4, PointT & result)
{
   result[0] = (p1[0] + p2[0] + p3[0] + p4[0])/4.;
   result[1] = (p1[1] + p2[1] + p3[1] + p4[1])/4.;
   result[2] = (p1[2] + p2[2] + p3[2] + p4[2])/4.;      
}
// --------------------------------------------------------------------------
template<typename PointT>
void mesh_kernel<viennamesh::tag::tetgen>::addPoint(PointT const& pnt)
{  std::cout << "adding point: " << pnt[0] << " " << pnt[1] << std::endl;
   this->extendPoints();
   std::cout << "back" << std::endl;
   this->in->pointlist[pointlist_index] = pnt[0];
   this->pointlist_index++;
   this->in->pointlist[pointlist_index] = pnt[1];
   this->pointlist_index++;             
   this->in->pointlist[pointlist_index] = pnt[2];
   this->pointlist_index++;                   
}
template<typename PointT>
void mesh_kernel<viennamesh::tag::tetgen>::addPoint(PointT & pnt)
{  //std::cout << "adding point: " << pnt[0] << " " << pnt[1] << std::endl;
   this->extendPoints();
   this->in->pointlist[pointlist_index] = pnt[0];
   this->pointlist_index++;
   this->in->pointlist[pointlist_index] = pnt[1];
   this->pointlist_index++;             
   this->in->pointlist[pointlist_index] = pnt[2];
   this->pointlist_index++;           
}

template<typename ConstraintT>
void mesh_kernel<viennamesh::tag::tetgen>::addConstraint(ConstraintT const& constraint)
{
   extendFacets();

   tetgenio::facet   *f;
   tetgenio::polygon *p;   

   f = &in->facetlist[mesher_facet_index];
   f->numberofpolygons = 1;
   f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
   f->numberofholes = 0;
   f->holelist = NULL;
   p = &f->polygonlist[0];
   p->numberofvertices = constraint.size();
   p->vertexlist = new int[p->numberofvertices];
   
   for(int i = 0; i < p->numberofvertices; i++)
   {
     p->vertexlist[i] = constraint[i];
   }
   mesher_facet_index++;   
}
template<typename ConstraintT>
void mesh_kernel<viennamesh::tag::tetgen>::addConstraint(ConstraintT & constraint)
{  //std::cout << "adding const: " << constraint[0] << " " << constraint[1] << std::endl;
   extendFacets();

   tetgenio::facet   *f;
   tetgenio::polygon *p;   

   f = &in->facetlist[mesher_facet_index];
   f->numberofpolygons = 1;
   f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
   f->numberofholes = 0;
   f->holelist = NULL;
   p = &f->polygonlist[0];
   p->numberofvertices = constraint.size();
   p->vertexlist = new int[p->numberofvertices];
   for(int i = 0; i < p->numberofvertices; i++)
   {
     p->vertexlist[i] = constraint[i];
   }

   mesher_facet_index++;      
}   

template<typename PointT>
void mesh_kernel<viennamesh::tag::tetgen>::addRegion(PointT const& pnt)
{
   extendRegions();
   in->regionlist[regionlist_index] = pnt[0];
   regionlist_index++;
   in->regionlist[regionlist_index] = pnt[1];
   regionlist_index++;
   in->regionlist[regionlist_index] = pnt[2];
   regionlist_index++;      
   in->regionlist[regionlist_index] = numeric_type(segment_index);  // assign region id
   regionlist_index++;
   in->regionlist[regionlist_index] = numeric_type(segment_index);  // assign region id
   regionlist_index++;
   segment_index++;  
}
template<typename PointT>
void mesh_kernel<viennamesh::tag::tetgen>::addRegion(PointT & pnt)
{  
   extendRegions();
   in->regionlist[regionlist_index] = pnt[0];
   regionlist_index++;
   in->regionlist[regionlist_index] = pnt[1];
   regionlist_index++;
   in->regionlist[regionlist_index] = pnt[2];
   regionlist_index++;
   in->regionlist[regionlist_index] = numeric_type(segment_index);  // assign region id
   regionlist_index++;
   in->regionlist[regionlist_index] = numeric_type(segment_index);  // assign region id
   regionlist_index++;
   segment_index++;
}   
// --------------------------------------------------------------------------

void mesh_kernel<viennamesh::tag::tetgen>::extendPoints()
{
   if ( !(this->in->pointlist))
      in->pointlist   = (numeric_type *)malloc( 3 * sizeof(numeric_type) );
   else
      in->pointlist = (numeric_type *)realloc ( in->pointlist, (in->numberofpoints+1) * 3 * sizeof(numeric_type));
   in->numberofpoints++;
}

void mesh_kernel<viennamesh::tag::tetgen>::extendFacets()
{
   if ( !in->facetlist)
      in->facetlist   = (tetgenio::facet *)malloc( 2 * sizeof(tetgenio::facet) );
   else   in->facetlist = (tetgenio::facet *)realloc ( in->facetlist, (in->numberoffacets+1) * 2 * sizeof(tetgenio::facet));
   in->numberoffacets++;
}
void mesh_kernel<viennamesh::tag::tetgen>::extendRegions()
{
   if ( !in->regionlist)
      in->regionlist   = (numeric_type *)malloc( 5 * sizeof(numeric_type) );
   else   in->regionlist = (numeric_type *)realloc ( in->regionlist, (in->numberofregions+1) * 5 * sizeof(numeric_type));
   in->numberofregions++;
}

// --------------------------------------------------------------------------

tetgenio * mesh_kernel<viennamesh::tag::tetgen>::init_kernel(tetgenio *io)
{
   io = new tetgenio;
   io->numberofpoints = 0;
   io->numberoffacets = 0;
   io->numberofregions = 0;      
   return io;
}

void mesh_kernel<viennamesh::tag::tetgen>::init()
{
   in  = init_kernel(in);
   out = init_kernel(out);
   options = "";
   pointlist_index               = 0;
   regionlist_index              = 0;
   mesher_facet_index            = 0;     
   constraint_list_index         = 0;
   segment_index        = 0;           
}

void mesh_kernel<viennamesh::tag::tetgen>::clear()
{
   this->freeMem();
}

void mesh_kernel<viennamesh::tag::tetgen>::reset()
{
   this->clear();
   this->init();
}

void mesh_kernel<viennamesh::tag::tetgen>::freeMem()
{
   tetgenio::facet   *f;
   tetgenio::polygon *p;   

   for(long facet_index = 0; facet_index <  in->numberoffacets; ++facet_index)
   {
      f = &in->facetlist[facet_index];    
      p = &f->polygonlist[0];

      delete[] p->vertexlist;
      delete[] f->polygonlist;  
   }      
   if(in->pointlist)           free(in->pointlist);
   if(in->facetlist)           free(in->facetlist);   
   
   in->pointlist = NULL;
   in->facetlist = NULL;
   
   delete[] out->pointmarkerlist;            

   delete in;
   delete out;
}

} // end namespace viennamesh



