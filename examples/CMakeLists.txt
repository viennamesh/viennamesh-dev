# add_executable(example1 src/example1.cpp ../external/triangle/triangle.c)

find_package(CGAL QUIET COMPONENTS Core )
find_package(Boost COMPONENTS thread)

add_executable(example2 src/example2.cpp)
target_link_libraries(example2 CGAL)
target_link_libraries(example2 CGAL_Core)
target_link_libraries(example2 gmp)

add_executable(example2a src/example2a.cpp)
target_link_libraries(example2a CGAL)
target_link_libraries(example2a CGAL_Core)
target_link_libraries(example2a gmp)

add_executable(example3 src/example3.cpp)
target_link_libraries(example3 CGAL)
target_link_libraries(example3 CGAL_Core)
target_link_libraries(example3 gmp)

add_executable(example3a src/example3a.cpp)
target_link_libraries(example3a CGAL)
target_link_libraries(example3a CGAL_Core)
target_link_libraries(example3a gmp)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frounding-math" )
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -frounding-math" )

add_executable(example4 src/example4.cpp)
target_link_libraries(example4 CGAL)
target_link_libraries(example4 CGAL_Core)
target_link_libraries(example4 ${Boost_LIBRARIES})
target_link_libraries(example4 gmp)
target_link_libraries(example4 mpfr)


add_executable(example4a src/example4a.cpp)
target_link_libraries(example4a CGAL)
target_link_libraries(example4a CGAL_Core)
target_link_libraries(example4a ${Boost_LIBRARIES})
target_link_libraries(example4a gmp)
target_link_libraries(example4a mpfr)