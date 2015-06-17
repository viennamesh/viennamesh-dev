SET(VGMODELER_GSSE2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../gsse/gsse02/ ${CMAKE_CURRENT_SOURCE_DIR}/../gsse/gsse02/external/)

include_directories(${VGMODELER_GSSE2_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VGMODELER_COMPILE_FLAGS}")

ADD_LIBRARY(viennamesh-vgmodeler-meshstl STATIC ../stlgeom/meshstlsurface.cpp)
