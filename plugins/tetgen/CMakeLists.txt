add_definitions( -DNO_TIMER -DTRILIBRARY -DANSI_DECLARATORS -DEXTERNAL_TEST )

add_library(tetgen MODULE plugin.cpp ${VIENNAMESH_PLUGIN_SOURCES} tetgen_mesh.cpp tetgen_make_mesh.cpp tetgen.cxx predicates.cxx)
set_target_properties(tetgen PROPERTIES PREFIX "")

VIENNAMESH_ADD_PLUGIN(tetgen)



# SET(TETGEN_SRC ${TETGEN_SOURCE_DIR}/tetgen.cxx ${TETGEN_SOURCE_DIR}/predicates.cxx)
#
# include_directories(${TETGEN_SOURCE_DIR}/)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBVIENNAMESH_COMPILE_FLAGS} -DTETLIBRARY")
# message(STATUS "Tetgen compile flags: ${CMAKE_CXX_FLAGS}")
#
# ADD_LIBRARY(viennamesh-tetgen ${TETGEN_SRC})
#
# install(TARGETS viennamesh-tetgen DESTINATION lib)
