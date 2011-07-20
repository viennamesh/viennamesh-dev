

# This module defines
#  CGAL_DEFINITIONS: compiler flags for compiling with CGAL
#  CGAL_INCLUDE_DIR: where to find CGAL.h
#  CGAL_LIB_DIR: the libraries needed to use CGAL
#  CGAL_FOUND: if false, do not try to use CGAL

find_package(PkgConfig)
include(FindPackageHandleStandardArgs)

SET(CGAL_DEFINITIONS -frounding-math)

# INCLUDE PART

FIND_PATH(CGAL_INCLUDE_DIR CGAL/version.h ${CGAL_INC_DIR})

set(CGAL_INCLUDE_DIRS ${CGAL_INCLUDE_DIR} )

find_package_handle_standard_args(CGAL_INC DEFAULT_MSG CGAL_INCLUDE_DIR)

mark_as_advanced(CGAL_INCLUDE_DIR)

IF( NOT CGAL_INC_FOUND )
   IF( CGAL_FIND_REQUIRED ) 
      message(FATAL_ERROR "CGAL Include directory has not been found")
   ENDIF( CGAL_FIND_REQUIRED )
ENDIF( NOT CGAL_INC_FOUND )

# LIBRARY PART

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so;.dylib")
FIND_LIBRARY(CGAL_LIBRARY NAMES libCGAL.a CGAL PATHS ${CGAL_LIB_DIR})
find_package_handle_standard_args(CGAL_LIB DEFAULT_MSG CGAL_LIBRARY)
mark_as_advanced(CGAL_LIBRARY)

set(CGAL_LIBRARIES ${CGAL_LIBRARY} )

IF( NOT CGAL_LIB_FOUND )
   IF( CGAL_FIND_REQUIRED ) 
      message(FATAL_ERROR "CGAL Library has not been found")
   ENDIF( CGAL_FIND_REQUIRED )
ENDIF( NOT CGAL_LIB_FOUND )

set(CGAL_LIBRARIES ${CGAL_LIBRARY} )
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so;.dylib")
FIND_LIBRARY(CGAL_CORE_LIBRARY NAMES libCGAL_Core.a CGAL_Core PATHS ${CGAL_LIB_DIR})
find_package_handle_standard_args(CGAL_CORE_LIB DEFAULT_MSG CGAL_CORE_LIBRARY)
mark_as_advanced(CGAL_CORE_LIBRARY)

set(CGAL_LIBRARIES ${CGAL_LIBRARIES} ${CGAL_CORE_LIBRARY})

IF( NOT CGAL_CORE_LIB_FOUND )
   IF( CGAL_FIND_REQUIRED ) 
      message(FATAL_ERROR "CGAL CORE Library has not been found")
   ENDIF( CGAL_FIND_REQUIRED )
ENDIF( NOT CGAL_CORE_LIB_FOUND )
