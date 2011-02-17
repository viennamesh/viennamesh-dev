PROJECT(ViennaMesh)

cmake_minimum_required(VERSION 2.6)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

# set the include paths
INCLUDE_DIRECTORIES(".")
INCLUDE_DIRECTORIES("external/triangle-1.6/")
INCLUDE_DIRECTORIES("external/tetgen-1.4.3/")
INCLUDE_DIRECTORIES("../ViennaGrid/")
INCLUDE_DIRECTORIES("../ViennaData/")
INCLUDE_DIRECTORIES("../ViennaUtils/")
INCLUDE_DIRECTORIES("../ViennaUtils/external/")
INCLUDE_DIRECTORIES("../ViennaUtils/external/gts/UtilityLibs/gtsutil/src/include")
INCLUDE_DIRECTORIES("../ViennaUtils/external/gts/UtilityLibs/gtsio3/src/")
INCLUDE_DIRECTORIES("../ViennaUtils/external/gts/3rdPartyTools/libkdtree++")

# build external libraries
add_subdirectory(external)
# 
# # build ViennaUtils
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/../ViennaUtils/ "${CMAKE_CURRENT_BINARY_DIR}/ViennaUtils/")

# REMOVEME
#add_subdirectory(tests)

IF(CMAKE_COMPILER_IS_GNUCXX)
   ADD_DEFINITIONS(-Wall -O0 -g -pedantic)
   #ADD_DEFINITIONS(-Wall -pedantic -O2)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

#list all source files here
ADD_EXECUTABLE(viennamesh src/viennamesh.cpp)

#link specific libraries to specific executables
TARGET_LINK_LIBRARIES(viennamesh gtsio3 tetgen triangle )

