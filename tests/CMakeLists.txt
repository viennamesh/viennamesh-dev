
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
# *** INSTALL PATHS ***
#
# ------------------------------------------------------------------------------
#message(STATUS "installpath: ${CMAKE_INSTALL_PREFIX}")
set(VIENNAMESH_BIN_PATH "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(VIENNAMESH_LIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/lib")

# ------------------------------------------------------------------------------
#
# *** SET FLAGS ***
#
# ------------------------------------------------------------------------------
SET(ENABLE_TEST_SPECIAL_MESHES OFF)
SET(ENABLE_TEST_ALL_SPECIAL_MESHES OFF)
SET(VIENNAMESH_USE_INTERNAL OFF CACHE BOOL "Build ViennaMesh with internal ViennaStar projects")
SET(VIENNAMESH_USE_MPI ON CACHE BOOL "Build ViennaMesh with MPI support")

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

IF(VIENNAMESH_USE_MPI)
   find_package(MPI REQUIRED)
   set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
   set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
   include_directories(MPI_INCLUDE_PATH)
ENDIF(VIENNAMESH_USE_MPI)

# ------------------------------------------------------------------------------
#
# *** SET VIENNASTAR PATHS ***
#
# ------------------------------------------------------------------------------
IF(VIENNAMESH_USE_INTERNAL)
   message(STATUS "Using Internal ViennaStar Projects")
   IF(NOT DEFINED VIENNAGRID_INC_DIR)
      SET(VIENNAGRID_INC_DIR "../external/ViennaGrid/")
   ENDIF(NOT DEFINED VIENNAGRID_INC_DIR)

   IF(NOT DEFINED VIENNADATA_INC_DIR)
      SET(VIENNADATA_INC_DIR "../external/ViennaData/")
   ENDIF(NOT DEFINED VIENNADATA_INC_DIR)

   IF(NOT DEFINED VIENNAUTILS_INC_DIR)
      SET(VIENNAUTILS_INC_DIR "../external/ViennaUtils/")
   ENDIF(NOT DEFINED VIENNAUTILS_INC_DIR)
   IF(NOT DEFINED VIENNAUTILS_LIB_DIR)   
      SET(VIENNAUTILS_LIB_DIR "../external/ViennaUtils/build/lib")         
   ENDIF(NOT DEFINED VIENNAUTILS_LIB_DIR)   
ELSE(VIENNAMESH_USE_INTERNAL)
   message(STATUS "Using External ViennaStar Projects")
   IF(NOT DEFINED VIENNAGRID_INC_DIR)
      SET(VIENNAGRID_INC_DIR "../../ViennaGrid/")
   ENDIF(NOT DEFINED VIENNAGRID_INC_DIR)

   IF(NOT DEFINED VIENNADATA_INC_DIR)
      SET(VIENNADATA_INC_DIR "../../ViennaData/")
   ENDIF(NOT DEFINED VIENNADATA_INC_DIR)

   IF(NOT DEFINED VIENNAUTILS_INC_DIR)   
      SET(VIENNAUTILS_INC_DIR "../../ViennaUtils/")
   ENDIF(NOT DEFINED VIENNAUTILS_INC_DIR)
   IF(NOT DEFINED VIENNAUTILS_LIB_DIR)      
      SET(VIENNAUTILS_LIB_DIR "../../ViennaUtils/build/lib")        
   ENDIF(NOT DEFINED VIENNAUTILS_LIB_DIR)       
ENDIF(VIENNAMESH_USE_INTERNAL)

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
IF(VIENNAMESH_USE_MPI)
   set(BOOST_LIBS thread mpi serialization)
ELSE()
   set(BOOST_LIBS thread)
ENDIF(VIENNAMESH_USE_MPI)
#set(Boost_USE_STATIC_LIBS TRUE)
#set(Boost_USE_MULTITHREADED TRUE)
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

# check for the ViennaUtils
#
find_package(VIENNAUTILS REQUIRED)
include_directories(${VIENNAUTILS_INCLUDE_DIRS})

# check for the ViennaGrid
#
find_package(VIENNAGRID REQUIRED)
include_directories(${VIENNAGRID_INCLUDE_DIRS})

# check for the ViennaData
#
find_package(VIENNADATA REQUIRED)
include_directories(${VIENNADATA_INCLUDE_DIRS})

# ------------------------------------------------------------------------------
#
# *** PROCESS RELATED CMAKE PROJECTS ***
#
# ------------------------------------------------------------------------------

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
# *** SETUP VIENNAMESH LIBRARY ***
#
# ------------------------------------------------------------------------------
ADD_LIBRARY(viennamesh STATIC ../viennamesh/generation/netgen.cpp 
                              ../viennamesh/generation/tetgen.cpp 
                              ../viennamesh/generation/triangle.cpp 
                              ../viennamesh/generation/cervpt.cpp
                              ../viennamesh/adaptation/orienter.cpp
                              ../viennamesh/adaptation/hull_quality.cpp)

IF(ENABLE_GTSIO)
   TARGET_LINK_LIBRARIES(viennamesh netgen tetgen triangle ${Boost_LIBRARIES} ${CERVPT_LIBS} ${VGMODHULL_LIBS} ${VIENNAUTILS_LIBRARIES} ${GTSIO_LIBS})   
ELSE(ENABLE_GTSIO)
   TARGET_LINK_LIBRARIES(viennamesh netgen tetgen triangle ${Boost_LIBRARIES} ${CERVPT_LIBS} ${VGMODHULL_LIBS} ${VIENNAUTILS_LIBRARIES})   
ENDIF(ENABLE_GTSIO)

# ------------------------------------------------------------------------------
#
# *** SETUP APPLICATIONS ***
#
# ------------------------------------------------------------------------------
ADD_EXECUTABLE(generate_hull  src/generate_hull.cpp)
ADD_EXECUTABLE(orient_hull    src/orient_hull.cpp)
ADD_EXECUTABLE(normals_hull   src/orient_normals_hull.cpp)
ADD_EXECUTABLE(adapt_hull     src/adapt_hull.cpp)
ADD_EXECUTABLE(vol_hull       src/generate_volume_of_hull.cpp)
ADD_EXECUTABLE(vol_adapt_hull src/generate_volume_of_adapt_hull.cpp)
ADD_EXECUTABLE(meta_sel       src/meta_selector.cpp)
ADD_EXECUTABLE(tetgen_test    src/tetgen.cpp)
ADD_EXECUTABLE(triangle_test  src/triangle.cpp)
ADD_EXECUTABLE(direct_domain  src/direct_domain_input.cpp)
IF(VIENNAMESH_USE_MPI)
   ADD_EXECUTABLE(mpi            src/mpi.cpp)
ENDIF(VIENNAMESH_USE_MPI)
# ------------------------------------------------------------------------------
#
# *** LINK APPLICATIONS AGAINST SPECIFIC LIBRARIES ***
#
# ------------------------------------------------------------------------------
TARGET_LINK_LIBRARIES(generate_hull    viennamesh)
TARGET_LINK_LIBRARIES(orient_hull      viennamesh)
TARGET_LINK_LIBRARIES(normals_hull     viennamesh)
TARGET_LINK_LIBRARIES(adapt_hull       viennamesh)
TARGET_LINK_LIBRARIES(vol_hull         viennamesh)
TARGET_LINK_LIBRARIES(vol_adapt_hull   viennamesh)
TARGET_LINK_LIBRARIES(meta_sel         viennamesh)
TARGET_LINK_LIBRARIES(tetgen_test      viennamesh)
TARGET_LINK_LIBRARIES(triangle_test    viennamesh)
TARGET_LINK_LIBRARIES(direct_domain    viennamesh)
IF(VIENNAMESH_USE_MPI)
   TARGET_LINK_LIBRARIES(mpi              viennamesh)
ENDIF(VIENNAMESH_USE_MPI)
# ------------------------------------------------------------------------------
#
# *** RUN TESTS ***
#
# ------------------------------------------------------------------------------

# hull generation process chain 
#
IF(ENABLE_TEST_SPECIAL_MESHES)
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
   IF(ENABLE_TEST_ALL_SPECIAL_MESHES)
      ADD_TEST(generate_hull17 generate_hull ../../../ViennaMeshCollection/bnd_examples/EEPROM_FOX_full.bnd)
   ENDIF(ENABLE_TEST_ALL_SPECIAL_MESHES)
ELSE(ENABLE_TEST_SPECIAL_MESHES)   
   ADD_TEST(generate_hull generate_hull ../../input/meshes/after_line_tri.bnd)
ENDIF(ENABLE_TEST_SPECIAL_MESHES)

# hull orientation process chain 
#
IF(ENABLE_TEST_SPECIAL_MESHES)
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
   IF(ENABLE_TEST_ALL_SPECIAL_MESHES)
      ADD_TEST(orient_hull17 orient_hull ../../../ViennaMeshCollection/bnd_examples/EEPROM_FOX_full.bnd)
   ENDIF(ENABLE_TEST_ALL_SPECIAL_MESHES)
ELSE(ENABLE_TEST_SPECIAL_MESHES)      
   ADD_TEST(orient_hull orient_hull ../../input/meshes/after_line_tri.bnd)
ENDIF(ENABLE_TEST_SPECIAL_MESHES)

# hull orientation normals process chain 
#
IF(ENABLE_TEST_SPECIAL_MESHES)
   ADD_TEST(normals_hull1 normals_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/cyl_mask_etched_flux.bnd)
   ADD_TEST(normals_hull2 normals_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/litho_shaped_gates_sram_cell.bnd)
   ADD_TEST(normals_hull3 normals_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/lto_depo_over_lines.bnd)
   ADD_TEST(normals_hull4 normals_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/pecvd_layer_in_trench.bnd)
   ADD_TEST(normals_hull5 normals_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/wordline_bitline_dram_cell.bnd)
   ADD_TEST(normals_hull6 normals_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/etched_hole_with_oxide_etch_model.bnd)
   ADD_TEST(normals_hull7 normals_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/sputter_etched_trench_with_litho_mask.bnd)
   ADD_TEST(normals_hull8 normals_hull ../../../ViennaMeshCollection/bnd_examples/EEPROM_FOX.bnd)
   ADD_TEST(normals_hull9 normals_hull ../../../ViennaMeshCollection/bnd_examples/cu_lowk_leti_right.bnd)
   ADD_TEST(normals_hull10 normals_hull ../../../ViennaMeshCollection/bnd_examples/after_line_ref.bnd)
   ADD_TEST(normals_hull11 normals_hull ../../../ViennaMeshCollection/bnd_examples/andi.bnd)
   ADD_TEST(normals_hull12 normals_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/imp_deposition_into_trench.bnd)
   ADD_TEST(normals_hull13 normals_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/interconnect_via.bnd)
   ADD_TEST(normals_hull14 normals_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/copper_and_barriers_in_hole.bnd)
   ADD_TEST(normals_hull15 normals_hull ../../../ViennaMeshCollection/bnd_examples/fdsoi_3d_25nm_sram.bnd)
   ADD_TEST(normals_hull16 normals_hull ../../../ViennaMeshCollection/bnd_examples/after_line_tri.bnd)
   IF(ENABLE_TEST_ALL_SPECIAL_MESHES)
      ADD_TEST(normals_hull17 normals_hull ../../../ViennaMeshCollection/bnd_examples/EEPROM_FOX_full.bnd)
   ENDIF(ENABLE_TEST_ALL_SPECIAL_MESHES)
ELSE(ENABLE_TEST_SPECIAL_MESHES)      
   ADD_TEST(normals_hull normals_hull ../../input/meshes/after_line_tri.bnd)
ENDIF(ENABLE_TEST_SPECIAL_MESHES)

# hull adaption process chain 
#
IF(ENABLE_TEST_SPECIAL_MESHES)
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
   IF(ENABLE_TEST_ALL_SPECIAL_MESHES)
      ADD_TEST(adapt_hull13 adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/interconnect_via.bnd)
      ADD_TEST(adapt_hull15 adapt_hull ../../../ViennaMeshCollection/bnd_examples/fdsoi_3d_25nm_sram.bnd)
      ADD_TEST(adapt_hull17 adapt_hull ../../../ViennaMeshCollection/bnd_examples/EEPROM_FOX_full.bnd)
   ENDIF(ENABLE_TEST_ALL_SPECIAL_MESHES)
ELSE(ENABLE_TEST_SPECIAL_MESHES)         
   ADD_TEST(adapt_hull adapt_hull ../../input/meshes/after_line_tri.bnd)
ENDIF(ENABLE_TEST_SPECIAL_MESHES)

# volume mesh of non-adapted hull process chain
#
IF(ENABLE_TEST_SPECIAL_MESHES)
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
   IF(ENABLE_TEST_ALL_SPECIAL_MESHES)
      ADD_TEST(vol_hull9 vol_hull ../../../ViennaMeshCollection/bnd_examples/cu_lowk_leti_right.bnd)
      ADD_TEST(vol_hull13 vol_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/interconnect_via.bnd)
      ADD_TEST(vol_hull15 vol_hull ../../../ViennaMeshCollection/bnd_examples/fdsoi_3d_25nm_sram.bnd)
      ADD_TEST(vol_hull17 vol_hull ../../../ViennaMeshCollection/bnd_examples/EEPROM_FOX_full.bnd)
   ENDIF(ENABLE_TEST_ALL_SPECIAL_MESHES)
ELSE(ENABLE_TEST_SPECIAL_MESHES)         
   ADD_TEST(vol_hull vol_hull ../../input/meshes/after_line_tri.bnd)   
ENDIF(ENABLE_TEST_SPECIAL_MESHES)

# volume mesh of adapted hull process chain
#
IF(ENABLE_TEST_SPECIAL_MESHES)
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
   IF(ENABLE_TEST_ALL_SPECIAL_MESHES)
      ADD_TEST(vol_adapt_hull13 vol_adapt_hull ../../../ViennaMeshCollection/companies/Fraunhofer/2008/originals/interconnect_via.bnd)
      ADD_TEST(vol_adapt_hull15 vol_adapt_hull ../../../ViennaMeshCollection/bnd_examples/fdsoi_3d_25nm_sram.bnd)
      ADD_TEST(vol_adapt_hull17 vol_adapt_hull ../../../ViennaMeshCollection/bnd_examples/EEPROM_FOX_full.bnd)
   ENDIF(ENABLE_TEST_ALL_SPECIAL_MESHES)
ELSE(ENABLE_TEST_SPECIAL_MESHES)         
   ADD_TEST(vol_adapt_hull vol_adapt_hull ../../input/meshes/after_line_tri.bnd)      
ENDIF(ENABLE_TEST_SPECIAL_MESHES)

# miscellaneous
#
ADD_TEST(meta_sel       meta_sel)
ADD_TEST(tetgen_test    tetgen_test ../../input/meshes/after_line_tri.bnd)      
ADD_TEST(direct_domain  direct_domain ../../input/meshes/after_line_tri.bnd)      
#ADD_TEST(triangle_test  triangle_test ../../input/square_geom.sgf)

IF(VIENNAMESH_USE_MPI)
  message(${MPI_EXEC} ${MPI_EXEC_PRE_NUMPROCS_FLAGS} ${MPI_EXEC_NUMPROCS_FLAG} 4 ${MPI_EXEC_POST_NUMPROCS_FLAGS})
  ADD_TEST(mpi mpirun -np 4 mpi  ../../input/meshes/homogenous_cubes_7_segments.bnd)      
ENDIF(VIENNAMESH_USE_MPI)




