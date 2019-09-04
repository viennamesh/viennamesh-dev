#include "viennameshpp/plugin.hpp"
#include "vtp_mesh.hpp"
#include "vtk_decimate_pro.hpp"
#include "vtk_quadric_clustering.hpp"
#include "vtk_quadric_decimation.hpp"
#include "vtk_mesh_quality.hpp"
#include "vtk_merge_points.hpp"
#include "vtk_ug_mesh.hpp"

viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
    viennamesh::register_data_type<viennamesh::VTK_PolyData::mesh>(context);
    viennamesh::register_data_type<viennamesh::VTK_UnstructuredGrid::mesh>(context);

    viennamesh::register_conversion<viennagrid_mesh, viennamesh::VTK_PolyData::mesh>(context);
    viennamesh::register_conversion<viennamesh::VTK_PolyData::mesh, viennagrid_mesh>(context);
    viennamesh::register_conversion<viennagrid_mesh, viennamesh::VTK_UnstructuredGrid::mesh>(context);
    viennamesh::register_conversion<viennamesh::VTK_UnstructuredGrid::mesh, viennagrid_mesh>(context);

    viennamesh::register_algorithm<viennamesh::VTK_PolyData::decimate_pro>(context);
    viennamesh::register_algorithm<viennamesh::VTK_PolyData::quadric_clustering>(context);
    viennamesh::register_algorithm<viennamesh::VTK_PolyData::quadric_decimation>(context);
    viennamesh::register_algorithm<viennamesh::VTK_UnstructuredGrid::mesh_quality>(context);
    viennamesh::register_algorithm<viennamesh::VTK_PolyData::merge_points>(context);

    return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
    return VIENNAMESH_VERSION;
}
