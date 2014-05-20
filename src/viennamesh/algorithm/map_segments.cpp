/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include "viennamesh/algorithm/map_segments.hpp"

namespace viennamesh
{
  template<typename SrcMeshT, typename SrcSegmentationT,
            typename DstMeshT, typename DstSegmentationT,
            typename SegmentIDMapT>
  void map_segments_impl(SrcMeshT const & src_mesh, SrcSegmentationT const & src_segmentation,
                         DstMeshT & dst_mesh, DstSegmentationT & dst_segmentation,
                         SegmentIDMapT const & segment_id_map)
  {
    viennagrid::vertex_copy_map<SrcMeshT, DstMeshT> vertex_map(dst_mesh);

    typedef typename viennagrid::result_of::cell<SrcMeshT>::type CellType;

    typedef typename viennagrid::result_of::const_cell_range<SrcMeshT>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

    typedef typename viennagrid::result_of::segment_id_range<SrcSegmentationT, CellType>::type SrcSegmentIDRangeType;
    typedef typename viennagrid::result_of::segment_id<SrcSegmentationT>::type SrcSegmentIDType;
    typedef typename viennagrid::result_of::segment_id<DstSegmentationT>::type DstSegmentIDType;

    typedef typename viennagrid::result_of::cell_handle<DstMeshT>::type CellHandleType;

    std::map<SrcSegmentIDType, std::string> segment_id_name_map;
    std::map<std::string, DstSegmentIDType> segment_name_id_map;
    for (typename SrcSegmentationT::const_iterator sit = src_segmentation.begin(); sit != src_segmentation.end(); ++sit)
    {
      segment_id_name_map[ (*sit).id() ]   = (*sit).name();
      segment_name_id_map[ (*sit).name() ] = (*sit).id();
    }


    ConstCellRangeType cells(src_mesh);
    for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
    {
      CellHandleType cell_handle = vertex_map.copy_element(*cit );
      std::set<DstSegmentIDType> dst_segment_ids;

      SrcSegmentIDRangeType segment_ids = viennagrid::segment_ids( src_segmentation, *cit );
      for (typename SrcSegmentIDRangeType::const_iterator sit = segment_ids.begin(); sit != segment_ids.end(); ++sit)
      {
        typename SegmentIDMapT::const_iterator dst_segment_id_it = segment_id_map.find( segment_id_name_map[*sit] );
        if (dst_segment_id_it != segment_id_map.end())
          dst_segment_ids.insert( segment_name_id_map[dst_segment_id_it->second] );
        else
          dst_segment_ids.insert(*sit);
      }

      for (typename std::set<DstSegmentIDType>::const_iterator dst_segment_id_it = dst_segment_ids.begin(); dst_segment_id_it != dst_segment_ids.end(); ++dst_segment_id_it)
        viennagrid::add( dst_segmentation[*dst_segment_id_it], cell_handle );
    }
  }



  map_segments::map_segments() :
    input_mesh(*this, parameter_information("mesh","mesh","The input mesh, segmented triangular 2d mesh, segmented triangular 3d mesh, segmented quadrilateral 2d mesh, segmented quadrilateral 3d mesh, segmented tetrahedral 3d mesh and segmented hexahedral 3d mesh supported")),
    segment_mapping(*this, parameter_information("segment_mapping","std::map<std::string,std::string>","A mapping of segments, each entry maps a segment to another")),
    output_mesh(*this, parameter_information("mesh", "mesh", "The output mesh, same type of mesh as input mesh")) {}

  std::string map_segments::name() const { return "ViennaGrid Map Segments"; }
  std::string map_segments::id() const { return "map_segments"; }



  template<typename MeshT, typename SegmentationT>
  bool map_segments::generic_run( std::map<std::string, std::string> const & segment_mapping )
  {
    typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;

    typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type imp = input_mesh.get<SegmentedMeshType>();

    if (imp)
    {
      output_parameter_proxy<SegmentedMeshType> omp(output_mesh);
      map_segments_impl( imp().mesh, imp().segmentation, omp().mesh, omp().segmentation, segment_mapping );

      return true;
    }

    return false;
  }

  bool map_segments::run_impl()
  {
    if (generic_run<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation>(segment_mapping()))
      return true;
    if (generic_run<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation>(segment_mapping()))
      return true;

    if (generic_run<viennagrid::quadrilateral_2d_mesh, viennagrid::quadrilateral_2d_segmentation>(segment_mapping()))
      return true;
    if (generic_run<viennagrid::quadrilateral_3d_mesh, viennagrid::quadrilateral_3d_segmentation>(segment_mapping()))
      return true;

    if (generic_run<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>(segment_mapping()))
      return true;

    if (generic_run<viennagrid::hexahedral_3d_mesh, viennagrid::hexahedral_3d_segmentation>(segment_mapping()))
      return true;


    error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
    return false;
  }

}
