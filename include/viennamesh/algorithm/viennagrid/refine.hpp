#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_REFINE_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_REFINE_HPP


#include "viennagrid/algorithm/refine.hpp"
#include "viennagrid/algorithm/boundary.hpp"

namespace viennamesh
{
  namespace edge_refine
  {


    template<typename ViennaGridMeshT, typename CellRefinementFunctorT>
    void generic_edge_refine( ViennaGridMeshT const & input_mesh, ViennaGridMeshT & output_mesh, CellRefinementFunctorT cell_refine )
    {
      typedef typename viennagrid::result_of::cell<ViennaGridMeshT>::type CellType;

      typedef typename viennagrid::result_of::edge<ViennaGridMeshT>::type EdgeType;
      typedef typename viennagrid::result_of::accessor_container<EdgeType, bool, viennagrid::std_map_tag>::type EdgeFlagContainerType;
      typedef typename viennagrid::result_of::field< EdgeFlagContainerType, EdgeType >::type EdgeFlagFieldType;

      EdgeFlagContainerType refine_flag_map;
      EdgeFlagFieldType refine_flag_field(refine_flag_map);

      typedef typename viennagrid::result_of::const_edge_range<ViennaGridMeshT>::type ConstEdgeRangeType;
      typedef typename viennagrid::result_of::iterator<ConstEdgeRangeType>::type ConstEdgeteratorType;

      ConstEdgeRangeType edges(input_mesh);
      for (ConstEdgeteratorType eit = edges.begin(); eit != edges.end(); ++eit)
      {
        if ( cell_refine(*eit) )
          refine_flag_field(*eit) = true;
      }

      viennagrid::refine<CellType>( input_mesh, output_mesh, refine_flag_field );
    }

    template<typename ViennaGridMeshT, typename ViennaGridSegmentationT, typename CellRefinementFunctorT>
    void generic_edge_refine( ViennaGridMeshT const & input_mesh, ViennaGridSegmentationT const & input_segmentation,
                              ViennaGridMeshT & output_mesh, ViennaGridSegmentationT & output_segmentation,
                              CellRefinementFunctorT cell_refine )
    {
      typedef typename viennagrid::result_of::cell<ViennaGridMeshT>::type CellType;

      typedef typename viennagrid::result_of::edge<ViennaGridMeshT>::type EdgeType;
      typedef typename viennagrid::result_of::accessor_container<EdgeType, bool, viennagrid::std_map_tag>::type EdgeFlagContainerType;
      typedef typename viennagrid::result_of::field< EdgeFlagContainerType, EdgeType >::type EdgeFlagFieldType;

      EdgeFlagContainerType refine_flag_map;
      EdgeFlagFieldType refine_flag_field(refine_flag_map);

      typedef typename viennagrid::result_of::const_edge_range<ViennaGridMeshT>::type ConstEdgeRangeType;
      typedef typename viennagrid::result_of::iterator<ConstEdgeRangeType>::type ConstEdgeteratorType;

      ConstEdgeRangeType edges(input_mesh);
      for (ConstEdgeteratorType eit = edges.begin(); eit != edges.end(); ++eit)
      {
        if ( cell_refine(*eit) )
          refine_flag_field(*eit) = true;
      }

      viennagrid::refine<CellType>( input_mesh, input_segmentation, output_mesh, output_segmentation, refine_flag_field );
    }




    struct uniform_refinement_functor
    {
      template<typename EdgeT>
      bool operator()( EdgeT const & ) { return true; }
    };

//     template<typename SegmentationT>
//     struct refine_if_in_different_segments
//     {
//       refine_if_in_different_segments(SegmentationT const & segmentation_) : segmentation(segmentation_) {}
//
//       template<typename EdgeT>
//       bool operator()( EdgeT const & edge )
//       {
//         typedef typename viennagrid::result_of::segment_id<SegmentationT>::type SegmentIDType;
//         typedef typename viennagrid::result_of::vertex<EdgeT>::type VertexType;
//         typedef typename viennagrid::result_of::segment_id_range<SegmentationT, VertexType>::type EdgeSegmentIDRangeType;
//         typedef typename viennagrid::result_of::segment_id_range<SegmentationT, VertexType>::type VertexSegmentIDRangeType;
//
//         EdgeSegmentIDRangeType segments_edge = viennagrid::segment_ids( segmentation, edge );
//         VertexSegmentIDRangeType segments_vtx0 = viennagrid::segment_ids( segmentation, viennagrid::vertices(edge)[0] );
//         VertexSegmentIDRangeType segments_vtx1 = viennagrid::segment_ids( segmentation, viennagrid::vertices(edge)[1] );
//
//
//         std::list<SegmentIDType> segment_ids_vtx0;
//         std::list<SegmentIDType> segment_ids_vtx1;
//
//         for (typename VertexSegmentIDRangeType::const_iterator vsit = segments_vtx0.begin(); vsit != segments_vtx0.end(); ++vsit)
//         {
//           typename EdgeSegmentIDRangeType::const_iterator seit = segments_edge.begin();
//           for (; seit != segments_edge.end(); ++seit)
//             if (*seit == *vsit)
//               break;
//
//           if (seit == segments_edge.end() && viennagrid::is_boundary(segmentation.get_segment(*vsit), viennagrid::vertices(edge)[0]) )
//             segment_ids_vtx0.push_back(*vsit);
//         }
//
//         for (typename VertexSegmentIDRangeType::const_iterator vsit = segments_vtx1.begin(); vsit != segments_vtx1.end(); ++vsit)
//         {
//           typename EdgeSegmentIDRangeType::const_iterator seit = segments_edge.begin();
//           for (; seit != segments_edge.end(); ++seit)
//             if (*seit == *vsit)
//               break;
//
//           if (seit == segments_edge.end() && viennagrid::is_boundary(segmentation.get_segment(*vsit), viennagrid::vertices(edge)[1]) )
//             segment_ids_vtx1.push_back(*vsit);
//         }
//
//         return !(segment_ids_vtx0.empty() && segment_ids_vtx1.empty());
//       }
//
//
//       SegmentationT const & segmentation;
//     };



    class algorithm : public base_algorithm
    {
    public:

      string name() const { return "ViennaGrid Cell Refinement"; }



//       template<typename MeshT, typename SegmentationT, typename CellRefinementFunctorT>
//       bool generic_run_impl( CellRefinementFunctorT functor )
//       {
//         typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;
//
//         typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type input_segmented_mesh = get_input<SegmentedMeshType>("default");
//         if (input_segmented_mesh)
//         {
//           output_parameter_proxy<SegmentedMeshType> output_mesh = output_proxy<SegmentedMeshType>( "default" );
//           generic_edge_refine( input_segmented_mesh().mesh, input_segmented_mesh().segmentation, output_mesh().mesh, output_mesh().segmentation, functor );
//           return true;
//         }
//         else
//         {
//           typename viennamesh::result_of::const_parameter_handle<MeshT>::type input_mesh = get_input<MeshT>("default");
//           if (input_mesh)
//           {
//             output_parameter_proxy<MeshT> output_mesh = output_proxy<MeshT>( "default" );
//             generic_edge_refine( input_mesh(), output_mesh(), functor );
//             return true;
//           }
//         }
//
//         return false;
//       }



      template<typename MeshT, typename SegmentationT>
      bool generic_run( string const & refine_strategy )
      {
        typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;

        typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type input_segmented_mesh = get_input<SegmentedMeshType>("default");
        if (input_segmented_mesh)
        {
          output_parameter_proxy<SegmentedMeshType> output_mesh = output_proxy<SegmentedMeshType>( "default" );

          if (refine_strategy == "uniform")
          {
            generic_edge_refine( input_segmented_mesh().mesh, input_segmented_mesh().segmentation,
                                 output_mesh().mesh, output_mesh().segmentation,
                                 uniform_refinement_functor() );
            return true;
          }
//           else if (refine_strategy == "refine_if_in_different_segments")
//           {
//             generic_edge_refine( input_segmented_mesh().mesh, input_segmented_mesh().segmentation,
//                                  output_mesh().mesh, output_mesh().segmentation,
//                                  refine_if_in_different_segments<SegmentationT>(input_segmented_mesh().segmentation) );
//             return true;
//           }

          return false;
        }
        else
        {
          typename viennamesh::result_of::const_parameter_handle<MeshT>::type input_mesh = get_input<MeshT>("default");
          if (input_mesh)
          {
            output_parameter_proxy<MeshT> output_mesh = output_proxy<MeshT>( "default" );


            if (refine_strategy == "uniform")
            {
              generic_edge_refine( input_mesh(), output_mesh(), uniform_refinement_functor() );
              return true;
            }

            return false;
          }
        }

        return false;
      }

      bool run_impl()
      {
        viennamesh::result_of::const_parameter_handle<string>::type refine_strategy = get_required_input<string>( "refine_strategy" );

//         if (generic_run<viennagrid::line_1d_mesh, viennagrid::line_1d_segmentation>( refine_strategy() ))
//           return true;
//         if (generic_run<viennagrid::line_2d_mesh, viennagrid::line_2d_segmentation>( refine_strategy() ))
//           return true;
//         if (generic_run<viennagrid::line_3d_mesh, viennagrid::line_3d_segmentation>( refine_strategy() ))
//           return true;

        if (generic_run<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation>( refine_strategy() ))
          return true;
        if (generic_run<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation>( refine_strategy() ))
          return true;

        if (generic_run<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>( refine_strategy() ))
          return true;

        error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
        return false;
      }

    private:
    };




  }
}


#endif
