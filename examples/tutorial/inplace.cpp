#include "viennamesh/viennamesh.hpp"
#include "viennagrid/io/vtk_writer.hpp"


int main()
{
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );
  reader->set_input( "filename", "../data/test.bnd" );
  reader->run();


  viennamesh::algorithm_handle transform( new viennamesh::affine_transform() );

  viennamesh::dynamic_point matrix(9, 0.0);

  double angle = 1.0;
  double scale = 1.0;

  matrix[0] = std::cos(angle) * scale;
  matrix[1] = -std::sin(angle) * scale;
  matrix[3] = std::sin(angle) * scale;
  matrix[4] = std::cos(angle) * scale;
  matrix[8] = scale;

  viennamesh::parameter_handle mesh = reader->get_output("mesh");

  transform->set_input( "matrix", matrix );
  transform->set_input( "mesh", mesh );
  transform->set_output( "mesh", mesh );
  transform->run();


  viennamesh::algorithm_handle writer0( new viennamesh::io::mesh_writer() );
  writer0->set_default_source(reader);
  writer0->set_input( "filename", "inplace_reader.vtu" );
  writer0->run();

  viennamesh::algorithm_handle writer1( new viennamesh::io::mesh_writer() );
  writer1->set_default_source(transform);
  writer1->set_input( "filename", "inplace_transform.vtu" );
  writer1->run();
}
