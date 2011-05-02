

# This module defines
#  GMP_INCLUDE_DIR
#  GMP_LIB_DIR
#  GMP_FOUND

find_package(PkgConfig)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so")

FIND_LIBRARY(GMP_LIBRARY NAMES libgmp.a gmp)

SET(GMP_LIBRARIES ${GMP_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP  DEFAULT_MSG GMP_LIBRARY)

MARK_AS_ADVANCED(GMP_LIB_DIR)

# if the required keyword has been used to invoke this module, 
# stop the configuration process by issuing a fatal error
#
IF( NOT GMP_FOUND )
   IF( GMP_FIND_REQUIRED ) 
      message(FATAL_ERROR "GMP library has not been found")
   ENDIF( GMP_FIND_REQUIRED )
ENDIF( NOT GMP_FOUND )

