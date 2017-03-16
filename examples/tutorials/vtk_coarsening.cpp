#include "viennameshpp/core.hpp"

int main()
{
    viennamesh::context_handle context;
    /*
     * Viennamesh automatically loads all plugins in the plugin dir during the context constructor call.
     * You don't have to call: context.load_plugins_in_directory(VIENNAMESH_DEFAULT_PLUGIN_DIRECTORY);
     */

    viennamesh::algorithm_handle mesh_reader = context.make_algorithm("mesh_reader");
    mesh_reader.set_input( "filename", "examples/data/elephant.vtu");
    mesh_reader.run();

    /*
     * ###### vtkQuadricClustering ######
     *
     * Algorithm name:
     *   vtk_quadric_clustering
     *
     * Required input parameters:
     * - x_division                 (int)
     * - y_division                 (int)
     * - z_division                 (int)
     *
     * Optional input parameters:
     * - use_input_points           (int)
     * - use_feature_edges          (int)
     * - use_feature_points         (int)
     * - feature_points_angle       (double)
     * - use_internal_triangles     (int)
     * - copy_cell_data             (int)
     * - prevent_dublicate_cells    (int)
     *
     */
    viennamesh::algorithm_handle quadricClustering = context.make_algorithm("vtk_quadric_clustering");
    quadricClustering.set_default_source(mesh_reader);
    quadricClustering.set_input("x_division", 32);
    quadricClustering.set_input("y_division", 32);
    quadricClustering.set_input("z_division", 32);
    quadricClustering.set_input("use_feature_edges", 1);
    quadricClustering.run();

    /*
     * ###### vtkQuadricDecimation ######
     *
     * Algorithm name:
     *   vtk_quadric_decimation
     *
     * Required input parameters:
     * - reduction_percentage       (double)
     */
    viennamesh::algorithm_handle quadricDecimation = context.make_algorithm("vtk_quadric_decimation");
    quadricDecimation.set_default_source(mesh_reader);
    quadricDecimation.set_input("reduction_percentage", 0.7);
    quadricDecimation.run();

    /*
     * ###### vtkDecimatePro ######
     *
     * Algorithm name:
     *   vtk_decimate_pro
     *
     * Required input parameters:
     * - reduction_percentage       (double)
     *
     * Optional input parameters:
     * - preserve_topology          (int)
     * - feature_angle              (double)
     * - splitting                  (int)
     * - split_angle                (double)
     * - pre_spilt_mesh             (int)
     * - maximum_error              (double)
     * - accumulate_error           (int)
     * - error_is_absolute          (int)
     * - absulute_error             (double)
     * - boudary_vertex_deletion    (int)
     * - degree                     (int)
     * - inflection_point_ratio     (double)
     */
    viennamesh::algorithm_handle decimatePro = context.make_algorithm("vtk_decimate_pro");
    decimatePro.set_default_source(mesh_reader);
    decimatePro.set_input("reduction_percentage", 0.7);
    decimatePro.run();

    /*
     * ###### vtkMeshQuality ######
     *
     * Algorithm name:
     *   vtk_mesh_quality
     *
     * Output values:
     * - min_area                   (double)
     * - av_area                    (double)
     * - max_area                   (double)
     * - min_angle                  (double)
     * - av_min_angle               (double)
     * - av_aspect_ratio            (double)
     * - max_aspect_ratio           (double)
     * - cell_count                 (int)
     * - vertex_count               (int)
     */
    viennamesh::algorithm_handle meshQuality = context.make_algorithm("vtk_mesh_quality");
    meshQuality.set_default_source(mesh_reader);
    meshQuality.run();

    viennamesh::data_handle<double> minAngle = meshQuality.get_output<double>("min_angle");
    std::cout << "Minimum angle is: " << minAngle() << std::endl;

    viennamesh::data_handle<double> avMinAngle = meshQuality.get_output<double>("av_min_angle");
    std::cout << "Average minimum angle is: " << avMinAngle() << std::endl;

    // Create mesh wirte to write output mesh file
    viennamesh::algorithm_handle mesh_writer = context.make_algorithm("mesh_writer");

    // Write vtk_quadric_clustering output mesh
    mesh_writer.set_default_source(quadricClustering);
    mesh_writer.set_input( "filename", "vtk_quadric_clustering_output_mesh.vtu" );
    mesh_writer.run();

    // Write vtk_quadric_decimation output mesh
    mesh_writer.set_default_source(quadricDecimation);
    mesh_writer.set_input( "filename", "vtk_quadric_decimation_output_mesh.vtu" );
    mesh_writer.run();

    // Write vtk_decimate_pro output mesh
    mesh_writer.set_default_source(decimatePro);
    mesh_writer.set_input( "filename", "vtk_decimate_pro_output_mesh.vtu" );
    mesh_writer.run();

    return 0;
}
