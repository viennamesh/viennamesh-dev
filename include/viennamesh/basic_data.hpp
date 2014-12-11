#ifndef _VIENNAMESH_BASIC_DATA_HPP_
#define _VIENNAMESH_BASIC_DATA_HPP_

#include "viennamesh/forwards.hpp"
#include <string>

typedef struct viennamesh_string_t * viennamesh_string;
typedef struct viennamesh_point_container_t * viennamesh_point_container;
typedef struct viennamesh_seed_point_container_t * viennamesh_seed_point_container;
typedef struct viennagrid_mesh_ * viennagrid_mesh;


namespace viennamesh
{
  typedef std::vector<point_t> PointContainerType;
  typedef std::vector< std::pair<point_t, int> > SeedPointContainerType;

  void convert( PointContainerType const & points, viennamesh_point_container internal_points );
  void convert( viennamesh_point_container internal_points, PointContainerType & points );

  void convert( point_t const & point, viennamesh_point_container internal_points );
  void convert( viennamesh_point_container internal_points, point_t & point );

  void convert( SeedPointContainerType const & seed_points, viennamesh_seed_point_container internal_seed_points );
  void convert( viennamesh_seed_point_container internal_seed_points, SeedPointContainerType & seed_points );



  namespace result_of
  {
    template<typename T>
    struct data_information;

    template<>
    struct data_information<bool>
    {
      static std::string type_name() { return "bool"; }
      static std::string local_binary_format() { return ""; }
    };

    template<>
    struct data_information<int>
    {
      static std::string type_name() { return "int"; }
      static std::string local_binary_format() { return ""; }
    };

    template<>
    struct data_information<double>
    {
      static std::string type_name() { return "double"; }
      static std::string local_binary_format() { return ""; }
    };

    template<>
    struct data_information<viennamesh_string>
    {
      static std::string type_name() { return "viennamesh_string"; }
      static std::string local_binary_format() { return ""; }
    };

    template<>
    struct data_information<viennamesh_point_container>
    {
      static std::string type_name() { return "viennamesh_point_container"; }
      static std::string local_binary_format() { return ""; }
    };

    template<>
    struct data_information<viennamesh_seed_point_container>
    {
      static std::string type_name() { return "viennamesh_seed_point_container"; }
      static std::string local_binary_format() { return ""; }
    };

    template<>
    struct data_information<viennagrid_mesh>
    {
      static std::string type_name() { return "viennagrid_mesh"; }
      static std::string local_binary_format() { return ""; }
    };
  }
}

#endif
