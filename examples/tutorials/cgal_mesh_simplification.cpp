#include "viennameshpp/core.hpp"


/*
* Simplifies a given mesh by using CGAL's triangulated surface mesh simplification capabilities. The choice of cost and placement policies is essential for the
* resulting coarsened mesh quality.
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



	viennamesh::algorithm_handle coarser = context.make_algorithm("cgal_mesh_simplification");
	coarser.set_default_source(mesh_reader); // is equivalent to coarser.set_input("mesh", mesh_reader.get_output("mesh"));

    //sets stop edge ratio, if the coarsened mesh has less edges, simplification is stopped
    coarser.set_input("stop_predicate", "ratio");
    coarser.set_input("ratio", 0.1);


    /* possible cost policies: lindstrom-turk, edgelength
    * possible placement policies: lindstrom-turk, midpoint
    * N.B.: if lindstrom-turk is chosen, the volume, boundary and shape weights should be defined.
    * Their default values are 0.5, 0.5, 0, resp., but lead for most meshes to poor quality
    */

    coarser.set_input("cost_policy", "lindstrom-turk");
    coarser.set_input("placement_policy", "lindstrom-turk");

    coarser.set_input("lindstrom_volume_weight", 0.1);    //how important is volume preservation?
    coarser.set_input("lindstrom_boundary_weight", 0.5);  //how important is boundary preservation?
    coarser.set_input("lindstrom_shape_weight", 0.4);     //how important is cell shape quality?

    /* Geometric feature can be tried to keep preserved by marking edges whose neighboring triangles' surface normals have an angle greater than 'feature_angle' to each other as being constraint.
     * N.B: This is EXPERIMENTAL, very long coarsening times and poor mesh quality for low stop edge ratios can be the case.
     */

    //coarser.set_input("feature_angle", 0.45);


    //start coarsening
	coarser.run();


    //the resulting coarsened mesh is written to a vtu file.
    viennamesh::algorithm_handle mesh_writer = context.make_algorithm("mesh_writer");
    mesh_writer.set_default_source(coarser);
    mesh_writer.set_input( "filename", "my_simple_mesh.vtu" );
    mesh_writer.run();

    return 0;
}
