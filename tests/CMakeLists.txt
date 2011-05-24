
PROJECT(ViennaMeshTest)

cmake_minimum_required(VERSION 2.6)


INCLUDE(CTest)
ENABLE_TESTING()
IF(BUILD_TESTING)
  SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
  MARK_AS_ADVANCED(BUILDNAME)
ENDIF(BUILD_TESTING)

# ------------------------------------------------------------------------------
#
# *** SET COMPILER FLAGS ***
#
# ------------------------------------------------------------------------------

# set compiler flags
IF(CMAKE_COMPILER_IS_GNUCXX)
   # Optimize Release Build Type with -O2 instead of -O3
   # and switched of compiler warnings (-w)
   set(CMAKE_CXX_FLAGS_RELEASE "-O2 -w")  
   # Strip release binaries   
   set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")    
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# add specific debug flags
IF(CMAKE_BUILD_TYPE MATCHES Debug)
   SET(CMAKE_VERBOSE_MAKEFILE ON)
   ADD_DEFINITIONS(-Wall -DMESH_KERNEL_DEBUG -DMESH_ADAPTOR_DEBUG)
#   ADD_DEFINITIONS(-Wall -DMESH_KERNEL_DEBUG -DMESH_KERNEL_DEBUG_FULL)
#   ADD_DEFINITIONS(-Wall -DMESH_KERNEL_DEBUG -DIODEBUG)   
#   ADD_DEFINITIONS(-Wall -DMESH_KERNEL_DEBUG -DMESH_KERNEL_DEBUG_FULL -DIODEBUG)   
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

# ------------------------------------------------------------------------------
#
# *** CHECK FOR DEPENDENCIES ***
#
# ------------------------------------------------------------------------------

# set modules directory
#
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/Modules/")

# check for the Boost libraries
#
set(BOOST_MIN_VERSION 1.42.0)
set(BOOST_LIBS thread)
set(Boost_USE_STATIC_LIBS TRUE)
set(Boost_USE_MULTITHREADED TRUE)
find_package(Boost ${BOOST_MIN_VERSION} COMPONENTS ${BOOST_LIBS} REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# check for the PThreads library
#
find_package(Threads REQUIRED)

# check for the GMP library
#
find_package(GMP REQUIRED)
include_directories(${GMP_INCLUDE_DIRS})

# check for the CGAL libraries
#
find_package(CGAL REQUIRED)
ADD_DEFINITIONS(${CGAL_DEFINITIONS})   
include_directories(${CGAL_INCLUDE_DIRS})


SET(ENABLE_GTSIO OFF)

# ------------------------------------------------------------------------------
#
# *** PROCESS RELATED CMAKE PROJECTS ***
#
# ------------------------------------------------------------------------------
INCLUDE_DIRECTORIES("../../ViennaGrid/")
INCLUDE_DIRECTORIES("../../ViennaData/")
INCLUDE_DIRECTORIES("../../ViennaUtils/")
INCLUDE_DIRECTORIES("../../ViennaUtils/external/")
IF(ENABLE_GTSIO)
   INCLUDE_DIRECTORIES("../../ViennaUtils/external/gts/UtilityLibs/gtsutil/src/include")
   INCLUDE_DIRECTORIES("../../ViennaUtils/external/gts/UtilityLibs/gtsio3/src/")
   INCLUDE_DIRECTORIES("../../ViennaUtils/external/gts/3rdPartyTools/libkdtree++")
ENDIF(ENABLE_GTSIO)


# build external libraries
ADD_SUBDIRECTORY(../external "${CMAKE_CURRENT_BINARY_DIR}/external")

# set the include paths
INCLUDE_DIRECTORIES("../")
INCLUDE_DIRECTORIES("../external/triangle-1.6/")
INCLUDE_DIRECTORIES("../external/tetgen-1.4.3/")
INCLUDE_DIRECTORIES("../external/vgmodeler-2.0/")
INCLUDE_DIRECTORIES("../external/vgmodeler-2.0/vgmodeler/hull_adaption/gsse/gsse01")
INCLUDE_DIRECTORIES("../external/vgmodeler-2.0/vgmodeler/hull_adaption/gsse/gsse01/external/mauch")
INCLUDE_DIRECTORIES("../external/cervpt-1.0/")
INCLUDE_DIRECTORIES("../external/netgen-4.9.13/netgen")
INCLUDE_DIRECTORIES("../external/netgen-4.9.13/netgen/nglib")
INCLUDE_DIRECTORIES("../external/netgen-4.9.13/netgen/libsrc/include")
INCLUDE_DIRECTORIES("../external/netgen-4.9.13/netgen/libsrc/meshing")

# ------------------------------------------------------------------------------
#
# *** SET MESH TOOLS RELATED LIBS ***
#
# ------------------------------------------------------------------------------
set(CERVPT_LIBS  ${CGAL_LIBRARIES} ${GMP_LIBRARIES} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
set(VGMODHULL_LIBS vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)
#set(VGMODHULL_LIBS vgmodeler_hull)
IF(ENABLE_GTSIO)
   set(GTSIO_LIBS gtsio3 z)
ENDIF(ENABLE_GTSIO)

# ------------------------------------------------------------------------------
#
# *** SETUP APPLICATIONS ***
#
# ------------------------------------------------------------------------------
ADD_EXECUTABLE(net2d src/netgen_direct_2d.cpp)
ADD_EXECUTABLE(netg src/netgen.cpp)
ADD_EXECUTABLE(metasel src/meta_selection.cpp)
ADD_EXECUTABLE(adapt_hull src/adapt_hull.cpp)

# ------------------------------------------------------------------------------
#
# *** LINK APPLICATIONS AGAINST SPECIFIC LIBRARIES ***
#
# ------------------------------------------------------------------------------
TARGET_LINK_LIBRARIES(net2d netgen)
TARGET_LINK_LIBRARIES(netg netgen ${CERVPT_LIBS})
TARGET_LINK_LIBRARIES(adapt_hull ${CERVPT_LIBS} ${VGMODHULL_LIBS})

# ------------------------------------------------------------------------------
#
# *** RUN TESTS ***
#
# ------------------------------------------------------------------------------
ADD_TEST(net2d net2d)
ADD_TEST(netg netg)
ADD_TEST(adapt_hull1 adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/cyl_mask_etched_flux.bnd)
ADD_TEST(adapt_hull2 adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/litho_shaped_gates_sram_cell.bnd)
ADD_TEST(adapt_hull3 adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/lto_depo_over_lines.bnd)
ADD_TEST(adapt_hull4 adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/pecvd_layer_in_trench.bnd)
ADD_TEST(adapt_hull5 adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/wordline_bitline_dram_cell.bnd)
ADD_TEST(adapt_hull6 adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/etched_hole_with_oxide_etch_model.bnd)
ADD_TEST(adapt_hull7 adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/sputter_etched_trench_with_litho_mask.bnd)
ADD_TEST(adapt_hull8 adapt_hull ../../../ViennaMeshCollection/bnd_examples/EEPROM_FOX.bnd)
ADD_TEST(adapt_hull9 adapt_hull ../../../ViennaMeshCollection/bnd_examples/cu_lowk_leti_right.bnd)
ADD_TEST(adapt_hull10 adapt_hull ../../../ViennaMeshCollection/bnd_examples/after_line_ref.bnd)
ADD_TEST(adapt_hull11 adapt_hull ../../../ViennaMeshCollection/bnd_examples/andi.bnd)




