set(TEST_PROGRAMS tetgen_simple netgen_simple)

foreach(PROG ${TEST_PROGRAMS})
  add_executable(${PROG}-test src/${PROG}.cpp)
  target_link_libraries(${PROG}-test viennamesh)
  add_test(${PROG} ${PROG}-test)
endforeach(PROG)
