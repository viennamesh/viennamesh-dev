add_definitions( -DNO_TIMER -DTRILIBRARY -DANSI_DECLARATORS -DEXTERNAL_TEST )

add_library(triangle MODULE plugin.cpp ${VIENNAMESH_PLUGIN_SOURCES} triangle_mesh.cpp triangle_make_mesh.cpp triangle.c triangle_interface.c)
set_target_properties(triangle PROPERTIES PREFIX "")

VIENNAMESH_ADD_PLUGIN(triangle)



# SET(TRIANGLE_SRC ${TRIANGLE_SOURCE_DIR}/triangle.c ${TRIANGLE_SOURCE_DIR}/triangle_interface.c)
#
# include_directories(${TRIANGLE_SOURCE_DIR}/)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBVIENNAMESH_COMPILE_FLAGS} -DNO_TIMER -DTRILIBRARY -DANSI_DECLARATORS -DEXTERNAL_TEST")
# set(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS})
#
# message(STATUS "Triangle compile flags: ${CMAKE_CXX_FLAGS}")
#
# ADD_LIBRARY(viennamesh-triangle ${TRIANGLE_SRC})
#
# install(TARGETS viennamesh-triangle DESTINATION lib)
