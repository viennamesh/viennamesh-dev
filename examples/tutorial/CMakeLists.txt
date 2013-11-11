# add_executable(example1 example1.cpp ../external/triangle/triangle.c)

find_package(CGAL QUIET COMPONENTS Core )

set( CGAL_LIBS CGAL CGAL_Core ${Boost_LIBRARIES} gmp mpfr )

add_executable(cgal_plc_2d_direct cgal_plc_2d_direct.cpp)
target_link_libraries(cgal_plc_2d_direct ${CGAL_LIBS})

add_executable(cgal_plc_2d_viennamesh cgal_plc_2d_viennamesh.cpp)
target_link_libraries(cgal_plc_2d_viennamesh ${CGAL_LIBS})

add_executable(cgal_plc_3d_direct cgal_plc_3d_direct.cpp)
target_link_libraries(cgal_plc_3d_direct ${CGAL_LIBS})

add_executable(cgal_plc_3d_viennamesh cgal_plc_3d_viennamesh.cpp)
target_link_libraries(cgal_plc_3d_viennamesh ${CGAL_LIBS})

# set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frounding-math" )
# set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -frounding-math" )

add_executable(cgal_tet_direct cgal_tet_direct.cpp)
target_link_libraries(cgal_tet_direct ${CGAL_LIBS})

add_executable(cgal_tet_viennamesh cgal_tet_viennamesh.cpp)
target_link_libraries(cgal_tet_viennamesh ${CGAL_LIBS})

add_executable(plc2tet_cgal plc2tet_cgal.cpp)
target_link_libraries(plc2tet_cgal ${CGAL_LIBS})


include_directories( ${PROJECT_SOURCE_DIR}/external/vgmodeler/gsse/gsse01 )
include_directories( ${PROJECT_SOURCE_DIR}/external/vgmodeler/gsse/gsse01/external/mauch )


add_executable(plc2tet_netgen plc2tet_netgen.cpp)
add_dependencies(plc2tet_netgen netgen)
target_link_libraries(plc2tet_netgen ${CGAL_LIBS} nglib)
target_link_libraries(plc2tet_netgen vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)


# add_executable(example7 example7.cpp)
# add_dependencies(example7 netgen)
# target_link_libraries(example7 ${CGAL_LIBS} nglib)
# target_link_libraries(example7 vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)
#
# add_executable(example7a example7a.cpp)
# add_dependencies(example7a netgen)
# target_link_libraries(example7a ${CGAL_LIBS} nglib)
# target_link_libraries(example7a vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)
#
# add_executable(example7b example7b.cpp)
# add_dependencies(example7b netgen)
# target_link_libraries(example7b ${CGAL_LIBS} nglib)
# target_link_libraries(example7b vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)

add_executable(aspect_ratio aspect_ratio.cpp)
add_dependencies(aspect_ratio netgen)
target_link_libraries(aspect_ratio ${CGAL_LIBS} nglib)
target_link_libraries(aspect_ratio vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)

add_executable(boost_accumulator boost_accumulator.cpp)
add_dependencies(boost_accumulator netgen)
target_link_libraries(boost_accumulator ${CGAL_LIBS} nglib)
target_link_libraries(boost_accumulator vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)

add_executable(extract_plc extract_plc.cpp)
add_dependencies(extract_plc netgen)
target_link_libraries(extract_plc ${CGAL_LIBS} nglib)
target_link_libraries(extract_plc vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)

# add_executable(example10 example10.cpp)
# add_executable(example11 example11.cpp)



add_executable(cavity_remesh cavity_remesh.cpp)
add_dependencies(cavity_remesh netgen)
target_link_libraries(cavity_remesh ${CGAL_LIBS} nglib tetgen)
target_link_libraries(cavity_remesh vgmodeler_base vgmodeler_stltopo vgmodeler_meshstl)


add_executable(tetgen_viennamesh tetgen_viennamesh.cpp)
target_link_libraries(tetgen_viennamesh tetgen)

add_executable(tetgen_plc_viennamesh tetgen_plc_viennamesh.cpp)
target_link_libraries(tetgen_plc_viennamesh tetgen)

add_executable(triangle_viennamesh triangle_viennamesh.cpp)
target_link_libraries(triangle_viennamesh triangle)


add_executable(mesh_conversion mesh_conversion.cpp)
target_link_libraries(mesh_conversion ${CGAL_LIBS} nglib tetgen)



add_executable(dynamic_mesh dynamic_mesh.cpp)
target_link_libraries(dynamic_mesh ${CGAL_LIBS} tetgen)


add_executable(logger logger.cpp)
