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

#include "extract_slice_3d_triangle.hpp"
#include "geometry.hpp"

#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/algorithm/centroid.hpp"


namespace viennamesh
{

  class cut_hyperplane_triangle
  {
  public:
    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::point<MeshType>::type PointType;
    typedef viennagrid::result_of::element<MeshType>::type ElementType;

    cut_hyperplane_triangle(double tol_) : tol(tol_) {}

    void operator()(MeshType const & input_mesh,
                    MeshType const & output_mesh,
                    MeshType const & plc_output_mesh,
                    PointType const & N)
    {
      typedef viennagrid::result_of::const_element_range<MeshType>::type ConstElementRangeType;
      typedef viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementRangeIterator;

      viennagrid::result_of::element_copy_map<>::type copy_map(output_mesh, false);

      ConstElementRangeType triangles( input_mesh, 2 );
      for (ConstElementRangeIterator tit = triangles.begin(); tit != triangles.end(); ++tit)
      {
        ElementType v[3];
        PointType p[3];
        double dp[3];

        for (int pi = 0; pi != 3; ++pi)
        {
          v[pi] = viennagrid::vertices(*tit)[pi];
          p[pi] = viennagrid::get_point( v[pi] );

          dp[pi] = viennagrid::inner_prod( p[pi], N );
        }

        if ( !inside(dp[0]) && !inside(dp[1]) && !inside(dp[2]) )
        {
          // all points outside -> ignore
          continue;
        }

        int on_plane_count = 0;
        for (int pi = 0; pi != 3; ++pi)
          if ( on_plane(dp[pi]) )
            ++on_plane_count;

        if (on_plane_count == 3)
          continue;

        if (on_plane_count == 2)
        {
//           std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!  on_plane_count = 2" << std::endl;
          int not_on_plane_index = !on_plane(dp[0]) ? 0 : !on_plane(dp[1]) ? 1 : 2;
          if ( inside(dp[not_on_plane_index]) )
          {
            copy_map(*tit);

            int oi0 = (not_on_plane_index == 0) ? 1 : 0;
            int oi1 = (not_on_plane_index == 2) ? 1 : 2;

            add_line(copy_map(v[oi0]), copy_map(v[oi1]));
          }
//           else
//             std::cout << "   outside -> skipping" << std::endl;
          continue;
        }

        if (on_plane_count == 1)
        {
//           std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!  on_plane_count = 1" << std::endl;
          int on_plane_index = on_plane(dp[0]) ? 0 : on_plane(dp[1]) ? 1 : 2;
          int oi0 = (on_plane_index == 0) ? 1 : 0;
          int oi1 = (on_plane_index == 2) ? 1 : 2;

          if ( !inside(dp[oi0]) && !inside(dp[oi1]) )
            continue;

          if ( inside(dp[oi0]) && inside(dp[oi1]) )
          {
            copy_map(*tit);
            continue;
          }

          // oi0 is inside
          if ( !inside(dp[oi0]) )
            std::swap(oi0, oi1);

          ElementType v_ = get_vertex(output_mesh, N, v[oi0], v[oi1]);
          viennagrid::make_triangle( output_mesh, copy_map(v[on_plane_index]), copy_map(v[oi0]), v_);

          add_line(v_, copy_map(v[on_plane_index]));
          continue;
        }


        if ( inside(dp[0]) && inside(dp[1]) && inside(dp[2])  )
        {
          // all points inside -> copy
          copy_map(*tit);
          continue;
        }

        int pos_count = 0;
        for (int pi = 0; pi != 3; ++pi)
          if ( inside(dp[pi]) )
            ++pos_count;

        int pi = (dp[0]*dp[1] > 0) ? 2 : (dp[1]*dp[2] > 0) ? 0 : 1;
        int oi0 = (pi == 0) ? 1 : 0;
        int oi1 = (pi == 2) ? 1 : 2;

        ElementType v1_ = get_vertex(output_mesh, N, v[pi], v[oi0]);
        ElementType v2_ = get_vertex(output_mesh, N, v[pi], v[oi1]);
        add_line(v1_, v2_);

        if (pos_count == 1)
        {
          viennagrid::make_triangle( output_mesh, copy_map(v[pi]), v1_, v2_);
        }
        else
        {
          viennagrid::make_triangle( output_mesh, copy_map(v[oi0]), copy_map(v[oi1]), v1_);
          viennagrid::make_triangle( output_mesh, copy_map(v[oi1]), v1_, v2_);
        }
      }

      viennagrid::result_of::element_copy_map<>::type plc_copy_map(plc_output_mesh, false);
      std::vector<ElementType> new_lines;

      for (LinesOnHyperplaneType::iterator it = lines_on_hyperplane.begin(); it != lines_on_hyperplane.end(); ++it)
        new_lines.push_back( viennagrid::make_line(plc_output_mesh, plc_copy_map((*it).first), plc_copy_map((*it).second))   );

      viennagrid::make_plc(plc_output_mesh, new_lines.begin(), new_lines.end());
    }

  private:


    typedef std::map< std::pair<ElementType, ElementType>, ElementType > LineVertexMapType;
    LineVertexMapType line_vertex_map;

    ElementType get_vertex(MeshType const & output_mesh, PointType const & N, ElementType v0, ElementType v1)
    {
      if (v1 < v0)
        std::swap(v0, v1);

      LineVertexMapType::iterator it = line_vertex_map.find( std::make_pair(v0, v1) );
      if (it != line_vertex_map.end())
        return it->second;

      PointType cut_point = cut_by_plane(N, viennagrid::get_point(v0), viennagrid::get_point(v1));
      ElementType vtx = viennagrid::make_vertex(output_mesh, cut_point);
      line_vertex_map[std::make_pair(v0, v1)] = vtx;
      return vtx;
    }


    typedef std::set< std::pair<ElementType, ElementType> > LinesOnHyperplaneType;
    LinesOnHyperplaneType lines_on_hyperplane;

    void add_line(ElementType v0, ElementType v1)
    {
      if (v1 < v0)
        std::swap(v0, v1);

//       std::cout << "  adding line " << v0 << " - " << v1 << std::endl;

      lines_on_hyperplane.insert( std::make_pair(v0,v1) );
    }




    bool inside(double pn)
    {
      return pn > 0;
    }

    bool on_plane(double pn)
    {
      return std::abs(pn) < tol;
    }

    double tol;
  };





  extract_symmetric_slice_3d_triangle::extract_symmetric_slice_3d_triangle() {}
  std::string extract_symmetric_slice_3d_triangle::name() { return "extract_symmetric_slice_3d_triangle"; }

  bool extract_symmetric_slice_3d_triangle::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    int geometric_dimension = viennagrid::geometric_dimension( input_mesh() );

    if (geometric_dimension != 3)
      return false;

    double tol = 1e-6;
    if (get_input<double>("tolerance").valid())
      tol = get_input<double>("tolerance")();

    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::point<MeshType>::type PointType;
    typedef viennagrid::result_of::element<MeshType>::type ElementType;

    typedef viennagrid::result_of::const_element_range<MeshType>::type ConstElementRangeType;
    typedef viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementRangeIterator;


    PointType axis = get_required_input<point_t>("axis")();
    axis.normalize();

    int rotational_frequency = get_required_input<int>("rotational_frequency")();
    double angle = 2*M_PI/rotational_frequency;

    PointType N[2];
    N[0] = viennagrid::make_point(0,1,0);
    if ( std::abs(viennagrid::inner_prod(axis,N[0])) > 1.0-tol )
      N[0] = viennagrid::make_point(-1,0,0);
    N[0] -= axis * viennagrid::inner_prod(axis,N[0]);
    N[0].normalize();

    N[1] = -rotate(N[0], point_t(geometric_dimension), axis, angle);


    info(1) << "Using rotational frequency " << rotational_frequency << std::endl;
    info(1) << "Angle = " << angle << std::endl;
    info(1) << "Axis = " << axis << std::endl;
    info(1) << "Normal[0] = " << N[0] << std::endl;
    info(1) << "Normal[1] = " << N[1] << std::endl;


    mesh_handle tmp = cut( input_mesh(), N[0], tol );
    mesh_handle output_mesh = cut( tmp(), N[1], tol );

    set_output( "mesh", output_mesh );

    return true;
  }






  extract_symmetric_slice_3d_triangle::mesh_handle extract_symmetric_slice_3d_triangle::cut(viennagrid::const_mesh_t const & mesh,
                                                       viennagrid::point_t const & N,
                                                       double tolerance)
  {
    typedef viennagrid::mesh_t MeshType;

    mesh_handle tmp = make_data<mesh_handle>();
    MeshType plc_tmp;

    (cut_hyperplane_triangle(tolerance))(mesh, tmp(), plc_tmp, N);

    algorithm_handle triangle_hull_mesher = context().make_algorithm( "triangle_make_hull" );
    triangle_hull_mesher.set_input("mesh", plc_tmp);
    triangle_hull_mesher.run();

    mesh_handle meshed_plane = triangle_hull_mesher.get_output<MeshType>("mesh");
    tmp.push_back( meshed_plane() );

    algorithm_handle merger = context().make_algorithm( "merge_meshes" );
    merger.set_input("mesh", tmp);
    merger.set_input("region_offset", false);
    merger.run();

    return merger.get_output<MeshType>("mesh");
  }




}
