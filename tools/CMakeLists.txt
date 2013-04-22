# add_executable(example1 src/example1.cpp ../external/triangle/triangle.c)

find_package(CGAL QUIET COMPONENTS Core )
find_package(Boost COMPONENTS thread program_options)

add_executable(fld2vtu src/fld2vtu.cpp)
target_link_libraries(fld2vtu ${Boost_LIBRARIES})