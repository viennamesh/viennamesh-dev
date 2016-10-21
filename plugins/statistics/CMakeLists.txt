
#libigl include path
include_directories(external/)

VIENNAMESH_ADD_PLUGIN(viennamesh-module-statistics plugin.cpp
                      make_statistic.cpp
                      mesh_information.cpp)
