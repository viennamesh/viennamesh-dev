find_package(Zoltan REQUIRED)

#find_path(ZOLTAN_INCLUDE_DIR zoltan.h   PATHS ${ZOLTAN_ROOT}   PATH_SUFFIXES "include"   NO_DEFAULT_PATH )

#find_library(Zoltan_LIBRARY 
             #Names "Zoltan" 
             #PATHS ${ZOLTAN_ROOT}     
             #PATH_SUFFIXES "lib/.libs" "lib"
             #NO_DEFAULT_PATH)

#if (Zoltan_FOUND)
  #message("Zoltan found")
  #message(STATUS "Zoltan Libraries: ${Zoltan_LIBRARIES}")
  #message(STATUS "Zoltan Library Directories: ${Zoltan_LIBRARY_DIRS}")
  #message(STATUS "Zoltan Include Directories: ${Zoltan_INCLUDE_DIRS}")
#elseif (NOT Zoltan_FOUND) 
 #message(FATAL_ERROR "No Zoltan Version found")
#endif()

VIENNAMESH_ADD_PLUGIN( viennamesh-module-zoltan  plugin.cpp
        zoltan_partitioning.cpp)

message(STATUS "Zoltan Libs found in plugin-directory: ${Zoltan_LIBRARIES}")
target_link_libraries(viennamesh-module-zoltan ${Zoltan_LIBRARIES})