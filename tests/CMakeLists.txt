foreach(PROG tetgen_cube)
  add_executable(${PROG}-test src/${PROG}.cpp)
  target_link_libraries(${PROG}-test viennamesh)
  add_test(${PROG} ${PROG}-test)
endforeach(PROG)

