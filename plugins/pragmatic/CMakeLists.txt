#Add the directory containing all header files from pragmatic (note that pragmatic is header only!)
set(PRAGMATIC_INCLUDES "${CMAKE_SOURCE_DIR}/plugins/pragmatic/headers")
include_directories(${PRAGMATIC_INCLUDES})
message (STATUS "Pragmatic includes: ${PRAGMATIC_INCLUDES}")

#Add plugin to ViennaMesh
VIENNAMESH_ADD_PLUGIN(viennamesh-module-pragmatic 	plugin.cpp
							pragmatic_coarsen.cpp
							pragmatic_refine.cpp
							pragmatic_smooth.cpp
							pragmatic_swapping.cpp)
if (METIS_FOUND)
  target_link_libraries(viennamesh-module-pragmatic ${METIS_LIBRARIES})
endif()

set(VTK_DIR "/home/lgnam/Desktop/software/VTK-build")
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
target_link_libraries(viennamesh-module-pragmatic ${VTK_LIBRARIES})
