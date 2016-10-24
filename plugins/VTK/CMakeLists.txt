set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )

if ( COMMAND cmake_policy )
  cmake_policy( SET CMP0003 NEW )
endif()

find_package( VTK QUIET COMPONENTS  )
if ( NOT VTK_FOUND )
  message(STATUS "This project requires the VTK 7.0 library.")
  return()
endif()
include( ${VTK_USE_FILE} )

VIENNAMESH_ADD_PLUGIN( viennamesh-module-vtk
        plugin.cpp
        vtk_mesh.cpp
        vtk_decimate_pro.cpp
        vtk_quadric_clustering.cpp
        vtk_quadric_decimation.cpp
        vtk_mesh_quality.cpp
        )

target_link_libraries(viennamesh-module-vtk ${VTK_LIBRARIES} ${VTK_3RD_PARTY_LIBRARIES} )
