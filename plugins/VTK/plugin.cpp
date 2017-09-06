#include "viennameshpp/plugin.hpp"
#include "vtk_mesh.hpp"
#include "vtk_decimate_pro.hpp"
#include "vtk_quadric_clustering.hpp"
#include "vtk_quadric_decimation.hpp"
#include "vtk_mesh_quality.hpp"

viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
    viennamesh::register_data_type<viennamesh::vtk::mesh>(context);

    viennamesh::register_conversion<viennagrid_mesh, viennamesh::vtk::mesh>(context);
    viennamesh::register_conversion<viennamesh::vtk::mesh, viennagrid_mesh>(context);

    viennamesh::register_algorithm<viennamesh::vtk::decimate_pro>(context);
    viennamesh::register_algorithm<viennamesh::vtk::quadric_clustering>(context);
    viennamesh::register_algorithm<viennamesh::vtk::quadric_decimation>(context);
    viennamesh::register_algorithm<viennamesh::vtk::mesh_quality>(context);

    return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
    return VIENNAMESH_VERSION;
}
