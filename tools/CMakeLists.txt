add_executable(convert src/convert.cpp)
target_link_libraries(convert viennamesh)

add_executable(generate_tetgen src/generate_tetgen.cpp)
target_link_libraries(generate_tetgen viennamesh)
