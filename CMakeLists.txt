cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(viennamesh_2)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

message( STATUS "BUILD TYPE = ${CMAKE_BUILD_TYPE}" )
message( STATUS "RELEASE FLAGS = ${CMAKE_CXX_FLAGS_RELEASE}" )
message( STATUS "DEBUG FLAGS = ${CMAKE_CXX_FLAGS_DEBUG}" )

set(VIENNAMESH_LIBRARIES ${VIENNAMESH_LIBRARIES} dl)

find_package(ViennaGrid 3.0.0 REQUIRED)
if (ViennaGrid_FOUND)
  message("ViennaGrid include directories: ${VIENNAGRID_INCLUDE_DIRS}")
  message("ViennaGrid libraries: ${VIENNAGRID_LIBRARIES}")

  include_directories(${VIENNAGRID_INCLUDE_DIRS})
  set(VIENNAMESH_LIBRARIES ${VIENNAMESH_LIBRARIES} ${VIENNAGRID_LIBRARIES})
endif()

link_directories("/usr/local/lib")


set(INSTALL_PLUGIN_DIR "${CMAKE_INSTALL_PREFIX}/lib/viennamesh_plugins")


set(VIENNAMESH_PLUGIN_DIRECTORIES "${VIENNAMESH_PLUGIN_DIRECTORIES}\\;viennamesh_plugins")
set(VIENNAMESH_PLUGIN_DIRECTORIES "${VIENNAMESH_PLUGIN_DIRECTORIES}\\;~/.viennamesh_plugins")
set(VIENNAMESH_PLUGIN_DIRECTORIES "${VIENNAMESH_PLUGIN_DIRECTORIES}\\;${INSTALL_PLUGIN_DIR}")
message(STATUS "ViennaMesh plugin directories: ${VIENNAMESH_PLUGIN_DIRECTORIES}")
add_definitions( "-DVIENNAMESH_DEFAULT_PLUGIN_DIRECTORY=\"${VIENNAMESH_PLUGIN_DIRECTORIES}\"" )


include_directories(include)

add_subdirectory(external)
include_directories(${VIENNAMESH_EXTERNAL_INCLUDE_DIRECTORIES})
set(VIENNAMESH_LIBRARIES ${VIENNAMESH_LIBRARIES} ${VIENNAMESH_EXTERNAL_LIBRARIES})
message("External include directories: ${VIENNAMESH_EXTERNAL_INCLUDE_DIRECTORIES}")

add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(tools)

add_subdirectory(examples)
