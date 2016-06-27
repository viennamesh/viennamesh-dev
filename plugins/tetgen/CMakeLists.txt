add_definitions( -DTETLIBRARY )

VIENNAMESH_ADD_PLUGIN(viennamesh-module-tetgen plugin.cpp
                      tetgen_mesh.cpp
                      tetgen_make_mesh.cpp
                      tetgen_convert.cpp
                      external/tetgen.cxx
                      external/predicates.cxx )
