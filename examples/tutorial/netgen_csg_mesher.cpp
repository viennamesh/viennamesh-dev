#include "viennamesh/viennamesh.hpp"


int main()
{
  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::algorithm_handle mesher( new viennamesh::netgen::csg_make_mesh() );

  // Define the CSG string
  std::string csg_string =
  "algebraic3d \n"
  "solid cube = plane (0, 0, 0; 0, 0, -1) \n"
  "       and plane (0, 0, 0; 0, -1, 0) \n"
  "       and plane (0, 0, 0; -1, 0, 0) \n"
  "       and plane (1, 1, 1; 0, 0, 1) \n"
  "       and plane (1, 1, 1; 0, 1, 0) \n"
  "       and plane (1, 1, 1; 1, 0, 0); \n"
  "tlo cube;\n";


  // setting the mesher paramters
  mesher->set_input( "csg", csg_string );               // the CSG string

  mesher->set_input( "delaunay", true );                    // use delaunay meshing
  mesher->set_input( "cell_size", 0.1 );                    // set the cell size
  mesher->set_input( "grading", 0.3 );                      // set the element grading, 0...1 (0 => uniform mesh; 1 => aggressive local grading)
  mesher->set_input( "optimization_steps", 3 );             // set the number of optimization steps for 3-D mesh optimization
  mesher->set_input( "optimize_string", "cmdmustm" );       // set the optimization strategy:
                                                            // m .. move nodes
                                                            // M .. move nodes, cheap functional
                                                            // s .. swap faces
                                                            // t .. swap and improve faces
                                                            // c .. combine elements
                                                            // d .. divide elements
                                                            // p .. plot, no pause
                                                            // P .. plot, Pause
                                                            // h .. Histogramm, no pause
                                                            // H .. Histogramm, pause
                                                            // t .. ignored

  // start the algorithm
  mesher->run();




  // creating an algorithm for writing a mesh to a file
  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );

  // linking the output from the mesher to the writer
  writer->set_default_source(mesher);

  // Setting the filename for the reader and writer
  writer->set_input( "filename", "cube_netgen_csg.vtu" );

  // start the algorithm
  writer->run();
}
