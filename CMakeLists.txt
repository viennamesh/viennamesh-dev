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
option(ENABLE_TRIANGLE                 "Enables the Triangle library" ON)
option(ENABLE_TETGEN                   "Enables the Tetgen library" ON)
option(ENABLE_NETGEN                   "Enables the Netgen library" ON)
option(ENABLE_NETGEN_OPENCASCADE       "Enables the OpenCascade for the Netgen library" ON)
option(ENABLE_VGMODELER                "Enables the VgModeler library" ON)
option(ENABLE_TDR_READER               "Enables the TDR reader" ON)
option(ENABLE_GEOMETRY_GENERATOR       "Enables the Geometry Generator" ON)


option(BUILD_PYVIENNAMESH "Build the ViennaMesh Python Wrapper PyViennaMesh" OFF)
option(BUILD_EXAMPLES "Build the ViennaMesh examples" OFF)
option(BUILD_TOOLS "Build the ViennaMesh tools" ON)
option(BUILD_MANUAL "Build the Doxygen-generated manual docs" OFF)

cmake_dependent_option(ENABLE_DIST "Enable the the distribution tarball target" ON "BUILD_MANUAL" OFF)

mark_as_advanced(BUILD_MANUAL ENABLE_DIST)



find_package (Threads)
if (Threads_FOUND)
  message(STATUS "Found Threads")
#   message(STATUS "Threads include directories: ${BOOST_INCLUDE_DIRS}")
  message(STATUS "Threads libraries: ${CMAKE_THREAD_LIBS_INIT}")

#   set(VIENNAMESH_INCLUDE_DIRECTORIES ${VIENNAMESH_INCLUDE_DIRECTORIES} ${BOOST_INCLUDE_DIRS})
  set(VIENNAMESH_LIBRARIES ${VIENNAMESH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

  set(VIENNAMESH_COMPILE_FLAGS "${VIENNAMESH_COMPILE_FLAGS} -DVIENNAMESH_HAS_THREADS")
  set(VIENNAMESH_HAS_THREADS TRUE)
else()
  message(STATUS "Threads not found")
endif()


find_package(Boost)
if(Boost_FOUND)
  message(STATUS "Found Boost")
  message(STATUS "Boost include directories: ${BOOST_INCLUDE_DIRS}")
  message(STATUS "Boost libraries: ${BOOST_LIBRARIES}")

  set(VIENNAMESH_INCLUDE_DIRECTORIES ${VIENNAMESH_INCLUDE_DIRECTORIES} ${BOOST_INCLUDE_DIRS})
  set(VIENNAMESH_LIBRARIES ${VIENNAMESH_LIBRARIES} ${BOOST_LIBRARIES})

  set(VIENNAMESH_COMPILE_FLAGS "${VIENNAMESH_COMPILE_FLAGS} -DVIENNAMESH_HAS_BOOST")
  set(VIENNAMESH_HAS_BOOST TRUE)
else()
  message(STATUS "Boost not found")
endif()


find_package(PythonLibs 2)
if (PYTHONLIBS_FOUND)
  message(STATUS "Found Python")
  message(STATUS "Python include directories: ${PYTHON_INCLUDE_DIRS}")
  message(STATUS "Python libraries: ${PYTHON_LIBRARIES}")

  set(VIENNAMESH_INCLUDE_DIRECTORIES ${VIENNAMESH_INCLUDE_DIRECTORIES} ${PYTHON_INCLUDE_DIRS})
  set(VIENNAMESH_LIBRARIES ${VIENNAMESH_LIBRARIES} ${PYTHON_LIBRARIES})

  set(VIENNAMESH_COMPILE_FLAGS "${VIENNAMESH_COMPILE_FLAGS} -DVIENNAMESH_HAS_PYTHON")
  set(VIENNAMESH_HAS_PYTHON TRUE)
else()
  message(STATUS "Python not found")
endif()


find_package(HDF5 COMPONENTS CXX)
if (HDF5_FOUND)
  message(STATUS "Found HDF5")
  message(STATUS "HDF5 include directories: ${HDF5_INCLUDE_DIRS}")
  message(STATUS "HDF5 libraries: ${HDF5_LIBRARIES}")

  set(VIENNAMESH_INCLUDE_DIRECTORIES ${VIENNAMESH_INCLUDE_DIRECTORIES} ${HDF5_INCLUDE_DIRS})
  set(VIENNAMESH_LIBRARIES ${VIENNAMESH_LIBRARIES} ${HDF5_LIBRARIES})

  set(VIENNAMESH_COMPILE_FLAGS "${VIENNAMESH_COMPILE_FLAGS} -DVIENNAMESH_HAS_HDF5")
  set(VIENNAMESH_HAS_HDF5 TRUE)
else()
  message(STATUS "HDF5 not found")
endif()



find_package(OCE)
if (OCE_FOUND)
  message(STATUS "Found OCE")
  message(STATUS "OCE include directories: ${OCE_INCLUDE_DIRS}")

  # HACK!!!
  set(OCE_LIBRARIES TKernel TKGeomBase TKMath TKG2d TKG3d TKXSBase TKOffset TKFillet TKShHealing TKMesh TKTopAlgo TKGeomAlgo TKBool TKPrim TKBO TKIGES TKBRep TKSTEPBase TKSTEP TKSTL TKSTEPAttr TKSTEP209 TKXDESTEP TKXDEIGES TKXCAF TKLCAF FWOSPlugin)
  # HACK!!!

  message(STATUS "OCE libraries: ${OCE_LIBRARIES}")

  set(VIENNAMESH_INCLUDE_DIRECTORIES ${VIENNAMESH_INCLUDE_DIRECTORIES} ${OCE_INCLUDE_DIRS})
  set(VIENNAMESH_LIBRARIES ${VIENNAMESH_LIBRARIES} ${OCE_LIBRARIES})

  set(VIENNAMESH_COMPILE_FLAGS "${VIENNAMESH_COMPILE_FLAGS} -DVIENNAMESH_HAS_OCE")
  set(VIENNAMESH_HAS_OCE TRUE)
else()
  message(STATUS "OCE not found")
endif()


find_package(SWIG)
if(SWIG_FOUND)
  message(STATUS "Found SWIG")
  set(VIENNAMESH_HAS_SWIG TRUE)
else()
  message(STATUS "SWIG not found")
endif()







set(VIENNAMESH_WITH_TRIANGLE FALSE)
if (ENABLE_TRIANGLE)
  message(STATUS "Triangle ENABLED")
  set(VIENNAMESH_COMPILE_FLAGS "${VIENNAMESH_COMPILE_FLAGS} -DVIENNAMESH_WITH_TRIANGLE")
  set(VIENNAMESH_WITH_TRIANGLE TRUE)
else()
  message(STATUS "Triangle DISABLED")
endif()


set(VIENNAMESH_WITH_TETGEN FALSE)
if (ENABLE_TETGEN)
  message(STATUS "Tetgen ENABLED")
  set(VIENNAMESH_COMPILE_FLAGS "${VIENNAMESH_COMPILE_FLAGS} -DVIENNAMESH_WITH_TETGEN")
  set(VIENNAMESH_WITH_TETGEN TRUE)
else()
  message(STATUS "Tetgen DISABLED")
endif()


set(VIENNAMESH_WITH_NETGEN FALSE)
if (ENABLE_NETGEN)
  message(STATUS "Netgen ENABLED")
  set(VIENNAMESH_COMPILE_FLAGS "${VIENNAMESH_COMPILE_FLAGS} -DVIENNAMESH_WITH_NETGEN")
  set(VIENNAMESH_WITH_NETGEN TRUE)
else()
  message(STATUS "Netgen DISABLED")
endif()


set(VIENNAMESH_WITH_VGMODELER FALSE)
if (ENABLE_VGMODELER)
  if (NOT MSVC)
    message(STATUS "VgModeler ENABLED")
    set(VIENNAMESH_COMPILE_FLAGS "${VIENNAMESH_COMPILE_FLAGS} -DVIENNAMESH_WITH_VGMODELER")
    set(VIENNAMESH_WITH_VGMODELER TRUE)
  else()
    message(WARNING "VgModeler not supported for MSVC, disabling")
  endif()
else()
  message(STATUS "VgModeler DISABLED")
endif()


set(VIENNAMESH_NETGEN_WITH_OPENCASCADE FALSE)
if (ENABLE_NETGEN_OPENCASCADE)
  if (VIENNAMESH_HAS_OCE)
    message(STATUS "Netgen OpenCascade ENABLED")
    set(VIENNAMESH_COMPILE_FLAGS "${VIENNAMESH_COMPILE_FLAGS} -DVIENNAMESH_WITH_NETGEN_OPENCASCADE")
    set(VIENNAMESH_NETGEN_WITH_OPENCASCADE TRUE)
  else()
    message(WARNING "ENABLE_NETGEN_OPENCASCADE has been enabled but OCE was not found, disabling ENABLE_NETGEN_OPENCASCADE")
  endif()
else()
  message(STATUS "Netgen OpenCascade DISABLED")
endif()


set(VIENNAMESH_WITH_GEOMETRY_GENERATOR FALSE)
if (ENABLE_GEOMETRY_GENERATOR)
  if (VIENNAMESH_HAS_PYTHON)
    message(STATUS "Geometry Generator ENABLED")
    set(VIENNAMESH_COMPILE_FLAGS "${VIENNAMESH_COMPILE_FLAGS} -DVIENNAMESH_WITH_GEOMETRY_GENERATOR")
    set(VIENNAMESH_WITH_GEOMETRY_GENERATOR TRUE)
  else()
    message(WARNING "ENABLE_GEOMETRY_GENERATOR has been enabled but Python was not found, disabling ENABLE_GEOMETRY_GENERATOR")
  endif()
else()
  message(STATUS "Geometry Generator DISABLED")
endif()


set(VIENNAMESH_WITH_TDR FALSE)
if (ENABLE_TDR_READER)
  if (VIENNAMESH_HAS_HDF5)
    message(STATUS "TDR Reader ENABLED")
    set(VIENNAMESH_COMPILE_FLAGS "${VIENNAMESH_COMPILE_FLAGS} -DVIENNAMESH_WITH_TDR_READER")
    set(VIENNAMESH_WITH_TDR TRUE)
  else()
    message(WARNING "ENABLE_TDR_READER has been enabled but TDR5 was not found, disabling ENABLE_TDR_READER")
  endif()
else()
  message(STATUS "TDR Reader DISABLED")
endif()


if (BUILD_PYVIENNAMESH)
  if (NOT VIENNAMESH_HAS_PYTHON)
    message(WARNING "BUILD_PYVIENNAMESH has been enabled but Python was not found, disabling BUILD_PYVIENNAMESH")
    set (BUILD_PYVIENNAMESH OFF)
  else()
    if (NOT VIENNAMESH_HAS_SWIG)
      message(WARNING "BUILD_PYVIENNAMESH has been enabled but SWIG was not found, disabling BUILD_PYVIENNAMESH")
      set (BUILD_PYVIENNAMESH OFF)
    else()
      message(STATUS "PyViennaMesh ENABLED")
    endif()
  endif()
else()
  message(STATUS "PyViennaMesh DISABLED")
endif()




message(STATUS "Include Directories: ${VIENNAMESH_INCLUDE_DIRECTORIES}")
include_directories(${VIENNAMESH_INCLUDE_DIRECTORIES})





# Find prerequisites
####################

# Set release build:
IF(DEFINED CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE)
 SET (CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
ELSE()
 SET (CMAKE_BUILD_TYPE "Release")
ENDIF()



# Compile options
#################

if (CMAKE_BUILD_TYPE MATCHES "Debug")
  set(ENABLE_PEDANTIC_FLAGS 1)
endif()

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
endif()



SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__STRICT_ANSI__")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")

message( STATUS "BUILD TYPE = ${CMAKE_BUILD_TYPE}" )
message( STATUS "RELEASE FLAGS = ${CMAKE_CXX_FLAGS_RELEASE}" )
message( STATUS "DEBUG FLAGS = ${CMAKE_CXX_FLAGS_DEBUG}" )


include_directories(BEFORE ${PROJECT_SOURCE_DIR}/include)


# libviennamesh
################


set(EXTERNAL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/)
set(VIENNAMESH_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/)

add_subdirectory(external)

include_directories(${VIENNAMESH_EXTERNAL_INCLUDE_DIRS})
message(STATUS "External include directories: ${VIENNAMESH_EXTERNAL_INCLUDE_DIRS}")

add_subdirectory(src)


# Python Layer
################

if (BUILD_PYVIENNAMESH)
  add_subdirectory(pyviennamesh)
endif (BUILD_PYVIENNAMESH)


# Subdirectories
################


FILE(GLOB_RECURSE EXAMPLE_MESHES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ examples/data/*)
FILE(GLOB_RECURSE EXAMPLE_PIPELINES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ examples/pipelines/*)

set(EXAMPLE_DATA ${EXAMPLE_MESHES} ${EXAMPLE_PIPELINES})
foreach(f ${EXAMPLE_DATA})
   configure_file(${f} "${CMAKE_CURRENT_BINARY_DIR}/${f}" COPYONLY)
endforeach()

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


# Install
#########


add_subdirectory(doc)


# Install
#########


install(DIRECTORY include/viennamesh
        DESTINATION ${INSTALL_INCLUDE_DIR} COMPONENT dev
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")





# Export
########

configure_file(cmake/ViennaMeshConfig.cmake.in
   ${PROJECT_BINARY_DIR}/ViennaMeshConfig.cmake @ONLY)

configure_file(cmake/ViennaMeshConfigVersion.cmake.in
   ${PROJECT_BINARY_DIR}/ViennaMeshConfigVersion.cmake @ONLY)

# Install
#########

install(FILES
   ${PROJECT_BINARY_DIR}/ViennaMeshConfig.cmake
   ${PROJECT_BINARY_DIR}/ViennaMeshConfigVersion.cmake
   DESTINATION ${INSTALL_CMAKE_DIR} COMPONENT dev)



# Dist target
#############


if(ENABLE_DIST)
  file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/dist")

  set(DISTDIR "${PROJECT_BINARY_DIR}/dist/ViennaMesh-${VERSION}-all-arch")
#   configure_file(cmake/CMakeLists.txt.in "${DISTDIR}/CMakeLists.txt" @ONLY)

  #
  # Full package:
  #
  add_custom_target(dist
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${DISTDIR}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${DISTDIR}/build"

    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${PROJECT_SOURCE_DIR}/CMakeLists.txt" "${DISTDIR}/CMakeLists.txt"

    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${PROJECT_SOURCE_DIR}/LICENSE" "${DISTDIR}/LICENSE"

    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${PROJECT_SOURCE_DIR}/changelog" "${DISTDIR}/changelog"

    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${PROJECT_SOURCE_DIR}/README.md" "${DISTDIR}/README.md"

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
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_BINARY_DIR}/doc/doxygen"
        "${DISTDIR}/doc/doxygen"

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

    COMMAND "${CMAKE_COMMAND}" -E make_directory "${DISTDIR}/include/viennamesh"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${PROJECT_SOURCE_DIR}/include/viennamesh" "${DISTDIR}/include/viennamesh"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${PROJECT_SOURCE_DIR}/src" "${DISTDIR}/src"

    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${PROJECT_SOURCE_DIR}/external" "${DISTDIR}/external"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${PROJECT_SOURCE_DIR}/pyviennamesh" "${DISTDIR}/pyviennamesh"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${PROJECT_SOURCE_DIR}/tools" "${DISTDIR}/tools"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${PROJECT_SOURCE_DIR}/doc" "${DISTDIR}/doc"

    COMMAND "${CMAKE_COMMAND}" -E make_directory "${DISTDIR}/include/viennagrid"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "$ENV{VIENNAGRIDPATH}/viennagrid" "${DISTDIR}/include/viennagrid"


    COMMAND "${CMAKE_COMMAND}" -E tar czf ViennaMesh-${VERSION}-all-arch.tar.gz
          ViennaMesh-${VERSION}-all-arch
    WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/dist"
    COMMENT "Generating dist/ViennaMesh-${VERSION}-all-arch.tar.gz"
    VERBATIM)
  add_dependencies(dist manual)

endif()
