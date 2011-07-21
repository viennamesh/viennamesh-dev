

INCLUDE_DIRECTORIES(".")
INCLUDE_DIRECTORIES("triangle/")

# we experienced problems when we imposed higher optimizations than -O1
IF(CMAKE_COMPILER_IS_GNUCXX)
  ADD_DEFINITIONS(-O1 -DTRILIBRARY)
  IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    ADD_DEFINITIONS(-DLINUX)
  ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")   
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
