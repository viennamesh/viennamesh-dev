#ifdef PARALLEL

#include "parallel.hpp"
using namespace metis;

namespace netgen
{

  // load mesh sent by master process
void Mesh :: ReceiveParallelMesh ( )
  {
    int timer = NgProfiler::CreateTimer ("ReceiveParallelMesh");
    NgProfiler::RegionTimer reg(timer);


#ifdef SCALASCA
#pragma pomp inst begin(loadmesh)
#endif

    // PrintMessage (1,  "LOAD PARALLEL MESH");
    MPI_Barrier (MPI_COMM_WORLD);

    
    // char str[100];
    // int intbuf[100];
    double doublebuf[100];
    int i, n;
    int invertsurf = 0;  // globflags.GetDefineFlag ("invertsurfacemesh");
    int tag_dim = 10, tag_token = 100, tag_n=11, tag_pnum=12, tag_point=13;
    int tag_index = 101, tag_facedescr = 102;
    MPI_Status status;
    
    bool endmesh = false;

    int nv=0, ne=0, ned=0, nfa=0;

    int nelglob, nelloc, nvglob, nedglob, nfaglob;

    int np = 0;

    string st;

    int dim;

    // receive global valuesyes
    
    MyMPI_Recv ( nelglob, 0 );
    MyMPI_Recv ( nelloc, 0 );
    MyMPI_Recv ( nvglob, 0);
    MyMPI_Recv ( nedglob, 0 );
    MyMPI_Recv ( nfaglob, 0 );
    MyMPI_Recv ( dimension, 0);

    paralleltop -> SetNVGlob ( nvglob );
    paralleltop -> SetNEGlob ( nelglob );

    //    ARRAY<int,1> glob2loc_vert(nvglob);
    // glob2loc_vert = -1;

    INDEX_CLOSED_HASHTABLE<int> glob2loc_vert_ht (1);

    int ve = 0;
    while (!endmesh)
      {
	MyMPI_Recv ( st, 0 );

	// receive vertices
        if (st == "vertex")
 	  {
	    double  endtime, starttime; 
	    starttime = clock();

            ARRAY<double> pointarray;
            MyMPI_Recv ( pointarray, 0 );

	    int numvert = pointarray.Size() / 5;
	    paralleltop -> SetNV (numvert);

            glob2loc_vert_ht.SetSize (2*numvert+1);

	    for ( int vert=0; vert<numvert; vert++ )
	      {
		int globvert = int (pointarray[ vert*5 ]);
		paralleltop->SetLoc2Glob_Vert ( vert+1, globvert  );
		// glob2loc_vert[globvert] = vert+1;
                glob2loc_vert_ht.Set (globvert, vert+1);

		Point3d p;
		p.X() = pointarray[vert*5+1];
		p.Y() = pointarray[vert*5+2];
		p.Z() = pointarray[vert*5+3];
		AddPoint (p);
		(*this)[PointIndex(vert+1)] .Singularity ( pointarray[vert*5+4] );
	      }

	    ARRAY<int> dist_pnums;
	    MyMPI_Recv ( dist_pnums, 0);

	    for (int hi = 0; hi < dist_pnums.Size(); hi += 3)
	      {
		paralleltop ->
		  SetDistantPNum ( dist_pnums[hi+1], dist_pnums[hi], dist_pnums[hi+2]);
	      }

	    endtime = clock();
	    (*testout) << "Receiving Time verts = " 
		       << double(endtime - starttime)/CLOCKS_PER_SEC << endl;

 	  }

	if ( strcmp (st.c_str(), "volumeelements" ) == 0 )
	  {
	    *testout << "receiving elements" << endl;
	    int endtime, starttime; 
	    starttime = clock();

	    Element el;

            ARRAY<int> elarray;
            MyMPI_Recv ( elarray, 0);

	    int ind = 0;
	    int elnum = 1;
	    int nelloc = elarray[ind++];

	    paralleltop -> SetNE (nelloc);

	    while ( ind < elarray.Size() )
	      {
		paralleltop->SetLoc2Glob_VolEl ( elnum,  elarray[ind++]);

		el.SetIndex(elarray[ind++]);
		el.SetNP(elarray[ind++]);

		for ( int j = 0; j < el.GetNP(); j++)
                  el[j] = glob2loc_vert_ht.Get (elarray[ind++]); 
                  // el[j] = glob2loc_vert[elarray[ind++]]; 
		
		AddVolumeElement (el);
		elnum++;
	      }

	    endtime = clock();
	    (*testout) << "Receiving Time volels = " << double(endtime - starttime)/CLOCKS_PER_SEC << endl;
	  }

	if (strcmp (st.c_str(), "facedescriptor") == 0)
	  {
 	    int endtime, starttime; 
	    starttime = clock();
   
	    //	    MPI_Recv( ibuf, 6, MPI_INT, 0, tag_facedescr, MPI_COMM_WORLD, &status);
	    int nn;
	    double * doublebuf = 0;
	    MyMPI_Recv( doublebuf, nn, 0 );
	    int faceind = AddFaceDescriptor (FaceDescriptor(int(doublebuf[0]), int(doublebuf[1]), int(doublebuf[2]), 0));
	    GetFaceDescriptor(faceind).SetBCProperty (int(doublebuf[3]));
	    GetFaceDescriptor(faceind).domin_singular = doublebuf[4];
	    GetFaceDescriptor(faceind).domout_singular = doublebuf[5];
	    delete [] doublebuf;
	    endtime = clock();
	    (*testout) << "Receiving Time fde = " << double(endtime - starttime)/CLOCKS_PER_SEC << endl;
	  }
       


	if (strcmp (st.c_str(), "surfaceelementsgi") == 0)
	  {
	    int endtime, starttime; 
	    starttime = clock();

	    int j;
	    int surfnr, bcp, domin, domout, nep, faceind = 0;
	    int globsel;
	    int * selbuf;
	    selbuf = 0;
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

	    nlocsel = selbuf[ii++];
	    paralleltop -> SetNSE ( nlocsel );

	    while ( ii < bufsize-1 )
	      {
		globsel = selbuf[ii++];
		faceind = selbuf[ii++];
		nep = selbuf[ii++];
		Element2d tri(nep);
		tri.SetIndex(faceind);
		for( j=1; j<=nep; j++)
		  {
                    tri.PNum(j) = glob2loc_vert_ht.Get (selbuf[ii++]);
		    //tri.PNum(j) = glob2loc_vert[selbuf[ii++]]; //paralleltop->Glob2Loc_Vert(selbuf[ii++]);
		    tri.GeomInfoPi(j).trignum = paralleltop->Glob2Loc_SurfEl(selbuf[ii++]);
		  }

		if (invertsurf)
		  tri.Invert();
		
                paralleltop->SetLoc2Glob_SurfEl ( sel+1, globsel );
                AddSurfaceElement (tri);
		sel ++;
	      }
	    endtime = clock();
	    (*testout) << "Receiving Time sel = " << double(endtime - starttime)/CLOCKS_PER_SEC << endl;

	    delete [] selbuf ;
	  }

	if (strcmp (st.c_str(), "edgesegmentsgi") == 0)
	  {
	    int endtime, starttime; 
	    starttime = clock();

	    double * segmbuf = 0;
	    int bufsize;
	    MyMPI_Recv ( segmbuf, bufsize, 0);
	    Segment seg;
	    int hi;
	    int globsegi;
	    int ii = 0;
	    int segi = 1;
	    int nsegloc = int ( bufsize / 14 ) ;
	    paralleltop -> SetNSegm ( nsegloc );
	    while ( ii < bufsize )
	      {
		globsegi = int (segmbuf[ii++]);
		seg.si = int (segmbuf[ii++]);

                seg.p1 = glob2loc_vert_ht.Get (int(segmbuf[ii++]));
                seg.p2 = glob2loc_vert_ht.Get (int(segmbuf[ii++]));

		// seg.p1 = glob2loc_vert[int(segmbuf[ii++])];
		// seg.p2 = glob2loc_vert[int(segmbuf[ii++])];
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
		    paralleltop-> SetLoc2Glob_Segm ( segi,  globsegi );
		    
		    AddSegment (seg);
		    segi++;
		  }

	      }
	    delete [] segmbuf;
	    endtime = clock();
	    (*testout) << "Receiving Time fde = " << double(endtime - starttime)/CLOCKS_PER_SEC << endl;
	  }
      
	for ( int eli = 1; eli < GetNE(); eli++ )
	  {
	    Element & el = VolumeElement(eli);
	  }
	
	if (strcmp (st.c_str(), "endmesh") == 0)
	  {
	    endmesh = true;
	  }


      }

    MPI_Barrier (MPI_COMM_WORLD);
    MPI_Barrier (MPI_COMM_WORLD);

    // delete glob2loc_vert;


 //    paralleltop -> SetNV ( this -> GetNV() );
//     for ( int i = 0; i < GetNV(); i++ )
//       paralleltop -> SetLoc2Glob_Vert ( i+1, (*loc2globvert)[i] );

      CalcSurfacesOfNode ();


      //  BuildConnectedNodes ();
     
      topology -> Update();

//       UpdateOverlap();
      clusters -> Update();
      
      SetNextMajorTimeStamp();
      //  PrintMemInfo (cout);
      
      // paralleltop->Print();
#ifdef SCALASCA
#pragma pomp inst end(loadmesh)
#endif
      
  }
  








#ifdef PARALLEL 

  // distribute the mesh to local processors
  // only call for id == 0 !!
    void Mesh :: Distribute ()
    {
      if ( id != 0 ) return;
      
      double scale = 1;  // globflags.GetNumFlag ("scale", 1);
      int inverttets = 0;  // globflags.GetDefineFlag ("inverttets");
      int invertsurf = 0;  // globflags.GetDefineFlag ("invertsurfacemesh");

#ifdef PARALLEL
    // metis partition of mesh, only if more than one proc

    if ( ntasks > 1 )
      {	
	// array contains local numbers of elements in meshes
	ARRAY<int> ne_local (ntasks);
        ne_local = 0;
	   

#ifdef SCALASCA
#pragma pomp inst begin(metis)
#endif
	// partition
	ParallelMetis ( ne_local );
#ifdef SCALASCA
#pragma pomp inst end(metis)
#endif
	// send partition
	if ( ntasks > 1 )
	SendMesh ( this, ne_local, scale, inverttets, invertsurf );
	//paralleltop -> UpdateCoarseGrid();

      }
#endif
    
//     for ( int dest = 1; dest < ntasks; dest++ )
//       MyMPI_Send ( "overlap++", dest );
//     CalcSurfacesOfNode ();
    
//     //  BuildConnectedNodes ();
    
//     // here also parallel information is computed
    topology -> Update();
   
    paralleltop -> UpdateCoarseGrid();

//     UpdateOverlap();

#ifdef SCALASCA
#pragma pomp inst end(loadmesh_seq)
#endif
    // paralleltop -> Print();

//     clusters -> Update();
      
// #ifdef PARALLEL  
//     if ( ntasks > 1 )
//       {    
// 	//  PrintMemInfo (cout);
// 	paralleltop -> Print();
// 	delete mastermesh;
//       }
	
// #endif
  }
  




#endif








  void Mesh :: ParallelMetis (  ARRAY<int> & neloc )
  {
    
      
    int ne = GetNE();
    
    int nn = GetNP();
    
    ELEMENT_TYPE elementtype = TET; // VolumeElement(1).GetType();

    // metis works only for uniform tet/hex meshes
    for ( int el = 2; el <= GetNE(); el++ )
      if ( VolumeElement(el).GetType() != elementtype )
	{
	  /*
	  int nelperproc = ne / (ntasks-1);
	  for (int i=1; i<=ne; i++)
	    { 
	      int partition = i / nelperproc + 1;
	      if ( partition >= ntasks ) partition = ntasks-1;
	      VolumeElement(i).SetPartition(partition);
	    }
	  */

	  PartHybridMesh ( neloc );
	  
	  return;
	}

    int npe = VolumeElement(1).GetNP();
    
    idxtype *elmnts;
    elmnts = new idxtype[ne*npe];
    
    int etype;
    if (elementtype == TET)
      etype = 2;
    else if (elementtype == HEX)
      etype = 3;
    
    
    for (int i=1; i<=ne; i++)
      for (int j=1; j<=npe; j++)
	elmnts[(i-1)*npe+(j-1)] = VolumeElement(i).PNum(j)-1;
    
    int numflag = 0;
    int nparts = ntasks-1;
    
    int edgecut;
    idxtype *epart, *npart;
    epart = new idxtype[ne];
    npart = new idxtype[nn];
    

//     if ( ntasks == 1 ) 
//       {
// 	(*this) = *mastermesh;
// 	nparts = 4;	   
// 	metis :: METIS_PartMeshDual (&ne, &nn, elmnts, &etype, &numflag, &nparts,
// 				     &edgecut, epart, npart);
// 	cout << "done" << endl;
	
// 	cout << "edge-cut: " << edgecut << ", balance: " << metis :: ComputeElementBalance(ne, nparts, epart) << endl;
	
// 	for (int i=1; i<=ne; i++)
// 	  {
// 	    mastermesh->VolumeElement(i).SetPartition(epart[i-1]);
// 	  }
	
// 	return;
//       }
    
#ifdef METIS
    
    metis :: METIS_PartMeshDual (&ne, &nn, elmnts, &etype, &numflag, &nparts,
				 &edgecut, epart, npart);
    cout << "done" << endl;
    
    cout << "edge-cut: " << edgecut << ", balance: " << metis :: ComputeElementBalance(ne, nparts, epart) << endl;
    

    // partition numbering by metis : 0 ...  ntasks - 1
    // we want:                       1 ...  ntasks

    for (int i=1; i<=ne; i++)
      VolumeElement(i).SetPartition(epart[i-1] + 1);
    
    for ( int i=1; i<=ne; i++)
      {
	neloc[epart[i-1]] ++;
      }

    delete [] elmnts; delete [] epart; delete [] npart;

#endif
    
  }



  void Mesh :: PartHybridMesh ( ARRAY<int> & neloc ) 
  {

    int ne = GetNE();
    
    int nn = GetNP();
    int nedges = topology->GetNEdges();

    idxtype  *xadj, * adjacency, *v_weights = NULL, *e_weights = NULL;

    int weightflag = 0;
    int numflag = 0;
    int nparts = ntasks - 1;

    int options[5];
    options[0] = 0;
    int edgecut;
    idxtype * part;

    xadj = new idxtype[nn+1];
    part = new idxtype[nn];

    ARRAY<int> cnt(nn+1);
    cnt = 0;

    for ( int edge = 1; edge <= nedges; edge++ )
      {
	int v1, v2;
	topology->GetEdgeVertices ( edge, v1, v2);
	cnt[v1-1] ++;
	cnt[v2-1] ++;
      }

    xadj[0] = 0;
    for ( int n = 1; n <= nn; n++ )
      {
	xadj[n] = idxtype(xadj[n-1] + cnt[n-1]); 
      }

    adjacency = new idxtype[xadj[nn]];
    cnt = 0;

    for ( int edge = 1; edge <= nedges; edge++ )
      {
	int v1, v2;
	topology->GetEdgeVertices ( edge, v1, v2);
	adjacency[ xadj[v1-1] + cnt[v1-1] ] = v2-1;
	adjacency[ xadj[v2-1] + cnt[v2-1] ] = v1-1;
	cnt[v1-1]++;
	cnt[v2-1]++;
      }

    for ( int vert = 0; vert < nn; vert++ )
      {
	FlatArray<int> array ( cnt[vert], &adjacency[ xadj[vert] ] );
	BubbleSort(array);
      }

    metis :: METIS_PartGraphKway ( &nn, xadj, adjacency, v_weights, e_weights, &weightflag, 
			  &numflag, &nparts, options, &edgecut, part );

    ARRAY<int> nodesinpart(ntasks);

    for ( int el = 1; el <= ne; el++ )
      {
	Element & volel = VolumeElement(el);
	nodesinpart = 0;

	//	VolumeElement(el).SetPartition(part[ volel[1] ] + 1);
	
	int el_np = volel.GetNP();
	int partition = 0; 
	for ( int i = 1; i <= el_np; i++ )
	  nodesinpart[ part[volel[i]]+1 ] ++;

	for ( int i = 1; i < ntasks; i++ )
	  if ( nodesinpart[i] > nodesinpart[partition] ) 
	    partition = i;

	volel.SetPartition(partition);

      }

    
    for ( int i=1; i<=ne; i++)
      {
	neloc[ VolumeElement(i).GetPartition() ] ++;
      }

    delete [] xadj;
    delete [] part;
    delete [] adjacency;
  }




  void Mesh :: SendMesh ( Mesh * mastermesh, ARRAY<int> & neloc, 
			  const double & scale, const int & inverttets, const int & invertsurf )
  {
    PrintMessage ( 1, "Sending Mesh to local processors" );
    
    int timer = NgProfiler::CreateTimer ("SendMesh");
    NgProfiler::RegionTimer reg(timer);
    
#ifdef SCALASCA
#pragma pomp inst begin(sendmesh)
#endif

    clock_t starttime, endtime, soltime;
    starttime = clock();


    int nv=0,  ned=0, nfa=0;

    int nelglob, nvglob, nedglob, nfaglob;

    paralleltop -> SetNV ( mastermesh-> GetNV() );
    paralleltop -> SetNE ( mastermesh -> GetNE() );
    paralleltop -> SetNSegm ( mastermesh ->GetNSeg() );
    paralleltop -> SetNSE ( mastermesh -> GetNSE() );
    int np = 0;


    nelglob = mastermesh -> GetNE();
    nvglob = mastermesh -> GetNP();
    nedglob = mastermesh -> GetTopology().GetNEdges();
    nfaglob = mastermesh -> GetTopology().GetNFaces();
    dimension = mastermesh -> GetDimension();

    MPI_Request sendrequest[ntasks];

    for ( int dest = 1; dest < ntasks; dest++ )
      MyMPI_Send ("mesh", dest);

    MPI_Barrier (MPI_COMM_WORLD);
    

    for ( int dest = 1; dest < ntasks; dest++ )
      {
        MyMPI_Send (nelglob, dest);
	MyMPI_Send (neloc[dest], dest);
	MyMPI_Send ( nvglob, dest);
	MyMPI_Send ( nedglob, dest );
	MyMPI_Send ( nfaglob, dest);
	MyMPI_Send( mastermesh->GetDimension(), dest );
      }




    // get number of vertices in each mesh
    ARRAY<int> elarraysize(ntasks);
    ARRAY<int> nelloc ( ntasks );
    
    // nvi = 0;
    nelloc = 0;
    elarraysize = 1;

    PrintMessage ( 3, "sending vertices");

    for ( int dest = 1; dest < ntasks; dest ++ )
      MyMPI_Send ( "vertex", dest );
    
    ARRAY<int> num_els_on_proc(ntasks);
    num_els_on_proc = 0;
    for (ElementIndex ei = 0; ei < mastermesh->GetNE(); ei++)
      num_els_on_proc[(*mastermesh)[ei].GetPartition()]++;

    TABLE<ElementIndex> els_of_proc (num_els_on_proc);
    for (ElementIndex ei = 0; ei < mastermesh->GetNE(); ei++)
      els_of_proc.Add ( (*mastermesh)[ei].GetPartition(), ei);


    ARRAY<int, PointIndex::BASE> vert_flag ( mastermesh -> GetNV() );

    ARRAY<int> num_verts_on_proc (ntasks);
    ARRAY<int, PointIndex::BASE> num_procs_on_vert ( mastermesh -> GetNV() );

    num_verts_on_proc = 0;
    num_procs_on_vert = 0;

    vert_flag = -1;
    for (int dest = 1; dest < ntasks; dest++)
      {
	FlatArray<ElementIndex> els = els_of_proc[dest];

	for (int hi = 0; hi < els.Size(); hi++)
	  {
	    const Element & el = (*mastermesh) [ els[hi] ];
	    
	    for (int i = 0; i < el.GetNP(); i++)
	      {
		PointIndex epi = el.PNum(i+1);
		if (vert_flag[epi] < dest)
		  {
		    vert_flag[epi] = dest;

		    num_verts_on_proc[dest]++;
		    num_procs_on_vert[epi]++;

		    paralleltop -> SetDistantPNum ( dest, epi, num_verts_on_proc[dest]);
		  }
	      }

	    elarraysize[dest] += 3 + el.GetNP();
	    nelloc[dest] ++;
	    paralleltop -> SetDistantEl ( dest, els[hi]+1, nelloc[dest] );
	  }
      }


    TABLE<PointIndex> verts_of_proc (num_verts_on_proc);
    TABLE<int, PointIndex::BASE> procs_of_vert (num_procs_on_vert);
    TABLE<int, PointIndex::BASE> loc_num_of_vert (num_procs_on_vert);

    vert_flag = -1;
    for (int dest = 1; dest < ntasks; dest++)
      {
	FlatArray<ElementIndex> els = els_of_proc[dest];

	for (int hi = 0; hi < els.Size(); hi++)
	  {
	    const Element & el = (*mastermesh) [ els[hi] ];
	    
	    for (int i = 0; i < el.GetNP(); i++)
	      {
		PointIndex epi = el.PNum(i+1);
		if (vert_flag[epi] < dest)
		  {
		    vert_flag[epi] = dest;
		    procs_of_vert.Add (epi, dest);
		  }
	      }
	  }
      }

    for (int vert = 1; vert <= mastermesh->GetNP(); vert++ )
      {
	FlatArray<int> procs = procs_of_vert[vert];
	for (int j = 0; j < procs.Size(); j++)
	  {
	    int dest = procs[j];
	    verts_of_proc.Add (dest, vert);
	    loc_num_of_vert.Add (vert, verts_of_proc[dest].Size());
	  }
      }


    ARRAY<int> nvi5(ntasks);
    for (int i = 0; i < ntasks; i++) 
      nvi5[i] = 5 * num_verts_on_proc[i];

    TABLE<double> pointarrays(nvi5);



    for (int dest = 1; dest < ntasks; dest++)
      {
	FlatArray<PointIndex> verts = verts_of_proc[dest];

	for ( int j = 0, ii = 0; j < verts.Size(); j++)
	  {
	    const MeshPoint & hp = mastermesh -> Point (verts[j]);
	    pointarrays.Add (dest, double(verts[j]));
	    pointarrays.Add (dest, hp(0));
	    pointarrays.Add (dest, hp(1));
	    pointarrays.Add (dest, hp(2));
	    pointarrays.Add (dest, hp.Singularity());
	  }

        MyMPI_ISend ( pointarrays[dest], dest, sendrequest[dest] );
      }

    for (int dest = 1; dest < ntasks; dest++)
      MPI_Request_free (&sendrequest[dest]);



    ARRAY<int> num_distpnums(ntasks);
    num_distpnums = 0;

    for (int vert = 1; vert <= mastermesh -> GetNP(); vert++)
      {
	FlatArray<int> procs = procs_of_vert[vert];
	for (int j = 0; j < procs.Size(); j++)
	  num_distpnums[procs[j]] += 3 * (procs.Size()-1);
      }

    TABLE<int> distpnums (num_distpnums);

    for (int vert = 1; vert <= mastermesh -> GetNP(); vert++)
      {
	FlatArray<int> procs = procs_of_vert[vert];
	for (int j = 0; j < procs.Size(); j++)
	  for (int k = 0; k < procs.Size(); k++)
	    if (j != k)
	      {
		distpnums.Add (procs[j], loc_num_of_vert[vert][j]);
		distpnums.Add (procs[j], procs_of_vert[vert][k]);
		distpnums.Add (procs[j], loc_num_of_vert[vert][k]);
	      }
      }
    
    for ( int dest = 1; dest < ntasks; dest ++ )
      MyMPI_ISend ( distpnums[dest], dest, sendrequest[dest] );

    for (int dest = 1; dest < ntasks; dest++)
      MPI_Request_free (&sendrequest[dest]);


    PrintMessage ( 1, "vertices sent");


    endtime = clock();
    (*testout) << "Sending Time verts = " << double(endtime - starttime)/CLOCKS_PER_SEC << endl;



    TABLE<int> elementarrays(elarraysize);

    starttime = clock();

    for ( int dest = 1; dest < ntasks; dest ++ )
      MyMPI_Send ( "volumeelements", dest);

    for ( int dest = 1; dest < ntasks; dest++ )
      elementarrays.Add (dest, nelloc[dest]);

    for ( int ei = 1; ei <= mastermesh->GetNE(); ei++)
      {
	const Element & el = mastermesh -> VolumeElement (ei);
	int dest = el.GetPartition();

	if ( dest > 0 )
	  {
            // send volume element

  	    elementarrays.Add (dest, ei); // 
 	    elementarrays.Add (dest, el.GetIndex());
 	    elementarrays.Add (dest, el.GetNP());
 	    for ( int ii=0; ii<el.GetNP(); ii++)
 	      elementarrays.Add (dest, el[ii]);
	  }
      }

    PrintMessage ( 3, "sending elements" );
    for ( int dest = 1; dest < ntasks; dest ++ )
      MyMPI_ISend ( elementarrays[dest], dest, sendrequest[dest] );

    for (int dest = 1; dest < ntasks; dest++)
      MPI_Request_free (&sendrequest[dest]);


    PrintMessage ( 3, "elements sent" );
  
    endtime = clock();
    (*testout) << "Sending Time els = " << double(endtime - starttime)/CLOCKS_PER_SEC << endl;
    starttime = clock();

    for ( int dest = 1; dest < ntasks; dest++)
      for ( int fdi = 1; fdi <= mastermesh->GetNFD(); fdi++)
        {
	  MyMPI_Send("facedescriptor", dest);
          double doublebuf[6]; 

          doublebuf[0] = GetFaceDescriptor(fdi).SurfNr();
          doublebuf[1] = GetFaceDescriptor(fdi).DomainIn();	
          doublebuf[2] = GetFaceDescriptor(fdi).DomainOut();
          doublebuf[3] = GetFaceDescriptor(fdi).BCProperty();
	  doublebuf[4] = GetFaceDescriptor(fdi).domin_singular;
	  doublebuf[5] = GetFaceDescriptor(fdi).domout_singular;

	  MyMPI_Send ( &doublebuf[0], 6, dest);
        }

    PrintMessage ( 3, "fdi sent" );
    
    endtime = clock();
    (*testout) << "Sending Time fdi = " << double(endtime - starttime)/CLOCKS_PER_SEC << endl;
    starttime = clock();

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
	bufsize[i] = 1;
	seli[i] = 1;
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

    
    int ** selbuf = 0;
    selbuf = new int*[ntasks];
    for ( int i = 0; i < ntasks; i++)
      if ( bufsize[i] > 0 )
	{*(selbuf+i) = new int[bufsize[i]];}
 	else 
	  selbuf[i] = 0;
    


    ARRAY<int> nselloc (ntasks);
    nselloc = 0;

    for ( int dest = 1; dest < ntasks; dest++ )
      {
	MyMPI_Send ( "surfaceelementsgi", dest);  
	selbuf[dest][0] = nlocsel[dest];                  
      }    

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
		    nselloc[dest] ++;
		    paralleltop -> SetDistantSurfEl ( dest, sei, nselloc[dest] );
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
	if (selbuf[dest])
	  delete [] *(selbuf+dest);
      }
    delete [] selbuf;
    
    PrintMessage ( 3, "surfels sent");
    
    endtime = clock();
    (*testout) << "Sending Time surfels = " << double(endtime - starttime)/CLOCKS_PER_SEC << endl;
    starttime = clock();


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
	const MeshTopology & topol = mastermesh -> GetTopology();
	topol . GetSegmentVolumeElements ( segi, volels );
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
      if ( bufsize[i] > 0 )
	segmbuf[i] = new double[bufsize[i]];
      else
	segmbuf[i] = 0;
    
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
		paralleltop -> SetDistantSegm ( dest, ls, int ( segi[dest] / 14 ) );
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
	 if ( segmbuf[dest] )
	   delete [] segmbuf[dest];
       }
     delete [] segmbuf;
    
    PrintMessage ( 3, "segments sent");

    endtime = clock();
    (*testout) << "Sending Time segments = " << double(endtime - starttime)/CLOCKS_PER_SEC << endl;
    starttime = clock();


    for ( int dest = 1; dest < ntasks; dest++ )
      {
	MyMPI_Send("endmesh", dest);
      }


    /*
    for ( int dest = 1; dest < ntasks; dest ++ )
      {
        MPI_Status status;
        MPI_Wait (&sendrequest[dest], &status);
      }
    */


    MPI_Barrier (MPI_COMM_WORLD);
    PrintMessage ( 1, "Sending mesh to local processors completed" ); 
    MPI_Barrier (MPI_COMM_WORLD);

#ifdef SCALASCA
#pragma pomp inst end(sendmesh)
#endif
  }








  void Mesh :: UpdateOverlap()
  {
    
    (*testout) << "UPDATE OVERLAP" << endl;
    ARRAY<double> ** sendpoints, ** recvpoints;
    ARRAY<int> **sendelements, **recvelements;
    ARRAY<int> ** sendsel, **recvsel;
    ARRAY<int> * globelnums;

#ifdef SCALASCA
#pragma pomp inst begin(updateoverlap)
#endif
    paralleltop->IncreaseOverlap();

    if ( id > 0 )
      {
       
	int nvglob = paralleltop->GetNVGlob (), nelglob = paralleltop->GetNEGlob();
	ARRAY<int,1> * glob2loc_vert = new ARRAY<int,1> (nvglob);
	(*glob2loc_vert) = -1;

	for ( int locv = 1; locv <= GetNV(); locv++)
	  {
	    int globv = paralleltop->GetLoc2Glob_Vert(locv);
	    (*glob2loc_vert)[globv] = locv;
	  }

	sendpoints = new ARRAY<double>* [ntasks];
	recvpoints = new ARRAY<double> * [ntasks];
	sendelements = new ARRAY<int>* [ntasks];
	recvelements = new ARRAY<int> * [ntasks];
	sendsel = new ARRAY<int> * [ntasks];
	recvsel = new ARRAY<int> * [ntasks];
	
	BitArray addedpoint ( paralleltop -> GetNVGlob () );
	BitArray addedel ( paralleltop -> GetNEGlob () );
	addedpoint . Clear();
	addedel.Clear();
	
	ARRAY<int> distvert ( ntasks ), distel(ntasks), nsenddistel(ntasks);
	for ( int sender = 1; sender < ntasks; sender ++ )
	  if ( id == sender )
	    {
	      for ( int dest = 1; dest < ntasks; dest ++ )
		if ( dest != id)
		  {
		    MyMPI_Send ( GetNV(), dest);
		    MyMPI_Send ( GetNE(), dest);
		  }
	    }
	  else
	    {
	      int anv, ane;
	      MyMPI_Recv (anv, sender);
	      MyMPI_Recv (ane, sender);
	      distvert[sender] = anv;
	      distel[sender] = ane;
	      nsenddistel[sender] = 0;
	    }
	
	
	BitArray appendedpoint ( GetNP() * ntasks );
	appendedpoint.Clear();
	
	for ( int i = 0; i < ntasks; i++ )
	  {
	    sendpoints[i] = new ARRAY<double> (0);
	    sendelements[i] = new ARRAY<int> (1);
	    recvpoints[i] = new ARRAY<double> (0);
	    recvelements[i] = new ARRAY<int> (0);
	    sendsel[i] = new ARRAY<int> (1);
	    recvsel[i] = new ARRAY<int> (0);
	  }
	
	ARRAY<int> nsentsel (ntasks);
	nsentsel = 0;

	for ( int seli = 1; seli <= GetNSE(); seli++ )
	  {
	    const Element2d & sel = SurfaceElement(seli);
	    int selnp = sel.GetNP();
	    ARRAY<int> vert (selnp);
	    
	    ARRAY<int> alldests (0), dests;
	    
	    bool isparsel = false;
	    for ( int i = 0; i < selnp; i++ )
	      {
		vert[i] = sel.PNum(i+1);
		if ( paralleltop -> IsExchangeVert ( vert[i] ) )
		  {
		    isparsel = true;
		    paralleltop -> GetVertNeighbours ( vert[i], dests );
		    for ( int j = 0; j < dests.Size(); j++ )
		      if ( !alldests.Contains ( dests[j] ) )
			alldests.Append( dests[j] );
		  }
		
		
	      }
	    
	    if ( !isparsel ) continue;
	    
	    for ( int i = 0; i < alldests.Size(); i ++ )
	      {
		// send the surface element to all distant procs:
		
		// loc number, 
		// number of points 
		// global vert numbers
		// surface_element_index
		
		int dest = alldests[i];
		
		// ***************** MISSING id = 0
		if ( dest == 0 ) continue;
		
		sendsel[dest]->Append(seli);
		sendsel[dest]->Append(selnp);
		for ( int ii=0; ii<selnp; ii++)
		  {
		    sendsel[dest] -> Append ( paralleltop -> GetLoc2Glob_Vert (vert[ii]) );
		  }
		sendsel[dest]->Append ( sel.GetIndex() );
		distel[dest] ++;
		nsentsel[dest] ++;
	      }
	  }

	for ( int dest = 1; dest < ntasks; dest++ )
	    (*sendsel[dest])[0] = nsentsel[dest];
	
	for ( int eli = 1; eli <= GetNE(); eli++ )
	  {
	    const Element & el = VolumeElement(eli);
	    int elnp = el.GetNP();
	    ARRAY<int> vert (elnp);
	    
	    ARRAY<int> alldests (0), dests;
	    
	    for ( int i = 0; i < elnp; i++ )
	      {
		vert[i] = el.PNum(i+1);
		if ( paralleltop -> IsExchangeVert ( vert[i] ) )
		  {
		    paralleltop -> GetVertNeighbours ( vert[i], dests );
		    for ( int j = 0; j < dests.Size(); j++ )
		      if ( !alldests.Contains ( dests[j] ) )
			{
			  alldests.Append( dests[j] );
			  paralleltop->SetExchangeElement ( dests[j], eli );
			}
		    paralleltop->SetExchangeElement ( eli );
		  }


	      }

	  }


	for ( int eli = 1; eli <= GetNE(); eli++ )
	  {
	    const Element & el = VolumeElement(eli);
	    int elnp = el.GetNP();
	    ARRAY<int> vert (elnp);

	    // 	  append to point list:
	    // 	  local pnum
	    // 	  global pnum
	    // 	  point coordinates

	    ARRAY<Point3d> points(elnp);
	    for ( int i = 0; i < elnp; i++ )
	      {
		vert[i] = el.PNum(i+1);
		points[i] = Point(vert[i]);
		ARRAY<int> knowndests;
		// send point to all dests which get the volume element
		for ( int dest = 0; dest < ntasks; dest ++ )
		  {
		    // nur die neuen verts
		    if ( !paralleltop -> IsExchangeElement ( dest, eli )  ) continue;

		    // jeder vertex nur ein mal
		    if ( appendedpoint.Test( (vert[i]-1) * ntasks + dest ) ) continue;
		
		    appendedpoint.Set( (vert[i]-1) * ntasks + dest );
		    paralleltop -> SetExchangeVert (dest,  vert[i]);
		    paralleltop -> SetExchangeVert ( vert[i] );


		    // append vertex to be sent
		    // loc pnum
		    // glob pnum
		    // coords

		    // local pnum
		    sendpoints[dest] -> Append ( vert[i] );
		    // global pnum
		    sendpoints[dest] -> Append ( paralleltop -> GetLoc2Glob_Vert ( vert[i] ) );
		    // coordinates
		    sendpoints[dest]->Append(points[i].X() );
		    sendpoints[dest]->Append(points[i].Y() );
		    sendpoints[dest]->Append(points[i].Z() );

		  }
	      }



	    for ( int dest = 1; dest < ntasks; dest ++ )
	      {
		// send the volume element to all distant procs:
	    
		// loc number, 
		// glob number
		// number of points 
		// glob vertices
		// element_index
		if ( !paralleltop -> IsExchangeElement ( dest, eli )  ) continue;

		// loc number
		sendelements[dest]->Append(eli);
		// glob number
		sendelements[dest]->Append ( paralleltop -> GetLoc2Glob_VolEl(eli) );

		sendelements[dest]->Append(elnp);

		for ( int j = 0; j < elnp; j++ )
		  sendelements[dest]->Append ( paralleltop -> GetLoc2Glob_Vert(vert[j]) );

		sendelements[dest]->Append ( el.GetIndex() );

		distel[dest]++;
		nsenddistel[dest] ++;
		paralleltop -> SetDistantEl ( dest, eli, distel[dest] );
	      }


	  }

	for ( int dest = 1; dest < ntasks; dest++ )
	  if ( dest != id )
	    (*sendelements[dest])[0] = nsenddistel[dest];
	// find parallel surface elements, if there, append to sendsel - list

	
	distel = 0;    

// 	for ( int dest = 1; dest < ntasks; dest++ )
// 	  if ( dest != id )
// 	    (*sendsel[dest])[0] = distel[dest];

	for ( int sender = 1; sender < ntasks; sender ++ )
	  {
	    if ( id == sender )
	      for ( int dest = 1; dest < ntasks; dest ++ )
		if ( dest != id)
		  {
		    MyMPI_Send ( *(sendpoints[dest]), dest);
		    MyMPI_Send ( *(sendelements[dest]), dest);
		    MyMPI_Send ( *(sendsel[dest]), dest );
		    delete sendelements[dest];
		    delete sendpoints[dest];
		    delete sendsel[dest];
		  }
	
	
	    if ( id != sender )
	      {
		MyMPI_Recv (*( recvpoints[sender]), sender);
		MyMPI_Recv (*( recvelements[sender]), sender);
		MyMPI_Recv (*( recvsel[sender]), sender);
	      }
	  }

	delete sendelements[id];
	delete sendpoints[id];
	delete sendsel[id];
   
	delete [] sendelements; delete [] sendpoints; delete [] sendsel;

  
	ARRAY<int> * distpnum2parpnum;
	distpnum2parpnum = new ARRAY<int> [2];
	distpnum2parpnum[0].SetSize(0);
	distpnum2parpnum[1].SetSize(0);
 
	ARRAY<int> firstdistpnum (ntasks);

	for ( int sender = 1; sender < ntasks; sender++)
	  {
	    firstdistpnum[sender] = distpnum2parpnum[0].Size(); 

	    if ( sender == id ) continue;

	    int ii = 0; 
	    // receiving points
	    // dist pnum
	    // glob pnum
	    // coords
	    int numrecvpts = int ( recvpoints[sender]->Size() / 5 );

	    paralleltop -> SetNV ( GetNV() + numrecvpts );
	    int expectnp = GetNV () + numrecvpts;

	    // received points
	    while ( ii < recvpoints[sender] -> Size() )
	      {
		int distpnum = int ( (*recvpoints[sender])[ii++] );
		int globpnum = int ( (*recvpoints[sender])[ii++] );

		Point3d point;
		point.X() = (*recvpoints[sender])[ii++];
		point.Y() = (*recvpoints[sender])[ii++];
		point.Z() = (*recvpoints[sender])[ii++];

		// append point as ghost
		// if not already there
		int pnum=  (*glob2loc_vert)[globpnum];//paralleltop -> Glob2Loc_Vert ( globpnum );
		if ( pnum <= 0 )
		  {
		    pnum = AddPoint ( point, true );
		  }
		paralleltop -> SetDistantPNum ( 0, pnum, globpnum );
		(*glob2loc_vert)[globpnum] = pnum;
		paralleltop -> SetDistantPNum ( sender, pnum, distpnum );
		paralleltop -> SetExchangeVert ( pnum );
	      }

	    ii = 0;

	    int recvnel = (*recvelements[sender])[ii++];


	    paralleltop -> SetNE ( recvnel + GetNE() );

	    while ( ii < recvelements[sender] -> Size() )
	      {
		// receive list:
		// distant number, 
		// glob number
		// number of points 
		// glob vertices
		// element_index

		int distelnum = (*recvelements[sender])[ii++];
		int globelnum = (*recvelements[sender])[ii++] ;
		int elnp = (*recvelements[sender])[ii++] ;
		ARRAY<int> pnums(elnp), globpnums(elnp);

		// append volel
		ELEMENT_TYPE eltype; 
		switch ( elnp )
		  {
		  case 4: eltype = TET; break;
		  case 5: eltype = PYRAMID; break;
		  case 6: eltype = PRISM; break;
		  case 8: eltype = HEX; break;
		  }
	    
		Element el ( eltype ) ;

		for ( int i = 0; i < elnp; i++ )
		  {
		    globpnums[i] = int ( (*recvelements[sender])[ii++] );
		    pnums[i] = (*glob2loc_vert)[globpnums[i]]; //paralleltop -> Glob2Loc_Vert(globpnums[i]);
		  }

		el.SetIndex ( (*recvelements[sender])[ii++] );
		el.SetGhost ( 1 );
	

		for ( int i = 0; i < elnp; i++)
		  {
		    (int&) el[i] = pnums[i];
		  }

		int eli = AddVolumeElement (el) + 1;

		paralleltop -> SetDistantEl ( sender, eli, distelnum);
		paralleltop -> SetDistantEl ( 0, eli, globelnum );
		paralleltop -> SetExchangeElement ( eli );
	      }

	    ii = 0;
	    int nrecvsendsel = 0;
	    if ( recvsel[sender] -> Size() > 0 )
	      nrecvsendsel = (*recvsel[sender])[ii++];

	    paralleltop -> SetNSE ( nrecvsendsel + GetNSE() );
 
	    while ( ii < recvsel[sender] -> Size() )
	      {
		// receive list:
		// distant number, 
		// number of points 
		// global vert numbers
		// surface_element_index

		int distselnum = (*recvsel[sender])[ii++];
		int selnp = (*recvsel[sender])[ii++] ;

		ARRAY<int> globpnums(selnp);
		ARRAY<int> pnums(selnp);

		// append volel
		ELEMENT_TYPE eltype; 
		switch ( selnp )
		  {
		  case 4: eltype = QUAD; break;
		  case 3: eltype = TRIG; break;
		  }
	    
		Element2d sel ( eltype ) ;
		for ( int i = 0; i < selnp; i++ )
		  {
		    globpnums[i] = int ( (*recvsel[sender])[ii++] );
		    pnums[i] =  (*glob2loc_vert)[globpnums[i]];//paralleltop -> Glob2Loc_Vert ( globpnums[i] );
		  }

		sel.SetIndex ( (*recvsel[sender])[ii++] );
		sel.SetGhost ( 1 );

	       
		for ( int i = 0; i < selnp; i++)
		  {
		    (int&) sel[i] = pnums[i];
		  }

		int seli = AddSurfaceElement (sel);

	      }

	    delete recvpoints[sender];
	    delete recvelements[sender];
	    delete recvsel[sender];

	  }	    

	delete [] distpnum2parpnum;

	delete recvpoints[id];
	delete recvelements[id];
	delete recvsel[id];
	delete [] recvpoints; delete []  recvelements; delete []  recvsel;
  
	delete glob2loc_vert;
      }

   
    globelnums = new ARRAY<int>;
    if ( id == 0 )
      {
	for ( int dest = 1; dest < ntasks; dest++)
	  {
	    MyMPI_Recv ( *globelnums, dest );
	    for ( int i = 0; i < globelnums->Size(); i++ )
	      {
		paralleltop -> SetDistantEl ( dest, (*globelnums)[i],i+1 );
		paralleltop -> SetExchangeElement ( dest, (*globelnums)[i] );
	      }
	  }
      }
    else
      {
	globelnums -> SetSize(GetNE());
	for ( int i = 0; i < GetNE(); i++ )
	  {
	    (*globelnums)[i] = paralleltop -> GetLoc2Glob_VolEl ( i+1 );
	  }
	MyMPI_Send ( *globelnums, 0 );
      }
    
    delete globelnums;
    // send which elements are where

    topology -> Update();
    // edge, facenums have probably changed as elements were added
    // paralleltop has to be updated


    paralleltop -> UpdateExchangeElements();
    

    paralleltop -> UpdateCoarseGridOverlap();
    //paralleltop -> UpdateTopology();
    
//     *testout << "############################################" << endl << endl;
//     paralleltop -> Print();

    clusters -> Update();
    ;
#ifdef SCALASCA
#pragma pomp inst end(updateoverlap)
#endif

  }








}



#endif
