set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )
 
if ( COMMAND cmake_policy )

  cmake_policy( SET CMP0003 NEW )  

endif()
find_package( CGAL QUIET COMPONENTS  )
if ( NOT CGAL_FOUND )

  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()  

endif()
include( ${CGAL_USE_FILE} )
find_package( Boost REQUIRED )
if ( NOT Boost_FOUND )

  message(STATUS "This project requires the Boost library, and will not be compiled.")

  return()  

endif()
message(STATUS "TEST")
message(STATUS "TEST ${CGAL_USE_FILE}")

VIENNAMESH_ADD_PLUGIN(viennamesh-module-cgal plugin.cpp
                      cgal_mesh.cpp
                      cgal_simplify_mesh.cpp )

# add_to_cached_list( CGAL_EXECUTABLE_TARGETS viennamesh-module-cgal )
# add_to_cached_list( CGAL_EXECUTABLE_TARGETS cgal_mesh.cpp )
# add_to_cached_list( CGAL_EXECUTABLE_TARGETS cgal_simplify_mesh.cpp )

target_link_libraries(viennamesh-module-cgal ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES} )
#target_link_libraries(cgal_mesh.cpp   ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES} )
#target_link_libraries(cgal_simplify_mesh.cpp   ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES} )