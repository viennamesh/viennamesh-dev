#ifdef PARALLEL




#include "parallel.hpp"



namespace netgen
{

  void ParallelMeshTopology :: Reset ()
{
  // MPI DATA
  int id, ntasks;
  
  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  PrintMessage ( 4, "RESET");

  
  ne = mesh.GetNE();
  nv = mesh.GetNV();
  nseg = mesh.GetNSeg();
  nsurfel = mesh.GetNSE();

  ned = mesh.GetTopology().GetNEdges();
  nfa = mesh.GetTopology().GetNFaces();

  nexp = 0;

  // loc2glob wird nur beim ersten, coarse mesh gesetzt
  // sollte man nachher nimmer anruehren
  if ( !loc2glob_vert.Size() )
    loc2glob_vert.SetSize(nv);
  if ( !loc2glob_volel.Size() )
    loc2glob_volel.SetSize(ne);
  if ( !loc2glob_surfel.Size() )
    loc2glob_surfel.SetSize(nsurfel);
  if ( !loc2glob_segm.Size() )
    loc2glob_segm.SetSize(nseg);
  
  //  SetExchange_VertSize(ntasks, nexp);
  //    exchange_vert.SetSize(ntasks);
  //    exchange_edge.SetSize(ned);
  //    exchange_face.SetSize(nfa);
  
  //   for ( int i = 0; i < ned; i++)
  //     exchange_edge.SetSize(0);
  //   for ( int i = 0; i < ntasks; i++)
  //     exchange_vert.SetSize(nv);

//   if ( !isexchangeface.Size() )
  isexchangeface.SetSize(nfa);
//   if ( !isexchangevert.Size() )
  isexchangevert.SetSize(nv);
//   if ( !isexchangeedge.Size() )
  isexchangeedge.SetSize(ned);

  isexchangeface.Clear(); isexchangevert.Clear(); isexchangeedge.Clear();


}


 ParallelMeshTopology :: ~ParallelMeshTopology ()
{

  delete exchange_vert;
  delete exchange_edge;
  delete exchange_face;
  ;
}



 ParallelMeshTopology :: ParallelMeshTopology ( const netgen::Mesh & amesh )
  : mesh(amesh)
{
  // MPI DATA
  int id, ntasks;
  
  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  
//   ne = mesh.GetNE();
//   nv = mesh.GetNV();
//   nseg = mesh.GetNSeg();
//   nsurfel = mesh.GetNSE();

  ned = 0; //mesh.GetTopology().GetNEdges();
  nfa = 0; //mesh.GetTopology().GetNFaces();

  nexp = 0;

  loc2glob_vert.SetSize(0);
  loc2glob_volel.SetSize(0);
  loc2glob_surfel.SetSize(0);
  loc2glob_segm.SetSize(0);

  exchange_vert = 0;
  exchange_edge = 0;
  exchange_face = 0;

//     for ( int i = 0; i < 0; i++)
//     exchange_edge.SetSize(0);
//   for ( int i = 0; i < ntasks; i++)
//     exchange_vert.SetSize(0);

  isexchangeface.SetSize(0);
  isexchangevert.SetSize(0);
  isexchangeedge.SetSize(0);

  isexchangeface.Clear(); isexchangevert.Clear(); isexchangeedge.Clear();

}




void  ParallelMeshTopology :: SetLoc2Glob_Vert ( const int & locnum, const int & globnum) 
{
  loc2glob_vert [ locnum-1 ] = globnum;
}


void  ParallelMeshTopology :: SetLocNV ( const int locnv )
{
  loc2glob_vert.SetSize(locnv);
}

void ParallelMeshTopology ::  AppendLoc2Glob_Vert ( const int & globnum )
  {
    loc2glob_vert.Append(globnum);
  }

void ParallelMeshTopology ::  AppendLoc2Glob_VolEl ( const int & globnum )
  {
    loc2glob_volel.Append(globnum);
  }

void ParallelMeshTopology ::  AppendLoc2Glob_SurfEl ( const int & globnum )
  {
    // cout << "append, size = " << loc2glob_surfel.Size()  << ", globnum = " << globnum << endl;
    //    cout << "append, allocsize = " << loc2glob_surfel.AllocSize() << endl;
    loc2glob_surfel.Append(globnum);
    // cout << "append complete" << endl;
  }

void ParallelMeshTopology ::  AppendLoc2Glob_Segm ( const int & globnum )
  {
    loc2glob_segm.Append(globnum);
  }

const   int ParallelMeshTopology :: Glob2Loc_Vert (const int & globnum )
  {
    int locnum = -1;
    for (int i = 0; i < loc2glob_vert.Size(); i++)
      {
	if ( globnum == loc2glob_vert[i] )
	  {
	    locnum = i+1;
	  }
      }

    return locnum;
  }

 const int ParallelMeshTopology :: Glob2Loc_VolEl (const int & globnum )
  {
    int locnum = -1;
    for (int i = 0; i < loc2glob_volel.Size(); i++)
      {
	if ( globnum == loc2glob_volel[i] )
	  {
	    locnum = i+1;
	  }
      }
    return locnum;
  }

 const  int ParallelMeshTopology :: Glob2Loc_SurfEl (const int & globnum )
  {
    int locnum = -1;
    for (int i = 0; i < loc2glob_surfel.Size(); i++)
      {
	if ( globnum == loc2glob_surfel[i] )
	  {
	    locnum = i+1;
	  }
      }
    return locnum;
  }

  const int ParallelMeshTopology :: Glob2Loc_Segm (const int & globnum )
  {
    int locnum = -1;
    for (int i = 0; i < loc2glob_segm.Size(); i++)
      {
	if ( globnum == loc2glob_segm[i] )
	  {
	    locnum = i+1;
	  }
      }
    return locnum;
  }


//   void ParallelMeshTopology :: Print () const
//   {
//     std::cout << "loc2glob_vert : " << endl<<loc2glob_vert << endl << endl;

//   }

void ParallelMeshTopology ::  SetExchange_Vert ( const int & ii, const int & locpnum, const ARRAY<int> &  intarray )
  {
//     std::cout << intarray << endl;
//     exchange_vert[locpnum][ii]  =  intarray ;
//     std::cout << intarray << endl;
//     std::cout << "******************" << endl;

    ;
  }
  void ParallelMeshTopology ::  AppendExchange_Vert ( const int & locpnum, const ARRAY<int> & intarray )
  {
    ;

//     std::cout << intarray << endl;
//     cout << exchange_vert.Size() << locpnum << endl;
//     if ( exchange_vert.Size() > locpnum )
//     exchange_vert[locpnum].Append ( intarray );
//     std::cout << intarray << endl;
//     std::cout << "******************" << endl;
  }

void ParallelMeshTopology ::  AppendExchange_Vert ( const int & locpnum, 
						    const int & proc, const int & distpnum )
  {
    exchange_vert->Set( proc+1, locpnum,  distpnum);
  }


  void ParallelMeshTopology :: AppendExchange_Edge ( const int & locedgenum, 
						     const int & proc, const int & distedgenum)
  {
    exchange_edge->Set ( proc+1, locedgenum, distedgenum);
  }


  void ParallelMeshTopology :: AppendExchange_Face ( const int & locfacenum, 
						     const int & proc, const int & distfacenum)
  {
    exchange_face->Set ( proc+1, locfacenum, distfacenum);
  }


  void ParallelMeshTopology ::  Print() const
  {
      int id, rc, ntasks;

      MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
      MPI_Comm_rank(MPI_COMM_WORLD, &id);
      //      testout = new ofstream (string("testout_proc") + id  );      

      std::cout << endl <<  "TOPOLOGY FOR PARALLEL MESHES" << endl << endl;
      for ( int i = 1; i <= nv; i++ )
	if ( isexchangevert.Test(i-1) )
	  {

	    std::cout << "exchange point  " << i << ":  global " << GetLoc2Glob_Vert(i) << endl;
	    if ( exchange_vert->Size() > 0 )
	    for ( int dest = 0; dest < ntasks; dest ++)
	      if ( GetDistantPNum( dest, i ) > 0 )
		std::cout << "   p" << dest << ": " << GetDistantPNum ( dest, i ) << endl; 

 
	  }

      if ( exchange_edge )
      for ( int i = 1; i <= ned; i++ )
	if ( isexchangeedge.Test(i-1) )
	  {
	    int v1, v2;
	    mesh . GetTopology().GetEdgeVertices(i, v1, v2);
	    std::cout << "exchange edge  " << i << ":  global vertices "  << GetLoc2Glob_Vert(v1) << "  " 
		       << GetLoc2Glob_Vert(v2) << endl;
	    for ( int dest = 0; dest < ntasks; dest++)
	      if ( GetDistantEdgeNum ( dest, i ) > 0 )
		std::cout << "   p" << dest << ": " << GetDistantEdgeNum ( dest, i ) << endl;
	  }


      if ( exchange_face )
      for ( int i = 1; i <= nfa; i++ )
	if ( isexchangeface.Test(i-1) )
	  {
	    ARRAY<int> facevert;
	    mesh . GetTopology().GetFaceVertices(i, facevert);
	    
	    std::cout << "exchange face  " << i << ":  global vertices " ;
	    for ( int fi=0; fi < facevert.Size(); fi++)
	      std::cout << GetLoc2Glob_Vert(facevert[fi]) << "  ";
	    std::cout << endl; 
 	    for ( int dest = 0; dest < ntasks; dest++)
	      {
 	      if ( GetDistantFaceNum ( dest, i ) > 0 )
 		std::cout << "   p" << dest << ": " << GetDistantFaceNum ( dest, i ) << endl;
	      }
	  }

  }


  void ParallelMeshTopology ::  SetExchangeFace ( const int fnr )
  {
    if ( fnr <= nfa && fnr > 0 )
      isexchangeface.Set(fnr-1);
  }

  void ParallelMeshTopology ::  SetExchangeVert (const int vnum )
  {
    if ( vnum <= nv && vnum > 0 )
      isexchangevert.Set(vnum-1);
  }
  void ParallelMeshTopology ::  SetExchangePoint (const int vnum )
  {
    if ( vnum <= nv && vnum > 0 )
      isexchangevert.Set(vnum-1);
  }

  void ParallelMeshTopology ::  SetExchangeEdge (const int ednum )
  {
    if ( ednum <= ned && ednum > 0 )
      isexchangeedge.Set (ednum-1);
  }

bool ParallelMeshTopology :: IsExchangeVert ( const int vnum ) const
{
  return isexchangevert.Test(vnum-1);
}
bool ParallelMeshTopology :: IsExchangeEdge ( const int ednum ) const
{
  return isexchangeedge.Test(ednum-1);
}
bool ParallelMeshTopology :: IsExchangeFace ( const int fnum ) const
{
  return isexchangeface.Test(fnum-1);
}


  void ParallelMeshTopology :: GetNeighbouringProc_Vert ( const int & locpnum, ARRAY<int> & procs ) const
  {
    int np = exchange_vert[locpnum-1].Size();
        procs.SetSize(np);

    //    procs = (&exchange_vert)[locpnum-1];
     for (int i=0; i<np; i++)
       procs[i] = exchange_vert->Get(i+1,locpnum);


  }

  const int  ParallelMeshTopology :: GetDistantPNum ( const int & proc, const int & locpnum ) const
  {
    int id, rc, ntasks;
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    if ( proc == id ) 
      return -1;

    int distpnum = exchange_vert->Get(proc+1,locpnum);
    return distpnum;
  } 


  const int  ParallelMeshTopology :: GetDistantEdgeNum ( const int & proc, const int & locedgenum ) const
  {
    int id, rc, ntasks;
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    if ( proc == id ) 
      return -1;

    int distedgenum = exchange_edge->Get(proc+1,locedgenum);
    return distedgenum;
  } 

  const int  ParallelMeshTopology :: GetDistantFaceNum ( const int & proc, const int & locfacenum ) const
  {
    int id, rc, ntasks;
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    if ( proc == id ) 
      return -1;

    int distfacenum = exchange_face->Get(proc+1,locfacenum);
    return distfacenum;
  } 



  void ParallelMeshTopology :: Update ()
{
  ne = mesh.GetNE();
  nv = mesh.GetNV();
  nseg = mesh.GetNSeg();
  nsurfel = mesh.GetNSE();

  ned = mesh.GetTopology().GetNEdges();
  nfa = mesh.GetTopology().GetNFaces();


}


  void ParallelMeshTopology :: UpdateExchangePoints () 
  {
    std::cout << "update EXCHANGE POINTS "  << endl;
 
   int id, rc, ntasks;
    MPI_Status status;
    int tag_list = 207, tag_len=208;
    int help[100];

    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    // find exchange vertices - first send exchangevert_globnum, 
    // receive distant globnums
    // find matching

    const MeshTopology & topology = mesh.GetTopology();

    int ** distantlist, *ownlist, *sendlist;
    int numberexp = 0;

    BitArray isrefinedvertex ( mesh.GetNP() );
    isrefinedvertex.Clear();
    for ( int i = 1; i <= mesh.GetNP(); i++ )
      if ( IsExchangeVert(i) )    isrefinedvertex.Set(i-1);
      
    ARRAY<int> nexp (ntasks);
    for ( int i = 1; i <= mesh.GetNP(); i++ )
      if ( IsExchangeVert(i) )
        {
          numberexp ++;
        }

    nexp[id] = numberexp;

    for ( int sender = 0; sender < ntasks; sender ++ )
      {
        if ( id == sender )
          for ( int dest = 0; dest < ntasks; dest ++ )
            if ( dest != id)
              {
                 MyMPI_Send (numberexp, dest);
              }
	  

        if ( id != sender )
          {
	    MyMPI_Recv ( nexp[sender], sender);
          }
	  
       }
      

       
    distantlist = new int* [ntasks];
    for ( int i = 0; i < ntasks; i++ )
      {
        distantlist[i] = new int [2*nexp[i]];
      }

    ownlist = new int [2*numberexp];


    int jj = 0;
    for ( int i = 1; i <= loc2glob_vert.Size(); i++ )
      if ( IsExchangeVert(i) )
        {
          ownlist[jj] = i;
          ownlist[jj+numberexp] = GetLoc2Glob_Vert(i);
          jj ++;
        }


    for ( int sender = 0; sender < ntasks; sender ++ )
      {
        if ( id == sender )
          for ( int dest = 0; dest < ntasks; dest ++ )
            if ( dest != id )
              {
		int n2 = 2*numberexp;
		MyMPI_Send ( ownlist, n2, dest);
              }
	  
        if ( id != sender )
          {
	    int n2;
	    MyMPI_Recv ( distantlist[sender], n2, sender);
	    nexp [sender] = n2/2;
          }
 	  
	  
      }



    for ( int sender = 0; sender < ntasks; sender ++ )
      {
        if ( id == sender )
          for ( int dest = 0; dest < ntasks; dest ++ )
            if ( dest != id )
              {
		int n2 = 2*numberexp;
		MyMPI_Send ( n2, dest);
              }
	  
        if ( id != sender )
          {
	    int n2;
	    MyMPI_Recv ( n2, sender);
	    
          }
 	  
	  
      }

   
//      testout sent data
//     for ( int dest = 0; dest < ntasks; dest++ )
//       if ( dest != id )
//      	{
//      	  for ( int i = 0; i < 2*nexp[dest]; i++ )
//      	    std::cout  << distantlist[dest][i] << "  ";
//      	  std::cout << endl;
//      	}
  
    // compare distant global points mit own global points, falls stimmt, setze exchange_vert
    // AppendExchange_Vert ( locpnum, ARRAY ( proc, distpnum ) )
    
    int ii = 0;
  
	SetExchange_VertSize ( ntasks, mesh.GetNV() );
	
       
	
	ARRAY<int> intarray (2);
	for ( int dest = 0; dest < ntasks; dest++)
	  if ( dest != id )
	    {
	      for ( int locpnum = 0; locpnum < nexp[id]; locpnum++)
		for ( int distpnum=0; distpnum < nexp[dest]; distpnum++)
		  {
		    if ( ownlist[locpnum +numberexp] > 0 &&   // hat globale knotennummer - ist coarse mesh point
			 distantlist[dest][distpnum + nexp[dest]] == ownlist[locpnum +numberexp])
		      {
			intarray[0] = dest;
			intarray[1] = distantlist[dest][distpnum];
			AppendExchange_Vert ( ownlist[locpnum], dest, distantlist[dest][distpnum] );
			std::cout << "matching " << id << ": " << ownlist[locpnum] << ", " << dest << ": " <<
			  distantlist[dest][distpnum] << ",  global: " << ownlist[locpnum+numberexp] << endl;
			// der vertex ist bekannt, nicht neu
			isrefinedvertex.Clear ( ownlist[locpnum] - 1 );
			ii++;
		      }
		  }
	    }
    // and now the refined vertices...

    for ( int i = 1; i < mesh.GetNP(); i++)
      if ( isrefinedvertex.Test(i-1) ) isexchangevert.Clear(i-1);

    for ( int ed = 1; ed < topology.GetNEdges(); ed++)
      if ( IsExchangeEdge(ed) )
      {
	int v1, v2;
	topology.GetEdgeVertices(ed, v1, v2);
	std::cout << "edge " << ed << ": " << v1 << " " << v2 << endl;
      }
    /*
    int globpnum, locpnum;
    cout << "Update exchange points " << endl;
    for ( int i = 0; i < nv; i++ )
      if ( isexchangevert.Test(i) )
	{
	  locpnum = i;
	  globpnum = GetLoc2Glob_Vert(locpnum);
	  //	  std::cout << "exchange point is " << locpnum << " -> " << globpnum << endl;
	}
    */
  }

  void ParallelMeshTopology :: UpdateExchangeFaces () 
  {
    int id, rc, ntasks;
    MPI_Status status;
 
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    // find exchange edges - first send exchangeedges locnum, v1, v2
    // receive distant distnum, v1, v2
    // find matching

    const MeshTopology & topology = mesh.GetTopology();

    int ** distantlist, **ownsendlist, ** ownlocallist;
    int numberexfa = 0;
    int nfa = topology . GetNFaces ();

    // nsendexfa[dest] ... number of faces sent to processor dest
    ARRAY<int> nsendexfa (ntasks), nrecvexfa(ntasks), sendbufsize(ntasks);
    for ( int i = 0; i < ntasks; i++)
      {
	nsendexfa[i] = 0;
	sendbufsize[i] = 0;
      }

    for ( int fa = 1; fa <= nfa; fa++ )
      if ( IsExchangeFace(fa) )
        {
	  ARRAY<int> facevert;
	  topology . GetFaceVertices ( fa, facevert);

	
	  for ( int dest = 0; dest < ntasks; dest++)
	    {
	      bool exfa_dest = true;
	      for ( int j=0; j < facevert.Size(); j++)
		if ( GetDistantPNum ( dest, facevert[j] ) <= 0 )
		  exfa_dest = 0;
	      if ( exfa_dest )
		{
		  nsendexfa[dest] ++;
		  sendbufsize[dest] += 1 + facevert.Size();
		}
	    }
	    
        }

    nsendexfa[id] = numberexfa;
    nrecvexfa[id] = 1;

    for ( int sender = 0; sender < ntasks; sender ++ )
      {
        if ( id == sender )
          for ( int dest = 0; dest < ntasks; dest ++ )
            if ( dest != id)
              {
		MyMPI_Send (nsendexfa[dest], dest);
              }
	  

        if ( id != sender )
          {
	    MyMPI_Recv ( nrecvexfa[sender], sender);
          }
	  
      }
    
    
    distantlist = new int* [ntasks];
    ownsendlist = new int* [ntasks];
    ownlocallist = new int* [ntasks];

    for ( int sender = 0; sender < ntasks; sender++ )
      distantlist[sender] = new int;
     
    for ( int dest = 0; dest < ntasks; dest++)
      {
	ownsendlist[dest] = new int [sendbufsize[dest] ];
	ownlocallist[dest] = new int [sendbufsize[dest] ];
      }
    
    ARRAY<int> jj ( ntasks );
    for ( int i = 0; i < ntasks; i++ ) jj[i] = 0;


    // ownlist[dest] ..... localednum, distv1, distv2 
    for ( int fa = 1; fa <= nfa; fa++ )
      if ( IsExchangeFace(fa) )
        {
	  ARRAY<int> facevert;
	  topology . GetFaceVertices ( fa, facevert);

	
	  for ( int dest = 0; dest < ntasks; dest++)
	    {
	      bool exfa_dest = true;
	      for ( int j=0; j < facevert.Size(); j++)
		if ( GetDistantPNum ( dest, facevert[j] ) <= 0 )
		  exfa_dest = 0;
	      if ( exfa_dest )
		{
		  ownsendlist[dest][jj[dest]] = fa;
		  ownlocallist[dest][jj[dest]++] = fa;
		  for ( int k=0; k<facevert.Size(); k++ )
		    {
		      ownsendlist[dest][jj[dest]] = GetDistantPNum ( dest, facevert[k]);
		      ownlocallist[dest][jj[dest]++] = facevert[k];
		    }
		}
	    }
        }


    for ( int sender = 0; sender < ntasks; sender ++ )
      {
        if ( id == sender )
          for ( int dest = 0; dest < ntasks; dest ++ )
            if ( dest != id)
              {
		int n3 = sendbufsize[dest];
		MyMPI_Send (ownsendlist[dest], n3, dest);
              }
	  

        if ( id != sender )
          {
	    int n3;
	    MyMPI_Recv (distantlist[sender], n3, sender);
          }
	  
      }

    SetExchange_FaceSize ( ntasks, nfa );

    // find local facenums for received local vertices, add the local-distant exchange info
    for ( int sender = 0; sender < ntasks; sender++ )
      if ( sender != id )
	{
	  int ii = 0;
	  while (  ii < sendbufsize[sender] )
	    {
	      int locfacenum, distfacenum = distantlist[sender][ii++];
	      int nfavert = 3;
	      ARRAY<int> facevert(nfavert);
	      for (int fav=0; fav < nfavert; fav++)
		facevert[fav] = distantlist[sender][ii++];

	      //	    locedgenum = topology -> GetVerticesEdge( v1, v2 );

	      int iloc = 0;
	      while ( iloc < sendbufsize[sender])
		{
		  locfacenum = ownlocallist[sender][iloc++];
		  int nfavert_loc = 3;
		  bool match = true;
		  ARRAY<int> facevert_loc(nfavert_loc);
		  for (int fav=0; fav < nfavert_loc; fav++)
		    {
		      facevert_loc[fav] = ownlocallist[sender][iloc++];
		      if ( ! facevert.Contains ( facevert_loc[fav] ) )
			match = false;
		    }
		  if ( match )
		    {
		      AppendExchange_Face ( locfacenum, sender, distfacenum );
		    
		    }
		}
	      
	      
	    }
	}
    for ( int i = 0; i < ntasks; i++)
      {
	delete ownsendlist[i], ownlocallist[i], distantlist[i];
      }
    delete ownsendlist, ownlocallist, distantlist;
    ;
  }

  void ParallelMeshTopology :: UpdateExchangeEdges () 
  {
    int id, rc, ntasks;
    MPI_Status status;
 
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    // find exchange edges - first send exchangeedges locnum, v1, v2
    // receive distant distnum, v1, v2
    // find matching

    const MeshTopology & topology = mesh.GetTopology();

    int ** distantlist, **ownsendlist, ** ownlocallist;
    int numberexed = 0;
    int ned = topology . GetNEdges ();

    // nsendexed[dest] ... number of edges sent to processor dest
    ARRAY<int> nsendexed (ntasks), nrecvexed(ntasks);
    for ( int i = 0; i < ntasks; i++)
      nsendexed[i] = 0;

    for ( int ed = 1; ed <= ned; ed++ )
      if ( IsExchangeEdge(ed) )
        {
	  int v1, v2;
	  topology . GetEdgeVertices ( ed, v1, v2 );

	  for ( int dest = 0; dest < ntasks; dest++)
	    if ( GetDistantPNum ( dest, v1 ) > 0 && GetDistantPNum (dest, v2) > 0)
	    {
	      nsendexed[dest] ++;
	    }
        }

    nsendexed[id] = numberexed;
    nrecvexed[id] = 1;

    for ( int sender = 0; sender < ntasks; sender ++ )
      {
        if ( id == sender )
          for ( int dest = 0; dest < ntasks; dest ++ )
            if ( dest != id)
              {
                 MyMPI_Send (nsendexed[dest], dest);
              }
	  

        if ( id != sender )
          {
	    MyMPI_Recv ( nrecvexed[sender], sender);
          }
	  
       }
    
    distantlist = new int* [ntasks];
    ownsendlist = new int* [ntasks];
    ownlocallist = new int* [ntasks];

    for ( int sender = 0; sender < ntasks; sender++ )
        distantlist[sender] = new int [3*nrecvexed[sender]];
     
    for ( int dest = 0; dest < ntasks; dest++)
      {
	ownsendlist[dest] = new int [3*nsendexed[dest] ];
	ownlocallist[dest] = new int [3*nsendexed[dest] ];
      }

    ARRAY<int> jj ( ntasks );
    for ( int i = 0; i < ntasks; i++ ) jj[i] = 0;


    // ownlist[dest] ..... localednum, distv1, distv2 
    for ( int ed = 1; ed <= ned; ed++ )
      if ( IsExchangeEdge(ed) )
        {
	  int v1, v2;
	  topology . GetEdgeVertices ( ed, v1, v2 );

	  for ( int dest = 0; dest < ntasks; dest++)
	    if (  GetDistantPNum ( dest, v1 ) > 0 &&  GetDistantPNum (dest, v2) > 0)
	    {
	      ownsendlist[dest][jj[dest]] = ed;
	      ownlocallist[dest][jj[dest]++] = ed;
	      ownsendlist[dest][jj[dest]] = GetDistantPNum ( dest, v1 );
	      ownlocallist[dest][jj[dest]++] = v1;
	      ownsendlist[dest][jj[dest]] = GetDistantPNum ( dest, v2 );
	      ownlocallist[dest][jj[dest]++] = v2;
	    }
        }


    for ( int sender = 0; sender < ntasks; sender ++ )
      {
        if ( id == sender )
          for ( int dest = 0; dest < ntasks; dest ++ )
            if ( dest != id)
              {
		int n3 = 3 * nsendexed[dest];
		MyMPI_Send (ownsendlist[dest], n3, dest);
              }
	  

        if ( id != sender )
          {
	    int n3;
	    MyMPI_Recv (distantlist[sender], n3, sender);
          }
	  
      }

    SetExchange_EdgeSize ( ntasks, ned );

    // find local edgenums for received local vertices, add the local-distant exchange info
    for ( int sender = 0; sender < ntasks; sender++ )
      if ( sender != id )
	for ( int ii = 0; ii < nrecvexed[sender]; ii++)
	  {
	    int locedgenum, distedgenum = distantlist[sender][3*ii];
	    int v1 = distantlist[sender][3*ii+1];
	    int v2 = distantlist[sender][3*ii+2];
	 
	    //	    locedgenum = topology -> GetVerticesEdge( v1, v2 );

	    for ( int iloc=0; iloc < nsendexed[sender]; iloc++)
	      {
		locedgenum = ownlocallist[sender][3*iloc];
		int locv1 = ownlocallist[sender][3*iloc+1];
		int locv2 = ownlocallist[sender][3*iloc+2];
		if ( (v1 == locv1 && v2 == locv2) || (v1 == locv2 && v2 == locv1) )
		  {
		    AppendExchange_Edge ( locedgenum, sender, distedgenum );
		    break;
		  }
	      }


	  }


    for ( int i = 0; i < ntasks; i++)
      {
	delete ownsendlist[i], ownlocallist[i], distantlist[i];
      }
    delete ownsendlist, ownlocallist, distantlist;
    ;
  }

void ParallelMeshTopology :: PrintExchange_Vert(int i, int j)
  {
    std::cout << exchange_vert->Get(i,j) << endl;
  }


  void ParallelMeshTopology :: SetZero()
  {
    exchange_vert = 0;
    exchange_face = 0;
    exchange_edge = 0;

  }
}






#endif
