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






  class cut_hyperplane
  {
  public:
    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::point<MeshType>::type PointType;
    typedef viennagrid::result_of::element<MeshType>::type ElementType;

    cut_hyperplane(double tol_) : tol(tol_) {}

    void operator()(MeshType const & input_mesh, MeshType const & output_mesh, PointType const & N)
    {
      typedef typename viennagrid::result_of::const_element_range<MeshType>::type ConstElementRangeType;
      typedef typename viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementRangeIterator;

      viennagrid::result_of::element_copy_map<>::type copy_map(output_mesh);




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

        int pi = ((dp[0]-tol)*(dp[1]-tol) > 0) ? 2 : ((dp[1]-tol)*(dp[2]-tol) > 0) ? 0 : 1;
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


      for (LinesOnHyperplaneType::iterator it = lines_on_hyperplane.begin(); it != lines_on_hyperplane.end(); ++it)
      {
        std::cout << (*it).first << " " << (*it).second << std::endl;
      }

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

      std::cout << "  adding line " << v0 << " - " << v1 << std::endl;

      lines_on_hyperplane.insert( std::make_pair(v0,v1) );
    }




    bool inside(double pn)
    {
      return pn > -tol;
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

//     mesh_handle output_mesh = make_data<mesh_handle>();
//     viennagrid::result_of::element_copy_map<>::type copy_map( output_mesh() );



    PointType plane_normal[2];
    plane_normal[0] = get_required_input<point_t>("normal")(0);

//     viennagrid::make_point( 1, 2.0, 0 );
    plane_normal[1] = get_required_input<point_t>("normal")(1);
//     viennagrid::make_point( 1, -2.0, 0 );


//     for (int i = 0; i != 2; ++i)
//       plane_normal[i] /= viennagrid::norm_2(plane_normal[i]);
//
//     PointType axis = viennagrid::cross_prod( plane_normal[0], plane_normal[1] );
//     axis /= viennagrid::norm_2(axis);
// //     viennagrid::make_point(0, 0, 1);
//
//
//     std::map<ElementType, ElementType> line_map;
//
//     ConstElementRangeType lines( input_mesh(), 1 );
//     for (ConstElementRangeIterator lit = lines.begin(); lit != lines.end(); ++lit)
//     {
//       PointType p[2];
//       for (int i = 0; i != 2; ++i)
//         p[i] = viennagrid::get_point( viennagrid::vertices(*lit)[i] );
//
//       double d[2][2];
//       for (int i = 0; i != 2; ++i)
//         for (int j = 0; j != 2; ++j)
//           d[i][j] = viennagrid::inner_prod( plane_normal[i], p[j] );
//
//
//       if ( (d[0][0] > -tol) && (d[0][1] > -tol) && (d[1][0] > -tol) && (d[1][1] > -tol) )
//       {
//         // points are on positive plane_normal[0] and positive plane_normal[1]
//         line_map[*lit] = copy_map(*lit);
//       }
//       else if ( ((d[0][0] < -tol) && (d[0][1] < -tol)) || ((d[1][0] < -tol) && (d[1][1] < -tol)) )
//       {
//         // points are on negative plane_normal[0] or negative plane_normal[1]
//         continue;
//       }
//       else
//       {
//
//         // one point is on positive side of both planes
//         if ( ((d[0][0] > -tol) && (d[1][0] > -tol)) || ((d[0][1] > -tol) && (d[1][1] > -tol)) )
//         {
//           int pi = ((d[0][0] > -tol) && (d[1][0] > -tol)) ? 1 : 0;       // negative side point index
//
//
//           if ( (d[0][pi] < -tol) && (d[1][pi] < -tol) )
//           {
//             PointType new_p[2];
//
//             for (int pni = 0; pni != 2; ++pni)
//             {
//               PointType p_[2];
//               for (int j = 0; j != 2; ++j)
//                 p_[j] = p[j] - plane_normal[pni]*d[pni][j];
//
//               new_p[pni] = (p_[0] * std::abs(d[pni][1]) + p_[1] * std::abs(d[pni][0])) /
//                            (std::abs(d[pni][0]) + std::abs(d[pni][1]));
//             }
//
//             PointType new_p_used = viennagrid::norm_2(p[1-pi] - new_p[0]) < viennagrid::norm_2(p[1-pi] - new_p[1]) ? new_p[0] : new_p[1];
//
//             line_map[*lit] = viennagrid::make_line( output_mesh(),
//                                                     copy_map( viennagrid::vertices(*lit)[1-pi] ),
//                                                     viennagrid::make_vertex(output_mesh(), new_p_used) );
//           }
//           else
//           {
//             int pni = (d[0][pi] > -tol) ? 1 : 0;                           // plane normal index
//
//             PointType p_[2];
//             for (int j = 0; j != 2; ++j)
//               p_[j] = p[j] - plane_normal[pni]*d[pni][j];
//
//             PointType new_p = (p_[0] * std::abs(d[pni][1]) + p_[1] * std::abs(d[pni][0])) /
//                               (std::abs(d[pni][0]) + std::abs(d[pni][1]));
//
//             line_map[*lit] = viennagrid::make_line( output_mesh(),
//                                                     copy_map( viennagrid::vertices(*lit)[1-pi] ),
//                                                     viennagrid::make_vertex(output_mesh(), new_p) );
//           }
//         }
//         else
//         {
//           // p0 is on positive n0 but negative n1; p1 is on positive n1 but negative n0 or
//           // p0 is on negative n0 but positive n1; p1 is on negative n1 but positive n0
//
//           int p0i = (d[0][0] < -tol) ? 0 : 1;
//
//           PointType new_p0 = cut_by_plane( plane_normal[p0i], p[0], p[1] );
//           PointType new_p1 = cut_by_plane( plane_normal[1-p0i], p[0], p[1] );
//
//           if ((viennagrid::inner_prod(new_p0,plane_normal[1-p0i]) > -tol) &&
//               (viennagrid::inner_prod(new_p1,plane_normal[p0i]) > -tol))
//             line_map[*lit] = viennagrid::make_line( output_mesh(),
//                                                     viennagrid::make_vertex(output_mesh(), new_p0),
//                                                     viennagrid::make_vertex(output_mesh(), new_p1) );
//         }
//
//       }
//     }
//
//
//     ElementType axis_line;
//
//     {
//       // +/- 1 will work here because input mesh is required to be centered
//       double max_z = -1;
//       double min_z = 1;
//
//       ConstElementRangeType vertices( output_mesh(), 0 );
//       for (ConstElementRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
//       {
//         PointType p = viennagrid::get_point(*vit);
//         double z = viennagrid::inner_prod(axis, p);
//
//         max_z = std::max(max_z, z);
//         min_z = std::min(min_z, z);
//       }
//
//       axis_line = viennagrid::make_line( output_mesh(),
//                                          viennagrid::make_vertex(output_mesh(), axis*min_z),
//                                          viennagrid::make_vertex(output_mesh(), axis*max_z) );
//     }
//
//
//
//     ConstElementRangeType triangles( input_mesh(), 2 );
//     for (ConstElementRangeIterator tit = triangles.begin(); tit != triangles.end(); ++tit)
//     {
//       PointType p[3];
//       double pd[3];
//
//       for (int pni = 0; pni != 2; ++pni)
//       {
//         PointType const & N = plane_normal[pni];
//
//         for (int pi = 0; pi != 3; ++pi)
//         {
//           p[pi] = viennagrid::get_point( viennagrid::vertices(*tit)[pi] );
//           pd[pi] = viennagrid::inner_prod( p[pi], N );
//         }
//
//         if ( ((pd[0] > 0) && (pd[1] > 0) && (pd[2] > 0)) ||
//              ((pd[0] < 0) && (pd[1] < 0) && (pd[2] < 0)) )
//         {
//           // triangle is completely on one side of the plane
//           continue;
//         }
//
//
//         PointType new_p0 = cut_by_plane(N, p[0], (pd[0]*pd[1] < 0) ? p[1] : p[2]);
//         PointType new_p1 = cut_by_plane(N, (pd[0]*pd[2] < 0) ? p[0] : p[1], p[2]);
//
//         if ((viennagrid::inner_prod(new_p0, plane_normal[1-pni]) > -tol) &&
//             (viennagrid::inner_prod(new_p1, plane_normal[1-pni]) > -tol))
//         {
//           viennagrid::make_line( output_mesh(),
//                                  viennagrid::make_vertex(output_mesh(), new_p0),
//                                  viennagrid::make_vertex(output_mesh(), new_p1) );
//         }
//       }
//     }




    MeshType tmp;
    mesh_handle output_mesh = make_data<mesh_handle>();
    (cut_hyperplane(tol))(input_mesh(), tmp, plane_normal[0]);
    std::cout << std::endl << std::endl;
    (cut_hyperplane(tol))(tmp, output_mesh(), plane_normal[1]);



    set_output( "mesh", output_mesh );
//     set_output( "hull", output_hull );

    return true;
  }

}
