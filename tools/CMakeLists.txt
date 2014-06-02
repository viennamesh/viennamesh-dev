set (TOOL_PROGRAMS convert_mesh extract_boundary transform_mesh project_mesh coarsen_linemesh hyperplane_clip merge_meshes map_segments vmesh)


if (VIENNAMESH_WITH_TRIANGLE)
  set(TOOL_PROGRAMS ${TOOL_PROGRAMS} triangle_make_mesh)
endif(VIENNAMESH_WITH_TRIANGLE)

if (VIENNAMESH_WITH_TETGEN)
  set(TOOL_PROGRAMS ${TOOL_PROGRAMS} tetgen_make_mesh)
endif(VIENNAMESH_WITH_TETGEN)


foreach(PROG ${TOOL_PROGRAMS})
  add_executable(${PROG} src/${PROG}.cpp)
  SET_PROPERTY(TARGET ${PROG} APPEND_STRING PROPERTY COMPILE_FLAGS "${LIBVIENNAMESH_COMPILE_FLAGS}")
  target_link_libraries(${PROG} viennamesh)
  install(TARGETS ${PROG} DESTINATION bin)
endforeach(PROG)

