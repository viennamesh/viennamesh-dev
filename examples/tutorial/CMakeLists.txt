
# add_executable(make_mosfet make_mosfet.cpp)
# target_link_libraries(make_mosfet viennamesh)


add_executable(triangle_mesher triangle_mesher.cpp)
target_link_libraries(triangle_mesher viennamesh)

add_executable(triangle_mesher_sizing_function triangle_mesher_sizing_function.cpp)
target_link_libraries(triangle_mesher_sizing_function viennamesh)

add_executable(triangle_mesher_with_seedpoints triangle_mesher_with_seedpoints.cpp)
target_link_libraries(triangle_mesher_with_seedpoints viennamesh)

add_executable(tetgen_mesher tetgen_mesher.cpp)
target_link_libraries(tetgen_mesher viennamesh)

add_executable(tetgen_mesher_with_seedpoints tetgen_mesher_with_seedpoints.cpp)
target_link_libraries(tetgen_mesher_with_seedpoints viennamesh)

add_executable(extract_hull_example extract_hull_example.cpp)
target_link_libraries(extract_hull_example viennamesh)

add_executable(reference_parameter reference_parameter.cpp)
target_link_libraries(reference_parameter viennamesh)

# add_executable(line_cut_example line_cut_example.cpp)
# target_link_libraries(line_cut_example viennamesh)

add_executable(mesher1d mesher1d.cpp)
target_link_libraries(mesher1d viennamesh)

# add_executable(gts_deva_reader gts_deva_reader.cpp)
# target_link_libraries(gts_deva_reader viennamesh)

# add_executable(mesh_yannick mesh_yannick.cpp)
# target_link_libraries(mesh_yannick viennamesh)

# add_executable(nld_extract_hull nld_extract_hull.cpp)
# target_link_libraries(nld_extract_hull viennamesh)
#
# add_executable(nld_coarsen_hull nld_coarsen_hull.cpp)
# target_link_libraries(nld_coarsen_hull viennamesh)

# add_executable(nld_mesh_quad_hull nld_mesh_quad_hull.cpp)
# target_link_libraries(nld_mesh_quad_hull viennamesh)

add_executable(netgen_csg_mesher netgen_csg_mesher.cpp)
target_link_libraries(netgen_csg_mesher viennamesh)

# add_executable(bnd_to_mphtxt bnd_to_mphtxt.cpp)
# target_link_libraries(bnd_to_mphtxt viennamesh)

# add_executable(bnd_to_vtk bnd_to_vtk.cpp)
# target_link_libraries(bnd_to_vtk viennamesh)

add_executable(tetgen_seedpoints_3segments tetgen_seedpoints_3segments.cpp)
target_link_libraries(tetgen_seedpoints_3segments viennamesh)

# add_executable(mesh_lado mesh_lado.cpp)
# target_link_libraries(mesh_lado viennamesh)

add_executable(inplace inplace.cpp)
target_link_libraries(inplace viennamesh)

# find_package(CGAL QUIET COMPONENTS Core )
# set( CGAL_LIBS CGAL CGAL_Core ${Boost_LIBRARIES} gmp mpfr )
# add_executable(cgal_tetrahedron_mesher cgal_tetrahedron_mesher.cpp)
# target_link_libraries(cgal_tetrahedron_mesher ${CGAL_LIBS} viennamesh)


# add_executable(benchmark_triangle benchmark_triangle.cpp)
# target_link_libraries(benchmark_triangle viennamesh)
#
# add_executable(benchmark_tetgen benchmark_tetgen.cpp)
# target_link_libraries(benchmark_tetgen viennamesh)
#
# add_executable(benchmark_cgal benchmark_cgal.cpp)
# target_link_libraries(benchmark_cgal ${CGAL_LIBS} viennamesh)

