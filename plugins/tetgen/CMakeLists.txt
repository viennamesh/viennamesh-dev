add_definitions( -DTETLIBRARY )

VIENNAMESH_ADD_PLUGIN(viennamesh-module-tetgen plugin.cpp tetgen_mesh.cpp tetgen_make_mesh.cpp tetgen.cxx predicates.cxx )
