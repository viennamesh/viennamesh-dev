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

#include "extract_plc_geometry.hpp"

#include <set>
#include "viennagridpp/algorithm/extract_hole_points.hpp"
#include "viennagridpp/algorithm/plane_to_2d_projector.hpp"
#include "viennagridpp/algorithm/geometry.hpp"

namespace viennamesh
{

  template<typename MeshT>
  class same_segments_functor
  {
  public:
    same_segments_functor(MeshT mesh_) : mesh(mesh_) {}

    template<typename CellT1, typename CellT2>
    bool operator()(CellT1 const & lhs, CellT2 const & rhs) const
    {
      return viennagrid::equal_regions( viennagrid::regions(mesh, lhs), viennagrid::regions(mesh, rhs) );
    }

  private:
    MeshT mesh;
  };

  template<typename NumericConfigT>
  class same_orientation_functor
  {
  public:
    same_orientation_functor(NumericConfigT nc_) : nc(nc_) {}

    template<typename CellT1, typename CellT2>
    bool operator()(CellT1 const & lhs, CellT2 const & rhs) const
    {
      typedef typename viennagrid::result_of::point<CellT1>::type PointType;

      PointType lhs_normal = viennagrid::normal_vector(lhs);
      lhs_normal /= viennagrid::norm_2(lhs_normal);

      PointType rhs_normal = viennagrid::normal_vector(rhs);
      rhs_normal /= viennagrid::norm_2(rhs_normal);

      return std::abs(viennagrid::inner_prod(lhs_normal, rhs_normal)) >=
             1.0-viennagrid::detail::absolute_tolerance<double>(nc);
    }

  private:
    NumericConfigT nc;
  };


  template<typename Functor1T, typename Functor2T>
  class same_cell_combine_functor
  {
  public:
    same_cell_combine_functor(Functor1T const & f1_, Functor2T const & f2_) : f1(f1_), f2(f2_) {}

    template<typename CellT1, typename CellT2>
    bool operator()(CellT1 const & lhs, CellT2 const & rhs) const
    {
      return f1(lhs, rhs) && f2(lhs, rhs);
    }

  private:
    Functor1T f1;
    Functor2T f2;
  };





  template<typename MeshT, typename CellT, typename SamePLCCellFunctorT,
           typename VisitedAccessorT, typename PLCIDAccessorT>
  void recursively_add_neighbours( MeshT const & mesh,
                                   CellT const & cell,
                                   SamePLCCellFunctorT same_cell_functor,
                                   VisitedAccessorT & visited_accessor,
                                   PLCIDAccessorT & plc_id_accessor,
                                   int plc_id)
  {
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;

    if ( visited_accessor.get(cell) )
      return;

    visited_accessor.set(cell, true);
    plc_id_accessor.set(cell, plc_id);

    PointType normal_vector = viennagrid::normal_vector( cell );
    normal_vector /= viennagrid::norm_2(normal_vector);

    typedef typename viennagrid::result_of::const_neighbor_range<MeshT>::type NeighbourRangeType;
    typedef typename viennagrid::result_of::iterator<NeighbourRangeType>::type NeighbourRangeIterator;

    NeighbourRangeType neighbors(mesh, cell, 2, viennagrid::topologic_dimension(mesh));
    for (NeighbourRangeIterator it = neighbors.begin(); it != neighbors.end(); ++it)
    {
      if ( visited_accessor.get(*it) )
        continue;

      if ( same_cell_functor(cell, *it) )
        recursively_add_neighbours( mesh, *it, same_cell_functor, visited_accessor, plc_id_accessor, plc_id );
    }
  }




  template<bool mesh_is_const, typename SamePLCFunctorT>
  void extract_plcs(viennagrid::base_mesh<mesh_is_const> const & mesh,
                    viennagrid::mesh_t const & output_mesh,
                    SamePLCFunctorT same_plc_functor)
  {
    typedef viennagrid::base_mesh<mesh_is_const> InputMeshType;
    typedef viennagrid::mesh_t OutputMeshType;

    typedef typename viennagrid::result_of::element<InputMeshType>::type CellType;

    typedef typename viennagrid::result_of::const_cell_range<InputMeshType>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;
    typedef typename viennagrid::result_of::point<InputMeshType>::type PointType;

    typedef typename viennagrid::result_of::const_element_range<InputMeshType, 1>::type ConstLineRangeType;
    typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineIteratorType;
    typedef typename viennagrid::result_of::element_id<InputMeshType>::type LineIDType;
    typedef typename viennagrid::result_of::element<OutputMeshType>::type LineType;

    ConstCellRangeType cells(mesh);

    std::vector<bool> cell_visited_container( cells.size(), false );
    typename viennagrid::result_of::accessor< std::vector<bool>, CellType >::type cell_visited(cell_visited_container);

    std::vector<int> plc_id_container( cells.size(), -1 );
    typename viennagrid::result_of::accessor< std::vector<int>, CellType >::type plc_ids(plc_id_container);
    int lowest_plc_id = 0;

    for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
    {
      if ( cell_visited.get(*cit) )
        continue;

      recursively_add_neighbours( mesh, *cit, same_plc_functor, cell_visited, plc_ids, lowest_plc_id++ );
    }

    viennagrid::result_of::element_copy_map<>::type vertex_map(output_mesh);

    std::map<LineIDType, LineType> line_map;

    for (int i = 0; i < lowest_plc_id; ++i)
    {
      std::vector<point_t> hole_points_3d;
      {
        // extract PLC hole points
        typedef typename viennagrid::result_of::element_id<InputMeshType>::type VertexIDType;
        std::map<VertexIDType, int> vertex_to_point_index;
        std::vector<point_t> plc_points_3d;

        for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
        {
          if (plc_ids.get(*cit) == i)
          {
            typedef typename viennagrid::result_of::const_vertex_range<CellType>::type VertexOnCellRangeType;
            typedef typename viennagrid::result_of::iterator<VertexOnCellRangeType>::type VertexOnCellIteratorType;
            VertexOnCellRangeType vertices_on_cells(*cit);
            for (VertexOnCellIteratorType vcit = vertices_on_cells.begin(); vcit != vertices_on_cells.end(); ++vcit)
            {
              typename std::map<VertexIDType, int>::iterator it = vertex_to_point_index.find( (*vcit).id() );
              if (it == vertex_to_point_index.end())
              {
                plc_points_3d.push_back( viennagrid::get_point(*vcit) );
                vertex_to_point_index[(*vcit).id()] = plc_points_3d.size()-1;
              }
            }
          }
        }

        std::vector<point_t> plc_points_2d( plc_points_3d.size() );
        viennagrid::plane_to_2d_projector<PointType> projection_functor;
        projection_functor.init( plc_points_3d.begin(), plc_points_3d.end(), 1e-6 );
        projection_functor.project( plc_points_3d.begin(), plc_points_3d.end(), plc_points_2d.begin() );



        typedef viennagrid::mesh_t Triangular2DMeshType;
        typedef viennagrid::result_of::element<Triangular2DMeshType>::type Vertex2DType;

        Triangular2DMeshType mesh2d;
        std::vector<Vertex2DType> vertex_handles_2d(plc_points_2d.size());
        for (std::size_t j = 0; j < plc_points_2d.size(); ++j)
          vertex_handles_2d[j] = viennagrid::make_vertex(mesh2d, plc_points_2d[j]);

        for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
        {
          if (plc_ids.get(*cit) == i)
          {
            viennagrid::make_triangle(
              mesh2d,
              vertex_handles_2d[ vertex_to_point_index[viennagrid::vertices(*cit)[0].id()] ],
              vertex_handles_2d[ vertex_to_point_index[viennagrid::vertices(*cit)[1].id()] ],
              vertex_handles_2d[ vertex_to_point_index[viennagrid::vertices(*cit)[2].id()] ]
            );
          }
        }

        std::vector<point_t> hole_points_2d;
        viennagrid::extract_hole_points( mesh2d, hole_points_2d );


        projection_functor.unproject( hole_points_2d.begin(), hole_points_2d.end(), std::back_inserter(hole_points_3d) );
      }


      // extract PLC lines
      std::vector<LineType> plc_line_handles;

      ConstLineRangeType lines(mesh);
      for (ConstLineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
      {
        typedef typename viennagrid::result_of::const_coboundary_range<InputMeshType>::type ConstCoboundaryRangeType;
        typedef typename viennagrid::result_of::iterator<ConstCoboundaryRangeType>::type                                                 ConstCoboundaryRangeIterator;

        ConstCoboundaryRangeType triangles(mesh, *lit, 2);
        int triangle_in_plc_count = 0;
        for (ConstCoboundaryRangeIterator ctit = triangles.begin(); ctit != triangles.end(); ++ctit)
        {
          if (plc_ids.get(*ctit) == i)
            ++triangle_in_plc_count;
        }

        if (triangle_in_plc_count == 2 && triangles.size() == 2)
          continue;

        if (triangle_in_plc_count > 0)
        {
          typename std::map<LineIDType,LineType>::iterator lhit = line_map.find( (*lit).id() );
          if (lhit == line_map.end())
          {
            LineType tmp_lh = viennagrid::make_line(output_mesh,
                  vertex_map(viennagrid::vertices(*lit)[0]),
                  vertex_map(viennagrid::vertices(*lit)[1]));
            line_map[(*lit).id()] = tmp_lh;
            plc_line_handles.push_back(tmp_lh);
          }
          else
            plc_line_handles.push_back(lhit->second);
        }
      }

      typedef typename viennagrid::result_of::element<OutputMeshType>::type OutputCellType;
      OutputCellType plc = viennagrid::make_plc(output_mesh, plc_line_handles.begin(), plc_line_handles.end());

      for (std::vector<point_t>::const_iterator hpit = hole_points_3d.begin(); hpit != hole_points_3d.end(); ++hpit)
        viennagrid::add_hole_point( plc, *hpit );
    }
  }





  template<typename LineMeshT, typename VertexT, typename LineT, typename PointT, typename NewLineIDAccessorT, typename NumericConfigT>
  VertexT get_endpoint( LineMeshT const & line_mesh,
                              VertexT const & vertex, LineT const & line,
                              PointT direction,
                              NewLineIDAccessorT & new_line_accessor, int new_line_id,
                              NumericConfigT numeric_config )
  {
    typedef typename viennagrid::result_of::const_coboundary_range<LineMeshT>::type  ConstCoboundaryRangeType;

    ConstCoboundaryRangeType lines(line_mesh, vertex, 1);

    if (lines.size() != 2)
      return vertex;

    LineT other_line;
    if (lines[0] == line)
        other_line = lines[1];
    else
        other_line = lines[0];

    PointT other_direction = viennagrid::get_point( line_mesh, viennagrid::vertices(other_line)[0] ) -
                             viennagrid::get_point( line_mesh, viennagrid::vertices(other_line)[1] );
    other_direction /= viennagrid::norm_2(other_direction);

    if ( std::abs(viennagrid::inner_prod(direction, other_direction)) >=
         1.0-viennagrid::detail::absolute_tolerance<double>(numeric_config))
    {
      new_line_accessor.set(other_line, new_line_id);

      VertexT other_vertex_handle;
      if ( viennagrid::vertices(other_line)[0] == vertex )
          other_vertex_handle = viennagrid::vertices(other_line)[1];
      else
          other_vertex_handle = viennagrid::vertices(other_line)[0];

      return get_endpoint( line_mesh, other_vertex_handle, other_line, direction, new_line_accessor, new_line_id, numeric_config );
    }

    return vertex;
  }

  template<bool mesh_is_const, typename NumericConfigT>
  void coarsen_plc_mesh(viennagrid::base_mesh<mesh_is_const> const & mesh,
                        viennagrid::mesh_t const & output_mesh,
                        NumericConfigT numeric_config)
  {
    typedef viennagrid::base_mesh<mesh_is_const> InputMeshType;
    typedef viennagrid::mesh_t OutputMeshType;

    typedef typename viennagrid::result_of::point<InputMeshType>::type PointType;
    typedef typename viennagrid::result_of::element<InputMeshType>::type LineType;
    typedef typename viennagrid::result_of::const_element_range<InputMeshType, 1>::type ConstLineRangeType;
    typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineIteratorType;

    typedef typename viennagrid::result_of::const_element<InputMeshType>::type VertexType;

    viennagrid::result_of::element_copy_map<>::type copy_map(output_mesh);

    ConstLineRangeType lines(mesh);

    std::vector<LineType> new_lines;
    std::vector<int> line_to_new_line_index_container(lines.size(), -1);
    typename viennagrid::result_of::accessor< std::vector<int>, LineType >::type line_to_new_line_index(line_to_new_line_index_container);

    for (ConstLineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
    {
      int new_line_id = new_lines.size();

      line_to_new_line_index.set(*lit, new_line_id);

      PointType direction = viennagrid::get_point(viennagrid::vertices(*lit)[0]) -
                            viennagrid::get_point(viennagrid::vertices(*lit)[1]);
      direction /= viennagrid::norm_2(direction);

      typedef typename viennagrid::result_of::const_element<InputMeshType>::type ConstElementHandle;

      VertexType first = get_endpoint( mesh, viennagrid::vertices(*lit)[0], *lit, direction, line_to_new_line_index, new_line_id, numeric_config );
      VertexType second = get_endpoint( mesh, viennagrid::vertices(*lit)[1], *lit, direction, line_to_new_line_index, new_line_id, numeric_config );

      new_lines.push_back( viennagrid::make_line(output_mesh, copy_map(first), copy_map(second) ));
    }

    typedef typename viennagrid::result_of::element<InputMeshType>::type CellType;
    typedef typename viennagrid::result_of::const_cell_range<InputMeshType>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;
    ConstCellRangeType cells(mesh);
    for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
    {
      typedef typename viennagrid::result_of::const_element_range<CellType, 1>::type ConstLineOnCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstLineOnCellRangeType>::type ConstLineOnCellIteratorType;

      std::set<int> used_lines_indices;
      std::vector<LineType> new_plc_lines;

      ConstLineOnCellRangeType lines_on_cell(*cit);
      for (ConstLineOnCellIteratorType locit = lines_on_cell.begin(); locit != lines_on_cell.end(); ++locit)
      {
        if (used_lines_indices.insert( line_to_new_line_index.get(*locit) ).second)
          new_plc_lines.push_back( new_lines[line_to_new_line_index.get(*locit)] );
      }

      CellType cell= viennagrid::make_plc(output_mesh, new_plc_lines.begin(), new_plc_lines.end() );


      viennagrid::add_hole_points( cell, viennagrid::hole_points(*cit) );

//       std::vector<point_t> hole_points = viennagrid::hole_points( *cit );
//       for (std::vector<point_t>::const_iterator hpit
//       viennagrid::hole_points(viennagrid::dereference_handle(output_mesh, cell)) = viennagrid::hole_points(*cit);

      if (new_plc_lines.size() < 3)
      {
        warning(1) << "One PLC has less than three lines" << std::endl;
        warning(1) << "Old PLC:" << std::endl;
        warning(1) << *cit << std::endl;
        warning(1) << "New PLC:" << std::endl;
        warning(1) << cell << std::endl;
      }
    }
  }






  extract_plc_geometry::extract_plc_geometry() {}
  std::string extract_plc_geometry::name() { return "extract_plc_geometry"; }

  bool extract_plc_geometry::run(viennamesh::algorithm_handle &)
  {
    data_handle<double> coplanar_tolerance = get_input<double>("coplanar_tolerance");
    data_handle<double> colinear_tolerance = get_input<double>("colinear_tolerance");

    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");

    mesh_handle tmp = make_data<mesh_handle>();
    mesh_handle output_mesh = make_data<mesh_handle>();


    typedef same_segments_functor<viennagrid::const_mesh_t> Functor1Type;
    typedef same_orientation_functor<double> Functor2Type;

    Functor1Type f1(input_mesh());
    Functor2Type f2(coplanar_tolerance());
    same_cell_combine_functor< Functor1Type, Functor2Type > functor(f1, f2);


    extract_plcs(input_mesh(), tmp(), functor);
    coarsen_plc_mesh(tmp(), output_mesh(), colinear_tolerance());

    std::cout << "Extracted " << viennagrid::cells(output_mesh()).size() << " PLCs" << std::endl;
    return true;
  }

}






