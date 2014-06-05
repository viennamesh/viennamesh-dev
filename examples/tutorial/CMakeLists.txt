set (TUTORIAL_PROGRAMS extract_hull_example inplace interface_information mesher1d statistic)

if (VIENNAMESH_WITH_TRIANGLE)
  set(TUTORIAL_PROGRAMS ${TUTORIAL_PROGRAMS} triangle_mesher triangle_mesher_sizing_function triangle_mesher_with_seedpoints)
endif(VIENNAMESH_WITH_TRIANGLE)

if (VIENNAMESH_WITH_TETGEN)
  set(TUTORIAL_PROGRAMS ${TUTORIAL_PROGRAMS} tetgen_mesher tetgen_mesher_with_seedpoints tetgen_seedpoints_3segments extract_hull_holes reference_parameter)
endif(VIENNAMESH_WITH_TETGEN)

if (VIENNAMESH_WITH_NETGEN)
  set(TUTORIAL_PROGRAMS ${TUTORIAL_PROGRAMS} netgen_csg_mesher)
endif(VIENNAMESH_WITH_NETGEN)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBVIENNAMESH_COMPILE_FLAGS}")
message(STATUS "Tutorial compile flags: ${CMAKE_CXX_FLAGS}")

foreach(PROG ${TUTORIAL_PROGRAMS})
  add_executable(${PROG} ${PROG}.cpp)
  target_link_libraries(${PROG} viennamesh)
endforeach(PROG)









