
include(CTest)
include(CMakeDependentOption)

# Installation directories
##########################

set(INSTALL_INCLUDE_DIR include CACHE PATH
   "Installation directory for headers")
if(WIN32 AND NOT CYGWIN)
   set(DEF_INSTALL_CMAKE_DIR CMake)
else()
   set(DEF_INSTALL_CMAKE_DIR lib/cmake/viennamesh)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
   "Installation directory for CMake files")

if(NOT IS_ABSOLUTE "${INSTALL_CMAKE_DIR}")
   set(INSTALL_CMAKE_DIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_CMAKE_DIR}")
endif()
file(RELATIVE_PATH CONF_REL_INSTALL_PREFIX "${INSTALL_CMAKE_DIR}"
   "${CMAKE_INSTALL_PREFIX}")
if(NOT IS_ABSOLUTE "${INSTALL_INCLUDE_DIR}")
   set(INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}")
endif()
file(RELATIVE_PATH CONF_REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")

# User options
##############

option(BUILD_EXAMPLES "Build example programs" ON)
option(BUILD_TOOLS "Build tool programs" ON)

option(ENABLE_PEDANTIC_FLAGS "Enable pedantic compiler flags" OFF)

mark_as_advanced(ENABLE_PEDANTIC_FLAGS)

include_directories($ENV{VIENNAGRIDPATH})
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/")

# Set high warning level on GCC
if(ENABLE_PEDANTIC_FLAGS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()


# Export
########

configure_file(cmake/ViennaMeshConfig.cmake.in
   ${PROJECT_BINARY_DIR}/ViennaMeshConfig.cmake @ONLY)

configure_file(cmake/ViennaMeshConfigVersion.cmake.in
   ${PROJECT_BINARY_DIR}/ViennaMeshConfigVersion.cmake @ONLY)

# Install
#########

install(FILES
   ${PROJECT_BINARY_DIR}/ViennaMeshConfig.cmake
   ${PROJECT_BINARY_DIR}/ViennaMeshConfigVersion.cmake
   DESTINATION ${INSTALL_CMAKE_DIR} COMPONENT dev)
