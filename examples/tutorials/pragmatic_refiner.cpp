/*
/ PRAGMATIC_REFINER
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
	int refinement_passes = 0;
	std::string filename;

	if (argc < 3)
	{
		std::cout << "Parameters missing!" << std::endl;
		std::cout << "Correct use of parameters: <refinement_passes> <filename>" << std::endl;
		return -1;
	}

	else if (atoi(argv[1]))
	{
		refinement_passes = atoi(argv[1]);
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
	viennamesh::algorithm_handle pragmatic_refine = context.make_algorithm("pragmatic_refine");
	pragmatic_refine.set_default_source(mesh_reader);
	//pragmatic_refine.set_input("region_count", region_count);
	pragmatic_refine.set_input("refinement_passes", refinement_passes);
	pragmatic_refine.set_input("input_file", filename);
	pragmatic_refine.run();

	// Write mesh to file
	viennamesh::algorithm_handle mesh_writer = context.make_algorithm("mesh_writer");
	mesh_writer.set_default_source(pragmatic_refine);
  mesh_writer.set_input( "filename", "/home/lgnam/Desktop/pragmatic_refined_mesh.vtu" );
  mesh_writer.run();

    	return 0;
}
