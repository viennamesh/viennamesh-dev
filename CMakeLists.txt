cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(viennamesh_2)




include_directories(include)
include_directories(../../ViennaGrid/viennagrid-dev/include)

add_definitions( -DVIENNAMESH_DEFAULT_PLUGIN_DIRECTORY=\"${CMAKE_CURRENT_BINARY_DIR}/plugins\" )

add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(examples)
