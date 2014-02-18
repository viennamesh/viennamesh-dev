add_executable(convert_mesh src/convert_mesh.cpp)
target_link_libraries(convert_mesh viennamesh)

add_executable(generate_tetgen src/generate_tetgen.cpp)
target_link_libraries(generate_tetgen viennamesh)

add_executable(extract_hull src/extract_hull.cpp)
target_link_libraries(extract_hull viennamesh)

add_executable(transform_mesh src/transform_mesh.cpp)
target_link_libraries(transform_mesh viennamesh)
