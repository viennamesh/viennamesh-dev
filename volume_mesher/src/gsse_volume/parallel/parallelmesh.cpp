#ifdef PARALLEL


#include "parallel.hpp"
using namespace metis;

namespace netgen
{


void Mesh :: LoadParallelMesh ( )
  {
    PrintMessage (1,  "LOAD PARALLEL MESH");
    int id, rc, ntasks;
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    //    testout = new ofstream (string("testout_proc") + id  );      
    
    char str[100];
    int intbuf[100];
    double doublebuf[100];
    int i, n;
    double scale = 1;  // globflags.GetNumFlag ("scale", 1);
    int inverttets = 0;  // globflags.GetDefineFlag ("inverttets");
    int invertsurf = 0;  // globflags.GetDefineFlag ("invertsurfacemesh");
    int tag_dim = 10, tag_token = 100, tag_n=11, tag_pnum=12, tag_point=13;
    int tag_index = 101, tag_facedescr = 102;
    MPI_Status status;
    //       cout << "proc " << id << "   " << facedecoding.Size() << endl;  
    //     facedecoding.SetSize(0);
//       cout << "proc " << id  << endl;  
    
    bool endmesh = false;

    int nv=0, ne=0, ned=0, nfa=0;

    int nelglob, nelloc, nvglob, nedglob, nfaglob;

    int np = 0;

    string st;

    int dim;
    MyMPI_Recv ( nelglob, 0 );
    MyMPI_Recv(nelloc, 0 );
    MyMPI_Recv( nvglob, 0);
    MyMPI_Recv( nedglob, 0 );
    MyMPI_Recv( nfaglob, 0 );
    MyMPI_Recv ( dimension, 0);

    paralleltop->SetLocNV (0);

    int ve = 0;
    while (!endmesh)
      {

	MyMPI_Recv ( st, 0 );
       
 	if (strcmp (st.c_str(), "vertex") == 0)
 	  {
	    int np = 0;
	    int numvert, buflen;
	    //	    MyMPI_Recv ( numvert, 0);
	    double * pointarray;
	    pointarray = new double;
	    //	    MPI_Recv( pointarray , 4*numvert, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	    MyMPI_Recv( pointarray, buflen, 0 );
	    numvert = buflen / 5;
	    for ( int vert=0; vert<numvert; vert++ )
	      {
		paralleltop->AppendLoc2Glob_Vert ( int (pointarray[ vert*5 ]) );
		Point3d p;
		p.X() = scale * pointarray[ vert*5+1];
		p.Y() = scale * pointarray[ vert*5+2];
		p.Z() = scale * pointarray[ vert*5+3];
		AddPoint (p);
		np ++;
		(*this)[PointIndex(np) ] .Singularity ( pointarray[ vert*5+4] );
		
	      }

	    delete pointarray;

	    std::cout << "received points" << endl;
 	  }

	if ( strcmp (st.c_str(), "volumeelements" ) == 0 )
	  {

	    Element el;
	   

	    int arraysize;
 	    double * elarray;
	    elarray = new double;
	    MyMPI_Recv ( elarray, arraysize, 0);
	    int ind = 0;
	    while ( ind < arraysize )
	      {
		paralleltop->AppendLoc2Glob_VolEl ( int ( elarray[ind++] ) );

		el.SetIndex(int (elarray[ind++]));
		el.SetNP(int(elarray[ind++]));

		for ( int j = 0; j < el.GetNP(); j++)
		  {
		    (int&) el[j] = paralleltop->Glob2Loc_Vert(int(elarray[ind++]));
		  }
		
		if (inverttets)
		  el.Invert();
		
		AddVolumeElement (el);
		
		
	      }
	    std::cout << "  received volels " << endl;
	  }

	if (strcmp (st.c_str(), "facedescriptor") == 0)
	  {
    
	    //	    MPI_Recv( ibuf, 6, MPI_INT, 0, tag_facedescr, MPI_COMM_WORLD, &status);
	    int nn;
	    double * doublebuf = new double;
	    MyMPI_Recv( doublebuf, nn, 0 );
	    int faceind = AddFaceDescriptor (FaceDescriptor(int(doublebuf[0]), int(doublebuf[1]), int(doublebuf[2]), 0));
	    GetFaceDescriptor(faceind).SetBCProperty (int(doublebuf[3]));
	    GetFaceDescriptor(faceind).domin_singular = doublebuf[4];
	    GetFaceDescriptor(faceind).domout_singular = doublebuf[5];
	    std::cout << "received fdi " << endl;
	    delete doublebuf;
	  }
       


	if (strcmp (st.c_str(), "surfaceelementsgi") == 0)
	  {
	    int j;
	    int surfnr, bcp, domin, domout, nep, faceind = 0;
	    int globsel;
	    int * selbuf;
	    selbuf = new int;
	    int bufsize;
	    // receive:
	    // faceindex
	    // nep
	    // tri.pnum
	    // tri.geominfopi.trignum
	    int nlocsel;
	    MyMPI_Recv ( selbuf, bufsize, 0);
	    int ii= 0;
	    int sel = 0;
	    while ( ii < bufsize-1 )
	      {
		globsel = selbuf[ii++];
		faceind = selbuf[ii++];
		nep = selbuf[ii++];
		Element2d tri(nep);
		tri.SetIndex(faceind);
		for( j=1; j<=nep; j++)
		  {
		    tri.PNum(j) = paralleltop->Glob2Loc_Vert(selbuf[ii++]);
		    tri.GeomInfoPi(j).trignum = paralleltop->Glob2Loc_SurfEl(selbuf[ii++]);
		  }

		if (invertsurf)
		  tri.Invert();
		
                paralleltop->AppendLoc2Glob_SurfEl ( globsel );
                AddSurfaceElement (tri);
		sel ++;
	      }
	    std::cout << "received surfaceelementsgi " << endl;
	  }

	if (strcmp (st.c_str(), "edgesegmentsgi") == 0)
	  {
	    double * segmbuf;
	    segmbuf = new double;
	    int bufsize;
	    MyMPI_Recv ( segmbuf, bufsize, 0);
	    Segment seg;
	    int hi;
	    int globsegi;
	    int ii = 0;
	    while ( ii < bufsize )
	      {
		globsegi = int (segmbuf[ii++]);
		seg.si = int (segmbuf[ii++]);
		seg.p1 = paralleltop -> Glob2Loc_Vert( int (segmbuf[ii++]));
		seg.p2 = paralleltop -> Glob2Loc_Vert( int (segmbuf[ii++]));
		seg.geominfo[0].trignum = int( segmbuf[ii++] );
		seg.geominfo[1].trignum = int ( segmbuf[ii++]);
		seg.surfnr1 = int ( segmbuf[ii++]);
		seg.surfnr2 = int ( segmbuf[ii++]);
		seg.edgenr = int ( segmbuf[ii++]);
		seg.epgeominfo[0].dist = segmbuf[ii++];
		seg.epgeominfo[1].edgenr = int (segmbuf[ii++]);
		seg.epgeominfo[1].dist = segmbuf[ii++];

		seg.singedge_left = segmbuf[ii++];
		seg.singedge_right = segmbuf[ii++];

		seg.epgeominfo[0].edgenr = seg.epgeominfo[1].edgenr;
		
		seg.domin = seg.surfnr1;
		seg.domout = seg.surfnr2;
		if ( seg.p1 >0 && seg.p2 > 0 )
		  {
		    paralleltop->AppendLoc2Glob_Segm ( globsegi );
		    
		    AddSegment (seg);
		  }

	      }
	    delete segmbuf;
	  }
      
  
	
	if (strcmp (st.c_str(), "endmesh") == 0)
	  {
	    endmesh = true;
	  }


      }


      CalcSurfacesOfNode ();

      std::cout << "calculated surfs" << endl;

      //  BuildConnectedNodes ();
     
      topology -> Update();
      clusters -> Update();
  
      SetNextMajorTimeStamp();
      //  PrintMemInfo (cout);

      //paralleltop->Print();
 

  }
  





  /*
  void Mesh :: FindExchangePoints ()
  {
    std::cout << "FIND EXCHANGE POINTS "  << endl;
 
    int id, rc, ntasks;
    MPI_Status status;
    int tag_list = 207, tag_len=208;
    int help[100];

    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    // find exchange vertices - first send exchangevert_globnum, 
    // receive distant globnums
    // find matching


    int ** distantlist, *ownlist, *sendlist;
    int numberexp = 0;

    BitArray isrefinedvertex ( GetNP() );
    isrefinedvertex.Set();
      
    ARRAY<int> nexp (ntasks);
    for ( int i = 1; i <= GetNP(); i++ )
      if ( paralleltop->IsExchangeVert(i) )
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
    for ( int i = 1; i <= GetNP(); i++ )
      if ( paralleltop->IsExchangeVert(i) )
        {
          ownlist[jj] = i;
          ownlist[jj+numberexp] = paralleltop->GetLoc2Glob_Vert(i);
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
    paralleltop -> SetExchange_VertSize ( ntasks, GetNV() );
    ARRAY<int> intarray (2);
    for ( int dest = 0; dest < ntasks; dest++)
      if ( dest != id )
	{
	  for ( int locpnum = 0; locpnum < nexp[id]; locpnum++)
	    for ( int distpnum=0; distpnum < nexp[dest]; distpnum++)
	      {
		if ( distantlist[dest][distpnum + nexp[dest]] == ownlist[locpnum +numberexp])
		  {
		    intarray[0] = dest;
		    intarray[1] = distantlist[dest][distpnum];
		    paralleltop->AppendExchange_Vert ( ownlist[locpnum], dest, distantlist[dest][distpnum] );
		    std::cout << "matching " << id << ": " << ownlist[locpnum] << ", " << dest << ": " <<
		      distantlist[dest][distpnum] << ",  global: " << ownlist[locpnum+numberexp] << endl;
		    ii++;
		  }
	      }
        }

    // and now the refined vertices...





  }






  void Mesh :: FindExchangeEdges ()
  {
    int id, rc, ntasks;
    MPI_Status status;
 
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    // find exchange edges - first send exchangeedges locnum, v1, v2
    // receive distant distnum, v1, v2
    // find matching


    int ** distantlist, **ownsendlist, ** ownlocallist;
    int numberexed = 0;
    int ned = topology -> GetNEdges ();

    // nsendexed[dest] ... number of edges sent to processor dest
    ARRAY<int> nsendexed (ntasks), nrecvexed(ntasks);
    for ( int i = 0; i < ntasks; i++)
      nsendexed[i] = 0;

    for ( int ed = 1; ed <= ned; ed++ )
      if ( paralleltop->IsExchangeEdge(ed) )
        {
	  int v1, v2;
	  topology -> GetEdgeVertices ( ed, v1, v2 );

	  for ( int dest = 0; dest < ntasks; dest++)
	    if ( paralleltop -> GetDistantPNum ( dest, v1 ) > 0 && paralleltop -> GetDistantPNum (dest, v2) > 0)
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
      if ( paralleltop->IsExchangeEdge(ed) )
        {
	  int v1, v2;
	  topology -> GetEdgeVertices ( ed, v1, v2 );

	  for ( int dest = 0; dest < ntasks; dest++)
	    if ( paralleltop -> GetDistantPNum ( dest, v1 ) > 0 && paralleltop -> GetDistantPNum (dest, v2) > 0)
	    {
	      ownsendlist[dest][jj[dest]] = ed;
	      ownlocallist[dest][jj[dest]++] = ed;
	      ownsendlist[dest][jj[dest]] = paralleltop->GetDistantPNum ( dest, v1 );
	      ownlocallist[dest][jj[dest]++] = v1;
	      ownsendlist[dest][jj[dest]] = paralleltop->GetDistantPNum ( dest, v2 );
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

    paralleltop -> SetExchange_EdgeSize ( ntasks, ned );

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
		    paralleltop->AppendExchange_Edge ( locedgenum, sender, distedgenum );
		    break;
		  }
	      }


	  }


    for ( int i = 0; i < ntasks; i++)
      {
	delete ownsendlist[i], ownlocallist[i], distantlist[i];
      }
    delete ownsendlist, ownlocallist, distantlist;
   }





  void Mesh :: FindExchangeFaces ()
  {
    int id, rc, ntasks;
    MPI_Status status;
 
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    // find exchange edges - first send exchangeedges locnum, v1, v2
    // receive distant distnum, v1, v2
    // find matching


    int ** distantlist, **ownsendlist, ** ownlocallist;
    int numberexfa = 0;
    int nfa = topology -> GetNFaces ();

    // nsendexfa[dest] ... number of faces sent to processor dest
    ARRAY<int> nsendexfa (ntasks), nrecvexfa(ntasks), sendbufsize(ntasks);
    for ( int i = 0; i < ntasks; i++)
      {
	nsendexfa[i] = 0;
	sendbufsize[i] = 0;
      }

    for ( int fa = 1; fa <= nfa; fa++ )
      if ( paralleltop->IsExchangeFace(fa) )
        {
	  ARRAY<int> facevert;
	  topology -> GetFaceVertices ( fa, facevert);

	
	  for ( int dest = 0; dest < ntasks; dest++)
	    {
	      bool exfa_dest = true;
	      for ( int j=0; j < facevert.Size(); j++)
		if ( paralleltop->GetDistantPNum ( dest, facevert[j] ) <= 0 )
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
      if ( paralleltop->IsExchangeFace(fa) )
        {
	  ARRAY<int> facevert;
	  topology -> GetFaceVertices ( fa, facevert);

	
	  for ( int dest = 0; dest < ntasks; dest++)
	    {
	      bool exfa_dest = true;
	      for ( int j=0; j < facevert.Size(); j++)
		if ( paralleltop->GetDistantPNum ( dest, facevert[j] ) <= 0 )
		  exfa_dest = 0;
	      if ( exfa_dest )
		{
		  ownsendlist[dest][jj[dest]] = fa;
		  ownlocallist[dest][jj[dest]++] = fa;
		  for ( int k=0; k<facevert.Size(); k++ )
		    {
		      ownsendlist[dest][jj[dest]] = paralleltop->GetDistantPNum ( dest, facevert[k]);
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

    paralleltop -> SetExchange_FaceSize ( ntasks, nfa );

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
		      paralleltop -> AppendExchange_Face ( locfacenum, sender, distfacenum );
		    
		    }
		}
	      
	     
	    }
	}
    for ( int i = 0; i < ntasks; i++)
      {
	delete ownsendlist[i], ownlocallist[i], distantlist[i];
      }
    delete ownsendlist, ownlocallist, distantlist;

  }
  
  */



#ifdef PARALLEL 

    // the new loading routine, should work as well
    // if called for only one processor
    void Mesh :: LoadAndDistribute (istream & infile)
    {
      // mpi information
      int id, rc, ntasks;
      MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
      MPI_Comm_rank(MPI_COMM_WORLD, &id);

      char str[100];
      int i, n;

      double scale = 1;  // globflags.GetNumFlag ("scale", 1);
      int inverttets = 0;  // globflags.GetDefineFlag ("inverttets");
      int invertsurf = 0;  // globflags.GetDefineFlag ("invertsurfacemesh");


      // mesh is mastermesh for only one processor, but for more processors mastermesh is deleted after 
      // loading
    

      Mesh *mastermesh;
      if ( ntasks > 1 )
	mastermesh = new Mesh;
      else
	mastermesh = this;
 


    facedecoding.SetSize(0);

    bool endmesh = false;

    while (infile.good() && !endmesh)
      {
	infile >> str;

	if (strcmp (str, "dimension") == 0)
	  {
	    infile >> dimension;
	    mastermesh -> SetDimension ( dimension );
	  }

	if (strcmp (str, "surfaceelements") == 0)
	  {
	    infile >> n;
	    PrintMessage (3, n, " surface elements");
	    for (i = 1; i <= n; i++)
	      {
		int j;
		int surfnr, bcp, domin, domout, nep, faceind = 0;

		infile >> surfnr >> bcp >> domin >> domout;
		surfnr--;

		for (j = 1; j <= mastermesh->facedecoding.Size(); j++)
		  if (mastermesh -> GetFaceDescriptor(j).SurfNr() == surfnr &&
		      mastermesh -> GetFaceDescriptor(j).BCProperty() == bcp &&
		      mastermesh -> GetFaceDescriptor(j).DomainIn() == domin &&
		      mastermesh -> GetFaceDescriptor(j).DomainOut() == domout)
		    faceind = j;

		if (!faceind)
		  {
		    faceind = mastermesh -> AddFaceDescriptor (FaceDescriptor(surfnr, domin, domout, 0));
		    mastermesh -> GetFaceDescriptor(faceind).SetBCProperty (bcp);
		  }

		infile >> nep;
		if (!nep) nep = 3;

		Element2d tri(nep);
		tri.SetIndex(faceind);

		for (j = 1; j <= nep; j++)
		  infile >> tri.PNum(j);

		if (invertsurf)
		  tri.Invert();

		mastermesh -> AddSurfaceElement (tri);
	      }
	  }
      
	if (strcmp (str, "surfaceelementsgi") == 0)
	  {
	    infile >> n;
	    PrintMessage (3, n, " surface elements");
	    for (i = 1; i <= n; i++)
	      {
		int j;
		int surfnr, bcp, domin, domout, nep, faceind = 0;
		infile >> surfnr >> bcp >> domin >> domout;
		surfnr--;

		for (j = 1; j <= mastermesh -> facedecoding.Size(); j++)
		  if (mastermesh -> GetFaceDescriptor(j).SurfNr() == surfnr &&
		      mastermesh -> GetFaceDescriptor(j).BCProperty() == bcp &&
		      mastermesh -> GetFaceDescriptor(j).DomainIn() == domin &&
		      mastermesh -> GetFaceDescriptor(j).DomainOut() == domout)
		    faceind = j;

		if (!faceind)
		  {
		    faceind = mastermesh -> AddFaceDescriptor (FaceDescriptor(surfnr, domin, domout, 0));
		    mastermesh -> GetFaceDescriptor(faceind).SetBCProperty (bcp);
		  }

		infile >> nep;
		if (!nep) nep = 3;

		Element2d tri(nep);
		tri.SetIndex(faceind);

		for (j = 1; j <= nep; j++)
		  infile >> tri.PNum(j);

		for (j = 1; j <= nep; j++)
		  infile >> tri.GeomInfoPi(j).trignum;

		if (invertsurf)
		  tri.Invert();

		mastermesh -> AddSurfaceElement (tri);
	      }
	  }

	if (strcmp (str, "volumeelements") == 0)
	  {
	    infile >> n;
	    PrintMessage (3, n, " volume elements");
	    for (i = 1; i <= n; i++)
	      {
		Element el;
		int hi, nep;
		infile >> hi;
		if (hi == 0) hi = 1;
		el.SetIndex(hi);
		infile >> nep;
		el.SetNP(nep);
	      
		for (int j = 0; j < nep; j++)
		  infile >> (int&)(el[j]);
	      
		if (inverttets)
		  el.Invert();

		mastermesh -> AddVolumeElement (el);
	      }
	  }
    

	if (strcmp (str, "edgesegments") == 0)
	  {
	    infile >> n;
	    for (i = 1; i <= n; i++)
	      {
		Segment seg;
		int hi;
		infile >> seg.si >> hi >> seg.p1 >> seg.p2;
		mastermesh -> AddSegment (seg);
	      }
	  }
      

	if (strcmp (str, "edgesegmentsgi") == 0)
	  {
	    infile >> n;
	    for (i = 1; i <= n; i++)
	      {
		Segment seg;
		int hi;
		infile >> seg.si >> hi >> seg.p1 >> seg.p2
		       >> seg.geominfo[0].trignum
		       >> seg.geominfo[1].trignum;
		mastermesh -> AddSegment (seg);
	      }
	  }
	
	if (strcmp (str, "edgesegmentsgi2") == 0)
	  {
	    int a; 
	    infile >> a;
	    n=a; 

	    PrintMessage (3, n, " curve elements");

	    for (i = 1; i <= n; i++)
	      {
		Segment seg;
		int hi;
		infile >> seg.si >> hi >> seg.p1 >> seg.p2
		       >> seg.geominfo[0].trignum
		       >> seg.geominfo[1].trignum
		       >> seg.surfnr1 >> seg.surfnr2
		       >> seg.edgenr
		       >> seg.epgeominfo[0].dist
		       >> seg.epgeominfo[1].edgenr
		       >> seg.epgeominfo[1].dist;

		seg.epgeominfo[0].edgenr = seg.epgeominfo[1].edgenr;

		seg.domin = seg.surfnr1;
		seg.domout = seg.surfnr2;

		seg.surfnr1--;
		seg.surfnr2--;
	      
		mastermesh -> AddSegment (seg);
	      }
	  }
      
	if (strcmp (str, "points") == 0)
	  {
	    infile >> n;
	    PrintMessage (3, n, " points");
	    for (i = 1; i <= n; i++)
	      {
		Point3d p;
		infile >> p.X() >> p.Y() >> p.Z();
		p.X() *= scale;
		p.Y() *= scale;
		p.Z() *= scale;
		mastermesh -> AddPoint (p);
	      }
	  }

	if (strcmp (str, "identifications") == 0)
	  {
	    infile >> n;
	    for (i = 1; i <= n; i++)
	      {
		PointIndex pi1, pi2;
		int ind;
		infile >> pi1 >> pi2 >> ind;
                mastermesh -> ident -> Add (pi1, pi2, ind);
                
	      }
	  }
	if (strcmp (str, "materials") == 0)
	  {
	    infile >> n;
	    for (i = 1; i <= n; i++)
	      {
		int nr;
		string mat;
		infile >> nr >> mat;
		mastermesh -> SetMaterial (nr, mat.c_str());
	      }
	  }
	
	if (strcmp (str, "singular_points") == 0)
	  {
	    infile >> n;
	    for (i = 1; i <= n; i++)
	      {
		PointIndex pi;
		double s; 
		infile >> pi;
		infile >> s; 
		(*mastermesh)[pi].Singularity (s);
	      }
	  }

	if (strcmp (str, "singular_edge_left") == 0)
	  {
	    infile >> n;
	    for (i = 1; i <= n; i++)
	      {
		SegmentIndex si;
		double s; 
		infile >> si;
		infile >> s; 
		(*mastermesh)[si].singedge_left = s;
	      }
	  }
	if (strcmp (str, "singular_edge_right") == 0)
	  {
	    infile >> n;
	    for (i = 1; i <= n; i++)
	      {
		SegmentIndex si;
		double s; 
		infile >> si;
		infile >> s; 
		(*mastermesh)[si].singedge_right = s;
	      }
	  }

	if (strcmp (str, "singular_face_inside") == 0)
	  {
	    infile >> n;
	    for (i = 1; i <= n; i++)
	      {
		SurfaceElementIndex sei;
		double s; 
		infile >> sei;
		infile >> s; 
		mastermesh -> GetFaceDescriptor((*mastermesh)[sei].GetIndex()).domin_singular = s;
	      }
	  }

	if (strcmp (str, "singular_face_outside") == 0)
	  {
	    infile >> n;
	    for (i = 1; i <= n; i++)
	      {
		SurfaceElementIndex sei;
		double s; 
		infile >> sei;
		infile >> s; 
		mastermesh -> GetFaceDescriptor((*mastermesh)[sei].GetIndex()).domout_singular = s;
	      }
	  }

	if (strcmp (str, "endmesh") == 0)
	  endmesh = true;
	  


	strcpy (str, "");
      }
  
    mastermesh -> CalcSurfacesOfNode ();
    mastermesh -> UpdateTopology();
  
    mastermesh -> SetNextMajorTimeStamp();

#ifdef PARALLEL
    // metis partition of mesh, only if more than one proc

    if ( ntasks > 1 )
      {	
	// array contains local numbers of elements in meshes
	ARRAY<int> ne_local (ntasks);
        ne_local = 0;

	// partition
	ParallelMetis ( mastermesh, ne_local );
	// send partition
	if ( ntasks > 1 )
	SendMesh ( mastermesh, ne_local, scale, inverttets, invertsurf );
      }
#endif
    
    CalcSurfacesOfNode ();
    
    //  BuildConnectedNodes ();
    
    // here also parallel information is computed
    topology -> Update();
    clusters -> Update();
      
    
#ifdef PARALLEL  
    if ( ntasks > 1 )
      {    
	/*
	PrintMessage( 1, "Computing exchange data ..." );
	
	for ( int dest = 1; dest < ntasks; dest++ )
	  {
	    MyMPI_Send ( "findexchangepoints", dest );  
	  }
	
	// find mapping from local to distant vertex numbers...
	FindExchangePoints ();
	
	PrintMessage ( 3, "Computed exchange vertices");
	
	for ( int dest = 1; dest < ntasks; dest++)
	  MyMPI_Send ( "findexchangeedges", dest);
	
	FindExchangeEdges ();
	
	PrintMessage (3, "Computed exchange edges");
	
	for ( int dest = 1; dest < ntasks; dest++)
	  MyMPI_Send ( "findexchangefaces", dest);
	
	FindExchangeFaces ();
	
	PrintMessage ( 3, "Computed exchange faces");

	PrintMessage ( 1, "Exchange data computed" );
	*/
	SetNextMajorTimeStamp();
	//  PrintMemInfo (cout);
	
	paralleltop -> Print();
	delete mastermesh;
      }
#endif
    
  }
  




#endif








  void Mesh :: ParallelMetis (  Mesh * mastermesh, ARRAY<int> & neloc )
  {
    
    int id, rc, ntasks;
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

      
    int ne = mastermesh -> GetNE();
    
    int nn = mastermesh -> GetNP();
    
    ELEMENT_TYPE elementtype = mastermesh -> VolumeElement(1).GetType();
    int npe = mastermesh -> VolumeElement(1).GetNP();
    
    idxtype *elmnts;
    elmnts = new idxtype[ne*npe];
    
    int etype;
    if (elementtype == TET)
      etype = 2;
    else if (elementtype == HEX)
      etype = 3;
    
    
    for (int i=1; i<=ne; i++)
      for (int j=1; j<=npe; j++)
	elmnts[(i-1)*npe+(j-1)] = mastermesh -> VolumeElement(i).PNum(j)-1;
    
    int numflag = 0;
    int nparts = ntasks;
    
    int edgecut;
    idxtype *epart, *npart;
    epart = new idxtype[ne];
    npart = new idxtype[nn];
    
    
    if ( ntasks == 1 ) 
      {
	(*this) = *mastermesh;
	nparts = 4;	   
	metis :: METIS_PartMeshDual (&ne, &nn, elmnts, &etype, &numflag, &nparts,
				     &edgecut, epart, npart);
	cout << "done" << endl;
	
	cout << "edge-cut: " << edgecut << ", balance: " << metis :: ComputeElementBalance(ne, nparts, epart) << endl;
	
	for (int i=1; i<=ne; i++)
	  {
	    mastermesh->VolumeElement(i).SetPartition(epart[i-1]);
	  }
	
	return;
      }
    
#ifdef METIS
    
    metis :: METIS_PartMeshDual (&ne, &nn, elmnts, &etype, &numflag, &nparts,
				 &edgecut, epart, npart);
    cout << "done" << endl;
    
    cout << "edge-cut: " << edgecut << ", balance: " << metis :: ComputeElementBalance(ne, nparts, epart) << endl;
    
    for (int i=1; i<=ne; i++)
      mastermesh -> VolumeElement(i).SetPartition(epart[i-1]);
    
    for ( int i=1; i<=ne; i++)
      {
	neloc[epart[i-1]] ++;
      }
#endif
    
  }



  void Mesh :: SendMesh ( Mesh * mastermesh, ARRAY<int> & neloc, 
			  const double & scale, const int & inverttets, const int & invertsurf )
  {
    PrintMessage ( 1, "Sending Mesh to local processors" );
    int id, rc, ntasks;
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    char  message[100];
    const int tag = 50;
    MPI_Status status;
	
    int tag_dim = 10, tag_token = 100, tag_n = 11, tag_point=13, tag_pnum=12;
    int tag_index = 101, tag_facedescr = 102;
    int intbuf[100];
    double doublebuf[100];

    int nv=0,  ned=0, nfa=0;

    int nelglob, nelloc, nvglob, nedglob, nfaglob;

    int np = 0;

    BitArray sentvert ( ntasks * mastermesh->GetNP() );
    sentvert.Clear();

    strcpy ( message, "mesh");
    for ( int dest = 1; dest < ntasks; dest++ )
      MyMPI_Send ("mesh", dest);
    //MPI_Send( message, strlen(message)+1 , MPI_CHAR, dest, tag, MPI_COMM_WORLD);


    nelglob = mastermesh -> GetNE();
    nelloc = neloc[0];
    nvglob = mastermesh -> GetNP();
    nedglob = mastermesh -> GetTopology().GetNEdges();
    nfaglob = mastermesh -> GetTopology().GetNFaces();
    dimension = mastermesh -> GetDimension();

    for ( int dest = 1; dest < ntasks; dest++ )
      {
        MyMPI_Send (nelglob, dest);
	MyMPI_Send (neloc[dest], dest);
	MyMPI_Send ( nvglob, dest);
	MyMPI_Send ( nedglob, dest );
	MyMPI_Send ( nfaglob, dest);
	MyMPI_Send( mastermesh->GetDimension(), dest );
      }



    double ** pointarray;
    pointarray = new double* [ntasks];

    // get number of vertices in each mesh
    ARRAY<int> nvi ( ntasks ), vi (ntasks), elarraysize(ntasks);
    for ( int i = 0; i < ntasks; i++)
      {
	nvi[i] = 0; vi[i] = 0; elarraysize[i] = 0;
      }

    int * epi;
    epi = new int[12];

    for ( int ei = 1; ei <= mastermesh->GetNE(); ei++)
      {
	const Element & el = mastermesh -> VolumeElement (ei);
	int dest = el.GetPartition();
	if (dest <= 0) continue;
	for (int i = 0; i < el.GetNP(); i++)
	  {
            epi[i] = el.PNum(i+1);
	  
	    if ( ! sentvert.Test(ntasks * (epi[i]-1) + dest) )
	      {
		sentvert.Set ( ntasks * (epi[i]-1) + dest );
		
		nvi[dest] ++;
	      }
	  }
	elarraysize[dest] += 3 + el.GetNP();

      }

    for ( int i = 1; i < ntasks; i++ )
//       pointarray[i] = new double[ 4*nvi[i] ];
      pointarray[i] = new double [ 5*nvi[i] ];

    for ( int dest = 1; dest < ntasks; dest ++ )
      {	    
	MyMPI_Send ( "vertex", dest );
	int num = nvi[dest];
      }


    sentvert.Clear();
    for ( int ei = 1; ei <= mastermesh->GetNE(); ei++)
      {

        const Element & el = mastermesh -> VolumeElement (ei);
	int dest = el.GetPartition();
	


	if ( dest > 0 )
	  {
	   
	    // send points
	    
	    for (int i = 0; i < el.GetNP(); i++)
	      {
		epi[i] = el.PNum(i+1);
		int pi = epi[i];
		double sing; 
		sing = (*mastermesh)[PointIndex(pi)].Singularity ();
		
		if ( ! sentvert.Test(ntasks * (epi[i]-1) + dest) )
		  {
		    sentvert.Set ( ntasks * (epi[i]-1) + dest );
		    
 		    pointarray[dest] [ vi[dest] * 5 ] = epi[i] * 1.0;
		    
		    const Point3d & hp = mastermesh -> Point (epi[i]);
		    
		    pointarray[dest][ vi[dest]*5+1 ] = hp.X();
		    pointarray[dest][ vi[dest]*5+2 ] = hp.Y();
		    pointarray[dest][ vi[dest]*5+3 ] = hp.Z();
		    pointarray[dest][ vi[dest]*5+4 ] = sing;
		    vi[dest] ++;
		  }
	      }

	  }
	else // dest == 0 ist hier
	  {
 	    for (int i = 0; i < el.GetNP(); i++)
	      {
		epi[i] = el.PNum(i+1);
		int pi = epi[i];
		double sing; 
		sing = (*mastermesh)[PointIndex(pi)].Singularity ();

		if ( ! sentvert.Test(ntasks * (epi[i]-1) + dest) )
		  {
		    sentvert.Set ( ntasks * (epi[i]-1) + dest );
		    np ++;
		    paralleltop->AppendLoc2Glob_Vert ( epi[i] );
		    // std::cout << np << "   " << paralleltop->GetLoc2Glob_Vert (np-1) << endl;
		    //		    cout << "global: " << epi[i] << "  local " << np << endl;
		    const Point3d & hp = mastermesh -> Point (epi[i]);
		    Point3d p;
		    p.X() = scale * hp.X();
		    p.Y() = scale * hp.Y();
		    p.Z() = scale * hp.Z();
		    AddPoint(p);
		    (*this)[PointIndex(np)].Singularity( sing );
		  }
	      }


	  }


      }

    PrintMessage ( 3, "sending vertices");
     for ( int dest = 1; dest < ntasks; dest ++ )
      MyMPI_Send ( pointarray[dest], 5*nvi[dest], dest );
    PrintMessage ( 3, "vertices sent");

    for ( int dest = 1; dest < ntasks; dest ++ )
      {
	delete pointarray[dest];
	pointarray[dest] = new double [elarraysize[dest] ] ;

	MyMPI_Send ( "volumeelements", dest);
// 	MyMPI_Send( elarraysize[dest] , dest);
      }

    delete epi;

    ARRAY<int> eli (ntasks);
    for ( int i = 0; i < ntasks; i++ )
      eli[i] = 0;
    for ( int ei = 1; ei <= mastermesh->GetNE(); ei++)
      {

	int * epi;
	const Element & el = mastermesh -> VolumeElement (ei);
	int dest = el.GetPartition();
	epi = new int[el.GetNP()];


	if ( dest > 0 )
	  {
	    
            // send volume element

  	    pointarray[dest][ eli[dest] ++ ]  = ei;
 	    pointarray[dest][ eli[dest] ++] = el.GetIndex();
 	    pointarray[dest][ eli[dest] ++] = el.GetNP();
 	    for ( int ii=0; ii<el.GetNP(); ii++)
 	      pointarray[dest][ eli[dest] ++] = (int&) el[ii];

	  }
	else // dest == 0 ist hier
	  {
 
            // send volume element
	    paralleltop->AppendLoc2Glob_VolEl ( ei );
	    Element elnew;
	    elnew.SetIndex (el.GetIndex() );
	    elnew.SetNP ( el.GetNP() );

	    for ( int ii=0; ii<el.GetNP(); ii++)
	      {
		(int&) elnew[ii] = paralleltop->Glob2Loc_Vert(el.PNum(ii+1));
	      }
	    if (inverttets)
	      elnew.Invert();
	    
	    AddVolumeElement (elnew);

	  }
      }

    PrintMessage ( 3, "sending elements" );
    for ( int dest = 1; dest < ntasks; dest ++ )
      {
// 	MPI_Send ( pointarray[dest], elarraysize[dest], MPI_DOUBLE, dest, tag_pnum, MPI_COMM_WORLD);
	MyMPI_Send ( pointarray[dest], elarraysize[dest], dest );
	delete pointarray[dest];
      }
    delete pointarray;
    PrintMessage ( 3, "elements sent" );
  
    for ( int fdi = 1; fdi <= mastermesh -> GetNFD(); fdi ++ )
      {
	double * doublebuf = new double[6];
	doublebuf[0] = mastermesh -> GetFaceDescriptor(fdi).SurfNr();
	doublebuf[1] = mastermesh -> GetFaceDescriptor(fdi).DomainIn();	
	doublebuf[2] = mastermesh -> GetFaceDescriptor(fdi).DomainOut();
	doublebuf[3] = mastermesh -> GetFaceDescriptor(fdi).BCProperty();
	int faceind = AddFaceDescriptor (FaceDescriptor(int(doublebuf[0]), int(doublebuf[1]), int(doublebuf[2]), 0));
	GetFaceDescriptor(faceind).SetBCProperty (int(doublebuf[3]));
	GetFaceDescriptor(faceind).domin_singular = mastermesh -> GetFaceDescriptor(fdi).domin_singular;
	GetFaceDescriptor(faceind).domout_singular = mastermesh -> GetFaceDescriptor(fdi).domout_singular;
	delete doublebuf;
      }

    for ( int dest = 1; dest < ntasks; dest++)
      for ( int fdi = 1; fdi <= mastermesh->GetNFD(); fdi++)
        {
	  MyMPI_Send("facedescriptor", dest);
	double * doublebuf = new double[6];

          doublebuf[0] = GetFaceDescriptor(fdi).SurfNr();
          doublebuf[1] = GetFaceDescriptor(fdi).DomainIn();	
          doublebuf[2] = GetFaceDescriptor(fdi).DomainOut();
          doublebuf[3] = GetFaceDescriptor(fdi).BCProperty();
	  doublebuf[4] = GetFaceDescriptor(fdi).domin_singular;
	  doublebuf[5] = GetFaceDescriptor(fdi).domout_singular;
//           MPI_Send( doublebuf, 6, MPI_INT, dest, tag_facedescr, MPI_COMM_WORLD);
	  MyMPI_Send ( doublebuf, 6, dest);
	  delete doublebuf;
        }

    PrintMessage ( 3, "fdi sent" );
    

    // hasglobalsurf( edgenr * ntasks + dest ) .... global edge edgenr is in mesh at dest
//     BitArray hasglobaledge;
    
//     hasglobaledge.SetSize(ntasks * nedglob);
//     hasglobaledge.Clear();
//     cout << "mmf " << mastermesh->GetTopology().GetNFaces() << endl;
    // determine sizes of local surface element arrays

    ARRAY <int> nlocsel(ntasks), bufsize ( ntasks), seli(ntasks);
    for ( int i = 0; i < ntasks; i++)
      {
	nlocsel[i] = 0;
	bufsize[i] = 0;
	seli[i] = 0;
      }

    for ( int sei = 1; sei <= mastermesh -> GetNSE(); sei ++ )
      {
	int ei1, ei2;
	mastermesh -> GetTopology().GetSurface2VolumeElement (sei, ei1, ei2);
	const Element2d & sel = mastermesh -> SurfaceElement (sei);
	int dest;
	// first element

        for (int j = 0; j < 2; j++)
          {
            int ei = (j == 0) ? ei1 : ei2;
            
            if ( ei > 0 && ei <= mastermesh->GetNE() )
              {
                const Element & el = mastermesh -> VolumeElement (ei);
                dest = el.GetPartition();
		nlocsel[dest] ++;
		bufsize[dest] += 3 + 2*sel.GetNP();
              }
	  }

      }

    int ** selbuf;
    selbuf = new int*[ntasks];
    for ( int i = 0; i < ntasks; i++)
      selbuf[i] = new int[bufsize[i]];


    for ( int dest = 1; dest < ntasks; dest++ )
      MyMPI_Send ( "surfaceelementsgi", dest);                    
		    
    for ( int sei = 1; sei <= mastermesh -> GetNSE(); sei ++ )
      {
	int ei1, ei2;
	mastermesh -> GetTopology().GetSurface2VolumeElement (sei, ei1, ei2);
	const Element2d & sel = mastermesh -> SurfaceElement (sei);
	int dest;



        for (int j = 0; j < 2; j++)
          {
            int ei = (j == 0) ? ei1 : ei2;
            
            if ( ei > 0 && ei <= mastermesh->GetNE() )
              {
                const Element & el = mastermesh -> VolumeElement (ei);
                dest = el.GetPartition();
                if (dest > 0)
                  {
                    // send:
                    // sei
                    // faceind
                    // nep
                    // tri.pnums, tri.geominfopi.trignums

		    selbuf[dest][seli[dest]++] = sei;
		    selbuf[dest][seli[dest]++] = sel.GetIndex();
		    selbuf[dest][seli[dest]++] = sel.GetNP();

                    for ( int ii = 1; ii <= sel.GetNP(); ii++)
                      {
                        selbuf[dest][seli[dest]++] = sel.PNum(ii);
                        selbuf[dest][seli[dest]++] = sel.GeomInfoPi(ii).trignum;
                      }
                  }
		
                else
                  {
                    Element2d trinew( sel.GetNP() );
                    trinew.SetIndex ( sel.GetIndex() );

                    for( int j=1; j<=sel.GetNP(); j++)
                      trinew.PNum(j) = paralleltop->Glob2Loc_Vert( sel.PNum(j) );
	
                    if (invertsurf)         // 
                      trinew.Invert();
		
                    paralleltop->AppendLoc2Glob_SurfEl ( sei );
                    AddSurfaceElement (trinew);
		    }
              }


           

	     
	  }

      }

    PrintMessage ( 3, "sending surfels" );

    for ( int dest = 1; dest < ntasks; dest++)
      {
	MyMPI_Send( selbuf[dest], bufsize[dest], dest);
      }
    
    for ( int dest = 0; dest < ntasks; dest++ )
      {
	delete selbuf[dest];
      }
    delete selbuf;
    
    PrintMessage ( 3, "surfels sent");
    


   for ( int dest = 1; dest < ntasks; dest++ )
      MyMPI_Send ( "edgesegmentsgi", dest);                    


    ARRAY <int> nlocseg(ntasks), segi(ntasks);
    for ( int i = 0; i < ntasks; i++)
      {
	nlocseg[i] = 0;
	bufsize[i] = 0;
	segi[i] = 0;
      }

    for ( int segi = 1; segi <= mastermesh -> GetNSeg(); segi ++ )
      {
	ARRAY<int> volels;
	mastermesh -> GetTopology().GetSegmentVolumeElements ( segi, volels );
	const Segment & segm = mastermesh -> LineSegment (segi);
	int dest;

        for (int j = 0; j < volels.Size(); j++)
          {
            int ei = volels[j];
            int dest;
            if ( ei > 0 && ei <= mastermesh->GetNE() )
              {
                const Element & el = mastermesh -> VolumeElement (ei);
                dest = el.GetPartition();
		nlocseg[dest] ++;
		bufsize[dest] += 14;
              }
	  }

      }

 
    double ** segmbuf;
    segmbuf = new double*[ntasks];
    for ( int i = 0; i < ntasks; i++)
      segmbuf[i] = new double[bufsize[i]];

    
    //     cout << "mastermesh " << mastermesh -> GetNSeg() << "    lineseg " << mastermesh -> LineSegment (1) << endl;
    for ( int ls=1; ls <= mastermesh -> GetNSeg(); ls++)
      {
	ARRAY<int> volels;
	mastermesh -> GetTopology().GetSegmentVolumeElements ( ls, volels );
	const Segment & seg = mastermesh -> LineSegment (ls);
	int dest;

        for (int j = 0; j < volels.Size(); j++)
          {
            int ei = volels[j];
            int dest;
            if ( ei > 0 && ei <= mastermesh->GetNE() )
              {
                const Element & el = mastermesh -> VolumeElement (ei);
                dest = el.GetPartition();


		if ( dest > 0 )
		  {
		    segmbuf[dest][segi[dest]++] = ls;
		    segmbuf[dest][segi[dest]++] = seg.si;
		    segmbuf[dest][segi[dest]++] = seg.p1;
		    segmbuf[dest][segi[dest]++] = seg.p2;
		    segmbuf[dest][segi[dest]++] = seg.geominfo[0].trignum;
		    segmbuf[dest][segi[dest]++] = seg.geominfo[1].trignum;
		    segmbuf[dest][segi[dest]++] = seg.surfnr1;
		    segmbuf[dest][segi[dest]++] = seg.surfnr2;
		    segmbuf[dest][segi[dest]++] = seg.edgenr;
		    segmbuf[dest][segi[dest]++] = seg.epgeominfo[0].dist;
		    segmbuf[dest][segi[dest]++] = seg.epgeominfo[1].edgenr;
		    segmbuf[dest][segi[dest]++] = seg.epgeominfo[1].dist;
		    segmbuf[dest][segi[dest]++] = seg.singedge_right;
		    segmbuf[dest][segi[dest]++] = seg.singedge_left;
		  }
		else
		  {
		    const Segment & seg = mastermesh -> LineSegment (ls);
		    
		    Segment segnew ;
		    segnew.si = seg.si;
		    segnew.p1 = paralleltop->Glob2Loc_Vert ( seg.p1 );
		    segnew.p2 = paralleltop->Glob2Loc_Vert ( seg.p2 );
		    segnew.surfnr1 = seg.surfnr1;
		    segnew.surfnr2 = seg.surfnr2;
		    segnew.edgenr = seg.edgenr;
		    segnew.epgeominfo[0].edgenr = segnew.epgeominfo[1].edgenr;
		    
		    segnew.domin = segnew.surfnr1;
		    segnew.domout = segnew.surfnr2;
		    segnew.singedge_left = seg.singedge_left;
		    segnew.singedge_right = seg.singedge_right;

		    if ( segnew.p1 >= 0 && segnew.p2 >= 0 )
		      {
			paralleltop->AppendLoc2Glob_Segm ( ls );
	    
			AddSegment (segnew);
		      }
		  }
	      }
	  }
      }

    PrintMessage ( 3, "sending segments" );

    for ( int dest = 1; dest < ntasks; dest++)
      {
	MyMPI_Send( segmbuf[dest], bufsize[dest], dest);
      }
    
    for ( int dest = 0; dest < ntasks; dest++ )
      {
	delete segmbuf[dest];
      }
    delete segmbuf;
    
    PrintMessage ( 3, "segments sent");



    for ( int dest = 1; dest < ntasks; dest++ )
      {
	MyMPI_Send("endmesh", dest);
      }
    PrintMessage ( 1, "Sending mesh to local processors completed" ); 

  }
}



#endif
