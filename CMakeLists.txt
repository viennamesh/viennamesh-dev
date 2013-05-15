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

# Set release build:
IF(DEFINED CMAKE_BUILD_TYPE)
 SET (CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
ELSE()
 SET (CMAKE_BUILD_TYPE Release)
ENDIF()

# User options
##############

option(BUILD_DOXYGEN_DOCS "Build the Doxygen-generated API docs" OFF)

option(BUILD_MANUAL "Build the PDF manual" OFF)

cmake_dependent_option(ENABLE_DIST "Enable the the distribution tarball target" ON "BUILD_DOXYGEN_DOCS;BUILD_MANUAL" OFF)

mark_as_advanced(BUILD_DOXYGEN_DOCS BUILD_MANUAL ENABLE_DIST)

# Find prerequisites
####################

if(BUILD_DOXYGEN_DOCS)
   find_package(Doxygen REQUIRED)
endif()

if(BUILD_MANUAL)
   find_package(LATEX REQUIRED)
endif()

find_package(Boost COMPONENTS thread program_options)

# Compile options
#################

include_directories(BEFORE ${PROJECT_SOURCE_DIR})

# external libraries
################

include(ExternalProject)

ExternalProject_Add(netgen
   URL "${PROJECT_SOURCE_DIR}/external/netgen-5.0.0/"
   CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --enable-nglib --disable-gui
   )
link_directories( ${PROJECT_BINARY_DIR}/netgen-prefix/lib )

add_subdirectory( external/vgmodeler )

# Subdirectories
################

if(BUILD_EXAMPLES)
   add_subdirectory(examples)
endif()

if(BUILD_TOOLS)
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
