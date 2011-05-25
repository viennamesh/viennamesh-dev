
PROJECT(ViennaUtilsTest)

cmake_minimum_required(VERSION 2.6)

# configure dart/cdash support
INCLUDE(CTest)
ENABLE_TESTING()
IF(BUILD_TESTING)
  SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
  MARK_AS_ADVANCED(BUILDNAME)
ENDIF(BUILD_TESTING)

# build external
ADD_SUBDIRECTORY(../external/ "${CMAKE_CURRENT_BINARY_DIR}/external/")

# set the include paths
INCLUDE_DIRECTORIES("../")
INCLUDE_DIRECTORIES("../external/")
INCLUDE_DIRECTORIES("../../ViennaGrid/")
INCLUDE_DIRECTORIES("../../ViennaData/")

# set the gnu g++ compiler flags
IF(CMAKE_COMPILER_IS_GNUCXX)
   ADD_DEFINITIONS(-Wall -pedantic -O2 -fPIC)       
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

SET(PROGS utils config timer contio )

FOREACH(PROG ${PROGS})
   ADD_EXECUTABLE(${PROG} src/${PROG}.cpp)
   ADD_test(${PROG} ${PROG})
ENDFOREACH(PROG)

# link with viennautils libraries
TARGET_LINK_LIBRARIES(config pugixml)

