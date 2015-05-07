VIENNAMESH_ADD_PLUGIN(viennamesh-module-symmetry plugin.cpp detection_2d.cpp extract_slice_2d.cpp recombine_slice_2d.cpp detection_3d.cpp extract_slice_3d.cpp recombine_slice.cpp)


# set(CGAL_DONT_OVERRIDE_CMAKE_FLAGS TRUE)
#
# find_package(CGAL COMPONENTS Core)
# if (CGAL_FOUND)
# #   set(CGAL_CXX_FLAGS "${CGAL_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
#   INCLUDE(${CGAL_USE_FILE})
#
#   message(STATUS "Found CGAL")
#   message(STATUS "CGAL include directories: ${CGAL_INCLUDE_DIRS}")
#   message(STATUS "CGAL libraries: ${CGAL_LIBRARIES}")
#   message(STATUS "CGAL 3RD party libraries: ${CGAL_3RD_PARTY_LIBRARIES}")
#
#   include_directories(${CGAL_INCLUDE_DIRS})
#   VIENNAMESH_ADD_PLUGIN(viennamesh-module-symmetry plugin.cpp detection_2d.cpp extract_slice_2d.cpp recombine_slice_2d.cpp detection_3d.cpp)
#   target_link_libraries(viennamesh-module-symmetry ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})
# else(CGAL_FOUND)
#   message(STATUS "CGAL not found")
# endif(CGAL_FOUND)

