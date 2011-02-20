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

#ifndef VIENNAMESH_INTERFACE_TRIANGLE_HPP
#define VIENNAMESH_INTERFACE_TRIANGLE_HPP

/** @file triangle.hpp
    @brief mesh generator interface for: Triangle - http://www.cs.cmu.edu/~quake/triangle.html    
*/

// *** system includes
#include <vector>
#include <cstring>
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

// *** Triangle
#define VOID void
#define ANSI_DECLARATORS
#define REAL double
extern "C"  { 
   #include "triangle/triangle.h" 
}

// *** vienna includes
#include "viennautils/dumptype.hpp"
#include "viennamesh/interfaces/base.hpp"
#include "viennamesh/tags.hpp"


#define MESH_KERNEL_DEBUG
#define MESH_KERNEL_DEBUG_FULL

/*
TODO 
there are parameters for triangle I wasn't aware of:
  -i Uses the incremental algorithm for Delaunay triangulation, rather than the divide-and-conquer algorithm.
  -F Uses Steven Fortune's sweepline algorithm for Delaunay triangulation, rather than the divide-and-conquer algorithm.
  
  also provide minimal triangulation:
  -pz  ... minimal triangulation?  check again
    -p triangulation
    -z start indices from zero

   -pqz .. constrained delaunay  check again
   -pqzD .. conforming delaunay  check again

provide itnerface
*/


namespace viennamesh {

/** @brief tag-dispatched mesher kernel specialization for Triangle
*/   
template <typename DatastructureT>
struct mesh_kernel <viennamesh::tag::triangle, DatastructureT>
{

   // -------------------------------------------------------------------------------------
   typedef double    numeric_type;  
   typedef int       integer_type;

   static const int DIMG = 2;
   static const int DIMT = 2;
   static const int CELL_SIZE = DIMT+1;            // this holds only for simplices
   
   typedef boost::array< numeric_type , 2 >        point_type;
   typedef boost::array< numeric_type , 3 >        cell_type;
   typedef std::vector < cell_type >               topology_container_type;   
   
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
      mesh_kernel_id = "Triangle";      
      
      // TODO provide conecept check mechanism - is it a ViennaMesh::Wrapper ?
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - initiating" << std::endl;
   #endif
      this->init();
      
      size_t segment_size = data.domain().segment_container()->size();
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
            typename DatastructureT::cell_complex_wrapper_type segment = *seg_iter;
            this->find_point_in_segment(segment, pnt);
            region_points.push_back(pnt);
         #ifdef MESH_KERNEL_DEBUG
            std::cout << "## MeshKernel::"+mesh_kernel_id+" - computed point in segment " 
               << seg_cnt << " : " << pnt[0] << " " << pnt[1] << std::endl;
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
         
      #ifdef MESH_KERNEL_DEBUG
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
   }
   // -------------------------------------------------------------------------------------   
   
   // -------------------------------------------------------------------------------------
   /** @brief destructor takes care of releasing mesh kernel memory
   */   
   ~mesh_kernel()
   {
      // TODO provide conecept check mechanism - is it a ViennaMesh::Wrapper ?
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
      //char buffer[options.length()];
      char *buffer;
      buffer = (char *)malloc( options.length() * sizeof(char) );
      std::strcpy(buffer,options.c_str());

   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - meshing" << std::endl;
      std::cout << "  parameter set:    \"" << buffer << "\"" << std::endl;
      std::cout << "  input point size:  " << in.numberofpoints << std::endl;
      std::cout << "  input const size:  " << in.numberofsegments << std::endl;      
      std::cout << "  input region size: " << in.numberofregions << std::endl;            
   #endif         
      
      BOOST_ASSERT(in.numberofpoints != 0);
      //BOOST_ASSERT(in.numberofsegments != 0);      

      triangulate(buffer, &in, &out, 0);
      
      if ( !out.pointlist)      
         std::cout << "\t::ERROR: pointlist" << std::endl;
      if ( !out.trianglelist)   
         std::cout << "\t::ERROR: trianglelist " << std::endl;

   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - finished:" << std::endl;
      std::cout << "  output point size:  " << out.numberofpoints << std::endl;
      std::cout << "  output cell size:   " << out.numberoftriangles << std::endl;      
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - extracting geometry" << std::endl;
   #endif               
      
      //
      // extracting geometry data
      //
      geometry_cont.resize(out.numberofpoints);
      for(integer_type pnt_index = 0; pnt_index < out.numberofpoints; ++pnt_index)
      {
         integer_type index = pnt_index * 2;
         
         point_type pnt;
         pnt[0] = out.pointlist[index];
         pnt[1] = out.pointlist[index+1];
         geometry_cont[pnt_index] = pnt;
      } 

   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - extracting topology" << std::endl;
   #endif          
      
      //
      // extracting cell complex
      //
      size_t seg_id = 0;
      // segment index is only increment in the addRegion method
      // in the case of a single segment, this particular method is never 
      // called, therefore we have to set it manually to one
      if(segment_index == 0) segment_index = 1; 
      segment_cont.resize(segment_index);
      for(integer_type tri_index = 0; tri_index < out.numberoftriangles; ++tri_index)
      {
         integer_type index = tri_index * 3; 

         cell_type cell;
         cell[0] = out.trianglelist[index];
         cell[1] = out.trianglelist[index+1];
         cell[2] = out.trianglelist[index+2];         
         
         // only access triangle attributes if there are any
         // otherwise we get ourselves a segfault
         // if, for example, there is only one region, 
         // there is no need to call "add<region>", therefore
         // we have to counter this case
         if(out.numberoftriangleattributes > 0)
            seg_id = out.triangleattributelist[tri_index];
         
         //std::cout << "tri: " << tri_index << " : " << cell[0] << " " << cell[1] << " " 
         //   << cell[2] << " attr: " << out.triangleattributelist[tri_index] << std::endl;
         segment_cont[seg_id].push_back(cell);
      }
      //
      // release internal mesher output container, as the data has been already 
      // retrieved
      //
      freeMem(out);         
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
         //std::cout << "region: " << cell << std::endl;
         for(int dim = 0; dim < DIMG; dim++)
         {
            if(!pnt_uniquer[cell[dim]])
            {  
               this->addPoint(this->data.domain().vertex(cell[dim]).getPoint());  
               pnt_uniquer[cell[dim]] = true;
               index_map[cell[dim]] = point_cnt;
               point_cnt++;
            }
         }
         boost::array< numeric_type , 2 > mapped_cell;
         mapped_cell[0] = index_map[cell[0]];
         mapped_cell[1] = index_map[cell[1]];
         this->addConstraint(mapped_cell);
      }
      
      // minimal meshing
      (*this)();     

      //std::cout << this->segment_cont.size() << " " << this->segment_cont[0].size() << std::endl;
      
      // check if there is at least one segment and at least one cell on this segment
      if(this->segment_cont.size() > 0 && this->segment_cont[0].size() > 0)
      {
         this->barycenter(
            geometry_cont[segment_cont[0][0][0]],  // first segment, first cell, first vertex
            geometry_cont[segment_cont[0][0][1]],  // first segment, first cell, second vertex
            geometry_cont[segment_cont[0][0][2]],  // first segment, first cell, third vertex
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
   void barycenter(PointT const& p1, PointT const& p2, PointT const& p3, PointT & result)
   {
      result[0] = (p1[0] + p2[0] + p3[0])/3.;
      result[1] = (p1[1] + p2[1] + p3[1])/3.;
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
   }
   template<typename PointT>
   void addPoint(PointT & pnt)
   {  //std::cout << "adding point: " << pnt[0] << " " << pnt[1] << std::endl;
      this->extendPoints();
      this->in.pointlist[pointlist_index] = pnt[0];
      this->pointlist_index++;
      this->in.pointlist[pointlist_index] = pnt[1];
      this->pointlist_index++;             
   }
   // -------------------------------------------------------------------------------------   

   // -------------------------------------------------------------------------------------
   template<typename ConstraintT>
   void addConstraint(ConstraintT const& constraint)
   {
      extendSegments();
      in.segmentlist[segmentlist_index] = constraint[0];
      segmentlist_index++;
      in.segmentlist[segmentlist_index] = constraint[1];
      segmentlist_index++;          
   }
   template<typename ConstraintT>
   void addConstraint(ConstraintT & constraint)
   {  //std::cout << "adding const: " << constraint[0] << " " << constraint[1] << std::endl;
      extendSegments();
      in.segmentlist[segmentlist_index] = constraint[0];
      segmentlist_index++;
      in.segmentlist[segmentlist_index] = constraint[1];
      segmentlist_index++;          
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
         in.pointlist   = (numeric_type *)malloc( 2 * sizeof(numeric_type) );
      else   in.pointlist = (numeric_type *)realloc ( in.pointlist, (in.numberofpoints+1) * 2 * sizeof(numeric_type));
      in.numberofpoints++;
   }
   // ------------------------------------------------------------------------------------- 

   // -------------------------------------------------------------------------------------
   void extendSegments()
   {
      if ( !in.segmentlist)
         in.segmentlist   = (integer_type *)malloc( 2 * sizeof(integer_type) );
      else   in.segmentlist = (integer_type *)realloc ( in.segmentlist, (in.numberofsegments+1) * 2 * sizeof(integer_type));
      in.numberofsegments++;
   }
   // ------------------------------------------------------------------------------------- 

   // -------------------------------------------------------------------------------------
   void extendRegions()
   {
      if ( !in.regionlist)
         in.regionlist   = (numeric_type *)malloc( 4 * sizeof(numeric_type) );
      else   in.regionlist = (numeric_type *)realloc ( in.regionlist, (in.numberofregions+1) * 4 * sizeof(numeric_type));
      in.numberofregions++;
   }
   // -------------------------------------------------------------------------------------   
   
   // -------------------------------------------------------------------------------------
   void init_kernel(triangulateio& io)
   {
      io.pointlist = io.pointattributelist = 0;
      io.pointmarkerlist   = 0;
      io.numberofpoints = io.numberofpointattributes = 0;
      io.trianglelist = 0;
      io.triangleattributelist = io.trianglearealist = 0;
      io.neighborlist = 0;
      io.numberoftriangles = io.numberofcorners = 0;
      io.numberoftriangleattributes = 0;
      io.segmentlist = io.segmentmarkerlist = 0;
      io.numberofsegments = 0;
      io.holelist = 0; 
      io.numberofholes = 0;
      io.regionlist = 0;
      io.numberofregions = 0;
      io.edgelist = io.edgemarkerlist = 0;
      io.normlist = 0;
      io.numberofedges = 0;
   }   
   // -------------------------------------------------------------------------------------
   
   // -------------------------------------------------------------------------------------    
   void init()
   {
      init_kernel(in);
      init_kernel(out);
      options = "";
      pointlist_index      = 0;
      segmentlist_index    = 0;
      regionlist_index     = 0;
      holelist_index       = 0;
      segment_index        = 0;             
   }
   // -------------------------------------------------------------------------------------          
   
   // -------------------------------------------------------------------------------------    
   void clear()
   {
      freeMem(in);
      freeMem(out); 
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
   void freeMem(triangulateio& io)
   {
      if (io.pointlist != NULL)
      {  
         free (io.pointlist);
         io.pointlist = NULL;
      }
      if (io.pointattributelist != NULL)
      {
         free (io.pointattributelist);
         io.pointattributelist = NULL;
      }
      if (io.pointmarkerlist != NULL)
      {
         free (io.pointmarkerlist);
         io.pointmarkerlist = NULL;
      }
      if (io.trianglelist != NULL)
      {
         free (io.trianglelist);
         io.trianglelist = NULL;
      }
      if (io.triangleattributelist != NULL)   
      {
         free (io.triangleattributelist);
         io.triangleattributelist = NULL;
      }
      if (io.neighborlist != NULL)
      {
         free (io.neighborlist);
         io.neighborlist = NULL;
      }
      if (io.segmentlist != NULL)
      {
         free (io.segmentlist);
         io.segmentlist = NULL;
      }
      if (io.segmentmarkerlist != NULL)
      {
         free (io.segmentmarkerlist);
         io.segmentmarkerlist = NULL;
      }
      if (io.edgelist != NULL)               
      {
         free (io.edgelist);
         io.edgelist = NULL;
      }
      if (io.edgemarkerlist != NULL)        
      {
         free (io.edgemarkerlist);
         io.edgemarkerlist = NULL;
      }
      if (io.normlist != NULL)               
      {
         free (io.normlist);
         io.normlist = NULL;
      }
   }
   // -------------------------------------------------------------------------------------
   DatastructureT & data;
   
   triangulateio  in, 
                  out;

   std::string    options;

   integer_type   pointlist_index,
                  segmentlist_index,
                  regionlist_index,
                  holelist_index,
                  segment_index;   

   geometry_container_type      geometry_cont;      
   segment_container_type       segment_cont;   
   
   std::string mesh_kernel_id;
   // -------------------------------------------------------------------------------------
};   
   
} // end namespace viennamesh

#endif