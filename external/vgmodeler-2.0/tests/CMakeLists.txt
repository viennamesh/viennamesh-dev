PROJECT(VGMODELER)

cmake_minimum_required(VERSION 2.6)

# build output verbosity
SET(CMAKE_VERBOSE_MAKEFILE ON)

# if boost is not in the system path, add it here ..
#SET(BOOSTPATH "/opt/boost")             

# set compiler flags
IF(CMAKE_COMPILER_IS_GNUCXX)
   ADD_DEFINITIONS(-g -fPIC)
   #ADD_DEFINITIONS(-Wall -O0 -g -pedantic -fPIC)
   #ADD_DEFINITIONS(-O2 -fPIC)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


# setup the include paths
INCLUDE_DIRECTORIES(".")
INCLUDE_DIRECTORIES("../")

#list all source files here
ADD_EXECUTABLE(hull_generator src/hull_generator.cpp)

#link specific libraries to specific executables
TARGET_LINK_LIBRARIES(hull_generator)

