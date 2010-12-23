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

  bool NgPar_IsExchangeFace ( const int & fnr ) ;
  bool NgPar_IsExchangeVert ( const int & vnum );
  bool NgPar_IsExchangeEdge ( const int & ednum );

  void NgPar_PrintParallelMeshTopology ();




#ifdef __cplusplus
}
#endif

#endif

#endif
