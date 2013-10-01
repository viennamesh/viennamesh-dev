#ifndef VIENNAMESH_BASE_DOMAIN_HPP
#define VIENNAMESH_BASE_DOMAIN_HPP

namespace viennamesh
{
  struct NoSegmentation {};
  
  namespace result_of
  {
    
    template<typename MeshT, typename SegmentationT = NoSegmentation>
    struct best_matching_viennagrid_mesh;    
    
    template<typename MeshT, typename SegmentationT = NoSegmentation>
    struct best_matching_viennagrid_segmentation;
    
//     template<typename SegementationT>

  }
}

#endif