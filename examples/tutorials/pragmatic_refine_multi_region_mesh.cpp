/*
/ PRAGMATIC_REFINE_MULTI_REGION_MESH
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
	int refinement_passes = 0;
	std::string filename;

	if (argc < 4)
	{
		std::cout << "Parameters missing!" << std::endl;
		std::cout << "Correct use of parameters: <input_file> <region_count> <refinement_passes>" << std::endl;
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

	else if (!atoi(argv[3]))
	{
		std::cout << "Parameter <refinement_passes> missing!" << std::endl;
		return -1;
	}	

	else
	{
		filename = argv[1];
		region_count = atoi(argv[2]);
		refinement_passes = atoi(argv[3]);
	}

  // Create context handle
	viennamesh::context_handle context;

	// Create algorithm handle for reading the mesh from a file and run it
	viennamesh::algorithm_handle mesh_reader = context.make_algorithm("mesh_reader");
	mesh_reader.set_input("filename", filename.c_str());
	mesh_reader.run();

  // Create algorithm handle for mesh partitioning, set its default source and input, and run it
	viennamesh::algorithm_handle mesh_partitioner = context.make_algorithm("metis_mesh_partitioning");
	mesh_partitioner.set_default_source(mesh_reader);
	mesh_partitioner.set_input("region_count", region_count);
	mesh_partitioner.set_input("multi_mesh_output", false);
	mesh_partitioner.run();

  //Create volume mesh using pragmatic refinement multi region algorithm
	viennamesh::algorithm_handle pragmatic_multi = context.make_algorithm("pragmatic_refine_multi_region");
	pragmatic_multi.set_default_source(mesh_partitioner);
  pragmatic_multi.set_input("refinement_passes", refinement_passes);
	pragmatic_multi.run();

  //Merge mesh
	viennamesh::algorithm_handle mesh_merger = context.make_algorithm("merge_meshes");
	mesh_merger.set_default_source(mesh_partitioner);
	mesh_merger.run();

	//Write mesh
	viennamesh::algorithm_handle write_merged_mesh = context.make_algorithm("mesh_writer");
	write_merged_mesh.set_default_source(mesh_merger);
	write_merged_mesh.set_input("filename", "/home/lgnam/Desktop/software/ViennaMesh/viennamesh-dev/build/examples/parpartmesh.vtu");
	write_merged_mesh.run();

  return 0;
}
