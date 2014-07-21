FILE(GLOB_RECURSE LIBVIENNAMESH_SRC *.cpp)

ADD_LIBRARY(viennamesh ${LIBVIENNAMESH_SRC})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VIENNAMESH_COMPILE_FLAGS}")

message(STATUS "libviennamesh compile flags: ${CMAKE_CXX_FLAGS}")

target_link_libraries(viennamesh ${VIENNAMESH_EXTERNAL_LIBRARIES})
target_link_libraries(viennamesh ${VIENNAMESH_LIBRARIES})

if (NOT WIN32)
  find_package (Threads REQUIRED)
  target_link_libraries(viennamesh ${CMAKE_THREAD_LIBS_INIT})
endif()

install(TARGETS viennamesh DESTINATION lib)
