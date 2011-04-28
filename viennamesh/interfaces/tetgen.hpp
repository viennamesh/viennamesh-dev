/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_INTERFACE_TETGEN_HPP
#define VIENNAMESH_INTERFACE_TETGEN_HPP

/** @file tetgen.hpp
    @brief mesh generator interface for: Tetgen - http://tetgen.berlios.de/
*/

// *** system includes
#include <vector>
#include <map>

// *** boost includes
#include <boost/array.hpp>
#include <boost/assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/static_assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/make_map.hpp>

// *** tetgen includes
#define REAL double
#define TETLIBRARY
#include "tetgen/tetgen.h"

// *** vienna includes
#include "viennautils/dumptype.hpp"
#include "viennautils/contio.hpp"
#include "viennamesh/interfaces/base.hpp"
#include "viennamesh/tags.hpp"

//#define MESH_KERNEL_DEBUG
//#define MESH_KERNEL_DEBUG_FULL

namespace viennamesh {

/** @brief tag-dispatched mesher kernel specialization for Tetgen
*/   
template <typename DatastructureT>
struct mesh_kernel <viennamesh::tag::tetgen, DatastructureT>
{
   // -------------------------------------------------------------------------------------
   typedef REAL      numeric_type;  
   typedef int       integer_type;

   static const int DIMG = 3;
   static const int DIMT = 3;
   static const int CELL_SIZE = DIMT+1;            // this holds only for simplices
   
   typedef boost::array< numeric_type , DIMG >        point_type;
   typedef boost::array< numeric_type , CELL_SIZE >   cell_type;
   typedef std::vector < cell_type >                  topology_container_type;   
   
   typedef typename DatastructureT::segment_iterator  vmesh_segment_iterator;
   typedef typename DatastructureT::cell_type         vmesh_cell_type;
   typedef typename DatastructureT::cell_iterator     vmesh_cell_iterator;   
   // -------------------------------------------------------------------------------------   

   typedef std::vector < point_type >              geometry_container_type;
   typedef std::vector <topology_container_type>   segment_container_type;      
   typedef viennamesh::tag::mesh_kernel            datastructure_type; // type is used for viennamesh::traits   
   // -------------------------------------------------------------------------------------   
   /** @brief constructor expects a ViennaMesh::wrapped datastructure
   */      
   mesh_kernel(DatastructureT & data) : data(data) 
   {
      mesh_kernel_id = "Tetgen";      
      
      // TODO provide conecept check mechanism - is it a ViennaMesh::Wrapper ?
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - initiating" << std::endl;
   #endif
      this->init();
      
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
         std::size_t seg_cnt = 0;
      #endif      
         for(vmesh_segment_iterator seg_iter = data.segment_begin();
            seg_iter != data.segment_end(); seg_iter++)
         {
            point_type pnt;
            typename DatastructureT::cell_complex_wrapper_type segment = *seg_iter;
            this->find_point_in_segment(segment, pnt);
            
            region_points.push_back(pnt);
         #ifdef MESH_KERNEL_DEBUG
            std::cout << "## MeshKernel::"+mesh_kernel_id+" - computed point in segment " 
               << seg_cnt << " : " << pnt[0] << " " << pnt[1] << " " << pnt[2] << std::endl;
            seg_cnt++;
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
      std::cout << std::endl;
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
      std::cout << std::endl;   
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing constraintss" << std::endl;
   #endif
      size_t si = 0;

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

//            for(size_t i = 0; i < vmesh_cell.size(); i++)  
//               cell[i] = vmesh_cell[i];
//            std::sort(cell.begin(), cell.end());
//            //std::cout << cell[0] << " " << cell[1] << " " << cell[2] << std::endl;
//            if(!cell_uniquer[cell])
//            {
//            #ifdef MESH_KERNEL_DEBUG_FULL
//               std::cout << "## MeshKernel::"+mesh_kernel_id+" - adding constraint   " << 
//                  cell_cnt << " : ";
//               for(size_t i = 0; i < cell.size(); i++)  
//                  std::cout << cell[i] << " ";
//               std::cout << std::endl;
//               cell_cnt++;
//            #endif               
//               cell_uniquer[cell] = true;
//               
//               this->addConstraint(vmesh_cell);
//            }
//         #ifdef MESH_KERNEL_DEBUG_FULL
//            else
//            { 
//               std::cout << "## MeshKernel::"+mesh_kernel_id+" - skipping constraint " << 
//                  cell_cnt << " : ";
//               for(size_t i = 0; i < cell.size(); i++)  
//                  std::cout << cell[i] << " ";
//               std::cout << std::endl;
//            }
//         #endif
         }
         si++;
      }            
   }
   // -------------------------------------------------------------------------------------
   
   // -------------------------------------------------------------------------------------   
   /** @brief destructor takes care of releasing mesh kernel memory
   */      
   ~mesh_kernel()
   {
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - shutting down" << std::endl;
   #endif
      this->clear();
   }   
   // -------------------------------------------------------------------------------------
   
   // -------------------------------------------------------------------------------------   
   /** @brief functor expects a parameter set based on a boost::fusion::map
   */   
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
      // if there are more than one regions, spread the regional attribute
      if(in.numberofregions > 1) options.append("A");  
      
   #ifndef MESH_KERNEL_DEBUG_FULL
      options.append("Q");
   #endif
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - meshing:" << std::endl;
      std::cout << "  parameter set:     " << options << std::endl;
      std::cout << "  input point size:  " << in.numberofpoints << std::endl;
      std::cout << "  input const size:  " << in.numberoffacets << std::endl;      
      std::cout << "  input region size: " << in.numberofregions << std::endl;            
      std::cout << "## MeshKernel::"+mesh_kernel_id+" starting mesh generation " << std::endl;               
   #endif         

      char *buffer;
      buffer = (char *)malloc( options.length() * sizeof(char) );
      std::strcpy(buffer,options.c_str());      

      BOOST_ASSERT(in.numberofpoints != 0);
      //BOOST_ASSERT(in.numberoffacets != 0);      
      //
      // start meshing process
      //
      tetrahedralize(buffer, &in, &out);
      
      if ( !out.pointlist)      
         std::cout << "\t::ERROR: pointlist" << std::endl;
      if ( !out.tetrahedronlist)   
         std::cout << "\t::ERROR: tetrahedronlist " << std::endl;      
      
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - finished:" << std::endl;
      std::cout << "  output point size:  " << out.numberofpoints << std::endl;
      std::cout << "  output cell size:   " << out.numberoftetrahedra << std::endl;      
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - extracting geometry" << std::endl;
   #endif               
      
      //
      // extracting geometry data
      //
      geometry_cont.resize(out.numberofpoints);
      for(integer_type pnt_index = 0; pnt_index < out.numberofpoints; ++pnt_index)
      {
         integer_type index = pnt_index * 3;
         
         point_type pnt;
         pnt[0] = out.pointlist[index];
         pnt[1] = out.pointlist[index+1];
         pnt[2] = out.pointlist[index+2];         
         geometry_cont[pnt_index] = pnt;
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
      segment_cont.resize(segment_index);
      for(integer_type tet_index = 0; tet_index < out.numberoftetrahedra; ++tet_index)
      {
         integer_type index = tet_index * 4; 

         cell_type cell;
         cell[0] = out.tetrahedronlist[index];
         cell[1] = out.tetrahedronlist[index+1];
         cell[2] = out.tetrahedronlist[index+2];         
         cell[3] = out.tetrahedronlist[index+3];                  
         
         // only access triangle attributes if there are any
         // otherwise we get ourselves a segfault
         // if, for example, there is only one region, 
         // there is no need to call "add<region>", therefore
         // we have to counter this case
         if(out.numberoftetrahedronattributes > 0)
         {
            seg_id = out.tetrahedronattributelist[tet_index];
         }
         
         //std::cout << "tri: " << tri_index << " : " << cell[0] << " " << cell[1] << " " 
         //   << cell[2] << " attr: " << out.triangleattributelist[tri_index] << std::endl;
         //std::cout << "transferring simplex to segment: " << seg_id << std::endl;
      #ifdef MESH_KERNEL_DEBUG
         seg_check[seg_id] = true;
      #endif
         segment_cont[seg_id].push_back(cell);
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
      setOptions_impl(boost::fusion::at_key<viennamesh::tag::criteria>(paras));
   }
   void setOptions_impl(viennamesh::tag::convex const&)               { options = "z";    }   
   void setOptions_impl(viennamesh::tag::constrained_delaunay const&) { options = "zp";  }
   void setOptions_impl(viennamesh::tag::conforming_delaunay const&)  { options = "zpD"; }
   // -------------------------------------------------------------------------------------   
   
   // -------------------------------------------------------------------------------------   
   void find_point_in_segment(typename DatastructureT::cell_complex_wrapper_type & cell_complex, 
                              point_type& pnt)
   {
      std::map<std::size_t, bool>  pnt_uniquer;
      std::map<std::size_t, std::size_t> index_map;
      std::size_t point_cnt = 0;
      typedef typename DatastructureT::geometry_iterator geometry_iterator;
      for(vmesh_cell_iterator cit = cell_complex.cell_begin();
            cit != cell_complex.cell_end(); cit++)
      {
         vmesh_cell_type cell = *cit;
         for(int dim = 0; dim < cell.size(); dim++)
         {
            if(!pnt_uniquer[cell[dim]])
            {  
               geometry_iterator geo = this->data.geometry_begin();
               std::advance(geo, cell[dim]);
               this->addPoint( *geo ); 
               pnt_uniquer[cell[dim]] = true;
               index_map[cell[dim]] = point_cnt;
               point_cnt++;
            }
         }
         std::vector<numeric_type> mapped_cell(cell.size());
         for(int i = 0; i < cell.size(); i++)
         {
           mapped_cell[i] = index_map[cell[i]];
         }
         
         this->addConstraint(mapped_cell);
      }
      
      // minimal meshing
      (*this)();     

      //std::cout << this->segment_cont.size() << " " << this->segment_cont[0].size() << std::endl;
      
      // check if there is at least one segment and at least one cell on this segment
      if(this->segment_cont.size() > 0 && this->segment_cont[0].size() > 0)
      {
         std::size_t cellid = 0;
         this->barycenter(
            geometry_cont[segment_cont[0][cellid][0]],  // first segment, first cell, first vertex
            geometry_cont[segment_cont[0][cellid][1]],  // first segment, first cell, second vertex
            geometry_cont[segment_cont[0][cellid][2]],  // first segment, first cell, third vertex
            geometry_cont[segment_cont[0][cellid][3]],  // first segment, first cell, third vertex                          
            pnt
         );
      }
      else 
      {
         std::cout << "## MeshKernel::"+mesh_kernel_id+" [ERROR] - point in segment algorithm failed as pre-meshing failed!" << std::endl;
      }
      this->reset();
   }
   // -------------------------------------------------------------------------------------         
   
   // -------------------------------------------------------------------------------------
   template<typename PointT>
   void barycenter(PointT const& p1, PointT const& p2, PointT const& p3, PointT const& p4, PointT & result)
   {
      result[0] = (p1[0] + p2[0] + p3[0] + p4[0])/4.;
      result[1] = (p1[1] + p2[1] + p3[1] + p4[1])/4.;
      result[2] = (p1[2] + p2[2] + p3[2] + p4[2])/4.;      
   }
   // -------------------------------------------------------------------------------------      
   
   // -------------------------------------------------------------------------------------
   template<typename PointT>
   void addPoint(PointT const& pnt)
   {  //std::cout << "adding point: " << pnt[0] << " " << pnt[1] << std::endl;
      this->extendPoints();
      this->in.pointlist[pointlist_index] = pnt[0];
      this->pointlist_index++;
      this->in.pointlist[pointlist_index] = pnt[1];
      this->pointlist_index++;             
      this->in.pointlist[pointlist_index] = pnt[2];
      this->pointlist_index++;                   
   }
   template<typename PointT>
   void addPoint(PointT & pnt)
   {  //std::cout << "adding point: " << pnt[0] << " " << pnt[1] << std::endl;
      this->extendPoints();
      this->in.pointlist[pointlist_index] = pnt[0];
      this->pointlist_index++;
      this->in.pointlist[pointlist_index] = pnt[1];
      this->pointlist_index++;             
      this->in.pointlist[pointlist_index] = pnt[2];
      this->pointlist_index++;           
   }
   // -------------------------------------------------------------------------------------   

   // -------------------------------------------------------------------------------------
   template<typename ConstraintT>
   void addConstraint(ConstraintT const& constraint)
   {
      extendFacets();

      f = &in.facetlist[mesher_facet_index];
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
   void addConstraint(ConstraintT & constraint)
   {  //std::cout << "adding const: " << constraint[0] << " " << constraint[1] << std::endl;
      extendFacets();

      f = &in.facetlist[mesher_facet_index];
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
   // -------------------------------------------------------------------------------------   

   // -------------------------------------------------------------------------------------
   template<typename PointT>
   void addRegion(PointT const& pnt)
   {
      extendRegions();
      in.regionlist[regionlist_index] = pnt[0];
      regionlist_index++;
      in.regionlist[regionlist_index] = pnt[1];
      regionlist_index++;
      in.regionlist[regionlist_index] = pnt[2];
      regionlist_index++;      
      in.regionlist[regionlist_index] = numeric_type(segment_index);  // assign region id
      regionlist_index++;
      in.regionlist[regionlist_index] = numeric_type(segment_index);  // assign region id
      regionlist_index++;
      segment_index++;  
   }
   template<typename PointT>
   void addRegion(PointT & pnt)
   {  
      extendRegions();
      in.regionlist[regionlist_index] = pnt[0];
      regionlist_index++;
      in.regionlist[regionlist_index] = pnt[1];
      regionlist_index++;
      in.regionlist[regionlist_index] = pnt[2];
      regionlist_index++;
      in.regionlist[regionlist_index] = numeric_type(segment_index);  // assign region id
      regionlist_index++;
      in.regionlist[regionlist_index] = numeric_type(segment_index);  // assign region id
      regionlist_index++;
      segment_index++;
   }   
   // -------------------------------------------------------------------------------------      
   
   // -------------------------------------------------------------------------------------
   void extendPoints()
   {
      if ( !in.pointlist)
         in.pointlist   = (numeric_type *)malloc( 3 * sizeof(numeric_type) );
      else   in.pointlist = (numeric_type *)realloc ( in.pointlist, (in.numberofpoints+1) * 3 * sizeof(numeric_type));
      in.numberofpoints++;
   }
   // -------------------------------------------------------------------------------------    
   
   // -------------------------------------------------------------------------------------
   void extendFacets()
   {
      if ( !in.facetlist)
         in.facetlist   = (tetgenio::facet *)malloc( 2 * sizeof(tetgenio::facet) );
      else   in.facetlist = (tetgenio::facet *)realloc ( in.facetlist, (in.numberoffacets+1) * 2 * sizeof(tetgenio::facet));
      in.numberoffacets++;
   }
   // ------------------------------------------------------------------------------------- 

   // -------------------------------------------------------------------------------------
   void extendRegions()
   {
      if ( !in.regionlist)
         in.regionlist   = (numeric_type *)malloc( 5 * sizeof(numeric_type) );
      else   in.regionlist = (numeric_type *)realloc ( in.regionlist, (in.numberofregions+1) * 5 * sizeof(numeric_type));
      in.numberofregions++;
   }
   // -------------------------------------------------------------------------------------      
   
   // -------------------------------------------------------------------------------------  
   void init_kernel(tetgenio& io)
   {
      io.numberofpoints = 0;
      io.numberoffacets = 0;
      io.numberofregions = 0;      
   }
   // -------------------------------------------------------------------------------------     
   
   // -------------------------------------------------------------------------------------    
   void init()
   {
      init_kernel(in);
      init_kernel(out);
      options = "";
      pointlist_index               = 0;
      regionlist_index              = 0;
      mesher_facet_index            = 0;     
      constraint_list_index         = 0;
      segment_index        = 0;           
   }
   // -------------------------------------------------------------------------------------          
   
   // -------------------------------------------------------------------------------------    
   void clear()
   {
      freeMem();
      geometry_cont.clear();
      segment_cont.clear();    
   }
   // -------------------------------------------------------------------------------------           

   // -------------------------------------------------------------------------------------    
   void reset()
   {
      this->clear();
      this->init();
   }
   // -------------------------------------------------------------------------------------        
   
   // -------------------------------------------------------------------------------------           
   void freeMem()
   {
      for(integer_type facet_index = 0; facet_index <  in.numberoffacets; ++facet_index)
      {
         f = &in.facetlist[facet_index];    
         p = &f->polygonlist[0];

         delete[] p->vertexlist;
         delete[] f->polygonlist;  
      }      
      if(in.pointlist)           free(in.pointlist);
      if(in.facetlist)           free(in.facetlist);   
      
      in.pointlist = NULL;
      in.facetlist = NULL;
      
      delete[] out.pointmarkerlist;            
   }
   // -------------------------------------------------------------------------------------           
   
   // -------------------------------------------------------------------------------------   
   DatastructureT & data;   
   
   std::string    options;

   tetgenio          in, 
                     out;
   tetgenio::facet   *f;
   tetgenio::polygon *p;
   
   integer_type   pointlist_index,
                  constraint_list_index,
                  regionlist_index,
                  mesher_facet_index,
                  segment_index;   
                  
   geometry_container_type      geometry_cont;      
   segment_container_type       segment_cont;                     

   std::string mesh_kernel_id;
};

} // end namespace viennamesh

#endif

