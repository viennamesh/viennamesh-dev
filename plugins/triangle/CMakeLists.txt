add_definitions( -DNO_TIMER -DTRILIBRARY -DANSI_DECLARATORS -DEXTERNAL_TEST )

VIENNAMESH_ADD_PLUGIN(viennamesh-module-triangle plugin.cpp
                      triangle_mesh.cpp
                      triangle_make_mesh.cpp
                      triangle_make_hull.cpp
                      triangle_convert.cpp
                      external/triangle.c
                      triangle_interface.c)
