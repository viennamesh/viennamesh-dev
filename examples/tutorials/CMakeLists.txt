add_executable(data data.cpp)
target_link_libraries(data viennamesh)

add_executable(file_read_write file_read_write.cpp)
target_link_libraries(file_read_write viennamesh)

add_executable(triangle_mesher triangle_mesher.cpp)
target_link_libraries(triangle_mesher viennamesh)

add_executable(triangle_hull_mesher triangle_hull_mesher.cpp)
target_link_libraries(triangle_hull_mesher viennamesh)

add_executable(tetgen_mesher tetgen_mesher.cpp)
target_link_libraries(tetgen_mesher viennamesh)

add_executable(extract_boundary extract_boundary.cpp)
target_link_libraries(extract_boundary viennamesh)

add_executable(symmetry_2d symmetry_2d.cpp)
target_link_libraries(symmetry_2d viennamesh)

add_executable(symmetry_3d symmetry_3d.cpp)
target_link_libraries(symmetry_3d viennamesh)
