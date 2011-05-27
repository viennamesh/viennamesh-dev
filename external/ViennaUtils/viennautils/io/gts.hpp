/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                ViennaUtils - The Vienna Utilities Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               Peter Lagger                       peter.lagger@ieee.org

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAUTILS_IO_GTS
#define VIENNAUTILS_IO_GTS

#include <vector>
#include <map>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cctype>
#include <exception>
#include <algorithm>

#include "boost/array.hpp"

#include "viennagrid/forwards.h"
#include "viennagrid/domain.hpp"

#include "creategridfromortho.h"
#include "kdtreeobjects.h"
#include "normalizedpointlist.h"

#include "gtsio3.h"

//#define IODEBUG

namespace viennautils
{
  namespace io
  {
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
   
   
struct gts_reader
{
   void printDeviceInfo(GTSIO::GTSDevice* pDevice, int maxListValues, bool printAttributes) {
      if (pDevice == NULL) {
         std::cerr << "ERROR: Can't show device-info as no device has been loaded!" << std::endl;
         return;
      }

      pDevice->DumpDeviceInfo(maxListValues, printAttributes);
   }   
   
   template<typename DomainT>
   void operator()(DomainT & domain, std::string const& filename, bool printdevinfo = false)
   {
      typedef typename DomainT::config_type                     DomainConfiguration;

      typedef typename DomainConfiguration::numeric_type                 CoordType;
      typedef typename DomainConfiguration::dimension_tag              DimensionTag;
      typedef typename DomainConfiguration::cell_tag                   CellTag;

      typedef typename viennagrid::result_of::point_type<DomainConfiguration>::type                              PointType;
      typedef typename viennagrid::result_of::ncell_type<DomainConfiguration, 0>::type                           VertexType;
      typedef typename viennagrid::result_of::ncell_type<DomainConfiguration, 1>::type                           EdgeType;      
      typedef typename viennagrid::result_of::ncell_type<DomainConfiguration, CellTag::topology_level>::type     CellType;
      //typedef typename DomainTypes<DomainConfiguration>::segment_type  Segment;

      typedef typename viennagrid::result_of::ncell_container<DomainT, 0>::type   VertexContainer;
      typedef typename viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;
         
      typedef typename viennagrid::result_of::ncell_container<DomainT, 1>::type   EdgeContainer;
      typedef typename viennagrid::result_of::iterator<EdgeContainer>::type          EdgeIterator;

      typedef typename viennagrid::result_of::ncell_container<DomainT, CellTag::topology_level-1>::type   FacetContainer;
      typedef typename viennagrid::result_of::iterator<FacetContainer>::type                                 FacetIterator;

      typedef typename viennagrid::result_of::ncell_container<DomainT, CellTag::topology_level>::type     CellContainer;
      typedef typename viennagrid::result_of::iterator<CellContainer>::type                                  CellIterator;
      
      typedef typename viennautils::io::gts_list_getter<DimensionTag::value>::list_type         GTSCellObjList;
      typedef typename viennautils::io::gts_list_getter<DimensionTag::value>::object_type       GTSCellObj;      
      
      typedef typename viennautils::io::gts_list_getter<1>::list_type         GTSEdgeObjList;
      typedef typename viennautils::io::gts_list_getter<1>::object_type       GTSEdgeObj;        
      
      GTSIO::DeviceFileReader* pReader = new GTSIO::DeviceFileReader();
      GTSIO::GTSDevice*        pDevice = new GTSIO::GTSDevice();
      try {
         pReader->loadDeviceFromFile(filename.c_str(), pDevice);
      } 
      catch (GTSIO::GtsioException ex) {
         std::cerr << ex.toString() << std::endl << std::endl;
      }
      catch (std::exception stdEx) {
         std::cerr << stdEx.what() << std::endl << std::endl;
      }
      if(printdevinfo)
      {
         int  maxListValues = 10;
         bool hasGridOption = true;
         printDeviceInfo(pDevice, maxListValues, !hasGridOption);
      }
      
      // ------------------------------------------------------------------
      // segments
      //
      
      // stores for a given cell id the corresponding segment id
      std::map<int, int>   cell_segment_map; 

      // create domain segments
      domain.create_segments(pDevice->getGTSSegmentListSize());
      
      //std::cout << "seg size: " << pDevice->getGTSSegmentListSize() << std::endl;
      for (int idx = 0; idx < pDevice->getGTSSegmentListSize(); ++idx) 
      {
         GTSIO::GTSSegment*         pSeg     = pDevice->getGTSSegment(idx);
         GTSIO::GTSSegmentBoundary* pSegBdry = reinterpret_cast<GTSIO::GTSSegmentBoundary*> (pSeg);
         
         //long        seg_id   = pSeg->getIndex();
         std::string seg_name = pSeg->getName();
         std::string material = pSeg->getMaterial();
         
         //std::cout << seg_id << std::endl;
         //std::cout << seg_name << std::endl;
         //std::cout << material << std::endl;
         //std::cout << GTSRefType::GetGTSRefTypeString(pSeg->getRefElementListType()) << std::endl;
         //std::cout << pSeg->getRefElementListName() << std::endl;
         
         typedef std::vector<int>   boundary_cont_type;
         boundary_cont_type boundary_element_indices = pSegBdry->getBoundary();
         //std::cout << "bnd element size: " << boundary_element_indices.size() << std::endl;
         
         // store the cell - segment index relation for later usage at the cell 
         // reading part ..
         //
         for(typename boundary_cont_type::iterator iter = boundary_element_indices.begin();
             iter != boundary_element_indices.end(); iter++)
         {
            cell_segment_map[*iter] = idx;
         }
      }
      
      

      // ------------------------------------------------------------------
      // read vertices
      //    note: alls point lists are transfered to the domain
      //
      for (int idx = 0; idx < pDevice->getGTSPointListSize(); ++idx) 
      {      
         //std::cout << "reading point-list: " << idx << std::endl;
      
         GTSIO::IGTSPointList* pointList = pDevice->getGTSPointList(idx);
         long lenPointList = 0;
         lenPointList = pointList->size();
         
         //std::cout << "  size " << lenPointList << std::endl;
         domain.reserve_vertices(lenPointList);
         
         std::vector<double> pointCoords;
         long pointDim = 0; 
         VertexType vertex;
         
         for(long i = 0; i < lenPointList; i++)
         {
            CoordType coords[DimensionTag::value]; 
            
            //std::cout << "point " << i << ": ";
            
            pointList->get(i,pointCoords); // TODO get rid of this runtime container approach ...
            pointDim = pointCoords.size();
            
            for(long j = 0; j < pointDim; j++)
            {
               //std::cout.precision(3);
               //std::cout << pointCoords[j] << " ";
               
               coords[j] = pointCoords[j];
            }
            
            vertex.getPoint().setCoordinates(coords);
            vertex.setID(i);
            domain.add(vertex);
            
            pointCoords.clear();
            //std::cout << std::endl;
         }      
      }
      // ------------------------------------------------------------------
      // read lines
      //    note if the sizes of the topo elements higher than edges are zero
      //    than the edges are actually cells
      if((pDevice->getGTSPolyhedronListSize()==0) && (pDevice->getGTSFaceListSize()==0))
      {
         //typedef boost::array<size_t, 2> bedge_type;
         //bedge_type bedge;
         //std::map<bedge_type, bool>    edge_uniquer;
         
         for (int idx = 0; idx < pDevice->getGTSLineListSize(); ++idx) 
         {          
            //std::cout << "reading line-list: " << idx << std::endl;
            
            GTSIO::GTSLineList* pLL = pDevice->getGTSLineList(idx);
            long lenList = 0;
            lenList = pLL->size();
            
            //std::cout << "  size " << lenList << std::endl;
            
            long pointIdx0, pointIdx1    = 0; 
            domain.reserve_cells(lenList);
            
            
            for(long i = 0; i < lenList; i++)
            {
               GTSIO::GTSLine* edge = pLL->get(i);
               VertexType *vertices[viennagrid::traits::subcell_desc<CellTag, 0>::num_elements];          

               pointIdx0 = viennautils::io::gts_point_getter<1,GTSIO::GTSLine>::getPointIdx(edge,0);
               pointIdx1 = viennautils::io::gts_point_getter<1,GTSIO::GTSLine>::getPointIdx(edge,1);

               //std::cout << "cell " << i << ": " << pointIdx0 << " " << pointIdx1 << std::endl;

               vertices[0] = &(domain.vertex(pointIdx0));               
               vertices[1] = &(domain.vertex(pointIdx1));            

//               std::cout << vertices[0]->getPoint() << std::endl;
//               std::cout << vertices[1]->getPoint() << std::endl;
               CellType cell;
               cell.setVertices(vertices);

               //bedge[0] = pointIdx0;
               //bedge[1] = pointIdx1;
               
               //std::sort(bedge.begin(), bedge.end());
               
               // retrieve the segment id for the current edge
               // and use it to store the cell on this specific segment
               
               //if(!edge_uniquer[bedge])
               {
                  domain.segment(cell_segment_map[i]).add(cell);

                  //edge_uniquer[bedge] = true;
               }
               //else
                  
               
//                cell.setVertices(&(vertices[0]));
//                cell.setID(i);
//                domain.add(cell);    
            }
         }      
      }
      else // a line is an edge ..
      {  // TODO
/*         for (int idx = 0; idx < pDevice->getGTSLineListSize(); ++idx) 
         {          
            std::cout << "reading line-list: " << idx << std::endl;
            
            GTSIO::GTSLineList* pLL = pDevice->getGTSLineList(idx);
            long lenList = 0;
            lenList = pLL->size();
            
            std::cout << "  size " << lenList << std::endl;
            
            long pointIdx    = 0; 
            domain.reserve_cells(lenList);
            EdgeType edge;            
            
            for(long i = 0; i < lenList; i++)
            {
               GTSIO::GTSLine* line = pLL->get(i);
               VertexType *vertices[viennagrid::traits::subcell_desc<CellTag, 0>::num_elements];          

               pointIdx = viennautils::io::gts_point_getter<1,GTSIO::GTSLine>::getPointIdx(line,0);
               vertices[0] = &(domain.vertex(pointIdx));               
               pointIdx = viennautils::io::gts_point_getter<1,GTSIO::GTSLine>::getPointIdx(line,1);
               vertices[1] = &(domain.vertex(pointIdx));            

               edge.setVertices(&(vertices[0]));
               edge.setID(i);
               domain.add(edge);    
            }
         }     */ 
      }         
      // ------------------------------------------------------------------
      // read cells
      //
      for (int idx = 0; idx < pDevice->getGTSPolyhedronListSize(); ++idx) 
      {          
         //std::cout << "reading polyhedron-list: " << idx << std::endl;
         
         GTSCellObjList* objList = gts_list_getter<DimensionTag::value>::getList(pDevice);
         
         long lenObjList = 0;  
         long pointsPerObj   = 0; 
         long pointIdx    = 0; 
                 
         lenObjList = objList->size();
         
         domain.reserve_cells(lenObjList);
         CellType cell;
         
         for(long i = 0; i < lenObjList; i++) 
         {
            GTSCellObj* pObject = objList->get(i);
            pointsPerObj = pObject->getNumberOfPoints(); 
        
            VertexType *vertices[viennagrid::traits::subcell_desc<CellTag, 0>::num_elements];          
         
            for(long j = 0; j < pointsPerObj; j++) 
            {
               pointIdx = viennautils::io::gts_point_getter<DimensionTag::value,GTSCellObj>::getPointIdx(pObject,j);
               vertices[j] = &(domain.vertex(pointIdx));
            }
         
            cell.setVertices(&(vertices[0]));
            cell.setID(i);
          
         }
      }
      delete pReader;
      delete pDevice;
   }
   
};    
    
  } // end namespace io
} // end namespace viennautils

#endif
