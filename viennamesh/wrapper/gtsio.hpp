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

#ifndef VIENNAMESH_WRAPPER_VIENNAGRID_HPP
#define VIENNAMESH_WRAPPER_VIENNAGRID_HPP

#include "viennamesh/wrapper/wrapper_base.hpp"
#include "viennamesh/tags.hpp"
#include "viennautils/value_type.hpp"

#include "include/gtsio3.h"

namespace viennamesh {

namespace tag {
struct gtsio3 {};
} // end namespace tag   

//*****************************************************************
// gts_list_getter
// ---------------
// Implements a generic approach for 1D, 2D and 3D handling of
// GTSObjects (GTSLine, GTSFace, GTSPolyhedron) and
// GTSObjectLists (GTSLineList, GTSFaceList, GTSPolyhedronList).
//
//******************************************************************
template <long i>
struct gts_list_getter;

//one dimension
template <>
struct gts_list_getter<1>
{ 
typedef GTSIO::GTSLineList       list_type;
typedef GTSIO::GTSLine           object_type;

static list_type* newList(GTSIO::GTSDevice * pDevice)
{
   list_type* pLineList = pDevice->NewGTSLineList("Lines");
   pLineList->setName("Lines");
         
   return pLineList;
}

static list_type* getList(GTSIO::GTSDevice* pDevice)
{
   list_type* pLineList = pDevice->getGTSLineList("Lines");
   //list_type* pFaceList = pDevice->getGTSLineList(0);
   
   std::cout << "#LineLists: " << pDevice->getGTSLineListSize() << std::endl;

   return pLineList;
}
};

// two dimensions
template <>
struct gts_list_getter<2>
{
typedef GTSIO::GTSFaceList       list_type;
typedef GTSIO::GTSFace           object_type;

static list_type* newList(GTSIO::GTSDevice * pDevice)
{
   list_type* pFaceList = pDevice->NewGTSFaceList("Faces");
   pFaceList->setName("Faces");
   return pFaceList;
}    

static list_type* getList(GTSIO::GTSDevice* pDevice)
{
   list_type* pFaceList = pDevice->getGTSFaceList("Faces");
   return pFaceList;
}
};

// three dimensions
template <>
struct gts_list_getter<3>
{
typedef GTSIO::GTSPolyhedronList       list_type;
typedef GTSIO::GTSPolyhedron           object_type;

static list_type* newList(GTSIO::GTSDevice * pDevice)
{
   list_type* pPolyhedronList = pDevice->NewGTSPolyhedronList("Polyhedrons");
   pPolyhedronList->setName("Polyhedrons");
   return pPolyhedronList;
}
      
static list_type* getList(GTSIO::GTSDevice* pDevice)
{
   list_type* pPolyhedronList = pDevice->getGTSPolyhedronList("Polyhedrons");
   
   std::cout << "Address pFaceList: " << pPolyhedronList << std::endl;
   std::cout << "PolyhedronListSize: " << pDevice->getGTSPolyhedronListSize() << std::endl;
   
   return pPolyhedronList;
}
};   

//**************************************************************
// gts_point_getter
// ----------------
// Extracts the related points of a n-th dimensional GTSObject.
// 1D GTSLine
// 2D GTSFace
// 3D GTSPolyhedron
//**************************************************************
template<long dim, typename obj>
struct gts_point_getter;

// one dimension
template<typename obj>
struct gts_point_getter<1, obj> {
static long getPointIdx(obj* pO, long idx) {
   long pointIdx;
   
   if(idx == 0) {
      pointIdx = pO->getStartIdx();
   } else if(idx == 1) {
      pointIdx = pO->getEndIdx(idx);
   } else {
      throw;
   }

   //std::cout << "Idx: " << pointIdx << "No. of points: " << pO->getNumberOfPoints() << std::endl;
   //std::cout << "getIndex(): " << pO->getIndex() << std::endl;
   
   return pointIdx;
}
};

// two dimensions
template<typename obj>
struct gts_point_getter<2, obj> {
static long getPointIdx(obj* pO, long idx) {
   return pO->getPointIdx(idx);
}
};

// three dimensions
template<typename obj>
struct gts_point_getter<3, obj> {
static long getPointIdx(obj* pO, long idx) {
   return pO->getPointIdx(idx);
}
};   

template<typename ArrayT>
struct gtsio3_point_wrapper
{
   // -------------------------------------------------------------------------------------------
   typedef gtsio3_point_wrapper<ArrayT>            self_type;
   typedef typename ArrayT::numeric_type           result_type;
   // -------------------------------------------------------------------------------------------   
   gtsio3_point_wrapper(ArrayT& array) : array_(array) {}
   // -------------------------------------------------------------------------------------------   
   result_type& operator[](size_t i) const
   { 
//       return array_[i];      
   }      
   result_type& operator[](size_t i) 
   { 
/*      return array_[i];    */  
   }      
   // -------------------------------------------------------------------------------------------
   size_t size() const
   {
//       return ArrayT::dimension_tag::value;
   }
   size_t size() 
   {
//       return ArrayT::dimension_tag::value;
   }
   // -------------------------------------------------------------------------------------------
   friend std::ostream& operator<<(std::ostream& ostr, self_type const& val) 
   {
//       size_t size = val.size();
//       if (size == 0) return ostr;
//       for (size_t i = 0; i < size; i++)
//       {
//          if (i == size-1) ostr << val[i];
//          else             ostr << val[i] << "  ";
//       }
      return ostr;
   }      
   // -------------------------------------------------------------------------------------------   
   ArrayT & array_;
};

template<typename Device>
struct gtsio3_cell_complex_wrapper
{
   typedef std::vector<double>  cell_type;
   
   gtsio3_cell_complex_wrapper(Device& device, size_t segment_id) : device_(device), segment_id_(segment_id) {}
   
   struct cell_iterator : viennamesh::iterator_base< gtsio3_cell_complex_wrapper<Device> > 
   {
      typedef  viennamesh::iterator_base< gtsio3_cell_complex_wrapper<Device> > iterator_base_type;

      cell_iterator(gtsio3_cell_complex_wrapper<Device>& obj)                : iterator_base_type (obj)         {};
      cell_iterator(gtsio3_cell_complex_wrapper<Device>& obj, size_t newpos) : iterator_base_type (obj, newpos) {};      

      cell_type 
      operator*() const
      {
         GTSIO::GTSSegment*         pSeg     = device_->getGTSSegment(segment_id_);
         GTSIO::GTSSegmentBoundary* pSegBdry = reinterpret_cast<GTSIO::GTSSegmentBoundary*> (pSeg);               
         typedef std::vector<int>   boundary_cont_type;
         boundary_cont_type boundary_element_indices = pSegBdry->getBoundary();
         size_t list_access = boundary_element_indices[this->pos()];
         
         // a cell is a line ..         
         if((device_->getGTSPolyhedronListSize()==0) && (device_->getGTSFaceListSize()==0))
         {
            GTSIO::GTSLineList* pLL = device_->getGTSLineList(0);  // TODO only using the first list ..
            GTSIO::GTSLine* edge = pLL->get( list_access );
            
            
            return cell_type();
         }
         // a cell is a face ..                  
         else
         if(device_->getGTSPolyhedronListSize()==0)
         {
            return cell_type();
         }
         // a cell is a polyhedron ..
         else
         {
            return cell_type();
         }
      }                   
   };
   cell_iterator cell_begin() const
   {
      return cell_iterator(*this);
   }     
   cell_iterator cell_begin()
   {
      return cell_iterator(*this);
   }  
   cell_iterator cell_end() const
   {
      GTSIO::GTSSegment*         pSeg     = device_->getGTSSegment(segment_id_);
      GTSIO::GTSSegmentBoundary* pSegBdry = reinterpret_cast<GTSIO::GTSSegmentBoundary*> (pSeg);      
      return cell_iterator(*this, pSegBdry->getBoundary().size());  
   }        
   cell_iterator cell_end()
   {
      GTSIO::GTSSegment*         pSeg     = device_->getGTSSegment(segment_id_);
      GTSIO::GTSSegmentBoundary* pSegBdry = reinterpret_cast<GTSIO::GTSSegmentBoundary*> (pSeg);      
      return cell_iterator(*this, pSegBdry->getBoundary().size());  
   }        
   
  
   inline size_t   segment_id()  { return segment_id_; }
   
   Device&  device_;
   size_t   segment_id_;

};


template<typename Datastructure>
struct wrapper <viennamesh::tag::gtsio3, Datastructure>
{ 
   // -------------------------------------------------------------------------------------------   
   // provide wrappers for the datastructures
   //
   typedef std::vector<double>                                                      point_wrapper_type;
   typedef typename viennamesh::gtsio3_cell_complex_wrapper<Datastructure>          cell_complex_wrapper_type;
   // -------------------------------------------------------------------------------------------   
   
   // -------------------------------------------------------------------------------------------   
   wrapper(Datastructure& device) : device(device) {}
   // -------------------------------------------------------------------------------------------      

   
   // -------------------------------------------------------------------------------------------
   struct geometry_iterator : viennamesh::iterator_base< viennamesh::wrapper< viennamesh::tag::gtsio3, Datastructure > >
   {
      typedef viennamesh::wrapper< viennamesh::tag::gtsio3, Datastructure >         wrapper_type;
      typedef viennamesh::iterator_base< wrapper_type >                             iterator_base_type;
      
      geometry_iterator(wrapper_type& obj)                : iterator_base_type (obj)         {};
      geometry_iterator(wrapper_type& obj, std::size_t newpos) : iterator_base_type (obj, newpos) {};      

      point_wrapper_type 
      operator*() const
      {
         // TODO reading only the first pointlist now ..!!
         GTSIO::IGTSPointList* pointList = device->getGTSPointList(0);     
         std::vector<double> pointCoords;
         pointList->get( this->pos(),pointCoords);
         return pointCoords;
      }            

   };   
   geometry_iterator geometry_begin()
   {
      return geometry_iterator(*this);
   }  
   geometry_iterator geometry_end()
   {
/*      for (int idx = 0; idx < device->getGTSPointListSize(); ++idx) 
      {      
         GTSIO::IGTSPointList* pointList = device->getGTSPointList(idx);      
      }*/
      // TODO reading only the first pointlist now ..!!
      //
      GTSIO::IGTSPointList* pointList = device->getGTSPointList(0);      
      return geometry_iterator(*this, pointList->size());
   }   
   // -------------------------------------------------------------------------------------------   
   
   // -------------------------------------------------------------------------------------------
   struct segment_iterator : viennamesh::iterator_base< viennamesh::wrapper< viennamesh::tag::gtsio3, Datastructure > >
   {
      typedef viennamesh::wrapper< viennamesh::tag::gtsio3, Datastructure >      wrapper_type;
      typedef viennamesh::iterator_base< wrapper_type >                          iterator_base_type;
      
      segment_iterator(wrapper_type& obj)                : iterator_base_type (obj)         {};
      segment_iterator(wrapper_type& obj, size_t newpos) : iterator_base_type (obj, newpos) {};      

      cell_complex_wrapper_type 
      operator*() const
      {  
         return cell_complex_wrapper_type(device, (*this).pos());
      }            
   };   
   segment_iterator segment_begin()
   {
      return segment_iterator(*this);
   }  
   segment_iterator segment_end()
   {
      return segment_iterator(*this, device->getGTSSegmentListSize());
   }      
   // -------------------------------------------------------------------------------------------      
   
   // -------------------------------------------------------------------------------------------      
   Datastructure& device;
   // -------------------------------------------------------------------------------------------      
};

} // end namespace viennamesh

#endif