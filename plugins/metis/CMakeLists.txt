find_package(METIS QUIET)
if (METIS_FOUND)
  message(STATUS "Found METIS")
  message(STATUS "METIS include directories: ${METIS_INCLUDE_DIRS}")
  message(STATUS "METIS libraries: ${METIS_LIBRARIES}")

  include_directories(${METIS_INCLUDE_DIRS})
  VIENNAMESH_ADD_PLUGIN(viennamesh-module-metis plugin.cpp
                        mesh_partitioning.cpp)
  target_link_libraries(viennamesh-module-metis ${METIS_LIBRARIES})
else()
  message(STATUS "METIS not found")
endif()
