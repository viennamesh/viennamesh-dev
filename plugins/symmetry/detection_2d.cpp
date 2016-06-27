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

#include "detection_2d.hpp"
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/centroid.hpp"



// see paper "On Symmetry Detection" by Mikhail J. Atallah, 1984


namespace viennamesh
{

  template<typename IteratorT>
  std::vector<double> string_from_points_on_circle(IteratorT begin, IteratorT end)
  {
//     typedef typename std::iterator_traits<IteratorT>::value_type NumericType;

    IteratorT prev = begin;
    IteratorT it = begin; ++it;

    std::vector<double> string;

    string.push_back(-1.0);

    for (; it != end; ++prev, ++it)
    {
      string.push_back( *it - *prev );
      string.push_back( *it - *prev );

      string.push_back(-1.0);
      string.push_back(-1.0);
    }

    string.push_back( 2*M_PI + *begin - *prev);
    string.push_back( 2*M_PI + *begin - *prev);

    string.push_back(-1.0);

    return string;
  }



  template<typename NumericConfigT>
  double angle(viennagrid::point const & p, NumericConfigT nc)
  {
    double a;
    if ( viennagrid::detail::is_equal(nc, p[0], 0.0) )
      a = (p[1] > 0) ? (M_PI/2.0) : (M_PI*3.0/2.0);
    else
    {
      a = std::atan(p[1]/p[0]);
      if (p[0] < 0)
        a += M_PI;

      if (a < 0)
        a += 2*M_PI;
    }

    return a;
  }

  template<typename ContainerT>
  typename ContainerT::value_type const & cyclic_access(ContainerT const & container, int index)
  {
    index = index % container.size();

    if (index < 0)
      index += container.size();

    return container[index];
  }



  template<typename ContainerT, typename NumericConfigT>
  bool is_palindrome(ContainerT const & container, int rotate, NumericConfigT nc)
  {
    for (int i = 0; i != container.size()/2; ++i)
    {
      int i0 = i+rotate;
      int i1 = container.size()-1-i+rotate;

      if (!viennagrid::detail::is_equal(nc, cyclic_access(container, i0), cyclic_access(container, i1)))
        return false;
    }

    return true;
  }


  template<typename ContainerT, typename NumericConfigT>
  bool is_equal(ContainerT const & container, int rotate, NumericConfigT nc)
  {
    for (std::size_t i = 0; i != container.size(); ++i)
    {
      std::size_t  i0 = i;
      std::size_t  i1 = i+rotate;

      if (!viennagrid::detail::is_equal(nc, cyclic_access(container, i0), cyclic_access(container, i1)))
        return false;
    }

    return true;
  }



  template<typename IteratorT, typename NumericConfigT>
  std::vector<double> mirror_symmetries(IteratorT points_begin, IteratorT points_end,
                                        std::vector<double> const & string, NumericConfigT nc)
  {
    std::vector<double> result;

    for (std::size_t i = 0; i < string.size()/4; ++i)
    {
      if (is_palindrome(string, 2*i, nc))
      {
        double axis_angle;

        IteratorT it = points_begin;
        if (i % 2 == 0)
        {
          std::advance(it, i/2);
          axis_angle = (*it);
        }
        else
        {
          std::advance(it, i/2);
          axis_angle = (*it);
          ++it;
          if (it != points_end)
            axis_angle += (*it);
          else
            axis_angle += (*points_begin);

          axis_angle /= 2.0;
        }

        if (axis_angle > M_PI)
          axis_angle -= M_PI;

        result.push_back(axis_angle);
      }
    }

    return result;
  }


  template<typename NumericConfigT>
  std::vector<int> rotational_symmetries(std::vector<double> const & string, NumericConfigT nc)
  {
    std::vector<int> result;

    for (std::size_t i = 2; i < string.size(); ++i)
    {
      if ( (string.size()/2) % i != 0 )
        continue;

      int freq = string.size()/i;

      if (is_equal(string, freq, nc))
        result.push_back(i);
    }

    return result;
  }


  template<typename PointIteratorT, typename NumericConfigT>
  void detect_symmetries_points(PointIteratorT begin, PointIteratorT end, NumericConfigT nc,
                                std::vector<double> & mirror_axis, std::vector<int> & rotational_frequencies)
  {
    typedef double NumericType;
    typedef typename std::iterator_traits<PointIteratorT>::value_type PointType;

    PointType centroid = viennagrid::make_point(0,0);
    int point_count = 0;

    for (PointIteratorT it = begin; it != end; ++it)
    {
      centroid += *it;
      ++point_count;
    }
    centroid /= static_cast<NumericType>(point_count);


    std::multimap<NumericType, PointType> sorted_points;
    for (PointIteratorT it = begin; it != end; ++it)
      sorted_points.insert( std::make_pair(viennagrid::norm_2(*it-centroid), *it-centroid) );

    double old_distance = -1;
    std::vector< std::vector<NumericType> > circles;
    for (typename std::multimap<NumericType, PointType>::iterator it = sorted_points.begin(); it != sorted_points.end(); ++it)
    {
      if (it->first > old_distance * (1.0+viennagrid::detail::absolute_tolerance<double>(nc)))
        circles.push_back( std::vector<NumericType>() );

      old_distance = it->first;
      circles.back().push_back( angle(it->second, nc) );
    }

    for (std::size_t i = 0; i != circles.size(); ++i)
      std::sort( circles[i].begin(), circles[i].end() );

    for (std::size_t i = 0; i != circles.size(); ++i)
    {
      std::vector<double> string = string_from_points_on_circle(circles[i].begin(), circles[i].end());

//       std::cout << "Circle " << i << std::endl;
//       std::cout << "  angles: ";
//       std::copy(circles[i].begin(), circles[i].end(), std::ostream_iterator<double>(std::cout, ","));
//       std::cout << std::endl;


      std::vector<double> local_mirror_axis = mirror_symmetries(circles[i].begin(), circles[i].end(), string, nc);
      std::sort(local_mirror_axis.begin(), local_mirror_axis.end());

      if (i == 0)
        mirror_axis = local_mirror_axis;
      else
      {
        for (std::vector<double>::iterator git = mirror_axis.begin(); git != mirror_axis.end();)
        {
          std::vector<double>::iterator lit = mirror_axis.begin();

          for (; lit != mirror_axis.end(); ++lit)
          {
            if (viennagrid::detail::is_equal(nc, *git, *lit))
              break;
          }

          if (lit == local_mirror_axis.end())
            git = mirror_axis.erase(git);
          else
            ++git;
        }
      }



      std::vector<int> local_rotational_symmetries = rotational_symmetries(string, nc);
      std::sort( local_rotational_symmetries.begin(), local_rotational_symmetries.end() );
      if (i == 0)
        rotational_frequencies = local_rotational_symmetries;
      else
      {
        std::vector<int> tmp;
        std::set_intersection( rotational_frequencies.begin(), rotational_frequencies.end(),
                               local_rotational_symmetries.begin(), local_rotational_symmetries.end(),
                               std::back_inserter(tmp) );
        rotational_frequencies = tmp;
      }
    }

    std::cout << "Centroid: " << centroid << std::endl;

    std::cout << "Global mirror axis: " << std::endl;
    for (std::size_t i = 0; i != mirror_axis.size(); ++i)
      std::cout << "  " << mirror_axis[i] << std::endl;

    std::cout << "Global rotational frequencies: " << std::endl;
    for (std::size_t i = 0; i != rotational_frequencies.size(); ++i)
      std::cout << "  " << rotational_frequencies[i] << std::endl;

  }






  struct line_triple
  {
    line_triple() {}

    template<typename LineT, typename NumericConfigT>
    line_triple(LineT const & line,
                point const & centroid,
                NumericConfigT nc)
    {
      p1 = viennagrid::get_point(line, 0) - centroid;
      p2 = viennagrid::get_point(line, 1) - centroid;

      l = viennagrid::volume(line);
      d1 = viennagrid::norm_2( p1 );
      d2 = viennagrid::norm_2( p2 );

      if (d2 < d1)
      {
        std::swap(d1,d2);
        std::swap(p1,p2);
      }

      angle_d1 = angle(p1, nc);
      angle_d2 = angle(p2, nc);

//       std::cout << line << std::endl;
//       std::cout << "  angle_d1 = " << angle_d1 << " angle_d2 = " << angle_d2 << "   left-right? " << std::boolalpha << left() << std::endl;
    }

    bool left() const
    {
      double tmp = angle_d2 - angle_d1;
      if (tmp > M_PI)
        tmp = 2*M_PI - tmp;
      if (tmp < -M_PI)
        tmp = 2*M_PI - tmp;
      return tmp > 0;
    }

    double l;
    double angle_d1;
    double angle_d2;
    double d1;
    double d2;
    point p1;
    point p2;
  };


  struct line_triple_sort
  {
    bool operator()(line_triple const & lhs, line_triple const & rhs)
    {
      return lhs.angle_d1 < rhs.angle_d1;
    }
  };



  template<typename IteratorT>
  std::vector<double> string_from_lines(IteratorT begin, IteratorT end)
  {
//     typedef typename std::iterator_traits<IteratorT>::value_type NumericType;

    IteratorT prev = begin;
    IteratorT it = begin; ++it;

    std::vector<double> string;

    string.push_back( (*prev).left() ? -1.0 : -2.0 );

    for (; it != end; ++prev, ++it)
    {
      string.push_back( (*it).angle_d1 - (*prev).angle_d1 );
      string.push_back( (*it).angle_d1 - (*prev).angle_d1 );

      string.push_back( (*it).left() ? -1.0 : -2.0 );
    }

    string.push_back( 2*M_PI + (*begin).angle_d1 - (*prev).angle_d1);
    string.push_back( 2*M_PI + (*begin).angle_d1 - (*prev).angle_d1);

    return string;
  }





  template<typename ContainerT, typename NumericConfigT>
  bool is_line_palindrome(ContainerT const & container, int rotate, NumericConfigT nc)
  {
    for (int i = 0; i != container.size()/2; ++i)
    {
      int i0 = i+rotate;
      int i1 = container.size()-1-i+rotate;

      double v0 = cyclic_access(container, i0);
      double v1 = cyclic_access(container, i1);

      if (v0*v1 < 0)
        return false;

      if (v0 < 0 && v1 < 0)
      {
        if (v0+v1 < -3.5 || -2.5 < v0+v1)
          return false;
      }
      else
        if (!viennagrid::detail::is_equal(nc, v0, v1))
          return false;
    }

    return true;
  }

  template<typename IteratorT, typename NumericConfigT>
  std::vector<double> mirror_symmetries_lines(IteratorT triples_begin, IteratorT triples_end,
                                              std::vector<double> const & string, NumericConfigT nc)
  {
    std::vector<double> result;

    for (std::size_t i = 2; i < string.size()/2; i+=3)
    {
      if (is_line_palindrome(string, i, nc))
      {
//         std::cout << "Found line palindrom " << i << std::endl;

        double axis_angle;
        IteratorT it = triples_begin;
        std::advance(it, (i-2)/3);
        axis_angle = (*it).angle_d1;
        ++it;
        if (it != triples_end)
          axis_angle += (*it).angle_d1;
        else
          axis_angle += (*triples_begin).angle_d1;

        axis_angle /= 2.0;


//         std::cout << "  angle = " << axis_angle << std::endl;

        if (axis_angle > M_PI)
          axis_angle -= M_PI;

        result.push_back(axis_angle);
      }
    }

    return result;
  }





  template<typename LineIteratorT, typename NumericConfigT>
  void detect_symmetries_lines(LineIteratorT lines_begin, LineIteratorT lines_end,
                               NumericConfigT nc,
                               point & centroid,
                               std::vector<double> & mirror_axis, std::vector<int> & rotational_frequencies)
  {
    typedef double NumericType;
    typedef point PointType;

    mirror_axis.clear();
    rotational_frequencies.clear();

    std::set<PointType> points;

    PointType centroid_lines = viennagrid::make_point(0,0);
    int line_count = 0;
    double line_centroid_weight_sum = 0.0;
    for (LineIteratorT it = lines_begin; it != lines_end; ++it)
    {
      double line_centroid_weight = viennagrid::volume(*it);
      centroid_lines += viennagrid::centroid(*it) * line_centroid_weight;
      line_centroid_weight_sum += line_centroid_weight;
      ++line_count;

      points.insert( viennagrid::get_point(*it,0) );
      points.insert( viennagrid::get_point(*it,1) );
    }
    centroid_lines /= line_centroid_weight_sum;


    centroid = viennagrid::make_point(0,0);
    for (std::set<PointType>::const_iterator it = points.begin(); it != points.end(); ++it)
      centroid += *it;
    centroid /= static_cast<NumericType>(points.size());

//     if ( !viennagrid::detail::is_equal(nc, centroid, centroid_lines) )
//     {
//
//
// //       return;
//     }


    std::vector< std::vector<line_triple> > line_triples;
    for (LineIteratorT it = lines_begin; it != lines_end; ++it)
    {
      line_triple triple(*it, centroid, nc);

      std::size_t i = 0;
      for (; i != line_triples.size(); ++i)
      {
        if ( viennagrid::detail::is_equal(nc, line_triples[i].front().l, triple.l) &&
             viennagrid::detail::is_equal(nc, line_triples[i].front().d1, triple.d1) &&
             viennagrid::detail::is_equal(nc, line_triples[i].front().d2, triple.d2) )
        {
          line_triples[i].push_back( triple );
          break;
        }
      }

      if (i == line_triples.size())
      {
        line_triples.push_back( std::vector<line_triple>() );
        line_triples.back().push_back( triple );
      }
    }

//     std::cout << "Found " << line_triples.size() << " different triple class(es)" << std::endl;


    for (std::size_t i = 0; i != line_triples.size(); ++i)
    {
      std::vector<line_triple> & lts = line_triples[i];
      std::sort( lts.begin(), lts.end(), line_triple_sort() );

      line_triple const & lt = lts.front();

      std::vector<double> local_mirror_axis;
      std::vector<int> local_rotational_symmetries;

      if ( viennagrid::detail::is_equal(nc, lt.d1, lt.d2) ||
           (viennagrid::detail::is_equal(nc, lt.d1, lt.d2) && viennagrid::detail::is_equal(nc, lt.l, lt.d2-lt.d1)) )
      {
        std::vector<double> point_angles;
        for (std::size_t j = 0; j != lts.size(); ++j)
          point_angles.push_back( angle((lts[j].p1+lts[j].p2)/2.0, nc) );
        std::sort(point_angles.begin(), point_angles.end());

        std::vector<double> string = string_from_points_on_circle(point_angles.begin(), point_angles.end());
        local_mirror_axis = mirror_symmetries(point_angles.begin(), point_angles.end(), string, nc);
        local_rotational_symmetries = rotational_symmetries(string, nc);
      }
      else
      {
        std::vector<double> string = string_from_lines(lts.begin(), lts.end());

//         std::cout << "  string = ";
//         std::copy( string.begin(), string.end(), std::ostream_iterator<double>(std::cout,",") );
//         std::cout << std::endl;

        local_mirror_axis = mirror_symmetries_lines(lts.begin(), lts.end(), string, nc);
        local_rotational_symmetries = rotational_symmetries(string, nc);
      }

      std::sort(local_mirror_axis.begin(), local_mirror_axis.end());
      if (i == 0)
        mirror_axis = local_mirror_axis;
      else
      {
        for (std::vector<double>::iterator git = mirror_axis.begin(); git != mirror_axis.end();)
        {
          std::vector<double>::iterator lit = mirror_axis.begin();

          for (; lit != mirror_axis.end(); ++lit)
          {
            if (viennagrid::detail::is_equal(nc, *git, *lit))
              break;
          }

          if (lit == local_mirror_axis.end())
            git = mirror_axis.erase(git);
          else
            ++git;
        }
      }

      std::sort( local_rotational_symmetries.begin(), local_rotational_symmetries.end() );
      if (i == 0)
        rotational_frequencies = local_rotational_symmetries;
      else
      {
        std::vector<int> tmp;
        std::set_intersection( rotational_frequencies.begin(), rotational_frequencies.end(),
                               local_rotational_symmetries.begin(), local_rotational_symmetries.end(),
                               std::back_inserter(tmp) );
        rotational_frequencies = tmp;
      }
    }


//     std::cout << "Global mirror axis: " << std::endl;
//     for (std::size_t i = 0; i != mirror_axis.size(); ++i)
//       std::cout << "  " << mirror_axis[i] << std::endl;
//
//     std::cout << "Global rotational frequencies: " << std::endl;
//     for (std::size_t i = 0; i != rotational_frequencies.size(); ++i)
//       std::cout << "  " << rotational_frequencies[i] << std::endl;

  }






  symmetry_detection_2d::symmetry_detection_2d() {}
  std::string symmetry_detection_2d::name() { return "symmetry_detection_2d"; }

  bool symmetry_detection_2d::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    int geometric_dimension = viennagrid::geometric_dimension( input_mesh() );

    if (geometric_dimension != 2)
      return false;


//     std::vector<PointType> points;
//
//     typedef viennagrid::result_of::const_vertex_range<MeshType>::type ConstVertexRange;
//     typedef viennagrid::result_of::iterator<ConstVertexRange>::type ConstVertexIterator;
//     ConstVertexRange vertices( input_mesh() );
//     for (ConstVertexIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
//       points.push_back( viennagrid::get_point(*vit) );
//
//     std::vector<double> mirror_axis_points;
//     std::vector<int> rotational_frequencies_points;
//     detect_symmetries_points( points.begin(), points.end(), 1e-6, mirror_axis_points, rotational_frequencies_points );

    typedef viennagrid::mesh                                                  MeshType;
    typedef point                                                             PointType;
    typedef viennagrid::result_of::const_element_range<MeshType, 1>::type     ConstLineRange;
    ConstLineRange lines( input_mesh() );

    std::vector<double> mirror_axis;
    std::vector<int> rotational_frequencies;
    PointType centroid;
    detect_symmetries_lines( lines.begin(), lines.end(), 1e-6, centroid, mirror_axis, rotational_frequencies );




    data_handle<double> output_mirror_axis = make_data<double>();
    data_handle<int> output_rotational_frequencies = make_data<int>();


    output_mirror_axis.resize( mirror_axis.size() );
    info(1) << " Found global mirror axis: " << std::endl;
    for (std::size_t i = 0; i != mirror_axis.size(); ++i)
    {
      info(1) << "  " << mirror_axis[i] << std::endl;
      output_mirror_axis.set(i, mirror_axis[i]);
    }


    output_rotational_frequencies.resize( rotational_frequencies.size() );
    info(1) << "Found global rotational frequencies: " << std::endl;
    for (std::size_t i = 0; i != rotational_frequencies.size(); ++i)
    {
      info(1) << "  " << rotational_frequencies[i] << std::endl;
      output_rotational_frequencies.set(i, rotational_frequencies[i]);
    }



    set_output( "centroid", centroid );
    set_output( "mirror_axis", output_mirror_axis );
    set_output( "rotational_frequencies", output_rotational_frequencies );

    return true;
  }

}
