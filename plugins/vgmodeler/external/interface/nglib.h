#ifndef VGMNGLIB
#define VGMNGLIB

/**************************************************************************/
/* File:   nglib.hh                                                       */
/* Author: Joachim Schoeberl                                              */
/* Date:   7. May. 2000                                                   */
/**************************************************************************/


// gsse01 include
//#include "gsse/domain.hpp"

/*

  Interface to the netgen meshing kernel

*/

#include "viennagridpp/mesh/mesh.hpp"

namespace vgmnetgen {

  /// Data type for NETGEN mesh
  typedef void * Ng_Mesh;

  /// Data type for NETGEN CSG geomty
  typedef void * Ng_CSG_Geometry;

  /// Data type for NETGEN 2D geomty
  typedef void * Ng_Geometry_2D;

  /// Data type for NETGEN STL geomty
  typedef void * Ng_STL_Geometry;



  // max number of nodes per element
  // #ifndef NG_VOLUME_ELEMENT_MAXPOINTS
  //     #define NG_VOLUME_ELEMENT_MAXPOINTS 10
  // #endif

  // implemented element types:
  enum Ng_Volume_Element_Type { NG_TET = 1, NG_PYRAMID = 2, NG_PRISM = 3,
          NG_TET10 = 4 };

  // max number of nodes per surface element
  // #ifndef NG_SURFACE_ELEMENT_MAXPOINTS
  //     #define NG_SURFACE_ELEMENT_MAXPOINTS 6
  // #endif

  // implemented element types:
  enum Ng_Surface_Element_Type { NG_TRIG = 1, NG_QUAD = 2,
              NG_TRIG6 = 3 };



  class Ng_Meshing_Parameters
  {
  public:
    /// maximal mesh size
    double maxh;
    /// minimal mesh size
    double minh;
    /// grading for local h
    double grading;
    /// file for meshsize
    char * meshsize_filename;
    /**
      2d optimization strategy:
      // s .. swap, opt 6 lines/node
      // S .. swap, optimal elements
      // m .. move nodes
      // p .. plot, no pause
      // P .. plot, pause
      // c .. combine
    **/
    char * optimize2d;
    /// number of 2d optimization steps
    int optsteps2d;
    /// use local h ?
    int uselocalh;
    /// use delaunay meshing
    int delaunay;
    /// check overlapping surfaces (debug)
    int checkoverlap;
    /// safty factor for curvatures (elemetns per radius)
    double curvaturesafety;  // [JW] sadly, this parameter has no effect
    /// use parallel threads
    int parthread;
    /// limit for max element angle (150-180)
    double badellimit;
    /// quad-dominated surface meshing
    int quad_dominated;

    // [JW] note:
    // the following parameters will be transferred to the STL parameters

    /// enable surface curvature handling
    int resthsurfcurvenable;
    /// factor for surface curvature partitioning
    double resthsurfcurvfac;


    Ng_Meshing_Parameters();
  };


  enum Ng_Result { NG_OK = 0,
      NG_SURFACE_INPUT_ERROR = 1,
      NG_VOLUME_FAILURE = 2,
      NG_STL_INPUT_ERROR = 3,
      NG_SURFACE_FAILURE = 4,
      NG_FILE_NOT_FOUND = 5 };





  // #ifdef __cplusplus
  // extern "C"
  // {
  // #endif

    // initialize, deconstruct Netgen library:
    void Ng_Init ();
    void Ng_Exit ();


    // Generates new mesh structure
    Ng_Mesh * Ng_NewMesh ();
    void Ng_DeleteMesh (Ng_Mesh * mesh);

    // feeds points, surface elements and volume elements to the mesh
    void Ng_AddPoint (Ng_Mesh * mesh, double * x);
    void Ng_AddSurfaceElement (Ng_Mesh * mesh, Ng_Surface_Element_Type et,
            int * pi);
    void Ng_AddVolumeElement (Ng_Mesh * mesh, Ng_Volume_Element_Type et,
            int * pi);

    // ask for number of points, surface and volume elements
    int Ng_GetNP (Ng_Mesh * mesh);
    int Ng_GetNSE (Ng_Mesh * mesh);
    int Ng_GetNE (Ng_Mesh * mesh);


    //  return point coordinates
    void Ng_GetPoint (Ng_Mesh * mesh, int num, double * x);

    // return surface and volume element in pi
    Ng_Surface_Element_Type
    Ng_GetSurfaceElement (Ng_Mesh * mesh, int num, int * pi);

    Ng_Volume_Element_Type
    Ng_GetVolumeElement (Ng_Mesh * mesh, int num, int * pi);


    // Defines MeshSize Functions
    void Ng_RestrictMeshSizeGlobal (Ng_Mesh * mesh, double h);
    void Ng_RestrictMeshSizePoint (Ng_Mesh * mesh, double * p, double h);
    void Ng_RestrictMeshSizeBox (Ng_Mesh * mesh, double * pmin, double * pmax, double h);

    // generates volume mesh from surface mesh
    Ng_Result Ng_GenerateVolumeMesh (Ng_Mesh * mesh, Ng_Meshing_Parameters * mp);

    void Ng_SaveMesh(Ng_Mesh * mesh, const char* filename);
    Ng_Mesh * Ng_LoadMesh(const char* filename);





    // **********************************************************
    // **   2D Meshing                                         **
    // **********************************************************


    // feeds points and boundary to mesh

    void Ng_AddPoint_2D (Ng_Mesh * mesh, double * x);
    void Ng_AddBoundarySeg_2D (Ng_Mesh * mesh, int pi1, int pi2);

    // ask for number of points, elements and boundary segments
    int Ng_GetNP_2D (Ng_Mesh * mesh);
    int Ng_GetNE_2D (Ng_Mesh * mesh);
    int Ng_GetNSeg_2D (Ng_Mesh * mesh);

    //  return point coordinates
    void Ng_GetPoint_2D (Ng_Mesh * mesh, int num, double * x);

    // return 2d triangles
    void Ng_GetElement_2D (Ng_Mesh * mesh, int num, int * pi, int * matnum = NULL);

    // return 2d boundary segment
    void Ng_GetSegment_2D (Ng_Mesh * mesh, int num, int * pi, int * matnum = NULL);


    // load 2d netgen spline geometry
    Ng_Geometry_2D * Ng_LoadGeometry_2D (const char * filename);

    // generate 2d mesh, mesh is allocated by function
    Ng_Result Ng_GenerateMesh_2D (Ng_Geometry_2D * geom,
          Ng_Mesh ** mesh,
          Ng_Meshing_Parameters * mp);

    void Ng_HP_Refinement (Ng_Geometry_2D * geom,
        Ng_Mesh * mesh,
        int levels);





    // **********************************************************
    // **   STL Meshing                                        **
    // **********************************************************


    // loads geometry from STL file
    Ng_STL_Geometry * Ng_STL_LoadGeometry (const char * filename, int binary = 0);


    // generate new STL Geometry
    Ng_STL_Geometry * Ng_STL_NewGeometry ();


    // fills STL Geometry
    // positive orientation
    // normal vector may be null-pointer
    void Ng_STL_AddTriangle (Ng_STL_Geometry * geom,
          double * p1, double * p2, double * p3,
          double * nv = NULL);

    // add (optional) edges :
    void Ng_STL_AddEdge (Ng_STL_Geometry * geom,
            double * p1, double * p2);

    // after adding triangles (and edges) initialize
  //  Ng_Result Ng_STL_InitSTLGeometry (Ng_STL_Geometry * geom);

  Ng_Result Ng_STL_InitSTLGeometry (Ng_STL_Geometry * geom, viennagrid::mesh_t const & vgrid_mesh);
  Ng_Result Ng_STL_InitSTLGeometry (Ng_STL_Geometry * geom, const char * filename);


    // automatically generates edges:
    Ng_Result Ng_STL_MakeEdges (Ng_STL_Geometry * geom,
              Ng_Mesh* mesh,
              Ng_Meshing_Parameters * mp);


    // generates mesh, empty mesh must be already created.
    Ng_Result Ng_STL_GenerateSurfaceMesh (Ng_STL_Geometry * geom,
            Ng_Mesh * mesh,
            Ng_Meshing_Parameters * mp);

  Ng_Result Ng_FS_SurfaceMesh(Ng_STL_Geometry * geom,
                              Ng_Mesh* mesh,
                              Ng_Meshing_Parameters * mp,
                              viennagrid::mesh_t const & vgrid_mesh);

  // #ifdef __cplusplus
  //  }
  // #endif
}

#endif
