#Include Pragmatic and Triangle header files
include_directories(external)

#Add definitions for triangle library
add_definitions( -DTRILIBRARY -DANSI_DECLARATORS -DEXTERNAL_TEST -DNO_TIMER )

option(ENABLE_CGAL_PREDICATES "Enable CGAL Predicates for Tetgen" OFF)

if(ENABLE_CGAL_PREDICATES)
  #Use CGAL's predicatets for tetgen
  add_definitions(-DUSE_CGAL_PREDICATES)
  message(STATUS "Using CGAL predicates for exact arithmetic for TetGen")
  find_package(CGAL REQUIRED)
  if (CGAL_FOUND)
    message(STATUS "Found CGAL")
    message(STATUS "CGAL include directories: ${CGAL_INCLUDE_DIRS}")
    include_directories(${CGAL_INCLUDE_DIRS})
  else()
    message(FATAL ERROR "CGAL not found")
  endif()
else()
  message(STATUS "Using Shewchuk's exact arithmetic for TetGen")
endif()

#Set flag if OpenMP has been found on the system or output an error if not, since it is required for the use of pragmatic
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    add_definitions(-DHAVE_OPENMP)
    message(STATUS "Found OPENMP")
    message(STATUS "${CMAKE_C_FLAGS}")
    message(STATUS "${CMAKE_CXX_FLAGS}")
else()
    message(FATAL_ERROR "OpenMP not found")
endif()

#find Eigen package
#find_package(Eigen3 REQUIRED)
#if (TARGET Eigen3::Eigen)
  #message(STATUS "Found Eigen3")
  #include_directories(${EIGEN3_INCLUDE_DIR})
#else()
  #message(FATAL_ERROR "Eigen3 not found!")
#endif()

#find METIS package
find_package(METIS QUIET)
if (METIS_FOUND)
  message(STATUS "Found METIS")
  message(STATUS "METIS include directories: ${METIS_INCLUDE_DIRS}")
  message(STATUS "METIS libraries: ${METIS_LIBRARIES}")
  include_directories(${METIS_INCLUDE_DIRS})

else()
  message(FATAL_ERROR "METIS not found")
endif()

#find MTMETIS library
find_library(MTMETIS_LIBRARY 
             NAMES mtmetis
             HINTS /usr/local/lib/
             PATH_SUFFIXES lib)
message(STATUS "MTMETIS library: ${MTMETIS_LIBRARY}")

find_library(WILDRIVER_LIBRARY 
             NAMES wildriver
             HINTS /usr/local/lib/
             PATH_SUFFIXES lib)
message(STATUS "WILDRIVER library: ${WILDRIVER_LIBRARY}")

#include GMP
include_directories(/usr/lib/)

#Add ViennaMesh Plugin
VIENNAMESH_ADD_PLUGIN(viennamesh-module-color-refinement plugin.cpp
                      color_refinement.cpp
                      pragmatic_mesh.cpp
                      external/triangle.c
                      triangle_interface.c  
                      consistency_check.cpp                    
                      external/tetgen.cxx
                      external/predicates.cxx)

#Link with VTK library
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
target_link_libraries(viennamesh-module-color-refinement ${VTK_LIBRARIES})

#Link with METIS library
target_link_libraries(viennamesh-module-color-refinement ${METIS_LIBRARIES})

#Link also with MTMETS
target_link_libraries(viennamesh-module-color-refinement ${MTMETIS_LIBRARY} ${WILDRIVER_LIBRARY})

if(ENABLE_CGAL_PREDICATES)
  #Link with GMP
  target_link_libraries(viennamesh-module-color-refinement gmp)
else()
endif()