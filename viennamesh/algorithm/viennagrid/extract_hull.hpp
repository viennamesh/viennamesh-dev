#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_EXTRACT_HULL_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_EXTRACT_HULL_HPP

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
      viennagrid::clear(hull_mesh);

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
      typedef typename viennagrid::result_of::element_tag<HullTypeOrTagT>::type HullTagType;

      viennagrid::clear(hull_mesh);
      viennagrid::clear(hull_segmentation);

      if (volume_segmentation.size() <= 1)
        extract_hull<HullTagType>(volume_mesh, hull_segmentation.make_segment() );

      typedef typename viennagrid::result_of::segment_handle<VolumeSegmentationT>::type    VolumeSegmentHandleType;
      typedef typename viennagrid::result_of::point<VolumeMeshT>::type            VolumePointType;

      typedef typename viennagrid::result_of::segment_handle<HullSegmentationT>::type      HullSegmentHandleType;

      for (typename VolumeSegmentationT::const_iterator sit = volume_segmentation.begin(); sit != volume_segmentation.end(); ++sit)
      {
        VolumeSegmentHandleType const & volume_segment = *sit;
        HullSegmentHandleType & hull_segment = hull_segmentation( volume_segment.id() );

        typedef typename viennagrid::result_of::const_element_range<VolumeSegmentHandleType, HullTagType>::type    HullRangeType;
        typedef typename viennagrid::result_of::iterator<HullRangeType>::type                                   HullRangeIterator;

        typedef typename viennagrid::result_of::element<VolumeSegmentHandleType, HullTagType>::type    VolumeHullElementType;
        typedef typename viennagrid::result_of::element<HullSegmentHandleType, HullTagType>::type     HullCellElementType;

        typedef typename viennagrid::result_of::id<VolumeHullElementType>::type VolumeHullElementIDType;
        typedef typename viennagrid::result_of::handle<HullSegmentHandleType, HullTagType>::type HullCellElementHandleType;

        typedef typename viennagrid::result_of::vertex_id<VolumeMeshT>::type VolumeVertexIDType;
        typedef typename viennagrid::result_of::vertex_handle<HullSegmentHandleType>::type HullVertexHandleType;

        std::map< VolumeVertexIDType, HullVertexHandleType > vertex_map;
        std::map< VolumeHullElementIDType, HullCellElementHandleType > hull_element_map;

        HullRangeType hull_elements( volume_segment );
        for (HullRangeIterator hit = hull_elements.begin(); hit != hull_elements.end(); ++hit)
        {
          VolumeHullElementType const & hull_element = *hit;

          if ( viennagrid::is_boundary( volume_segment, hull_element ) )
          {
            typename std::map< VolumeHullElementIDType, HullCellElementHandleType >::iterator hemit = hull_element_map.find( hit->id() );
            if ( hemit != hull_element_map.end() )
            {
              viennagrid::add( hull_segment, viennagrid::dereference_handle(hull_segment, hemit->second) );
            }
            else
            {
              typedef typename viennagrid::result_of::const_vertex_range<VolumeHullElementType>::type ConstVertexOnHullElementRangeType;
              typedef typename viennagrid::result_of::iterator<ConstVertexOnHullElementRangeType>::type ConstVertexOnHullElementIteratorType;

              ConstVertexOnHullElementRangeType vertices_on_hull_element( hull_element );
              std::vector<HullVertexHandleType> vertex_handles;

              for (ConstVertexOnHullElementIteratorType vit = vertices_on_hull_element.begin(); vit != vertices_on_hull_element.end(); ++vit)
              {
                typename std::map< VolumeVertexIDType, HullVertexHandleType >::iterator vmit = vertex_map.find( vit->id() );
                if (vmit != vertex_map.end())
                  vertex_handles.push_back( vmit->second );
                else
                {
                  vertex_handles.push_back( viennagrid::make_vertex( hull_mesh, viennagrid::point(*vit) ) );
                  vertex_map[ vit->id() ] = vertex_handles.back();
                }
              }

              hull_element_map[hit->id()] = viennagrid::make_element<HullCellElementType>( hull_segment, vertex_handles.begin(), vertex_handles.end() );
            }
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



    class algorithm : public base_algorithm
    {
    public:

      string name() const { return "ViennaGrid Extract Hull"; }

      template<typename MeshT, typename SegmentationT>
      bool generic_run()
      {
        typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;
        typedef typename viennagrid::result_of::facet_tag<MeshT>::type FacetTagType;

        typedef typename viennamesh::result_of::default_mesh<
                FacetTagType,
                viennagrid::result_of::geometric_dimension<MeshT>::value
            >::type FacetMeshType;
        typedef typename viennagrid::result_of::segmentation<FacetMeshType>::type FacetSegmentationType;
        typedef viennagrid::segmented_mesh<FacetMeshType, FacetSegmentationType> SegmentedFacetMeshType;

        typedef typename viennamesh::result_of::point< viennagrid::result_of::geometric_dimension<MeshT>::value >::type PointType;
        typedef typename viennamesh::result_of::seed_point_container<PointType>::type SeedPointContainerType;

        typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type input_mesh = get_input<SegmentedMeshType>("default");
        if (input_mesh)
        {
          output_parameter_proxy<SegmentedFacetMeshType> output_mesh = output_proxy<SegmentedFacetMeshType>( "default" );
          output_parameter_proxy<SeedPointContainerType> seed_points = output_proxy<SeedPointContainerType>( "seed_points" );

          extract_hull<FacetTagType>(input_mesh().mesh, input_mesh().segmentation,
                                                   output_mesh().mesh, output_mesh().segmentation);
          extract_seed_points( input_mesh().segmentation, seed_points() );

          return true;
        }

        return false;
      }

      bool run_impl()
      {
        if (generic_run<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation>())
          return true;
        if (generic_run<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation>())
          return true;

        if (generic_run<viennagrid::quadrilateral_2d_mesh, viennagrid::quadrilateral_2d_segmentation>())
          return true;
        if (generic_run<viennagrid::quadrilateral_3d_mesh, viennagrid::quadrilateral_3d_segmentation>())
          return true;

        if (generic_run<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>())
          return true;

        if (generic_run<viennagrid::hexahedral_3d_mesh, viennagrid::hexahedral_3d_segmentation>())
          return true;


        error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
        return false;
      }

    private:
    };

  }
}

#endif
