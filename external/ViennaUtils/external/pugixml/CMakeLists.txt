PROJECT(ViennaUtils CXX)

# build the viennautils library
ADD_LIBRARY(pugixml src/pugixml.cpp)

# ------------------------------------------------------------------------------
#
# *** INSTALL ***
#
# ------------------------------------------------------------------------------
INSTALL(TARGETS pugixml 
  RUNTIME DESTINATION ${VIENNAUTILS_BIN_PATH}
  LIBRARY DESTINATION ${VIENNAUTILS_LIB_PATH}
  ARCHIVE DESTINATION ${VIENNAUTILS_LIB_PATH})
