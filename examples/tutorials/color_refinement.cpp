//ViennaMesh includes
#include "viennameshpp/core.hpp"

int main(int argc, char *argv[])
//int main()
{
    // Check and read console input 
	int region_count = 0;
	int num_threads = 0;
	std::string filename;
	std::string algorithm;
	std::string options;
	std::string coloring;
	int max_num_iterations = 0;

	if (argc < 5)
	{
		std::cout << "Parameters missing!" << std::endl;
		std::cout << "Correct use of parameters: <input_file> <region_count> <number_threads> <algorithm>" << std::endl;
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
		std::cout << "Parameter <num_threads> missing!" << std::endl;
		return -1;
	}

	else
	{
		filename = argv[1];
		region_count = atoi(argv[2]);
		num_threads = atoi(argv[3]);
	}

	if (argv[4])
	{
		coloring = argv[4];
	}

	if (argv[5])
	{
		algorithm = argv[5];
	}

	if(!atoi(argv[6]))
	{
		std::cout << "Parameter <max_num_iterations> missing!" << std::endl;
		return -1;
	}

	else
	{
		max_num_iterations = atoi(argv[6]);
	}

	if (argv[7])
	{
		options = argv[7];
	}

    // Create context handle
	viennamesh::context_handle context;

	// Create algorithm handle for reading the mesh from a file and run it
	viennamesh::algorithm_handle mesh_reader = context.make_algorithm("mesh_reader");
	mesh_reader.set_input("filename", filename.c_str());
	//mesh_reader.set_input("filename", "examples/data/half-trigate_0.vtu");
	mesh_reader.run();

	// Create algorithm handle for the color-based-refinement
	viennamesh::algorithm_handle color = context.make_algorithm("color_refinement");
	color.set_default_source(mesh_reader);
	color.set_input("coloring", coloring);
	color.set_input("algorithm", algorithm);
	color.set_input("options", options);
	color.set_input("num_partitions", region_count);
	color.set_input("filename", filename.c_str());
	color.set_input("num_threads", num_threads);
	color.set_input("single_mesh_output", true);
	color.set_input("max_num_iterations", max_num_iterations);
	color.run();
/*
	//Consistency check
	viennamesh::algorithm_handle consistency_check = context.make_algorithm("consistency_check");
	consistency_check.set_default_source(color);
	consistency_check.run();
*/

	//VTK merge points
	viennamesh::algorithm_handle merger = context.make_algorithm("vtk_merge_points");
	merger.set_default_source(color);
	merger.run();//*/

/*
	//Make statistic
	viennamesh::algorithm_handle stats = context.make_algorithm("make_statistic");
	stats.set_default_source(color);
	//stats.set_input("histogram_bin", 1.0);
	stats.set_input("histogram_min", 0.0);
	stats.set_input("histogram_max", 3.0);
	stats.set_input("histogram_bin_count", 30);
	stats.set_input("metric_type", "radius_ratio");
	stats.run();//*/
	
/*
	//VTK mesh quality
	viennamesh::algorithm_handle mesh_quality = context.make_algorithm("vtk_mesh_quality");
	mesh_quality.set_default_source(merger);
	mesh_quality.set_input("EdgeRatio", true);
	mesh_quality.run();//*/
	
	/*
	//Write unmerged output mesh
	viennamesh::algorithm_handle mesh_writer = context.make_algorithm("mesh_writer");
	mesh_writer.set_default_source(color);
	
	//construct filename
	size_t found = filename.find_last_of("/");
	size_t find_vtu = filename.find_last_of(".");
	std::string output_file = "examples/data/color_refinement/output/";
	output_file+=filename.substr(found+1, find_vtu-found-1);
	output_file+="_";
	output_file+=algorithm;
	output_file+="_";
	output_file+=std::to_string(region_count);
	output_file+="partitions_";
	output_file+=std::to_string(num_threads);
	output_file+="threads_";
	output_file+=std::to_string(max_num_iterations);
	output_file+="iterations_refineboundary";
	output_file+=".vtu";

	//mesh_writer.set_input("filename", "pragmatic.vtu");
	mesh_writer.set_input("filename", output_file.c_str());
	mesh_writer.run();//*/

	//Write merged output mesh
	viennamesh::algorithm_handle mesh_writer_merged = context.make_algorithm("mesh_writer");
	mesh_writer_merged.set_default_source(merger);
	
	//construct filename
	size_t found_merged = filename.find_last_of("/");
	size_t find_merged_vtu = filename.find_last_of(".");
	std::string output_file_merged = "examples/data/color_refinement/output/";
	output_file_merged+=filename.substr(found_merged+1, find_merged_vtu-found_merged-1);
	output_file_merged+="_";
	output_file_merged+=algorithm;
	output_file_merged+="_";
	output_file_merged+=std::to_string(region_count);
	output_file_merged+="partitions_";
	output_file_merged+=std::to_string(num_threads);
	output_file_merged+="threads_";
	output_file_merged+=std::to_string(max_num_iterations);
	output_file_merged+="iterations_refineboundary_merged";
	output_file_merged+=".vtu";

	//mesh_writer.set_input("filename", "pragmatic.vtu");
	mesh_writer_merged.set_input("filename", output_file_merged.c_str());
	mesh_writer_merged.run();//*/
//*/
    return -1;
}