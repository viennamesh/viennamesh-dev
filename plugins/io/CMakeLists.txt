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

VIENNAMESH_ADD_PLUGIN(viennamesh-module-io plugin.cpp
                      common.cpp
                      mesh_reader.cpp
                      mesh_writer.cpp
                      plc_reader.cpp
                      plc_writer.cpp)

target_link_libraries(viennamesh-module-io viennautils_dfise)
target_link_libraries(viennamesh-module-io ${VTK_LIBRARIES})