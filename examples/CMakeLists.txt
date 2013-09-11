# add_executable(example1 src/example1.cpp ../external/triangle/triangle.c)

find_package(CGAL QUIET COMPONENTS Core )

set( CGAL_LIBS CGAL CGAL_Core ${Boost_LIBRARIES} gmp mpfr )

add_executable(cgal_plc_2d_direct src/cgal_plc_2d_direct.cpp)
target_link_libraries(cgal_plc_2d_direct ${CGAL_LIBS})

add_executable(cgal_plc_2d_viennamesh src/cgal_plc_2d_viennamesh.cpp)
target_link_libraries(cgal_plc_2d_viennamesh ${CGAL_LIBS})

add_executable(cgal_plc_3d_direct src/cgal_plc_3d_direct.cpp)
target_link_libraries(cgal_plc_3d_direct ${CGAL_LIBS})

add_executable(cgal_plc_3d_viennamesh src/cgal_plc_3d_viennamesh.cpp)
target_link_libraries(cgal_plc_3d_viennamesh ${CGAL_LIBS})

# set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frounding-math" )
# set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -frounding-math" )

add_executable(cgal_tet_direct src/cgal_tet_direct.cpp)
target_link_libraries(cgal_tet_direct ${CGAL_LIBS})

add_executable(cgal_tet_viennamesh src/cgal_tet_viennamesh.cpp)
target_link_libraries(cgal_tet_viennamesh ${CGAL_LIBS})

add_executable(plc2tet_cgal src/plc2tet_cgal.cpp)
target_link_libraries(plc2tet_cgal ${CGAL_LIBS})


include_directories( ${PROJECT_SOURCE_DIR}/external/vgmodeler/gsse/gsse01 )
include_directories( ${PROJECT_SOURCE_DIR}/external/vgmodeler/gsse/gsse01/external/mauch )


add_executable(plc2tet_netgen src/plc2tet_netgen.cpp)
add_dependencies(plc2tet_netgen netgen)
target_link_libraries(plc2tet_netgen ${CGAL_LIBS} nglib)
target_link_libraries(plc2tet_netgen vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)


# add_executable(example7 src/example7.cpp)
# add_dependencies(example7 netgen)
# target_link_libraries(example7 ${CGAL_LIBS} nglib)
# target_link_libraries(example7 vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)
# 
# add_executable(example7a src/example7a.cpp)
# add_dependencies(example7a netgen)
# target_link_libraries(example7a ${CGAL_LIBS} nglib)
# target_link_libraries(example7a vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)
# 
# add_executable(example7b src/example7b.cpp)
# add_dependencies(example7b netgen)
# target_link_libraries(example7b ${CGAL_LIBS} nglib)
# target_link_libraries(example7b vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)

add_executable(aspect_ratio src/aspect_ratio.cpp)
add_dependencies(aspect_ratio netgen)
target_link_libraries(aspect_ratio ${CGAL_LIBS} nglib)
target_link_libraries(aspect_ratio vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)

add_executable(boost_accumulator src/boost_accumulator.cpp)
add_dependencies(boost_accumulator netgen)
target_link_libraries(boost_accumulator ${CGAL_LIBS} nglib)
target_link_libraries(boost_accumulator vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)

add_executable(extract_plc src/extract_plc.cpp)
add_dependencies(extract_plc netgen)
target_link_libraries(extract_plc ${CGAL_LIBS} nglib)
target_link_libraries(extract_plc vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)

# add_executable(example10 src/example10.cpp)
# add_executable(example11 src/example11.cpp)



add_executable(cavity_remesh src/cavity_remesh.cpp)
add_dependencies(cavity_remesh netgen)
target_link_libraries(cavity_remesh ${CGAL_LIBS} nglib tetgen)
target_link_libraries(cavity_remesh vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)


add_executable(tetgen_viennamesh src/tetgen_viennamesh.cpp)
target_link_libraries(tetgen_viennamesh tetgen)


