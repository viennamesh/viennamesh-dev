PROJECT(ViennaMesh)

cmake_minimum_required(VERSION 2.6)

#if you don't want the full compiler output, remove the following line
#SET(CMAKE_VERBOSE_MAKEFILE ON)

# set the include paths
INCLUDE_DIRECTORIES("netgen/")
INCLUDE_DIRECTORIES("netgen/nglib/")
INCLUDE_DIRECTORIES("netgen/libsrc/include")
#INCLUDE_DIRECTORIES("netgen/libsrc/interface")
#INCLUDE_DIRECTORIES("netgen/libsrc/geom2d")
#INCLUDE_DIRECTORIES("netgen/libsrc/gprim")
#INCLUDE_DIRECTORIES("netgen/libsrc/general")
#INCLUDE_DIRECTORIES("TclTkTogl/include")

# ------------------------------------------------------------------------------
# LIBSRC / INTERFACE
#
SET(LIBSRC_INT_TMP read_fnf_mesh.cpp readtetmesh.cpp readuser.cpp writeabaqus.cpp writediffpack.cpp
   writedolfin.cpp writeelmer.cpp writefeap.cpp writefluent.cpp writegmsh.cpp writejcm.cpp
   writepermas.cpp writetecplot.cpp writetet.cpp writetochnog.cpp writeuser.cpp
   wuchemnitz.cpp writegmsh2.cpp writeOpenFOAM15x.cpp )
FOREACH(SRC ${LIBSRC_INT_TMP})
   LIST(APPEND LIBSRC_INT ${CMAKE_CURRENT_SOURCE_DIR}/netgen/libsrc/interface/${SRC})
ENDFOREACH(SRC)
# ------------------------------------------------------------------------------
# LIBSRC / GEOM2D
#
SET(LIBSRC_GEOM_TMP genmesh2d.cpp geom2dmesh.cpp spline.cpp splinegeometry.cpp)
FOREACH(SRC ${LIBSRC_GEOM_TMP})
   LIST(APPEND LIBSRC_GEOM ${CMAKE_CURRENT_SOURCE_DIR}/netgen/libsrc/geom2d/${SRC})
ENDFOREACH(SRC)
# ------------------------------------------------------------------------------
# LIBSRC / CSG
#
SET(LIBSRC_CSG_TMP algprim.cpp brick.cpp   
   bspline2d.cpp csgeom.cpp csgparser.cpp curve2d.cpp edgeflw.cpp	      
   explicitcurve2d.cpp extrusion.cpp gencyl.cpp genmesh.cpp identify.cpp 
   manifold.cpp meshsurf.cpp polyhedra.cpp revolution.cpp singularref.cpp 
   solid.cpp specpoin.cpp spline3d.cpp surface.cpp triapprox.cpp)
FOREACH(SRC ${LIBSRC_CSG_TMP})
   LIST(APPEND LIBSRC_CSG ${CMAKE_CURRENT_SOURCE_DIR}/netgen/libsrc/csg/${SRC})
ENDFOREACH(SRC)
# ------------------------------------------------------------------------------
# LIBSRC / STL
#
SET(LIBSRC_STL_TMP meshstlsurface.cpp stlgeom.cpp stlgeomchart.cpp 
   stlgeommesh.cpp stlline.cpp stltool.cpp stltopology.cpp)
FOREACH(SRC ${LIBSRC_STL_TMP})
   LIST(APPEND LIBSRC_STL ${CMAKE_CURRENT_SOURCE_DIR}/netgen/libsrc/stlgeom/${SRC})
ENDFOREACH(SRC)
# ------------------------------------------------------------------------------
# LIBSRC / OCC
#
SET(LIBSRC_OCC_TMP Partition_Inter2d.cxx Partition_Inter3d.cxx 
   Partition_Loop.cxx Partition_Loop2d.cxx Partition_Loop3d.cxx Partition_Spliter.cxx 
   occconstruction.cpp occgenmesh.cpp occgeom.cpp occmeshsurf.cpp)
FOREACH(SRC ${LIBSRC_OCC_TMP})
   LIST(APPEND LIBSRC_OCC ${CMAKE_CURRENT_SOURCE_DIR}/netgen/libsrc/occ/${SRC})
ENDFOREACH(SRC)
# ------------------------------------------------------------------------------
# LIBSRC / MESHING
#
SET(LIBSRC_MESHING_TMP adfront2.cpp adfront3.cpp bisect.cpp boundarylayer.cpp    
	clusters.cpp curvedelems.cpp delaunay.cpp geomsearch.cpp global.cpp   
	 hprefinement.cpp improve2.cpp improve2gen.cpp improve3.cpp localh.cpp 
	meshclass.cpp meshfunc.cpp meshfunc2d.cpp meshing2.cpp meshing3.cpp    
	meshtool.cpp meshtype.cpp msghandler.cpp netrule2.cpp		       
	netrule3.cpp parser2.cpp parser3.cpp prism2rls.cpp		       
	pyramid2rls.cpp pyramidrls.cpp quadrls.cpp refine.cpp		      
	ruler2.cpp ruler3.cpp secondorder.cpp smoothing2.5.cpp		       
	smoothing2.cpp smoothing3.cpp specials.cpp tetrarls.cpp		       
	topology.cpp triarls.cpp validate.cpp zrefine.cpp bcfunctions.cpp     
	parallelmesh.cpp  paralleltop.cpp  paralleltop.hpp basegeom.cpp)
FOREACH(SRC ${LIBSRC_MESHING_TMP})
   LIST(APPEND LIBSRC_MESHING ${CMAKE_CURRENT_SOURCE_DIR}/netgen/libsrc/meshing/${SRC})
ENDFOREACH(SRC)
# ------------------------------------------------------------------------------
# LIBSRC / GPRIM
#
SET(LIBSRC_GPRIM_TMP adtree.cpp geom2d.cpp geom3d.cpp geomfuncs.cpp 
	geomtest3d.cpp transform3d.cpp)
FOREACH(SRC ${LIBSRC_GPRIM_TMP})
   LIST(APPEND LIBSRC_GPRIM ${CMAKE_CURRENT_SOURCE_DIR}/netgen/libsrc/gprim/${SRC})
ENDFOREACH(SRC)
# ------------------------------------------------------------------------------
# LIBSRC / LINALG
#
SET(LIBSRC_LINALG_TMP densemat.cpp polynomial.cpp bfgs.cpp linopt.cpp linsearch.cpp)
FOREACH(SRC ${LIBSRC_LINALG_TMP})
   LIST(APPEND LIBSRC_LINALG ${CMAKE_CURRENT_SOURCE_DIR}/netgen/libsrc/linalg/${SRC})
ENDFOREACH(SRC)
# ------------------------------------------------------------------------------
# LIBSRC / GENERAL
#
SET(LIBSRC_GENERAL_TMP array.cpp bitarray.cpp dynamicmem.cpp flags.cpp 
	hashtabl.cpp mystring.cpp ngexception.cpp optmem.cpp parthreads.cpp 
	profiler.cpp seti.cpp sort.cpp spbita2d.cpp symbolta.cpp table.cpp)
FOREACH(SRC ${LIBSRC_GENERAL_TMP})
   LIST(APPEND LIBSRC_GENERAL ${CMAKE_CURRENT_SOURCE_DIR}/netgen/libsrc/general/${SRC})
ENDFOREACH(SRC)
# # ------------------------------------------------------------------------------
# # LIBSRC / VISUALIZATION
# #
# SET(LIBSRC_VISUALIZATION_TMP meshdoc.cpp mvdraw.cpp stlmeshing.cpp vscsg.cpp 
# 	vsfieldlines.cpp vsmesh.cpp vsocc.cpp vssolution.cpp importsolution.cpp)
# FOREACH(SRC ${LIBSRC_VISUALIZATION_TMP})
#    LIST(APPEND LIBSRC_VISUALIZATION ${CMAKE_CURRENT_SOURCE_DIR}/netgen/libsrc/visualization/${SRC})
# ENDFOREACH(SRC)
# ------------------------------------------------------------------------------
# NGLIB
#
SET(NGLIB ${CMAKE_CURRENT_SOURCE_DIR}/netgen/nglib/nglib.cpp)
# ------------------------------------------------------------------------------
# BUILD NGLIB
#    note: it consists of sources from libsrc/ and from nglib/ as well as TCL includes 
#
ADD_LIBRARY(netgen SHARED ${LIBSRC_INT} ${LIBSRC_GEOM} ${LIBSRC_CSG} ${LIBSRC_STL} 
   ${LIBSRC_OCC} ${LIBSRC_MESHING} ${LIBSRC_GPRIM} ${LIBSRC_LINALG} ${LIBSRC_GENERAL} 
    ${NGLIB}) #${LIBSRC_VISUALIZATION})
# ------------------------------------------------------------------------------

















