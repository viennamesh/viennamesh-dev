#include "viennamesh/viennamesh.hpp"


int main( int argc, char** argv )
{
  if (argc != 3)
  {
    std::cout << "Usage: nld_mesh_quad_hull <input_line_mesh> <output_triangle_mesh>" << std::endl;
    return -1;
  }


  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );
  viennamesh::algorithm_handle linear_transform( new viennamesh::linear_transform::algorithm() );
  viennamesh::algorithm_handle mesher( new viennamesh::triangle::algorithm() );
  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );

  // linking the output from the reader to the mesher
  linear_transform->link_input( "default", reader, "default" );
  mesher->link_input( "default", linear_transform, "default" );
  writer->link_input( "default", mesher, "default" );

  std::vector<double> matrix(3*2, 0);
  matrix[0*3+0] = 1;
  matrix[1*3+1] = 1;
  linear_transform->set_input( "matrix", matrix );



  viennamesh::seed_point_2d_container seed_points;
  typedef viennamesh::point_2d PointType;
  seed_points.push_back( std::make_pair( PointType(2.01501e-07, -2.98603e-07), 0 ) );
  seed_points.push_back( std::make_pair( PointType(2.60261e-09, 7.60161e-08), 1 ) );
  seed_points.push_back( std::make_pair( PointType(4.05243e-06, -2.98603e-07), 2 ) );
  seed_points.push_back( std::make_pair( PointType(8.62697e-07, -1.59858e-07), 3 ) );
  seed_points.push_back( std::make_pair( PointType(2.60261e-09, -2.98603e-07), 4 ) );
  seed_points.push_back( std::make_pair( PointType(5.0011e-06, -2.98603e-07), 5 ) );
  seed_points.push_back( std::make_pair( PointType(3.65202e-06, -2.98603e-07), 6 ) );
  seed_points.push_back( std::make_pair( PointType(2.60261e-09, 1.50095e-05), 7 ) );

  mesher->set_input( "seed_points", seed_points );



  // setting the mesher paramters
//   mesher->set_input( "cell_size", 1.0 );      // maximum cell size
  mesher->set_input( "min_angle", 0.35 );     // minimum angle in radiant, 0.35 are about 20 degrees
  mesher->set_input( "delaunay", true  );     // we want a Delaunay triangulation
  mesher->set_input( "algorithm_type", "incremental_delaunay" );  // incremental Delaunay algorithm is used


  // linking the output from the mesher to the writer
  reader->set_input( "filename", argv[1] );
  writer->set_input( "filename", argv[2] );


  // start the algorithms
  reader->run();
  linear_transform->run();
  mesher->run();
  writer->run();
}
