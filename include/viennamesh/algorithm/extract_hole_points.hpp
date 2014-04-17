#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_EXTRACT_HOLE_POINTS_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_EXTRACT_HOLE_POINTS_HPP

#include "viennagrid/mesh/neighbor_iteration.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/geometry.hpp"

////////////////////////////////////////////////////////////////////////
//                       intersect.hpp                                //
////////////////////////////////////////////////////////////////////////

namespace viennagrid
{
  namespace detail
  {


    template<typename PointAccessorT, typename ElementT, typename PointT, typename NumericConfigT>
    bool element_line_intersect_impl(PointAccessorT const point_accessor,
                            ElementT const & element,
                            viennagrid::line_tag,
                            viennagrid::dimension_tag<2>,
                            PointT const & ray_start,
                            PointT const & ray_end,
                            NumericConfigT nc)
    {
      typedef typename viennagrid::result_of::coord< PointT >::type CoordType;
      typedef typename detail::result_of::numeric_type<NumericConfigT, CoordType>::type inner_numeric_type;

      PointT const & v0 = viennagrid::point(viennagrid::vertices(element)[0]);
      PointT const & v1 = viennagrid::point(viennagrid::vertices(element)[1]);

      PointT const & w0 = ray_start;
      PointT const & w1 = ray_end;

      // write V(s) = v0 + s * (v1 - v0), s \in [0,1]
      //       W(t) = w0 + t * (w1 - w0), t \in [0,1]

      // compute s and t assuming V(s) and W(t) to be infinite lines:
      // cf. http://www.softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm
      PointT dir_v = v1 - v0;  //direction vector for line V(s)
      PointT dir_w = w1 - w0;  //direction vector for line W(t)



      CoordType v_in_v = viennagrid::inner_prod(dir_v, dir_v);
      CoordType v_in_w = viennagrid::inner_prod(dir_v, dir_w);
      CoordType w_in_w = viennagrid::inner_prod(dir_w, dir_w);

      CoordType denominator = v_in_v * w_in_w - v_in_w * v_in_w;

      if ( std::abs(denominator) < detail::relative_tolerance(nc, v_in_v * w_in_w) ) //lines parallel (up to round-off)
      {
        PointT lhs_v = w0 - v0;
        inner_numeric_type relative_eps = detail::relative_tolerance(nc, viennagrid::norm_1(dir_v) );

        if ( std::abs(dir_v[0] * lhs_v[1] - lhs_v[0] * dir_v[1]) < relative_eps ) // lines share a common ray
        {
          std::size_t index = (std::abs(dir_v[0]) < relative_eps) ? 1 : 0;

          CoordType w_first = lhs_v[index];
          CoordType w_second = w1[index] - v0[index];
          if (w_first > w_second)
            std::swap(w_first, w_second);

          CoordType v_first = 0;
          CoordType v_second = dir_v[index];
          if (v_first > v_second)
            std::swap(v_first, v_second);

          if (w_second < v_first || v_second < w_first)
            return false;
          else
            return true;

//           if (interval_intersect( coord_type(0), dir_v[index], tag1, w_first, w_second, tag2, nc ))
//           {
//             overlapping_lines_functor(v0, v1, tag1, w0, w1, tag2, coord_type(0), dir_v[index], w_first, w_second);
//             return intersection_result::intersection_dimension(1);
//           }
//           else
//             return intersection_result::no_intersection();
        }
        else
          return false;
//           return intersection_result::no_intersection();
      }

      //Lines are not parallel: Compute minimizers s, t:
      PointT dir_distance = v0 - w0;  //any vector connecting two points on V and W

      CoordType v_in_dir_distance = viennagrid::inner_prod(dir_v, dir_distance);
      CoordType w_in_dir_distance = viennagrid::inner_prod(dir_w, dir_distance);

      CoordType s = (v_in_w * w_in_dir_distance - w_in_w * v_in_dir_distance);
      CoordType t = (v_in_v * w_in_dir_distance - v_in_w * v_in_dir_distance);

      if (denominator < 0)
      {
        denominator = -denominator;
        s = -s;
        t = -t;
      }

//       if (point_in_interval(coord_type(0), denominator, tag1, s, nc) && point_in_interval(coord_type(0), denominator, tag2, t, nc))
      if (0 < s && s < denominator && 0 < t && t < denominator)
        return true;
      else
        return false;
//       {
//         point_intersection_functor(v0, v1, tag1, w0, w1, tag2, s, t, denominator);
//         return intersection_result::intersection_dimension(0);
//       }
//       else
//         return intersection_result::no_intersection();
    }



    template<typename PointAccessorT, typename ElementT, typename PointT, typename NumericConfigT>
    bool element_line_intersect_impl(PointAccessorT const point_accessor,
                            ElementT const & element,
                            viennagrid::triangle_tag,
                            viennagrid::dimension_tag<3>,
                            PointT const & ray_start,
                            PointT const & ray_end,
                            NumericConfigT nc)
    {
      PointT const & A = point_accessor(viennagrid::vertices(element)[0]);
      PointT const & B = point_accessor(viennagrid::vertices(element)[1]);
      PointT const & C = point_accessor(viennagrid::vertices(element)[2]);

      PointT r = ray_start;
      PointT d = ray_end - ray_start;

      PointT b = B-A; // vector from A to B
      PointT c = C-A; // vector from A to C

      PointT rhs = r-A;

      // solve the equation:
      // r + lambda*d = A + beta*b + gamma*c
      // beta*b + gamma*c - lambda*d = r-A
      // (b c -d) (beta gamma lambda)^T = r-A
      // (beta gamma lambda)^T = (b c -d)^-1 (r-A)

      double det = viennagrid::determinant( b, c, -d );

      if ( std::abs(det) < 1e-6)
      {
        PointT n = viennagrid::cross_prod( B-A, C-A );

        PointT center = (A+B+C)/3.0;
        if( std::abs( viennagrid::inner_prod( n, r-center ) ) < 1e-6 )
          return true; // r lies on triangle plane, TODO behandeln: kreuzt strahl dreieck?
        else
          return false;
      }
      else
      {
        PointT im[3]; // inverse matrix (b c -d)^-1

        d = -d; // invert for simplicity

        im[0][0] = c[1]*d[2] - c[2]*d[1];
        im[1][0] = -(b[1]*d[2] - b[2]*d[1]);
        im[2][0] = b[1]*c[2] - b[2]*c[1];

        im[0][1] = -(c[0]*d[2] - c[2]*d[0]);
        im[1][1] = b[0]*d[2] - b[2]*d[0];
        im[2][1] = -(b[0]*c[2] - b[2]*c[0]);

        im[0][2] = c[0]*d[1] - c[1]*d[0];
        im[1][2] = -(b[0]*d[1] - b[1]*d[0]);
        im[2][2] = b[0]*c[1] - b[1]*c[0];

        double beta   = viennagrid::inner_prod( rhs, im[0] );
        double gamma  = viennagrid::inner_prod( rhs, im[1] );
        double lambda = viennagrid::inner_prod( rhs, im[2] );

        if (det < 0)
        {
          det    = -det;
          beta   = -beta;
          gamma  = -gamma;
          lambda = -lambda;
        }

        double alpha  = det - beta - gamma;
        double offset = det * 1e-6;
        double lower  = 0 - offset;
        double upper  = det + offset;

        if ( (alpha >= lower ) && (beta >= lower) && (gamma >= lower) && (alpha <= upper) && (beta <= upper) && (gamma <= upper) && (lambda >= lower) && (lambda <= upper))
        {
          return true;
        }
        else
          return false;
      }
    }

  }


  template<typename PointAccessorT, typename ElementT, typename PointT, typename NumericConfigT>
  bool element_line_intersect(PointAccessorT const point_accessor,
                      ElementT const & element,
                      PointT const & ray_start,
                      PointT const & ray_direction,
                      NumericConfigT nc)
  {
    typedef typename viennagrid::result_of::element_tag<ElementT>::type ElementTag;
    typedef viennagrid::dimension_tag< result_of::static_size<PointT>::value > DimensionTag;

    return detail::element_line_intersect_impl(point_accessor,
                                              element,
                                              ElementTag(), DimensionTag(),
                                              ray_start, ray_direction,
                                              nc);
  }


  template<typename ElementT, typename PointT, typename NumericConfigT>
  bool element_line_intersect( ElementT const & element,
                      PointT const & ray_start,
                      PointT const & ray_direction,
                      NumericConfigT nc)
  {
    return element_line_intersect( default_point_accessor(element), element, ray_start, ray_direction, nc );
  }
}








////////////////////////////////////////////////////////////////////////
//                       extract boundary.hpp                         //
////////////////////////////////////////////////////////////////////////
namespace viennagrid
{


  template<typename ConnectorTagT, typename MeshT, typename ElementT, typename HullIDAccessor>
  void recursively_mark_neighbours( MeshT const & mesh,
                                    ElementT const & element,
                                    int hull_id,
                                    HullIDAccessor & hull_id_accessor)
  {
    if (!viennagrid::is_boundary(mesh, element))
      return;

    if (hull_id_accessor(element) != -1)
      return;

    hull_id_accessor(element) = hull_id;

    typedef typename viennagrid::result_of::const_neighbor_range<MeshT, ElementT, ConnectorTagT>::type NeighbourRangeType;
    typedef typename viennagrid::result_of::iterator<NeighbourRangeType>::type NeighbourRangeIterator;

    NeighbourRangeType neighbors(mesh, element);
    for (NeighbourRangeIterator neit = neighbors.begin(); neit != neighbors.end(); ++neit)
      recursively_mark_neighbours<ConnectorTagT>(mesh, *neit, hull_id, hull_id_accessor);
  }


  template<typename IDT>
  bool is_direct_child(std::vector<IDT> const & parent_to_test, std::vector<IDT> child_to_test, IDT current)
  {
    typename std::vector<IDT>::iterator it = std::find(child_to_test.begin(), child_to_test.end(), current);
    if (it != child_to_test.end())
      child_to_test.erase(it);
    return parent_to_test == child_to_test;
  }


  template<typename BoundaryElementTypeOrTagT, typename MeshT, typename HolePointContainerT>
  void extract_hole_points(MeshT const & mesh, HolePointContainerT & hole_points)
  {
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::coord<MeshT>::type CoordType;

    typedef typename viennagrid::result_of::element_tag<BoundaryElementTypeOrTagT>::type BoundaryElementTag;
    typedef typename viennagrid::result_of::element<MeshT, BoundaryElementTag>::type BoundaryElementType;
    typedef typename viennagrid::result_of::const_element_range<MeshT, BoundaryElementTag>::type ConstBoundaryRangeType;
    typedef typename viennagrid::result_of::iterator<ConstBoundaryRangeType>::type ConstBoundaryIteratorType;

    ConstBoundaryRangeType boundary_elements(mesh);

    std::vector<int> hull_id_container( boundary_elements.size(), -1 );
    typename viennagrid::result_of::accessor<std::vector<int>, BoundaryElementType>::type hull_id_accessor(hull_id_container);

    int num_hulls = 0;
    for (ConstBoundaryIteratorType beit = boundary_elements.begin(); beit != boundary_elements.end(); ++beit)
    {
      if (hull_id_accessor(*beit) != -1)
        continue;

      if (!viennagrid::is_boundary(mesh, *beit))
        continue;

      typedef typename viennagrid::result_of::facet_tag<BoundaryElementType>::type ConnectorTagT;
      recursively_mark_neighbours<ConnectorTagT>(mesh, *beit, num_hulls++, hull_id_accessor);
    }

    CoordType mesh_size = viennagrid::mesh_size(mesh);

    std::vector< std::vector<int> > hull_parents( num_hulls );

    for (int i = 0; i < num_hulls; ++i)
    {
      // finding an element which is in hull i
      ConstBoundaryIteratorType beit = boundary_elements.begin();
      for (; beit != boundary_elements.end(); ++beit)
      {
        if (hull_id_accessor(*beit) == i)
          break;
      }

      PointType centroid = viennagrid::centroid(*beit);
      PointType normal = viennagrid::normal_vector(*beit);
      normal /= viennagrid::norm_2(normal);
      normal *= mesh_size;

      for (int j = 0; j < num_hulls; ++j)
      {
        if (i == j)
          continue;

        int intersect_count = 0;
        for (ConstBoundaryIteratorType beit2 = boundary_elements.begin(); beit2 != boundary_elements.end(); ++beit2)
        {
          if (hull_id_accessor(*beit2) == j)
          {
            if (element_line_intersect(*beit2, centroid, centroid+normal, 1e-8))
              ++intersect_count;
          }
        }

        if (intersect_count % 2 == 1)
          hull_parents[i].push_back(j);
      }
    }

    for (int i = 0; i < num_hulls; ++i)
      std::sort(hull_parents[i].begin(), hull_parents[i].end());

    std::vector< std::vector<int> > direct_hull_children( num_hulls );
    for (int child = 0; child < num_hulls; ++child)
    {
      for (int parent = 0; parent < num_hulls; ++parent)
      {
        if (child == parent)
          continue;

        std::vector<int> diff;
        std::set_difference( hull_parents[child].begin(), hull_parents[child].end(),
                             hull_parents[parent].begin(), hull_parents[parent].end(),
                             std::back_inserter(diff) );

        if (diff.size() == 1 && diff[0] == parent)
          direct_hull_children[parent].push_back(child);
      }
    }

    for (int hull = 0; hull < num_hulls; ++hull)
    {
      if (hull_parents[hull].size() % 2 != 0)
      {
        bool found_hole_point = false;
        for (ConstBoundaryIteratorType beit = boundary_elements.begin(); beit != boundary_elements.end(); ++beit)
        {
          if (hull_id_accessor(*beit) == hull)
          {

            for (ConstBoundaryIteratorType beit2 = boundary_elements.begin(); beit2 != boundary_elements.end(); ++beit2)
            {
              if (beit2.handle() == beit.handle())
                continue;

              int j = 0;
              for (; j < direct_hull_children[hull].size(); ++j)
              {
                if (hull_id_accessor(*beit2) == direct_hull_children[hull][j])
                  break;
              }

              if (j == direct_hull_children[hull].size() && hull_id_accessor(*beit2) != hull)
                continue;

              PointType centroid0 = viennagrid::centroid(*beit);
              PointType centroid1 = viennagrid::centroid(*beit2);

              ConstBoundaryIteratorType beit3 = boundary_elements.begin();
              for (; beit3 != boundary_elements.end(); ++beit3)
              {
                if (!viennagrid::is_boundary(mesh, *beit3))
                  continue;

                if (beit3.handle() == beit.handle() || beit3.handle() == beit2.handle())
                  continue;

                if (viennagrid::element_line_intersect(*beit3, centroid0, centroid1, 1e-8))
                  break;
              }

              if (beit3 == boundary_elements.end())
              {
                hole_points.push_back( (centroid0+centroid1)/2.0 );
                found_hole_point = true;
                break;
              }
            }

            if (found_hole_point)
              break;
          }
        }

        assert( found_hole_point );
      }
    }
  }


  template<typename MeshT, typename HolePointContainerT>
  void extract_hole_points(MeshT const & mesh, HolePointContainerT & hole_points)
  {
    typedef typename viennagrid::result_of::facet_tag<MeshT>::type FacetTag;
    extract_hole_points<FacetTag>(mesh, hole_points);
  }
}

#endif
