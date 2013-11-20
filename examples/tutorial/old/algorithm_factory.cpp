#include <iostream>
#include <map>

#include "viennamesh/forwards.hpp"

#include "viennamesh/algorithm/tetgen_tetrahedron_mesher.hpp"
#include "viennamesh/algorithm/file_reader.hpp"
#include "viennamesh/algorithm/file_writer.hpp"
#include "viennamesh/core/algorithm_factory.hpp"


int main()
{
  viennamesh::AlgorithmFactory factory;

  factory.register_algorithm< viennamesh::FileReader >();
  viennamesh::set_feature< viennamesh::FileReader >( "type", "reader" );

  factory.register_algorithm< viennamesh::Algorithm<viennamesh::tetgen_tetrahedron_tag> >();
  viennamesh::set_feature< viennamesh::Algorithm<viennamesh::tetgen_tetrahedron_tag> >( "type", "meshing" );
  viennamesh::set_feature< viennamesh::Algorithm<viennamesh::tetgen_tetrahedron_tag> >( "meshing.type", "creation" );
  viennamesh::set_feature< viennamesh::Algorithm<viennamesh::tetgen_tetrahedron_tag> >( "meshing.algorithm", "incremental_delaunay" );
  viennamesh::set_feature< viennamesh::Algorithm<viennamesh::tetgen_tetrahedron_tag> >( "meshing.geometric_dimension", "3" );
  viennamesh::set_feature< viennamesh::Algorithm<viennamesh::tetgen_tetrahedron_tag> >( "meshing.input_cell_type", "plc, triangle" );
  viennamesh::set_feature< viennamesh::Algorithm<viennamesh::tetgen_tetrahedron_tag> >( "meshing.output_cell_type", "tetrahedron" );

  factory.register_algorithm< viennamesh::FileWriter >();
  viennamesh::set_feature< viennamesh::FileWriter >( "type", "writer" );












  viennamesh::AlgorithmHandle reader = factory.create_from_expression( "type==reader" );
  viennamesh::AlgorithmHandle mesher = factory.create_from_expression( "(type==mesh_creation) && (meshing.algorithm=delauney) && (meshing.geometric_dimension==3))" ); // XML ???
  viennamesh::AlgorithmHandle writer = factory.create_from_expression( "type==writer" );



  // linking the output from the reader to the mesher
  mesher->link_input( "default", reader, "default" );
  mesher->link_input( "seed_points", reader, "seed_points" );
  mesher->link_input( "hole_points", reader, "hole_points" );
  // set the cell size parameter for the mesher
  mesher->set_input( "cell_size", 1.0 );

  // linking the output from the mesher to the writer
  writer->link_input( "default", mesher, "default" );


  // Setting the filename for the reader and writer
  reader->set_input( "filename", "../data/big_and_small_cube.poly" );
  writer->set_input( "filename", "big_and_small_cube.vtu" );

  // start the algorithms
  reader->run();
  mesher->run();
  writer->run();

}
