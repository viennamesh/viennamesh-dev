if (VIENNAMESH_WITH_TETGEN)
  set(TEST_PROGRAMS ${TEST_PROGRAMS} tetgen_simple)
endif (VIENNAMESH_WITH_TETGEN)

if (VIENNAMESH_WITH_NETGEN)
  set(TEST_PROGRAMS ${TEST_PROGRAMS} netgen_simple)
endif (VIENNAMESH_WITH_NETGEN)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBVIENNAMESH_COMPILE_FLAGS}")
message(STATUS "Tests compile flags: ${CMAKE_CXX_FLAGS}")

foreach(PROG ${TEST_PROGRAMS})
  add_executable(${PROG}-test src/${PROG}.cpp)
  target_link_libraries(${PROG}-test viennamesh)
  add_test(${PROG} ${PROG}-test)
endforeach(PROG)
