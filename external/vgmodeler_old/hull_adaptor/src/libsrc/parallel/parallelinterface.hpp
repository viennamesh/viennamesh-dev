#ifndef FILE_PARALLELINTERFACE
#define FILE_PARALLELINTERFACE

#ifdef PARALLEL

#ifdef __cplusplus
extern "C" {
#endif
  
  int NgPar_Glob2Loc_SurfEl ( const int & locnum ) ;
  int NgPar_Glob2Loc_VolEl ( const int & locnum )  ;
  int NgPar_Glob2Loc_Segm ( const int & locnum )  ;
  int NgPar_Glob2Loc_Vert ( const int & locnum ) ;
  int NgPar_GetDistantPNum ( const int & proc, const int & locpnum ) ;
  int NgPar_GetDistantEdgeNum ( const int & proc, const int & locpnum ) ;
  int NgPar_GetDistantFaceNum ( const int & proc, const int & locpnum ) ;
  
  int NgPar_GetDistantElNum ( const int & proc, const int & locelnum );

  bool NgPar_IsExchangeFace ( const int & fnr ) ;
  bool NgPar_IsExchangeVert ( const int & vnum );
  bool NgPar_IsExchangeEdge ( const int & ednum );

  void NgPar_PrintParallelMeshTopology ();

  bool NgPar_IsElementInPartition ( const int elnum, const int dest );

  int NgPar_GetLoc2Glob_VolEl ( const int & locnum );

  bool NgPar_IsGhostFace ( const int facenum );
  bool NgPar_IsGhostEdge ( const int edgenum );

#ifdef __cplusplus
}
#endif

#endif

#endif
