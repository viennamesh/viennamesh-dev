

# This module defines
#  CGAL_DEFINITIONS: compiler flags for compiling with CGAL
#  CGAL_INCLUDE_DIR: where to find CGAL.h
#  CGAL_LIB_DIR: the libraries needed to use CGAL
#  CGAL_FOUND: if false, do not try to use CGAL

find_package(PkgConfig)

SET(CGAL_DEFINITIONS -frounding-math)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so")

#
# if paths have been provided, test them ..
#
IF(CGAL_INCLUDE AND CGAL_LIB)

   FIND_PATH(CGAL_INCLUDE_DIR CGAL/version.h ${CGAL_INCLUDE} cgal)
   FIND_LIBRARY(CGAL_LIB_DIR NAMES libCGAL.a CGAL PATHS ${CGAL_LIB})

   include(FindPackageHandleStandardArgs)

   find_package_handle_standard_args(CGAL_INC  DEFAULT_MSG CGAL_INCLUDE_DIR)
   find_package_handle_standard_args(CGAL_LIB  DEFAULT_MSG CGAL_LIB_DIR)

   IF( CGAL_INC_FOUND AND CGAL_LIB_FOUND )
      SET(CGAL_FOUND TRUE)
      SET(CGAL_INCLUDE_DIRS ${CGAL_INCLUDE})
      SET(CGAL_LIBRARIES ${CGAL_LIB})
   ELSE()
      SET(CGAL_FOUND FALSE)
   ENDIF()

   MARK_AS_ADVANCED(CGAL_INCLUDE_DIR CGAL_LIB_DIR)
#
# if NO paths have been provided, check potential system paths
#
ELSE(CGAL_INCLUDE AND CGAL_LIB)

   SET(CGAL_DEFAULT_INC_PATHS /usr/include;/usr/local/include)
   SET(CGAL_DEFAULT_LIB_PATHS /usr/lib;/usr/local/lib;/usr/lib64;/usr/local/lib64)

   FIND_PATH(CGAL_INCLUDE_DIR CGAL/version.h ${CGAL_DEFAULT_INC_PATHS})

   FIND_LIBRARY(CGAL_LIB_DIR NAMES libCGAL.a CGAL PATHS ${CGAL_DEFAULT_LIB_PATHS})
    
   include(FindPackageHandleStandardArgs)

   find_package_handle_standard_args(CGAL_INC  DEFAULT_MSG CGAL_INCLUDE_DIR)
   find_package_handle_standard_args(CGAL_LIB  DEFAULT_MSG CGAL_LIB_DIR)

   IF( CGAL_INC_FOUND AND CGAL_LIB_FOUND )
      SET(CGAL_FOUND TRUE)
      SET(CGAL_INCLUDE_DIRS ${CGAL_DEFAULT_INC_PATHS})
      SET(CGAL_LIBRARIES ${CGAL_DEFAULT_LIB_PATHS})
   ELSE()
      SET(CGAL_FOUND FALSE)
   ENDIF()

    MARK_AS_ADVANCED(CGAL_INCLUDE_DIR CGAL_LIB_DIR)
ENDIF(CGAL_INCLUDE AND CGAL_LIB)

