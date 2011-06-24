# ------------------------------------------------------------------------------
#
# *** PROJECT DETAILS***
#
# ------------------------------------------------------------------------------
PROJECT(ViennaMesh)
SET( ${PROJECT_NAME}_MAJOR_VERSION 0 )
SET( ${PROJECT_NAME}_MINOR_VERSION 2 )
SET( ${PROJECT_NAME}_PATCH_LEVEL   2 )


cmake_minimum_required(VERSION 2.6)


# ------------------------------------------------------------------------------
#
# *** SET FLAGS ***
#
# ------------------------------------------------------------------------------
SET(ENABLE_INTERNALVIENNASTAR OFF)

# ------------------------------------------------------------------------------
#
# *** SET BUILD FLAGS ***
#
# ------------------------------------------------------------------------------
IF(CMAKE_COMPILER_IS_GNUCXX)
   # build all with the -fPIC flag
   #set(CMAKE_CXX_FLAGS "-fPIC")

   # Optimize Release Build Type with -O2 instead of -O3
   # and switched of compiler warnings (-w)
   set(CMAKE_CXX_FLAGS_RELEASE "-O2 -w")  
   # Strip release binaries   
   set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")    

   # add specific debug flags
   IF(CMAKE_BUILD_TYPE MATCHES Debug)
      SET(CMAKE_VERBOSE_MAKEFILE ON)
      ADD_DEFINITIONS(-Wall -DMESH_KERNEL_DEBUG -DMESH_ADAPTOR_DEBUG)
   #   ADD_DEFINITIONS(-Wall -DMESH_KERNEL_DEBUG -DMESH_KERNEL_DEBUG_FULL)
   #   ADD_DEFINITIONS(-Wall -DMESH_KERNEL_DEBUG -DIODEBUG)   
   #   ADD_DEFINITIONS(-Wall -DMESH_KERNEL_DEBUG -DMESH_KERNEL_DEBUG_FULL -DIODEBUG)   
   ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# ------------------------------------------------------------------------------
#
# *** CHECK FOR DEPENDENCIES ***
#
# ------------------------------------------------------------------------------

# set modules directory
#
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

# check for the Boost libraries
#
set(BOOST_MIN_VERSION 1.42.0)
set(BOOST_LIBS thread)
#set(Boost_USE_STATIC_LIBS TRUE)
#set(Boost_USE_MULTITHREADED TRUE)
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

# ------------------------------------------------------------------------------
#
# *** PROCESS RELATED CMAKE PROJECTS ***
#
# ------------------------------------------------------------------------------
IF(ENABLE_INTERNALVIENNASTAR)
   message(STATUS "Using Internal ViennaStar Projects")
   INCLUDE_DIRECTORIES("external/ViennaGrid/")
   INCLUDE_DIRECTORIES("external/ViennaData/")
   INCLUDE_DIRECTORIES("external/ViennaUtils/")
   INCLUDE_DIRECTORIES("external/ViennaUtils/external/")
ELSE(ENABLE_INTERNALVIENNASTAR)
   message(STATUS "Using External ViennaStar Projects")
   INCLUDE_DIRECTORIES("../ViennaGrid/")
   INCLUDE_DIRECTORIES("../ViennaData/")
   INCLUDE_DIRECTORIES("../ViennaUtils/")
   INCLUDE_DIRECTORIES("../ViennaUtils/external/")
ENDIF(ENABLE_INTERNALVIENNASTAR)


# build external libraries
#   those have to be build bevore the collected include 
#   hierarchy of viennamesh is established here
#   otherwise the netgen and vgmodeler libraries won't compile
ADD_SUBDIRECTORY(external)

# build ViennaUtils:pugixml support
IF(ENABLE_INTERNALVIENNASTAR)
ADD_SUBDIRECTORY(external/ViennaUtils/external/pugixml/ "${CMAKE_CURRENT_BINARY_DIR}/ViennaUtils/external/pugixml/")
ELSE(ENABLE_INTERNALVIENNASTAR)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/../ViennaUtils/external/pugixml/ "${CMAKE_CURRENT_BINARY_DIR}/ViennaUtils/external/pugixml/")
ENDIF(ENABLE_INTERNALVIENNASTAR)

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
# *** SETUP APPLICATIONS ***
#
# ------------------------------------------------------------------------------

ADD_LIBRARY(viennamesh STATIC viennamesh/generation/netgen.cpp 
                       viennamesh/generation/tetgen.cpp 
                       viennamesh/generation/triangle.cpp 
                       viennamesh/generation/cervpt.cpp
                       viennamesh/adaptation/orienter.cpp
                       viennamesh/adaptation/hull_quality.cpp)

#list all source files here
#ADD_EXECUTABLE(vmesh src/vmesh.cpp)
ADD_EXECUTABLE(dev src/dev.cpp)
#ADD_EXECUTABLE(apptest src/apptest.cpp)


# ------------------------------------------------------------------------------
#
# *** LINK APPLICATIONS AGAINST SPECIFIC LIBRARIES ***
#
# ------------------------------------------------------------------------------

#link specific libraries to specific executables  ${VGMODHULL_LIBS} netgen
#IF(ENABLE_GTSIO)
#   TARGET_LINK_LIBRARIES(vmesh viennamesh ${GTSIO_LIBS})
#ELSE(ENABLE_GTSIO)
#   TARGET_LINK_LIBRARIES(vmesh viennamesh)   
#ENDIF(ENABLE_GTSIO)

TARGET_LINK_LIBRARIES(viennamesh netgen tetgen triangle ${CERVPT_LIBS} ${VGMODHULL_LIBS})   
TARGET_LINK_LIBRARIES(dev viennamesh)   

