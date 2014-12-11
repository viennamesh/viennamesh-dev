#include "viennamesh/basic_data.hpp"

namespace viennamesh
{
  void convert( PointContainerType const & points, viennamesh_point_container internal_points )
  {
    if (points.empty())
      return;

    int geometric_dimension = points[0].size();

    std::vector<double> point_buffer;
    for (std::size_t i = 0; i != points.size(); ++i)
    {
      if (points[i].size() != geometric_dimension)
      {
        // TODO throw
        return;
      }

      std::copy( points[i].begin(), points[i].end(), std::back_inserter(point_buffer) );
    }

    viennamesh_point_container_set(internal_points, &point_buffer[0],
                                   geometric_dimension, points.size() );
  }

  void convert( viennamesh_point_container internal_points, PointContainerType & points )
  {
    int geometric_dimension = 0;
    int point_count = 0;
    double * point_data = 0;
    if (internal_points)
      viennamesh_point_container_get(internal_points, &point_data,
                                      &geometric_dimension, &point_count);

    points.resize(point_count);

    for (int i = 0; i != point_count; ++i)
    {
      point_t p(geometric_dimension);
      std::copy( point_data+geometric_dimension*i, point_data+geometric_dimension*(i+1), p.begin() );
      points[i] = p;
    }
  }


  void convert( point_t const & point, viennamesh_point_container internal_points )
  {
    PointContainerType tmp;
    tmp.push_back(point);
    convert(tmp, internal_points);
  }

  void convert( viennamesh_point_container internal_points, point_t & point )
  {
    PointContainerType tmp;
    convert(internal_points, tmp);
    if (tmp.size() != 1)
    {
      // throw
      return;
    }
    point = tmp[0];
  }


  void convert( SeedPointContainerType const & seed_points, viennamesh_seed_point_container internal_seed_points )
  {
    if (seed_points.empty())
      return;

    int geometric_dimension = seed_points[0].first.size();

    std::vector<double> point_buffer;
    std::vector<int> regions;

    for (std::size_t i = 0; i != seed_points.size(); ++i)
    {
      if (seed_points[i].first.size() != geometric_dimension)
      {
        // TODO throw
        return;
      }

      std::copy( seed_points[i].first.begin(), seed_points[i].first.end(), std::back_inserter(point_buffer) );
      regions.push_back( seed_points[i].second );
    }

    viennamesh_seed_point_container_set(internal_seed_points, &point_buffer[0], &regions[0],
                                        geometric_dimension, seed_points.size() );
  }



  void convert( viennamesh_seed_point_container internal_seed_points, SeedPointContainerType & seed_points )
  {
    int geometric_dimension = 0;
    int point_count = 0;
    double * point_data = 0;
    int * seed_point_regions = 0;
    if (internal_seed_points)
      viennamesh_seed_point_container_get(internal_seed_points, &point_data, &seed_point_regions, &geometric_dimension, &point_count);

    seed_points.resize(point_count);

    for (int i = 0; i != point_count; ++i)
    {
      point_t p(geometric_dimension);
      std::copy( point_data+geometric_dimension*i, point_data+geometric_dimension*(i+1), p.begin() );
      seed_points[i] = std::make_pair(p, seed_point_regions[i]);
    }
  }
}





