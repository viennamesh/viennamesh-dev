#include "viennameshpp/plugin.hpp"
#include "zoltan_partitioning.hpp"

viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
    viennamesh::register_algorithm<viennamesh::zoltan_partitioning>(context);

    return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
    return VIENNAMESH_VERSION;
}
