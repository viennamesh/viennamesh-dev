cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
include ($ENV{VIENNAUTILSPATH}/cmake/vienna_build_common.cmake NO_POLICY_SCOPE)

vienna_build_setup_project(viennamesh viennautils viennagrid)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")

if (VIENNA_BUILD_IS_MAIN_PROJECT)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

  option(ENABLE_PEDANTIC_FLAGS "Enable pedantic compiler flags" ON)
  if(ENABLE_PEDANTIC_FLAGS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
  endif()

  message( STATUS "BUILD TYPE = ${CMAKE_BUILD_TYPE}" )
  message( STATUS "CXX FLAGS = ${CMAKE_CXX_FLAGS}" )
  message( STATUS "C FLAGS = ${CMAKE_C_FLAGS}" )
  message( STATUS "RELEASE FLAGS = ${CMAKE_CXX_FLAGS_RELEASE}" )
  message( STATUS "DEBUG FLAGS = ${CMAKE_CXX_FLAGS_DEBUG}" )
endif ()

set(VIENNAMESH_LIBRARIES ${VIENNAMESH_LIBRARIES} dl)

#find_package(ViennaGrid 3.0.0 REQUIRED)
#if (ViennaGrid_FOUND)
#  message( STATUS "ViennaGrid include directories: ${VIENNAGRID_INCLUDE_DIRS}")
#  message( STATUS "ViennaGrid libraries: ${VIENNAGRID_LIBRARIES}")

#  include_directories(${VIENNAGRID_INCLUDE_DIRS})
#  set(VIENNAMESH_LIBRARIES ${VIENNAMESH_LIBRARIES} ${VIENNAGRID_LIBRARIES})
#endif()

#link_directories("/usr/local/lib")

set(VIENNAMESH_LIBRARIES ${VIENNAMESH_LIBRARIES} viennagrid)


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
message(STATUS "External include directories: ${VIENNAMESH_EXTERNAL_INCLUDE_DIRECTORIES}")

vienna_build_add_subprojects()

add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(tools)

add_subdirectory(examples)
