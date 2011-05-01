

# This module defines
#  BOOST_INCLUDE_DIR: where to find BOOST.h
#  BOOST_LIB_DIR: the libraries needed to use BOOST
#  BOOST_FOUND: if false, do not try to use BOOST

find_package(PkgConfig)

SET(BOOST_DEFINITIONS -frounding-math)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so")

#
# if paths have been provided, test them ..
#
IF(BOOST_INCLUDE AND BOOST_LIB)

   FIND_PATH(BOOST_INCLUDE_DIR boost/version.hpp ${BOOST_INCLUDE} cgal)
   FIND_LIBRARY(BOOST_LIB_DIR NAMES libboost_thread.a boost_thread PATHS ${BOOST_LIB})

   include(FindPackageHandleStandardArgs)

   find_package_handle_standard_args(BOOST_INC  DEFAULT_MSG BOOST_INCLUDE_DIR)
   find_package_handle_standard_args(BOOST_LIB  DEFAULT_MSG BOOST_LIB_DIR)

   IF( BOOST_INC_FOUND AND BOOST_LIB_FOUND)
      SET(BOOST_FOUND TRUE)
      SET(BOOST_INCLUDE_DIRS ${BOOST_INCLUDE})
      SET(BOOST_LIBRARIES ${BOOST_LIB})
   ELSE()
      SET(BOOST_FOUND FALSE)
   ENDIF()

   MARK_AS_ADVANCED(BOOST_INCLUDE_DIR BOOST_LIB_DIR)
#
# if NO paths have been provided, check potential system paths
#
ELSE(BOOST_INCLUDE AND BOOST_LIB)

   SET(BOOST_DEFAULT_INC_PATHS /usr/include;/usr/local/include)
   SET(BOOST_DEFAULT_LIB_PATHS /usr/lib;/usr/local/lib;/usr/lib64;/usr/local/lib64)

   FIND_PATH(BOOST_INCLUDE_DIR boost/version.hpp ${BOOST_DEFAULT_INC_PATHS})
   FIND_LIBRARY(BOOST_LIB_DIR NAMES libboost_thread.a boost_thread PATHS ${BOOST_DEFAULT_LIB_PATHS})
    
   include(FindPackageHandleStandardArgs)

   find_package_handle_standard_args(BOOST_INC  DEFAULT_MSG BOOST_INCLUDE_DIR)
   find_package_handle_standard_args(BOOST_LIB  DEFAULT_MSG BOOST_LIB_DIR)

   IF( BOOST_INC_FOUND AND BOOST_LIB_FOUND)
      SET(BOOST_FOUND TRUE)
      SET(BOOST_INCLUDE_DIRS ${BOOST_DEFAULT_INC_PATHS})
      SET(BOOST_LIBRARIES ${BOOST_DEFAULT_LIB_PATHS})
   ELSE()
      SET(BOOST_FOUND FALSE)
   ENDIF()

    MARK_AS_ADVANCED(BOOST_INCLUDE_DIR BOOST_LIB_DIR)
ENDIF(BOOST_INCLUDE AND BOOST_LIB)

