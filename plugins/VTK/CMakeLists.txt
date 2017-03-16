set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )

if ( COMMAND cmake_policy )
  cmake_policy( SET CMP0003 NEW )
endif()

find_package( VTK QUIET COMPONENTS  )
if ( NOT VTK_FOUND )
  message(FATAL_ERROR "No VTK Version found")
elseif(VTK_MAJOR_VERSION LESS 7)
  message(STATUS "Found VTK Version ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}")
  message(FATAL_ERROR "This project requires VTK 7.0 or higher")
else()
  message(STATUS "Found VTK Version ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}")
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
