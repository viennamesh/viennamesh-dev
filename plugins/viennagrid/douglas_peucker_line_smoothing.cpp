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

#include "douglas_peucker_line_smoothing.hpp"
#include "viennagrid/algorithm/angle.hpp"


namespace viennamesh
{

  douglas_peucker_line_smoothing::douglas_peucker_line_smoothing() {}
  std::string douglas_peucker_line_smoothing::name() { return "douglas_peucker_line_smoothing"; }



  template<typename MeshT, typename ElementT>
  ElementT other_line(MeshT const & mesh, ElementT line, ElementT vertex)
  {
    typedef typename viennagrid::result_of::const_coboundary_range<MeshT>::type ConstCoboundaryElementRangeType;
    ConstCoboundaryElementRangeType lines(mesh, vertex, 1);
    return lines[0] == line ? lines[1] : lines[0];
  }

  template<typename MeshT, typename ElementT>
  ElementT other_vertex(MeshT const &, ElementT vertex, ElementT line)
  {
    typedef typename viennagrid::result_of::const_element_range<ElementT>::type ConstElementRangeType;
    ConstElementRangeType vertices(line, 0);
    return vertices[0] == vertex ? vertices[1] : vertices[0];
  }



  template<typename MeshT, typename ElementT, typename AccessorT, typename PolylineContainerT>
  void extract_polyline(MeshT const & mesh,
                        ElementT vertex, ElementT line,
                        AccessorT & polyline_ids,
                        PolylineContainerT & polylines,
                        viennagrid_numeric min_angle)
  {
    polylines.resize(polylines.size()+1);
    typename PolylineContainerT::value_type & polyline = polylines.back();

    while (true)
    {
      polyline_ids.set(line, true);

      ElementT next_vertex = other_vertex(mesh, vertex, line);
      polyline.push_back(next_vertex);
      if (viennagrid::coboundary_elements(mesh, next_vertex, 1).size() != 2)
        break;

      ElementT next_line = other_line(mesh, line, next_vertex);
      if (polyline_ids.get(next_line))
        break;

      ElementT nv2 = other_vertex(mesh, next_vertex, next_line);
      viennagrid_numeric angle = viennagrid::angle(viennagrid::get_point(vertex),
                                                   viennagrid::get_point(nv2),
                                                   viennagrid::get_point(next_vertex));
      if (angle < min_angle)
      {
        std::cout << "Angle = " << angle << " (min_angle=" << min_angle << ")" << std::endl;
        break;
      }

      line = next_line;
      vertex = next_vertex;
    }
  }



  viennagrid_numeric distance(viennagrid::point const & p,
                              viennagrid::point const & l1, viennagrid::point const & l2)
  {
    // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
    viennagrid_numeric x0 = p[0];
    viennagrid_numeric y0 = p[1];
    viennagrid_numeric x1 = l1[0];
    viennagrid_numeric y1 = l1[1];
    viennagrid_numeric x2 = l2[0];
    viennagrid_numeric y2 = l2[1];

    return std::abs((y2-y1)*x0 - (x2-x1)*y0 + x2*y1 - y2*x1) / std::sqrt((y2-y1)*(y2-y1) + (x2-x1)*(x2-x1)) ;
  }

  viennagrid_numeric distance(viennagrid::const_element const & vp,
                              viennagrid::const_element const & vl1, viennagrid::const_element const & vl2)
  {
    if (vl1 == vl2)
      return viennagrid::norm_2( viennagrid::get_point(vp) - viennagrid::get_point(vl1) );

    return distance( viennagrid::get_point(vp), viennagrid::get_point(vl1), viennagrid::get_point(vl2) );
  }





  template<typename ElementT>
  std::deque<ElementT> douglas_peucker(std::deque<ElementT> const & vertices,
                                       viennagrid_numeric eps)
  {
    if (vertices.size() == 2)
      return vertices;

    viennagrid_numeric max_distance = -1;
    std::size_t max_index = 0;
    for (std::size_t i = 1; i != vertices.size()-1; ++i)
    {
      viennagrid_numeric d = distance( vertices[i], vertices.front(), vertices.back() );
      if (d > max_distance)
      {
        max_distance = d;
        max_index = i;
      }
    }

    std::deque<ElementT> res;
    if (max_distance > eps)
    {
      std::deque<ElementT> res1 = douglas_peucker(std::deque<ElementT>(vertices.begin(), vertices.begin()+max_index+1),
                                                  eps);
      std::deque<ElementT> res2 = douglas_peucker(std::deque<ElementT>(vertices.begin()+max_index, vertices.end()),
                                                  eps);

      std::copy(res1.begin(), res1.end(), std::back_inserter(res));
      std::copy(res2.begin()+1, res2.end(), std::back_inserter(res));
    }
    else
    {
      res.push_back( vertices.front() );
      res.push_back( vertices.back() );
    }

    return res;
  }




  bool douglas_peucker_line_smoothing::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    data_handle<double> eps = get_required_input<double>("eps");
    data_handle<double> min_angle = get_required_input<double>("min_angle");

    mesh_handle output_mesh = make_data<mesh_handle>();

    typedef viennagrid::mesh MeshType;
    typedef viennagrid::result_of::element<MeshType>::type ElementType;
    typedef viennagrid::result_of::const_element_range<MeshType>::type ConstElementRangeType;
    typedef viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementRangeIterator;
    typedef viennagrid::result_of::const_coboundary_range<MeshType>::type ConstCoboundaryElementRangeType;
    typedef viennagrid::result_of::iterator<ConstCoboundaryElementRangeType>::type ConstCoboundaryElementRangeIterator;

    int line_count = viennagrid::elements(input_mesh(), 1).size();
    std::vector<bool> polyline_id_cont(line_count, false);
    viennagrid::result_of::accessor<std::vector<bool>, ElementType>::type polyline_ids(polyline_id_cont);

    std::vector< std::deque<ElementType> > polylines;

    ConstElementRangeType vertices(input_mesh(), 0);
    for (ConstElementRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      ConstCoboundaryElementRangeType lines(input_mesh(),*vit, 1);
      if (lines.size() == 2)
        continue;

      for (ConstCoboundaryElementRangeIterator lit = lines.begin(); lit != lines.end(); ++lit)
      {
        if (polyline_ids.get(*lit))
          continue;

        extract_polyline(input_mesh(), *vit, *lit, polyline_ids, polylines, min_angle());
        polylines.back().push_front(*vit);
      }
    }

    ConstElementRangeType lines(input_mesh(), 1);
    for (ConstElementRangeIterator lit = lines.begin(); lit != lines.end(); ++lit)
    {
      if (polyline_ids.get(*lit))
        continue;

      extract_polyline(input_mesh(), viennagrid::elements(*lit,0)[0], *lit, polyline_ids, polylines, min_angle());
      polylines.back().push_front(viennagrid::elements(*lit,0)[0]);

      std::deque<ElementType> & pl = polylines.back();
      int hs = polylines.back().size()/2;
      polylines.resize(polylines.size()+1);
      std::copy(pl.begin()+hs, pl.end(), std::back_inserter(polylines.back()));
      pl.erase(pl.begin()+hs+1, pl.end());
    }



    viennagrid::result_of::element_copy_map<>::type copy_map( output_mesh(), false );
    for (std::size_t i = 0; i != polylines.size(); ++i)
    {
      std::deque<ElementType> polyline = douglas_peucker(polylines[i], eps());

      ElementType ov = copy_map( polyline[0] );
      for (std::size_t j = 1; j != polyline.size(); ++j)
      {
        ElementType v = copy_map( polyline[j] );
        viennagrid::make_line(output_mesh(), ov, v);
        ov = v;
      }
    }

    set_output("mesh", output_mesh);


    return true;
  }

}
