   

# This module defines
#  GTS_INCLUDE_DIR
#  GTS_LIB_DIR
#  GTS_FOUND

find_package(PkgConfig)
include(FindPackageHandleStandardArgs)

# INCLUDE PART

FIND_PATH(GTS_INCLUDE_DIR UtilityLibs/gtsio3/src/include/gtsio3.h ${GTS_INC_DIR})

set(GTS_INCLUDE_DIRS ${GTS_INCLUDE_DIR})

find_package_handle_standard_args(GTS_INC DEFAULT_MSG GTS_INCLUDE_DIR)

IF( NOT GTS_INC_FOUND )
   IF( GTS_FIND_REQUIRED ) 
      message(FATAL_ERROR "GTS Include directory has not been found")
   ENDIF( GTS_FIND_REQUIRED )
ENDIF( NOT GTS_INC_FOUND )

mark_as_advanced(GTS_INCLUDE_DIR)

# LIBRARY PART
FIND_LIBRARY(GTS_LIBRARY NAMES libgtsio3.a gtsio3 PATHS ${GTS_LIB_DIR})
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so")
SET(GTS_LIBRARIES ${GTS_LIBRARY})
find_package_handle_standard_args(GTS_LIB DEFAULT_MSG GTS_LIBRARY)

IF( NOT GTS_LIB_FOUND )
   IF( GTS_LIB_FIND_REQUIRED ) 
      message(FATAL_ERROR "GTS library has not been found")
   ENDIF( GTS_LIB_FIND_REQUIRED )
ENDIF( NOT GTS_LIB_FOUND )

MARK_AS_ADVANCED(GTS_LIB_DIR)

IF(GTS_INC_FOUND)
   IF(GTS_LIB_FOUND)
      SET(ENABLE_GTSIO ON)
   ENDIF(GTS_LIB_FOUND)
ENDIF(GTS_INC_FOUND)
