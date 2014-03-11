SET(LIBVIENNAMESH_SRC
    ${LIBVIENNAMESH_SRC}
    ${VIENNAMESH_SOURCE_DIR}/viennamesh/algorithm/io/common.cpp
    ${VIENNAMESH_SOURCE_DIR}/viennamesh/algorithm/io/mesh_reader.cpp
    ${VIENNAMESH_SOURCE_DIR}/viennamesh/algorithm/io/mesh_writer.cpp
    ${VIENNAMESH_SOURCE_DIR}/viennamesh/utils/logger.cpp
    ${VIENNAMESH_SOURCE_DIR}/viennamesh/core/algorithm.cpp
    ${VIENNAMESH_SOURCE_DIR}/viennamesh/core/algorithm_factory.cpp
    ${VIENNAMESH_SOURCE_DIR}/viennamesh/core/algorithm_pipeline.cpp
    ${VIENNAMESH_SOURCE_DIR}/viennamesh/core/parameter.cpp)

IF(WITH_TETGEN)
  SET(LIBVIENNAMESH_SRC
      ${LIBVIENNAMESH_SRC}
      ${VIENNAMESH_SOURCE_DIR}/viennamesh/algorithm/tetgen/generator.cpp)
ENDIF(WITH_TETGEN)

IF(WITH_TRIANGLE)
  SET(LIBVIENNAMESH_SRC
      ${LIBVIENNAMESH_SRC}
      ${VIENNAMESH_SOURCE_DIR}/viennamesh/algorithm/triangle/generator.cpp)
ENDIF(WITH_TRIANGLE)

ADD_LIBRARY(viennamesh-core ${LIBVIENNAMESH_SRC})
