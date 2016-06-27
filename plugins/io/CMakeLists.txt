VIENNAMESH_ADD_PLUGIN(viennamesh-module-io plugin.cpp
                      common.cpp
                      mesh_reader.cpp
                      mesh_writer.cpp
                      plc_reader.cpp
                      plc_writer.cpp)

target_link_libraries(viennamesh-module-io viennautils_dfise)
