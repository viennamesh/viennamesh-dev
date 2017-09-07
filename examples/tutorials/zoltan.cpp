#include <iostream>
#include <vector>

//ViennaMesh includes
#include "viennameshpp/core.hpp"

int main(int argc, char *argv[])
{
    // Create context handle
	viennamesh::context_handle context;

    // Create algorithm handle for reading the mesh from a file and run it
	viennamesh::algorithm_handle zoltan = context.make_algorithm("zoltan_partitioning");
    zoltan.set_input("argc", argc);
	zoltan.run();

    std::cerr << "check message" << std::endl;

    return 0;
}