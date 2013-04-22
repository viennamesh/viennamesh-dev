# add_executable(example1 src/example1.cpp ../external/triangle/triangle.c)

find_package(CGAL QUIET COMPONENTS Core )
find_package(Boost COMPONENTS thread program_options)

set( CGAL_LIBS CGAL CGAL_Core ${Boost_LIBRARIES} gmp mpfr )

add_executable(example2 src/example2.cpp)
target_link_libraries(example2 ${CGAL_LIBS})

add_executable(example2a src/example2a.cpp)
target_link_libraries(example2a ${CGAL_LIBS})

add_executable(example3 src/example3.cpp)
target_link_libraries(example3 ${CGAL_LIBS})

add_executable(example3a src/example3a.cpp)
target_link_libraries(example3a ${CGAL_LIBS})

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frounding-math" )
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -frounding-math" )

add_executable(example4 src/example4.cpp)
target_link_libraries(example4 ${CGAL_LIBS})

add_executable(example4a src/example4a.cpp)
target_link_libraries(example4a ${CGAL_LIBS})

add_executable(example5 src/example5.cpp)
target_link_libraries(example5 ${CGAL_LIBS})




include_directories( ${PROJECT_SOURCE_DIR}/external/vgmodeler/gsse/gsse01 )
include_directories( ${PROJECT_SOURCE_DIR}/external/vgmodeler/gsse/gsse01/external/mauch )
link_directories( ${PROJECT_SOURCE_DIR}/external/netgen-5.0.0/nglib/.libs )

add_executable(example6 src/example6.cpp)
target_link_libraries(example6 ${CGAL_LIBS} nglib)
target_link_libraries(example6 vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)


add_executable(example7 src/example7.cpp)
target_link_libraries(example7 ${CGAL_LIBS} nglib)
target_link_libraries(example7 vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)

add_executable(example7a src/example7a.cpp)
target_link_libraries(example7a ${CGAL_LIBS} nglib)
target_link_libraries(example7a vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)

add_executable(example7b src/example7b.cpp)
target_link_libraries(example7b ${CGAL_LIBS} nglib)
target_link_libraries(example7b vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)
