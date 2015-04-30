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

#include "extract_slice_3d.hpp"
#include "viennagridpp/algorithm/distance.hpp"
#include "viennagridpp/algorithm/centroid.hpp"


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




  template<typename PointT, typename ToleranceT>
  PointT point_on_planes(PointT const & P0, PointT const & N0,
                         PointT const & P1, PointT const & N1,
                         ToleranceT tol)
  {
    typedef typename viennagrid::result_of::coord<PointT>::type CoordType;
    int index = 0;

    CoordType det = N0[1]*N1[2] - N1[1]*N0[2];
    if (std::abs(det) < tol)
    {
      index = 1;
      det = N0[0]*N1[2] - N1[0]*N0[2];

      if (std::abs(det) < tol)
      {
        index = 2;
        det = N0[0]*N1[1] - N1[0]*N0[1];
      }
    }

    int i0 = (index == 0) ? 1 : 0;
    int i1 = (index == 2) ? 1 : 2;

    PointT result(3);
    result[index] = 0;

    CoordType ip0 = viennagrid::inner_prod(P0,N0);
    CoordType ip1 = viennagrid::inner_prod(P1,N1);

    result[i0] = ( ip0*N1[i1] - ip1*N0[i1]) / det;
    result[i1] = (-ip0*N1[i0] + ip1*N0[i0]) / det;

    return result;
  }



  class cut_hyperplane
  {
  public:
    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::point<MeshType>::type PointType;
    typedef viennagrid::result_of::element<MeshType>::type ElementType;

    cut_hyperplane(double tol_) : tol(tol_) {}

    void operator()(MeshType const & input_mesh, MeshType const & output_mesh, PointType N)
    {
      N /= viennagrid::norm_2(N);

      typedef viennagrid::result_of::const_element_range<MeshType>::type ConstElementRangeType;
      typedef viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementRangeIterator;

      typedef viennagrid::result_of::const_element_range<ElementType>::type ConstBoundaryElementRangeType;
      typedef viennagrid::result_of::iterator<ConstBoundaryElementRangeType>::type ConstBoundaryElementRangeIterator;


      viennagrid::result_of::element_copy_map<>::type copy_map(output_mesh);

      typedef std::map<ElementType, std::pair<ElementType, ElementType> > LineLineMapType;
      LineLineMapType line_map;

      std::set<ElementType> hp_lines;

      ConstElementRangeType lines(input_mesh, 1);
      for (ConstElementRangeIterator lit = lines.begin(); lit != lines.end(); ++lit)
      {
        ElementType v[2] = { viennagrid::vertices(*lit)[0], viennagrid::vertices(*lit)[1] };
        PointType p[2] = { viennagrid::get_point(v[0]), viennagrid::get_point(v[1]) };
        double dp[2] = { viennagrid::inner_prod(N,p[0]), viennagrid::inner_prod(N,p[1]) };

//         std::cout << "  Line " << *lit << std::endl;
//         std::cout << "      " << dp[0] << "  " << dp[1] << std::endl;

        if (on_plane(dp[0]) && on_plane(dp[1]))
        {
//           std::cout << "    copy line, both vertices are on plane" << std::endl;
          // both are inside -> use
          ElementType l = copy_map(*lit);
          line_map[*lit] = std::make_pair(l, ElementType());
          hp_lines.insert(l);
          continue;
        }

        if (!inside(dp[0]) && !inside(dp[1]))
        {
//           std::cout << "    skip line, both vertices outside" << std::endl;
          // both are outside -> ignore
          continue;
        }

        if (inside(dp[0]) && inside(dp[1]))
        {
//           std::cout << "    copy line, both vertices inside" << std::endl;
          // both are inside -> use
          line_map[*lit] = std::make_pair(copy_map(*lit), ElementType());
          continue;
        }

        if (on_plane(dp[0]) || on_plane(dp[1]))
        {
          int pi = on_plane(dp[0]) ? 0 : 1;
          int oi = 1-pi;

          if (inside(dp[oi]))
          {
//             std::cout << "    copy line, one vertex on plane, other inside" << std::endl;
            line_map[*lit] = std::make_pair(copy_map(*lit), copy_map(v[pi]));
          }
          else
          {
//             std::cout << "    skip line, one vertex on plane, other outside" << std::endl;
          }

          continue;
        }


        int ii = inside(dp[0]) ? 0 : 1;

        ElementType v0 = copy_map( v[ii] );
        PointType p1 = cut_by_plane(N, p[0], p[1]);

        ElementType v_new = viennagrid::make_vertex(output_mesh, p1);
        ElementType l = viennagrid::make_line(output_mesh, v0, v_new);

//         std::cout << "    adding new point " << p1 << " (cut of " << p[0] << " and " << p[1] << ")" << std::endl;

        line_map[*lit] = std::make_pair(l, v_new);
      }


      ConstElementRangeType plcs( input_mesh, 2 );
      for (ConstElementRangeIterator plcit = plcs.begin(); plcit != plcs.end(); ++plcit)
      {
        ConstBoundaryElementRangeType vertices( *plcit, 0 );
        if (vertices.size() < 3)
        {
          std::cout << "ERROR!!" << std::endl;
          continue;
        }

        ElementType plcv[3] = { vertices[0], vertices[1], vertices[2] };
        PointType plcp[3] = { viennagrid::get_point(plcv[0]), viennagrid::get_point(plcv[1]), viennagrid::get_point(plcv[2]) };

        PointType plcN = viennagrid::cross_prod( plcp[1]-plcp[0], plcp[2]-plcp[0] );
        plcN /= viennagrid::norm_2(plcN);

        if (viennagrid::inner_prod(plcN,N) > 1.0-tol)
        {
          if (viennagrid::inner_prod(N,plcp[0]) > -tol)
            copy_map(*plcit);
          continue;
        }

        PointType plc_hp_dir = viennagrid::cross_prod(plcN, N);
        plc_hp_dir /= viennagrid::norm_2(plc_hp_dir);

        PointType plc_hp_point = point_on_planes( viennagrid::make_point(0,0,0), N,
                                                  plcp[0], plcN,
                                                  tol );


        std::map<double, ElementType> plc_hyperplane_points;
        std::set<ElementType> plc_lines;

        ConstBoundaryElementRangeType lines( *plcit, 1 );
        for (ConstBoundaryElementRangeIterator lit = lines.begin(); lit != lines.end(); ++lit)
        {
          LineLineMapType::iterator it = line_map.find(*lit);
          if (it == line_map.end())
            continue;

          plc_lines.insert( it->second.first );
          if (it->second.second.valid())
          {
            PointType p_hp = viennagrid::get_point(it->second.second);
            double d = viennagrid::inner_prod(p_hp-plc_hp_point, plc_hp_dir);
            plc_hyperplane_points[d] = it->second.second;
          }
        }

        {
          if (plc_hyperplane_points.size() >= 2)
          {
            std::map<double, ElementType>::iterator it0 = plc_hyperplane_points.begin();
            std::map<double, ElementType>::iterator it1 = it0; ++it1;
            for (; it1 != plc_hyperplane_points.end(); ++it0, ++it1)
            {
              ElementType l = viennagrid::make_line(output_mesh, (*it0).second, (*it1).second);
              plc_lines.insert(l);
              hp_lines.insert(l);
            }
          }
        }

        if (plc_lines.size() >= 3)
          viennagrid::make_plc( output_mesh, plc_lines.begin(), plc_lines.end() );
      }

      viennagrid::make_plc( output_mesh, hp_lines.begin(), hp_lines.end() );
    }

  private:

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


  extract_symmetric_slice_3d::extract_symmetric_slice_3d() {}
  std::string extract_symmetric_slice_3d::name() { return "extract_symmetric_slice_3d"; }

  bool extract_symmetric_slice_3d::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    int geometric_dimension = viennagrid::geometric_dimension( input_mesh() );

    if (geometric_dimension != 3)
      return false;

    double tol = 1e-6;

    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::point<MeshType>::type PointType;
    typedef viennagrid::result_of::element<MeshType>::type ElementType;

    typedef viennagrid::result_of::const_element_range<MeshType>::type ConstElementRangeType;
    typedef viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementRangeIterator;


    PointType N[2];
    N[0] = get_required_input<point_t>("normal")(0);
    N[1] = get_required_input<point_t>("normal")(1);

    for (int i = 0; i != 2; ++i)
      N[i] /= viennagrid::norm_2(N[i]);

    PointType axis = viennagrid::cross_prod(N[0], N[1]);
    axis /= viennagrid::norm_2(axis);



    MeshType tmp;
    mesh_handle output_mesh = make_data<mesh_handle>();
    (cut_hyperplane(tol))(input_mesh(), tmp, N[0]);
//     std::cout << std::endl << std::endl;
    (cut_hyperplane(tol))(tmp, output_mesh(), N[1]);


    set_output( "mesh", output_mesh );

    return true;
  }

}
