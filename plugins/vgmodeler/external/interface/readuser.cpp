//
//  Read user dependent output file
//


#include <mystdlib.h>


#include <myadt.hpp>
#include <linalg.hpp>
#include <csg.hpp>
#include <meshing.hpp>

namespace vgmnetgen
{
#include "writeuser.hpp"

void ReadFile (Mesh & mesh,
	       const string & hfilename)
{
  cout << "Read User File" << endl;

  const char * filename = hfilename.c_str();

  int i, j;

  char reco[100];
  int np, nbe;



  // ".surf" - mesh
  
  if ( (strlen (filename) > 5) &&
       strcmp (&filename[strlen (filename)-5], ".surf") == 0 )
    
    {
      cout << "Surface file" << endl;
      
      ifstream in (filename);
      
      in >> reco;
      in >> np;
      for (i = 1; i <= np; i++)
	{
	  Point3d p;
	  in >> p.X() >> p.Y() >> p.Z();
	  mesh.AddPoint (p);
	}
      
      
      in >> nbe;
      //      int invert = globflags.GetDefineFlag ("invertsurfacemesh");
      for (i = 1; i <= nbe; i++)
	{
	  Element2d el;
	  int hi;
	  
	  el.SetIndex(1);
	  
	  for (j = 1; j <= 3; j++)
	    {
	      in >> el.PNum(j);
	      if (el.PNum(j) < PointIndex(1) || 
		  el.PNum(j) > PointIndex(np))
		{
		  cerr << "Point Number " << el.PNum(j) << " out of range 1..."
		       << np << endl;
		  return;
		}
	    }
	  
	  /*
	    if (invert)
	    swap (el.PNum(2), el.PNum(3));
	  */
	  
	  mesh.AddSurfaceElement (el);
	}
      
      mesh.ClearFaceDescriptors();
      mesh.AddFaceDescriptor (FaceDescriptor(0,1,0,0));
      
      cout << "points: " << np << " faces: " << nbe << endl;
    }
  
  
  

  
  // Universal mesh (AVL)
  if ( (strlen (filename) > 4) &&
       strcmp (&filename[strlen (filename)-4], ".unv") == 0 )
    {  
      int i, j, k;
      
      double h;
      char reco[100];
      int np, nbe;
      int invert;
      
      
      ifstream in(filename);

      invert = 0;    // globflags.GetDefineFlag ("invertsurfacemesh");
      double scale = 1;  // globflags.GetNumFlag ("scale", 1);


      while (in.good())
	{
	  in >> reco;
	  if (strcmp (reco, "NODES") == 0)
	    {
	      cout << "nodes found" << endl;
	      for (j = 1; j <= 4; j++)
		in >> reco;  // read dummy

	      while (1)
		{
		  int pi, hi;
		  double x, y, z;
		  Point3d p;

		  in >> pi;
		  if (pi == -1)
		    break;

		  in >> hi >> hi >> hi;
		  in >> p.X() >> p.Y() >> p.Z();

		  p.X() *= scale;
		  p.Y() *= scale;
		  p.Z() *= scale;


		  mesh.AddPoint (p);
		}
	    }

	  if (strcmp (reco, "ELEMENTS") == 0)
	    {
	      cout << "elements found" << endl;
	      for (j = 1; j <= 4; j++)
		in >> reco;  // read dummy

	      while (1)
		{
		  int hi;
		  in >> hi;
		  if (hi == -1) break;
		  for (j = 1; j <= 7; j++)
		    in >> hi;
	      
		  Element2d el;
		  el.SetIndex(1);
		  in >> el.PNum(1) >> el.PNum(2) >> el.PNum(3);
	      
		  if (invert)
		    swap (el.PNum(2), el.PNum(3));
		  mesh.AddSurfaceElement (el);	  
	      
		  for (j = 1; j <= 5; j++)
		    in >> hi;
		}
	    }
	}
      
      mesh.ClearFaceDescriptors();
      mesh.AddFaceDescriptor (FaceDescriptor(0,1,0,0));

      Point3d pmin, pmax;
      mesh.GetBox (pmin, pmax);
      cout << "bounding-box = " << pmin << "-" << pmax << endl;
    }



  // fepp format2d:
  
  if ( (strlen (filename) > 7) &&
       strcmp (&filename[strlen (filename)-7], ".mesh2d") == 0 )
    {
      cout << "Reading FEPP2D Mesh" << endl;
      
      char buf[100];
      int np, ne, nseg, i, j;

      ifstream in (filename);

      in >> buf;

      in >> nseg;
      for (i = 1; i <= nseg; i++)
	{
	  int bound, p1, p2;
	  in >> bound >> p1 >> p2;
	  // forget them
	}

      in >> ne;
      for (i = 1; i <= ne; i++)
	{
	  int mat, nelp;
	  in >> mat >> nelp;
	  Element2d el (nelp == 3 ? TRIG : QUAD);
	  el.SetIndex (mat);
	  for (j = 1; j <= nelp; j++)
	    in >> el.PNum(j);
	  mesh.AddSurfaceElement (el);
	}

      in >> np;
      for (i = 1; i <= np; i++)
	{
	  Point3d p(0,0,0);
	  in >> p.X() >> p.Y();
	  mesh.AddPoint (p);
	}
    }

  
  else if ( (strlen (filename) > 5) &&
	    strcmp (&filename[strlen (filename)-5], ".mesh") == 0 )
    {
      cout << "Reading Neutral Format" << endl;
      
      int np, ne, nse, i, j;

      ifstream in (filename);

      in >> np;

      if (in.good())
	{
	  // file starts with an integer

	  for (i = 1; i <= np; i++)
	    {
	      Point3d p(0,0,0);
	      in >> p.X() >> p.Y() >> p.Z();
	      mesh.AddPoint (p);
	    }
	  
	  in >> ne;
	  for (i = 1; i <= ne; i++)
	    {
	      int mat;
	      in >> mat;
	      Element el (4);
	      el.SetIndex (mat);
	      for (j = 1; j <= 4; j++)
		in >> el.PNum(j);
	      mesh.AddVolumeElement (el);
	    }
	  
	  in >> nse;
	  for (i = 1; i <= nse; i++)
	    {
	      int mat, nelp;
	      in >> mat;
	      Element2d el (TRIG);
	      el.SetIndex (mat);
	      for (j = 1; j <= 3; j++)
		in >> el.PNum(j);
	      mesh.AddSurfaceElement (el);
	    }

          mesh.AddFaceDescriptor (FaceDescriptor (1, 1, 0, 0));
	}
      else
	{
	  char buf[100];
	  in.clear();
	  do
	    {
	      in >> buf;
	      cout << "buf = " << buf << endl;
	      if (strcmp (buf, "points") == 0)
		{
		  in >> np;
		  cout << "np = " << np << endl;
		}
	    }
	  while (in.good());
	}
    }


  if ( (strlen (filename) > 4) &&
       strcmp (&filename[strlen (filename)-4], ".emt") == 0 )
    {
      ifstream inemt (filename);
      
      string pktfile = filename;
      int len = strlen (filename);
      pktfile[len-3] = 'p';
      pktfile[len-2] = 'k';
      pktfile[len-1] = 't';
      cout << "pktfile = " << pktfile << endl;

      int np, nse, i;
      int num, bcprop;
      ifstream inpkt (pktfile.c_str());
      inpkt >> np;
      ARRAY<double> values(np);
      for (i = 1; i <= np; i++)
	{
	  Point3d p(0,0,0);
	  inpkt >> p.X() >> p.Y() >> p.Z()
		>> bcprop >> values.Elem(i);
	  mesh.AddPoint (p);
	}      

      mesh.ClearFaceDescriptors();
      mesh.AddFaceDescriptor (FaceDescriptor(0,1,0,0));
      mesh.GetFaceDescriptor(1).SetBCProperty (1);
      mesh.AddFaceDescriptor (FaceDescriptor(0,1,0,0));
      mesh.GetFaceDescriptor(2).SetBCProperty (2);
      mesh.AddFaceDescriptor (FaceDescriptor(0,1,0,0));
      mesh.GetFaceDescriptor(3).SetBCProperty (3);
      mesh.AddFaceDescriptor (FaceDescriptor(0,1,0,0));
      mesh.GetFaceDescriptor(4).SetBCProperty (4);
      mesh.AddFaceDescriptor (FaceDescriptor(0,1,0,0));
      mesh.GetFaceDescriptor(5).SetBCProperty (5);

      int p1, p2, p3;
      double value;
      inemt >> nse;
      for (i = 1; i <= nse; i++)
	{
	  inemt >> p1 >> p2 >> p3 >> bcprop >> value;

	  if (bcprop < 1 || bcprop > 4)
	    cerr << "bcprop out of range, bcprop = " << bcprop << endl;
	  p1++;
	  p2++;
	  p3++;
	  if (p1 < 1 || p1 > np || p2 < 1 || p2 > np || p3 < 1 || p3 > np)
	    {
	      cout << "p1 = " << p1 << " p2 = " << p2 << " p3 = " << p3 << endl;
	    }

	  if (i > 110354) Swap (p2, p3);
	  if (mesh.Point(p1)(0) < 0.25)
	    Swap (p2,p3);

	  Element2d el(TRIG);

	  if (bcprop == 1)
	    {
	      if (values.Get(p1) < -69999)
		el.SetIndex(1);
	      else
		el.SetIndex(2);
	    }
	  else
	    el.SetIndex(3);


	  el.PNum(1) = p1;
	  el.PNum(2) = p2;
	  el.PNum(3) = p3;
	  mesh.AddSurfaceElement (el);
	}


      ifstream incyl ("ngusers/guenter/cylinder.surf");
      int npcyl, nsecyl; 
      incyl >> npcyl;
      cout << "npcyl = " << npcyl << endl;
      for (i = 1; i <= npcyl; i++)
	{
	  Point3d p(0,0,0);
	  incyl >> p.X() >> p.Y() >> p.Z();
	  mesh.AddPoint (p);
	}
      incyl >> nsecyl;
      cout << "nsecyl = " << nsecyl << endl;
      for (i = 1; i <= nsecyl; i++)
	{
	  incyl >> p1 >> p2 >> p3;
	  p1 += np;
	  p2 += np;
	  p3 += np;
	  Element2d el(TRIG);
	  el.SetIndex(5);
	  el.PNum(1) = p1;
	  el.PNum(2) = p2;
	  el.PNum(3) = p3;
	  mesh.AddSurfaceElement (el);
	}
    }


  // .tet mesh
  if ( (strlen (filename) > 4) &&
       strcmp (&filename[strlen (filename)-4], ".tet") == 0 )
    {
      cout << "Reading .tet mesh" << endl;

      ifstream in (filename);

      int inputsection = 0;
      bool done = false;

      char ch;
      string str;

      string version;

      int unitcode;
      double tolerance;
      double dS1, dS2, alphaDeg, x3D, y3D, z3D;
      int nelts,nfaces,nedges,nnodes;
      int nperiodicmasternodes,ncornerperiodicmasternodes,ncubicperiodicmasternodes;
      int nperiodicmasteredges,ncornerperiodicmasteredges;
      int nperiodicmasterfaces;
      int nodeid,type,pid;
      int dummyint;
      int modelverts,modeledges,modelfaces,modelcells;
      Point3d p;
      int numObj3D,numObj2D,numObj1D,numObj0D;
      bool nullstarted;
      ARRAY<int> eldom;
      int minId3D,minId2D;
      int maxId3D(-1), maxId2D(-1), maxId1D(-1), maxId0D(-1);
      ARRAY<ARRAY<int> *> segmentdata;
      ARRAY<Element2d* > tris;

      ARRAY<int> userdata_int;  // just save data for 1:1 output
      ARRAY<double> userdata_double;
      ARRAY<int> point_pids;
      ARRAY<int> tetfacedata;
      ARRAY<int> uid_to_group_3D, uid_to_group_2D, uid_to_group_1D, uid_to_group_0D;

      while(!done)
	{
	  // skip "//" comment
	  bool comment = true;
	  while(comment)
	    {
	      ch = in.get();
	      while(ch == ' ' || ch == '\n' || ch == '\t' || ch =='\r')
		ch = in.get();
	      
	      if(ch != '/')
		{
		  comment = false;
		  in.putback(ch);
		}
	      else
		{
		  ch = in.get();
		  if(ch != '/')
		    {
		      comment = false;
		      in.putback(ch);
		      in.putback('/');
		    }
		  else
		    {
		      in.ignore(10000,'\n');
		    }
		}
	    }

	  
	  switch(inputsection)
	    {
	    case 0:
	      // version number
	      in >> version;
	      cout << "Version number " << version << endl;
	      if(version != "1.1" && version != "2" && version != "2.0")
		{
		  cerr << "WARNING: import only tested for versions 1.1 and 2" << endl;
		  //done = true;
		}
	      userdata_double.Append(atof(version.c_str()));
	      break;

	    case 1:
	      // unit code (1=CM 2=MM 3=M 4=MIC 5=NM 6=FT 7=IN 8=MIL)
	      in >> unitcode;
	      cout << "unit code " << unitcode << endl;
	      userdata_int.Append(unitcode);
	      break;

	    case 2:
	      // Geometric coord "zero" tolerance threshold
	      in >> tolerance;
	      cout << "tolerance " << tolerance << endl;
	      userdata_double.Append(tolerance);
	      break;

	    case 3:
	      // Periodic UnitCell dS1 , dS2 , alphaDeg
	      in >> dS1 >> dS2 >> alphaDeg;
	      userdata_double.Append(dS1);
	      userdata_double.Append(dS2);
	      userdata_double.Append(alphaDeg);
	      break;

	    case 4:
	      // Periodic UnitCell origin in global coords (x3D,y3D,z3D)
	      in >> x3D >> y3D >> z3D;
	      userdata_double.Append(x3D);
	      userdata_double.Append(y3D);
	      userdata_double.Append(z3D);
	      break;

	    case 5:
	      // Model entity count: Vertices, Edges, Faces, Cells (Version 2)
	      in >> modelverts >> modeledges >> modelfaces >> modelcells;
	      userdata_int.Append(modelverts);
	      userdata_int.Append(modeledges);
	      userdata_int.Append(modelfaces);
	      userdata_int.Append(modelcells);
	      break;

	    case 6:
	      // Topological mesh-entity counts (#elements,#faces,#edges,#nodes)
	      in >> nelts >> nfaces >> nedges >> nnodes;
	      cout << nelts << " elements, " << nfaces << " faces, " << nedges << " edges, " << nnodes << " nodes" << endl;
	      mesh.SetAllocSize(nnodes,2*nedges,nfaces,nelts);
	      break;

	    case 7:
	      // NodeID, X, Y, Z, Type (0=Reg 1=PMaster 2=PSlave 3=CPMaster 4=CPSlave), PID:
	      {
		cout << "read nodes" << endl;
		for(int i=0; i<nnodes; i++)
		  {
		    in >> nodeid >> p.X() >> p.Y() >> p.Z() >> type >> pid;
		    mesh.AddPoint(p);		  
		    point_pids.Append(pid);
		    if(pid > maxId0D)
		      maxId0D = pid;
		    //(*testout) << "point " << p << " type " << type << " mastersexist " << mastersexist << endl;
		  }
	      }
	      break;

	    case 8:
	      // Number of Periodic Master Nodes
	      in >> nperiodicmasternodes;
	      break;

	    case 9:
	      // MasterNodeID, SlaveNodeID, TranslCode (1=dS1 2=dS2 3=dS1+dS2)
	      for(int i=0; i<nperiodicmasternodes; i++)
		{
		  for(int j=0; j<2; j++)
		    in >> dummyint;

		  in >> dummyint;
		}
	      break;

	    case 10:
	      // Number of Corner Periodic Master Nodes
	      in >> ncornerperiodicmasternodes;
	      break;

	    case 11:
	      // MasterNodeID, 3-SlaveNodeID's, 3-TranslCodes (1=dS1 2=dS2 3=dS1+dS2)
	      for(int i=0; i<ncornerperiodicmasternodes; i++)
		{
		  for(int j=0; j<4; j++)
		    in >> dummyint;

		  for(int j=0; j<3; j++)
		    in >> dummyint;
		}
	      break;

	    case 12:
	      // Number of Cubic Periodic Master Nodes
	      in >> ncubicperiodicmasternodes;
	      break;

	    case 13:
	      //MasterNodeID, 7-SlaveNodeID's, TranslCodes
	      for(int i=0; i<ncubicperiodicmasternodes; i++)
		{
		  for(int j=0; j<8; j++)
		    in >> dummyint;

		  for(int j=0; j<7; j++)
		    in >> dummyint;
		}
	      break;

	    case 14:
	      // EdgeID, NodeID0, NodeID1, Type (0=Reg 1=PMaster 2=PSlave 3=CPMaster 4=CPSlave), PID
	      cout << "read edges" << endl;
	      nullstarted = false;
	      segmentdata.SetSize(nedges);
	      for(int i=0; i<nedges; i++)
		{
		  segmentdata[i] = new ARRAY<int>(7);
		  *segmentdata[i] = -1;
		  in >> dummyint;
		  in >> (*segmentdata[i])[0] >> (*segmentdata[i])[1];
		  in >> type;
		  in >> (*segmentdata[i])[2];
		  if((*segmentdata[i])[2] > maxId1D)
		    maxId1D = (*segmentdata[i])[2];
		}
	      break;

	    case 15:
	      // Number of Periodic Master Edges
	      in >> nperiodicmasteredges;
	      break;

	    case 16:
	      // MasterEdgeID, SlaveEdgeID, TranslCode (1=dS1 2=dS2 3=dS1+dS2)
	      for(int i=0; i<nperiodicmasteredges; i++)
		in >> dummyint >> dummyint >> dummyint;
	      break;

	    case 17:
	      // Number of Corner Periodic Master Edges
	      in >> ncornerperiodicmasteredges;
	      break;

	    case 18:
	      // MasterEdgeID, 3 SlaveEdgeID's, 3 TranslCode (1=dS1 2=dS2 3=dS1+dS2)
	      for(int i=0; i<ncornerperiodicmasteredges; i++)
		{
		  in >> dummyint;
		  for(int j=0; j<3; j++)
		    in >> dummyint;
		  for(int j=0; j<3; j++)
		    in >> dummyint;
		}
	      break;

	    case 19:
	      // FaceID, EdgeID0, EdgeID1, EdgeID2, FaceType (0=Reg 1=PMaster 2=PSlave), PID
	      {
		//Segment seg;
		int segnum_ng[3];
		bool neg[3];
		cout << "read faces" << endl;
		nullstarted = false;
		for(int i=0; i<nfaces; i++)
		  {
		    int trinum;
		    int segnum;
		    
		    tris.Append(new Element2d(3));

		    in >> trinum;
		    for(int j=0; j<3; j++)
		      {
			in >> segnum;
			neg[j] = (segnum<0);
			if(!neg[j])
			  segnum_ng[j] = segnum-1;
			else
			  segnum_ng[j] = -segnum-1;
			
			if(neg[j])
			  tris.Last()->PNum(j+1) = (*segmentdata[segnum_ng[j]])[1];
			else
			  tris.Last()->PNum(j+1) = (*segmentdata[segnum_ng[j]])[0];

			tris.Last()->GeomInfoPi(j+1).trignum = trinum;
		      }
		    in >> type;
		    int faceid;
		    in >> faceid;
		    
		    if(faceid > maxId2D)
		      maxId2D = faceid;

		    if(i==0 || faceid < minId2D)
		      minId2D = faceid;
		    
		    tris.Last()->SetIndex(faceid);

		    if(faceid > 0)
		      {
			//if(nullstarted)
			//  {
			//    cout << "Faces: Assumption about index 0 wrong (face"<<trinum <<")" << endl;
			//  }
			//mesh.AddSurfaceElement(tri);
			
			for(int j=0; j<3; j++)
			  {
			    if(neg[j])
			      {
				(*segmentdata[segnum_ng[j]])[4] = faceid;
				(*segmentdata[segnum_ng[j]])[6] = trinum;
			      }
			    else
			      {
				(*segmentdata[segnum_ng[j]])[3] = faceid;
				(*segmentdata[segnum_ng[j]])[5] = trinum;
			      }
			  }
		      }
		    else
		      nullstarted = true;
		  }
	      }
	      break;

	    case 20:
	      // Number of Periodic Master Faces
	      in >> nperiodicmasterfaces;
	      break;

	    case 21:
	      // MasterFaceID, SlaveFaceID, TranslCode (1=dS1 2=dS2)
	      {
		Vec<3> randomvec(-1.32834,3.82399,0.5429151);
		int maxtransl = -1;
		for(int i=0; i<nperiodicmasterfaces; i++)
		  {
		    int tri1,tri2,transl;
		    ARRAY<PointIndex> nodes1(3),nodes2(3);
		    ARRAY<double> sortval1(3),sortval2(3);
		    in >> tri1 >> tri2 >> transl;

		    if(transl > maxtransl)
		      maxtransl = transl;
		    
		    
		    for(int j=0; j<3; j++)
		      {
			nodes1[j] = tris[tri1-1]->PNum(j+1);
			sortval1[j] = Vec<3>(mesh[nodes1[j]])*randomvec;
			nodes2[j] = tris[tri2-1]->PNum(j+1);
			sortval2[j] = Vec<3>(mesh[nodes2[j]])*randomvec;
		      }

		    BubbleSort(sortval1,nodes1);
		    BubbleSort(sortval2,nodes2);

		    for(int j=0; j<3; j++)
		      mesh.GetIdentifications().Add(nodes1[j],nodes2[j],transl);
			
		  }
		for(int i=1; i<= maxtransl; i++)
		  mesh.GetIdentifications().SetType(i,Identifications::PERIODIC);
	      }	      
	      break;

	    case 22:
	      // ElemID, FaceID0, FaceID1, FaceID2, FaceID3, PID
	      {
		cout << "read elements (1)" << endl;

		//SurfaceElementIndex surf[4];
		bool neg[4];
		int elemid;
		int domain;
		
		eldom.SetSize(nelts);

		for(int i=0; i<nelts; i++)
		  {
		    if(int(100.*i/nelts) % 5 == 0)
		      cout << int(100.*i/nelts)
#ifdef WIN32
			   << "%%\r"
#else
			   << "\%\r"
#endif 
			   << flush;
		    in >> elemid;
		    for(int j=0; j<4;j++)
		      {
			in >> dummyint;
			neg[j] = (dummyint < 0);
			if(neg[j])
			  tetfacedata.Append(-dummyint-1);
			//surf[j] = -dummyint-1;
			else
			  tetfacedata.Append(dummyint-1);
			tetfacedata.Append(((neg[j]) ? 1 : 0));
			//surf[j] = dummyint-1;
		      }
		    
		    in >> domain;
		    eldom[i] = domain;
		    tetfacedata.Append(domain);

		    if(i==0 || domain < minId3D)
		      minId3D = domain;

		    if(domain > maxId3D)
		      maxId3D = domain;
		    
// 		    for(int j=0; j<4; j++)
// 		      {
// 			if(mesh.GetNSE() <= surf[j])
// 			  continue;

// 			int faceind = 0;
// 			for(int k=1; k<=mesh.GetNFD(); k++)
// 			  {
// 			    if(mesh.GetFaceDescriptor(k).SurfNr() == mesh[surf[j]].GetIndex())
// 			      faceind = k;
// 			  }
// 			if(faceind)
// 			  {
// 			    if(neg[j])
// 			      mesh.GetFaceDescriptor(faceind).SetDomainOut(domain);
// 			    else
// 			      mesh.GetFaceDescriptor(faceind).SetDomainIn(domain);
// 			  }
// 			else
// 			  {
// 			    if(neg[j])
// 			      faceind = mesh.AddFaceDescriptor(FaceDescriptor(mesh[surf[j]].GetIndex(),0,domain,0));
// 			    else
// 			      faceind = mesh.AddFaceDescriptor(FaceDescriptor(mesh[surf[j]].GetIndex(),domain,0,0));
// 			    mesh.GetFaceDescriptor(faceind).SetBCProperty(mesh[surf[j]].GetIndex());
// 			  }
// 		      }
		  }
		cout << endl;
		
		
// 		ARRAY<int> indextodescriptor(maxId2D+1);
		
// 		for(int i=1; i<=mesh.GetNFD(); i++)
// 		  indextodescriptor[mesh.GetFaceDescriptor(i).SurfNr()] = i;
		
		
// 		for(SurfaceElementIndex i=0; i<mesh.GetNSE(); i++)
// 		  mesh[i].SetIndex(indextodescriptor[mesh[i].GetIndex()]);
	      }
	      break;

	    case 23:
	      // ElemID, NodeID0, NodeID1, NodeID2, NodeID3
	      { 
		cout << "read elements (2)" << endl;
		Element el(TET);
		for(ElementIndex i=0; i<nelts; i++)
		  {
		    in >> dummyint;
		    for(int j=1; j<=4; j++)
		      in >> el.PNum(j);
		    swap(el.PNum(1),el.PNum(2));
		    
		    el.SetIndex(eldom[i]);
		    mesh.AddVolumeElement(el);
		  }	
	      }	  
	      break;
	      
	    case 24:
	      // Physical Object counts (#Obj3D,#Obj2D,#Obj1D,#Obj0D)
	      {
		in >> numObj3D;
		userdata_int.Append(numObj3D);
		in >> numObj2D;
		userdata_int.Append(numObj2D);
		in >> numObj1D;
		userdata_int.Append(numObj1D);
		in >> numObj0D;
		userdata_int.Append(numObj0D);
	      }
	      break;

	    case 25:
	      // Number of Ports (Ports are a subset of Object2D list)
	      {
		in >> dummyint;
		//userdata_int.Append(dummyint);
	      }
	      break;

	    case 26:
	      // Object3D GroupID, #Elems <immediately followed by> ElemID List
	      {
		uid_to_group_3D.SetSize(maxId3D+1);
		uid_to_group_3D = -1;
		for(int i=0; i<numObj3D; i++)
		  {
		    int groupid;
		    in >> groupid;
		    (*testout) << "3d groupid " << groupid << endl;
		    //userdata_int.Append(groupid);
		    int nelems;
		    in >> nelems;
		    //userdata_int.Append(nelems);
		    for(int j=0; j<nelems; j++)
		      {
			in >> dummyint;
			
			(*testout) << "read " << dummyint << endl;
			//userdata_int.Append(dummyint);
			
			if(dummyint < 0) 
			  dummyint *= -1;
			uid_to_group_3D[eldom[dummyint-1]] = groupid;
		      }
		  }
	      }
	      break;

	    case 27:
	      // Object2D GroupID, #Faces <immediately followed by> FaceID List
	      {
		ARRAY<int> ports;
		//int totnum = 0;
		uid_to_group_2D.SetSize(maxId2D+1);
		uid_to_group_2D = -1;

		for(int i=0; i<numObj2D; i++)
		  {
		    int groupid;
		    in >> groupid;
		    (*testout) << "2d groupid " << groupid << endl;
		    //userdata_int.Append(groupid);
		    int nelems;
		    in >> nelems;
		    //userdata_int.Append(nelems);
		    for(int j=0; j<nelems; j++)
		      {
			in >> dummyint;
			char port;
			while((port = in.get()) == ' ')
			  ;

			(*testout) << "read " << dummyint << endl;
			if(dummyint < 0) 
			  dummyint *= -1;
			int uid = tris[dummyint-1]->GetIndex();

			if(port == 'P' || port == 'p')
			  {
			    if(!ports.Contains(uid))
			      ports.Append(uid);
			  }
			else
			  in.putback(port);
			
			//userdata_int.Append(dummyint);
			
			uid_to_group_2D[uid] = groupid;
			(*testout) << "setting " << uid << endl;

			//totnum++;
		      }
		  }
		mesh.SetUserData("TETmesh:ports",ports);
	      }
	      break;

	    case 28:
	      // Object1D GroupID, #Edges <immediately followed by> EdgeID List
	      {
		uid_to_group_1D.SetSize(maxId1D+1);
		uid_to_group_1D = -1;

		for(int i=0; i<numObj1D; i++)
		  {
		    int groupid;
		    in >> groupid;
		    //userdata_int.Append(groupid);
		    int nelems;
		    in >> nelems;
		    //userdata_int.Append(nelems);
		    for(int j=0; j<nelems; j++)
		      {
			in >> dummyint;
			//userdata_int.Append(dummyint);

			if(dummyint < 0) 
			  dummyint *= -1;
			uid_to_group_1D[(*segmentdata[dummyint-1])[2]] = groupid;
		      }
		  }
	      }
	      break;

	    case 29:
	      // Object0D GroupID, #Nodes <immediately followed by> NodeID List
	      {
		uid_to_group_0D.SetSize(maxId0D+1);
		uid_to_group_0D = -1;
		for(int i=0; i<numObj0D; i++)
		  {
		    int groupid;
		    in >> groupid;
		    //userdata_int.Append(groupid);
		    int nelems;
		    in >> nelems;
		    //userdata_int.Append(nelems);
		    for(int j=0; j<nelems; j++)
		      {
			in >> dummyint;
			//userdata_int.Append(dummyint);

			if(dummyint < 0) 
			  dummyint *= -1;
			uid_to_group_0D[point_pids[dummyint-1]] = groupid;
		      }
		  }
	      }
	      break;



	    default:
	      done = true;
	      
	    }
	  
	  if(inputsection == 4 && version == "1.1")
	    inputsection++;

	  inputsection++;
	}
      in.close();


      mesh.SetUserData("TETmesh:double",userdata_double);
      userdata_int.Append(minId2D);
      userdata_int.Append(minId3D);
      mesh.SetUserData("TETmesh:int",userdata_int);   
      //if(version == "1.1")
      mesh.SetUserData("TETmesh:point_id",point_pids);

      mesh.SetUserData("TETmesh:uid_to_group_3D",uid_to_group_3D);
      mesh.SetUserData("TETmesh:uid_to_group_2D",uid_to_group_2D);
      mesh.SetUserData("TETmesh:uid_to_group_1D",uid_to_group_1D);
      mesh.SetUserData("TETmesh:uid_to_group_0D",uid_to_group_0D);


      ARRAY<SurfaceElementIndex> surfindices(tris.Size());
      surfindices = -1;

      for(int i=0; i<tris.Size(); i++)
	{
	  if(atof(version.c_str()) <= 1.999999)
	    {
	      if(tris[i]->GetIndex() > 0)
		surfindices[i] = mesh.AddSurfaceElement(*tris[i]);
	    }
	  else
	    {
	      if(tris[i]->GetIndex() > 0 &&
		 tris[i]->GetIndex() < minId3D)
		{
		  tris[i]->SetIndex(tris[i]->GetIndex()-minId2D+1);
		  surfindices[i] = mesh.AddSurfaceElement(*tris[i]);
		}
	    }
	  delete tris[i];
	}

      
      mesh.ClearFaceDescriptors();
      if(atof(version.c_str()) <= 1.999999)
	for(int i = 1; i <= maxId2D; i++)
	  mesh.AddFaceDescriptor(FaceDescriptor(i,0,0,0));
      else
	for(int i=minId2D; i<minId3D; i++)
	  mesh.AddFaceDescriptor(FaceDescriptor(i,0,0,0));
	

      for(int i=0; i<tetfacedata.Size(); i+=9)
	{
	  for(int j=0; j<4; j++)
	    {
	      SurfaceElementIndex surf = surfindices[tetfacedata[i+2*j]];
	      
	      //if(mesh.GetNSE() <= surf)
	      if(surf == -1)
		continue;

	      if(tetfacedata[i+2*j+1] == 1)
		mesh.GetFaceDescriptor(mesh[surf].GetIndex()).SetDomainOut(tetfacedata[i+8]);
	      else
		mesh.GetFaceDescriptor(mesh[surf].GetIndex()).SetDomainIn(tetfacedata[i+8]);
			

	      /*
	      int faceind = 0;
	      for(int k=1; k<=mesh.GetNFD(); k++)
		{
		  if(mesh.GetFaceDescriptor(k).SurfNr() == mesh[surf].GetIndex())
		    faceind = k;
		}
	      if(faceind)
		{
		  if(tetfacedata[i+4+j] == 1)
		    mesh.GetFaceDescriptor(faceind).SetDomainOut(tetfacedata[i+8]);
		  else
		    mesh.GetFaceDescriptor(faceind).SetDomainIn(tetfacedata[i+8]);
		}
	      else
		{
		  if(tetfacedata[i+4+j] == 1)
		    faceind = mesh.AddFaceDescriptor(FaceDescriptor(mesh[surf].GetIndex(),0,tetfacedata[i+8],0));
		  else
		    faceind = mesh.AddFaceDescriptor(FaceDescriptor(mesh[surf].GetIndex(),tetfacedata[i+8],0,0));
		  mesh.GetFaceDescriptor(faceind).SetBCProperty(mesh[surf].GetIndex());
		}
	      */
	    }

	}
      
//       ARRAY<int> indextodescriptor(maxId2D+1);
		
//       for(int i=1; i<=mesh.GetNFD(); i++)
// 	indextodescriptor[mesh.GetFaceDescriptor(i).SurfNr()] = i;
		
		
//       for(SurfaceElementIndex i=0; i<mesh.GetNSE(); i++)
// 	mesh[i].SetIndex(indextodescriptor[mesh[i].GetIndex()]);


      for(int i=0; i<segmentdata.Size(); i++)
	{
	  Segment seg;

	  
	  if((atof(version.c_str()) <= 1.999999 && (*segmentdata[i])[2] > 0) ||
	     (atof(version.c_str()) > 1.999999  && (*segmentdata[i])[2] > 0 && (*segmentdata[i])[2] < minId2D))
	    {
	      seg.p1 = (*segmentdata[i])[0];
	      seg.p2 = (*segmentdata[i])[1];
	      seg.edgenr = (*segmentdata[i])[2];
	      seg.epgeominfo[0].edgenr = (*segmentdata[i])[2];
	      seg.epgeominfo[1].edgenr = (*segmentdata[i])[2];
	      seg.si = (*segmentdata[i])[3]-minId2D+1;
	      seg.surfnr1 = -1;//(*segmentdata[i])[3];
	      seg.surfnr2 = -1;//(*segmentdata[i])[4];
	      seg.geominfo[0].trignum = (*segmentdata[i])[5];
	      seg.geominfo[1].trignum = (*segmentdata[i])[5];
	      mesh.AddSegment(seg);

	      seg.p1 = (*segmentdata[i])[1];
	      seg.p2 = (*segmentdata[i])[0];
	      seg.si = (*segmentdata[i])[4]-minId2D+1;
	      seg.surfnr1 = -1;//(*segmentdata[i])[3];
	      seg.surfnr2 = -1;//(*segmentdata[i])[4];
	      seg.geominfo[0].trignum = (*segmentdata[i])[6];
	      seg.geominfo[1].trignum = (*segmentdata[i])[6];
	      mesh.AddSegment(seg);
	    }
	  delete segmentdata[i];
	}

      /*
      for(int i=mesh.GetNSeg(); i>=1; i--)
	if(mesh.LineSegment(i).epgeominfo[0].edgenr == 0 ||
	   mesh.LineSegment(i).epgeominfo[1].edgenr == 0)
	  mesh.FullDeleteSegment(i);
      */	
  
      mesh.CalcSurfacesOfNode();
      
    }

}

}
