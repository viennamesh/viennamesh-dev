PROJECT(ViennaMesh)

cmake_minimum_required(VERSION 2.6)

# build output verbosity
SET(CMAKE_VERBOSE_MAKEFILE ON)


# ------------------------------------------------------------------------------
#
# *** SET COMPILER FLAGS ***
#
# ------------------------------------------------------------------------------

IF(CMAKE_COMPILER_IS_GNUCXX)
   # Optimize Release Build Type with -O2 instead of -O3
   set(CMAKE_CXX_FLAGS_RELEASE "-O2")  
   # Strip release binaries   
   set(CMAKE_EXE_LINKER_FLAGS_RELEASE"-s")    
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# add specific debug flags
IF(CMAKE_BUILD_TYPE MATCHES Debug)
   ADD_DEFINITIONS(-Wall -DMESH_KERNEL_DEBUG -DMESH_ADAPTOR_DEBUG)
#   ADD_DEFINITIONS(-Wall -DMESH_KERNEL_DEBUG -DMESH_KERNEL_DEBUG_FULL)
#   ADD_DEFINITIONS(-Wall -DMESH_KERNEL_DEBUG -DIODEBUG)   
#   ADD_DEFINITIONS(-Wall -DMESH_KERNEL_DEBUG -DMESH_KERNEL_DEBUG_FULL -DIODEBUG)   
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

# ------------------------------------------------------------------------------
#
# *** CHECK FOR DEPENDENCIES ***
#
# ------------------------------------------------------------------------------

# set modules directory
#
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# check for the Boost libraries
#
set(BOOST_MIN_VERSION 1.42.0)
set(BOOST_LIBS thread)
set(Boost_USE_STATIC_LIBS TRUE)
set(Boost_USE_MULTITHREADED TRUE)
find_package(Boost ${BOOST_MIN_VERSION} COMPONENTS ${BOOST_LIBS} REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# check for the PThreads library
#
find_package(Threads REQUIRED)

# check for the GMP library
#
find_package(GMP REQUIRED)
include_directories(${GMP_INCLUDE_DIRS})

# check for the CGAL libraries
#
find_package(CGAL REQUIRED)
ADD_DEFINITIONS(${CGAL_DEFINITIONS})   
include_directories(${CGAL_INCLUDE_DIRS})

# ------------------------------------------------------------------------------
#
# *** PROCESS RELATED CMAKE PROJECTS ***
#
# ------------------------------------------------------------------------------

# build external libraries
#   those have to be build bevore the collected include 
#   hierarchy of viennamesh is established here
#   otherwise the netgen and vgmodeler libraries won't compile
ADD_SUBDIRECTORY(external)

# build ViennaUtils
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/../ViennaUtils/ "${CMAKE_CURRENT_BINARY_DIR}/ViennaUtils/")

# ------------------------------------------------------------------------------
#
# *** SET INCLUDE PATHS OF MESH KERNELS AND RELATED VIENNASTAR PROJECTS ***
#
# ------------------------------------------------------------------------------

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


# ------------------------------------------------------------------------------
#
# *** SET MESH TOOLS RELATED LIBS ***
#
# ------------------------------------------------------------------------------
set(CERVPT_LIBS  ${CGAL_LIBRARIES} ${GMP_LIBRARIES} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
set(GTSIO_LIBS gtsio3 z)

# ------------------------------------------------------------------------------
#
# *** SETUP APPLICATIONS ***
#
# ------------------------------------------------------------------------------

#list all source files here
ADD_EXECUTABLE(generate src/generate.cpp)
#ADD_EXECUTABLE(mesh src/mesh.cpp)
#ADD_EXECUTABLE(playground src/playground.cpp)
#ADD_EXECUTABLE(playground2 src/playground2.cpp)
#ADD_EXECUTABLE(playground3 src/playground3.cpp)

# ------------------------------------------------------------------------------
#
# *** LINK APPLICATIONS AGAINST SPECIFIC LIBRARIES ***
#
# ------------------------------------------------------------------------------

#link specific libraries to specific executables
TARGET_LINK_LIBRARIES(generate netgen tetgen triangle vgmodeler_volume ${GTSIO_LIBS} pugixml ${CERVPT_LIBS})
#TARGET_LINK_LIBRARIES(mesh tetgen)

