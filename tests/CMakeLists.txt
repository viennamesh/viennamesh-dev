
PROJECT(ViennaMeshTest)

cmake_minimum_required(VERSION 2.6)


INCLUDE(CTest)
ENABLE_TESTING()
IF(BUILD_TESTING)
  SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
  MARK_AS_ADVANCED(BUILDNAME)
ENDIF(BUILD_TESTING)

# build output verbosity
SET(CMAKE_VERBOSE_MAKEFILE ON)

# set compiler flags
IF(CMAKE_COMPILER_IS_GNUCXX)
   ADD_DEFINITIONS(-g -fPIC -DMESH_KERNEL_DEBUG)
   #ADD_DEFINITIONS(-Wall -O0 -g -pedantic -fPIC)
   #ADD_DEFINITIONS(-Wall -pedantic -O2 -fPIC)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# build external libraries
ADD_SUBDIRECTORY(../external "${CMAKE_CURRENT_BINARY_DIR}/external")

# set the include paths
INCLUDE_DIRECTORIES("../")
INCLUDE_DIRECTORIES("../external/triangle-1.6/")
INCLUDE_DIRECTORIES("../external/tetgen-1.4.3/")
INCLUDE_DIRECTORIES("../external/vgmodeler-2.0/")
INCLUDE_DIRECTORIES("../external/vgmodeler-2.0/vgmodeler/volume_generation/include")
INCLUDE_DIRECTORIES("../external/netgen-4.9.13/netgen")
INCLUDE_DIRECTORIES("../external/netgen-4.9.13/netgen/nglib")
INCLUDE_DIRECTORIES("../external/netgen-4.9.13/netgen/libsrc/include")
INCLUDE_DIRECTORIES("../../ViennaGrid/")
INCLUDE_DIRECTORIES("../../ViennaData/")
INCLUDE_DIRECTORIES("../../ViennaUtils/")
INCLUDE_DIRECTORIES("../../ViennaUtils/external/")
INCLUDE_DIRECTORIES("../../ViennaUtils/external/gts/UtilityLibs/gtsutil/src/include")
INCLUDE_DIRECTORIES("../../ViennaUtils/external/gts/UtilityLibs/gtsio3/src/")
INCLUDE_DIRECTORIES("../../ViennaUtils/external/gts/3rdPartyTools/libkdtree++")
INCLUDE_DIRECTORIES("../../ViennaUtils/external/gsse01/")
INCLUDE_DIRECTORIES("../../ViennaUtils/external/gsse01/external/mauch")


#list all source files here
#ADD_EXECUTABLE(net2d src/netgen_direct_2d.cpp)
ADD_EXECUTABLE(vgmodeler src/vgmodeler.cpp)
ADD_EXECUTABLE(netg src/netgen.cpp)
ADD_EXECUTABLE(tetg src/tetgen.cpp)
ADD_EXECUTABLE(classify src/classifier.cpp)

#link specific libraries to specific executables
#TARGET_LINK_LIBRARIES(net2d netgen)
TARGET_LINK_LIBRARIES(vgmodeler vgmodeler_volume)
TARGET_LINK_LIBRARIES(netg netgen)
TARGET_LINK_LIBRARIES(tetg tetgen)
TARGET_LINK_LIBRARIES(classify netgen)

#ADD_TEST(net2d net2d)
ADD_TEST(vgmodeler vgmodeler)
ADD_TEST(netg netg)
ADD_TEST(tetg tetg)
ADD_TEST(classify netg)
