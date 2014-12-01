cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(viennamesh_2)

include_directories(include)
include_directories(../../ViennaGrid/viennagrid-dev/include)


add_subdirectory(src)
add_subdirectory(plugins)

add_executable(main main.cpp)
target_link_libraries(main viennamesh)
# target_link_libraries(main dl)

# add_library(myplugin MODULE myplugin.cpp)
# set_target_properties(myplugin PROPERTIES PREFIX "")
