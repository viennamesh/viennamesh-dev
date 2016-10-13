/*
/   PRAGMATIC_SWAPPER
/
/
/
/   Developer: 	Lukas Gnam
/		Institute for Microelectronics
/		Faculty of Electrical Engineering and Information Engineering
/		Vienna University of Technology
/		gnam@iue.tuwien.ac.at
/		2016
/
/
/
*/

#include <iostream>
#include <string>
#include <vector>
#include <cfloat>
#include <deque>
#include <cassert>
#include <cmath>

//ViennaMesh includes
#include "viennameshpp/core.hpp"

int main(int argc, char **argv)
{
	// Check and read console input 
	double parameter = 0;
	std::string filename;

	if (argc < 3)
	{
		std::cout << "Parameters missing!" << std::endl;
		std::cout << "Correct use of parameters:  <parameter> <filename>" << std::endl;
		return -1;
	}

	else if (atof(argv[1]))
	{
		parameter = atof(argv[1]);
    filename = argv[2];
	}

	else if (!argv[2])
	{
		std::cout << "Parameter <input_file> missing!" << std::endl;
		return -1;
	}
	
	else
	{
		std::cout << "Wrong type of input parameter <region count>" << std::endl;
		return -1;
	}

	// Create context handle
	viennamesh::context_handle context;

	// Create algorithm handle for reading the mesh from a file and run it
	viennamesh::algorithm_handle mesh_reader = context.make_algorithm("mesh_reader");
	mesh_reader.set_input("filename", filename.c_str());
	mesh_reader.run();

	// Create algorithm handle for pragmatic refining, set its defalt source and addiational inputs and finally run it
	viennamesh::algorithm_handle pragmatic_swap = context.make_algorithm("pragmatic_swapping");
	pragmatic_swap.set_default_source(mesh_reader);
	pragmatic_swap.set_input("parameter", parameter);
	pragmatic_swap.set_input("input_file", filename);
	pragmatic_swap.run();

	// Write mesh to file
	viennamesh::algorithm_handle mesh_writer = context.make_algorithm("mesh_writer");
	mesh_writer.set_default_source(pragmatic_swap);
  mesh_writer.set_input( "filename", "/home/lgnam/Desktop/software/ViennaMesh/viennamesh-dev/build/examples/data/myfirsttask/pragmatic_swapped_mesh.vtu" );
  mesh_writer.run();

  return 0;
}
