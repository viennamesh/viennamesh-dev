#ifndef VIENNAMESH_ALGORITHM_EXTRACT_HULL_HPP
#define VIENNAMESH_ALGORITHM_EXTRACT_HULL_HPP

#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/boundary.hpp"

#include "viennamesh/core/algorithm.hpp"



namespace viennamesh
{

  namespace extract_hull
  {




    template<typename HullTypeOrTagT, typename VolumeMeshT, typename HullMeshT>
    void extract_hull( VolumeMeshT const & volume_mesh,
                        HullMeshT & hull_mesh)
    {
      typedef typename viennagrid::result_of::point<VolumeMeshT>::type            VolumePointType;

      typedef typename viennagrid::result_of::const_element_range<VolumeMeshT, HullTypeOrTagT>::type    HullRangeType;
      typedef typename viennagrid::result_of::iterator<HullRangeType>::type                                   HullRangeIterator;

      typedef typename viennagrid::result_of::element<VolumeMeshT, HullTypeOrTagT>::type    VolumeHullElement;
      typedef typename viennagrid::result_of::element<HullMeshT, HullTypeOrTagT>::type      HullHullElement;

      HullRangeType hull_elements( volume_mesh );
      for (HullRangeIterator hit = hull_elements.begin(); hit != hull_elements.end(); ++hit)
      {
        VolumeHullElement const & hull_element = *hit;

        if ( viennagrid::is_boundary( volume_mesh, hull_element ) )
        {
          typedef typename viennagrid::result_of::vertex_handle<HullMeshT>::type HullVertexHandleType;

          std::vector<HullVertexHandleType> vertices;
          vertices.resize( viennagrid::vertices(hull_element).size() );

          for (std::size_t i = 0; i < viennagrid::vertices(hull_element).size(); ++i)
            vertices[i] = viennagrid::make_unique_vertex( hull_mesh, viennagrid::point(volume_mesh, viennagrid::vertices(hull_element)[i]) );

          viennagrid::make_element<HullHullElement>( hull_mesh, vertices.begin(), vertices.end() );
        }
      }
    }



    template<typename HullTypeOrTagT, typename VolumeMeshT, typename VolumeSegmentationT, typename HullMeshT, typename HullSegmentationT>
    void extract_hull( VolumeMeshT const & volume_mesh,
                        VolumeSegmentationT const & volume_segmentation,
                        HullMeshT & hull_mesh,
                        HullSegmentationT & hull_segmentation )
    {
      if (volume_segmentation.size() <= 1)
        extract_hull<HullTypeOrTagT>(volume_mesh, hull_mesh);

      typedef typename viennagrid::result_of::segment_handle<VolumeSegmentationT>::type    VolumeSegmentHandleType;
      typedef typename viennagrid::result_of::point<VolumeMeshT>::type            VolumePointType;

      typedef typename viennagrid::result_of::segment_handle<HullSegmentationT>::type      HullSegmentHandleType;

      for (typename VolumeSegmentationT::const_iterator sit = volume_segmentation.begin(); sit != volume_segmentation.end(); ++sit)
      {
        VolumeSegmentHandleType const & volume_segment = *sit;
        HullSegmentHandleType & hull_segment = hull_segmentation( volume_segment.id() );

        typedef typename viennagrid::result_of::const_element_range<VolumeSegmentHandleType, HullTypeOrTagT>::type    HullRangeType;
        typedef typename viennagrid::result_of::iterator<HullRangeType>::type                                   HullRangeIterator;

        typedef typename viennagrid::result_of::element<VolumeSegmentHandleType, HullTypeOrTagT>::type    VolumeHullElement;
        typedef typename viennagrid::result_of::element<HullSegmentHandleType, HullTypeOrTagT>::type      HullHullElement;

        HullRangeType hull_elements( volume_segment );
        for (HullRangeIterator hit = hull_elements.begin(); hit != hull_elements.end(); ++hit)
        {
          VolumeHullElement const & hull_element = *hit;

          if ( viennagrid::is_boundary( volume_segment, hull_element ) )
          {
            typedef typename viennagrid::result_of::vertex_handle<HullSegmentHandleType>::type HullVertexHandleType;

            std::vector<HullVertexHandleType> vertices;
            vertices.resize( viennagrid::vertices(hull_element).size() );

            for (std::size_t i = 0; i < viennagrid::vertices(hull_element).size(); ++i)
              vertices[i] = viennagrid::make_unique_vertex( hull_mesh, viennagrid::point(volume_segment, viennagrid::vertices(hull_element)[i]) );

            viennagrid::make_element<HullHullElement>( hull_segment, vertices.begin(), vertices.end() );
          }
        }
      }
    }









    template<typename SegmentationT, typename SeedPointContainerT>
    void extract_seed_points( SegmentationT const & segmentation, SeedPointContainerT & seed_points )
    {
      typedef typename viennagrid::result_of::cell_tag<SegmentationT>::type CellTag;
      typedef typename viennagrid::result_of::point<SegmentationT>::type point_type;

      for (typename SegmentationT::const_iterator it = segmentation.begin(); it != segmentation.end(); ++it)
      {
        if (!viennagrid::elements<CellTag>(*it).empty())
        {
          point_type centroid = viennagrid::centroid( viennagrid::elements<CellTag>(*it)[0] );
          seed_points.push_back( std::make_pair(centroid, it->id()) );
        }
      }
    }



    class Algorithm : public BaseAlgorithm
    {
    public:

      string name() const { return "ViennaGrid Extract Hull"; }

      bool run_impl()
      {
        {
          typedef viennagrid::segmented_mesh<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> InputMeshType;
          typedef viennagrid::segmented_mesh<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> OutputMeshType;

          viennamesh::result_of::const_parameter_handle<InputMeshType>::type input_mesh = inputs.get<InputMeshType>("default");

          if (input_mesh)
          {
            viennamesh::result_of::parameter_handle<OutputMeshType>::type output_mesh = make_parameter<OutputMeshType>();
            SeedPoint3DContainer seed_points;

            extract_hull<viennagrid::triangle_tag>(input_mesh->get().mesh, input_mesh->get().segmentation,
                                                   output_mesh->get().mesh, output_mesh->get().segmentation);

            extract_seed_points(input_mesh->get().segmentation, seed_points);

            outputs.set( "default", output_mesh );
            outputs.set( "seed_points", seed_points );
            return true;
          }
        }


        error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
        return false;
      }

    private:
    };

  }
}

#endif
