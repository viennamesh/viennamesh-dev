#include "viennamesh/algorithm/merge_meshes.hpp"

#include "viennagrid/algorithm/geometry.hpp"

namespace viennamesh
{
  template<typename SrcMeshT, typename SrcSegmentationT,
            typename DstMeshT, typename DstSegmentationT>
  void merge_meshes_impl( SrcMeshT const & src_mesh, SrcSegmentationT const & src_segmentation,
                      DstMeshT & dst_mesh, DstSegmentationT & dst_segmentation )
  {
    viennagrid::vertex_copy_map<SrcMeshT, DstMeshT> vertex_map(dst_mesh);

    typedef typename viennagrid::result_of::cell<SrcMeshT>::type CellType;
    typedef typename viennagrid::result_of::coord<SrcMeshT>::type NumericType;

    typedef typename viennagrid::result_of::const_cell_range<SrcMeshT>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

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



  merge_meshes::merge_meshes() :
    input_mesh0(*this, parameter_information("mesh0","mesh","The input mesh, segmented triangular 2d mesh, segmented triangular 3d mesh, segmented quadrilateral 2d mesh, segmented quadrilateral 3d mesh, segmented tetrahedral 3d mesh and segmented hexahedral 3d mesh supported")),
    input_mesh1(*this, parameter_information("mesh1","mesh","The input mesh, segmented triangular 2d mesh, segmented triangular 3d mesh, segmented quadrilateral 2d mesh, segmented quadrilateral 3d mesh, segmented tetrahedral 3d mesh and segmented hexahedral 3d mesh supported")),
    output_mesh(*this, parameter_information("mesh", "mesh", "The output mesh, same type of mesh as input meshes")) {}

  std::string merge_meshes::name() const { return "ViennaGrid Merge Meshes"; }
  std::string merge_meshes::id() const { return "merge_meshes"; }

  template<typename MeshT, typename SegmentationT>
  bool merge_meshes::generic_run()
  {
    typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;

    typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type imp0 = input_mesh0.get<SegmentedMeshType>();
    typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type imp1 = input_mesh1.get<SegmentedMeshType>();

    if (imp0 && imp1)
    {
      output_parameter_proxy<SegmentedMeshType> omp(output_mesh);

      merge_meshes_impl( imp0().mesh, imp0().segmentation, omp().mesh, omp().segmentation );
      merge_meshes_impl( imp1().mesh, imp1().segmentation, omp().mesh, omp().segmentation );

      return true;
    }

    return false;
  }

  bool merge_meshes::run_impl()
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

}
