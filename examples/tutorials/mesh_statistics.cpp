#include <string>
#include <iostream>

#include "viennameshpp/core.hpp"


/*
 * Prints the statistical quantities of an input mesh. If a second mesh is given, the input mesh is compared to the second and the comprehensive mesh quality metric can be calculated.
 *
 * Below additional examples for the usage of viennamesh capabilities is provided.

*/

int main(int argc, char **argv)
{

    if (argc > 3)
    {
        std::cout << "1 or 2 argument required: \n\t1) mesh to analyze\n\t2) Mesh Comparison: mesh to analyze, original mesh" << std::endl;
        return 0;
    }


    viennamesh::context_handle context;


    std::string meshPath(argv[1]);

    //mesh_reader algorithm reads the given mesh file and converts to viennagrid datastructure
    viennamesh::algorithm_handle mesh_reader = context.make_algorithm("mesh_reader");
    mesh_reader.set_input( "filename", meshPath ); //set path of mesh to analyze
    mesh_reader.run();

    viennamesh::algorithm_handle mesh_reader2;

    //reads the original mesh file and converts to viennagrid datastructure
    if(argc == 3)
    {
        std::string meshPath2(argv[2]);
        mesh_reader2 = context.make_algorithm("mesh_reader");
        mesh_reader2.set_input( "filename", meshPath2 );
        mesh_reader2.run();
    }


    //hull mesh statistics algorithm
    viennamesh::algorithm_handle stats = context.make_algorithm("make_statistic");

    //sets the resulting viennagrid mesh of mesh_reader as the input mesh for statistics algorithm
    stats.set_input("mesh", mesh_reader.get_output("mesh"));

    if(argc == 3)
    {
        stats.set_input("original_mesh", mesh_reader2.get_output("mesh"));

        //weighting factors for calculation of comprehensive mesh quality metric. All have to be given, otherwise default values are used.
        stats.set_input("alpha", 0.18);
        stats.set_input("beta", 8.82);
        stats.set_input("gamma", 0.88);
        stats.set_input("delta", 0.7);
    }

    //set cell shape quality metric type
    stats.set_input("metric_type", "radius_ratio");

    /* Further possible cell shape quality metrics:
     *      "radius_edge_ratio"
            "aspect_ratio"
            "min_angle"
            "max_angle"
            "min_dihedral_angle"
            "radius_ratio"
            "edge_ratio"
            "circum_perimeter_ratio"
            "skewness"
            "perimeter_inradius_ratio"
      */

    //start statistics calculations
    stats.run();



    //further examples for only one input mesh
    /*
    stats = context.make_algorithm("make_statistic");
    stats.set_default_source(mesh_reader);
    stats.set_input("metric_type", "aspect_ratio");

    //user defined threshold for high quality cell determination, if a second mesh (=original mesh) is given, the median value of the second mesh is implicitely the "good_element_threshold"
    stats.set_input("good_element_threshold",1.42);

    stats = context.make_algorithm("make_statistic");
    stats.set_default_source(mesh_reader);
    stats.set_input("metric_type", "edge_ratio");
    stats.set_input("good_element_threshold",1.73);

    stats = context.make_algorithm("make_statistic");
    stats.set_default_source(mesh_reader);
    stats.set_input("metric_type", "skewness");
    stats.set_input("good_element_threshold",0.5);
    */




    /*Additional viennamesh capabilities - Use the calculated mesh comparison metric in a local variable */

    if(argc == 3)
    {
        //outputs are provided enclosed in a data_handle
        viennamesh::data_handle<double> mesh_quality_metric_handle = stats.get_output<double>("mesh_quality_metric"); //template parameter is essential!

        //without template parameter (i.e., stats.get_output("mesh_quality_metric") ) an abstract data_handle is returned

        double mesh_quality_metric = mesh_quality_metric_handle(); //() operator provides access to output itself

        std::cout << "Local mesh quality metric = " << mesh_quality_metric << std::endl;
    }


    return 0;
}
