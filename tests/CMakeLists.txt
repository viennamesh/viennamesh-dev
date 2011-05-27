
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
# *** TEST ALL FILES ***
#
# ------------------------------------------------------------------------------
SET(ENABLE_TESTNONWORKINGINPUT OFF)

# ------------------------------------------------------------------------------
#
# *** USE INTERNAL VIENNASTAR PROJECTS ***
#
# ------------------------------------------------------------------------------
SET(ENABLE_INTERNALVIENNASTAR ON)

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
#    module provided by cmake
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

# check for the GTS library
#
find_package(GTS)
IF(ENABLE_GTSIO)
   # if GTS is available, check for the Z library
   #    module provided by cmake
   #
   find_package(ZLIB REQUIRED)
ENDIF(ENABLE_GTSIO)


# ------------------------------------------------------------------------------
#
# *** PROCESS RELATED CMAKE PROJECTS ***
#
# ------------------------------------------------------------------------------
IF(ENABLE_INTERNALVIENNASTAR)
   message(STATUS "Using Internal ViennaStar Projects")
   INCLUDE_DIRECTORIES("../external/ViennaGrid/")
   INCLUDE_DIRECTORIES("../external/ViennaData/")
   INCLUDE_DIRECTORIES("../external/ViennaUtils/")
   INCLUDE_DIRECTORIES("../external/ViennaUtils/external/")
ELSE(ENABLE_INTERNALVIENNASTAR)
   message(STATUS "Using External ViennaStar Projects")
   INCLUDE_DIRECTORIES("../../ViennaGrid/")
   INCLUDE_DIRECTORIES("../../ViennaData/")
   INCLUDE_DIRECTORIES("../../ViennaUtils/")
   INCLUDE_DIRECTORIES("../../ViennaUtils/external/")
ENDIF(ENABLE_INTERNALVIENNASTAR)

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

IF(ENABLE_GTSIO)
   include_directories("${GTS_INCLUDE_DIRS}/UtilityLibs/gtsutil/src/include")
   include_directories("${GTS_INCLUDE_DIRS}/UtilityLibs/gtsio3/src/include")
   include_directories("${GTS_INCLUDE_DIRS}/UtilityLibs/gtsio3/src/shared/include")   
   include_directories("${GTS_INCLUDE_DIRS}/UtilityLibs/gtsio3/src/manipulators/include")
   include_directories("${GTS_INCLUDE_DIRS}/3rdPartyTools/libkdtree++")
ENDIF(ENABLE_GTSIO)

# ------------------------------------------------------------------------------
#
# *** SET MESH TOOLS RELATED LIBS ***
#
# ------------------------------------------------------------------------------
set(CERVPT_LIBS  ${CGAL_LIBRARIES} ${GMP_LIBRARIES} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
set(VGMODHULL_LIBS vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)
IF(ENABLE_GTSIO)
   set(GTSIO_LIBS ${GTS_LIBRARIES} ${Z_LIBRARIES})
ENDIF(ENABLE_GTSIO)

# ------------------------------------------------------------------------------
#
# *** SETUP APPLICATIONS ***
#
# ------------------------------------------------------------------------------
ADD_EXECUTABLE(generate_hull  src/generate_hull.cpp)
ADD_EXECUTABLE(orient_hull    src/orient_hull.cpp)
ADD_EXECUTABLE(adapt_hull     src/adapt_hull.cpp)
ADD_EXECUTABLE(vol_hull       src/generate_volume_of_hull.cpp)
ADD_EXECUTABLE(vol_adapt_hull src/generate_volume_of_adapt_hull.cpp)
ADD_EXECUTABLE(meta_sel       src/meta_selector.cpp)
ADD_EXECUTABLE(tetgen_test    src/tetgen.cpp)
ADD_EXECUTABLE(triangle_test  src/triangle.cpp)

# ------------------------------------------------------------------------------
#
# *** LINK APPLICATIONS AGAINST SPECIFIC LIBRARIES ***
#
# ------------------------------------------------------------------------------
TARGET_LINK_LIBRARIES(generate_hull    ${CERVPT_LIBS} )
TARGET_LINK_LIBRARIES(orient_hull      ${CERVPT_LIBS} )
TARGET_LINK_LIBRARIES(adapt_hull       ${CERVPT_LIBS} ${VGMODHULL_LIBS})
TARGET_LINK_LIBRARIES(vol_hull         netgen ${CERVPT_LIBS})
TARGET_LINK_LIBRARIES(vol_adapt_hull   netgen ${CERVPT_LIBS} ${VGMODHULL_LIBS})
TARGET_LINK_LIBRARIES(meta_sel         ${CERVPT_LIBS} tetgen netgen)
TARGET_LINK_LIBRARIES(tetgen_test      ${CERVPT_LIBS} tetgen)
TARGET_LINK_LIBRARIES(triangle_test    triangle ${GTSIO_LIBS})

# ------------------------------------------------------------------------------
#
# *** RUN TESTS ***
#
# ------------------------------------------------------------------------------

# hull generation process chain 
#
ADD_TEST(generate_hull1 generate_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/cyl_mask_etched_flux.bnd)
ADD_TEST(generate_hull2 generate_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/litho_shaped_gates_sram_cell.bnd)
ADD_TEST(generate_hull3 generate_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/lto_depo_over_lines.bnd)
ADD_TEST(generate_hull4 generate_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/pecvd_layer_in_trench.bnd)
ADD_TEST(generate_hull5 generate_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/wordline_bitline_dram_cell.bnd)
ADD_TEST(generate_hull6 generate_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/etched_hole_with_oxide_etch_model.bnd)
ADD_TEST(generate_hull7 generate_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/sputter_etched_trench_with_litho_mask.bnd)
ADD_TEST(generate_hull8 generate_hull ../../../ViennaMeshCollection/bnd_examples/EEPROM_FOX.bnd)
ADD_TEST(generate_hull9 generate_hull ../../../ViennaMeshCollection/bnd_examples/cu_lowk_leti_right.bnd)
ADD_TEST(generate_hull10 generate_hull ../../../ViennaMeshCollection/bnd_examples/after_line_ref.bnd)
ADD_TEST(generate_hull11 generate_hull ../../../ViennaMeshCollection/bnd_examples/andi.bnd)
ADD_TEST(generate_hull12 generate_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/imp_deposition_into_trench.bnd)
ADD_TEST(generate_hull13 generate_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/interconnect_via.bnd)
ADD_TEST(generate_hull14 generate_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/copper_and_barriers_in_hole.bnd)
ADD_TEST(generate_hull15 generate_hull ../../../ViennaMeshCollection/bnd_examples/fdsoi_3d_25nm_sram.bnd)
ADD_TEST(generate_hull16 generate_hull ../../../ViennaMeshCollection/bnd_examples/after_line_tri.bnd)
IF(ENABLE_TESTNONWORKINGINPUT)
   ADD_TEST(generate_hull17 generate_hull ../../../ViennaMeshCollection/bnd_examples/EEPROM_FOX_full.bnd)
ENDIF(ENABLE_TESTNONWORKINGINPUT)

# hull orientation process chain 
#
ADD_TEST(orient_hull1 orient_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/cyl_mask_etched_flux.bnd)
ADD_TEST(orient_hull2 orient_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/litho_shaped_gates_sram_cell.bnd)
ADD_TEST(orient_hull3 orient_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/lto_depo_over_lines.bnd)
ADD_TEST(orient_hull4 orient_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/pecvd_layer_in_trench.bnd)
ADD_TEST(orient_hull5 orient_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/wordline_bitline_dram_cell.bnd)
ADD_TEST(orient_hull6 orient_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/etched_hole_with_oxide_etch_model.bnd)
ADD_TEST(orient_hull7 orient_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/sputter_etched_trench_with_litho_mask.bnd)
ADD_TEST(orient_hull8 orient_hull ../../../ViennaMeshCollection/bnd_examples/EEPROM_FOX.bnd)
ADD_TEST(orient_hull9 orient_hull ../../../ViennaMeshCollection/bnd_examples/cu_lowk_leti_right.bnd)
ADD_TEST(orient_hull10 orient_hull ../../../ViennaMeshCollection/bnd_examples/after_line_ref.bnd)
ADD_TEST(orient_hull11 orient_hull ../../../ViennaMeshCollection/bnd_examples/andi.bnd)
ADD_TEST(orient_hull12 orient_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/imp_deposition_into_trench.bnd)
ADD_TEST(orient_hull13 orient_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/interconnect_via.bnd)
ADD_TEST(orient_hull14 orient_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/copper_and_barriers_in_hole.bnd)
ADD_TEST(orient_hull15 orient_hull ../../../ViennaMeshCollection/bnd_examples/fdsoi_3d_25nm_sram.bnd)
ADD_TEST(orient_hull16 orient_hull ../../../ViennaMeshCollection/bnd_examples/after_line_tri.bnd)
IF(ENABLE_TESTNONWORKINGINPUT)
   ADD_TEST(orient_hull17 orient_hull ../../../ViennaMeshCollection/bnd_examples/EEPROM_FOX_full.bnd)
ENDIF(ENABLE_TESTNONWORKINGINPUT)

# hull adaption process chain 
#
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
ADD_TEST(adapt_hull12 adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/imp_deposition_into_trench.bnd)
ADD_TEST(adapt_hull14 adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/copper_and_barriers_in_hole.bnd)
ADD_TEST(adapt_hull16 adapt_hull ../../../ViennaMeshCollection/bnd_examples/after_line_tri.bnd)
IF(ENABLE_TESTNONWORKINGINPUT)
   ADD_TEST(adapt_hull13 adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/interconnect_via.bnd)
   ADD_TEST(adapt_hull15 adapt_hull ../../../ViennaMeshCollection/bnd_examples/fdsoi_3d_25nm_sram.bnd)
   ADD_TEST(adapt_hull17 adapt_hull ../../../ViennaMeshCollection/bnd_examples/EEPROM_FOX_full.bnd)
ENDIF(ENABLE_TESTNONWORKINGINPUT)

# volume mesh of non-adapted hull process chain
#
ADD_TEST(vol_hull1 vol_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/cyl_mask_etched_flux.bnd)
ADD_TEST(vol_hull2 vol_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/litho_shaped_gates_sram_cell.bnd)
ADD_TEST(vol_hull3 vol_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/lto_depo_over_lines.bnd)
ADD_TEST(vol_hull4 vol_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/pecvd_layer_in_trench.bnd)
ADD_TEST(vol_hull5 vol_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/wordline_bitline_dram_cell.bnd)
ADD_TEST(vol_hull6 vol_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/etched_hole_with_oxide_etch_model.bnd)
ADD_TEST(vol_hull7 vol_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/sputter_etched_trench_with_litho_mask.bnd)
ADD_TEST(vol_hull8 vol_hull ../../../ViennaMeshCollection/bnd_examples/EEPROM_FOX.bnd)
ADD_TEST(vol_hull10 vol_hull ../../../ViennaMeshCollection/bnd_examples/after_line_ref.bnd)
ADD_TEST(vol_hull11 vol_hull ../../../ViennaMeshCollection/bnd_examples/andi.bnd)
ADD_TEST(vol_hull12 vol_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/imp_deposition_into_trench.bnd)
ADD_TEST(vol_hull14 vol_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/copper_and_barriers_in_hole.bnd)
ADD_TEST(vol_hull16 vol_hull ../../../ViennaMeshCollection/bnd_examples/after_line_tri.bnd)
IF(ENABLE_TESTNONWORKINGINPUT)
   ADD_TEST(vol_hull9 vol_hull ../../../ViennaMeshCollection/bnd_examples/cu_lowk_leti_right.bnd)
   ADD_TEST(vol_hull13 vol_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/interconnect_via.bnd)
   ADD_TEST(vol_hull15 vol_hull ../../../ViennaMeshCollection/bnd_examples/fdsoi_3d_25nm_sram.bnd)
   ADD_TEST(vol_hull17 vol_hull ../../../ViennaMeshCollection/bnd_examples/EEPROM_FOX_full.bnd)
ENDIF(ENABLE_TESTNONWORKINGINPUT)

# volume mesh of adapted hull process chain
#
ADD_TEST(vol_adapt_hull1 vol_adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/cyl_mask_etched_flux.bnd)
ADD_TEST(vol_adapt_hull2 vol_adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/litho_shaped_gates_sram_cell.bnd)
ADD_TEST(vol_adapt_hull3 vol_adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/lto_depo_over_lines.bnd)
ADD_TEST(vol_adapt_hull4 vol_adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/pecvd_layer_in_trench.bnd)
ADD_TEST(vol_adapt_hull5 vol_adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/wordline_bitline_dram_cell.bnd)
ADD_TEST(vol_adapt_hull6 vol_adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/etched_hole_with_oxide_etch_model.bnd)
ADD_TEST(vol_adapt_hull7 vol_adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/sputter_etched_trench_with_litho_mask.bnd)
ADD_TEST(vol_adapt_hull8 vol_adapt_hull ../../../ViennaMeshCollection/bnd_examples/EEPROM_FOX.bnd)
ADD_TEST(vol_adapt_hull9 vol_adapt_hull ../../../ViennaMeshCollection/bnd_examples/cu_lowk_leti_right.bnd)
ADD_TEST(vol_adapt_hull10 vol_adapt_hull ../../../ViennaMeshCollection/bnd_examples/after_line_ref.bnd)
ADD_TEST(vol_adapt_hull11 vol_adapt_hull ../../../ViennaMeshCollection/bnd_examples/andi.bnd)
ADD_TEST(vol_adapt_hull12 vol_adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/imp_deposition_into_trench.bnd)
ADD_TEST(vol_adapt_hull14 vol_adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/copper_and_barriers_in_hole.bnd)
ADD_TEST(vol_adapt_hull16 vol_adapt_hull ../../../ViennaMeshCollection/bnd_examples/after_line_tri.bnd)
IF(ENABLE_TESTNONWORKINGINPUT)
   ADD_TEST(vol_adapt_hull13 vol_adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/interconnect_via.bnd)
   ADD_TEST(vol_adapt_hull15 vol_adapt_hull ../../../ViennaMeshCollection/bnd_examples/fdsoi_3d_25nm_sram.bnd)
   ADD_TEST(vol_adapt_hull17 vol_adapt_hull ../../../ViennaMeshCollection/bnd_examples/EEPROM_FOX_full.bnd)
ENDIF(ENABLE_TESTNONWORKINGINPUT)

# miscellaneous
#
ADD_TEST(meta_sel       meta_sel)
ADD_TEST(tetgen_test    tetgen_test ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/cyl_mask_etched_flux.bnd)
ADD_TEST(triangle_test  triangle_test ../../input/square_geom.sgf)


