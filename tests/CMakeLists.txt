


INCLUDE(CTest)
ENABLE_TESTING()


#list all source files here
ADD_EXECUTABLE(tri triangle.cpp)

#link specific libraries to specific executables
TARGET_LINK_LIBRARIES(tri triangle)