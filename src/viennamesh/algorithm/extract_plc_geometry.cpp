#include "viennamesh/algorithm/extract_plc_geometry.hpp"

#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/mesh/neighbor_iteration.hpp"

namespace viennamesh
{
  template<typename MeshT, typename SegmentationT, typename CellHandleT,
           typename PointT, typename SegmentIDRangeT,
           typename VisitedAccessorT, typename PLCIDAccessorT,
           typename NumericConfigT>
  void recursively_add_neighbours( MeshT const & mesh, SegmentationT const & segmentation,
                                   CellHandleT const & cell_handle, int plc_id,
                                   PointT const & normal_vector,
                                   SegmentIDRangeT const & segment_ids,
                                   VisitedAccessorT & visited_accessor,
                                   PLCIDAccessorT & plc_id_accessor,
                                   NumericConfigT numeric_config)
  {
    typedef typename viennagrid::detail::result_of::value_type<CellHandleT>::type     CellType;
    CellType const & cell = viennagrid::dereference_handle( mesh, cell_handle );

    if ( visited_accessor(cell) )
        return;

    PointT current_normal_vector = viennagrid::normal_vector( cell );
    current_normal_vector /= viennagrid::norm_2(current_normal_vector);

    if (std::abs(viennagrid::inner_prod(normal_vector, current_normal_vector)) >=
                  1.0-viennagrid::detail::absolute_tolerance<double>(numeric_config))
    {
      visited_accessor(cell) = true;
      plc_id_accessor(cell) = plc_id;

      typedef typename viennagrid::result_of::const_neighbor_range<MeshT, CellType, viennagrid::line_tag>::type NeighbourRangeType;
      typedef typename viennagrid::result_of::iterator<NeighbourRangeType>::type NeighbourRangeIterator;

      NeighbourRangeType neighgbours = viennagrid::neighbor_elements<CellType, viennagrid::line_tag>(mesh, cell_handle);
      for (NeighbourRangeIterator it = neighgbours.begin(); it != neighgbours.end(); ++it)
          recursively_add_neighbours( mesh, segmentation, it.handle(), plc_id, normal_vector, segment_ids, visited_accessor, plc_id_accessor, numeric_config );
    }
  }




  template<typename MeshT, typename SegmentationT, typename OutputMeshT>
  void extract_plcs(MeshT const & mesh, SegmentationT const & segmentation,
                    OutputMeshT & output_mesh)
  {
    typedef typename viennagrid::result_of::cell<MeshT>::type CellType;
    typedef typename viennagrid::result_of::cell_handle<OutputMeshT>::type OutputCellHandleType;

    typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;

    ConstCellRangeType cells(mesh);

    std::vector<bool> cell_visited_container( cells.size(), false );
    typename viennagrid::result_of::accessor< std::vector<bool>, CellType >::type cell_visited(cell_visited_container);

    std::vector<int> plc_id_container( cells.size(), -1 );
    typename viennagrid::result_of::accessor< std::vector<int>, CellType >::type plc_ids(plc_id_container);
    int lowest_plc_id = 0;

    for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
    {
      if ( cell_visited(*cit) )
        continue;

      cell_visited(*cit) = true;
      plc_ids(*cit) = lowest_plc_id;

      typedef typename viennagrid::result_of::const_neighbor_range<MeshT, CellType, viennagrid::line_tag>::type NeighbourRangeType;
      typedef typename viennagrid::result_of::iterator<NeighbourRangeType>::type NeighbourRangeIterator;

      PointType normal_vector = viennagrid::normal_vector(*cit);
      normal_vector /= viennagrid::norm_2(normal_vector);

      NeighbourRangeType neighgbours = viennagrid::neighbor_elements<CellType, viennagrid::line_tag>(mesh, cit.handle());
      for (NeighbourRangeIterator it = neighgbours.begin(); it != neighgbours.end(); ++it)
        recursively_add_neighbours( mesh, segmentation, it.handle(), lowest_plc_id, normal_vector, viennagrid::segment_ids(segmentation, *cit), cell_visited, plc_ids, 1e-6 );

      lowest_plc_id++;
    }

    typedef typename viennagrid::result_of::line<MeshT>::type LineType;
    typedef typename viennagrid::result_of::const_line_range<MeshT>::type ConstLineRangeType;
    typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineIteratorType;
    typedef typename viennagrid::result_of::line_id<MeshT>::type LineIDType;
    std::vector<bool> line_used_container( viennagrid::lines(mesh).size() );
    typename viennagrid::result_of::accessor< std::vector<bool>, LineType >::type line_used(line_used_container);

    std::vector<int> line_use_count_container( viennagrid::lines(mesh).size() );

    viennagrid::vertex_copy_map<MeshT, OutputMeshT> vertex_map(output_mesh);
    typedef typename viennagrid::result_of::line_handle<OutputMeshT>::type LineHandleType;
    typedef typename viennagrid::result_of::const_line_handle<MeshT>::type ConstLineHandleType;

    std::map<LineIDType,LineHandleType> line_map;

    std::cout << "Num PLCs " << lowest_plc_id << std::endl;
    for (int i = 0; i < lowest_plc_id; ++i)
    {
      std::vector<LineHandleType> plc_line_handles;

      ConstLineRangeType lines(mesh);
      for (ConstLineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
      {
        typedef typename viennagrid::result_of::const_coboundary_range<MeshT, viennagrid::line_tag, viennagrid::triangle_tag>::type ConstCoboundaryRangeType;
        typedef typename viennagrid::result_of::iterator<ConstCoboundaryRangeType>::type                                                 ConstCoboundaryRangeIterator;

        ConstCoboundaryRangeType triangles(mesh, lit.handle());
        int triangle_in_plc_count = 0;
        for (ConstCoboundaryRangeIterator ctit = triangles.begin(); ctit != triangles.end(); ++ctit)
        {
          if (plc_ids(*ctit) == i)
            ++triangle_in_plc_count;
        }

        if (triangle_in_plc_count == 2 && triangles.size() == 2)
          continue;

        if (triangle_in_plc_count > 0)
        {
          typename std::map<LineIDType,LineHandleType>::iterator lhit = line_map.find( (*lit).id() );
          if (lhit == line_map.end())
          {
            LineHandleType tmp_lh = viennagrid::make_line(output_mesh,
                  vertex_map(viennagrid::vertices(*lit)[0]),
                  vertex_map(viennagrid::vertices(*lit)[1]));
            line_map[(*lit).id()] = tmp_lh;
            plc_line_handles.push_back(tmp_lh);
          }
          else
            plc_line_handles.push_back(lhit->second);
        }
      }

      std::cout << "PLC " << i << " has " << plc_line_handles.size() << " lines" << std::endl;
      viennagrid::make_plc(output_mesh, plc_line_handles.begin(), plc_line_handles.end());
    }
  }





  template<typename LineMeshT, typename VertexHandleT, typename LineHandleT, typename PointT, typename NewLineIDAccessorT, typename NumericConfigT>
  VertexHandleT get_endpoint( LineMeshT const & line_mesh,
                              VertexHandleT const & vertex_handle, LineHandleT const & line_handle,
                              PointT direction,
                              NewLineIDAccessorT & new_line_accessor, int new_line_id,
                              NumericConfigT numeric_config )
  {
    typedef typename viennagrid::result_of::const_coboundary_range<LineMeshT, viennagrid::vertex_tag, viennagrid::line_tag>::type  ConstCoboundaryRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCoboundaryRangeType>::type                                                 ConstCoboundaryRangeIterator;
    typedef typename viennagrid::detail::result_of::value_type<LineHandleT>::type                                      LineType;

    ConstCoboundaryRangeType lines = viennagrid::coboundary_elements<viennagrid::vertex_tag, viennagrid::line_tag>(line_mesh, vertex_handle);

    if (lines.size() != 2)
      return vertex_handle;

    LineHandleT other_line_handle;
    if (lines.handle_at(0) == line_handle)
        other_line_handle = lines.handle_at(1);
    else
        other_line_handle = lines.handle_at(0);

    LineType const & other_line = viennagrid::dereference_handle( line_mesh, other_line_handle );

    PointT other_direction = viennagrid::point( line_mesh, viennagrid::vertices(other_line)[0] ) -
                             viennagrid::point( line_mesh, viennagrid::vertices(other_line)[1] );
    other_direction /= viennagrid::norm_2(other_direction);

    if ( std::abs(viennagrid::inner_prod(direction, other_direction)) >=
         1.0-viennagrid::detail::absolute_tolerance<double>(numeric_config))
    {
      new_line_accessor( viennagrid::dereference_handle(line_mesh, other_line_handle) ) = new_line_id;

      VertexHandleT other_vertex_handle;
      if ( viennagrid::vertices(other_line).handle_at(0) == vertex_handle )
          other_vertex_handle = viennagrid::vertices(other_line).handle_at(1);
      else
          other_vertex_handle = viennagrid::vertices(other_line).handle_at(0);

      return get_endpoint( line_mesh, other_vertex_handle, other_line_handle, direction, new_line_accessor, new_line_id, numeric_config );
    }

    return vertex_handle;
  }

  template<typename MeshT, typename OutputMeshT>
  void coarsen_plc_mesh(MeshT const & mesh, OutputMeshT & output_mesh)
  {
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::line<MeshT>::type LineType;
    typedef typename viennagrid::result_of::const_line_range<MeshT>::type ConstLineRangeType;
    typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineIteratorType;

    typedef typename viennagrid::result_of::line_handle<MeshT>::type LineHandleType;

    viennagrid::vertex_copy_map<MeshT, OutputMeshT> vertex_map(output_mesh);

    ConstLineRangeType lines(mesh);

    std::vector<LineHandleType> new_line_handles;
    std::vector<int> line_to_new_line_index_container(lines.size(), -1);
    typename viennagrid::result_of::accessor< std::vector<int>, LineType >::type line_to_new_line_index(line_to_new_line_index_container);

    for (ConstLineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
    {
      int new_line_id = new_line_handles.size();

      line_to_new_line_index(*lit) = new_line_id;

      PointType direction = viennagrid::point(viennagrid::vertices(*lit)[0]) -
                            viennagrid::point(viennagrid::vertices(*lit)[1]);
      direction /= viennagrid::norm_2(direction);

      typedef typename viennagrid::result_of::const_vertex_handle<MeshT>::type ConstVertexHandle;

      ConstVertexHandle first = get_endpoint( mesh, viennagrid::vertices(*lit).handle_at(0), lit.handle(), direction, line_to_new_line_index, new_line_id, 1e-6 );
      ConstVertexHandle second = get_endpoint( mesh, viennagrid::vertices(*lit).handle_at(1), lit.handle(), direction, line_to_new_line_index, new_line_id, 1e-6 );

      new_line_handles.push_back( viennagrid::make_line(output_mesh,
                                                        vertex_map(viennagrid::dereference_handle(mesh, first)), vertex_map(viennagrid::dereference_handle(mesh, second)) ));
    }

    typedef typename viennagrid::result_of::cell<MeshT>::type CellType;
    typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;
    ConstCellRangeType cells(mesh);
    for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
    {
      typedef typename viennagrid::result_of::const_line_range<CellType>::type ConstLineOnCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstLineOnCellRangeType>::type ConstLineOnCellIteratorType;

      std::set<int> used_lines_indices;
      std::vector<LineHandleType> new_plc_line_handles;

      ConstLineOnCellRangeType lines_on_cell(*cit);
      for (ConstLineOnCellIteratorType locit = lines_on_cell.begin(); locit != lines_on_cell.end(); ++locit)
      {
        if (used_lines_indices.insert( line_to_new_line_index(*locit) ).second)
          new_plc_line_handles.push_back( new_line_handles[line_to_new_line_index(*locit)] );
      }


      viennagrid::make_plc(output_mesh, new_plc_line_handles.begin(), new_plc_line_handles.end() );


//       for (std::set<int>::iterator iit = used_lines_indices.begin(); iit != used_lines_indices.end(); ++iit)

    }
  }


  bool extract_plc::run_impl()
  {
    typedef viennagrid::segmented_mesh<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> InputMeshType;
    typedef viennagrid::plc_3d_mesh OutputMeshType;

    viennamesh::result_of::const_parameter_handle<InputMeshType>::type input_mesh = get_input<InputMeshType>("default");

    if (input_mesh)
    {
//       std::cout << "DID IT" << std::endl;

      output_parameter_proxy<OutputMeshType> output_mesh = output_proxy<OutputMeshType>( "default" );

      OutputMeshType tmp;
      extract_plcs(input_mesh().mesh, input_mesh().segmentation, tmp);
      coarsen_plc_mesh(tmp, output_mesh());

      return true;
    }

    error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
    return false;
  }

}
