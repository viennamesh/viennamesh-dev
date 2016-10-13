/*
/   PRAGMATIC_SMOOTHER
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
	std::string smoothing_algorithm;
	int smoothing_passes = 0;
	std::string filename;
  
  //TODO
  std::cout << "THIS EXAMPLE AND ITS CORRESPONDING PRAGMATIC ALGORITHM ARE NOT IMPLEMENTED YET" << std::endl;

	if (argc < 4)
	{
		std::cout << "Parameters missing!" << std::endl;
		std::cout << "Correct use of parameters: <smoothing_algorithm> <smoothing_passes> <filename>" << std::endl;
		return -1;
	}

	else if (argv[1])
	{
		smoothing_algorithm = argv[1];
		smoothing_passes = atoi(argv[2]);
		filename = argv[3];
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

	// Create algorithm handle for pragmatic smoothing, set its defalt source and addiational inputs and finally run it
	viennamesh::algorithm_handle pragmatic_smooth = context.make_algorithm("pragmatic_smooth");
	pragmatic_smooth.set_default_source(mesh_reader);
	pragmatic_smooth.set_input("smoothing_algorithm", smoothing_algorithm);
	pragmatic_smooth.set_input("refinement_passes", smoothing_passes);
	pragmatic_smooth.set_input("input_file", filename);
	pragmatic_smooth.run();

	// Write mesh to file
	viennamesh::algorithm_handle mesh_writer = context.make_algorithm("mesh_writer");
	mesh_writer.set_default_source(pragmatic_smooth);
  	mesh_writer.set_input( "filename", "/home/lgnam/Desktop/software/ViennaMesh/viennamesh-dev/build/examples/data/myfirsttask/pragmatic_smoothed_mesh.vtu" );
  	mesh_writer.run();

/*END OF VIENNAMESH FRAMEWORK PART*/

    	return 0;
}
