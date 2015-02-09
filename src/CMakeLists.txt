FILE(GLOB_RECURSE VIENNAMESH_BACKEND_SOURCES *.cpp)
add_library(viennamesh SHARED ${VIENNAMESH_BACKEND_SOURCES})

find_package (Threads)
if (Threads_FOUND)
  message(STATUS "Found Threads")
  message(STATUS "Threads libraries: ${CMAKE_THREAD_LIBS_INIT}")

  set(VIENNAMESH_LIBRARIES ${VIENNAMESH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

  set(VIENNAMESH_COMPILE_FLAGS "${VIENNAMESH_COMPILE_FLAGS} -DVIENNAMESH_HAS_THREADS")
  set(VIENNAMESH_HAS_THREADS TRUE)
else()
  message(STATUS "Threads not found")
endif()

target_link_libraries(viennamesh ${VIENNAMESH_LIBRARIES})
install(TARGETS viennamesh DESTINATION lib)

