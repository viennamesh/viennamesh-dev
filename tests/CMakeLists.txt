if (VIENNAMESH_WITH_TETGEN)
  set(TEST_PROGRAMS ${TEST_PROGRAMS} tetgen_simple)
endif (VIENNAMESH_WITH_TETGEN)

if (VIENNAMESH_WITH_NETGEN)
  set(TEST_PROGRAMS ${TEST_PROGRAMS} netgen_simple)
endif (VIENNAMESH_WITH_NETGEN)

foreach(PROG ${TEST_PROGRAMS})
  add_executable(${PROG}-test src/${PROG}.cpp)
  SET_PROPERTY(TARGET ${PROG}-test APPEND_STRING PROPERTY COMPILE_FLAGS "${LIBVIENNAMESH_COMPILE_FLAGS}")
  target_link_libraries(${PROG}-test viennamesh)
  add_test(${PROG} ${PROG}-test)
endforeach(PROG)
