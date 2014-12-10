add_definitions( -DNO_TIMER -DTRILIBRARY -DANSI_DECLARATORS -DEXTERNAL_TEST )

add_library(triangle MODULE plugin.cpp ${VIENNAMESH_PLUGIN_SOURCES} triangle_mesh.cpp triangle_make_mesh.cpp triangle_make_hull.cpp triangle.c triangle_interface.c)
set_target_properties(triangle PROPERTIES PREFIX "")

VIENNAMESH_ADD_PLUGIN(triangle)
