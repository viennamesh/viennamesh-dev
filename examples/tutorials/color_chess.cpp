/*
/ COLOR_CHESS
/
/ 
/   Developer: 	Lukas Gnam
/		Institute for Microelectronics
/		Faculty of Electrical Engineering and Information Engineering
/		Vienna University of Technology
/		gnam@iue.tuwien.ac.at
/		2017
/
/
/
*/

//ViennaMesh includes
#include "viennameshpp/core.hpp"
#include <chrono>

int main(int argc, char **argv)
{
    
    // Check and read console input 
	int refinement_passes = 0;
	std::string filename;

	if (argc < 2)
	{
		std::cout << "Parameters missing!" << std::endl;
		std::cout << "Correct use of parameters: <filename>" << std::endl;
		return -1;
	}

	else if (argv[1])
	{
        filename = argv[1];
	}
	
	else
	{
		std::cout << "Correct use of parameters: <filename>" << std::endl;
		return -1;
	}

    viennamesh::context_handle context;

	auto total_tic = std::chrono::system_clock::now();

    //viennamesh reader
    viennamesh::algorithm_handle mesh_reader = context.make_algorithm("mesh_reader");
    mesh_reader.set_input( "filename", filename.c_str() );
    mesh_reader.run();

	auto tic = std::chrono::system_clock::now();

    //simple chessboard coloring algorithm
	viennamesh::algorithm_handle chess_coloring = context.make_algorithm("chessboard_coloring");
	chess_coloring.set_default_source(mesh_reader); 
    chess_coloring.run();

	std::chrono::duration<double> dur = std::chrono::system_clock::now() - tic;

	std::cout << dur.count() << std::endl;

    //viennamesh writer
    viennamesh::algorithm_handle mesh_writer = context.make_algorithm("mesh_writer");
    mesh_writer.set_default_source(chess_coloring);
    mesh_writer.set_input("quantities", chess_coloring.get_output("color_field"));
    mesh_writer.set_input("filename", "chessboard_mesh.vtu");
    mesh_writer.run();

	std::chrono::duration<double> total_dur = std::chrono::system_clock::now() - total_tic;
	std::cout << total_dur.count() << std::endl;

    return 0;
}
