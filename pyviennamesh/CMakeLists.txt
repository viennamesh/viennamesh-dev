FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(PythonLibs 2)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

get_filename_component(PYTHON_LIBRARY_DIR ${PYTHON_LIBRARIES} PATH)
LINK_DIRECTORIES(${PYTHON_LIBRARY_DIR})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_SWIG_FLAGS "")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBVIENNAMESH_COMPILE_FLAGS}")
message(STATUS "pyviennamesh compile flags: ${CMAKE_CXX_FLAGS}")


SET_SOURCE_FILES_PROPERTIES(pyviennamesh.i PROPERTIES CPLUSPLUS ON)
SWIG_ADD_MODULE(pyviennamesh python pyviennamesh.i)

SWIG_LINK_LIBRARIES(pyviennamesh viennamesh)

# from http://stackoverflow.com/questions/14784134/make-install-with-cmake-swig-python
execute_process(COMMAND python -c "from distutils.sysconfig import get_python_lib; print get_python_lib()" OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
install(TARGETS _pyviennamesh DESTINATION ${PYTHON_SITE_PACKAGES})
install(FILES ${CMAKE_BINARY_DIR}/pyviennamesh/pyviennamesh.py DESTINATION ${PYTHON_SITE_PACKAGES})
