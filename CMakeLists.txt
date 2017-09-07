cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
include ($ENV{VIENNAUTILSPATH}/cmake/vienna_build_common.cmake NO_POLICY_SCOPE)

vienna_build_setup_project(viennamesh viennautils viennagrid)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")

if (VIENNA_BUILD_IS_MAIN_PROJECT)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

  option(ENABLE_PEDANTIC_FLAGS "Enable pedantic compiler flags" OFF)
  if(ENABLE_PEDANTIC_FLAGS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
  endif()

  message( STATUS "BUILD TYPE = ${CMAKE_BUILD_TYPE}" )
  message( STATUS "CXX FLAGS = ${CMAKE_CXX_FLAGS}" )
  message( STATUS "C FLAGS = ${CMAKE_C_FLAGS}" )
  message( STATUS "RELEASE FLAGS = ${CMAKE_CXX_FLAGS_RELEASE}" )
  message( STATUS "DEBUG FLAGS = ${CMAKE_CXX_FLAGS_DEBUG}" )

  # the compiler used for C files 
  MESSAGE( STATUS "CMAKE_C_COMPILER: " ${CMAKE_C_COMPILER} )

  # the compiler used for C++ files 
  MESSAGE( STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER} )
endif ()

set(VIENNAMESH_LIBRARIES ${VIENNAMESH_LIBRARIES} dl)
set(VIENNAMESH_LIBRARIES ${VIENNAMESH_LIBRARIES} viennagridpp)

set(INSTALL_PLUGIN_DIR "${CMAKE_INSTALL_PREFIX}/lib/viennamesh_plugins")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(VIENNAMESH_PLUGIN_DIRECTORIES "${VIENNAMESH_PLUGIN_DIRECTORIES}\\;viennamesh_plugins")
set(VIENNAMESH_PLUGIN_DIRECTORIES "${VIENNAMESH_PLUGIN_DIRECTORIES}\\;~/.viennamesh_plugins")
set(VIENNAMESH_PLUGIN_DIRECTORIES "${VIENNAMESH_PLUGIN_DIRECTORIES}\\;${INSTALL_PLUGIN_DIR}")
set(VIENNAMESH_PLUGIN_DIRECTORIES "${VIENNAMESH_PLUGIN_DIRECTORIES}\\;${PROJECT_BINARY_DIR}/plugins")
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

option(BUILD_EXAMPLES "Build Examples" ON)
if(BUILD_EXAMPLES)
   add_subdirectory(examples)
endif()
