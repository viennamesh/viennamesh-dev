
add_library(test MODULE test.cpp)
target_link_libraries(test viennamesh)
set_target_properties(test PROPERTIES PREFIX "")
