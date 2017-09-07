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
  mesh_reader.set_input( "filename", "examples/data/cross_2d_brep.vtu" );
//   mesh_reader.set_input( "filename", "../data/half-trigate.mesh" );
  mesh_reader.run();


  viennamesh::algorithm_handle mesher = context.make_algorithm("triangle_make_mesh");
  mesher.set_default_source(mesh_reader);
  mesher.run();
/*
  viennamesh::algorithm_handle mesh_writer = context.make_algorithm("mesh_writer");
  mesh_writer.set_default_source(mesher);
  mesh_writer.set_input( "filename", "triangle.vtu" );
  mesh_writer.run();
  */
  return -1;
}
