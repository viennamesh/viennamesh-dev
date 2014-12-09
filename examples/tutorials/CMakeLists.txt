add_executable(data data.cpp)
target_link_libraries(data viennamesh)

add_executable(file_read_write file_read_write.cpp)
target_link_libraries(file_read_write viennamesh)

add_executable(triangle_mesher triangle_mesher.cpp)
target_link_libraries(triangle_mesher viennamesh)

add_executable(tetgen_mesher tetgen_mesher.cpp)
target_link_libraries(tetgen_mesher viennamesh)
