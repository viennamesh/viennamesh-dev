#include "viennamesh/basic_data.hpp"

namespace viennamesh
{
//   void convert( PointContainerType const & points, viennamesh_point_container internal_points )
//   {
//     if (points.empty())
//       return;
//
//     int geometric_dimension = points[0].size();
//
//     std::vector<double> point_buffer;
//     for (std::size_t i = 0; i != points.size(); ++i)
//     {
//       if (static_cast<int>(points[i].size()) != geometric_dimension)
//       {
//         // TODO throw
//         return;
//       }
//
//       std::copy( points[i].begin(), points[i].end(), std::back_inserter(point_buffer) );
//     }
//
//     viennamesh_point_container_set(internal_points, &point_buffer[0],
//                                    geometric_dimension, points.size() );
//   }

//   void convert( viennamesh_point_container internal_points, PointContainerType & points )
//   {
//     int geometric_dimension = 0;
//     int point_count = 0;
//     double * point_data = 0;
//     if (internal_points)
//       viennamesh_point_container_get(internal_points, &point_data,
//                                       &geometric_dimension, &point_count);
//
//     points.resize(point_count);
//
//     for (int i = 0; i != point_count; ++i)
//     {
//       point_t p(geometric_dimension);
//       std::copy( point_data+geometric_dimension*i, point_data+geometric_dimension*(i+1), p.begin() );
//       points[i] = p;
//     }
//   }


//   void convert( viennagrid::point_t const & point, viennamesh_point internal_point )
//   {
//     viennamesh_point_set(internal_point, const_cast<double*>(&point[0]), point.size());
//   }
//
//   void convert( viennamesh_point internal_point, viennagrid::point_t & point )
//   {
//     int size = 0;
//     double * values = 0;
//     if (internal_point)
//       viennamesh_point_get(internal_point, &values, &size);
//
//     point.resize(size);
//     std::copy( values, values+size, point.begin() );
//   }
//
//
//
// 
//   void convert( seed_point_t const & seed_point, viennamesh_seed_point internal_seed_point )
//   {
//     viennamesh_seed_point_set(internal_seed_point,
//                               const_cast<viennagrid_numeric*>(&seed_point.first[0]), seed_point.first.size(),
//                               seed_point.second);
//   }
//
//   void convert( viennamesh_seed_point internal_seed_point, seed_point_t & seed_point )
//   {
//     viennagrid_numeric * values;
//     int size;
//     int region;
//
//     viennamesh_seed_point_get(internal_seed_point, &values, &size, &region);
//
//     seed_point.first.resize(size);
//     std::copy( values, values+size, seed_point.first.begin() );
//     seed_point.second = region;
//   }



//   void convert( SeedPointContainerType const & seed_points, viennamesh_seed_point_container internal_seed_points )
//   {
//     if (seed_points.empty())
//       return;
//
//     int geometric_dimension = seed_points[0].first.size();
//
//     std::vector<double> point_buffer;
//     std::vector<int> regions;
//
//     for (std::size_t i = 0; i != seed_points.size(); ++i)
//     {
//       if (static_cast<int>(seed_points[i].first.size()) != geometric_dimension)
//       {
//         // TODO throw
//         return;
//       }
//
//       std::copy( seed_points[i].first.begin(), seed_points[i].first.end(), std::back_inserter(point_buffer) );
//       regions.push_back( seed_points[i].second );
//     }
//
//     viennamesh_seed_point_container_set(internal_seed_points, &point_buffer[0], &regions[0],
//                                         geometric_dimension, seed_points.size() );
//   }
//
//
//
//   void convert( viennamesh_seed_point_container internal_seed_points, SeedPointContainerType & seed_points )
//   {
//     int geometric_dimension = 0;
//     int point_count = 0;
//     double * point_data = 0;
//     int * seed_point_regions = 0;
//     if (internal_seed_points)
//       viennamesh_seed_point_container_get(internal_seed_points, &point_data, &seed_point_regions, &geometric_dimension, &point_count);
//
//     seed_points.resize(point_count);
//
//     for (int i = 0; i != point_count; ++i)
//     {
//       point_t p(geometric_dimension);
//       std::copy( point_data+geometric_dimension*i, point_data+geometric_dimension*(i+1), p.begin() );
//       seed_points[i] = std::make_pair(p, seed_point_regions[i]);
//     }
//   }
}





