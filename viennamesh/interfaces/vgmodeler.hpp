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

#ifndef VIENNAMESH_INTERFACE_VGMODELER_HPP
#define VIENNAMESH_INTERFACE_VGMODELER_HPP

// *** system includes
#include <vector>

// *** boost includes
#include <boost/array.hpp>

// *** vienna includes
#include "viennautils/dumptype.hpp"
#include "viennamesh/interfaces/base.hpp"
#include "viennamesh/tags.hpp"

namespace viennamesh {

template <typename DatastructureT>
struct mesh_kernel <viennamesh::tag::vgmodeler, DatastructureT>
{
private:
   // -------------------------------------------------------------------------------------
   typedef double    numeric_type;  
   typedef int       integer_type;

   static const int DIMG = 3;
   static const int DIMT = 3;
   static const int CELL_SIZE = DIMT+1;            // this holds only for simplices
   
   typedef boost::array< numeric_type , DIMG >        point_type;
   typedef boost::array< numeric_type , CELL_SIZE >   cell_type;
   typedef std::vector < cell_type >                  topology_container_type;   
   // -------------------------------------------------------------------------------------   

public:
   // -------------------------------------------------------------------------------------      
   typedef std::vector < point_type >              geometry_container_type;
   typedef std::vector <topology_container_type>   segment_container_type;         
   // -------------------------------------------------------------------------------------      
   
   // -------------------------------------------------------------------------------------      
   mesh_kernel(DatastructureT & data) : data(data) 
   {
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::VGModeler - initiating" << std::endl;
   #endif      
   }
   // -------------------------------------------------------------------------------------      
   
   // -------------------------------------------------------------------------------------      
   ~mesh_kernel()
   {
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::VGModeler - shutting down" << std::endl;
   #endif
      this->clear();
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
   
   // -------------------------------------------------------------------------------------   
   DatastructureT & data;   
   
   geometry_container_type      geometry_cont;      
   segment_container_type       segment_cont;             
   // -------------------------------------------------------------------------------------      
};

} // end namespace viennamesh

#endif