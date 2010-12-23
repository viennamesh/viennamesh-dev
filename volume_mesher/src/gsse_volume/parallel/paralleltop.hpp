#ifndef FILE_PARALLELTOP
#define FILE_PARALLELTOP


class ParallelMeshTopology
{
  const Mesh & mesh;

  int ne, nv, ned, nfa;
  int nseg, nsurfel;

  int nexp;

  ARRAY< int > loc2glob_vert;
  ARRAY< int > loc2glob_volel;
  ARRAY< int > loc2glob_surfel;
  ARRAY< int > loc2glob_segm;


  // exchange_vert [proc][locpnum] = distnum
  // locnum ... lokale vertexnummer
  // distnum ... lokale vertexnummer am processor proc
  TABLE < int, 0 > * exchange_vert;
  TABLE <int, 0 > * exchange_edge;
  TABLE <int, 0 > * exchange_face;

  BitArray isexchangeface, isexchangevert, isexchangeedge;

  BitArray hasglobalsurf;


public:


  ParallelMeshTopology (const Mesh & amesh);
  ~ParallelMeshTopology ();

  void Reset ();
  void SetLocNV( const int locnv );

  void SetLoc2Glob_Vert ( const int & locnum, const int & globnum );
  void SetLoc2Glob_VolEl ( const int & locnum, const int & globnum );
  void SetLoc2Glob_SurfEl ( const int & locnum, const int & globnum )
  {
    loc2glob_surfel[locnum] = globnum;
  }
  void SetLoc2Glob_Segm ( const int & locnum, const int & globnum );

  void Loc2Glob_SurfEl_SetSize ( const int & n )
  {
    loc2glob_surfel.SetSize(n);
  }
  const int GetLoc2Glob_Vert ( const int & locnum ) const
  {
    if ( locnum <= loc2glob_vert.Size() )
    return loc2glob_vert [ locnum-1 ];
    else
      cout << "locnum = " << locnum << ", size only " << loc2glob_vert.Size() << endl;
    return -1;
  }


  void  AppendLoc2Glob_Vert ( const int & globnum );
  void  AppendLoc2Glob_VolEl ( const int & globnum );
  void AppendLoc2Glob_SurfEl ( const int & globnum );
  void AppendLoc2Glob_Segm ( const int & globnum );

  const int Glob2Loc_SurfEl (const int & locnum );
  const int Glob2Loc_VolEl (const int & locnum );
  const int Glob2Loc_Segm (const int & locnum );
  const int Glob2Loc_Vert (const int & locnum );

  void SetExchange_VertSize(const int & ntasks, const int & nexp )
  {
    if ( exchange_vert) delete exchange_vert;
    ARRAY<int> cnt ( ntasks );
    for ( int i = 0; i < ntasks; i++)
      cnt[i] = nexp;
    exchange_vert = new TABLE<int> (cnt);
 
     for (int i = 0; i < ntasks; i++ )
       for ( int j = 0; j < cnt[i]; j++)
	 exchange_vert->Set ( i+1, j+1, -1 );
  }


  void SetExchange_EdgeSize(const int & ntasks, const int & ned )
  {
    if ( exchange_edge ) delete exchange_edge;
    ARRAY<int> cnt ( ntasks );
    for ( int i = 0; i < ntasks; i++)
      cnt[i] = ned;
    exchange_edge = new TABLE<int> (cnt);
     for (int i = 0; i < ntasks; i++ )
       for ( int j = 0; j < cnt[i]; j++)
	 exchange_edge->Set(i+1, j+1, -1);
  }


  void SetExchange_FaceSize(const int & ntasks, const int & nfa )
  {
    if ( exchange_face ) delete exchange_face;
    ARRAY<int> cnt ( ntasks );
    for ( int i = 0; i < ntasks; i++)
      cnt[i] = nfa;
    exchange_face = new TABLE<int> (cnt);
     for (int i = 0; i < ntasks; i++ )
       for ( int j = 0; j < cnt[i]; j++)
	 exchange_face->Set(i+1, j+1, -1);
  }

  void SetExchange_VertSize ( const ARRAY<int> & nexp )
  {
    if ( exchange_vert) delete exchange_vert;
    exchange_vert = new TABLE<int> (nexp);
    for (int i = 1; i <= nexp.Size(); i++ )
      for ( int j = 1; j <= nexp[i]; j++)
	exchange_vert->Set(i, j, -1);;

  }

  void SetExchange_EdgeSize ( const ARRAY<int> & nexp )
  {
    if ( exchange_edge ) delete exchange_edge;
    exchange_edge = new TABLE<int> (nexp);
    for (int i = 1; i <= nexp.Size(); i++ )
      for ( int j = 1; j <= nexp[i]; j++)
	exchange_edge->Set(i, j, -1 );

  }

  void SetExchange_FaceSize ( const ARRAY<int> & nexp )
  {
    if ( exchange_face ) delete exchange_face;
    exchange_face = new TABLE<int> (nexp);
    for (int i = 1; i <= nexp.Size(); i++ )
      for ( int j = 1; j <= nexp[i]; j++)
	exchange_face->Set(i, j, -1 );

  }

  void GetNeighbouringProc_Vert ( const int & vnum, ARRAY<int> & procs ) const;

  const int  GetDistantPNum ( const int & proc, const int & locpnum ) const;
  const int  GetDistantEdgeNum ( const int & proc, const int & locedgenum ) const;
  const int  GetDistantFaceNum ( const int & proc, const int & locedgenum ) const;


  void AppendExchange_Vert ( const int & locpnum, const ARRAY<int> & intarray );
  void SetExchange_Vert ( const int & ii, const  int & locpnum, const ARRAY<int> & intarray );
  void AppendExchange_Vert ( const int & locpnum, const int & proc, const int & distpnum);
  void AppendExchange_Edge ( const int & locedgenum, const int & proc, const int & distedgenum);
  void AppendExchange_Face ( const int & locfacenum, const int & proc, const int & distfacenum);



  void Print() const;

  void SetExchangeFace ( const int fnr );
  void SetExchangeVert (const int vnum );
  void SetExchangePoint (const int vnum );
  void SetExchangeEdge (const int ednum );



  bool IsExchangeFace ( const int fnr ) const ;
  bool IsExchangeVert (const int vnum ) const ;
  bool IsExchangeEdge (const int ednum ) const;

  void Update();
  void UpdateExchangePoints();
  void UpdateExchangeFaces();
  void UpdateExchangeEdges();

  void PrintExchange_Vert(int i, int j);

  void SetZero();
};
 






#endif
