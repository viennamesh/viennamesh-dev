PROJECT(ViennaMesh)

cmake_minimum_required(VERSION 2.6)

# build output verbosity
SET(CMAKE_VERBOSE_MAKEFILE ON)



# set compiler flags
IF(CMAKE_COMPILER_IS_GNUCXX)
#   ADD_DEFINITIONS(-g -DMESH_KERNEL_DEBUG -DMESH_KERNEL_DEBUG_FULL -frounding-math)
   ADD_DEFINITIONS(-g -DMESH_KERNEL_DEBUG -DMESH_KERNEL_DEBUG_FULL)
   #ADD_DEFINITIONS(-g)
   #ADD_DEFINITIONS(-g -Wall -pedantic)
   #ADD_DEFINITIONS(-O2)
   #ADD_DEFINITIONS(-O2 -Wall -pedantic)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


# ------------------------------------------------------------------------------
#
# *** CHECK FOR DEPENDENCIES ***
#
# ------------------------------------------------------------------------------

# set modules directory
#
set(MODULES_DIR     ${CMAKE_SOURCE_DIR}/cmake )

# check for BOOST
#
include(${MODULES_DIR}/FindBOOST.cmake)
IF( BOOST_FOUND )
   message(STATUS "setting BOOST Paths ..")
   INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIRS})
   LINK_DIRECTORIES(${BOOST_LIBRARIES})
ENDIF()

# check for GMP
#
include(${MODULES_DIR}/FindGMP.cmake)
IF( GMP_FOUND )
   message(STATUS "setting GMP Paths ..")
   LINK_DIRECTORIES(${GMP_LIBRARIES})
ENDIF()

# check for CGAL
#
include(${MODULES_DIR}/FindCGAL.cmake)
IF( CGAL_FOUND )
   message(STATUS "setting CGAL Paths ..")
   INCLUDE_DIRECTORIES(${CGAL_INCLUDE_DIRS})
   LINK_DIRECTORIES(${CGAL_LIBRARIES})
ENDIF()
# ------------------------------------------------------------------------------


# build external libraries
#   those have to be build bevore the collected include 
#   hierarchy of viennamesh is established here
#   otherwise the netgen and vgmodeler libraries won't compile
ADD_SUBDIRECTORY(external)

# build ViennaUtils
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/../ViennaUtils/ "${CMAKE_CURRENT_BINARY_DIR}/ViennaUtils/")

# setup the include paths
INCLUDE_DIRECTORIES(".")
INCLUDE_DIRECTORIES("external/triangle-1.6/")
INCLUDE_DIRECTORIES("external/tetgen-1.4.3/")
INCLUDE_DIRECTORIES("external/vgmodeler-2.0/")
INCLUDE_DIRECTORIES("external/vgmodeler-2.0/vgmodeler/volume_generation/include")
INCLUDE_DIRECTORIES("external/netgen-4.9.13/netgen")
INCLUDE_DIRECTORIES("external/netgen-4.9.13/netgen/nglib")
INCLUDE_DIRECTORIES("external/netgen-4.9.13/netgen/libsrc/include")
INCLUDE_DIRECTORIES("external/netgen-4.9.13/netgen/libsrc/meshing")
INCLUDE_DIRECTORIES("external/cervpt-1.0/")
#INCLUDE_DIRECTORIES("external/ViennaGrid/")
#INCLUDE_DIRECTORIES("external/ViennaData/")
#INCLUDE_DIRECTORIES("external/ViennaUtils/")
#INCLUDE_DIRECTORIES("external/ViennaUtils/external/")
#INCLUDE_DIRECTORIES("external/ViennaUtils/external/gsse01/")
#INCLUDE_DIRECTORIES("external/ViennaUtils/external/gsse01/external/mauch")
INCLUDE_DIRECTORIES("../ViennaGrid/")
INCLUDE_DIRECTORIES("../ViennaData/")
INCLUDE_DIRECTORIES("../ViennaUtils/")
INCLUDE_DIRECTORIES("../ViennaUtils/external/")
INCLUDE_DIRECTORIES("../ViennaUtils/external/gts/UtilityLibs/gtsutil/src/include")
INCLUDE_DIRECTORIES("../ViennaUtils/external/gts/UtilityLibs/gtsio3/src/")
INCLUDE_DIRECTORIES("../ViennaUtils/external/gts/3rdPartyTools/libkdtree++")
INCLUDE_DIRECTORIES("../ViennaUtils/external/gsse01/")
INCLUDE_DIRECTORIES("../ViennaUtils/external/gsse01/external/mauch")





#INCLUDE_DIRECTORIES("/mnt/data/libraries/vtk/vtk-install/include/vtk-5.6")
#LINK_DIRECTORIES(/mnt/data/libraries/vtk/vtk-install/lib/vtk-5.6)

# REMOVEME
#add_subdirectory(tests)

#list all source files here
#ADD_EXECUTABLE(viennamesh src/viennamesh.cpp)
#ADD_EXECUTABLE(vgmodeler src/vgmodeler.cpp)
#ADD_EXECUTABLE(ng src/netgen.cpp)
#ADD_EXECUTABLE(tet src/tetgen.cpp)
ADD_EXECUTABLE(generate src/generate.cpp)
ADD_EXECUTABLE(mesh src/mesh.cpp)

#link specific libraries to specific executables
#TARGET_LINK_LIBRARIES(viennamesh gtsio3 tetgen triangle vgmodeler_volume)
#TARGET_LINK_LIBRARIES(vgmodeler vgmodeler_volume)
#TARGET_LINK_LIBRARIES(ng netgen)
#TARGET_LINK_LIBRARIES(tet tetgen)
TARGET_LINK_LIBRARIES(generate gtsio3 netgen tetgen triangle vgmodeler_volume pugixml CGAL CGAL_Core gmp boost_thread)
TARGET_LINK_LIBRARIES(mesh tetgen)

