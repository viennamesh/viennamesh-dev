/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_INTERFACE_NETGEN_HPP
#define VIENNAMESH_INTERFACE_NETGEN_HPP

// *** system includes
#include <vector>
#include <map>

// *** boost includes
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/make_map.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

// *** vienna includes
#include "viennautils/dumptype.hpp"
#include "viennamesh/interfaces/base.hpp"
#include "viennamesh/tags.hpp"

namespace nglib {
#include "nglib.h"
}

// TODO 
//  netgen produces tons of debug output ..
//  they use streams, here: meshing/global.cpp:
//  ostream * mycout = &cout;
//  ostream * myerr = &cerr;
//  we need a way to redirect mycout/myerr to 0 ...
//  problem is, I don't know how to access it from outside :(

namespace viennamesh {

template <typename DatastructureT>
struct mesh_kernel <viennamesh::tag::netgen, DatastructureT>
{
   // -------------------------------------------------------------------------------------      
   typedef double    numeric_type;  
   typedef int       integer_type;

   static const int DIMG = 3;
   static const int DIMT = 3;
   static const int CELL_SIZE = DIMT+1;            // this holds only for simplices
   
   typedef boost::array< numeric_type , DIMG >        point_type;
   typedef boost::array< numeric_type , CELL_SIZE >   cell_type;
   typedef std::vector < cell_type >                  topology_container_type;      
   
   typedef typename DatastructureT::segment_iterator  vmesh_segment_iterator;
   typedef typename DatastructureT::geometry_iterator vmesh_geometry_iterator;   
   typedef typename DatastructureT::cell_type         vmesh_cell_type;
   typedef typename DatastructureT::cell_iterator     vmesh_cell_iterator;      
   typedef typename DatastructureT::point_type        vmesh_point_type;         
   
   typedef boost::shared_ptr<nglib::Ng_Mesh>          mesh_pointer_type;
   typedef std::vector<mesh_pointer_type>             mesh_container_type;
   typedef typename mesh_container_type::iterator     mesh_iterator_type;         
   typedef typename std::map<point_type, std::size_t> vertex_map_type;
   typedef typename vertex_map_type::iterator         vertex_map_iterator_type;   
   
   typedef std::vector < point_type >              geometry_container_type;
   typedef std::vector <topology_container_type>   segment_container_type;         
   typedef viennamesh::tag::mesh_kernel            datastructure_type; // type is used for viennamesh::traits
   // -------------------------------------------------------------------------------------         
   
   mesh_kernel(DatastructureT & data) : data(data) 
   {
      mesh_kernel_id = "Netgen";      
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - initiating" << std::endl;
   #endif      

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
   
      std::size_t segment_cnt = 0;
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
         
         segment_cnt++;
         mesh_container.push_back(meshpnt);         
      }            
   }
   
   // -------------------------------------------------------------------------------------      
   ~mesh_kernel()
   {
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - shutting down" << std::endl;
   #endif

   }      
   // -------------------------------------------------------------------------------------      
   
   // -------------------------------------------------------------------------------------      
   void operator()() // default meshing
   {
      (*this)(boost::fusion::make_map<tag::criteria>(tag::constrained_delaunay()));
   }   
   template<typename ParametersMapT>
   void operator()(ParametersMapT const& paras )  // TODO provide ct-test if fusion::map
   {
      // redirect to reference implementation 
      ParametersMapT paras_new(paras);
      (*this)(paras_new);
   }
   template<typename ParametersMapT>
   void operator()(ParametersMapT & paras)   // TODO provide ct-test if fusion::map
   {  
      #ifdef MESH_KERNEL_DEBUG
         std::cout << "## MeshKernel::"+mesh_kernel_id+" - meshing:" << std::endl;
         std::cout << "  input region size: " << mesh_container.size() << std::endl;            
/*         std::cout << "  parameters: " << std::endl;
         std::cout << "  ----------------------------------------" << std::endl;
         mesh_parameters.Print(std::cout, "  ");
         std::cout << "  ----------------------------------------" << std::endl;        */ 
         std::size_t region_cnt = 0;
      #endif            
      
       // the mesh container size is the domain::segment size
       segment_cont.resize(mesh_container.size());         

      // count the current mesh aka the current segment
      std::size_t mesh_cnt = 0;
      // threadID ensures that calclocalH and meshvolume get the same 
      // threadID. it is not used currently for parallel meshing, 
      // but for future extensions .. 
      std::size_t threadID = 0;

      // map ensures a unique point set
      vertex_map_type vertex_domain_mapping;
      
      // counts the global number of unique points
      std::size_t point_cnt = 0;
      
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
  
        
         for(std::size_t i = 1; i <= point_size; i++)
         {
            //std::cout << "direct point output: " << (*meshpnt)[i] << std::endl;
            
            double ngpoint[DIMG];
            nglib::Ng_GetPoint (&(*meshpnt), i, ngpoint);
            point_type point = {{ ngpoint[0],ngpoint[1],ngpoint[2] }}; 
            vertex_map_iterator_type vdm_it = vertex_domain_mapping.find(point);
            if(vdm_it == vertex_domain_mapping.end())          // point is not in the map
            {   
                //std::cout << "point is new " << std::endl;
                //std::cout << "  mapping: " << point[0] << " " << point[1] << " " << point[2] << " -- " << point_cnt << std::endl;
               vertex_domain_mapping[point] = point_cnt;
               //mesh_domain_mapping[i-netgen::PointIndex::BASE] = point_cnt;
                //std::cout << "  mapping: " << i << " -- " << point_cnt << std::endl;
               mesh_domain_mapping[i] = point_cnt;
               //std::cout << "   mapping: " << i-netgen::PointIndex::BASE << " -- " << point[0] << " " << point[1] << " " << point[2] << std::endl;
               //geometry_cont[i-netgen::PointIndex::BASE] = point;
               //std::cout << point[0] << " " << point[1] << " " << point[2] << std::endl;
               geometry_cont.push_back(point);
               point_cnt++;
            }
            else                                       // point is already in the map - use this handle
            {
                //std::cout << "point alread added: " << point[0] << " " << point[1] << " " << point[2] << std::endl;
                //std::cout << "  mapping: " << i << " -- " << (*vdm_it).second << std::endl;
               mesh_domain_mapping[i] = (*vdm_it).second;    
            }            

         }
         
      #ifdef MESH_KERNEL_DEBUG
         std::cout << "## MeshKernel::"+mesh_kernel_id+" - extracting topology" << std::endl;
      #endif          
         for(std::size_t i = 1; i <= element_size; i++)
         {
            int ngcell[CELL_SIZE]; // netgen requires an int array ..:(
            nglib::Ng_GetVolumeElement (&(*meshpnt), i, ngcell);            
            cell_type cell = {{ mesh_domain_mapping[ngcell[0]], 
                                mesh_domain_mapping[ngcell[1]], 
                                mesh_domain_mapping[ngcell[2]], 
                                mesh_domain_mapping[ngcell[3]] }};
            segment_cont[mesh_cnt].push_back(cell);
         }
         mesh_cnt++;
       }            
   }
   // -------------------------------------------------------------------------------------     
   
   // -------------------------------------------------------------------------------------
   inline geometry_container_type &
   geometry() const  { return geometry_cont; }         
   inline geometry_container_type &
   geometry()        { return geometry_cont; }         
   // -------------------------------------------------------------------------------------
   inline segment_container_type &
   topology() const  { return segment_cont; }       
   inline segment_container_type &
   topology()        { return segment_cont; }       
   // -------------------------------------------------------------------------------------      
   
private:   
   // -------------------------------------------------------------------------------------     
   DatastructureT &              data;     
   geometry_container_type       geometry_cont;      
   segment_container_type        segment_cont;           
   mesh_container_type           mesh_container;
   nglib::Ng_Meshing_Parameters  mesh_parameters;   
   std::string                   mesh_kernel_id;   
   // -------------------------------------------------------------------------------------     
};

} // end namespace viennamesh

#endif 
