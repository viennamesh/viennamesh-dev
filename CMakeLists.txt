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

add_subdirectory(external)
add_subdirectory(tests)

#specify include and source directory
INCLUDE_DIRECTORIES(".")

#add definitions, compiler switches, etc.
IF(DEFINED CMAKE_BUILD_TYPE)
 SET (CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
ELSE()
 SET (CMAKE_BUILD_TYPE Release)
ENDIF()

IF(CMAKE_COMPILER_IS_GNUCXX)
   #ADD_DEFINITIONS(-Wall -O0 -g -lpthread)
   ADD_DEFINITIONS(-Wall -pedantic -O2)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

#list all source files here
ADD_EXECUTABLE(viennamesh src/viennamesh.cpp)

#TODO get rid of the global path to the lib
#try to incorporate the ViennaUtils project somehow ...
ADD_LIBRARY(gtsio3 SHARED IMPORTED)
SET_PROPERTY(TARGET gtsio3 PROPERTY IMPORTED_LOCATION /home/weinbub/git/ViennaUtils/external/gts/dist/local/native/libgtsio3.so)

#link specific libraries to specific executables
TARGET_LINK_LIBRARIES(viennamesh triangle gtsio3)

