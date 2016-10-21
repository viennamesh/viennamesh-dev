#include "viennameshpp/core.hpp"


/*
* Simplifies a given mesh by using CGAL's triangulated surface mesh simplification capabilities. A high quality coarsening setup is found automatically by
* the cost of multiple coarsening iterations.
*/


int main(int argc, char **argv)
{

    if (argc != 2)
    {
        std::cout << "1 argument required: mesh to simplifiy" << std::endl;
        return 0;
    }


    std::string meshPath(argv[1]);

    viennamesh::context_handle context;
//   context.load_plugins_in_directory(VIENNAMESH_DEFAULT_PLUGIN_DIRECTORY);


    //viennamesh reader
    viennamesh::algorithm_handle mesh_reader = context.make_algorithm("mesh_reader");
    mesh_reader.set_input( "filename", meshPath );
    mesh_reader.run();



    viennamesh::algorithm_handle auto_coarser = context.make_algorithm("cgal_automatic_mesh_simplification");
    auto_coarser.set_default_source(mesh_reader);

    //required input
    auto_coarser.set_input("stop_predicate", "ratio");
    auto_coarser.set_input("ratio", 0.1);

    // also possible:
    //auto_coarser.set_input("stop_predicate", "count");
    //auto_coarser.set_input("ratio", 600);

    //comprehensive mesh quality weighting factors (optional)
    auto_coarser.set_input("alpha", 0.4);
    auto_coarser.set_input("beta", 20);
    auto_coarser.set_input("gamma", 1.0);
    auto_coarser.set_input("delta", 1.3);

    /* Geometric feature can be tried to keep preserved by marking edges whose neighboring triangles' surface normals have an angle greater than 'feature_angle' to each other as being constraint.
     * N.B: This is EXPERIMENTAL, EXTREMELY LONG coarsening times and poor mesh quality for low stop edge ratios can be the case.
     */
    //auto_coarser.set_input("feature_angle", 0.45);

    //start coarsening
    auto_coarser.run();


    viennamesh::algorithm_handle mesh_writer = context.make_algorithm("mesh_writer");
    mesh_writer.set_default_source(auto_coarser);
    mesh_writer.set_input( "filename", "auto_simple_mesh.vtu" );
    mesh_writer.run();

    return 0;
}
