

# This module defines
#  GMP_INCLUDE_DIR
#  GMP_LIB_DIR
#  GMP_FOUND

find_package(PkgConfig)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so")

#
# if paths have been provided, test them ..
#
IF(GMP_LIB)

    FIND_LIBRARY(GMP_LIB_DIR NAMES libgmp.a gmp PATHS ${GMP_LIB})

    SET(GMP_LIBRARIES ${GMP_LIB_DIR})

    include(FindPackageHandleStandardArgs)

    find_package_handle_standard_args(GMP  DEFAULT_MSG GMP_LIB_DIR)

    MARK_AS_ADVANCED(GMP_LIB_DIR)
#
# if NO paths have been provided, check potential system paths
#
ELSE(GMP_LIB)

   SET(GMP_DEFAULT_PATHS /usr/lib;/usr/local/lib;/usr/lib64;/usr/local/lib64)

   FIND_LIBRARY(GMP_LIB_DIR NAMES libgmp.a gmp PATHS ${GMP_DEFAULT_PATHS})

   include(FindPackageHandleStandardArgs)

   find_package_handle_standard_args(GMP  DEFAULT_MSG GMP_LIB_DIR)

   IF( GMP_FOUND )
      SET(GMP_LIBRARIES ${GMP_DEFAULT_PATHS})
   ENDIF()

   MARK_AS_ADVANCED(GMP_LIB_DIR)
ENDIF(GMP_LIB)

