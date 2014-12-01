set(IO_SOURCES
      io.cpp
      common.cpp
      mesh_reader.cpp
      mesh_writer.cpp
)

add_library(io MODULE ${IO_SOURCES})
set_target_properties(io PROPERTIES PREFIX "")
