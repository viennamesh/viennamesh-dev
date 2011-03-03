PROJECT(ViennaUtils)

cmake_minimum_required(VERSION 2.6)

# for full cmake output, activate the following
# SET(CMAKE_VERBOSE_MAKEFILE ON)

# build external
ADD_SUBDIRECTORY(external/)

# set the include paths
INCLUDE_DIRECTORIES(".")
INCLUDE_DIRECTORIES("external/")
INCLUDE_DIRECTORIES("external/gsse01/gsse")
INCLUDE_DIRECTORIES("external/gts/UtilityLibs/gtsutil/src/include")
INCLUDE_DIRECTORIES("external/gts/UtilityLibs/gtsio3/src/")
INCLUDE_DIRECTORIES("external/gts/3rdPartyTools/libkdtree++")
INCLUDE_DIRECTORIES("../ViennaGrid/")
INCLUDE_DIRECTORIES("../ViennaData/")

# TODO remove this .. cmake should build external/gts
# therefore this explicit link path should get obsolete ..
#LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/external/gts/dist/local/native/)

# set the gnu g++ compiler flags
IF(CMAKE_COMPILER_IS_GNUCXX)
   ADD_DEFINITIONS(-Wall -pedantic -O2 -fPIC)       
ENDIF(CMAKE_COMPILER_IS_GNUCXX)



