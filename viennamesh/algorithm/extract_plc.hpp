#ifndef VIENNAMESH_ALGORITHM_EXTRACT_PLC_HPP
#define VIENNAMESH_ALGORITHM_EXTRACT_PLC_HPP

#include "viennagrid/mesh/segmentation.hpp"

#include "viennamesh/utils/utils.hpp"



namespace viennamesh
{

  template<typename HullMeshT, typename ElementHandleT, typename PointT, typename HullViewT, typename VisitedAccessorT>
  void recursively_add_neighbours( HullMeshT & hull_mesh, ElementHandleT const & element_handle, PointT const & normal_vector, VisitedAccessorT visited_accessor, HullViewT & view )
  {
      typedef typename viennagrid::detail::result_of::value_type<ElementHandleT>::type     ElementType;
      ElementType & element = viennagrid::dereference_handle( hull_mesh, element_handle );

      if ( visited_accessor(element) )
          return;


      PointT current_normal_vector = viennagrid::normal_vector( element );
      current_normal_vector /= viennagrid::norm_2(current_normal_vector);

      if ( std::abs(viennagrid::inner_prod(normal_vector, current_normal_vector)) >= 1.0-1e-6)
      {
          visited_accessor(element) = true;
          viennagrid::add_single_handle( view, element_handle );
          viennagrid::add_single_handle( view, viennagrid::lines(element).handle_at(0) );
          viennagrid::add_single_handle( view, viennagrid::lines(element).handle_at(1) );
          viennagrid::add_single_handle( view, viennagrid::lines(element).handle_at(2) );

          typedef typename viennagrid::result_of::neighbor_range<HullMeshT, ElementType, viennagrid::line_tag>::type NeighbourRangeType;
          typedef typename viennagrid::result_of::iterator<NeighbourRangeType>::type NeighbourRangeIterator;

          NeighbourRangeType neighgbours = viennagrid::neighbor_elements< ElementType, viennagrid::line_tag >( hull_mesh, element_handle );
          for (NeighbourRangeIterator it = neighgbours.begin(); it != neighgbours.end(); ++it)
              recursively_add_neighbours( hull_mesh, it.handle(), normal_vector, visited_accessor, view );
      }
  }




  template<typename LineMeshT, typename VertexHandleT, typename LineHandleT, typename PointT, typename VisitedAccessorT>
  VertexHandleT get_endpoint( LineMeshT & line_mesh, VertexHandleT const & vertex_handle, LineHandleT const & line_handle, PointT direction, VisitedAccessorT visited_accessor )
  {
      typedef typename viennagrid::result_of::coboundary_range<LineMeshT, viennagrid::vertex_tag, viennagrid::line_tag>::type   CoboundaryRangeType;
      typedef typename viennagrid::result_of::iterator<CoboundaryRangeType>::type                                                 CoboundaryRangeIterator;
      typedef typename viennagrid::detail::result_of::value_type<LineHandleT>::type                                      LineType;

      CoboundaryRangeType lines = viennagrid::coboundary_elements<viennagrid::vertex_tag, viennagrid::line_tag>(line_mesh, vertex_handle);
      if (lines.size() != 2)
      {
          return vertex_handle;
      }


      LineType & line = viennagrid::dereference_handle( line_mesh, line_handle );

      LineHandleT other_line_handle;
      if (lines.handle_at(0) == line_handle)
          other_line_handle = lines.handle_at(1);
      else
          other_line_handle = lines.handle_at(0);


      LineType & other_line = viennagrid::dereference_handle( line_mesh, other_line_handle );

      PointT other_direction = viennagrid::point( line_mesh, viennagrid::vertices(other_line)[0] ) - viennagrid::point( line_mesh, viennagrid::vertices(other_line)[1] );
      other_direction /= viennagrid::norm_2(other_direction);

      if ( std::abs(viennagrid::inner_prod(direction, other_direction)) >= 1.0-1e-6)
      {
          visited_accessor( viennagrid::dereference_handle(line_mesh, other_line_handle) ) = true;

          VertexHandleT other_vertex_handle;
          if ( viennagrid::vertices(other_line).handle_at(0) == vertex_handle )
              other_vertex_handle = viennagrid::vertices(other_line).handle_at(1);
          else
              other_vertex_handle = viennagrid::vertices(other_line).handle_at(0);

          return get_endpoint( line_mesh, other_vertex_handle, other_line_handle, direction, visited_accessor );
      }

      return vertex_handle;
  }




  template<typename HullMeshT, typename HullSegmentationT, typename PLCMeshT>
  void extract_plcs( HullMeshT & hull_mesh, HullSegmentationT & hull_segmentation, PLCMeshT & plc_mesh )
  {
//       typedef typename hull_segment_container_type::value_type hull_segment_type;

      typedef typename HullSegmentationT::segment_id_type                               SegmentIDType;
      typedef typename viennagrid::result_of::segment_handle<HullSegmentationT>::type          HullSegmentHandleType;
      typedef typename viennagrid::result_of::point<HullMeshT>::type                  HullPointType;
      typedef typename viennagrid::result_of::cell<HullMeshT>::type                   HullCellType;
      typedef typename viennagrid::result_of::handle<HullMeshT, HullCellType>::type   HullCellHandleType;

      typedef typename viennagrid::result_of::cell_range<HullMeshT>::type     CellRangeType;
      typedef typename viennagrid::result_of::iterator<CellRangeType>::type     CellRangeItertor;

      CellRangeType cells = viennagrid::elements( hull_mesh );

      std::deque<bool> visited( cells.size(), false );
      typename viennagrid::result_of::accessor< std::deque<bool>, HullCellType >::type visited_accessor( visited );

      typedef typename viennagrid::result_of::mesh_view<HullMeshT>::type                                  HullViewType;
      typedef typename viennagrid::result_of::vertex_handle<HullViewType>::type                               HullViewVertexHandleType;
      typedef typename viennagrid::result_of::element_range<HullViewType, viennagrid::line_tag>::type         ViewLineRangeType;
      typedef typename viennagrid::result_of::iterator<ViewLineRangeType>::type                               ViewLineRangeIterator;

      std::deque<HullViewType> plane_lines;

      HullViewType all_lines = viennagrid::make_view( hull_mesh );

      for (CellRangeItertor cit = cells.begin(); cit != cells.end(); ++cit)
      {
          HullCellType const & cell = *cit;

          if (visited_accessor(cell))
              continue;

          HullPointType normal_vector = viennagrid::normal_vector( cell );
          normal_vector /= viennagrid::norm_2(normal_vector);


          HullViewType current_plane = viennagrid::make_view( hull_mesh );
          std::map<SegmentIDType, HullViewType> current_plane_view;
          for (typename HullSegmentationT::const_iterator sit = hull_segmentation.begin(); sit != hull_segmentation.end(); ++sit)
              current_plane_view[ sit->id() ] = viennagrid::make_view( hull_mesh );

          plane_lines.resize( plane_lines.size()+1 );
          HullViewType & current_plane_lines = plane_lines.back();
          current_plane_lines = viennagrid::make_view( hull_mesh );


          recursively_add_neighbours( hull_mesh, cit.handle(), normal_vector, visited_accessor, current_plane );

          typedef typename viennagrid::result_of::cell_range<HullSegmentHandleType>::type     ViewCellRangeType;
          typedef typename viennagrid::result_of::iterator<ViewCellRangeType>::type     ViewCellRangeIterator;

          ViewCellRangeType plane_elements = viennagrid::elements( current_plane );
          for (ViewCellRangeIterator it = plane_elements.begin(); it != plane_elements.end(); ++it)
          {
              for (typename std::map<SegmentIDType, HullViewType>::iterator pit = current_plane_view.begin(); pit != current_plane_view.end(); ++pit)
              {
                HullSegmentHandleType & current_segment = hull_segmentation( pit->first );
                ViewCellRangeIterator jt = viennagrid::find_by_handle( current_segment, it.handle() );
                  if ( jt != viennagrid::cells(current_segment).end() )
                      viennagrid::add_single_handle( current_plane_view[ current_segment.id() ], it.handle() );
              }

//               for (int i = 0; i < current_plane_view.size(); ++i)
//               {
//                   hull_segment_type & current_segment = hull_segments[i];
//                   view_cell_range_iterator jt = viennagrid::find_by_handle(current_segment, it.handle());
//                   if ( jt != viennagrid::cells(current_segment).end() )
//                       viennagrid::add_handle( current_plane_view[i], hull_mesh, it.handle() );
//               }


          }


//           for (int i = 0; i < current_plane_view.size(); ++i)
//           {
//               viennadata::access<viennagrid::boundary_key<hull_view_type>, bool>(viennagrid::boundary_key<hull_view_type>(current_plane_view[i]))(current_plane_view[i]) = false;
//               viennadata::erase<viennadata::all, viennadata::all>()(current_plane_view[i]);
//               for (view_line_range_iterator it = lines.begin(); it != lines.end(); ++it)
//                   viennadata::erase<viennadata::all, viennadata::all>()(*it);
//           }

          ViewLineRangeType lines = viennagrid::elements( current_plane );
          for (ViewLineRangeIterator lit = lines.begin(); lit != lines.end(); ++lit)
//           for (typename ViewLineRangeType::iterator lit = lines.begin(); lit != lines.end(); ++lit)
          {

              for (typename std::map<SegmentIDType, HullViewType>::iterator pit = current_plane_view.begin(); pit != current_plane_view.end(); ++pit)
//               for (int i = 0; i < current_plane_view.size(); ++i)
              {
                  HullViewType & current_view = pit->second; //current_plane_view[i];

                  if (viennagrid::is_boundary( current_view, *lit ))
                  {
                      viennagrid::add_single_handle( all_lines, lit.handle() );
                      viennagrid::add_single_handle( current_plane_lines, lit.handle() );
                      break;
                  }
              }
          }
      }



      typedef typename viennagrid::result_of::line_handle<PLCMeshT>::type         PLCLineHandleType;
      typedef typename viennagrid::result_of::vertex_handle<PLCMeshT>::type       PLCVertexHandleType;
      typedef typename viennagrid::result_of::point<PLCMeshT>::type               PLCPointType;

      typedef typename viennagrid::result_of::line<HullViewType>::type              HullLineType;


      for (std::size_t i = 0; i != plane_lines.size(); ++i)
      {
          HullViewType & current_plane_lines_view = plane_lines[i];

          ViewLineRangeType current_plane_lines = viennagrid::elements(current_plane_lines_view);

          std::deque<bool> lines_visited( current_plane_lines.size(), false );
          typename viennagrid::result_of::accessor< std::deque<bool>, HullLineType >::type visited_accessor( lines_visited );

          std::deque<PLCLineHandleType> plc_lines;

          for (ViewLineRangeIterator lit = current_plane_lines.begin(); lit != current_plane_lines.end(); ++lit)
          {
//               if (viennagrid::look_up(lines_visited, *it) == true)
              if (visited_accessor(*lit))
                  continue;


              HullPointType direction = viennagrid::point( all_lines, viennagrid::vertices(*lit)[0] ) - viennagrid::point( all_lines, viennagrid::vertices(*lit)[1] );
              direction /= viennagrid::norm_2(direction);

              HullViewVertexHandleType first = get_endpoint( all_lines, viennagrid::vertices(*lit).handle_at(0), lit.handle(), direction, visited_accessor );
              HullViewVertexHandleType second = get_endpoint( all_lines, viennagrid::vertices(*lit).handle_at(1), lit.handle(), direction, visited_accessor );

              visited_accessor(*lit) = true;
//               viennagrid::look_up( lines_visited, *it ) = true;

              plc_lines.push_back(viennagrid::make_line(plc_mesh,
                                          viennagrid::make_unique_vertex(plc_mesh, viennagrid::point( all_lines, first )),
                                          viennagrid::make_unique_vertex(plc_mesh, viennagrid::point( all_lines, second ))));
          }

          PLCVertexHandleType tmp_vtx_handle;
          PLCPointType tmp_pt;
          viennagrid::make_plc(plc_mesh, plc_lines.begin(), plc_lines.end(), &tmp_vtx_handle, &tmp_vtx_handle, &tmp_pt, &tmp_pt);
      }
  }

}

#endif
