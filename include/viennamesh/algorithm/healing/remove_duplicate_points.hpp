#ifndef VIENNAMESH_ALGORITHM_HEALING_REMOVE_DUPLICATES_POINTS_HPP
#define VIENNAMESH_ALGORITHM_HEALING_REMOVE_DUPLICATES_POINTS_HPP

#include "viennagrid/mesh/mesh_operations.hpp"

namespace viennamesh
{
  template<typename NumericConfigT>
  struct remove_duplicate_points_heal_functor
  {
    remove_duplicate_points_heal_functor(NumericConfigT nc_) : nc(nc_) {}

    template<typename MeshT>
    bool operator()(MeshT const & mesh) const
    {
      // TODO optimize!!
      MeshT tmp;
      (*this)(mesh, tmp);
      return viennagrid::vertices(mesh).size() == viennagrid::vertices(tmp).size();
    }

    template<typename MeshT>
    std::size_t operator()(MeshT const & input_mesh, MeshT & output_mesh) const
    {
      viennagrid::vertex_copy_map<MeshT, MeshT> vertex_map( output_mesh, nc );
      viennagrid::copy( vertex_map, input_mesh, output_mesh,
                        viennagrid::true_functor() );

      return viennagrid::vertices(input_mesh).size() == viennagrid::vertices(output_mesh).size();
    }

    template<typename MeshT, typename SegmentationT>
    std::size_t operator()(viennagrid::segmented_mesh<MeshT, SegmentationT> const & input_mesh,
                    viennagrid::segmented_mesh<MeshT, SegmentationT> & output_mesh) const
    {
      viennagrid::vertex_copy_map<MeshT, MeshT> vertex_map( output_mesh.mesh, nc );
      viennagrid::copy( vertex_map,
                        input_mesh.mesh, input_mesh.segmentation,
                        output_mesh.mesh, output_mesh.segmentation,
                        viennagrid::true_functor() );

      return viennagrid::vertices(input_mesh.mesh).size() == viennagrid::vertices(output_mesh.mesh).size();
    }

    NumericConfigT nc;
  };

}

#endif
