set(EXAMPLES_TESTDATA
   data/big_and_small_cube.poly
   data/cube.poly
   data/example.poly
   data/cube_triangular_hull.vtu
   data/cube_with_cube_hole.poly
   data/cube_with_tunnel.poly
   data/cube_using_lines.poly
   data/two_cubes.poly
   data/test.bnd
   data/simple.bnd

   data/half-trigate.geo
   data/half-trigate.mesh
   data/half-trigate_0.vtu
   data/half-trigate_1.vtu
   data/half-trigate_2.vtu
   data/half-trigate_3.vtu
   data/half-trigate_4.vtu
   data/half-trigate_5.vtu
   data/half-trigate_6.vtu
   data/half-trigate_7.vtu
   data/half-trigate_8.vtu
   data/half-trigate_main.pvd
   )
foreach(f IN LISTS EXAMPLES_TESTDATA)
   configure_file(${f} "${CMAKE_CURRENT_BINARY_DIR}/${f}" COPYONLY)
endforeach()

add_subdirectory(tutorial)

