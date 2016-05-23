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
#include "viennagrid/algorithm/extract_hole_points.hpp"
#include "viennagrid/algorithm/plane_to_2d_projector.hpp"
#include "viennagrid/algorithm/geometry.hpp"

namespace viennamesh
{

  class same_segments_functor
  {
  public:
    same_segments_functor() {}

    template<typename CellT1, typename CellT2>
    bool operator()(CellT1 const & lhs, CellT2 const & rhs) const
    {
      return viennagrid::equal_regions( viennagrid::regions(lhs), viennagrid::regions(rhs) );
    }
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

    NeighbourRangeType neighbors(mesh, cell, 1, viennagrid::topologic_dimension(mesh));
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
                    viennagrid_plc plc,
                    SamePLCFunctorT same_plc_functor)
  {
    viennagrid_plc_geometric_dimension_set(plc, 3);

    typedef viennagrid::base_mesh<mesh_is_const> MeshType;

    typedef typename viennagrid::result_of::element<MeshType>::type ElementType;

    typedef typename viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;
    typedef typename viennagrid::result_of::point<MeshType>::type PointType;

    typedef typename viennagrid::result_of::const_element_range<MeshType, 1>::type ConstLineRangeType;
    typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineIteratorType;

    ConstCellRangeType cells(mesh);

    std::vector<bool> cell_visited_container( cells.size(), false );
    typename viennagrid::result_of::accessor< std::vector<bool>, ElementType >::type cell_visited(cell_visited_container);

    std::vector<int> plc_id_container( cells.size(), -1 );
    typename viennagrid::result_of::accessor< std::vector<int>, ElementType >::type plc_ids(plc_id_container);
    int lowest_plc_id = 0;

    for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
    {
      if ( cell_visited.get(*cit) )
        continue;

      recursively_add_neighbours( mesh, *cit, same_plc_functor, cell_visited, plc_ids, lowest_plc_id++ );
    }

    std::map<ElementType, viennagrid_int> vertex_map;

    for (int i = 0; i < lowest_plc_id; ++i)
    {
      std::vector<point> hole_points_3d;
      {
        // extract PLC hole points
        typedef typename viennagrid::result_of::element_id<MeshType>::type VertexIDType;
        std::map<VertexIDType, int> vertex_to_point_index;
        std::vector<point> plc_points_3d;

        for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
        {
          if (plc_ids.get(*cit) == i)
          {
            typedef typename viennagrid::result_of::const_vertex_range<ElementType>::type VertexOnCellRangeType;
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

        std::vector<point> plc_points_2d( plc_points_3d.size() );
        viennagrid::plane_to_2d_projector<PointType> projection_functor;
        projection_functor.init( plc_points_3d.begin(), plc_points_3d.end(), 1e-6 );
        projection_functor.project( plc_points_3d.begin(), plc_points_3d.end(), plc_points_2d.begin() );



        typedef viennagrid::mesh Triangular2DMeshType;
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

        std::vector<point> hole_points_2d;
        viennagrid::extract_hole_points( mesh2d, hole_points_2d );



        projection_functor.unproject( hole_points_2d.begin(), hole_points_2d.end(), std::back_inserter(hole_points_3d) );
      }


      // extract PLC lines
      std::map<ElementType, viennagrid_int> plc_vertices;
      std::set< std::pair<ElementType, ElementType> > plc_lines;

      ConstLineRangeType lines(mesh);
      for (ConstLineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
      {
        typedef typename viennagrid::result_of::const_coboundary_range<MeshType>::type ConstCoboundaryRangeType;
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
          ElementType v0 = viennagrid::vertices(*lit)[0];
          ElementType v1 = viennagrid::vertices(*lit)[1];

          plc_vertices.insert( std::make_pair(v0, -1) );
          plc_vertices.insert( std::make_pair(v1, -1) );


          if (v1 < v0)
            std::swap(v0, v1);

          plc_lines.insert( std::make_pair(v0, v1) );
        }
      }

      for (typename std::map<ElementType, viennagrid_int>::iterator vit = plc_vertices.begin(); vit != plc_vertices.end(); ++vit)
      {
        typename std::map<ElementType, viennagrid_int>::iterator gvit = vertex_map.find( (*vit).first );
        if (gvit == vertex_map.end())
        {
          PointType p = viennagrid::get_point( vit->first );
          viennagrid_plc_vertex_create(plc, &p[0], &vit->second);
          vertex_map.insert( *vit );
        }
        else
          vit->second = gvit->second;
      }

      std::vector<viennagrid_int> line_ids;
      for (typename std::set< std::pair<ElementType, ElementType> >::iterator it = plc_lines.begin(); it != plc_lines.end(); ++it)
      {
        viennagrid_int line_id;



        viennagrid_plc_line_create(plc,
                                   plc_vertices[(*it).first],
                                   plc_vertices[(*it).second],
                                   &line_id);
        line_ids.push_back(line_id);
      }


      viennagrid_int facet_id;
      viennagrid_plc_facet_create(plc, line_ids.size(), &line_ids[0], &facet_id);

      for (std::vector<point>::const_iterator hpit = hole_points_3d.begin(); hpit != hole_points_3d.end(); ++hpit)
      {
        viennagrid_plc_facet_hole_point_add(plc, facet_id, &(*hpit)[0]);
      }
    }
  }






  template<typename CoboundaryRange, typename PointT, typename NewLineIDContainerT, typename NumericConfigT>
  viennagrid_int get_endpoint(viennagrid_plc plc,
                              CoboundaryRange const & coboundary_lines,
                              viennagrid_int vertex_id, viennagrid_int line_id,
                              PointT direction,
                              NewLineIDContainerT & line_to_new_line_index, viennagrid_int new_line_id,
                              NumericConfigT numeric_config )
  {
    if (coboundary_lines[vertex_id].size() != 2)
      return vertex_id;

    viennagrid_int other_line;
    if (coboundary_lines[vertex_id][0] == line_id)
        other_line = coboundary_lines[vertex_id][1];
    else
        other_line = coboundary_lines[vertex_id][0];

    viennagrid_int * other_line_vertex_begin;
    viennagrid_int * other_line_vertex_end;
    viennagrid_plc_boundary_elements(plc, other_line, 0, &other_line_vertex_begin, &other_line_vertex_end);

    PointT other_direction = viennagrid::get_point( plc, *(other_line_vertex_begin+0) ) -
                             viennagrid::get_point( plc, *(other_line_vertex_begin+1) );
    other_direction /= viennagrid::norm_2(other_direction);

    if ( std::abs(viennagrid::inner_prod(direction, other_direction)) >=
         1.0-viennagrid::detail::absolute_tolerance<double>(numeric_config))
    {
      line_to_new_line_index[ viennagrid_index_from_element_id(other_line) ] = new_line_id;

      viennagrid_int other_vertex;
      if ( *(other_line_vertex_begin+0) == vertex_id )
          other_vertex = *(other_line_vertex_begin+1);
      else
          other_vertex = *(other_line_vertex_begin+0);

      return get_endpoint( plc, coboundary_lines, other_vertex, other_line, direction, line_to_new_line_index, new_line_id, numeric_config );
    }

    return vertex_id;
  }




  template<typename NumericConfigT>
  void coarsen_plc_mesh(viennagrid_plc plc,
                        viennagrid_plc output_plc,
                        NumericConfigT numeric_config)
  {
    typedef viennagrid::point PointType;

    std::map<viennagrid_int, viennagrid_int> vertex_copy_map;

    viennagrid_dimension geometric_dimension;
    viennagrid_plc_geometric_dimension_get(plc, &geometric_dimension);
    viennagrid_plc_geometric_dimension_set(output_plc, geometric_dimension);

    viennagrid_element_id vertex_begin;
    viennagrid_element_id vertex_end;
    viennagrid_plc_elements_get(plc, 0, &vertex_begin, &vertex_end);

    viennagrid_element_id line_begin;
    viennagrid_element_id line_end;
    viennagrid_plc_elements_get(plc, 1, &line_begin, &line_end);



    std::vector< std::vector<viennagrid_int> > coboundary_lines( vertex_end-vertex_begin );

    for (viennagrid_int line_id = line_begin; line_id != line_end; ++line_id)
    {
      viennagrid_int * vertices_begin;
      viennagrid_int * vertices_end;
      viennagrid_plc_boundary_elements(plc, line_id, 0, &vertices_begin, &vertices_end);

      coboundary_lines[ *(vertices_begin+0) ].push_back(line_id);
      coboundary_lines[ *(vertices_begin+1) ].push_back(line_id);
    }



    std::vector<viennagrid_int> new_line_ids;
    std::vector<viennagrid_int> line_to_new_line_index(line_end-line_begin, -1);


    for (viennagrid_int line_id = line_begin; line_id != line_end; ++line_id)
    {
      int new_line_id = viennagrid_compose_element_id(1, new_line_ids.size());

      if (line_to_new_line_index[ viennagrid_index_from_element_id(line_id) ] != -1)
        continue;

      line_to_new_line_index[ viennagrid_index_from_element_id(line_id) ] = new_line_id;

      viennagrid_int * vertices_begin;
      viennagrid_int * vertices_end;
      viennagrid_plc_boundary_elements(plc, line_id, 0, &vertices_begin, &vertices_end);

//       std::cout << "Starting with line " << line_id << ": " << *(vertices_begin+0) << "," << *(vertices_begin+1) << std::endl;


      PointType direction = viennagrid::get_point( plc, *(vertices_begin+0) ) - viennagrid::get_point( plc, *(vertices_begin+1) );
      direction /= viennagrid::norm_2(direction);

      viennagrid_int first = get_endpoint( plc, coboundary_lines, *(vertices_begin+0), line_id, direction, line_to_new_line_index, new_line_id, numeric_config );
      viennagrid_int second = get_endpoint( plc, coboundary_lines, *(vertices_begin+1), line_id, direction, line_to_new_line_index, new_line_id, numeric_config );


      std::map<viennagrid_int, viennagrid_int>::iterator new_first_it = vertex_copy_map.find(first);
      if (new_first_it == vertex_copy_map.end())
      {
        viennagrid_int tmp;
        viennagrid_numeric * coords;
        viennagrid_plc_vertex_coords_get(plc, first, &coords);
        viennagrid_plc_vertex_create(output_plc, coords, &tmp);
        new_first_it = vertex_copy_map.insert( std::make_pair(first, tmp) ).first;
      }

      std::map<viennagrid_int, viennagrid_int>::iterator new_second_it = vertex_copy_map.find(second);
      if (new_second_it == vertex_copy_map.end())
      {
        viennagrid_int tmp;
        viennagrid_numeric * coords;
        viennagrid_plc_vertex_coords_get(plc, second, &coords);
        viennagrid_plc_vertex_create(output_plc, coords, &tmp);
        new_second_it = vertex_copy_map.insert( std::make_pair(second, tmp) ).first;
      }


      viennagrid_int tmp;
      viennagrid_plc_line_create(output_plc, new_first_it->second, new_second_it->second, &tmp);

      assert(tmp == new_line_id);
      new_line_ids.push_back( new_line_id );
    }

    viennagrid_element_id facet_begin;
    viennagrid_element_id facet_end;
    viennagrid_plc_elements_get(plc, 2, &facet_begin, &facet_end);

    for (viennagrid_int facet_id = facet_begin; facet_id != facet_end; ++facet_id)
    {
      std::set<viennagrid_int> used_lines_indices;
      std::vector<viennagrid_int> new_plc_lines;

      viennagrid_int * facet_lines_begin;
      viennagrid_int * facet_lines_end;
      viennagrid_plc_boundary_elements(plc, facet_id, 1, &facet_lines_begin, &facet_lines_end);

      for (viennagrid_int * facet_line_id_it = facet_lines_begin; facet_line_id_it != facet_lines_end; ++facet_line_id_it)
      {
        if (used_lines_indices.insert( line_to_new_line_index[ viennagrid_index_from_element_id(*facet_line_id_it) ] ).second)
          new_plc_lines.push_back( new_line_ids[viennagrid_index_from_element_id(line_to_new_line_index[ viennagrid_index_from_element_id(*facet_line_id_it) ])] );
      }

      viennagrid_int new_facet_id;
      viennagrid_plc_facet_create( output_plc, new_plc_lines.size(), &new_plc_lines[0], &new_facet_id );

      viennagrid_int facet_hole_point_count;
      viennagrid_numeric * facet_hole_points;
      viennagrid_plc_facet_hole_points_get(plc, facet_id, &facet_hole_point_count, &facet_hole_points);

      for (viennagrid_int i = 0; i != facet_hole_point_count; ++i)
        viennagrid_plc_facet_hole_point_add(output_plc, new_facet_id, facet_hole_points+i*geometric_dimension);

      if (new_plc_lines.size() < 3)
      {
        warning(1) << "One PLC facet has less than three lines" << std::endl;
        warning(1) << "Old PLC facet: " << facet_id << std::endl;
        warning(1) << "New PLC facet: " << new_facet_id << std::endl;
      }
    }
  }






  extract_plc_geometry::extract_plc_geometry() {}
  std::string extract_plc_geometry::name() { return "extract_plc_geometry"; }

  bool extract_plc_geometry::run(viennamesh::algorithm_handle &)
  {
    typedef viennagrid::mesh MeshType;
    typedef viennagrid::result_of::point<MeshType>::type PointType;

    data_handle<double> coplanar_tolerance_input = get_input<double>("coplanar_tolerance");
    data_handle<double> colinear_tolerance_input = get_input<double>("colinear_tolerance");

    double coplanar_tolerance = 1e-8;
    if (coplanar_tolerance_input.valid())
      coplanar_tolerance = coplanar_tolerance_input();

    double colinear_tolerance = 1e-8;
    if (colinear_tolerance_input.valid())
      colinear_tolerance = colinear_tolerance_input();


    point_handle input_hole_points = get_input<point_handle>("hole_points");
    seed_point_handle input_seed_points = get_input<seed_point_handle>("seed_points");


    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");

    data_handle<viennagrid_plc> tmp = make_data<viennagrid_plc>();
    data_handle<viennagrid_plc> output_plc = make_data<viennagrid_plc>();


    typedef same_segments_functor Functor1Type;
    typedef same_orientation_functor<double> Functor2Type;

    Functor1Type f1;
    Functor2Type f2(coplanar_tolerance);
    same_cell_combine_functor< Functor1Type, Functor2Type > functor(f1, f2);

    extract_plcs(input_mesh(), tmp(), functor);
    coarsen_plc_mesh(tmp(), output_plc(), colinear_tolerance);

    viennagrid_dimension geo_dim;
    viennagrid_plc_geometric_dimension_get(output_plc(), &geo_dim);

    if (input_hole_points.valid())
    {
      point_container hole_points = input_hole_points.get_vector();
      for (std::size_t i = 0; i != hole_points.size(); ++i)
      {
        if (static_cast<viennagrid_dimension>(hole_points[i].size()) == geo_dim)
        {
          viennagrid_plc_volumetric_hole_point_add(output_plc(), &hole_points[i][0]);
        }
      }
    }

    if (input_seed_points.valid())
    {
      seed_point_container seed_points = input_seed_points.get_vector();
      for (std::size_t i = 0; i != seed_points.size(); ++i)
      {
        if (static_cast<viennagrid_dimension>(seed_points[i].first.size()) == geo_dim)
        {
          viennagrid_plc_seed_point_add(output_plc(), &seed_points[i].first[0], seed_points[i].second);
        }
      }
    }

    viennagrid_int facet_count;
    viennagrid_plc_element_count(output_plc(), 2, &facet_count);
    info(1) << "Extracted " << facet_count << " PLC facets" << std::endl;

    set_output("geometry", output_plc);
    return true;
  }

}






