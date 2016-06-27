set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )

if ( COMMAND cmake_policy )
  cmake_policy( SET CMP0003 NEW )
endif()

set(CGAL_DONT_OVERRIDE_CMAKE_FLAGS TRUE CACHE BOOL "" FORCE)
find_package( CGAL QUIET COMPONENTS  )
if ( NOT CGAL_FOUND )
  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()
endif()
include( ${CGAL_USE_FILE} )

find_package(Eigen3 3.0.91)
if(EIGEN3_FOUND)
   include( ${EIGEN3_USE_FILE} )
else()
   message(STATUS "NOTICE: Eigen library is not found.")
endif()
#message(STATUS "TEST ${CGAL_USE_FILE}")

VIENNAMESH_ADD_PLUGIN(viennamesh-module-poisson plugin.cpp
                      poisson_mesh.cpp
                      poisson_reconstruct_surface.cpp
                      poisson_estimate_normals.cpp
                      scale_reconstruction.cpp )

target_link_libraries(viennamesh-module-poisson ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES} )
