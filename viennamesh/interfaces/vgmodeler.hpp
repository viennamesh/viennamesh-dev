/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_INTERFACE_VGMODELER_HPP
#define VIENNAMESH_INTERFACE_VGMODELER_HPP

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

#include "vgmodeler/volume.hpp"

namespace viennamesh {

template <typename DatastructureT>
struct mesh_kernel <viennamesh::tag::vgmodeler, DatastructureT>
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
   
   typedef boost::shared_ptr<netgen::Mesh>            mesh_pointer_type;
   typedef std::vector<mesh_pointer_type>             mesh_container_type;
   typedef typename mesh_container_type::iterator     mesh_iterator_type;         
   typedef typename std::map<point_type, std::size_t> vertex_map_type;
   typedef typename vertex_map_type::iterator         vertex_map_iterator_type;
   // -------------------------------------------------------------------------------------   

   // -------------------------------------------------------------------------------------      
   typedef std::vector < point_type >              geometry_container_type;
   typedef std::vector <topology_container_type>   segment_container_type;         
   typedef viennamesh::tag::mesh_kernel            datastructure_type; // type is used for viennamesh::traits
   // -------------------------------------------------------------------------------------      
   
   // -------------------------------------------------------------------------------------      
   mesh_kernel(DatastructureT & data) : data(data) 
   {
      mesh_kernel_id = "VGModeler";
      
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - initiating" << std::endl;
   #endif      
      
      size_t segment_size = data.segment_size();
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
         mesh_pointer_type meshpnt = mesh_pointer_type(new netgen::Mesh());
         
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
            for(size_t i = 0; i < vmesh_cell.size(); i++)  
            {
               std::size_t vertex_index = vmesh_cell[i];
               if(!vertex_uniquer[vertex_index])
               {
                  // retrieve the geometry information
                  vmesh_geometry_iterator geoit = data.geometry_begin();
                  std::advance(geoit, vertex_index);
                  vmesh_point_type point = *geoit;
                  netgen::Point3d netgen_point;
                  
                  // set very small values to zero to increase
                  // numerical stability
                  if(std::abs(point[0]) <= value_min)
                     netgen_point.X() = 0;
                  else netgen_point.X() = point[0];
                  
                  if(std::abs(point[1]) <= value_min)
                     netgen_point.Y() = 0;
                  else netgen_point.Y() = point[1];
                  
                  if(std::abs(point[2]) <= value_min)
                     netgen_point.Z() = 0;
                  else netgen_point.Z() = point[2];
                  
                  //std::cout << "adding point: " << netgen_point << std::endl;
                  
                  // add the point to the mesh datastructure
                  meshpnt->AddPoint(netgen_point);
                     
                  // build a new cell with the correct vertex indices
                  cell[localcounter] = meshpnt->GetNP();
                  
                  // map the input vertex index with index within the mesh datastructure
                  segment_vertices[vertex_index] = meshpnt->GetNP();

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
            netgen::Element2d el;
            el.SetIndex(1);  // TODO find out what this index is .. but it is crucial that it is set to 1!
            el.PNum(1) = cell[0];
            el.PNum(2) = cell[1];
            el.PNum(3) = cell[2];   
            
            // each face shall hold segment_id+1 id in the facedescriptor
            // this number is correlated with the "mesh_cnt" variable in the functor!
            // if this index is not +1 the segment id, we shall get a segfault ..
            meshpnt->AddFaceDescriptor(netgen::FaceDescriptor(segment_cnt+1,1,0,0));

            //std::cout << "adding constraint: " << el << std::endl;
            
            // add netgen cell to mesh structure            
            meshpnt->AddSurfaceElement(el);         
         }
         
         segment_cnt++;
         
         mesh_container.push_back(meshpnt);         
      }         
   }
   // -------------------------------------------------------------------------------------      
   
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
      this->setOptions(paras);      
      
      //mesh_parameters
      //mesh_parameters.maxh=0.01;
      //mesh_parameters.secondorder = 2;
      //mesh_parameters.badellimit = 150;   // 150-180
      
      
      #ifdef MESH_KERNEL_DEBUG
         std::cout << "## MeshKernel::"+mesh_kernel_id+" - meshing:" << std::endl;
         std::cout << "  input region size: " << mesh_container.size() << std::endl;            
         std::cout << "  parameters: " << std::endl;
         std::cout << "  ----------------------------------------" << std::endl;
         mesh_parameters.Print(std::cout, "  ");
         std::cout << "  ----------------------------------------" << std::endl;         
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
         std::cout << "  input point size:  " << meshpnt->GetNP() << std::endl;
         std::cout << "  input const size:  " << meshpnt->GetNSE() << std::endl;      
      #endif        

//      ?? not needed ..         
//          netgen::Point3d pmin, pmax;
//          meshpnt->GetBox(pmin, pmax);

         // compute the local feature size for this segment
         meshpnt->CalcLocalH(threadID);  
         
         netgen::MeshVolume (mesh_parameters, *meshpnt, threadID);
         
      #ifdef MESH_KERNEL_DEBUG         
         // write a volume file for each segment
         // can be viewed with, for example, netgen
         //
         std::string filename = "segment_" + boost::lexical_cast<std::string>(mesh_cnt) + ".vol";
         meshpnt->Save(filename);         
      #endif
         
         long point_size   = meshpnt->GetNP();
         long element_size = meshpnt->GetNE();
         
      #ifdef MESH_KERNEL_DEBUG
         std::cout << "## MeshKernel::"+mesh_kernel_id+" - finished:" << std::endl;
         std::cout << "  output point size:  " << point_size << std::endl;
         std::cout << "  output cell size:   " << element_size << std::endl;      
         std::cout << "## MeshKernel::"+mesh_kernel_id+" - extracting geometry" << std::endl;
         std::cout << "## MeshKernel::"+mesh_kernel_id+" starting mesh generation " << std::endl;         
      #endif            
         
         std::map<std::size_t, std::size_t>  mesh_domain_mapping; 
  
        
         for(netgen::PointIndex i = netgen::PointIndex::BASE; i < (point_size + netgen::PointIndex::BASE); i++)
         {
            //std::cout << "direct point output: " << (*meshpnt)[i] << std::endl;
            
            point_type point = {{(*meshpnt)[i].X(), (*meshpnt)[i].Y(), (*meshpnt)[i].Z() }};
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
         for(netgen::ElementIndex i = 0; i < element_size; i++)
         {
             //std::cout << "mapping cell" << std::endl;
             //std::cout << "direct cell output: " << (*meshpnt)[i] << std::endl;
             //std::cout << mesh_domain_mapping[(*meshpnt)[i][0]] << " " << mesh_domain_mapping[(*meshpnt)[i][1]] << " " << 
             //   mesh_domain_mapping[(*meshpnt)[i][2]] << " " << mesh_domain_mapping[(*meshpnt)[i][3]] << std::endl;

            
            cell_type cell = {{ mesh_domain_mapping[(*meshpnt)[i][0]], 
                                mesh_domain_mapping[(*meshpnt)[i][1]], 
                                mesh_domain_mapping[(*meshpnt)[i][2]], 
                                mesh_domain_mapping[(*meshpnt)[i][3]] }};
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
   template<typename ParametersMapT>
   void setOptions(ParametersMapT & paras,
      typename boost::enable_if< typename boost::fusion::result_of::has_key<ParametersMapT, viennamesh::tag::criteria>::type >::type* dummy = 0) 
   {
   }
   // -------------------------------------------------------------------------------------      
   
   // -------------------------------------------------------------------------------------   
   DatastructureT & data;   
   
   geometry_container_type      geometry_cont;      
   segment_container_type       segment_cont;            
   
   mesh_container_type          mesh_container;
   netgen::MeshingParameters    mesh_parameters;

   std::string mesh_kernel_id;
   // -------------------------------------------------------------------------------------      
};

} // end namespace viennamesh

#endif
