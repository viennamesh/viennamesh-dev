
set(VIENNAMESH_PLUGIN_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})
# set(VIENNAMESH_PLUGIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/viennamesh_plugin.cpp)
set(VIENNAMESH_PLUGIN_SOURCES "")

FUNCTION(VIENNAMESH_ADD_PLUGIN PLUGIN_NAME)

  list(GET ARGV 0 PLUGIN_NAME)
  message(STATUS "ADDED PLUGIN: ${PLUGIN_NAME}")
  list(REMOVE_AT ARGV 0)
  set(PLUGIN_SOURCES ${ARGV} ${VIENNAMESH_PLUGIN_SOURCES})
  message(STATUS "  SOURCES: ${PLUGIN_SOURCES}")

  include_directories(${VIENNAMESH_PLUGIN_INCLUDES})

  add_library(${PLUGIN_NAME} MODULE ${PLUGIN_SOURCES})
  set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")

  add_custom_command(TARGET ${PLUGIN_NAME} POST_BUILD COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/../${PLUGIN_NAME}.so)
  add_custom_command(TARGET ${PLUGIN_NAME} POST_BUILD COMMAND ln -s ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_NAME}.so ${CMAKE_CURRENT_BINARY_DIR}/..)

  install(TARGETS ${PLUGIN_NAME}
   DESTINATION ${INSTALL_PLUGIN_DIR}
   COMPONENT lib)

ENDFUNCTION(VIENNAMESH_ADD_PLUGIN)


include_directories(${VIENNAMESH_PLUGIN_INCLUDES})


option(ENABLE_PLUGIN_IO "Enable IO plugin" ON)
if (ENABLE_PLUGIN_IO)
  add_subdirectory(io)
endif()

option(ENABLE_PLUGIN_TRIANGLE "Enable triangle plugin" ON)
if (ENABLE_PLUGIN_TRIANGLE)
  add_subdirectory(triangle)
endif()

option(ENABLE_PLUGIN_TETGEN "Enable tetgen plugin" ON)
if (ENABLE_PLUGIN_TETGEN)
  add_subdirectory(tetgen)
endif()

option(ENABLE_PLUGIN_VIENNAGRID "Enable viennagrid plugin" ON)
if (ENABLE_PLUGIN_VIENNAGRID)
  add_subdirectory(viennagrid)
endif()

option(ENABLE_PLUGIN_SYMMETRY "Enable symmetry plugin" ON)
if (ENABLE_PLUGIN_SYMMETRY)
  add_subdirectory(symmetry)
endif()

option(ENABLE_PLUGIN_MESH_HEALING "Enable mesh healing plugin" ON)
if (ENABLE_PLUGIN_MESH_HEALING)
  add_subdirectory(mesh_healing)
endif()

option(ENABLE_PLUGIN_METIS "Enable metis plugin" ON)
if (ENABLE_PLUGIN_METIS)
  add_subdirectory(metis)
endif()

option(ENABLE_PLUGIN_NETGEN "Enable netgen plugin" ON)
if (ENABLE_PLUGIN_NETGEN)
  add_subdirectory(netgen)
endif()

option(ENABLE_PLUGIN_VGMODELER "Enable vgmodeler plugin" OFF)
if (ENABLE_PLUGIN_VGMODELER)
  add_subdirectory(vgmodeler)
endif()

option(ENABLE_PLUGIN_STATISTICS "Enable statistics plugin" ON)
if (ENABLE_PLUGIN_STATISTICS)
  add_subdirectory(statistics)
endif()

option(ENABLE_PLUGIN_CGAL "Enable cgal plugin" ON)
if (ENABLE_PLUGIN_CGAL)
  add_subdirectory(CGAL)
endif()

option(ENABLE_PLUGIN_POISSON "Enable poisson plugin" OFF)
if (ENABLE_PLUGIN_POISSON)
  add_subdirectory(poisson)
endif()

option(ENABLE_PLUGIN_VTK "Enable VTK mesh coarsening plugin" ON)
if (ENABLE_PLUGIN_VTK)
  add_subdirectory(VTK)
endif()

option(ENABLE_PLUGIN_PRAGMATIC "Enable pragmatic plugin" OFF)
if (ENABLE_PLUGIN_PRAGMATIC)
  add_subdirectory(pragmatic)
endif()

option(ENABLE_PLUGIN_ZOLTAN "Enable Zoltan plugin" OFF)
if (ENABLE_PLUGIN_ZOLTAN)
  find_package(Zoltan)
  if (Zoltan_FOUND)
    add_subdirectory(zoltan)
  else()
    message(FATAL_ERROR "No Zoltan Version found")
  endif()
endif()

option(ENABLE_PLUGIN_COLOR_REFINEMENT "Enable Color-Based Refinement" ON)
if(ENABLE_PLUGIN_COLOR_REFINEMENT)
  add_subdirectory(color_based_refinement)
endif()
