
add_subdirectory(external)
include_directories(external)

VIENNAMESH_ADD_PLUGIN(viennamesh-module-vgmodeler plugin.cpp
                      vgmodeler_adapt_hull.cpp)

target_link_libraries(viennamesh-module-vgmodeler viennamesh-vgmodeler)
