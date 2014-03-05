#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_MERGE_MESHES_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_MERGE_MESHES_HPP

#include "viennamesh/core/algorithm.hpp"

#include "viennagrid/algorithm/geometry.hpp"


namespace viennamesh
{

  namespace merge_meshes
  {
    template<typename SrcMeshT, typename SrcSegmentationT,
             typename DstMeshT, typename DstSegmentationT>
    void merge_meshes( SrcMeshT const & src_mesh, SrcSegmentationT const & src_segmentation,
                       DstMeshT & dst_mesh, DstSegmentationT & dst_segmentation )
    {
      viennagrid::vertex_copy_map<SrcMeshT, DstMeshT> vertex_map(dst_mesh);

      typedef typename viennagrid::result_of::cell<SrcMeshT>::type CellType;
      typedef typename viennagrid::result_of::coord<SrcMeshT>::type NumericType;

      typedef typename viennagrid::result_of::const_cell_range<SrcMeshT>::type ConstCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

      typedef typename viennagrid::result_of::segment_id<SrcSegmentationT>::type SrcSegmentIDType;
      typedef typename viennagrid::result_of::segment_id_range<SrcSegmentationT, CellType>::type SrcSegmentIDRangeType;
      typedef typename viennagrid::result_of::segment_id<DstSegmentationT>::type DstSegmentIDType;

      typedef typename viennagrid::result_of::cell_handle<DstMeshT>::type CellHandleType;

      DstSegmentIDType segment_id_offset = dst_segmentation.size();
      NumericType mesh_size = viennagrid::mesh_size( src_mesh );

      ConstCellRangeType cells(src_mesh);
      for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
      {
        CellHandleType cell_handle = vertex_map.copy_element(*cit, mesh_size / 1e6 );

        SrcSegmentIDRangeType segment_ids = viennagrid::segment_ids( src_segmentation, *cit );
        for (typename SrcSegmentIDRangeType::const_iterator sit = segment_ids.begin(); sit != segment_ids.end(); ++sit)
        {
          viennagrid::add( dst_segmentation[*sit+segment_id_offset], cell_handle );
        }
      }
    }



    class algorithm : public base_algorithm
    {
    public:

      string name() const { return "ViennaGrid Merge Meshes"; }

      template<typename MeshT, typename SegmentationT>
      bool generic_run()
      {
        typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;

        typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type input_mesh0 = get_input<SegmentedMeshType>("input0_mesh");
        typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type input_mesh1 = get_input<SegmentedMeshType>("input1_mesh");

        if (input_mesh0 && input_mesh1)
        {
          output_parameter_proxy<SegmentedMeshType> output_mesh = output_proxy<SegmentedMeshType>( "default" );

          merge_meshes( input_mesh0().mesh, input_mesh0().segmentation, output_mesh().mesh, output_mesh().segmentation );
          merge_meshes( input_mesh1().mesh, input_mesh1().segmentation, output_mesh().mesh, output_mesh().segmentation );

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
