#ifndef VIENNAMESH_CORE_MESH_HPP
#define VIENNAMESH_CORE_MESH_HPP

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
