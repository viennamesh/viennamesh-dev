# Project setup
###############

cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


project(ViennaMesh)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

#SET(CMAKE_VERBOSE_MAKEFILE ON)   #uncomment this line to increase CMake verbosity

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Common options
################

include(ViennaMeshCommon)

# User options
##############
option(VIENNAMESH_WITH_TRIANGLE "Build the Triangle library" ON)
option(VIENNAMESH_WITH_TETGEN "Build the Tetgen library" ON)
option(VIENNAMESH_WITH_NETGEN "Build the Netgen library" ON)
option(VIENNAMESH_NETGEN_WITH_OPENCASCADE "Build the Netgen with OpenCascade" OFF)
option(VIENNAMESH_WITH_VGMODELER "Build the VGModeler library" ON)
option(VIENNAMESH_WITH_TDR "Build the TDR reader" OFF)

option(BUILD_PYVIENNAMESH "Build the ViennaMesh Python Wrapper PyVienneMesh" ON)
option(BUILD_EXAMPLES "Build the ViennaMesh examples" ON)
option(BUILD_TOOLS "Build the ViennaMesh tools" ON)
option(BUILD_DOXYGEN_DOCS "Build the Doxygen-generated API docs" OFF)
option(BUILD_MANUAL "Build the PDF manual" OFF)

cmake_dependent_option(ENABLE_DIST "Enable the the distribution tarball target" ON "BUILD_DOXYGEN_DOCS;BUILD_MANUAL" OFF)

mark_as_advanced(BUILD_DOXYGEN_DOCS BUILD_MANUAL ENABLE_DIST)

# Find prerequisites
####################

# Set release build:
IF(DEFINED CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE)
 SET (CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
ELSE()
 SET (CMAKE_BUILD_TYPE "Release")
ENDIF()


if(BUILD_DOXYGEN_DOCS)
  find_package(Doxygen REQUIRED)
endif()

if(BUILD_MANUAL)
  find_package(LATEX REQUIRED)
endif()

find_package(Boost COMPONENTS)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()


# FIND_PACKAGE(CGAL)
# IF(CGAL_FOUND)
#   MESSAGE(STATUS "CGAL detected")
#   set(CGAL_DONT_OVERRIDE_CMAKE_FLAGS TRUE)
# #   INCLUDE(${CGAL_USE_FILE})
#   set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CGAL_CXX_FLAGS_DEBUG_INIT} -DCGAL_CFG_ARRAY_MEMBER_INITIALIZATION_BUG")
#   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CGAL_CXX_FLAGS_RELEASE_INIT} -DCGAL_CFG_ARRAY_MEMBER_INITIALIZATION_BUG")
# ENDIF(CGAL_FOUND)

# Compile options
#################

if (CMAKE_BUILD_TYPE MATCHES "Debug")
  set(ENABLE_PEDANTIC_FLAGS 1)
endif()

message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID}")

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  if(ENABLE_PEDANTIC_FLAGS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
  endif()

  IF(NOT BUILD_SHARED_LIBS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  ENDIF(NOT BUILD_SHARED_LIBS)
endif()

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /wd4503")

  if(ENABLE_PEDANTIC_FLAGS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  
  set(VIENNAMESH_WITH_VGMODELER OFF)
endif()



SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__STRICT_ANSI__")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")

message( STATUS "BUILD TYPE = ${CMAKE_BUILD_TYPE}" )
message( STATUS "RELEASE FLAGS = ${CMAKE_CXX_FLAGS_RELEASE}" )
message( STATUS "DEBUG FLAGS = ${CMAKE_CXX_FLAGS_DEBUG}" )


# include_directories(BEFORE ${PROJECT_SOURCE_DIR})
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/include)

# libviennamesh
################




set(EXTERNAL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/)
set(VIENNAMESH_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/)

add_subdirectory(external)
include_directories(${VIENNAMESH_EXTERNAL_INCLUDE_DIRS})

add_subdirectory(src)


# Python Layer
################

if (BUILD_PYVIENNAMESH)
  add_subdirectory(pyviennamesh)
endif()



# Subdirectories
################

FILE(GLOB_RECURSE EXAMPLE_MESHES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ examples/data/*)
FILE(GLOB_RECURSE EXAMPLE_PIPELINES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ examples/pipelines/*)

set(EXAMPLE_DATA ${EXAMPLE_MESHES} ${EXAMPLE_PIPELINES})
foreach(f IN LISTS EXAMPLE_DATA)
   configure_file(${f} "${CMAKE_CURRENT_BINARY_DIR}/${f}" COPYONLY)
endforeach()

if(BUILD_EXAMPLES)
   add_subdirectory(examples)
endif()

if(BUILD_TOOLS)
   include_directories(${PROJECT_SOURCE_DIR}/external/tclap/include/)
   add_subdirectory(tools)
endif()

if(BUILD_TESTING)
   INCLUDE(CTest)
   add_subdirectory(tests)
endif()

add_subdirectory(doc)

# Install
#########

install(DIRECTORY viennamesh ${CMAKE_CURRENT_BINARY_DIR}/viennamesh
   DESTINATION ${INSTALL_INCLUDE_DIR} COMPONENT dev
   FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")

# Dist target
#############

if(ENABLE_DIST)
   file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/dist")

   set(DISTDIR "${PROJECT_BINARY_DIR}/dist/ViennaMesh-${VERSION}-all-arch")
   configure_file(cmake/CMakeLists.txt.in "${DISTDIR}/CMakeLists.txt" @ONLY)

   #
   # Full package:
   #
   add_custom_target(dist
      COMMAND "${CMAKE_COMMAND}" -E make_directory "${DISTDIR}"

      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/LICENSE" "${DISTDIR}/LICENSE"

      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/changelog" "${DISTDIR}/changelog"

      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/README" "${DISTDIR}/README"

      COMMAND "${CMAKE_COMMAND}" -E make_directory "${DISTDIR}/build"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/build/README" "${DISTDIR}/build/README"

      COMMAND "${CMAKE_COMMAND}" -E make_directory "${DISTDIR}/cmake"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/cmake/copymanual.cmake" "${DISTDIR}/cmake/copymanual.cmake"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/cmake/ViennaMeshCommon.cmake" "${DISTDIR}/cmake/ViennaMeshCommon.cmake"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/cmake/ViennaMeshConfig.cmake.in" "${DISTDIR}/cmake/ViennaMeshConfig.cmake.in"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/cmake/ViennaMeshConfigVersion.cmake.in" "${DISTDIR}/cmake/ViennaMeshConfigVersion.cmake.in"

      COMMAND "${CMAKE_COMMAND}" -E make_directory "${DISTDIR}/doc"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/doc/viennamesh.pdf" "${DISTDIR}/doc/viennamesh.pdf"
      COMMAND "${CMAKE_COMMAND}" -E copy_directory
         "${CMAKE_CURRENT_BINARY_DIR}/doc/doxygen"
         "${DISTDIR}/doc/doxygen"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/doc/viennamesh.pdf" "${DISTDIR}/doc/viennamesh.pdf"

      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/doc/Doxyfile.in" "${DISTDIR}/doc/Doxyfile.in"

      COMMAND "${CMAKE_COMMAND}" -E copy_directory
         "${PROJECT_SOURCE_DIR}/examples" "${DISTDIR}/examples"

      COMMAND "${CMAKE_COMMAND}" -E make_directory "${DISTDIR}/tests"
      COMMAND "${CMAKE_COMMAND}" -E make_directory "${DISTDIR}/tests/src"
      COMMAND "${CMAKE_COMMAND}" -E copy_directory
         "${PROJECT_SOURCE_DIR}/tests/src" "${DISTDIR}/tests/src"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/tests/CMakeLists.txt" "${DISTDIR}/tests/CMakeLists.txt"

      COMMAND "${CMAKE_COMMAND}" -E copy_directory
         "${PROJECT_SOURCE_DIR}/viennamesh" "${DISTDIR}/viennamesh"

      COMMAND "${CMAKE_COMMAND}" -E tar czf ViennaMesh-${VERSION}-all-arch.tar.gz
           ViennaMesh-${VERSION}-all-arch
      WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/dist"
      COMMENT "Generating dist/ViennaMesh-${VERSION}-all-arch.tar.gz"
      VERBATIM)
   add_dependencies(dist apidoc manual)

endif()
