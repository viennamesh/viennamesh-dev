#Add the directory containing all header files from pragmatic (note that pragmatic is header only!)
set(PRAGMATIC_INCLUDES "${CMAKE_SOURCE_DIR}/plugins/pragmatic/headers")
include_directories(${PRAGMATIC_INCLUDES})
message (STATUS "Pragmatic includes: ${PRAGMATIC_INCLUDES}")

#find METIS package
find_package(METIS QUIET)
if (METIS_FOUND)
  message(STATUS "Found METIS")
  message(STATUS "METIS include directories: ${METIS_INCLUDE_DIRS}")
  message(STATUS "METIS libraries: ${METIS_LIBRARIES}")
  include_directories(${METIS_INCLUDE_DIRS})
else()
  message(STATUS "METIS not found")
endif()

#Add plugin to ViennaMesh
VIENNAMESH_ADD_PLUGIN(viennamesh-module-pragmatic 	plugin.cpp
							pragmatic_coarsen.cpp
							pragmatic_refine.cpp
							pragmatic_smooth.cpp
							pragmatic_swapping.cpp
              pragmatic_metis_partitioning.cpp)
if (METIS_FOUND)
  target_link_libraries(viennamesh-module-pragmatic ${METIS_LIBRARIES})
endif()

set(VTK_DIR "/home/lgnam/Desktop/software/VTK-build")
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
target_link_libraries(viennamesh-module-pragmatic ${VTK_LIBRARIES})
