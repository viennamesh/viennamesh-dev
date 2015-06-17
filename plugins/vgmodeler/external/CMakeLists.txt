
# ------------------------------------------------------------------------------
# LINALG
#
#AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/linalg/ LINALG)
SET(LINALG_TMP densemat.cpp vector.cpp polynomial.cpp)
FOREACH(SRC ${LINALG_TMP})
   LIST(APPEND LINALG ${CMAKE_CURRENT_SOURCE_DIR}/linalg/${SRC})
ENDFOREACH(SRC)

# ------------------------------------------------------------------------------
# OPTI
#
#AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/opti/ OPTI)
SET(OPTI_TMP bfgs.cpp linopt.cpp linsearch.cpp)
FOREACH(SRC ${OPTI_TMP})
   LIST(APPEND OPTI ${CMAKE_CURRENT_SOURCE_DIR}/opti/${SRC})
ENDFOREACH(SRC)

# ------------------------------------------------------------------------------
# MESHING
#
#AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/meshing/ MESHING)
SET(MESHING_TMP global.cpp bisect.cpp meshtool.cpp refine.cpp ruler3.cpp improve3.cpp
   smoothing3.cpp adfront3.cpp tetrarls.cpp prism2rls.cpp pyramidrls.cpp pyramid2rls.cpp
   netrule3.cpp ruler2.cpp meshclass.cpp improve2.cpp smoothing2.cpp smoothing2.5.cpp
   adfront2.cpp netrule2.cpp triarls.cpp geomsearch.cpp secondorder.cpp meshtype.cpp
   parser3.cpp meshing2.cpp quadrls.cpp specials.cpp parser2.cpp meshing3.cpp meshfunc.cpp
   localh.cpp improve2gen.cpp delaunay.cpp boundarylayer.cpp msghandler.cpp
   meshfunc2d.cpp topology.cpp clusters.cpp curvedelems_new.cpp hprefinement.cpp
   validate.cpp
)
FOREACH(SRC ${MESHING_TMP})
   LIST(APPEND MESHING ${CMAKE_CURRENT_SOURCE_DIR}/meshing/${SRC})
ENDFOREACH(SRC)

# ------------------------------------------------------------------------------
# GPRIM
#
#AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/gprim/ GPRIM)
SET(GPRIM_TMP geomtest3d.cpp geom2d.cpp geom3d.cpp adtree.cpp transform3d.cpp
   geomfuncs.cpp)
FOREACH(SRC ${GPRIM_TMP})
   LIST(APPEND GPRIM ${CMAKE_CURRENT_SOURCE_DIR}/gprim/${SRC})
ENDFOREACH(SRC)

# ------------------------------------------------------------------------------
# INTERFACE
#
#AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/interface/ INTERFACE)
SET(INTERFACE_TMP nglib.cpp)
FOREACH(SRC ${INTERFACE_TMP})
   LIST(APPEND INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/interface/${SRC})
ENDFOREACH(SRC)

# ------------------------------------------------------------------------------
# STLGEOM
#
#AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/stlgeom/ STLGEOM)
SET(STLGEOM_TMP stlline.cpp stltool.cpp stlgeom.cpp stlgeomchart.cpp stlgeommesh.cpp)
FOREACH(SRC ${STLGEOM_TMP})
   LIST(APPEND STLGEOM ${CMAKE_CURRENT_SOURCE_DIR}/stlgeom/${SRC})
ENDFOREACH(SRC)

# ------------------------------------------------------------------------------
# GEOM2D
#
SET(GEOM2D_TMP geom2dmesh.cpp spline.cpp splinegeometry.cpp genmesh2d.cpp)
FOREACH(SRC ${GEOM2D_TMP})
   LIST(APPEND GEOM2D ${CMAKE_CURRENT_SOURCE_DIR}/geom2d/${SRC})
ENDFOREACH(SRC)

# ------------------------------------------------------------------------------
# CSG
#
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/csg/ CSG)


# ------------------------------------------------------------------------------
# GENERAL
#
#AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/general/ GENERAL)
SET(GENERAL_TMP moveablemem.cpp ngexception.cpp table.cpp optmem.cpp spbita2d.cpp
   hashtabl.cpp flags.cpp seti.cpp bitarray.cpp array.cpp symbolta.cpp mystring.cpp
   profiler.cpp sort.cpp)
FOREACH(SRC ${GENERAL_TMP})
   LIST(APPEND GENERAL ${CMAKE_CURRENT_SOURCE_DIR}/general/${SRC})
ENDFOREACH(SRC)

# ------------------------------------------------------------------------------
# BUILD VGMODELERLIB
#


IF(MSVC)
	SET(VGMODELER_COMPILE_FLAGS "${LIBVIENNAMESH_COMPILE_FLAGS} -DNO_PARALLEL_THREADS -DUSE_NON_CONST")
else()
	SET(VGMODELER_COMPILE_FLAGS "${LIBVIENNAMESH_COMPILE_FLAGS} -DNO_PARALLEL_THREADS -DUSE_NON_CONST -DLINUX -w")
endif()

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/ )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

SET(VGMODELER_GSSE1_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/gsse/gsse01/ ${CMAKE_CURRENT_SOURCE_DIR}/gsse/gsse01/external/mauch)

add_subdirectory(cmake.stl)

# note: we have to use specific include paths for these sources, as they won't compile
# if gsse01 and gsse02 is used together in the global INCLUDE_DIRECTORIES path

include_directories(${VGMODELER_GSSE1_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VGMODELER_COMPILE_FLAGS}")

message(STATUS "VgModeler compile flags: ${CMAKE_CXX_FLAGS}")



ADD_LIBRARY(viennamesh-vgmodeler STATIC ${GENERAL} ${INTERFACE} ${LINALG} ${OPTI} ${MESHING} ${GPRIM} ${STLGEOM} ${GEOM2D} ${CSG} stlgeom/stltopology.cpp vgmodeler.cpp)
target_link_libraries(viennamesh-vgmodeler viennamesh-vgmodeler-meshstl)

