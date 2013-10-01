#ifndef VIENNAMESH_BASE_CONVERT_HPP
#define VIENNAMESH_BASE_CONVERT_HPP

#include "viennagrid/mesh/segmentation.hpp"
#include "viennamesh/base/mesh.hpp"

namespace viennamesh
{
  template<typename InputMeshT, typename InputSegmentationT, typename OutputMeshT, typename OutputSegmentationT>
  struct convert_impl
  {
  public:
//     template<typename InputMeshT, typename InputSegmentationT, typename OutputMeshT, typename OutputSegmentationT>
//     static bool convert( InputMeshT const & input_mesh, InputSegmentationT const & input_segmentation,
//                           OutputMeshT & output_mesh, OutputSegmentationT & output_segmentation );
    
      // Prototype for convert
      // static bool convert( input_mesh_type const & in, input_segmentation_type const & input_segmentation,
      //                      output_mesh_type & out, output_segmentation_type & output_segmentation );
  };
  
  
  template<typename InputMeshT, typename InputSegmentationT, typename OutputMeshT, typename OutputSegmentationT>
  bool convert( InputMeshT const & in, InputSegmentationT const & input_segmentation, OutputMeshT & out, OutputSegmentationT & output_segmentation )
  {
      bool status = convert_impl<InputMeshT, InputSegmentationT, OutputMeshT, OutputSegmentationT>::
          convert(in, input_segmentation, out, output_segmentation);
      
      return status;
  }
  
  
  template<typename InputMeshT, typename OutputMeshT>
  bool convert( InputMeshT const & in, OutputMeshT & out )
  {
    NoSegmentation source_segmentation;
    NoSegmentation destination_segmentation;
    
    bool status = convert_impl<InputMeshT, NoSegmentation, OutputMeshT, NoSegmentation>::convert(in, source_segmentation, out, destination_segmentation);

      return status;
  }
  
  template<typename mesh_type>
  bool convert( mesh_type const & in, mesh_type & out )
  {
      if (&in != &out)
          out = in;

      return true;
  }
  
  
//   template<typename InputMeshT, typename InputSegmentationT, typename OutputMeshT, typename OutputSegmentationT>
//   bool convert_impl<InputMeshT, InputSegmentationT, OutputMeshT, OutputSegmentationT>::convert( InputMeshT const & input_mesh, InputSegmentationT const & input_segmentation,
//                           OutputMeshT & output_mesh, OutputSegmentationT & output_segmentation )
//     {
//       typedef typename viennamesh::result_of::best_matching_viennagrid_mesh<InputMeshT, InputSegmentationT>::type BestMatchingViennaGridMeshType;
//       typedef typename viennamesh::result_of::best_matching_viennagrid_segmentation<InputMeshT, InputSegmentationT>::type BestMatchingViennaGridSegmentationType;
//       
//       BestMatchingViennaGridMeshType viennagrid_mesh;
//       BestMatchingViennaGridSegmentationType viennagrid_segmentation;
//       
//       convert( input_mesh, input_segmentation, viennagrid_mesh, viennagrid_segmentation );
//       convert( viennagrid_mesh, viennagrid_segmentation, output_mesh, output_segmentation );
//     }
  
  
}

#endif