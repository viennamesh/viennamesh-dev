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

#include <set>
#include <map>
#include <iterator>
#include <boost/concept_check.hpp>

#include "extract_plc_slice_3d.hpp"
#include "geometry.hpp"

#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/algorithm/centroid.hpp"


namespace viennamesh
{

  template<typename PointT>
  PointT cut_by_plane(PointT const & N, PointT const & p0, PointT const & p1)
  {
    typedef typename viennagrid::result_of::coord<PointT>::type CoordType;
    CoordType ip0 = viennagrid::inner_prod(p0, N);
    CoordType ip1 = viennagrid::inner_prod(p1, N);
    return ((p0 - N*ip0)*std::abs(ip1) + (p1 - N*ip1)*std::abs(ip0)) / (std::abs(ip0) + std::abs(ip1));
  }


  struct line_info
  {
    line_info() : id(-1), vertex_on_plane0(-1), vertex_on_plane1(-1) {}
    line_info(viennagrid_int id_) : id(id_), vertex_on_plane0(-1), vertex_on_plane1(-1) {}
    line_info(viennagrid_int id_, viennagrid_int v0, viennagrid_int v1) : id(id_), vertex_on_plane0(v0), vertex_on_plane1(v1) {}

    viennagrid_int id;
    viennagrid_int vertex_on_plane0;
    viennagrid_int vertex_on_plane1;
  };


  class cut_hyperplanes
  {
  public:
    typedef viennagrid::point point_type;

    cut_hyperplanes(double tol_,
                   viennagrid_plc input_plc_,
                   viennagrid_plc output_plc_,
                   point_type const & N0_,
                   point_type const & N1_) : input_plc(input_plc_), output_plc(output_plc_), tol(tol_)
    {
      N[0] = N0_;
      N[1] = N1_;

      axis = viennagrid::cross_prod( N[1], N[0] );
      axis.normalize();

      P[0] = viennagrid::cross_prod( N[0], axis );
      P[1] = viennagrid::cross_prod( axis, N[1] );


      viennagrid_dimension geometric_dimension;
      viennagrid_plc_geometric_dimension_get(input_plc, &geometric_dimension);

      assert( geometric_dimension == 3 );
      viennagrid_plc_geometric_dimension_set(output_plc, 3);

      viennagrid_plc_element_count(input_plc, 0, &num_vertices);
      viennagrid_plc_element_count(input_plc, 1, &num_lines);
      viennagrid_plc_element_count(input_plc, 2, &num_facets);

      viennagrid_plc_vertex_coords_pointer(input_plc, &coords);

      viennagrid_plc_boundary_element_pointer(input_plc, 1, 0, NULL, &line_vertices);
    }

    void clear()
    {
      for (int i = 0; i != 2; ++i)
        dp[i].clear();

      num_vertices = num_lines = num_facets = 0;

      coords = 0;
    }

    void operator()()
    {
      for (int pi = 0; pi != 2; ++pi)
      {
        dn[pi].resize(num_vertices);
        dp[pi].resize(num_vertices);
      }

      for (int pi = 0; pi != 2; ++pi)
      {
        for (viennagrid_int vid = 0; vid != num_vertices; ++vid)
        {
          dn[pi][vid] = viennagrid::inner_prod( point_type(3,coords+3*vid), N[pi] );
          dp[pi][vid] = viennagrid::inner_prod( point_type(3,coords+3*vid), P[pi] );
        }
      }



      std::vector<line_info> line_map(num_lines);


      viennagrid_element_id line_begin;
      viennagrid_element_id line_end;
      viennagrid_plc_elements_get(input_plc, 1, &line_begin, &line_end);

      for (viennagrid_int lid = line_begin; lid != line_end; ++lid)
      {
        viennagrid_int v0 = *(line_vertices+2*viennagrid_index_from_element_id(lid)+0);
        viennagrid_int v1 = *(line_vertices+2*viennagrid_index_from_element_id(lid)+1);

        if ( line_on_plane_or_inside(0, lid) && line_on_plane_or_inside(1, lid) )
        {
          line_map[viennagrid_index_from_element_id(lid)] = line_info( copy_line(lid) );
//           std::cout << "Copying line (old id=" << lid << " new id=" << line_map[lid].id << ")   " << v0 << " " << v1 << std::endl;
          continue;
        }
      }



      std::vector<viennagrid_int> lines_on_plane[2];
      std::map<viennagrid_numeric, viennagrid_int> vertices_on_axis;

      viennagrid_element_id facet_begin;
      viennagrid_element_id facet_end;
      viennagrid_plc_elements_get(input_plc, 2, &facet_begin, &facet_end);

      for (viennagrid_int fid = facet_begin; fid != facet_end; ++fid)
      {
//         std::cout << "Old facet: " << fid << std::endl;

        viennagrid_int * vit_start;
        viennagrid_int * vit_end;
        viennagrid_plc_boundary_elements(input_plc, fid, 0, &vit_start, &vit_end);

        viennagrid_int * lit_start;
        viennagrid_int * lit_end;
        viennagrid_plc_boundary_elements(input_plc, fid, 1, &lit_start, &lit_end);



        point_type facet_normal;
        for (viennagrid_int * vit0 = vit_start; vit0 != vit_end; ++vit0)
        {
          viennagrid_int * vit1 = vit0; ++vit1;
          point_type p = point(*vit0);

          for (; vit1 != vit_end; ++vit1)
          {
            viennagrid_int * vit2 = vit1; ++vit2;
            point_type d0 = point(*vit1) - p;
            d0.normalize();

            for (; vit2 != vit_end; ++vit2)
            {
              point_type d1 = point(*vit2) - p;
              d1.normalize();

              if ( std::abs(viennagrid::inner_prod(d0, d1)) > tol )
              {
                facet_normal = viennagrid::cross_prod(d0, d1);
                break;
              }
            }

            if (vit2 != vit_end)
              break;
          }

          if (vit1 != vit_end)
            break;
        }

        facet_normal.normalize();
        bool facet_orthogonal_to_axis = std::abs(viennagrid::inner_prod(facet_normal, axis)) < tol;




        viennagrid_int facet_hole_point_count;
        viennagrid_numeric * facet_hole_points;
        viennagrid_plc_facet_hole_points_get(input_plc, fid, &facet_hole_point_count, &facet_hole_points);


        std::map< viennagrid_int, std::vector<viennagrid_int> > coboundary_lines;
        for (viennagrid_int * lit = lit_start; lit != lit_end; ++lit)
        {
          viennagrid_int lid = *lit;
          viennagrid_int v0 = vertex(lid, 0);
          viennagrid_int v1 = vertex(lid, 1);

          coboundary_lines[v0].push_back(lid);
          coboundary_lines[v1].push_back(lid);
        }


        std::map<viennagrid_int, int> line_hole_flag; // 0 - no hole, 1 - hole on left side, -1 - hole on right side
        for (viennagrid_int * lit = lit_start; lit != lit_end; ++lit)
          line_hole_flag[*lit] = 0;


        for (viennagrid_int i = 0; i != facet_hole_point_count; ++i)
        {
          point_type hole_point(3, facet_hole_points+3*i);
//           std::cout << "  Hole point " << hole_point << std::endl;

          viennagrid_int found_lid = get_non_intersecting_line(hole_point, lit_start, lit_end);
          assert( found_lid != -1 );

          viennagrid_int v0 = vertex(found_lid, 0);
          viennagrid_int v1 = vertex(found_lid, 1);

          point_type p0 = point(v0);
          point_type p1 = point(v1);
          point_type middle = (p0+p1)/2;

          point_type tmp = viennagrid::cross_prod( p1-p0, hole_point-middle );

          line_hole_flag[found_lid] = viennagrid::inner_prod(facet_normal, tmp) > 0 ? 1 : -1;

//           std::cout << "    found line: " << p0 << " " << p1 << "   flag: " << line_hole_flag[found_lid] << std::endl;
//           std::cout << "    facet normal: " << facet_normal << std::endl;

          viennagrid_int vid =     vertex(found_lid, 1);
          viennagrid_int end_vid = vertex(found_lid, 0);
          assert( coboundary_lines[vid].size() == 2 );

          viennagrid_int prev_lid = found_lid;
          while (vid != end_vid)
          {
            assert( coboundary_lines[vid].size() == 2 );
            viennagrid_int lid = coboundary_lines[vid][0] != prev_lid ? coboundary_lines[vid][0] : coboundary_lines[vid][1];
            assert( line_hole_flag[lid] == 0 );

            viennagrid_int v0 = vertex(lid, 0);
            viennagrid_int v1 = vertex(lid, 1);

            if (v0 == vid)
            {
              line_hole_flag[lid] =   line_hole_flag[prev_lid];
              vid = v1;
            }
            else
            {
              line_hole_flag[lid] = - line_hole_flag[prev_lid];
              vid = v0;
            }

//             std::cout << "      marked line " << point_type(3, coords+3*v0) << " " << point_type(3, coords+3*v1) << "   with " << line_hole_flag[lid] << std::endl;

            prev_lid = lid;
          }


        }




        viennagrid_int vertex_on_axis = -1;

        if (!facet_orthogonal_to_axis)
        {
          viennagrid_numeric d = viennagrid::inner_prod(point_type(3, coords+3**vit_start), facet_normal) / viennagrid::inner_prod(axis, facet_normal);

          point_type p = axis*d;

          viennagrid_int lid = get_non_intersecting_line( p, lit_start, lit_end );
          viennagrid_int v0 = vertex(lid, 0);
          viennagrid_int v1 = vertex(lid, 1);

          point_type p0 = point(v0);
          point_type p1 = point(v1);
          point_type middle = (p0+p1)/2;

          viennagrid_numeric tmp = viennagrid::inner_prod(facet_normal, viennagrid::cross_prod( p1-p0, p-middle ));

          // adding point on axis only if point on axis is not in a facet hole region
          if ( ! (((tmp > 0) && (line_hole_flag[lid] == 1)) || ((tmp < 0) && (line_hole_flag[lid] == -1))) )
          {
            std::map<viennagrid_numeric, viennagrid_int>::iterator it = vertices_on_axis.find(d);
            if (it == vertices_on_axis.end())
            {
//               std::cout << "Found vertex on axis: " << p << std::endl;

              viennagrid_plc_vertex_create(output_plc, &p[0], &vertex_on_axis);
              vertices_on_axis[d] = vertex_on_axis;
            }
            else
              vertex_on_axis = it->second;
          }
        }



        std::vector<viennagrid_int> facet_lines;
        for (viennagrid_int * lit = lit_start; lit != lit_end; ++lit)
        {
          viennagrid_int lid = *lit;
          viennagrid_int v0 = vertex(lid, 0);
          viennagrid_int v1 = vertex(lid, 1);

          if ( line_map[viennagrid_index_from_element_id(lid)].id >= 0 )
          {
            facet_lines.push_back(line_map[viennagrid_index_from_element_id(lid)].id);
          }
        }



        for (int pi = 0; pi != 2; ++pi)
        {
          std::map<viennagrid_numeric, viennagrid_int> vertices_in_plane;
          if (vertex_on_axis >= 0)
            vertices_in_plane[0] = vertex_on_axis;

          for (viennagrid_int * vit = vit_start; vit != vit_end; ++vit)
          {
            viennagrid_int vid = *vit;
            if ( vertex_on_plane(pi,vid) )
            {
              if (dp[pi][vid] > -tol)
              {
                viennagrid_numeric d = facet_orthogonal_to_axis ? viennagrid::inner_prod(axis, point(vid)) : dp[pi][vid];
                vertices_in_plane[d] = copy_vertex(vid);
              }
            }
          }

          if (vertices_in_plane.size() > 1)
          {
            std::map<viennagrid_numeric, viennagrid_int>::iterator it0 = vertices_in_plane.begin();
            std::map<viennagrid_numeric, viennagrid_int>::iterator it1 = it0; ++it1;

            for (; it1 != vertices_in_plane.end(); ++it0, ++it1)
            {
              facet_lines.push_back( get_make_line(it0->second, it1->second) );
              lines_on_plane[pi].push_back( facet_lines.back() );
            }
          }
        }

        std::sort(facet_lines.begin(), facet_lines.end());
        facet_lines.erase( unique( facet_lines.begin(), facet_lines.end() ), facet_lines.end() );

        if (facet_lines.size() > 2)
        {
          viennagrid_int new_fid;
          viennagrid_plc_facet_create(output_plc, facet_lines.size(), &facet_lines[0], &new_fid);

          for (viennagrid_int i = 0; i != facet_hole_point_count; ++i)
            viennagrid_plc_facet_hole_point_add(output_plc, new_fid, facet_hole_points+3*i);
        }
      }

      for (int pi = 0; pi != 2; ++pi)
      {
        std::sort(lines_on_plane[pi].begin(), lines_on_plane[pi].end());
        lines_on_plane[pi].erase( unique( lines_on_plane[pi].begin(), lines_on_plane[pi].end() ), lines_on_plane[pi].end() );


        if (vertices_on_axis.size() > 1)
        {
          std::map<viennagrid_numeric, viennagrid_int>::iterator it0 = vertices_on_axis.begin();
          std::map<viennagrid_numeric, viennagrid_int>::iterator it1 = it0; ++it1;
          for (; it1 != vertices_on_axis.end(); ++it0, ++it1)
            lines_on_plane[pi].push_back( get_make_line(it0->second, it1->second) );
        }

        if (lines_on_plane[pi].size() > 2)
        {
          viennagrid_int new_fid;
          viennagrid_plc_facet_create(output_plc, lines_on_plane[pi].size(), &lines_on_plane[pi][0], &new_fid);
        }
      }

    }

  private:

    viennagrid_plc input_plc;
    viennagrid_numeric * coords;

    viennagrid_element_id * line_vertices;

    viennagrid_element_id vertex(viennagrid_element_id lid, viennagrid_int index) const
    {
      return *(line_vertices + 2*viennagrid_index_from_element_id(lid) + index);
    }

    point_type point(viennagrid_element_id vid) const
    {
      return point_type(3, coords+3*viennagrid_index_from_element_id(vid));
    }


    viennagrid_plc output_plc;

    bool vertex_on_plane(viennagrid_int pi, viennagrid_element_id vid) const { return std::abs(dn[pi][viennagrid_index_from_element_id(vid)]) < tol; }
    bool vertex_inside(viennagrid_int pi, viennagrid_element_id vid) const { return dn[pi][viennagrid_index_from_element_id(vid)] > 0; }
    bool vertex_outside(viennagrid_int pi, viennagrid_element_id vid) const { return dn[pi][viennagrid_index_from_element_id(vid)] < 0; }
    bool vertex_on_plane_or_inside(viennagrid_int pi, viennagrid_element_id vid) const { return dn[pi][viennagrid_index_from_element_id(vid)] > -tol; }
    bool vertex_on_plane_or_outside(viennagrid_int pi, viennagrid_element_id vid) const { return dn[pi][viennagrid_index_from_element_id(vid)] < tol; }

    bool line_on_plane(viennagrid_int pi, viennagrid_element_id lid) const
    { return vertex_on_plane(pi, vertex(lid, 0)) && vertex_on_plane(pi, vertex(lid, 1)); }
    bool line_inside(viennagrid_int pi, viennagrid_element_id lid) const
    { return vertex_inside(pi, vertex(lid, 0)) && vertex_inside(pi, vertex(lid, 1)); }
    bool line_outside(viennagrid_int pi, viennagrid_element_id lid) const
    { return vertex_outside(pi, vertex(lid, 0)) && vertex_outside(pi, vertex(lid, 1)); }
    bool line_on_plane_or_inside(viennagrid_int pi, viennagrid_element_id lid) const
    { return vertex_on_plane_or_inside(pi, vertex(lid, 0)) && vertex_on_plane_or_inside(pi, vertex(lid, 1)); }
    bool line_on_plane_or_outside(viennagrid_int pi, viennagrid_element_id lid) const
    { return vertex_on_plane_or_outside(pi, vertex(lid, 0)) && vertex_on_plane_or_outside(pi, vertex(lid, 1)); }


    viennagrid_element_id copy_vertex(viennagrid_element_id vid)
    {
      std::map<viennagrid_element_id, viennagrid_element_id>::iterator vit = vertex_map.find(vid);
      if (vit != vertex_map.end())
        return vit->second;

      viennagrid_element_id new_vid;
      viennagrid_plc_vertex_create(output_plc, coords+3*vid, &new_vid);
      vertex_map[vid] = new_vid;

      return new_vid;
    }

    viennagrid_element_id get_make_line( std::pair<viennagrid_element_id, viennagrid_element_id> const & vids )
    {
      std::map< std::pair<viennagrid_element_id, viennagrid_element_id>, viennagrid_element_id>::iterator lit = line_map.find(vids);
      if ( lit != line_map.find(vids) )
        return lit->second;

      viennagrid_element_id lid;
      viennagrid_plc_line_create(output_plc, vids.first, vids.second, &lid);
      line_map[vids] = lid;
      return lid;
    }

    viennagrid_element_id get_make_line( viennagrid_element_id vid0, viennagrid_element_id vid1 )
    {
      return get_make_line( std::make_pair(vid0, vid1) );
    }

    viennagrid_element_id copy_line( viennagrid_element_id vid0, viennagrid_element_id vid1 )
    {
      viennagrid_element_id new_vid0 = copy_vertex(vid0);
      viennagrid_element_id new_vid1 = copy_vertex(vid1);

      return get_make_line(new_vid0, new_vid1);
    }

    viennagrid_element_id copy_line( viennagrid_element_id lid )
    {
      viennagrid_element_id * lvit_start;
      viennagrid_element_id * lvit_end;
      viennagrid_plc_boundary_elements(input_plc, lid, 0, &lvit_start, &lvit_end);

      return copy_line( *lvit_start, *(lvit_start+1));
    }



    viennagrid_element_id get_non_intersecting_line(point_type const & p,
                                                    viennagrid_element_id * lit_start, viennagrid_element_id * lit_end)
    {
      for (viennagrid_element_id * lit = lit_start; lit != lit_end; ++lit)
      {
        viennagrid_element_id lid = *lit;
        viennagrid_element_id v0 = *(line_vertices+2*viennagrid_index_from_element_id(lid)+0);
        viennagrid_element_id v1 = *(line_vertices+2*viennagrid_index_from_element_id(lid)+1);

        point_type p0(3, coords+3*v0);
        point_type p1(3, coords+3*v1);
        point_type middle = (p0+p1)/2;

        viennagrid_element_id * lit1 = lit_start;
        for (; lit1 != lit_end; ++lit1)
        {
          // no self intersect test
          if (lit1 == lit)
            continue;

          point_type l0(3, coords+3**(line_vertices+2*viennagrid_index_from_element_id(*lit1)+0));
          point_type l1(3, coords+3**(line_vertices+2*viennagrid_index_from_element_id(*lit1)+1));

          std::pair<point_type, point_type> cp = viennagrid::detail::closest_points_line_line( p, middle, l0, l1 );

          if ( viennagrid::norm_2(l0-l1) < tol )
          {
            break;
          }
        }

        if (lit1 == lit_end)
          return lid;
      }

      return -1;
    }


    viennagrid_int num_vertices;
    viennagrid_int num_lines;
    viennagrid_int num_facets;

    point_type axis;
    point_type N[2];
    point_type P[2];
    std::vector<viennagrid_numeric> dn[2];
    std::vector<viennagrid_numeric> dp[2];

    std::map<viennagrid_element_id, viennagrid_element_id> vertex_map;
    std::map< std::pair<viennagrid_element_id, viennagrid_element_id>, viennagrid_element_id> line_map;

    double tol;
  };





  extract_symmetric_plc_slice_3d::extract_symmetric_plc_slice_3d() {}
  std::string extract_symmetric_plc_slice_3d::name() { return "extract_symmetric_plc_slice_3d"; }

  bool extract_symmetric_plc_slice_3d::run(viennamesh::algorithm_handle &)
  {
    data_handle<viennagrid_plc> input = get_required_input<viennagrid_plc>("geometry");

    viennagrid_dimension geometric_dimension;
    viennagrid_plc_geometric_dimension_get(input(), &geometric_dimension);

    if (geometric_dimension != 3)
      return false;

    double tolerance = 1e-6;
    if (get_input<double>("tolerance").valid())
      tolerance = get_input<double>("tolerance")();

    typedef viennagrid::mesh                                                MeshType;
    typedef viennagrid::result_of::point<MeshType>::type                    PointType;


    PointType axis = get_required_input<point>("axis")();
    axis.normalize();

    int rotational_frequency = get_required_input<int>("rotational_frequency")();
    double angle = 2*M_PI/rotational_frequency;

    PointType N[2];
    N[0] = viennagrid::make_point(0,1,0);
    if ( std::abs(viennagrid::inner_prod(axis,N[0])) > 1.0-tolerance )
      N[0] = viennagrid::make_point(-1,0,0);
    N[0] -= axis * viennagrid::inner_prod(axis,N[0]);
    N[0].normalize();

    N[1] = -rotate(N[0], point(geometric_dimension), axis, angle);


    info(1) << "Using rotational frequency " << rotational_frequency << std::endl;
    info(1) << "Angle = " << angle << std::endl;
    info(1) << "Axis = " << axis << std::endl;
    info(1) << "Normal[0] = " << N[0] << std::endl;
    info(1) << "Normal[1] = " << N[1] << std::endl;


    data_handle<viennagrid_plc> output = make_data<viennagrid_plc>();
    (cut_hyperplanes(tolerance, input(), output(), N[0], N[1]))();

    set_output( "geometry", output );

    return true;
  }




}
