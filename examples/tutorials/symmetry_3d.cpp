#include <string>
#include <deque>
#include <vector>

#include <cassert>
#include <iostream>

#include "viennameshpp/core.hpp"


int main()
{
  viennamesh::context_handle context;
//   context.load_plugins_in_directory(VIENNAMESH_DEFAULT_PLUGIN_DIRECTORY);


  viennamesh::algorithm_handle mesh_reader = context.make_algorithm("mesh_reader");
//   mesh_reader.set_input( "filename", "../data/cube.poly" );
  mesh_reader.set_input( "filename", "../data/cube_with_tunnel.poly" );
  mesh_reader.run();


  viennamesh::algorithm_handle mesher = context.make_algorithm("triangle_make_hull");
  mesher.set_default_source(mesh_reader);
  mesher.run();


  viennamesh::algorithm_handle symmetry_detection = context.make_algorithm("symmetry_detection_3d");
  symmetry_detection.set_default_source(mesher);
  symmetry_detection.run();


  viennamesh::algorithm_handle mesh_writer = context.make_algorithm("mesh_writer");
  mesh_writer.set_default_source(symmetry_detection);
  mesh_writer.set_input( "filename", "sphere.vtu" );
  mesh_writer.run();

  return -1;
}
