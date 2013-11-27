


add_executable(triangle_mesher triangle_mesher.cpp)
target_link_libraries(triangle_mesher triangle)

add_executable(tetgen_mesher tetgen_mesher.cpp)
target_link_libraries(tetgen_mesher tetgen)

add_executable(tetgen_mesh_geometry tetgen_mesh_geometry.cpp)
target_link_libraries(tetgen_mesh_geometry tetgen)

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


add_executable(mesh_yannick mesh_yannick.cpp)
add_executable(nld_extract_hull nld_extract_hull.cpp)
add_executable(nld_coarsen_hull nld_coarsen_hull.cpp)

add_executable(nld_mesh_quad_hull nld_mesh_quad_hull.cpp)
target_link_libraries(nld_mesh_quad_hull triangle)


# add_executable(netgen_csg netgen_csg.cpp)
# target_link_libraries(netgen_csg ${NETGEN_LIBS})

add_executable(netgen_csg_mesher netgen_csg_mesher.cpp)
target_link_libraries(netgen_csg_mesher ${NETGEN_LIBS})



add_executable(tetgen_seedpoints_3segments tetgen_seedpoints_3segments.cpp)
target_link_libraries(tetgen_seedpoints_3segments triangle tetgen)


# add_dependencies(some_netgen_project netgen)



