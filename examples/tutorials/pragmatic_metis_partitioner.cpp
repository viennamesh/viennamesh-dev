/*
/ PRAGMATIC_METIS_PARTITIONING
/
/ 
/
/ Developer: 	Lukas Gnam
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
	int region_count = 0;
	std::string filename;

	if (argc < 3)
	{
		std::cout << "Parameters missing!" << std::endl;
		std::cout << "Correct use of parameters: <input_file> <region_count> " << std::endl;
		return -1;
	}

	else if (!argv[1])
	{
		std::cout << "Parameter <input_file> missing!" << std::endl;
		return -1;
	}

	else if (!atoi(argv[2]))
	{
		std::cout << "Parameter <region_count> missing!" << std::endl;
		return -1;
	}

	else
	{
		filename = argv[1];
		region_count = atoi(argv[2]);
	}

	// Create context handle
	viennamesh::context_handle context;

	// Create algorithm handle for reading the mesh from a file and run it
	viennamesh::algorithm_handle mesh_reader = context.make_algorithm("mesh_reader");
	mesh_reader.set_input("filename", filename.c_str());
	mesh_reader.run();

	//create partitions on the pragmatic data structure
	viennamesh::algorithm_handle mesh_partitioner = context.make_algorithm("pragmatic_metis_partitioner");
	mesh_partitioner.set_default_source(mesh_reader);
	mesh_partitioner.set_input("filename", filename.c_str());
	mesh_partitioner.set_input("region_count", region_count);
	mesh_partitioner.set_input("multi_mesh_output", false);
	mesh_partitioner.run();
/*	
	//Write mesh
	viennamesh::algorithm_handle write_merged_mesh = context.make_algorithm("mesh_writer");
	write_merged_mesh.set_default_source(mesh_partitioner);
	write_merged_mesh.set_input("filename", "/home/lgnam/Desktop/software/ViennaMesh/viennamesh-dev/build/examples/data/pragmatic_metis_partitioning/parpartmesh.vtu");
	write_merged_mesh.run();
*/  

  return 0;
}
