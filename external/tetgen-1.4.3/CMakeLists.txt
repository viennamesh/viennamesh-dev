

INCLUDE_DIRECTORIES(".")
INCLUDE_DIRECTORIES("tetgen/")

IF(CMAKE_COMPILER_IS_GNUCXX)
   ADD_DEFINITIONS(-DTETLIBRARY -DNDEBUG)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY(tetgen tetgen/tetgen.cxx tetgen/predicates.cxx)

# ------------------------------------------------------------------------------
#
# *** INSTALL ***
#
# ------------------------------------------------------------------------------
INSTALL(TARGETS tetgen
  RUNTIME DESTINATION ${VIENNAMESH_BIN_PATH}
  LIBRARY DESTINATION ${VIENNAMESH_LIB_PATH}
  ARCHIVE DESTINATION ${VIENNAMESH_LIB_PATH})
