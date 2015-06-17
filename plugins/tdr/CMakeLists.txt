find_package(HDF5 COMPONENTS CXX)
if (HDF5_FOUND)
  message(STATUS "Found HDF5")
  message(STATUS "HDF5 include directories: ${HDF5_INCLUDE_DIRS}")
  message(STATUS "HDF5 libraries: ${HDF5_LIBRARIES}")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-extended-offsetof")

  include_directories(${HDF5_INCLUDE_DIRS})
  VIENNAMESH_ADD_PLUGIN(viennamesh-module-tdr plugin.cpp
                        tdr_reader.cpp
                        sentaurus_tdr_reader.cpp
                        tdr_writer.cpp
                        sentaurus_tdr_writer.cpp)

  target_link_libraries(viennamesh-module-tdr ${HDF5_LIBRARIES})
else()
  message(STATUS "HDF5 not found")
endif()
