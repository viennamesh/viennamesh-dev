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

#ifndef VIENNAMESH_CLASSIFIER_VGMODELER_HPP
#define VIENNAMESH_CLASSIFIER_VGMODELER_HPP

#include <vector>
#include <map>

#include "viennamesh/classifier/base.hpp"
#include "viennamesh/classifier/vgmodeler_algorithm.hpp"
#include "viennamesh/traits.hpp"

#define MESH_CLASSIFIER_DEBUG

namespace viennamesh {
 
  
template <>
struct mesh_classifier <viennamesh::tag::vgmodeler>
{
   // -------------------------------------------------------------------------------------
   mesh_classifier() : id("vgmodeler") {}
   // -------------------------------------------------------------------------------------
    
   // ------------------------------------------------------------------------------------- 
   template<typename DatastructureT>
   void operator()(DatastructureT& data)
   {
   #ifdef MESH_CLASSIFIER_DEBUG
      std::cout << "## MeshClassifier::"+id+" - initiating" << std::endl;
   #endif            
      eval(data, typename viennamesh::traits::datastructure<DatastructureT>::type());
   }
   // -------------------------------------------------------------------------------------    
   
private:
   // -------------------------------------------------------------------------------------    
   template<typename DatastructureT>   
   inline void eval(DatastructureT& data, viennamesh::tag::mesh_kernel const&)
   {
   #ifdef MESH_CLASSIFIER_DEBUG
      std::cout << "## MeshClassifier::"+id+" - processing mesh kernel datastructure" << std::endl;
   #endif            
      static const int DIMG = DatastructureT::DIMG;
      static const int DIMT = DatastructureT::DIMT;
      static const int CELLSIZE = DIMT+1; // only for simplices!
   #ifdef MESH_CLASSIFIER_DEBUG
      std::cout << "## MeshClassifier::"+id+" - detected topology dimension: " << DIMT << std::endl;
   #endif                  
      
      typedef typename DatastructureT::geometry_container_type             mesher_geometry_cont_type;
      typedef typename DatastructureT::segment_container_type              mesher_segment_container_type;
      typedef typename mesher_segment_container_type::value_type           mesher_cell_container_type;
      typedef typename DatastructureT::numeric_type                        numeric_type;
      
      typedef viennamesh::vgmodeler::SimplexClassification<numeric_type>            simplex_classification_type;
      typedef typename simplex_classification_type::angle_container_type::iterator  angle_iterator_type;
      
      typedef typename viennamesh::writeLatex<3>::histogram_type histogram_type;
      histogram_type histogram;      
      histogram[viennamesh::vgmodeler::keys::round]   = 0;
      histogram[viennamesh::vgmodeler::keys::wedge]   = 0;
      histogram[viennamesh::vgmodeler::keys::slat]    = 0;
      histogram[viennamesh::vgmodeler::keys::needle]  = 0;
      histogram[viennamesh::vgmodeler::keys::spindle] = 0;
      histogram[viennamesh::vgmodeler::keys::spade]   = 0;
      histogram[viennamesh::vgmodeler::keys::cap]     = 0;
      histogram[viennamesh::vgmodeler::keys::sliver]  = 0;          
      
      typedef std::vector< numeric_type >   angles_type;
      angles_type angles;
      
      mesher_geometry_cont_type&     geom_cont = data.geometry();
      mesher_segment_container_type& seg_cont  = data.topology();      

      static const int SIMPLEX_POINT_SIZE = DIMG*CELLSIZE;
      double simplex_point_array[SIMPLEX_POINT_SIZE];
      
      for(typename mesher_segment_container_type::iterator si = seg_cont.begin();
         si != seg_cont.end(); si++)
      {
         for(typename mesher_cell_container_type::iterator ci = (*si).begin();
            ci != (*si).end(); ci++)
         {
            for(int i = 0; i < CELLSIZE; i++)
            {
               std::size_t vertex = (*ci)[i];
               
               for(int dim = 0; dim < DIMG; dim++)
               {
                  // load the simplex point array
                  // take care of the offset, the data ordering is
                  // [p0_x, p0_y, p0_z, p1_x, p1_y,...]^T
                  simplex_point_array[(i*DIMG)+dim] = geom_cont[vertex][dim];
               }
            }    
//             for(size_t i = 0; i < SIMPLEX_POINT_SIZE; i++)
//             {
//                std::cout << simplex_point_array[i] << std::endl;
//             };

            simplex_classification_type classification(simplex_point_array, simplex_point_array+SIMPLEX_POINT_SIZE);
//             std::cout << classification << std::endl;
//             exit(0);
            
            // record the result within the histogram
            histogram[classification.ResultName]++;
            
            // record all angles within the angles container
            for(angle_iterator_type iter = classification.DiAngle.begin();
                  iter != classification.DiAngle.end(); iter++)
            {
               angles.push_back(*iter);
            }            
         }
      }    
      
     
   #ifdef MESH_CLASSIFIER_DEBUG
      std::cout << "## MeshClassifier::"+id+" - writing latex code" << std::endl;
   #endif               
      // transform histogram data into latex code, and dump it to screen
      // note: use a file stream to print to a file ..
      std::ofstream  histo_stream("histogram.txt"); 
      viennamesh::writeLatex<3>::eval(histogram, histo_stream );
      histo_stream.close();
      
   #ifdef MESH_CLASSIFIER_DEBUG
      std::cout << "## MeshClassifier::"+id+" - writing latex code" << std::endl;
   #endif               
      std::ofstream  angle_stream("angles.txt");       
      this->dump_angles(angles, angle_stream);
      angle_stream.close();
      
   #ifdef MESH_CLASSIFIER_DEBUG
      std::cout << "## MeshClassifier::"+id+" - result:" << std::endl;
   #endif  
      // dump the classification results in a human readable form to the screen
      this->dump_histogram(histogram, "  ", std::cout);
   }
   // -------------------------------------------------------------------------------------    
   
   // -------------------------------------------------------------------------------------    
   template<typename DatastructureT>   
   inline void eval(DatastructureT& data, viennamesh::tag::wrapper const&)
   {
   #ifdef MESH_CLASSIFIER_DEBUG
      std::cout << "## MeshClassifier::"+id+" - ERROR: not yet implemented for a wrapper datastructure" << std::endl;
   #endif              
   }
   // -------------------------------------------------------------------------------------    
   
   template<typename KeyT, typename ValueT>
   void dump_histogram(std::map<KeyT, ValueT>& histogram, std::string prefix = "", std::ostream& ostr = std::cout)
   {
      size_t sum = 0;
      for(typename std::map<KeyT, ValueT>::iterator iter = histogram.begin();
          iter != histogram.end(); iter++)
      {
         sum += iter->second;
      }      
      ostr << prefix << "----------------------------------------" << std::endl;
      for(typename std::map<KeyT, ValueT>::iterator iter = histogram.begin();
          iter != histogram.end(); iter++)
      {
         double percentage = ( double(iter->second) / double(sum))*100.0;
         ostr.precision(2);
         ostr << prefix << iter->first << ":  " << percentage << "%" << std::endl;
      }      
      ostr << prefix << "----------------------------------------" << std::endl;      
   }
   
   template<typename ValueT>
   void dump_angles(std::vector<ValueT>& histogram, std::ostream& ostr = std::cout)
   {
      for(typename std::vector<ValueT>::iterator iter = histogram.begin();
          iter != histogram.end(); iter++)
      {
         ostr.precision(2);
         ostr << *iter << std::endl;
      }      
   }   
   
   // ------------------------------------------------------------------------------------- 
   std::string id;
   // -------------------------------------------------------------------------------------    
};
  


} // end namespace viennamesh

#endif
