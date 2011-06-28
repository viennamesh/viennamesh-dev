

INCLUDE_DIRECTORIES(".")
INCLUDE_DIRECTORIES("triangle/")

IF(CMAKE_COMPILER_IS_GNUCXX)
   ADD_DEFINITIONS(-DTRILIBRARY -DLINUX -I/usr/X11R6/include -L/usr/X11R6/lib)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY(triangle STATIC triangle/triangle.c)

# ------------------------------------------------------------------------------
#
# *** INSTALL ***
#
# ------------------------------------------------------------------------------
INSTALL(TARGETS triangle
  RUNTIME DESTINATION ${VIENNAMESH_BIN_PATH}
  LIBRARY DESTINATION ${VIENNAMESH_LIB_PATH}
  ARCHIVE DESTINATION ${VIENNAMESH_LIB_PATH})
