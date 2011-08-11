# ------------------------------------------------------------------------------
#
# *** PROJECT DETAILS***
#
# ------------------------------------------------------------------------------
PROJECT(ViennaMesh)
SET( ${PROJECT_NAME}_MAJOR_VERSION 0 )
SET( ${PROJECT_NAME}_MINOR_VERSION 2 )
SET( ${PROJECT_NAME}_PATCH_LEVEL   3 )


cmake_minimum_required(VERSION 2.6)

# ------------------------------------------------------------------------------
#
# *** INSTALL PATHS ***
#
# ------------------------------------------------------------------------------
#message(STATUS "installpath: ${CMAKE_INSTALL_PREFIX}")
set(VIENNAMESH_BIN_PATH "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(VIENNAMESH_LIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/lib")

# ------------------------------------------------------------------------------
#
# *** SET FLAGS ***
#
# ------------------------------------------------------------------------------
SET(VIENNAMESH_USE_INTERNAL OFF CACHE BOOL "Build ViennaMesh with internal ViennaStar projects")
SET(VIENNAMESH_USE_MPI OFF CACHE BOOL "Build ViennaMesh with MPI support")
SET(VIENNAMESH_USE_MT ON CACHE BOOL "Build ViennaMesh with Multi-Threading support")

# ------------------------------------------------------------------------------
#
# *** SET COMPILER/LINKER FLAGS ***
#
# ------------------------------------------------------------------------------
IF(CMAKE_COMPILER_IS_GNUCXX)
   # Optimize Release Build Type with -O2 instead of -O3
   # and switched of compiler warnings (-w)
   set(CMAKE_CXX_FLAGS_RELEASE "-O2 -w")  
   # Strip release binaries   
   #set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")    

   ADD_DEFINITIONS(-DMESH_STATISTICS)

   # add specific debug flags
   IF(CMAKE_BUILD_TYPE MATCHES Debug)
      SET(CMAKE_VERBOSE_MAKEFILE ON)
      ADD_DEFINITIONS(-Wall -DMESH_KERNEL_DEBUG -DMESH_ADAPTOR_DEBUG)
   #   ADD_DEFINITIONS(-Wall -DMESH_KERNEL_DEBUG -DMESH_KERNEL_DEBUG_FULL)
   #   ADD_DEFINITIONS(-Wall -DMESH_KERNEL_DEBUG -DIODEBUG)   
   #   ADD_DEFINITIONS(-Wall -DMESH_KERNEL_DEBUG -DMESH_KERNEL_DEBUG_FULL -DIODEBUG)   
   ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

IF(VIENNAMESH_USE_MT)
   ADD_DEFINITIONS(-DVIENNAMESH_USE_MT)
ENDIF(VIENNAMESH_USE_MT)

IF(VIENNAMESH_USE_MPI)
   find_package(MPI REQUIRED)
   set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
   set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
   include_directories(MPI_INCLUDE_PATH)
ENDIF(VIENNAMESH_USE_MPI)

# ------------------------------------------------------------------------------
#
# *** SET VIENNASTAR PATHS ***
#
# ------------------------------------------------------------------------------
IF(VIENNAMESH_USE_INTERNAL)
   message(STATUS "Using Internal ViennaStar Projects")
   IF(NOT DEFINED VIENNAGRID_INC_DIR)
      SET(VIENNAGRID_INC_DIR "external/ViennaGrid/")
   ENDIF(NOT DEFINED VIENNAGRID_INC_DIR)

   IF(NOT DEFINED VIENNADATA_INC_DIR)
      SET(VIENNADATA_INC_DIR "external/ViennaData/")
   ENDIF(NOT DEFINED VIENNADATA_INC_DIR)

   IF(NOT DEFINED VIENNAUTILS_INC_DIR)
      SET(VIENNAUTILS_INC_DIR "external/ViennaUtils/")
   ENDIF(NOT DEFINED VIENNAUTILS_INC_DIR)
   IF(NOT DEFINED VIENNAUTILS_LIB_DIR)   
      SET(VIENNAUTILS_LIB_DIR "external/ViennaUtils/build/lib")         
   ENDIF(NOT DEFINED VIENNAUTILS_LIB_DIR)   
ELSE(VIENNAMESH_USE_INTERNAL)
   message(STATUS "Using External ViennaStar Projects")
   IF(NOT DEFINED VIENNAGRID_INC_DIR)
      SET(VIENNAGRID_INC_DIR "../ViennaGrid/")
   ENDIF(NOT DEFINED VIENNAGRID_INC_DIR)

   IF(NOT DEFINED VIENNADATA_INC_DIR)
      SET(VIENNADATA_INC_DIR "../ViennaData/")
   ENDIF(NOT DEFINED VIENNADATA_INC_DIR)

   IF(NOT DEFINED VIENNAUTILS_INC_DIR)   
      SET(VIENNAUTILS_INC_DIR "../ViennaUtils/")
   ENDIF(NOT DEFINED VIENNAUTILS_INC_DIR)
   IF(NOT DEFINED VIENNAUTILS_LIB_DIR)      
      SET(VIENNAUTILS_LIB_DIR "../ViennaUtils/build/lib")        
   ENDIF(NOT DEFINED VIENNAUTILS_LIB_DIR)       
ENDIF(VIENNAMESH_USE_INTERNAL)


# ------------------------------------------------------------------------------
#
# *** CHECK FOR DEPENDENCIES ***
#
# ------------------------------------------------------------------------------

# set modules directory
#
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

# check for the Boost libraries
#
set(BOOST_MIN_VERSION 1.42.0)
IF(VIENNAMESH_USE_MPI)
   set(BOOST_LIBS thread mpi serialization)
ELSE()
   set(BOOST_LIBS thread)
ENDIF(VIENNAMESH_USE_MPI)
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

# check for the GTS library
#
find_package(GTS)
IF(ENABLE_GTSIO)
   # if GTS is available, check for the Z library
   #    module provided by cmake
   #
   find_package(ZLIB REQUIRED)
ENDIF(ENABLE_GTSIO)

# check for the ViennaUtils
#
find_package(VIENNAUTILS REQUIRED)
include_directories(${VIENNAUTILS_INCLUDE_DIRS})

# check for the ViennaGrid
#
find_package(VIENNAGRID REQUIRED)
include_directories(${VIENNAGRID_INCLUDE_DIRS})

# check for the ViennaData
#
find_package(VIENNADATA REQUIRED)
include_directories(${VIENNADATA_INCLUDE_DIRS})

include_directories(external/threadpool-0.2.5/threadpool)

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

INCLUDE_DIRECTORIES(".")
INCLUDE_DIRECTORIES("external/triangle-1.6/")
INCLUDE_DIRECTORIES("external/tetgen-1.4.3/")
INCLUDE_DIRECTORIES("external/vgmodeler-2.0/")
INCLUDE_DIRECTORIES("external/vgmodeler-2.0/vgmodeler/hull_adaption/gsse/gsse01")
INCLUDE_DIRECTORIES("external/vgmodeler-2.0/vgmodeler/hull_adaption/gsse/gsse01/external/mauch")
INCLUDE_DIRECTORIES("external/cervpt-1.0/")
INCLUDE_DIRECTORIES("external/netgen-4.9.13/netgen")
INCLUDE_DIRECTORIES("external/netgen-4.9.13/netgen/nglib")
INCLUDE_DIRECTORIES("external/netgen-4.9.13/netgen/libsrc/include")
INCLUDE_DIRECTORIES("external/netgen-4.9.13/netgen/libsrc/meshing")

IF(ENABLE_GTSIO)
   include_directories("${GTS_INCLUDE_DIRS}/UtilityLibs/gtsutil/src/include")
   include_directories("${GTS_INCLUDE_DIRS}/UtilityLibs/gtsio3/src/include")
   include_directories("${GTS_INCLUDE_DIRS}/UtilityLibs/gtsio3/src/shared/include")   
   include_directories("${GTS_INCLUDE_DIRS}/UtilityLibs/gtsio3/src/manipulators/include")
   include_directories("${GTS_INCLUDE_DIRS}/3rdPartyTools/libkdtree++")
ENDIF(ENABLE_GTSIO)

# ------------------------------------------------------------------------------
#
# *** SET MESH TOOLS RELATED LIBS ***
#
# ------------------------------------------------------------------------------
set(CERVPT_LIBS  ${CGAL_LIBRARIES} ${GMP_LIBRARIES} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
set(VGMODHULL_LIBS vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)
IF(ENABLE_GTSIO)
   set(GTSIO_LIBS ${GTS_LIBRARIES} ${Z_LIBRARIES})
ENDIF(ENABLE_GTSIO)

# ------------------------------------------------------------------------------
#
# *** SETUP VIENNAMESH LIBRARY ***
#
# ------------------------------------------------------------------------------
ADD_LIBRARY(viennamesh viennamesh/generation/netgen.cpp 
                       viennamesh/generation/tetgen.cpp 
                       viennamesh/generation/triangle.cpp 
                       viennamesh/generation/cervpt.cpp
                       viennamesh/adaptation/orienter.cpp
                       viennamesh/adaptation/hull_quality.cpp
                       viennamesh/adaptation/topology_check.cpp
                       viennamesh/adaptation/geometry_check.cpp)

IF(ENABLE_GTSIO)
   TARGET_LINK_LIBRARIES(viennamesh netgen tetgen triangle ${Boost_LIBRARIES} ${CERVPT_LIBS} ${VGMODHULL_LIBS} ${VIENNAUTILS_LIBRARIES} ${GTSIO_LIBS})   
ELSE(ENABLE_GTSIO)
   TARGET_LINK_LIBRARIES(viennamesh netgen tetgen triangle ${Boost_LIBRARIES} ${CERVPT_LIBS} ${VGMODHULL_LIBS} ${VIENNAUTILS_LIBRARIES})   
ENDIF(ENABLE_GTSIO)

# ------------------------------------------------------------------------------
#
# *** SETUP APPLICATIONS ***
#
# ------------------------------------------------------------------------------
ADD_EXECUTABLE(vmesh_hull           src/hull.cpp)
ADD_EXECUTABLE(vmesh_adapthull      src/hull_adapt.cpp)
ADD_EXECUTABLE(vmesh_checkhull      src/hull_checker.cpp)
ADD_EXECUTABLE(vmesh_adaptvolng     src/vol_adapt_netgen.cpp)
ADD_EXECUTABLE(vmesh_adaptvoltet    src/vol_adapt_tetgen.cpp)
ADD_EXECUTABLE(vmesh_volng          src/vol_netgen.cpp)
ADD_EXECUTABLE(vmesh_voltet         src/vol_tetgen.cpp)
ADD_EXECUTABLE(vmesh_areatri        src/area_triangle.cpp)

TARGET_LINK_LIBRARIES(vmesh_hull          viennamesh)
TARGET_LINK_LIBRARIES(vmesh_adapthull     viennamesh)
TARGET_LINK_LIBRARIES(vmesh_checkhull     viennamesh)
TARGET_LINK_LIBRARIES(vmesh_adaptvolng    viennamesh)
TARGET_LINK_LIBRARIES(vmesh_adaptvoltet   viennamesh)
TARGET_LINK_LIBRARIES(vmesh_volng         viennamesh)
TARGET_LINK_LIBRARIES(vmesh_voltet        viennamesh)
TARGET_LINK_LIBRARIES(vmesh_areatri       viennamesh)

# ------------------------------------------------------------------------------
#
# *** INSTALL ***
#
# ------------------------------------------------------------------------------
INSTALL(TARGETS viennamesh vmesh_hull vmesh_adapthull vmesh_checkhull 
  vmesh_adaptvolng vmesh_adaptvoltet vmesh_volng vmesh_voltet vmesh_areatri
  RUNTIME DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin
  LIBRARY DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/lib
  ARCHIVE DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/lib)





