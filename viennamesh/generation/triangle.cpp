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
#include "triangle.hpp"

// *** boost includes


// *** Triangle includes
// note: macros need to be set before including triangle.h ..
#define VOID void
#define ANSI_DECLARATORS
#define REAL double
extern "C"  { 
   #include "triangle/triangle.h" 
}

namespace viennamesh {


// --------------------------------------------------------------------------
mesh_kernel<viennamesh::tag::triangle>::mesh_kernel()
{
   mesh_kernel_id = "Triangle";      
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - initiating" << std::endl;
#endif
}
// --------------------------------------------------------------------------
mesh_kernel<viennamesh::tag::triangle>::~mesh_kernel()
{
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - shutting down" << std::endl;
   #endif
}
// --------------------------------------------------------------------------
mesh_kernel<viennamesh::tag::triangle>::result_type 
mesh_kernel<viennamesh::tag::triangle>::operator()(viennamesh::wrapper<viennamesh::tag::bnd, viennautils::io::bnd_reader>& data) // default meshing
{ 
   typedef viennamesh::wrapper<viennamesh::tag::bnd, viennautils::io::bnd_reader> DatastructureT;
   this->init();      

   options = "zpD";  // conforming delaunay
   // options = "zp"; // constrained delaunay
   // options = "z"; // convex

   typedef DatastructureT::segment_iterator  vmesh_segment_iterator;
   typedef DatastructureT::cell_type         vmesh_cell_type;
   typedef DatastructureT::cell_iterator     vmesh_cell_iterator;   

      std::size_t segment_size = data.segment_size();
      //size_t segment_size = data.domain().segment_container()->size();
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
         std::size_t seg_cnt = 0;
      #endif      
         point_type pnt;
         for(vmesh_segment_iterator seg_iter = data.segment_begin();
            seg_iter != data.segment_end(); seg_iter++)
         {         
            DatastructureT::cell_complex_wrapper_type segment = *seg_iter;
            this->find_point_in_segment(data, segment, pnt);
            region_points.push_back(pnt);
         #ifdef MESH_KERNEL_DEBUG
            std::cout << "## MeshKernel::"+mesh_kernel_id+" - computed point in segment " 
               << seg_cnt << " : " << pnt[0] << " " << pnt[1] << std::endl;
            seg_cnt++;
         #endif
         }
         for(region_points_type::iterator iter = region_points.begin();
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
      std::cout << std::endl;
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing geometry" << std::endl;
   #endif
      typedef DatastructureT::geometry_iterator geometry_iterator;
      std::size_t pnt_cnt = 0;
      for(geometry_iterator iter = data.geometry_begin();
          iter != data.geometry_end(); iter++)
      {
         
      #ifdef MESH_KERNEL_DEBUG
          std::cout << "## MeshKernel::"+mesh_kernel_id+" - adding point " << 
             pnt_cnt++ << " : " << (*iter)[0] << " " << (*iter)[1] << std::endl;
      #endif   
         this->addPoint(*iter);
      }

      // traverse and add the topology information
      // aka the set of boundary constraints
      //
   #ifdef MESH_KERNEL_DEBUG
      std::cout << std::endl;   
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing constraints" << std::endl;
   #endif
      size_t si = 0;

      typedef boost::array<std::size_t, 2>   boost_cell_type;
      std::map<boost_cell_type, bool>        cell_uniquer;      
   #ifdef MESH_KERNEL_DEBUG      
      size_t cell_cnt = 0;
   #endif

      for(vmesh_segment_iterator seg_iter = data.segment_begin();
         seg_iter != data.segment_end(); seg_iter++)
      {
         for(vmesh_cell_iterator cit = (*seg_iter).cell_begin();
             cit != (*seg_iter).cell_end(); cit++)
         {
            vmesh_cell_type vmesh_cell = *cit;
            boost_cell_type cell;
            
            for(size_t i = 0; i < vmesh_cell.size(); i++)  
               cell[i] = vmesh_cell[i];
            
            std::sort(cell.begin(), cell.end());

            if(!cell_uniquer[cell])
            {
            #ifdef MESH_KERNEL_DEBUG
               std::cout << "## MeshKernel::"+mesh_kernel_id+" - adding constraint   " << 
                  cell_cnt << " : ";
               for(size_t i = 0; i < cell.size(); i++)  
                  std::cout << cell[i] << " ";
               std::cout << std::endl;
               cell_cnt++;
            #endif               
               cell_uniquer[cell] = true;
               
               this->addConstraint(cell);
            }
         #ifdef MESH_KERNEL_DEBUG            
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
         si++;
      }      
   
      domain_ptr_type domain(new domain_type);               

      this->do_meshing(domain);

      this->clear();   
      
      return domain;   
}
// --------------------------------------------------------------------------
mesh_kernel<viennamesh::tag::triangle>::result_type 
mesh_kernel<viennamesh::tag::triangle>::operator()(boost::shared_ptr< viennagrid::domain<viennagrid::config::line_2d> > hull_domain) 
{
   this->init();         
   options = "zpD";  // conforming delaunay
   // options = "zp"; // constrained delaunay
   // options = "z"; // convex

      typedef viennagrid::domain<viennagrid::config::line_2d>        HullDomain;
      typedef HullDomain::config_type                                HullDomainConfiguration;      
      typedef HullDomain::segment_type                               HullSegmentType;      
      typedef HullDomainConfiguration::cell_tag                      HullCellTag;      
      typedef viennagrid::result_of::point_type<HullDomainConfiguration>::type                                 HullPointType;   
      typedef viennagrid::result_of::ncell_container<HullDomain, 0>::type                                      HullPointContainer;            
      typedef viennagrid::result_of::iterator<HullPointContainer>::type                                        HullPointIterator;            
      typedef viennagrid::result_of::ncell_type<HullDomainConfiguration, HullCellTag::topology_level>::type    HullCellType;      
      typedef viennagrid::result_of::ncell_container<HullSegmentType, HullCellTag::topology_level>::type       HullCellContainer;      
      typedef viennagrid::result_of::iterator<HullCellContainer>::type                                         HullCellIterator;         
      typedef viennagrid::result_of::ncell_container<HullCellType, 0>::type                                    HullVertexOnCellContainer;
      typedef viennagrid::result_of::iterator<HullVertexOnCellContainer>::type                                 HullVertexOnCellIterator;                     
      
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
         for(region_points_type::iterator iter = region_points.begin();
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
            // note: triangle does _not_ require the input cells to be 
            // oriented in any manner - however, maybe triangle derives 
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
void mesh_kernel<viennamesh::tag::triangle>::do_meshing(domain_ptr_type domain)
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
   std::cout << "  input const size:  " << in->numberofsegments << std::endl;      
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
   triangulate(buffer, in, out, 0);

   free(buffer);
   
   if ( !out->pointlist)      
      std::cout << "\t::ERROR: pointlist" << std::endl;
   if ( !out->trianglelist)   
      std::cout << "\t::ERROR: trianglelist " << std::endl;
   
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - finished:" << std::endl;
   std::cout << "  output point size:  " << out->numberofpoints << std::endl;
   std::cout << "  output cell size:   " << out->numberoftriangles << std::endl;      
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - extracting geometry" << std::endl;
#endif               

   transfer_to_domain(domain, out);
}   


// --------------------------------------------------------------------------

template<typename PointT>
void mesh_kernel<viennamesh::tag::triangle>::find_point_in_segment(boost::shared_ptr< viennagrid::domain<viennagrid::config::line_2d> >       hull_domain,
                           typename viennagrid::domain<viennagrid::config::line_2d>::segment_type&    seg, 
                           PointT& pnt, std::size_t segid = 0)
{
      typedef viennagrid::domain<viennagrid::config::line_2d>        HullDomain;
      typedef HullDomain::config_type                                HullDomainConfiguration;      
      typedef HullDomain::segment_type                               HullSegmentType;         
      typedef HullDomainConfiguration::cell_tag                      HullCellTag;            
   
      typedef viennagrid::result_of::point_type<HullDomainConfiguration>::type                                 HullPointType;   
      typedef viennagrid::result_of::ncell_type<HullDomainConfiguration, HullCellTag::topology_level>::type    HullCellType;         
      typedef viennagrid::result_of::ncell_container<HullSegmentType, HullCellTag::topology_level>::type       HullCellContainer;      
      typedef viennagrid::result_of::iterator<HullCellContainer>::type                                         HullCellIterator;            
      typedef viennagrid::result_of::ncell_container<HullCellType, 0>::type                                    HullVertexOnCellContainer;
      typedef viennagrid::result_of::iterator<HullVertexOnCellContainer>::type                                 HullVertexOnCellIterator;            

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
      triangulate(buffer, temp_mesher.in, temp_mesher.out, 0);
      free(buffer);

      if ( (!temp_mesher.out->pointlist) || !temp_mesher.out->trianglelist )      
         std::cout << "## MeshKernel::"+mesh_kernel_id+" [ERROR] - point in segment algorithm failed as pre-meshing failed!" << std::endl;

      PointT pnt1 (temp_mesher.out->pointlist[(temp_mesher.out->trianglelist[0]*DIMG)],
                   temp_mesher.out->pointlist[(temp_mesher.out->trianglelist[0]*DIMG+1)]);

      PointT pnt2 (temp_mesher.out->pointlist[(temp_mesher.out->trianglelist[1]*DIMG)],
                   temp_mesher.out->pointlist[(temp_mesher.out->trianglelist[1]*DIMG+1)]);

      PointT pnt3 (temp_mesher.out->pointlist[(temp_mesher.out->trianglelist[2]*DIMG)],
                   temp_mesher.out->pointlist[(temp_mesher.out->trianglelist[2]*DIMG+1)]);


      temp_mesher.barycenter(pnt1, pnt2, pnt3, pnt);

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
void mesh_kernel<viennamesh::tag::triangle>::find_point_in_segment(DatastructureT& data, 
                           typename DatastructureT::cell_complex_wrapper_type & cell_complex, 
                           point_type& pnt)
{
   self_type   temp_mesher;
   temp_mesher.init();

   typedef typename DatastructureT::geometry_iterator geometry_iterator;
   typedef typename DatastructureT::cell_type         vmesh_cell_type;      
   typedef typename DatastructureT::cell_iterator     vmesh_cell_iterator;            

   std::map<std::size_t, bool>  pnt_uniquer;
   std::map<std::size_t, std::size_t> index_map;
   std::size_t point_cnt = 0;
   typedef typename DatastructureT::geometry_iterator geometry_iterator;
   for(vmesh_cell_iterator cit = cell_complex.cell_begin();
         cit != cell_complex.cell_end(); cit++)
   {
      vmesh_cell_type cell = *cit;
      //std::cout << "region: " << cell << std::endl;
      for(int dim = 0; dim < DIMG; dim++)
      {
         if(!pnt_uniquer[cell[dim]])
         {  
            geometry_iterator geo = data.geometry_begin();
            std::advance(geo, cell[dim]);
            temp_mesher.addPoint( *geo ); 
            pnt_uniquer[cell[dim]] = true;
            index_map[cell[dim]] = point_cnt;
            point_cnt++;
         }
      }
      boost::array< numeric_type , 2 > mapped_cell;
      mapped_cell[0] = index_map[cell[0]];
      mapped_cell[1] = index_map[cell[1]];
      temp_mesher.addConstraint(mapped_cell);
      
   }
   
   std::string temp_options = "zpQ";
   char* buffer;
   buffer = (char *)malloc( temp_options.length() * sizeof(char) );
   std::strcpy(buffer,temp_options.c_str());      
   triangulate(buffer, temp_mesher.in, temp_mesher.out, 0);
   free(buffer);

   if ( (!temp_mesher.out->pointlist) || !temp_mesher.out->trianglelist )      
      std::cout << "## MeshKernel::"+mesh_kernel_id+" [ERROR] - point in segment algorithm failed as pre-meshing failed!" << std::endl;

   point_type pnt1 = {{temp_mesher.out->pointlist[(temp_mesher.out->trianglelist[0]*DIMG)],
                     temp_mesher.out->pointlist[(temp_mesher.out->trianglelist[0]*DIMG+1)]}};

   point_type pnt2 = {{temp_mesher.out->pointlist[(temp_mesher.out->trianglelist[1]*DIMG)],
                     temp_mesher.out->pointlist[(temp_mesher.out->trianglelist[1]*DIMG+1)]}};

   point_type pnt3 = {{temp_mesher.out->pointlist[(temp_mesher.out->trianglelist[2]*DIMG)],
                     temp_mesher.out->pointlist[(temp_mesher.out->trianglelist[2]*DIMG+1)]}};


   temp_mesher.barycenter(pnt1, pnt2, pnt3, pnt);

//   #ifdef MESH_KERNEL_DEBUG_FULL
//      // export the current segment to a vtk file, to investigate it ..
//      domain_ptr_type temp_domain(new domain_type);               
//      transfer_to_domain(temp_domain, temp_mesher.out);
//      viennagrid::io::vtk_writer<domain_type>  my_volume_vtk_writer;         
//      my_volume_vtk_writer.writeDomain(*temp_domain, "premesh_seg_"+boost::lexical_cast<std::string>(segid));
//   #endif

   temp_mesher.clear();      
}


void mesh_kernel<viennamesh::tag::triangle>::transfer_to_domain(domain_ptr_type domain, triangulateio *mesh)
{
      //
      // extracting geometry data
      //
      domain->reserve_vertices(mesh->numberofpoints);             

      std::size_t point_cnt = 0;
      for(long pnt_index = 0; pnt_index < mesh->numberofpoints; ++pnt_index)
      {
         integer_type index = pnt_index * 2;
         
//         numeric_type pnt[DIMG];
//         pnt[0] = mesh->pointlist[index];
//         pnt[1] = mesh->pointlist[index+1];
//         pnt[2] = mesh->pointlist[index+2];         

         vertex_type    vertex;
         vertex.getPoint()[0] = mesh->pointlist[index];
         vertex.getPoint()[1] = mesh->pointlist[index+1];
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
      domain->reserve_cells(mesh->numberoftriangles);
      
      for(long tri_index = 0; tri_index < mesh->numberoftriangles; ++tri_index)
      {
         integer_type index = tri_index * 3; 

         vertex_type *vertices[CELL_SIZE];

         vertices[0] = &(domain->vertex( mesh->trianglelist[index] ));
         vertices[1] = &(domain->vertex( mesh->trianglelist[index+1] ));
         vertices[2] = &(domain->vertex( mesh->trianglelist[index+2] ));

         // only access triangle attributes if there are any
         // otherwise we get ourselves a segfault
         // if, for example, there is only one region, 
         // there is no need to call "add<region>", therefore
         // we have to counter this case
         if(mesh->numberoftriangleattributes > 0)
         {
            seg_id = mesh->triangleattributelist[tri_index];
         }

      #ifdef MESH_KERNEL_DEBUG_FULL
         std::cout << "tet: " << tet_index << " : " << vertices[0]->getID() << " " 
                                                    << vertices[1]->getID() << " " 
                                                    << vertices[2]->getID() << " " 
                                                    << vertices[3]->getID() << " " 
                                                    << "segid: " << mesh->triangleattributelist[tet_index] 
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
void mesh_kernel<viennamesh::tag::triangle>::barycenter(PointT const& p1, PointT const& p2, PointT const& p3, PointT & result)
{
      result[0] = (p1[0] + p2[0] + p3[0])/3.;
      result[1] = (p1[1] + p2[1] + p3[1])/3.;
}
// --------------------------------------------------------------------------
template<typename PointT>
void mesh_kernel<viennamesh::tag::triangle>::addPoint(PointT const& pnt)
{  //std::cout << "adding point: " << pnt[0] << " " << pnt[1] << std::endl;
   this->extendPoints();
   this->in->pointlist[pointlist_index] = pnt[0];
   this->pointlist_index++;
   this->in->pointlist[pointlist_index] = pnt[1];
   this->pointlist_index++;             
}
template<typename PointT>
void mesh_kernel<viennamesh::tag::triangle>::addPoint(PointT & pnt)
{  //std::cout << "adding point: " << pnt[0] << " " << pnt[1] << std::endl;
   this->extendPoints();
   this->in->pointlist[pointlist_index] = pnt[0];
   this->pointlist_index++;
   this->in->pointlist[pointlist_index] = pnt[1];
   this->pointlist_index++;            
}

template<typename ConstraintT>
void mesh_kernel<viennamesh::tag::triangle>::addConstraint(ConstraintT const& constraint)
{
   extendSegments();
   in->segmentlist[segmentlist_index] = constraint[0];
   segmentlist_index++;
   in->segmentlist[segmentlist_index] = constraint[1];
   segmentlist_index++;          
}
template<typename ConstraintT>
void mesh_kernel<viennamesh::tag::triangle>::addConstraint(ConstraintT & constraint)
{  //std::cout << "adding const: " << constraint[0] << " " << constraint[1] << std::endl;
   extendSegments();
   in->segmentlist[segmentlist_index] = constraint[0];
   segmentlist_index++;
   in->segmentlist[segmentlist_index] = constraint[1];
   segmentlist_index++;         
}   

template<typename PointT>
void mesh_kernel<viennamesh::tag::triangle>::addRegion(PointT const& pnt)
{
   extendRegions();
   in->regionlist[regionlist_index] = pnt[0];
   regionlist_index++;
   in->regionlist[regionlist_index] = pnt[1];
   regionlist_index++;
   in->regionlist[regionlist_index] = numeric_type(segment_index);  // assign region id
   regionlist_index++;
   in->regionlist[regionlist_index] = numeric_type(segment_index);  // assign region id
   regionlist_index++;
   segment_index++;  
}
template<typename PointT>
void mesh_kernel<viennamesh::tag::triangle>::addRegion(PointT & pnt)
{  
   extendRegions();
   in->regionlist[regionlist_index] = pnt[0];
   regionlist_index++;
   in->regionlist[regionlist_index] = pnt[1];
   regionlist_index++;
   in->regionlist[regionlist_index] = numeric_type(segment_index);  // assign region id
   regionlist_index++;
   in->regionlist[regionlist_index] = numeric_type(segment_index);  // assign region id
   regionlist_index++;
   segment_index++;
}   
// --------------------------------------------------------------------------

void mesh_kernel<viennamesh::tag::triangle>::extendPoints()
{
   if ( !in->pointlist)
      in->pointlist   = (numeric_type *)malloc( 2 * sizeof(numeric_type) );
   else   in->pointlist = (numeric_type *)realloc ( in->pointlist, (in->numberofpoints+1) * 2 * sizeof(numeric_type));
   in->numberofpoints++;
}

void mesh_kernel<viennamesh::tag::triangle>::extendSegments()
{
   if ( !in->segmentlist)
      in->segmentlist   = (integer_type *)malloc( 2 * sizeof(integer_type) );
   else   in->segmentlist = (integer_type *)realloc ( in->segmentlist, (in->numberofsegments+1) * 2 * sizeof(integer_type));
   in->numberofsegments++;
}
void mesh_kernel<viennamesh::tag::triangle>::extendRegions()
{
   if ( !in->regionlist)
      in->regionlist   = (numeric_type *)malloc( 4 * sizeof(numeric_type) );
   else   in->regionlist = (numeric_type *)realloc ( in->regionlist, (in->numberofregions+1) * 4 * sizeof(numeric_type));
   in->numberofregions++;
}

// --------------------------------------------------------------------------

triangulateio * mesh_kernel<viennamesh::tag::triangle>::init_kernel(triangulateio *io)
{
   io = new triangulateio;
   io->pointlist = io->pointattributelist = 0;
   io->pointmarkerlist   = 0;
   io->numberofpoints = io->numberofpointattributes = 0;
   io->trianglelist = 0;
   io->triangleattributelist = io->trianglearealist = 0;
   io->neighborlist = 0;
   io->numberoftriangles = io->numberofcorners = 0;
   io->numberoftriangleattributes = 0;
   io->segmentlist = io->segmentmarkerlist = 0;
   io->numberofsegments = 0;
   io->holelist = 0; 
   io->numberofholes = 0;
   io->regionlist = 0;
   io->numberofregions = 0;
   io->edgelist = io->edgemarkerlist = 0;
   io->normlist = 0;
   io->numberofedges = 0;
   return io;
}

void mesh_kernel<viennamesh::tag::triangle>::init()
{
   in  = init_kernel(in);
   out = init_kernel(out);

   options = "";
   pointlist_index      = 0;
   segmentlist_index    = 0;
   regionlist_index     = 0;
   holelist_index       = 0;
   segment_index        = 0;             

}

void mesh_kernel<viennamesh::tag::triangle>::clear()
{
   this->freeMem();
}

void mesh_kernel<viennamesh::tag::triangle>::reset()
{
   this->clear();
   this->init();
}

void mesh_kernel<viennamesh::tag::triangle>::freeMem()
{
      if (in->pointlist != NULL)
      {  
         free (in->pointlist);
         in->pointlist = NULL;
      }
      if (in->pointattributelist != NULL)
      {
         free (in->pointattributelist);
         in->pointattributelist = NULL;
      }
      if (in->pointmarkerlist != NULL)
      {
         free (in->pointmarkerlist);
         in->pointmarkerlist = NULL;
      }
      if (in->trianglelist != NULL)
      {
         free (in->trianglelist);
         in->trianglelist = NULL;
      }
      if (in->triangleattributelist != NULL)   
      {
         free (in->triangleattributelist);
         in->triangleattributelist = NULL;
      }
      if (in->neighborlist != NULL)
      {
         free (in->neighborlist);
         in->neighborlist = NULL;
      }
      if (in->segmentlist != NULL)
      {
         free (in->segmentlist);
         in->segmentlist = NULL;
      }
      if (in->segmentmarkerlist != NULL)
      {
         free (in->segmentmarkerlist);
         in->segmentmarkerlist = NULL;
      }
      if (in->edgelist != NULL)               
      {
         free (in->edgelist);
         in->edgelist = NULL;
      }
      if (in->edgemarkerlist != NULL)        
      {
         free (in->edgemarkerlist);
         in->edgemarkerlist = NULL;
      }
      if (in->normlist != NULL)               
      {
         free (in->normlist);
         in->normlist = NULL;
      }

   delete in;
   delete out;
}



} // end namespace viennamesh
