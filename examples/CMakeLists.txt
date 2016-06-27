FILE(GLOB EXAMPLE_DATA RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ data/*)
foreach(f ${EXAMPLE_DATA})
  configure_file(${f} "${CMAKE_CURRENT_BINARY_DIR}/${f}" COPYONLY)
endforeach()

add_subdirectory(tutorials)
