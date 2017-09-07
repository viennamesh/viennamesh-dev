#include <string>
#include <iostream>

#include "viennameshpp/core.hpp"

int main(int argc, char **argv)
{

    viennamesh::context_handle context;


    std::string meshPath(argv[1]);

    //mesh_reader algorithm reads the given mesh file and converts to viennagrid datastructure
    viennamesh::algorithm_handle mesh_reader = context.make_algorithm("mesh_reader");
    mesh_reader.set_input( "filename", meshPath ); //set path of mesh to analyze
    mesh_reader.run();

    //hull mesh statistics algorithm
    viennamesh::algorithm_handle stats = context.make_algorithm("mesh_information");
    stats.set_input("mesh", mesh_reader.get_output("mesh"));
    stats.run();

    return 0;
}
