if (DEFINED VIENNAMESH_WITH_TDR AND VIENNAMESH_WITH_TDR)
  add_definitions( -DVIENNAMESH_WITH_TDR )
endif()

FILE(GLOB_RECURSE LIBVIENNAMESH_SRC *.cpp)

ADD_LIBRARY(viennamesh ${LIBVIENNAMESH_SRC})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBVIENNAMESH_COMPILE_FLAGS}")

message(STATUS "libviennamesh compile flags: ${CMAKE_CXX_FLAGS}")

target_link_libraries(viennamesh ${VIENNAMESH_EXTERNAL_LIBRARIES})

if (NOT WIN32)
  find_package (Threads REQUIRED)
  target_link_libraries(viennamesh ${CMAKE_THREAD_LIBS_INIT})
endif()

IF (DEFINED VIENNAMESH_WITH_TDR AND VIENNAMESH_WITH_TDR)
  find_package(HDF5 COMPONENTS CXX)
  target_link_libraries(viennamesh ${HDF5_LIBRARIES})
ENDIF()

install(TARGETS viennamesh DESTINATION lib)
