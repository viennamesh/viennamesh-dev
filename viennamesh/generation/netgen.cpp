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

// *** vienna includes
#include "viennautils/dumptype.hpp"
#include "viennagrid/domain.hpp"

// *** local includes
#include "netgen.hpp"

// *** boost includes 
#include <boost/make_shared.hpp>

namespace nglib {
#include "nglib.h"
}



namespace viennamesh {

// --------------------------------------------------------------------------
mesh_kernel<viennamesh::tag::netgen>::mesh_kernel()
{
   mesh_kernel_id = "Netgen";      
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - initiating" << std::endl;
#endif
}
// --------------------------------------------------------------------------
mesh_kernel<viennamesh::tag::netgen>::~mesh_kernel()
{
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - shutting down" << std::endl;
   #endif
}
// --------------------------------------------------------------------------
template<typename DatastructureT>
mesh_kernel<viennamesh::tag::netgen>::result_type 
mesh_kernel<viennamesh::tag::netgen>::operator()(DatastructureT& data)
{
   std::size_t segment_size = data.segment_size();
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing segments" << std::endl;
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - detected segments: " << segment_size << std::endl;
   if( segment_size > 1 )
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - dealing with multi-segment input" << std::endl;
   else
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - dealing with single-segment input" << std::endl;
#endif                 
   const double value_min = 1.0e-15;   
   
   //
   // *** Extract the geometry and topology data of the wrapped datastructure
   //     and transfer it to the mesh datastructure
   //
   typedef typename DatastructureT::segment_iterator  vmesh_segment_iterator;
   typedef typename DatastructureT::geometry_iterator vmesh_geometry_iterator;   
   typedef typename DatastructureT::cell_type         vmesh_cell_type;
   typedef typename DatastructureT::cell_iterator     vmesh_cell_iterator;      
   typedef typename DatastructureT::point_type        vmesh_point_type;         
   
   for(vmesh_segment_iterator seg_iter = data.segment_begin();
      seg_iter != data.segment_end(); seg_iter++)
   {
      std::map<std::size_t, bool>         vertex_uniquer;
      std::map<std::size_t, std::size_t>  segment_vertices;

      // create a new mesh datastructure for this segment
      // the reason we use a pointer approach here, is that the 
      // netgen::mesh object does not like to be placed within a vector ..
      // however, this approach should not hit on performance, as nothing gets copied ..
      //
      mesh_pointer_type meshpnt = mesh_pointer_type(nglib::Ng_NewMesh());
   
      for(vmesh_cell_iterator cit = (*seg_iter).cell_begin();
          cit != (*seg_iter).cell_end(); cit++)
      {  
         vmesh_cell_type vmesh_cell = *cit;
         std::size_t localcounter = 0;            

         // TODO we should be able to use a ct-container here ..
         typedef std::vector<std::size_t>  cell_vertex_mapping;
         cell_vertex_mapping cell;
         cell.resize(vmesh_cell.size());  
         
         // traverse the vertices of the current cell
         for(std::size_t i = 0; i < vmesh_cell.size(); i++)  
         {
            std::size_t vertex_index = vmesh_cell[i];
            if(!vertex_uniquer[vertex_index])
            {
               // retrieve the geometry information
               vmesh_geometry_iterator geoit = data.geometry_begin();
               std::advance(geoit, vertex_index);
               vmesh_point_type point = *geoit;
               numeric_type netgen_point[DIMG];
               
               // set very small values to zero to increase
               // numerical stability
               if(std::abs(point[0]) <= value_min)
                  netgen_point[0] = 0;
               else netgen_point[0] = point[0];
               
               if(std::abs(point[1]) <= value_min)
                  netgen_point[1] = 0;
               else netgen_point[1] = point[1];
               
               if(std::abs(point[2]) <= value_min)
                  netgen_point[2] = 0;
               else netgen_point[2] = point[2];
               
               //std::cout << "adding point: " << netgen_point << std::endl;
               
               // add the point to the mesh datastructure
               nglib::Ng_AddPoint (&(*meshpnt), netgen_point);
                  
               // build a new cell with the correct vertex indices
               cell[localcounter] = nglib::Ng_GetNP(&(*meshpnt));
               
               // map the input vertex index with index within the mesh datastructure
               segment_vertices[vertex_index] =  nglib::Ng_GetNP(&(*meshpnt));
               
               // ensure that this vertex is not added again
               vertex_uniquer[vertex_index] = true;
            }
            else // point has been already added, reroute indices
            {
               cell[localcounter] = segment_vertices[vertex_index];
            }
            localcounter++;
         }
         
         // build netgen cell
         integer_type el[3];
         el[0] = cell[0];
         el[1] = cell[1];
         el[2] = cell[2];   
         
         //std::cout << "adding constraint: " << el << std::endl;
         
         // add netgen cell to mesh structure            
         nglib::Ng_AddSurfaceElement (&(*meshpnt), nglib::NG_TRIG, el);
      }
      mesh_container.push_back(meshpnt);         
   }            

   domain_ptr_type domain(new domain_type);         

   this->do_meshing(domain);

   return domain;

}
// --------------------------------------------------------------------------
mesh_kernel<viennamesh::tag::netgen>::result_type 
mesh_kernel<viennamesh::tag::netgen>::operator()(viennagrid::domain<viennagrid::config::triangular_3d>& hull_domain) 
{
   return (*this)(boost::make_shared< viennagrid::domain<viennagrid::config::triangular_3d> >(hull_domain));
}
// --------------------------------------------------------------------------
mesh_kernel<viennamesh::tag::netgen>::result_type 
mesh_kernel<viennamesh::tag::netgen>::operator()(boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> > hull_domain) 
{
   typedef viennagrid::domain<viennagrid::config::triangular_3d>  HullDomain;
   typedef HullDomain::config_type                                HullDomainConfiguration;      
   typedef HullDomain::segment_type                               HullSegmentType;      
   typedef HullDomainConfiguration::cell_tag                      HullCellTag;      

   typedef viennagrid::result_of::point_type<HullDomainConfiguration>::type                                 HullPointType;   
   typedef viennagrid::result_of::ncell_type<HullDomainConfiguration, HullCellTag::topology_level>::type    HullCellType;      
   typedef viennagrid::result_of::ncell_container<HullSegmentType, HullCellTag::topology_level>::type       HullCellContainer;      
   typedef viennagrid::result_of::iterator<HullCellContainer>::type                                         HullCellIterator;         
   typedef viennagrid::result_of::ncell_container<HullCellType, 0>::type                                    HullVertexOnCellContainer;
   typedef viennagrid::result_of::iterator<HullVertexOnCellContainer>::type                                 HullVertexOnCellIterator;         

   std::size_t segment_size = hull_domain->segment_size();
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing segments" << std::endl;
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - detected segments: " << segment_size << std::endl;
   if( segment_size > 1 )
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - dealing with multi-segment input" << std::endl;
   else
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - dealing with single-segment input" << std::endl;
#endif                 
   const double value_min = 1.0e-15;      

   //
   // *** Extract the geometry and topology data of the ViennaGrid domain
   //     and transfer it to the mesh datastructure
   //
   for (std::size_t si = 0; si < hull_domain->segment_size(); ++si)
   {
      //std::cout << "extracting segment: " << si << std::endl;
   
      std::map<std::size_t, bool>         vertex_uniquer;
      std::map<std::size_t, std::size_t>  segment_vertices;
   
      // create a new mesh datastructure for this segment
      // the reason we use a pointer approach here, is that the 
      // netgen::mesh object does not like to be placed within a vector ..
      // however, this approach should not hit on performance, as nothing gets copied ..
      //
      mesh_pointer_type meshpnt = mesh_pointer_type(nglib::Ng_NewMesh());      
   
      HullSegmentType & seg = hull_domain->segment(si);      
      HullCellContainer cells = viennagrid::ncells<HullCellTag::topology_level>(seg);      
      
      for (HullCellIterator cit = cells.begin(); cit != cells.end(); ++cit)
      {
         // TODO we should be able to use a ct-container here ..
         typedef std::vector<std::size_t>  cell_vertex_mapping;
         cell_vertex_mapping  cell;
         
         HullVertexOnCellContainer vertices_for_cell = viennagrid::ncells<0>(*cit);
         for (HullVertexOnCellIterator vocit = vertices_for_cell.begin();
             vocit != vertices_for_cell.end();
             ++vocit)
         {
            std::size_t vertex_index = vocit->getID();
            
            if(!vertex_uniquer[vertex_index])
            {
               // retrieve the geometry information
               HullPointType point = vocit->getPoint();

               numeric_type netgen_point[DIMG];               
               // set very small values to zero to increase
               // numerical stability
               if(std::abs(point[0]) <= value_min)
                  netgen_point[0] = 0;
               else netgen_point[0] = point[0];
               
               if(std::abs(point[1]) <= value_min)
                  netgen_point[1] = 0;
               else netgen_point[1] = point[1];
               
               if(std::abs(point[2]) <= value_min)
                  netgen_point[2] = 0;
               else netgen_point[2] = point[2];

               // add the point to the mesh datastructure
               nglib::Ng_AddPoint (&(*meshpnt), netgen_point);
                  
               // build a new cell with the correct vertex indices
               cell.push_back( nglib::Ng_GetNP(&(*meshpnt)) );
               
               // map the input vertex index with index within the mesh datastructure
               segment_vertices[vertex_index] =  nglib::Ng_GetNP(&(*meshpnt));
               
               // ensure that this vertex is not added again
               vertex_uniquer[vertex_index] = true;
            }
            else
            {
               cell.push_back( segment_vertices[vertex_index] );
            }
         }                        
         
         // build netgen cell
         integer_type el[3];
         el[0] = cell[0];
         el[1] = cell[1];
         el[2] = cell[2];   
         
         //std::cout << "adding constraint: " << el << std::endl;
         
         // add netgen cell to mesh structure            
         nglib::Ng_AddSurfaceElement (&(*meshpnt), nglib::NG_TRIG, el);            
      }
      mesh_container.push_back(meshpnt);         
   }

   domain_ptr_type domain(new domain_type);         
   
   this->do_meshing(domain);
   
   return domain;
}


// --------------------------------------------------------------------------
void mesh_kernel<viennamesh::tag::netgen>::do_meshing(result_type  domain)
{
   nglib::Ng_Meshing_Parameters  mesh_parameters;   



   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - meshing:" << std::endl;
      std::cout << "  input region size: " << mesh_container.size() << std::endl;            
/*         std::cout << "  parameters: " << std::endl;
      std::cout << "  ----------------------------------------" << std::endl;
      mesh_parameters.Print(std::cout, "  ");
      std::cout << "  ----------------------------------------" << std::endl;        */ 
      std::size_t region_cnt = 0;
   #endif            
   
   // count the current mesh aka the current segment
   std::size_t mesh_cnt = 0;
   // threadID ensures that calclocalH and meshvolume get the same 
   // threadID. it is not used currently for parallel meshing, 
   // but for future extensions .. 
   //std::size_t threadID = 0;

   // map ensures a unique point set
   // note: as we process one segment after another, 
   // we need to keep track of the points of the previously processed segments ..
   typedef boost::array<numeric_type, DIMG>           point_type;
   typedef std::map<point_type, std::size_t>          vertex_map_type;
   typedef vertex_map_type::iterator                  vertex_map_iterator_type;      
   vertex_map_type vertex_domain_mapping;
   
   // we have to use a temporary cell container too .. for the same reasons
   typedef boost::array<int,CELL_SIZE>                 bngcell_type;
   typedef std::vector<bngcell_type>                   cell_cont_type;
   typedef std::map<std::size_t, cell_cont_type >   segment_cell_map_type;
   segment_cell_map_type segment_cell_map;
   
   // this container is a temporary container, which stores 
   // the viennagrid vertices.
   vertex_container_type vertex_container;
   
   // counts the global number of unique points
   std::size_t point_cnt = 0;
   
   // and the global number of cells ..
   std::size_t global_cell_size = 0;
   
   for(mesh_iterator_type mesh_iter = mesh_container.begin();
       mesh_iter != mesh_container.end(); mesh_iter++)
   {
       mesh_pointer_type meshpnt = *mesh_iter;

   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - region: " << region_cnt++ << std::endl;
      //std::cout << "  parameter set:     " << options << std::endl;
      std::cout << "  input point size:  " << nglib::Ng_GetNP(&(*meshpnt)) << std::endl;
      std::cout << "  input const size:  " << nglib::Ng_GetNSE(&(*meshpnt)) << std::endl;      
      std::cout << "## MeshKernel::"+mesh_kernel_id+" starting mesh generation " << std::endl;         
   #endif        

      nglib::Ng_GenerateVolumeMesh (&(*meshpnt), &mesh_parameters); 

   #ifdef MESH_KERNEL_DEBUG         
      // write a volume file for each segment
      // can be viewed with, for example, netgen
      //
      std::string filename = "segment_" + boost::lexical_cast<std::string>(mesh_cnt) + ".vol";
      nglib::Ng_SaveMesh(&(*meshpnt),filename.c_str());  
   #endif
               
      
      long point_size   = nglib::Ng_GetNP(&(*meshpnt));
      long element_size = nglib::Ng_GetNE(&(*meshpnt));
      
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - finished:" << std::endl;
      std::cout << "  output point size:  " << point_size << std::endl;
      std::cout << "  output cell size:   " << element_size << std::endl;      
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - extracting geometry" << std::endl;
   #endif            
      
      std::map<std::size_t, std::size_t>  mesh_domain_mapping; 


      
      for(long i = 1; i <= point_size; i++)
      {
         numeric_type point[DIMG];
         nglib::Ng_GetPoint (&(*meshpnt), i, point);

         // note: we cannot use the array point for the lookup table,
         // as arrays do not fulfill the map requirements, ie, copy constructible
         // therefore we have to create a temporary point object, a boost array ..
         point_type bpoint = {{ point[0],point[1],point[2] }}; 
         vertex_map_iterator_type vdm_it = vertex_domain_mapping.find(bpoint);

         if(vdm_it == vertex_domain_mapping.end())          // point is not in the map
         {   
            vertex_domain_mapping[bpoint] = point_cnt;
            mesh_domain_mapping[i]       = point_cnt;

            vertex_type    vertex;
            vertex.getPoint()[0] = point[0];
            vertex.getPoint()[1] = point[1];         
            vertex.getPoint()[2] = point[2];                                 
            vertex.setID(point_cnt);
      
            // we cannot directly push the vertices on the viennagrid domain, 
            // as we have to know, how many in total we have. 
            // therefore, we collect em in this container, and after all segments 
            // have been processed, we push them alltogether on the domain
            vertex_container.push_back(vertex);

            point_cnt++;
         }
         else  // point is already in the map - use this handle
         {
            mesh_domain_mapping[i] = (*vdm_it).second;    
         }            

      }
      
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - extracting topology" << std::endl;
   #endif          
      for(long i = 1; i <= element_size; i++)
      {
         int ngcell[CELL_SIZE]; 
         
         nglib::Ng_GetVolumeElement (&(*meshpnt), i, ngcell);            
            
//            vertex_type *vertices[CELL_SIZE];      

         // sadly we have to copy the cell array into a boost array, 
         // to be able to move it into the dynamic vector container.
         // a normal array is not copyable ..  
         //
         bngcell_type  bngcell;
         for(int ci = 0; ci < CELL_SIZE; ci++)
         {
            bngcell[ci] = mesh_domain_mapping[ngcell[ci]];
            //vertices[ci] = &(domain->vertex(ngcell[ci]));
         }

//            cell_type cell;
//            cell.setVertices(vertices);
//            cell.setID(global_cell_size);            

//            std::cout << "  extracting cell: "; cell.print();

//            segment_cell_map[mesh_cnt].push_back(cell);
           segment_cell_map[mesh_cnt].push_back(bngcell);            
       
         global_cell_size++;
      }
      mesh_cnt++;
    }            
    
   domain->reserve_vertices(vertex_container.size());       
   for(vertex_container_type::iterator iter = vertex_container.begin();
       iter != vertex_container.end(); iter++)
   {
      domain->add(*iter);
   }

   //std::cout << "mesh cnt: " << mesh_cnt << std::endl;
   domain->create_segments(mesh_cnt);             
    
   // now that all segments and cells have been read, and due to that 
   // we are aware of how many cells in total there are, 
   // we can actually create the cells within the domain
   //std::cout << "global cell size: " << global_cell_size << std::endl;
   domain->reserve_cells(global_cell_size);

   std::size_t cell_id = 0;
   for(segment_cell_map_type::iterator sit = segment_cell_map.begin();
       sit != segment_cell_map.end(); sit++)
   {
      for(cell_cont_type::iterator cit = sit->second.begin();
          cit != sit->second.end(); cit++)
      {
//            std::cout << "adding cell: "; 
//            cit->print();
//            std::cout << " to segment: " << sit->first << std::endl;
         
         vertex_type *vertices[CELL_SIZE];      

         for(int ci = 0; ci < CELL_SIZE; ci++)
         {
            vertices[ci] = &(domain->vertex( (*cit)[ci] ));
         }            
         
         cell_type cell;
         cell.setVertices(vertices);
         cell.setID(cell_id++);          
         domain->segment(sit->first).add(cell);            
      }
   }       

   mesh_container.clear(); // cleanup ..   
}
// -----------------------------------------------------------------------------
// 
// explicit declarations for the template functions
// 
template mesh_kernel<viennamesh::tag::netgen>::result_type 
mesh_kernel<viennamesh::tag::netgen>::operator()(viennamesh::wrapper<viennamesh::tag::bnd, viennautils::io::bnd_reader>& data);

template mesh_kernel<viennamesh::tag::netgen>::result_type 
mesh_kernel<viennamesh::tag::netgen>::operator()(viennamesh::wrapper<viennamesh::tag::hin, viennautils::io::hin_reader>& data);
// -----------------------------------------------------------------------------

} // end namespace viennamesh



