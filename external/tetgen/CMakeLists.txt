SET(TETGEN_SRC ${TETGEN_SOURCE_DIR}/tetgen.cxx ${TETGEN_SOURCE_DIR}/predicates.cxx)

include_directories(${TETGEN_SOURCE_DIR}/)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBVIENNAMESH_COMPILE_FLAGS} -DTETLIBRARY")
message(STATUS "Tetgen compile flags: ${CMAKE_CXX_FLAGS}")

ADD_LIBRARY(viennamesh-tetgen ${TETGEN_SRC})

install(TARGETS viennamesh-tetgen DESTINATION lib)
