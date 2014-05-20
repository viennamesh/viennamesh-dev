/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include "viennamesh/viennamesh.hpp"


int main()
{
  // creating an algorithm for reading a mesh from a file
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );

  // creating a meshing algorithm
  viennamesh::algorithm_handle mesher( new viennamesh::tetgen::make_mesh() );

  // creating a hull extraction algorithm
  viennamesh::algorithm_handle extract_hull( new viennamesh::extract_boundary() );

  // creating an algorithm for writing a mesh to a file
  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );

  // Setting the filename for the reader
  reader->set_input( "filename", "../data/cubeception.poly" );

  // using the reader algorithm as default source for the mesher
  mesher->set_default_source(reader);

  // using the mesher algorithm as default source for the extract hull
  extract_hull->set_default_source(mesher);

  // using the extract hull algorithm as default source for the writer
  writer->set_default_source(extract_hull);
  // Setting the filename for the writer
  writer->set_input( "filename", "half-trigate_hull.vtu" );

  // start the algorithms
  reader->run();
  mesher->run();
  extract_hull->run();
  writer->run();
}
