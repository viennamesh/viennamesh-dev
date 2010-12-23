#ifdef PARALLEL




#include "parallel.hpp"


namespace netgen
{






  void ParallelMeshTopology :: Reset ()
{
  *testout << "ParallelMeshTopology::Reset" << endl;

  if ( ntasks == 1 ) return;
  PrintMessage ( 4, "RESET");

  int nvold = nv;
  int nedold = ned;
  int nfaold = nfa;

  ne = mesh.GetNE();
  nv = mesh.GetNV();
  nseg = mesh.GetNSeg();
  nsurfel = mesh.GetNSE();

  ned = mesh.GetTopology().GetNEdges();
  nfa = mesh.GetTopology().GetNFaces();

  if ( !loc2distface )
    {
      loc2distface = new ARRAY<int> *[nfa];
      for ( int i = 0; i < nfa; i++ )
	{
	  loc2distface[i] = new ARRAY<int> (1);
	  (*loc2distface[i])[0] = -1;
	}
    }
  else if ( nfaold != nfa )
    {
      ARRAY<int> ** loc2distface2 = new ARRAY<int> * [nfa];
      for ( int i = 0; i < nfaold && i < nfa; i++ )
	{
	  loc2distface2[i] = new ARRAY<int> (*loc2distface[i]);
	}
      for ( int i = nfaold; i < nfa; i++ )
	{
	  loc2distface2[i] = new ARRAY<int> (1);
	  (*loc2distface2[i])[0] = -1;
	}
      for ( int i = 0; i < nfaold; i++ )
	delete loc2distface[i];
      delete [] loc2distface;

      loc2distface = loc2distface2;
    }

  if ( !loc2distedge )
    {
      loc2distedge = new ARRAY<int> *[ned];
      for ( int i = 0; i < ned; i++ )
	{
	  loc2distedge[i] = new ARRAY<int> (1);
	  (*loc2distedge[i])[0] = -1;
	}
    }
  else if ( nedold != ned )
    {
      ARRAY<int> ** loc2distedge2 = new ARRAY<int> * [ned];
      for ( int i = 0; i < nedold && i < ned; i++ )
	{
	  loc2distedge2[i] = new ARRAY<int> (*loc2distedge[i]);
	}
      for ( int i = nedold; i < ned; i++ )
	{
	  loc2distedge2[i] = new ARRAY<int> (1);
	  (*loc2distedge2[i])[0] = -1;
	}
      
      for ( int i = 0; i < nedold; i++ )
	delete loc2distedge[i];
      delete [] loc2distedge;

      loc2distedge = loc2distedge2;
    }

  if ( !isexchangevert ) 
    {
      isexchangevert = new BitArray (nv * ( ntasks+1 ));
      isexchangevert->Clear();
    }
  if ( !isexchangeedge ) 
    {
      isexchangeedge = new BitArray (ned*(ntasks+1) );
      isexchangeedge->Clear();
    }
  if ( !isexchangeface ) 
    {
      isexchangeface = new BitArray (nfa*(ntasks+1) );
      isexchangeface->Clear();
    }
  if ( !isexchangeel ) 
    {
      isexchangeel = new BitArray (ne*(ntasks+1) );
      isexchangeel->Clear();
    }


  // if the number of vertices did not change, return
  if ( nvold == nv ) return;

  // faces and edges get new numbers -> delete 
  isexchangeface -> SetSize(nfa*(ntasks+1) );
  isexchangeedge -> SetSize(ned*(ntasks+1) );
  isexchangeface -> Clear();
  isexchangeedge -> Clear();


  *testout << "call setnv" << endl;

  SetNV(nv);
  SetNE(ne);

  /*
  // numbers of elements, verts are kept
  BitArray * isexchangevert2 = new BitArray ( nv*(ntasks+1) );
  isexchangevert2->Clear();
  for ( int i = 0; i < min2(isexchangevert->Size(), isexchangevert2->Size()); i++)
    if ( isexchangevert->Test(i) ) isexchangevert2->Set (i);
  delete isexchangevert;
  isexchangevert = isexchangevert2;

  BitArray * isexchangeel2 = new BitArray ( ne*(ntasks+1) );
  isexchangeel2->Clear();
  for ( int i = 0; i < min2(isexchangeel->Size(), isexchangeel2.Size()); i++)
    if ( isexchangeel->Test(i) ) isexchangeel2->Set ( i);
  delete isexchangeel;
  isexchangeel = isexchangeel2;
  */

  if ( !isghostedge.Size() )
    {
      isghostedge.SetSize(ned);
      isghostedge.Clear();
    }
  if ( !isghostface.Size() )
    {
      isghostface.SetSize(nfa);
      isghostface.Clear();
    }

}


 ParallelMeshTopology :: ~ParallelMeshTopology ()
{
  if ( isexchangeface ) delete isexchangeface;
  if ( isexchangevert ) delete isexchangevert;
  if ( isexchangeedge ) delete isexchangeedge;
  if ( isexchangeel )   delete isexchangeel;

  /*
  if ( loc2distvert )
    {
      for ( int i = 0; i < nv; i++ )
	if ( loc2distvert[i]  ) delete loc2distvert[i];
      delete [] loc2distvert;
    }
  */

  if ( loc2distface )
    {
      for ( int i = 0; i < nfa; i++ )
	if ( loc2distface[i]  ) delete loc2distface[i];
      delete [] loc2distface;
    }

  if ( loc2distedge )
    {
      for ( int i = 0; i < ned; i++ )
	if ( loc2distedge[i]  ) delete loc2distedge[i];
      delete [] loc2distedge;
    }

  if ( loc2distel )
    {
      for ( int i = 0; i < ne; i++ )
	if ( loc2distel[i]  ) delete loc2distel[i];
      delete [] loc2distel;
    }

  if ( loc2distsurfel )
    {
      for ( int i = 0; i < nsurfel; i++ )
	if ( loc2distsurfel[i]  ) { delete loc2distsurfel[i];}
      delete [] loc2distsurfel;
    }

  if ( loc2distsegm )
    {
      for ( int i = 0; i < nseg; i++ )
	if ( loc2distsegm[i]  ) delete loc2distsegm[i];
      delete [] loc2distsegm;
    }
}



 ParallelMeshTopology :: ParallelMeshTopology ( const netgen::Mesh & amesh )
  : mesh(amesh)
{
  ned = 0; //mesh.GetTopology().GetNEdges();
  nfa = 0; //mesh.GetTopology().GetNFaces();
  nv = 0;
  ne = 0;
  np = 0;
  nseg = 0;
  nsurfel = 0;
  neglob = 0;
  nvglob = 0;

  nparel = 0;

  isexchangeface = 0;
  isexchangevert = 0;
  isexchangeel = 0;
  isexchangeedge = 0;

  coarseupdate = 0;

  isghostedge.SetSize(0);
  isghostface.SetSize(0);

  // loc2distvert = 0; 
  loc2distedge = 0;
  loc2distface = 0;
  loc2distel = 0;
  loc2distsegm = 0;
  loc2distsurfel = 0;

  overlap = 0;
}





const   int ParallelMeshTopology :: Glob2Loc_Vert (const int & globnum )
  {
    int locnum = -1;
    for (int i = 0; i < nv; i++)
      {
	if ( globnum == loc2distvert[i+1][0] )
	  {
	    locnum = i+1;
	  }
      }

    return locnum;
  }

 const int ParallelMeshTopology :: Glob2Loc_VolEl (const int & globnum )
  {
    int locnum = -1;
    for (int i = 0; i < ne; i++)
      {
	if ( globnum == (*loc2distel[i])[0] )
	  {
	    locnum = i+1;
	  }
      }
    return locnum;
  }

 const  int ParallelMeshTopology :: Glob2Loc_SurfEl (const int & globnum )
  {
    int locnum = -1;
    for (int i = 0; i < nsurfel; i++)
      {
	if ( globnum == (*loc2distsurfel[i])[0] )
	  {
	    locnum = i+1;
	  }
      }
    return locnum;
  }

  const int ParallelMeshTopology :: Glob2Loc_Segm (const int & globnum )
  {
    int locnum = -1;
    for (int i = 0; i < nseg; i++)
      {
	if ( globnum == (*loc2distsegm[i])[0] )
	  {
	    locnum = i+1;
	  }
      }
    return locnum;
  }

 

  void ParallelMeshTopology ::  Print() const
  {

      (*testout) << endl <<  "TOPOLOGY FOR PARALLEL MESHES" << endl << endl;


      for ( int i = 1; i <= nv; i++ )
       	if ( IsExchangeVert (i) )
	  {
	    (*testout) << "exchange point  " << i << ":  global " << GetLoc2Glob_Vert(i) << endl;
 	    for ( int dest = 0; dest < ntasks; dest ++)
	      if ( dest != id )
 	      if ( GetDistantPNum( dest, i  ) > 0 )
 		(*testout) << "   p" << dest << ": " << GetDistantPNum ( dest, i ) << endl; 
	  }

      for ( int i = 1; i <= ned; i++ )
	if ( IsExchangeEdge ( i ) )
	  {
	    int v1, v2;
	    mesh . GetTopology().GetEdgeVertices(i, v1, v2);
	    (*testout) << "exchange edge  " << i << ":  global vertices "  << GetLoc2Glob_Vert(v1) << "  " 
		       << GetLoc2Glob_Vert(v2) << endl;
	    for ( int dest = 0; dest < ntasks; dest++)
	      if ( GetDistantEdgeNum ( dest, i ) > 0 )
		if ( dest != id )
		{
		  (*testout) << "   p" << dest << ": " << GetDistantEdgeNum ( dest, i ) << endl;
		}
	  }


      for ( int i = 1; i <= nfa; i++ )
	if ( IsExchangeFace(i) )
	  {
	    ARRAY<int> facevert;
	    mesh . GetTopology().GetFaceVertices(i, facevert);
	    
	    (*testout) << "exchange face  " << i << ":  global vertices " ;
	    for ( int fi=0; fi < facevert.Size(); fi++)
	      (*testout) << GetLoc2Glob_Vert(facevert[fi]) << "  ";
	    (*testout) << endl; 
 	    for ( int dest = 0; dest < ntasks; dest++)
	      if ( dest != id )
	      {
		if ( GetDistantFaceNum ( dest, i ) >= 0 )
 		(*testout) << "   p" << dest << ": " << GetDistantFaceNum ( dest, i ) << endl;
	      }
	  }


      for ( int i = 1; i < mesh.GetNE(); i++)
	{
	  if ( !IsExchangeElement(i) ) continue;
	  ARRAY<int> vert;
	  const Element & el = mesh.VolumeElement(i);

	  (*testout) << "parallel local element " << i << endl;
	  
	  (*testout) << "vertices " ;
	  for ( int j = 0; j < el.GetNV(); j++)
	    (*testout) << el.PNum(j+1)  << "  ";
	  (*testout) << "is ghost " << IsGhostEl(i) << endl;
	  (*testout) << endl;


	}
  }





  const int  ParallelMeshTopology :: GetDistantPNum ( const int & proc, const int & locpnum ) const
  {
    if ( proc == 0 )
      return loc2distvert[locpnum][0];

    /*
    int i = 1; 
    // while ( i < loc2distvert[locpnum-1]->Size() )
    while ( i < loc2distvert[locpnum].Size() )
      if ( loc2distvert[locpnum][i] == proc )
	return loc2distvert[locpnum][i+1];
      else
	i += 2;
    */

    for (int i = 1; i < loc2distvert[locpnum].Size(); i += 2)
      if ( loc2distvert[locpnum][i] == proc )
	return loc2distvert[locpnum][i+1];

    return -1;
  } 

  const int  ParallelMeshTopology :: GetDistantFaceNum ( const int & proc, const int & locfacenum ) const
  {
    if ( proc == 0 )
      return (*loc2distface[locfacenum-1])[0];
    int i = 1; 
    while ( i < loc2distface[locfacenum-1]->Size() )
      if ( (*loc2distface[locfacenum-1])[i] == proc )
	return (*loc2distface[locfacenum-1])[i+1];
      else
	i += 2;
    return -1;
  } 

  const int  ParallelMeshTopology :: GetDistantEdgeNum ( const int & proc, const int & locedgenum ) const
  {
    if ( proc == 0 )
      return (*loc2distedge[locedgenum-1])[0];
    int i = 1; 
    while ( i < loc2distedge[locedgenum-1]->Size() )
      if ( (*loc2distedge[locedgenum-1])[i] == proc )
	return (*loc2distedge[locedgenum-1])[i+1];
      else
	i += 2;
    return -1;
  } 

  const int  ParallelMeshTopology :: GetDistantElNum ( const int & proc, const int & locelnum ) const
  {
    if ( proc == 0 )
      return (*loc2distel[locelnum-1])[0];
    int i = 1; 
    while ( i < loc2distel[locelnum-1]->Size() )
      if ( (*loc2distel[locelnum-1])[i] == proc )
	return (*loc2distel[locelnum-1])[i+1];
      else
	i += 2;
    return -1;
  } 


  void ParallelMeshTopology :: SetDistantFaceNum ( const int dest, const int & locnum, const int & distnum )
  {
    if ( dest == 0 )
      {
	(*loc2distface[locnum-1])[0] = distnum;
	return;
      }
    int i = 1;
    while ( i < loc2distface[locnum-1]->Size() )
      if ( (*loc2distface[locnum-1])[i] == dest )
	{
	  (*loc2distface[locnum-1])[i+1] = distnum;
	  return;
	}
      else
	i += 2;

    loc2distface[locnum-1] -> Append ( dest );
    loc2distface[locnum-1] -> Append ( distnum );
    return ;

  }

  void ParallelMeshTopology :: SetDistantPNum ( const int dest, const int & locnum, const int & distnum )
  {
    if ( dest == 0 )
      {
	loc2distvert[locnum][0] = distnum;
	return;
      }
    int i = 1;
    while ( i < loc2distvert[locnum].Size() )
      if ( loc2distvert[locnum][i] == dest )
	{
	  loc2distvert[locnum][i+1] = distnum;
	  return;
	}
      else
	i += 2;

    loc2distvert.Add (locnum, dest);  // [locnum-1] -> Append ( dest );
    loc2distvert.Add (locnum, distnum); // [locnum-1] -> Append ( distnum );
    return ;

  }
  void ParallelMeshTopology :: SetDistantEdgeNum ( const int dest, const int & locnum, const int & distnum )
  {
    if ( dest == 0 )
      {
	(*loc2distedge[locnum-1])[0] = distnum;
	return;
      }
    int i = 1;

    while ( i < loc2distedge[locnum-1]->Size() )
      if ( (*loc2distedge[locnum-1])[i] == dest )
	{
	  (*loc2distedge[locnum-1])[i+1] = distnum;
	  return;
	}
      else
	i += 2;

    loc2distedge[locnum-1] -> Append ( dest );
    loc2distedge[locnum-1] -> Append ( distnum );
    return ;

  }
  void ParallelMeshTopology :: SetDistantEl ( const int dest, const int & locnum, const int & distnum )
  {
    if ( dest == 0 )
      {
	(*loc2distel[locnum-1])[0] = distnum;
	return;
      }
    int i = 1;
    while ( i < loc2distel[locnum-1]->Size() )
      if ( (*loc2distel[locnum-1])[i] == dest )
	{
	  (*loc2distel[locnum-1])[i+1] = distnum;
	  return;
	}
      else
	i += 2;

    loc2distel[locnum-1] -> Append ( dest );
    loc2distel[locnum-1] -> Append ( distnum );
    return ;

  }
  void ParallelMeshTopology :: SetDistantSurfEl ( const int dest, const int & locnum, const int & distnum )
  {
     if ( dest == 0 )
      {
	(*loc2distsurfel[locnum-1])[0] = distnum;
	return;
      }
   int i = 1;
    while ( i < loc2distsurfel[locnum-1]->Size() )
      if ( (*loc2distsurfel[locnum-1])[i] == dest )
	{
	  (*loc2distsurfel[locnum-1])[i+1] = distnum;
	  return;
	}
      else
	i += 2;

    loc2distsurfel[locnum-1] -> Append ( dest );
    loc2distsurfel[locnum-1] -> Append ( distnum );
    return ;

  }
  void ParallelMeshTopology :: SetDistantSegm ( const int dest, const int & locnum, const int & distnum )
  {
    if ( dest == 0 )
      {
	(*loc2distsegm[locnum-1])[0] = distnum;
	return;
      }
    int i = 1;
    while ( i < loc2distsegm[locnum-1]->Size() )
      if ( (*loc2distsegm[locnum-1])[i] == dest )
	{
	  (*loc2distsegm[locnum-1])[i+1] = distnum;
	  return;
	}
      else
	i += 2;

    loc2distsegm[locnum-1] -> Append ( dest );
    loc2distsegm[locnum-1] -> Append ( distnum );
    return ;

  }

  void ParallelMeshTopology :: GetVertNeighbours ( const int vnum, ARRAY<int> & dests ) const
  {
    dests.SetSize(0);
    int i = 1;
    while ( i < loc2distvert[vnum].Size() )
      {
	dests.Append ( loc2distvert[vnum][i] );
	i+=2;
      }
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








void ParallelMeshTopology :: UpdateRefinement ()
{
  ;
}




void ParallelMeshTopology :: UpdateCoarseGridGlobal ()
{
  int timer = NgProfiler::CreateTimer ("UpdateCoarseGridGlobal");
  NgProfiler::RegionTimer reg(timer);


  PrintMessage ( 1, "UPDATE GLOBAL COARSEGRID  STARTS " );      // JS


  *testout << "ParallelMeshTopology :: UpdateCoarseGridGlobal" << endl;
  const MeshTopology & topology = mesh.GetTopology();
  
  ARRAY<int> sendarray, recvarray;

  nfa = topology . GetNFaces();
  ned = topology . GetNEdges();
  np = mesh . GetNP();
  nv = mesh . GetNV();
  ne = mesh . GetNE();
  nseg = mesh.GetNSeg();
  nsurfel = mesh.GetNSE();
  

  // low order processor - save mesh partition
  if ( id == 0 )
    {
      if ( !isexchangeel )
        {
          isexchangeel = new BitArray ( (ntasks+1) * ne );
          isexchangeel -> Clear();
        }


      for ( int eli = 1; eli <= ne; eli++ )
        {
          (*loc2distel[eli-1])[0] = eli;
          SetExchangeElement ( eli );
          const Element & el = mesh . VolumeElement ( eli );
          int dest = el . GetPartition ( );
          SetExchangeElement ( dest, eli );
	  
          for ( int i = 0; i < el.GetNP(); i++ )
            {
              SetExchangeVert ( dest, el.PNum(i+1) );
              SetExchangeVert ( el.PNum(i+1) );
            }
          ARRAY<int> edges;
          topology . GetElementEdges ( eli, edges );
          for ( int i = 0; i < edges.Size(); i++ )
            {
              SetExchangeEdge ( dest, edges[i] );
              SetExchangeEdge ( edges[i] );
            }
          topology . GetElementFaces ( eli, edges );
          for ( int i = 0; i < edges.Size(); i++ )
            {
              SetExchangeFace ( dest, edges[i] );
              SetExchangeFace ( edges[i] );
            }
          
        }
      
      for ( int i = 1; i <= mesh .GetNV(); i++)
        loc2distvert[i][0] = i;
      
      for ( int i = 0; i < mesh . GetNSeg(); i++)
        (*loc2distsegm[i])[0] = i+1;
      
      for ( int i = 0; i < mesh . GetNSE(); i++)
        (*loc2distsurfel[i])[0] = i+1;
      
      for ( int i = 0; i < topology .GetNEdges(); i++)
        (*loc2distedge[i])[0] = i+1;
      for ( int i = 0; i < topology .GetNFaces(); i++)
        (*loc2distface[i])[0] = i+1;
    }
  

    if ( id == 0 )
      sendarray.Append (nfa);

    BitArray recvface(nfa);
    recvface.Clear();
   
    ARRAY<int> edges, pnums, faces;
    for ( int el = 1; el <= ne; el++ )
      {
	topology.GetElementFaces (el, faces);
	int globeli = GetLoc2Glob_VolEl(el);

	for ( int fai = 0; fai < faces.Size(); fai++)
	  {
	    int fa = faces[fai];

            // face2edge ist sehr langsam, da nicht gespeichert
            // warum nicht alle edges vom Element ?

            // topology.GetFaceEdges ( fa, edges );  
            topology.GetElementEdges ( el, edges );

	    topology.GetFaceVertices ( fa, pnums );
            
            // send : 
            // localfacenum
            // np
            // ned
            // globalpnums
            // localpnums
            
            // localedgenums mit globalv1, globalv2
                
            sendarray. Append ( fa );
            sendarray. Append ( globeli );
            sendarray. Append ( pnums.Size() );
            sendarray. Append ( edges.Size() );

            if (id == 0)
              for ( int i = 0; i < pnums.Size(); i++ )
                sendarray. Append( pnums[i] );
            else
              for ( int i = 0; i < pnums.Size(); i++ )
                sendarray. Append( GetLoc2Glob_Vert(pnums[i]) );

            for ( int i = 0; i < pnums.Size(); i++ )
              sendarray. Append(pnums[i] );
                
            for ( int i = 0; i < edges.Size(); i++ )
              {
                sendarray. Append(edges[i] );
                int v1, v2;
                topology . GetEdgeVertices ( edges[i], v1, v2 );
                int dv1 = GetLoc2Glob_Vert ( v1 );
                int dv2 = GetLoc2Glob_Vert ( v2 );
                
                if (id > 0) if ( dv1 > dv2 ) swap ( dv1, dv2 );
                sendarray . Append ( dv1 );
                sendarray . Append ( dv2 );
              }
	  }
      }

    
    BitArray edgeisinit(ned), vertisinit(np);
    edgeisinit.Clear();
    vertisinit.Clear();
    
    // ARRAY for temporary use, to find local from global element fast
    // only for not too big meshes 
    // seems ok, as low-order space is treated on one proc
    ARRAY<int,1> glob2loc_el;
    if ( id != 0 ) 
      {
	glob2loc_el.SetSize (neglob);  //  = new ARRAY<int,1> ( neglob );
	glob2loc_el = -1;
	for ( int locel = 1; locel <= mesh.GetNE(); locel++)
	  glob2loc_el[GetLoc2Glob_VolEl(locel)] = locel;
      }

    
    MPI_Request sendrequest;

    if (id == 0)
      {
	PrintMessage (1, "UpdateCoarseGridGlobal : bcast, size = ", int (sendarray.Size()*sizeof(int)) );
	MyMPI_BCast ( sendarray );
      }
    else
      MyMPI_ISend ( sendarray, 0, sendrequest );


    int nloops = (id == 0) ? ntasks-1 : 1;
    for (int hi = 0; hi < nloops; hi++)
      {
	int sender;

	if (id == 0)
	  {
	    PrintMessage (1, "I am waiting");
	    sender = MyMPI_Recv ( recvarray );
	    PrintMessage (1, "have received from ", sender);
	  }
	else
	  {
	    MyMPI_BCast ( recvarray );
	    sender = 0;
	  }
	
	// compare received vertices with own ones
	int ii = 0;
	int cntel = 0;
	int volel = 1;

	if ( id != 0 )
	  nfaglob = (recvarray)[ii++];

	while ( ii < recvarray.Size() )
	  {
	    // receive list : 
	    // distant facenum
	    // np
	    // ned
	    // globalpnums
	    // distant pnums

	    // distant edgenums mit globalv1, globalv2

	    int distfa = (recvarray)[ii++];
	    int globvolel = (recvarray)[ii++];
	    int distnp = (recvarray)[ii++];
	    int distned =(recvarray)[ii++];

	    if ( id > 0 )      // GetLoc2Glob_VolEl ( volel ) != globvolel )
	      volel = glob2loc_el[globvolel]; //Glob2Loc_VolEl ( globvolel );
	    else
	      volel = globvolel;

	    if ( volel == -1 ) 
	      {
		ii += 2*distnp + 3*distned;
		volel = 1;
		continue;
	      }

	    ARRAY<int> faces, edges;
            ARRAY<int> pnums, globalpnums;

	    topology.GetElementFaces( volel, faces);
	    topology.GetElementEdges ( volel, edges);
	    for ( int fai= 0; fai < faces.Size(); fai++ )
	      {
		int fa = faces[fai];
		topology.GetFaceVertices ( fa, pnums );

		// find exchange faces ...
		// have to be of same type
		if ( pnums.Size () != distnp ) continue;

		globalpnums.SetSize ( distnp );
		for ( int i = 0; i < distnp; i++)
		  {
		    globalpnums[i] = GetLoc2Glob_Vert ( pnums[i] );
		  }


		// test if 3 vertices match
		bool match = 1;
		for ( int i = 0;  i < distnp; i++)
		  if ( !globalpnums.Contains ( (recvarray)[ii+i] ) )
		    match = 0;
		    
		if ( !match ) continue;

		//  recvface.Set(fa-1);


		SetDistantFaceNum ( sender, fa, distfa );

		// find exchange points
		for ( int i = 0;  i < distnp; i++)
		  {
		    int distglobalpnum = recvarray[ii+i];
		    for ( int j = 0; j < distnp; j++ )
		      if ( globalpnums[j] == distglobalpnum )
			{
			  // set sender -- distpnum  ---- locpnum
			  int distpnum = recvarray[ii + i +distnp];
			  SetDistantPNum ( sender, pnums[j], distpnum );
			}
		  }

		for ( int i = 0; i  < edges.Size(); i++)
		  {
		    int v1, v2;
		    topology . GetEdgeVertices ( edges[i], v1, v2 );
		    int dv1 = GetLoc2Glob_Vert ( v1 );
		    int dv2 = GetLoc2Glob_Vert ( v2 );
		    if ( dv1 > dv2 ) swap ( dv1, dv2 );
		    for ( int ed = 0; ed < distned; ed++)
		      {
			int distedgenums = recvarray[ii + 2*distnp + 3*ed];
			int ddv1 = recvarray[ii + 2*distnp + 3*ed + 1];
			int ddv2 = recvarray[ii + 2*distnp + 3*ed + 2];
			if ( ddv1 > ddv2 ) swap ( ddv1, ddv2 );
			if ( dv1 == ddv1 && dv2 == ddv2 )
			  {
			    // set sender -- distednum -- locednum
			    SetDistantEdgeNum ( sender, edges[i], distedgenums);
			  }
		      }
		  }
	      }
	    ii += 2*distnp + 3*distned;
	  }
      }


    coarseupdate = 1;
    
    if (id != 0)
      {
	MPI_Status status;
	MPI_Wait (&sendrequest, &status);
      }

#ifdef SCALASCA
#pragma pomp inst end(updatecoarsegrid)
#endif
}






  void ParallelMeshTopology :: UpdateCoarseGrid ()
  {
    int timer = NgProfiler::CreateTimer ("UpdateCoarseGrid");
    NgProfiler::RegionTimer reg(timer);

#ifdef SCALASCA
#pragma pomp inst begin(updatecoarsegrid)
#endif
    (*testout) << "UPDATE COARSE GRID PARALLEL TOPOLOGY " << endl;
    PrintMessage ( 1, "UPDATE COARSE GRID PARALLEL TOPOLOGY " );

    // find exchange edges - first send exchangeedges locnum, v1, v2
    // receive distant distnum, v1, v2
    // find matching
    const MeshTopology & topology = mesh.GetTopology();

    UpdateCoarseGridGlobal();


    if ( id == 0 ) return;

    ARRAY<int> * sendarray, *recvarray;
    sendarray = new ARRAY<int> (0);
    recvarray = new ARRAY<int>;

    nfa = topology . GetNFaces();
    ned = topology . GetNEdges();
    np = mesh . GetNP();
    nv = mesh . GetNV();
    ne = mesh . GetNE();
    nseg = mesh.GetNSeg();
    nsurfel = mesh.GetNSE();
    

    sendarray->SetSize (0);

    BitArray recvface(nfa);
    recvface.Clear();
    for ( int fa = 1; fa <= nfa; fa++ )
      {

	if ( !IsExchangeFace ( fa ) ) continue;

	ARRAY<int> edges, pnums;
	int globfa = GetDistantFaceNum ( 0, fa );

	topology.GetFaceEdges ( fa, edges );
	topology.GetFaceVertices ( fa, pnums );
	
	
	// send : 
	// localfacenum
	// globalfacenum
	// np
	// ned
	// globalpnums
	// localpnums
	
	// localedgenums mit globalv1, globalv2
	// 
	
	sendarray -> Append ( fa );
	sendarray -> Append ( globfa );
	sendarray -> Append ( pnums.Size() );
	sendarray -> Append ( edges.Size() );
	for ( int i = 0; i < pnums.Size(); i++ )
	  {
	    sendarray -> Append( GetLoc2Glob_Vert(pnums[i]) );
	  }
	for ( int i = 0; i < pnums.Size(); i++ )
	  {
	    sendarray -> Append(pnums[i] );
	  }
	for ( int i = 0; i < edges.Size(); i++ )
	  {
	    sendarray -> Append(edges[i] );
	    int v1, v2;
	    topology . GetEdgeVertices ( edges[i], v1, v2 );
	    int dv1 = GetLoc2Glob_Vert ( v1 );
	    int dv2 = GetLoc2Glob_Vert ( v2 );
	    sendarray -> Append ( dv1 );
	    sendarray -> Append ( dv2 );
	  }
      }

    BitArray edgeisinit(ned), vertisinit(np);
    edgeisinit.Clear();
    vertisinit.Clear();
    
    // ARRAY for temporary use, to find local from global element fast
    // only for not too big meshes 
    // seems ok, as low-order space is treated on one proc
    ARRAY<int,1> * glob2locfa;
    glob2locfa = new ARRAY<int,1> ( nfaglob );
    (*glob2locfa) = -1;

    for ( int locfa = 1; locfa <= nfa; locfa++)
      {
	if ( !IsExchangeFace ( locfa ) ) continue;
	(*glob2locfa)[GetDistantFaceNum(0, locfa) ] = locfa;
      }

    for ( int sender = 1; sender < ntasks; sender ++ )
      {
	if ( id == sender )
	  {
	    for ( int dest = 1; dest < ntasks; dest ++ )
	      if ( dest != id)
		{
		  MyMPI_Send (*sendarray, dest);
		}
	  }
	    
	if ( id != sender )
	  {
	    MyMPI_Recv ( *recvarray, sender);
		
	    // compare received vertices with own ones
	    int ii = 0;
	    int cntel = 0;
	    int locfa = 1;

	    while ( ii< recvarray -> Size() )
	      {
		
		// receive list : 
		// distant facenum
		// global facenum
		// np
		// ned
		// globalpnums
		// distant pnums
		
		// distant edgenums mit globalv1, globalv2
		
		int distfa = (*recvarray)[ii++];
		int globfa = (*recvarray)[ii++];
		int distnp = (*recvarray)[ii++];
		int distned =(*recvarray)[ii++];
		
		int locfa = (*glob2locfa) [globfa];
		
		if ( locfa == -1 ) 
		  {
		    ii += 2*distnp + 3*distned;
		    locfa = 1;
		    continue;
		  }
		
		ARRAY<int> edges;
		int fa = locfa;
		
		ARRAY<int> pnums, globalpnums;
		topology.GetFaceEdges ( fa, edges );
		topology.GetFaceVertices ( fa, pnums );
		
		
		globalpnums.SetSize ( distnp );
		for ( int i = 0; i < distnp; i++)
		  globalpnums[i] = GetLoc2Glob_Vert ( pnums[i] );
		
		SetDistantFaceNum ( sender, fa, distfa );
		
		// find exchange points
		for ( int i = 0;  i < distnp; i++)
		  {
		    int distglobalpnum = (*recvarray)[ii+i];
		    for ( int j = 0; j < distnp; j++ )
		      if ( globalpnums[j] == distglobalpnum )
			{
			  // set sender -- distpnum  ---- locpnum
			  int distpnum = (*recvarray)[ii + i +distnp];
			  SetDistantPNum ( sender, pnums[j], distpnum );
			}
		    
		  }
			
		int * distedgenums  = new int [distned];
		// find exchange edges
		for ( int i = 0; i  < edges.Size(); i++)
		  {
		    int v1, v2;
		    topology . GetEdgeVertices ( edges[i], v1, v2 );
		    int dv1 = GetLoc2Glob_Vert ( v1 );
		    int dv2 = GetLoc2Glob_Vert ( v2 );
		    if ( dv1 > dv2 ) swap ( dv1, dv2 );
		    for ( int ed = 0; ed < distned; ed++)
		      {
			distedgenums[ed] = (*recvarray)[ii + 2*distnp + 3*ed];
			int ddv1 = (*recvarray)[ii + 2*distnp + 3*ed + 1];
			int ddv2 = (*recvarray)[ii + 2*distnp + 3*ed + 2];
			if ( ddv1 > ddv2 ) swap ( ddv1, ddv2 );
			if ( dv1 == ddv1 && dv2 == ddv2 )
			  {
			    // set sender -- distednum -- locednum
			    SetDistantEdgeNum ( sender, edges[i], distedgenums[ed] );
			  }
		      }
		    
		    
		  }
		delete [] distedgenums; 
		
		ii += 2*distnp + 3*distned;
		
	      }
	  }
	
	
	
	
      }
  

    // set which elements are where for the master processor

    delete sendarray; delete recvarray;
    if ( id > 0 )
      delete glob2locfa;
    coarseupdate = 1;
	
#ifdef SCALASCA
#pragma pomp inst end(updatecoarsegrid)
#endif
  }

  void ParallelMeshTopology :: UpdateCoarseGridOverlap ()
  {

    UpdateCoarseGridGlobal();

#ifdef SCALASCA
#pragma pomp inst begin(updatecoarsegrid)
#endif
    (*testout) << "UPDATE COARSE GRID PARALLEL TOPOLOGY, OVERLAP " << endl;
    PrintMessage ( 1, "UPDATE COARSE GRID PARALLEL TOPOLOGY, OVERLAP " );

    const MeshTopology & topology = mesh.GetTopology();

    nfa = topology . GetNFaces();
    ned = topology . GetNEdges();
    np = mesh . GetNP();
    nv = mesh . GetNV();
    ne = mesh . GetNE();
    nseg = mesh.GetNSeg();
    nsurfel = mesh.GetNSE();


    if ( id != 0 )
      {

    // find exchange edges - first send exchangeedges locnum, v1, v2
    // receive distant distnum, v1, v2
    // find matching

    ARRAY<int> * sendarray, *recvarray;
    sendarray = new ARRAY<int> (0);
    recvarray = new ARRAY<int>;
    

    sendarray->SetSize (0);

    BitArray recvface(nfa);
    recvface.Clear();
   
    for ( int el = 1; el <= ne; el++ )
      {
	ARRAY<int> edges, pnums, faces;
	topology.GetElementFaces (el, faces);
	int globeli = GetLoc2Glob_VolEl(el);
	for ( int fai = 0; fai < faces.Size(); fai++)
	  {
	    int fa = faces[fai];

	    topology.GetFaceEdges ( fa, edges );
	    topology.GetFaceVertices ( fa, pnums );
	    
	    if ( !IsExchangeElement ( el ) ) continue;

	    int globfa = GetDistantFaceNum(0, fa) ;

	    // send : 
	    // localfacenum
	    // globalfacenum
	    // globalelnum
	    // np
	    // ned
	    // globalpnums
	    // localpnums
	    
	    // localedgenums mit globalelnums mit globalv1, globalv2
	    // 

	    sendarray -> Append ( fa );
	    sendarray -> Append ( globfa );
	    sendarray -> Append ( globeli );
	    sendarray -> Append ( pnums.Size() );
	    sendarray -> Append ( edges.Size() );
	    for ( int i = 0; i < pnums.Size(); i++ )
	      {
		sendarray -> Append( GetLoc2Glob_Vert(pnums[i]) );
	      }
	    for ( int i = 0; i < pnums.Size(); i++ )
	      {
		sendarray -> Append(pnums[i] );
  	      }
	    for ( int i = 0; i < edges.Size(); i++ )
	      {
		int globedge = GetDistantEdgeNum(0, edges[i] );
		int v1, v2;
		topology . GetEdgeVertices ( edges[i], v1, v2 );
		int dv1 = GetLoc2Glob_Vert ( v1 );
		int dv2 = GetLoc2Glob_Vert ( v2 );

		sendarray -> Append(edges[i] );
		sendarray -> Append (globedge);
		sendarray -> Append ( dv1 );
		sendarray -> Append ( dv2 );
	      }
	  }
      }
    
    BitArray edgeisinit(ned), vertisinit(np);
    edgeisinit.Clear();
    vertisinit.Clear();
    
    // ARRAY for temporary use, to find local from global element fast
    // only for not too big meshes 
    // seems ok, as low-order space is treated on one proc
    ARRAY<int,1> * glob2loc_el;

    glob2loc_el = new ARRAY<int,1> ( neglob );
    (*glob2loc_el) = -1;
    for ( int locel = 1; locel <= mesh.GetNE(); locel++)
      (*glob2loc_el)[GetLoc2Glob_VolEl(locel)] = locel;
      
    for ( int sender = 1; sender < ntasks; sender ++ )
      {
	if ( id == sender )
	  {
	    for ( int dest = 1; dest < ntasks; dest ++ )
	      if ( dest != id)
		{
		  MyMPI_Send (*sendarray, dest);
		}
	  }
	    
	if ( id != sender )
	  {
	    MyMPI_Recv ( *recvarray, sender);
		
	    // compare received vertices with own ones
	    int ii = 0;
	    int cntel = 0;
	    int volel = 1;

	    while ( ii< recvarray -> Size() )
	      {

		// receive list : 
		// distant facenum
		// np
		// ned
		// globalpnums
		// distant pnums

		// distant edgenums mit globalv1, globalv2

		int distfa = (*recvarray)[ii++];
		int globfa = (*recvarray)[ii++];
		int globvolel = (*recvarray)[ii++];
		int distnp = (*recvarray)[ii++];
		int distned =(*recvarray)[ii++];

		if ( id > 0 ) // GetLoc2Glob_VolEl ( volel ) != globvolel )
		  volel = (*glob2loc_el)[globvolel]; //Glob2Loc_VolEl ( globvolel );
		else
		  volel = globvolel;

		if ( volel == -1 ) 
		  {
		    ii += 2*distnp + 4*distned;
		    volel = 1;
		    continue;
		  }

		ARRAY<int> faces, edges;
		topology.GetElementFaces( volel, faces);
		topology.GetElementEdges ( volel, edges);
		for ( int fai= 0; fai < faces.Size(); fai++ )
		  {
		    int fa = faces[fai];
		    if ( !IsExchangeFace ( fa ) && sender != 0 ) continue;
		    //		    if ( recvface.Test ( fa-1 ) ) continue;

		    ARRAY<int> pnums, globalpnums;
		    //topology.GetFaceEdges ( fa, edges );
		    topology.GetFaceVertices ( fa, pnums );


		    // find exchange faces ...
		    // have to be of same type
		    if ( pnums.Size () != distnp ) continue;


		    globalpnums.SetSize ( distnp );
		    for ( int i = 0; i < distnp; i++)
		      globalpnums[i] = GetLoc2Glob_Vert ( pnums[i] );





		    // test if 3 vertices match
		    bool match = 1;
		    for ( int i = 0;  i < distnp; i++)
		      if ( !globalpnums.Contains ( (*recvarray)[ii+i] ) )
			match = 0;
		    
		    if ( !match ) continue;

		    //  recvface.Set(fa-1);

		    SetDistantFaceNum ( sender, fa, distfa );

		    SetDistantFaceNum ( 0, fa, globfa );

		    // find exchange points
		    for ( int i = 0;  i < distnp; i++)
		      {
			int distglobalpnum = (*recvarray)[ii+i];
			for ( int j = 0; j < distnp; j++ )
			  if ( globalpnums[j] == distglobalpnum )
			    {
			      // set sender -- distpnum  ---- locpnum
			      int distpnum = (*recvarray)[ii + i +distnp];
			      SetDistantPNum ( sender, pnums[j], distpnum );
			    }
			
		      }

		    int * distedgenums  = new int [distned];
		    // find exchange edges
      		    for ( int i = 0; i  < edges.Size(); i++)
		      {
			int v1, v2;
			topology . GetEdgeVertices ( edges[i], v1, v2 );
			int dv1 = GetLoc2Glob_Vert ( v1 );
			int dv2 = GetLoc2Glob_Vert ( v2 );
			if ( dv1 > dv2 ) swap ( dv1, dv2 );
			for ( int ed = 0; ed < distned; ed++)
			  {
			    distedgenums[ed] = (*recvarray)[ii + 2*distnp + 4*ed];
			    int globedgenum = (*recvarray)[ii + 2*distnp + 4*ed + 1];
			    int ddv1 = (*recvarray)[ii + 2*distnp + 4*ed + 2];
			    int ddv2 = (*recvarray)[ii + 2*distnp + 4*ed + 3];
			    if ( ddv1 > ddv2 ) swap ( ddv1, ddv2 );
			    if ( dv1 == ddv1 && dv2 == ddv2 )
			      {
				// set sender -- distednum -- locednum
				SetDistantEdgeNum ( sender, edges[i], distedgenums[ed] );
				SetDistantEdgeNum ( 0, edges[i], globedgenum );
			      }
			  }


		      }
		    delete [] distedgenums; 


		  }
		ii += 2*distnp + 4*distned;
	      }
		
	    
	    
	     
          }
	
      }

    // set which elements are where for the master processor

    delete sendarray; delete recvarray;
    if ( id > 0 )
      delete glob2loc_el;
    coarseupdate = 1;
	
      }


    // send global-local el/face/edge/vert-info to id 0


//     nfa = topology . GetNFaces();
//     ned = topology . GetNEdges();
//     np = mesh . GetNP();
//     nv = mesh . GetNV();
//     ne = mesh . GetNE();
//     nseg = mesh.GetNSeg();
//     nsurfel = mesh.GetNSE();
    if ( id != 0 )
      {
	ARRAY<int> * sendarray;
	sendarray = new ARRAY<int> (4);

	int sendnfa = 0, sendned = 0;

	(*sendarray)[0] = ne;
	(*sendarray)[1] = nfa;
	(*sendarray)[2] = ned;
	(*sendarray)[3] = np;

	int ii = 4;
	for ( int el = 1; el <= ne; el++ )
	  (*sendarray).Append ( GetLoc2Glob_VolEl (el ) );

	for ( int fa = 1; fa <= nfa; fa++ )
	  {
	    if ( !IsExchangeFace (fa) ) continue;
	    sendnfa++;
	    (*sendarray).Append ( fa );
	    (*sendarray).Append ( GetDistantFaceNum (0, fa) );
	  }

	for ( int ed = 1; ed <= ned; ed++ )
	  {
	    if ( !IsExchangeEdge (ed) ) continue;
	    sendned++;
	    sendarray->Append ( ed );
	    sendarray->Append ( GetDistantEdgeNum(0, ed) );
	  }

	for ( int vnum = 1; vnum <= np; vnum++ )
	  sendarray->Append ( GetLoc2Glob_Vert(vnum) );

	(*sendarray)[1] = sendnfa;
	(*sendarray)[2] = sendned;

	MyMPI_Send (*sendarray, 0);

	delete sendarray;
      }

    else
      {
	ARRAY<int> * recvarray = new ARRAY<int>;

	for ( int sender = 1; sender < ntasks; sender++ )
	  {
	    MyMPI_Recv ( *recvarray, sender);

	    int distnel = (*recvarray)[0];
	    int distnfa = (*recvarray)[1];
	    int distned = (*recvarray)[2];
	    int distnp = (*recvarray)[3];

	    int ii = 4;

	    for ( int el = 1; el <= distnel; el++ )
	      SetDistantEl ( sender, (*recvarray)[ii++], el );

	    for ( int fa = 1; fa <= distnfa; fa++ )
	      {
		int distfa = (*recvarray)[ii++];
		SetDistantFaceNum ( sender, (*recvarray)[ii++], distfa );
	      }
	    for ( int ed = 1; ed <= distned; ed++ )
	      {
		int disted = (*recvarray)[ii++];
		SetDistantEdgeNum ( sender, (*recvarray)[ii++], disted );
	      }
	    for ( int vnum = 1; vnum <= distnp; vnum++ )
	      SetDistantPNum ( sender, (*recvarray)[ii++], vnum );
	  }

	delete recvarray;
      }    
#ifdef SCALASCA
#pragma pomp inst end(updatecoarsegrid)
#endif
  }



  void ParallelMeshTopology :: UpdateTopology () 
  {
    // loop over parallel faces and edges, find new local face/edge number, 

    const MeshTopology & topology = mesh.GetTopology();
    int nfa = topology.GetNFaces();
    int ned = topology.GetNEdges();

    isghostedge.SetSize(ned);
    isghostface.SetSize(nfa);
    isghostedge.Clear();
    isghostface.Clear();

    for ( int ed = 1; ed <= ned; ed++)
      {
	int v1, v2;
	topology.GetEdgeVertices ( ed, v1, v2 );
	if ( IsGhostVert(v1) || IsGhostVert(v2) )
	  SetGhostEdge ( ed );
      }


    for ( int fa = 1; fa <= nfa; fa++)
      {
        ARRAY<int> pnums;
	topology.GetFaceVertices ( fa, pnums );
	for ( int i = 0; i < pnums.Size(); i++)
	  if ( IsGhostVert( pnums[i] ) )
	    {
	      SetGhostFace ( fa );
	      break;
	    }
      }


  }


void ParallelMeshTopology :: UpdateExchangeElements()
{

  (*testout) << "UPDATE EXCHANGE ELEMENTS " << endl;
  const MeshTopology & topology = mesh.GetTopology();

  isexchangeedge->SetSize ( (ntasks+1) * topology.GetNEdges() );
  isexchangeface->SetSize ( (ntasks+1) * topology.GetNFaces() );

  isexchangeedge->Clear();
  isexchangeface->Clear();

  for ( int eli = 1; eli <= mesh.GetNE(); eli++)
    {
      if ( ! IsExchangeElement ( eli ) ) continue;
      const Element & el = mesh.VolumeElement(eli);
      ARRAY<int> faces, edges;
      int np = el.NP();

      topology.GetElementEdges ( eli, edges );
      topology.GetElementFaces ( eli, faces );
      for ( int i = 0; i < edges.Size(); i++)
	{
	  SetExchangeEdge ( edges[i] );
	}
      for ( int i = 0; i < faces.Size(); i++)
	{
	  SetExchangeFace ( faces[i] );
	}
      for ( int i = 0; i < np; i++)
	{
	  SetExchangeVert ( el[i] );
	}
    }
}




void ParallelMeshTopology :: SetNV ( const int anv )
  {
    *testout << "called setnv"  << endl
             << "old size: " << loc2distvert.Size() << endl
             << "new size: " << anv << endl;
    /*
    ARRAY<int> ** loc2distvert2;
    loc2distvert2 = new ARRAY<int> * [anv];
    for ( int i = 0; i < anv; i++ )
      loc2distvert2[i] = new ARRAY<int> (1);

    if ( loc2distvert ) 
      {
	for ( int i = 0; i < min2(nv, anv); i++ ) 
	  {
	    loc2distvert2[i] -> SetSize ( loc2distvert[i]->Size() );
	    for ( int j = 0; j < loc2distvert[i]->Size(); j++ )
	      (*loc2distvert2[i])[j] = (*loc2distvert[i])[j];
	    delete loc2distvert[i];
	  }
	delete [] loc2distvert;
      }
    */

    loc2distvert.ChangeSize (anv);
    for (int i = 1; i <= anv; i++)
      if (loc2distvert.EntrySize(i) == 0)
        loc2distvert.Add (i, -1);  // will be the global nr

    BitArray * isexchangevert2 = new BitArray( (ntasks+1) * anv );
    isexchangevert2->Clear();
    if ( isexchangevert )
      {
	for ( int i = 0; i < min2( isexchangevert->Size(), isexchangevert2->Size() ); i++ )
	  if ( isexchangevert->Test(i) ) isexchangevert2->Set(i);
	delete isexchangevert;
      }

    // loc2distvert= loc2distvert2;
    isexchangevert = isexchangevert2;
    nv = anv;

  }

void ParallelMeshTopology :: SetNE ( const int ane )
  {
    ARRAY<int> ** loc2distel2;

    loc2distel2 = new ARRAY<int> * [ane];
    for ( int i = 0; i < ane; i++ )
      loc2distel2[i] = new ARRAY<int> (1);

    if ( loc2distel ) 
      {
	for ( int i = 0; i < min2(ne, ane)  ; i++ )
	  {
	    loc2distel2[i] -> SetSize ( loc2distel[i]->Size() );
	    for ( int j = 0; j < loc2distel[i]->Size(); j++ )
	      (*loc2distel2[i])[j] = (*loc2distel[i])[j];
	    delete loc2distel[i];
	  }
	delete [] loc2distel;
      }

    BitArray * isexchangeel2 = new BitArray ( (ntasks+1) * ane );
    isexchangeel2->Clear();
    if ( isexchangeel )
      {
	for ( int i = 0; i < min2(isexchangeel->Size(), isexchangeel2->Size() ) ; i++ )
	  if ( isexchangeel->Test(i) ) isexchangeel2->Set(i);
	delete isexchangeel;
      }


    ne = ane;
    loc2distel = loc2distel2;
    isexchangeel = isexchangeel2;

  }

void ParallelMeshTopology :: SetNSE ( const int anse )
  {
    ARRAY<int> ** loc2distsurfel2;

    loc2distsurfel2 = new ARRAY<int> * [anse];
    for ( int i = 0; i < anse; i++ )
      loc2distsurfel2[i] = new ARRAY<int> (1);

    if ( loc2distsurfel ) 
      {
	for ( int i = 0; i < min2(nsurfel, anse); i++ )
	  {
	    loc2distsurfel2[i] -> SetSize ( loc2distsurfel[i]->Size() );
	    for ( int j = 0; j < loc2distsurfel[i]->Size(); j++ )
	      (*loc2distsurfel2[i])[j] = (*loc2distsurfel[i])[j];
	    delete loc2distsurfel[i];
	  }
	delete [] loc2distsurfel;
      }

   nsurfel = anse;
   loc2distsurfel = loc2distsurfel2;

  }

void ParallelMeshTopology :: SetNSegm ( const int anseg )
  {
    ARRAY<int> ** loc2distsegm2;

    loc2distsegm2 = new ARRAY<int> * [anseg];
    for ( int i = 0; i < anseg; i++ )
      loc2distsegm2[i] = new ARRAY<int> (1);

   if ( loc2distsegm ) 
      {for ( int i = 0; i < nseg; i++ )
	  {
	    loc2distsegm2[i] -> SetSize ( loc2distsegm[i]->Size() );
	    for ( int j = 0; j < loc2distsegm[i]->Size(); j++ )
	      (*loc2distsegm2[i])[j] = (*loc2distsegm[i])[j];
	    delete loc2distsegm[i];
	  }
	delete [] loc2distsegm;
      }

   nseg = anseg;
   loc2distsegm = loc2distsegm2;
  }


}




#endif
