set (TOOL_PROGRAMS vmesh)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VIENNAMESH_COMPILE_FLAGS}")
message(STATUS "Tools compile flags: ${CMAKE_CXX_FLAGS}")

foreach(PROG ${TOOL_PROGRAMS})
  add_executable(${PROG} src/${PROG}.cpp)
  target_link_libraries(${PROG} viennamesh)
  install(TARGETS ${PROG} DESTINATION bin)
endforeach(PROG)

