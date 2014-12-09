add_library(io MODULE plugin.cpp common.cpp mesh_reader.cpp mesh_writer.cpp ${VIENNAMESH_PLUGIN_SOURCES})
set_target_properties(io PROPERTIES PREFIX "")

VIENNAMESH_ADD_PLUGIN(io)
