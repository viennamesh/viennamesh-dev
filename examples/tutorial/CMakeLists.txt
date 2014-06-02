if (VIENNAMESH_WITH_TRIANGLE)

  add_executable(triangle_mesher triangle_mesher.cpp)
  target_link_libraries(triangle_mesher viennamesh)

  add_executable(triangle_mesher_sizing_function triangle_mesher_sizing_function.cpp)
  target_link_libraries(triangle_mesher_sizing_function viennamesh)

  add_executable(triangle_mesher_with_seedpoints triangle_mesher_with_seedpoints.cpp)
  target_link_libraries(triangle_mesher_with_seedpoints viennamesh)

endif(VIENNAMESH_WITH_TRIANGLE)


if (VIENNAMESH_WITH_TETGEN)

  add_executable(tetgen_mesher tetgen_mesher.cpp)
  target_link_libraries(tetgen_mesher viennamesh)

  add_executable(tetgen_mesher_with_seedpoints tetgen_mesher_with_seedpoints.cpp)
  target_link_libraries(tetgen_mesher_with_seedpoints viennamesh)

  add_executable(tetgen_seedpoints_3segments tetgen_seedpoints_3segments.cpp)
  target_link_libraries(tetgen_seedpoints_3segments viennamesh)

  add_executable(extract_hull_holes extract_hull_holes.cpp)
  target_link_libraries(extract_hull_holes viennamesh)

  add_executable(reference_parameter reference_parameter.cpp)
  target_link_libraries(reference_parameter viennamesh)

endif(VIENNAMESH_WITH_TETGEN)


if (VIENNAMESH_WITH_NETGEN)

  add_executable(netgen_csg_mesher netgen_csg_mesher.cpp)
  target_link_libraries(netgen_csg_mesher viennamesh)

endif(VIENNAMESH_WITH_NETGEN)


add_executable(extract_hull_example extract_hull_example.cpp)
target_link_libraries(extract_hull_example viennamesh)

add_executable(inplace inplace.cpp)
target_link_libraries(inplace viennamesh)

add_executable(interface_information interface_information.cpp)
target_link_libraries(interface_information viennamesh)

add_executable(mesher1d mesher1d.cpp)
target_link_libraries(mesher1d viennamesh)

add_executable(statistic statistic.cpp)
target_link_libraries(statistic viennamesh)
