#ifndef FILE_PARALLELTOP
#define FILE_PARALLELTOP

extern int ntasks;

class ParallelMeshTopology
{
  const Mesh & mesh;

  int ne, nv, np, ned, nfa;
  int nseg, nsurfel;

  int neglob, nvglob;
  int nparel;
  int nfaglob;

  TABLE<int,PointIndex::BASE> loc2distvert;

  ARRAY<int> /* ** loc2distvert, */ ** loc2distedge, **loc2distface, **loc2distel;
  ARRAY<int> ** loc2distsegm, ** loc2distsurfel;

  BitArray * isexchangeface, * isexchangevert, * isexchangeedge, * isexchangeel;

  BitArray isghostedge, isghostface;

  bool coarseupdate;
  int overlap;

public:

  ParallelMeshTopology (const Mesh & amesh);
  ~ParallelMeshTopology ();

  /// set nv, reset sizes of loc2dist_vert, isexchangevert...
  void SetNV ( const int anv );

  void SetNE ( const int ane );

  void SetNSE ( const int anse );

  void SetNSegm ( const int anseg );

  void Reset ();

  inline void SetLoc2Glob_Vert ( const int & locnum, const int & globnum )
  {
    loc2distvert[locnum][0] = globnum;
    // (*loc2distvert[locnum-1])[0] = globnum;
  }
  inline void SetLoc2Glob_VolEl ( const int & locnum, const int & globnum )
  {
    (*loc2distel[locnum-1])[0] = globnum;
  }
  inline void SetLoc2Glob_SurfEl ( const int & locnum, const int & globnum )
  {
    (*loc2distsurfel[locnum-1])[0] = globnum;
  }
  inline void SetLoc2Glob_Segm ( const int & locnum, const int & globnum )
  {
    (*loc2distsegm[locnum-1])[0] = globnum;
  }

  const inline int GetLoc2Glob_Vert ( const int & locnum ) const
  {
    return loc2distvert[locnum][0];
    // return (*loc2distvert [ locnum-1 ])[0];
  }
  const inline int GetLoc2Glob_VolEl ( const int & locnum ) const
  {
    return (*loc2distel[ locnum-1 ])[0];
  }

  void GetVertNeighbours ( const int vnum, ARRAY<int> & dests ) const;

  const int Glob2Loc_SurfEl (const int & globnum );
  const int Glob2Loc_VolEl (const int & globnum );
  const int Glob2Loc_Segm (const int & globnum );
  const int Glob2Loc_Vert (const int & globnum );

  const int  GetDistantPNum ( const int & proc, const int & locpnum ) const;
  const int  GetDistantEdgeNum ( const int & proc, const int & locedgenum ) const;
  const int  GetDistantFaceNum ( const int & proc, const int & locedgenum ) const;
  const int  GetDistantElNum ( const int & proc, const int & locelnum ) const;

  void Print() const;

  inline void  SetExchangeFace ( const int fnr )
  {
    isexchangeface->Set( (fnr-1)*(ntasks+1) );
  }

  inline void  SetExchangeVert (const int vnum )
  {
    isexchangevert->Set( (vnum-1)*(ntasks+1) );
  }
  inline   void  SetExchangeElement (const int elnum )
  {
    isexchangeel->Set((elnum-1)*(ntasks+1) );
  }

  inline  void  SetExchangeEdge (const int ednum )
  {
    isexchangeedge->Set ((ednum-1)*(ntasks+1));
  }

  inline bool IsExchangeVert ( const int vnum ) const
  {
    return isexchangevert->Test((vnum-1)*(ntasks+1));
  }
  inline bool IsExchangeEdge ( const int ednum ) const
  {
    int i = (ednum-1)*(ntasks+1);
    return isexchangeedge->Test(i);
  }
  inline bool IsExchangeFace ( const int fnum ) const
  {
    return isexchangeface->Test( (fnum-1)*(ntasks+1) );
  }

  inline  bool IsExchangeElement ( const int elnum ) const
  {
    return isexchangeel->Test((elnum-1)*(ntasks+1));
  }

  inline bool IsExchangeSEl ( const int selnum ) const
  {
    return ( loc2distsurfel[selnum-1]->Size() > 1 );
  }

  inline void  SetExchangeFace (const int dest,  const int fnr )
  {
    isexchangeface->Set((fnr-1)*(ntasks+1) + (dest+1));
  }

  inline void  SetExchangeVert (const int dest,  const int vnum )
  {
    isexchangevert->Set((vnum-1)*(ntasks+1) + (dest+1) );
  }
  inline void  SetExchangeElement (const int dest,  const int vnum )
  {
    isexchangeel->Set( (vnum-1)*(ntasks+1) + (dest+1) );
  }

  inline   void  SetExchangeEdge (const int dest,  const int ednum )
  {
    isexchangeedge->Set ( (ednum-1)*(ntasks+1) + (dest+1) );
  }

  inline bool IsExchangeVert (const int dest,   const int vnum ) const
  {
    return isexchangevert->Test((vnum-1)*(ntasks+1) + (dest+1) );
  }
  inline bool IsExchangeEdge (const int dest,   const int ednum ) const
  {
    return isexchangeedge->Test((ednum-1)*(ntasks+1) + (dest+1));
  }
  inline bool IsExchangeFace (const int dest,   const int fnum ) const
  {
    return isexchangeface->Test((fnum-1)*(ntasks+1) + (dest+1) );
  }

  inline bool IsExchangeElement (const int dest,   const int elnum ) const
  {
    return isexchangeel->Test((elnum-1)*(ntasks+1) + (dest+1));
  }

  inline int Overlap() const
  { return overlap; }

  inline int IncreaseOverlap ()
  { overlap++; return overlap; }

  void Update();

  void UpdateCoarseGrid();
  void UpdateCoarseGridOverlap();
  void UpdateRefinement ();
  void UpdateTopology ();
  void UpdateExchangeElements();

  void UpdateCoarseGridGlobal();

  const inline bool DoCoarseUpdate() const 
  { return !coarseupdate; }


  void SetDistantFaceNum ( const int dest, const int & locnum, const int & distnum );
  void SetDistantPNum ( const int dest, const int & locnum, const int & distnum );
  void SetDistantEdgeNum ( const int dest, const int & locnum, const int & distnum );
  void SetDistantEl ( const int dest, const int & locnum, const int & distnum );
  void SetDistantSurfEl ( const int dest, const int & locnum, const int & distnum );
  void SetDistantSegm ( const int dest, const int & locnum, const int & distnum );

  inline bool IsGhostEl ( const int elnum ) const
  { return mesh.VolumeElement(elnum).IsGhost(); }

  inline bool IsGhostFace ( const int fanum ) const
  { return isghostface.Test(fanum-1); }

  inline bool IsGhostEdge ( const int ednum ) const
  { return isghostedge.Test(ednum-1); }

   inline bool IsGhostVert ( const int pnum ) const
  { return mesh.Point(pnum).IsGhost(); }

//    inline void SetGhostVert ( const int pnum )
//    { isghostvert.Set(pnum-1); }

  inline void SetGhostEdge ( const int ednum )
  { isghostedge.Set(ednum-1); }

  inline void ClearGhostEdge ( const int ednum )
  { isghostedge.Clear(ednum-1); }

  inline void SetGhostFace ( const int fanum )
  { isghostface.Set(fanum-1); }

  inline void ClearGhostFace ( const int fanum )
  { isghostface.Clear(fanum-1); }

  inline bool IsElementInPartition ( const int elnum, const int dest ) const
  { 
    return IsExchangeElement ( dest, elnum); 
  }
  
  inline void SetElementInPartition ( const int elnum, const int dest )
  { 
    SetExchangeElement ( dest, elnum );
  }

  inline void SetNVGlob ( const int anvglob )
  { nvglob = anvglob; }

  inline void SetNEGlob ( const int aneglob )
  { neglob = aneglob; }

  inline int GetNVGlob ()
  { return nvglob; }

  inline int GetNEGlob ()
  { return neglob; }
};
 






#endif
