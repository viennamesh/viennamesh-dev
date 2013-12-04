# Project setup
###############

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
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
option(BUILD_EXTERNAL_LIB_TRIANGLE "Build the Triangle library" ON)
option(BUILD_EXTERNAL_LIB_TETGEN "Build the Tetgen library" ON)
option(BUILD_EXTERNAL_LIB_NETGEN "Build the Netgen library" ON)

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

find_package(Boost COMPONENTS thread program_options)

FIND_PACKAGE(CGAL)
IF(CGAL_FOUND)
  MESSAGE(STATUS "CGAL detected")
  set(CGAL_DONT_OVERRIDE_CMAKE_FLAGS TRUE)
#   INCLUDE(${CGAL_USE_FILE})
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CGAL_CXX_FLAGS_DEBUG_INIT} -DCGAL_CFG_ARRAY_MEMBER_INITIALIZATION_BUG")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CGAL_CXX_FLAGS_RELEASE_INIT} -DCGAL_CFG_ARRAY_MEMBER_INITIALIZATION_BUG")
ENDIF(CGAL_FOUND)

# Compile options
#################

if (CMAKE_BUILD_TYPE MATCHES "Debug")
  set(ENABLE_PEDANTIC_FLAGS 1)
endif()

if(ENABLE_PEDANTIC_FLAGS)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__STRICT_ANSI__")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")

message( STATUS "BUILD TYPE = ${CMAKE_BUILD_TYPE}" )
message( STATUS "RELEASE FLAGS = ${CMAKE_CXX_FLAGS_RELEASE}" )
message( STATUS "DEBUG FLAGS = ${CMAKE_CXX_FLAGS_DEBUG}" )


include_directories(BEFORE ${PROJECT_SOURCE_DIR})

# libviennamesh
################


IF(BUILD_EXTERNAL_LIB_NETGEN)
  file(GLOB NETGEN_CSG_SRC "${PROJECT_SOURCE_DIR}/external/netgen/libsrc/csg/*.cpp")
  list(REMOVE_ITEM NETGEN_CSG_SRC "${PROJECT_SOURCE_DIR}/external/netgen/libsrc/csg/csgpkg.cpp")
  list(REMOVE_ITEM NETGEN_CSG_SRC "${PROJECT_SOURCE_DIR}/external/netgen/libsrc/csg/vscsg.cpp")

  file(GLOB NETGEN_GENERAL_SRC "${PROJECT_SOURCE_DIR}/external/netgen/libsrc/general/*.cpp")

  file(GLOB NETGEN_GEOM2D_SRC "${PROJECT_SOURCE_DIR}/external/netgen/libsrc/geom2d/*.cpp")
  list(REMOVE_ITEM NETGEN_GEOM2D_SRC "${PROJECT_SOURCE_DIR}/external/netgen/libsrc/geom2d/vsgeom2d.cpp")
  list(REMOVE_ITEM NETGEN_GEOM2D_SRC "${PROJECT_SOURCE_DIR}/external/netgen/libsrc/geom2d/geom2dpkg.cpp")

  file(GLOB NETGEN_GPRIM_SRC "${PROJECT_SOURCE_DIR}/external/netgen/libsrc/gprim/*.cpp")
  file(GLOB NETGEN_INTERFACE_SRC "${PROJECT_SOURCE_DIR}/external/netgen/libsrc/interface/*.cpp")
  file(GLOB NETGEN_LINALG_SRC "${PROJECT_SOURCE_DIR}/external/netgen/libsrc/linalg/*.cpp")
  file(GLOB NETGEN_MESHING_SRC "${PROJECT_SOURCE_DIR}/external/netgen/libsrc/meshing/*.cpp")

  file(GLOB NETGEN_STLGEOM_SRC "${PROJECT_SOURCE_DIR}/external/netgen/libsrc/stlgeom/*.cpp")
  list(REMOVE_ITEM NETGEN_STLGEOM_SRC "${PROJECT_SOURCE_DIR}/external/netgen/libsrc/stlgeom/vsstl.cpp")
  list(REMOVE_ITEM NETGEN_STLGEOM_SRC "${PROJECT_SOURCE_DIR}/external/netgen/libsrc/stlgeom/stlpkg.cpp")

  SET(NETGEN_SRC ${NETGEN_CSG_SRC} ${NETGEN_GENERAL_SRC} ${NETGEN_GEOM2D_SRC} ${NETGEN_GPRIM_SRC} ${NETGEN_INTERFACE_SRC} ${NETGEN_LINALG_SRC} ${NETGEN_MESHING_SRC} ${NETGEN_STLGEOM_SRC} ${PROJECT_SOURCE_DIR}/external/netgen/nglib/nglib.cpp)


  SET(NETGEN_COMPILE_FLAGS "-DPACKAGE_VERSION=5.1")
  include_directories(${PROJECT_SOURCE_DIR}/external/netgen/libsrc/include)

  SET(LIBVIENNAMESH_SRC ${LIBVIENNAMESH_SRC} ${NETGEN_SRC})
  SET(LIBVIENNAMESH_COMPILE_FLAGS "${LIBVIENNAMESH_COMPILE_FLAGS} ${NETGEN_COMPILE_FLAGS}")

  set(THREADS_REQUIRED_FOR_LIB_VIENNAMESH TRUE)
  set(ZLIB_REQUIRED_FOR_LIB_VIENNAMESH TRUE)
endif(BUILD_EXTERNAL_LIB_NETGEN)


IF(BUILD_EXTERNAL_LIB_TETGEN)
  include_directories(${PROJECT_SOURCE_DIR}/external/tetgen/)
  SET(LIBVIENNAMESH_SRC ${LIBVIENNAMESH_SRC} ${PROJECT_SOURCE_DIR}/external/tetgen/tetgen.cxx ${PROJECT_SOURCE_DIR}/external/tetgen/predicates.cxx)
  SET(LIBVIENNAMESH_COMPILE_FLAGS "${LIBVIENNAMESH_COMPILE_FLAGS} -DTETLIBRARY")
ENDIF(BUILD_EXTERNAL_LIB_TETGEN)

IF(BUILD_EXTERNAL_LIB_TRIANGLE)
  include_directories(${PROJECT_SOURCE_DIR}/external/triangle/)
  SET(LIBVIENNAMESH_SRC ${LIBVIENNAMESH_SRC} ${PROJECT_SOURCE_DIR}/external/triangle/triangle.c ${LIBVIENNAMESH_SRC} ${PROJECT_SOURCE_DIR}/external/triangle/triangle_interface.c ${PROJECT_SOURCE_DIR}/viennamesh/algorithm/triangle/generator.cpp)
  SET(LIBVIENNAMESH_COMPILE_FLAGS "${LIBVIENNAMESH_COMPILE_FLAGS} -DTRILIBRARY -DANSI_DECLARATORS -DEXTERNAL_TEST")
ENDIF(BUILD_EXTERNAL_LIB_TRIANGLE)


ADD_LIBRARY(viennamesh SHARED ${LIBVIENNAMESH_SRC})
SET_TARGET_PROPERTIES(viennamesh PROPERTIES COMPILE_FLAGS "${LIBVIENNAMESH_COMPILE_FLAGS}")

# IF(BUILD_SHARED_LIBS)
#   SET_TARGET_PROPERTIES(viennamesh PROPERTIES COMPILE_FLAGS "${LIBVIENNAMESH_COMPILE_FLAGS}")
# else(BUILD_SHARED_LIBS)
#   SET(LIBVIENNAMESH_COMPILE_FLAGS "${LIBVIENNAMESH_COMPILE_FLAGS} -fPIC")
#   SET_TARGET_PROPERTIES(viennamesh PROPERTIES COMPILE_FLAGS "${LIBVIENNAMESH_COMPILE_FLAGS}")
# ENDIF(BUILD_SHARED_LIBS)


IF (DEFINED THREADS_REQUIRED_FOR_LIB_VIENNAMESH)
  find_package (Threads REQUIRED)
  target_link_libraries(viennamesh ${CMAKE_THREAD_LIBS_INIT})
ENDIF()

IF (DEFINED ZLIB_REQUIRED_FOR_LIB_VIENNAMESH)
  find_package(ZLIB REQUIRED)
  target_link_libraries(viennamesh ${ZLIB_LIBRARIES})
ENDIF()


# Subdirectories
################

if(BUILD_EXAMPLES)
   add_subdirectory(examples)
endif()

# if(BUILD_TOOLS)
#    add_subdirectory(tools)
# endif()

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
