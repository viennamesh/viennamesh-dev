


add_executable(triangle_mesher triangle_mesher.cpp)
target_link_libraries(triangle_mesher triangle)

add_executable(tetgen_mesher tetgen_mesher.cpp)
target_link_libraries(tetgen_mesher tetgen)

add_executable(extract_hull extract_hull.cpp)

add_executable(seed_point_locator_1d seed_point_locator_1d.cpp)
target_link_libraries(seed_point_locator_1d triangle tetgen)

add_executable(seed_point_locator_2d seed_point_locator_2d.cpp)
target_link_libraries(seed_point_locator_2d triangle tetgen)

add_executable(seed_point_locator_3d seed_point_locator_3d.cpp)
target_link_libraries(seed_point_locator_3d triangle tetgen)

add_executable(reference_parameter reference_parameter.cpp)
target_link_libraries(reference_parameter tetgen)

add_executable(mesher1d mesher1d.cpp)


# add_dependencies(some_netgen_project netgen)



