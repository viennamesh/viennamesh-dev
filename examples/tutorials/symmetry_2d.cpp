#include <string>
#include <deque>
#include <vector>

#include <cassert>
#include <iostream>

#include "viennameshpp/core.hpp"


int main()
{
  viennamesh::context_handle context;


  typedef viennagrid::mesh MeshType;
  viennamesh::data_handle<viennagrid_mesh> mesh = context.make_data<viennagrid_mesh>();

  typedef viennagrid::result_of::point<MeshType>::type    PointType;
  typedef viennagrid::result_of::element<MeshType>::type  VertexType;

  std::vector<PointType> points;
  std::vector<VertexType> vertices;

  double gamma = (1.0+std::sqrt(5.0))/2.0;
  double gamma_squared = gamma*gamma;
  double pi = M_PI;


  points.push_back( viennagrid::make_point(0, 1) );
  points.push_back( viennagrid::make_point( -std::sin(pi/5.0)/gamma_squared, std::cos(pi/5.0)/gamma_squared) );
  points.push_back( viennagrid::make_point( -std::cos(pi/10.0), std::sin(pi/10.0) ));
  points.push_back( viennagrid::make_point( -std::cos(pi/10.0)/gamma_squared, -std::sin(pi/10.0)/gamma_squared ));
  points.push_back( viennagrid::make_point( -std::sin(pi/5.0), -std::cos(pi/5.0) ));
  points.push_back( viennagrid::make_point(0, -1.0/gamma_squared));
  points.push_back( viennagrid::make_point( std::sin(pi/5.0), -std::cos(pi/5.0) ));
  points.push_back( viennagrid::make_point( std::cos(pi/10.0)/gamma_squared, -std::sin(pi/10.0)/gamma_squared ));
  points.push_back( viennagrid::make_point( std::cos(pi/10.0), std::sin(pi/10.0) ));
  points.push_back( viennagrid::make_point( std::sin(pi/5.0)/gamma_squared, std::cos(pi/5.0)/gamma_squared) );

//   for (std::size_t i = 0; i < points.size(); ++i)
//     points[i] += viennagrid::make_point(0,1);

//   for (std::size_t i = 0; i < points.size(); ++i)
//     points[i][0] *= 2.0;

  for (std::size_t i = 0; i < points.size(); ++i)
    vertices.push_back( viennagrid::make_vertex( mesh(), points[i] ) );

  for (std::size_t i = 0; i != vertices.size()-1; ++i)
    viennagrid::make_line( mesh(), vertices[i], vertices[i+1] );
  viennagrid::make_line( mesh(), vertices.back(), vertices.front() );


  viennamesh::algorithm_handle symmetry_detection = context.make_algorithm("symmetry_detection_2d");
  symmetry_detection.set_input( "mesh", mesh );
  symmetry_detection.run();



  viennamesh::algorithm_handle mesh_writer = context.make_algorithm("mesh_writer");
  mesh_writer.set_input( "mesh", mesh );
//   mesh_writer.set_default_source(extract_boundary);
  mesh_writer.set_input( "filename", "symmetry.vtu" );
  mesh_writer.run();

  return -1;
}
