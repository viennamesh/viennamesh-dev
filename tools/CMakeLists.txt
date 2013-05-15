# add_executable(example1 src/example1.cpp ../external/triangle/triangle.c)

add_executable(fld2vtu src/fld2vtu.cpp)
target_link_libraries(fld2vtu ${Boost_LIBRARIES})