/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) LGPL  Joachim Schoeberl      Date:   7. May. 2000                                                   

     Copyright (c) 2003-2007 René Heinzl         heinzl@iue.tuwien.ac.at
     Copyright (c) 2004-2007 Philipp Schwaha
     Copyright (c) 2007      Franz Stimpfl

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#include <mystdlib.h>
#include <myadt.hpp>

#include <linalg.hpp>
#include <csg.hpp>
#include <stlgeom.hpp>
#include <geometry2d.hpp>
#include <meshing.hpp>


namespace vgmodeler {
#include "nglib.h"
}

using namespace vgmodeler;

// constants and types:

namespace vgmodeler
{
// initialize, deconstruct Netgen library:
void Ng_Init ()
{
//   mycout = &cout;
//   myerr = &cerr;
//   testout = new ofstream ("test.out");
//   mycout = new ofstream ("/dev/null");
//   myerr = new ofstream ("/dev/null");
//   testout = new ofstream ("/dev/null");

}

void Ng_Exit ()
{
  ;
}
  


Ng_Mesh * Ng_NewMesh ()
{
  Mesh * mesh = new Mesh;  
  mesh->AddFaceDescriptor (FaceDescriptor (1, 1, 0, 1));
  return (Ng_Mesh*)(void*)mesh;
}

void Ng_DeleteMesh (Ng_Mesh * mesh)
{
  delete (Mesh*)mesh;
}


// feeds points, surface elements and volume elements to the mesh
void Ng_AddPoint (Ng_Mesh * mesh, double * x)
{
  Mesh * m = (Mesh*)mesh;
  m->AddPoint (Point3d (x[0], x[1], x[2]));
}

void Ng_add_point_surface (Ng_Mesh * mesh, double * x)
{
  Mesh * m = (Mesh*)mesh;
  m->AddPoint (Point3d (x[0], x[1], x[2]), 1, SURFACEPOINT);
}

  
void Ng_AddSurfaceElement (Ng_Mesh * mesh, Ng_Surface_Element_Type et,
			   int * pi)
{
  Mesh * m = (Mesh*)mesh;
  Element2d el (3);
  el.SetIndex (1);
  el.PNum(1) = pi[0];
  el.PNum(2) = pi[1];
  el.PNum(3) = pi[2];
  m->AddSurfaceElement (el);
}

void Ng_AddVolumeElement (Ng_Mesh * mesh, Ng_Volume_Element_Type et,
			  int * pi)
{
  Mesh * m = (Mesh*)mesh;
  Element el (4);
  el.SetIndex (1);
  el.PNum(1) = pi[0];
  el.PNum(2) = pi[1];
  el.PNum(3) = pi[2];
  el.PNum(4) = pi[3];
  m->AddVolumeElement (el);
}

// ask for number of points, surface and volume elements
int Ng_GetNP (Ng_Mesh * mesh)
{
  return ((Mesh*)mesh) -> GetNP();
}

int Ng_GetNSE (Ng_Mesh * mesh)
{
  return ((Mesh*)mesh) -> GetNSE();
}

int Ng_GetNE (Ng_Mesh * mesh)
{
  return ((Mesh*)mesh) -> GetNE();
}

int Ng_GetNumberOfSegments (Ng_Mesh * mesh)
{
  return ((Mesh*)mesh) -> GetNSeg();
}



//  return point coordinates
void Ng_GetPoint (Ng_Mesh * mesh, int num, double * x)
{
  const Point3d & p = ((Mesh*)mesh)->Point(num);
  x[0] = p.X();
  x[1] = p.Y();
  x[2] = p.Z();
}

// return surface and volume element in pi
Ng_Surface_Element_Type 
Ng_GetSurfaceElement (Ng_Mesh * mesh, int num, int * pi)
{
  const Element2d & el = ((Mesh*)mesh)->SurfaceElement(num);
  for (int i = 1; i <= el.GetNP(); i++)
    pi[i-1] = el.PNum(i);
  Ng_Surface_Element_Type et;
  switch (el.GetNP())
    {
    case 3: et = NG_TRIG; break;
    case 4: et = NG_QUAD; break;
    case 6: et = NG_TRIG6; break;
    }
  return et;
}

Ng_Volume_Element_Type
Ng_GetVolumeElement (Ng_Mesh * mesh, int num, int * pi)
{
  const Element & el = ((Mesh*)mesh)->VolumeElement(num);
  for (int i = 1; i <= el.GetNP(); i++)
    {
    pi[i-1] = el.PNum(i);
    //std::cout << "  pi: " << el.PNum(i) << std::endl;
    }
  Ng_Volume_Element_Type et;
  switch (el.GetNP())
    {
    case 4: et = NG_TET; break;
    case 5: et = NG_PYRAMID; break;
    case 6: et = NG_PRISM; break;
    case 10: et = NG_TET10; break;
    }
  return et;
}



// generates volume mesh from surface mesh
Ng_Result Ng_GenerateVolumeMesh (Ng_Mesh * mesh, Ng_Meshing_Parameters * mp)
{
  Mesh * m = (Mesh*)mesh;
  
  
  MeshingParameters mparam;
  mparam.maxh = mp->maxh;
  mparam.meshsizefilename = mp->meshsize_filename;

  std::cout << " ## Calculating local feature size from input.. " << std::endl;
  //m->CalcLocalHFromPointDistances();
  m->CalcLocalH();

  MeshVolume (mparam, *m);
//  RemoveIllegalElements (*m);
//  OptimizeVolume (mparam, *m);

  return NG_OK;
}



// 2D Meshing Functions:

void Ng_ClearVolumeElements(Ng_Mesh *mesh)
 {
  Mesh * m = (Mesh*)mesh;

  m->ClearVolumeElements();
 }

void Ng_AddPoint_2D (Ng_Mesh * mesh, double * x)
{
  Mesh * m = (Mesh*)mesh;
  
  m->AddPoint (Point3d (x[0], x[1], 0));
}

void Ng_AddBoundarySeg_2D (Ng_Mesh * mesh, int pi1, int pi2)
{
  Mesh * m = (Mesh*)mesh;

  Segment seg;
  seg.p1 = pi1;
  seg.p2 = pi2;
  m->AddSegment (seg);
}
  

int Ng_GetNP_2D (Ng_Mesh * mesh)
{
  Mesh * m = (Mesh*)mesh;
  return m->GetNP();
}

int Ng_GetNE_2D (Ng_Mesh * mesh)
{
  Mesh * m = (Mesh*)mesh;
  return m->GetNSE();
}

int Ng_GetNSeg_2D (Ng_Mesh * mesh)
{
  Mesh * m = (Mesh*)mesh;
  return m->GetNSeg();
}
  
void Ng_GetPoint_2D (Ng_Mesh * mesh, int num, double * x)
{
  Mesh * m = (Mesh*)mesh;

  Point3d & p = m->Point(num);
  x[0] = p.X();
  x[1] = p.Y();
}

void Ng_GetElement_2D (Ng_Mesh * mesh, int num, int * pi, int * matnum)
{
  const Element2d & el = ((Mesh*)mesh)->SurfaceElement(num);
  for (int i = 1; i <= 3; i++)
    pi[i-1] = el.PNum(i);
  if (matnum)
    *matnum = el.GetIndex();
}


void Ng_GetSegment_2D (Ng_Mesh * mesh, int num, int * pi, int * matnum)
{
  const Segment & seg = ((Mesh*)mesh)->LineSegment(num);
  pi[0] = seg.p1;
  pi[1] = seg.p2;

  if (matnum)
    *matnum = seg.edgenr;
}




Ng_Geometry_2D * Ng_LoadGeometry_2D (const char * filename)
{
  SplineGeometry2d * geom = new SplineGeometry2d();
  geom -> Load (filename);
  return (Ng_Geometry_2D *)geom;
}

Ng_Result Ng_GenerateMesh_2D (Ng_Geometry_2D * geom,
			      Ng_Mesh ** mesh,
			      Ng_Meshing_Parameters * mp)
{
  // use global variable mparam
  //  MeshingParameters mparam;  
  mparam.maxh = mp->maxh;
  mparam.meshsizefilename = mp->meshsize_filename;
  mparam.quad = mp->quad_dominated;

  Mesh * m;
  MeshFromSpline2D (*(SplineGeometry2d*)geom, m, mparam);
  
  cout << m->GetNSE() << " elements, " << m->GetNP() << " points" << endl;
  
  *mesh = (Ng_Mesh*)m;
  return NG_OK;
}

void Ng_HP_Refinement (Ng_Geometry_2D * geom,
		       Ng_Mesh * mesh,
		       int levels)
{
  Refinement2d ref(*(SplineGeometry2d*)geom);
  HPRefinement (*(Mesh*)mesh, &ref, levels);
}

void Ng_HP_Refinement (Ng_Geometry_2D * geom,
		       Ng_Mesh * mesh,
		       int levels, double parameter)
{
  Refinement2d ref(*(SplineGeometry2d*)geom);
  HPRefinement (*(Mesh*)mesh, &ref, levels, parameter);
}






// ======= STL part ===== .. [RH] modified







// [RH] .. ;-( global STL element container
//
ARRAY<STLReadTriangle> readtrias; //only before initstlgeometry
ARRAY<Point<3> > readedges; //only before init stlgeometry
 
void Ng_SaveMesh(Ng_Mesh * mesh, const char* filename)
{
  ((Mesh*)mesh)->Save(filename);
}

Ng_Mesh * Ng_LoadMesh(const char* filename)
{
  Mesh * mesh = new Mesh;
  mesh->Load(filename);
  return ( (Ng_Mesh*)mesh );
}

// loads geometry from STL file
Ng_STL_Geometry * Ng_STL_LoadGeometry (const char * filename, int binary)
{
  int i;
  STLGeometry geom;
  STLGeometry* geo;
  ifstream ist(filename);

  if (binary)
    {
      geo = geom.LoadBinary(ist);
    }
  else
    {
      geo = geom.Load(ist);
    }

  readtrias.SetSize(0);
  readedges.SetSize(0);

  Point3d p;
  Vec3d normal;
  double p1[3];
  double p2[3];
  double p3[3];
  double n[3];

  Ng_STL_Geometry * geo2 = Ng_STL_NewGeometry();

  for (i = 1; i <= geo->GetNT(); i++)
    {
      const STLTriangle& t = geo->GetTriangle(i);
      p = geo->GetPoint(t.PNum(1));
      p1[0] = p.X(); p1[1] = p.Y(); p1[2] = p.Z(); 
      p = geo->GetPoint(t.PNum(2));
      p2[0] = p.X(); p2[1] = p.Y(); p2[2] = p.Z(); 
      p = geo->GetPoint(t.PNum(3));
      p3[0] = p.X(); p3[1] = p.Y(); p3[2] = p.Z();
      normal = t.Normal();
      n[0] = normal.X(); n[1] = normal.Y(); n[2] = normal.Z();
      
      Ng_STL_AddTriangle(geo2, p1, p2, p3, n);
    }

  return geo2;
}

// loads geometry from STL file
Ng_STL_Geometry * Ng_STL_LoadGeometryMultiple (const char * filename, int binary)
{
  int i;
  STLGeometry geom;
  STLGeometry* geo;
  ifstream ist(filename);

  if (binary)
    {
      geo = geom.LoadBinary(ist);
    }
  else
    {
      geo = geom.Load_rh(ist);
    }

  readtrias.SetSize(0);
  readedges.SetSize(0);

  Point3d p;
  Vec3d normal;
  double p1[3];
  double p2[3];
  double p3[3];
  double n[3];

  Ng_STL_Geometry * geo2 = Ng_STL_NewGeometry();

  for (i = 1; i <= geo->GetNT(); i++)
    {
      const STLTriangle& t = geo->GetTriangle(i);
      p = geo->GetPoint(t.PNum(1));
      p1[0] = p.X(); p1[1] = p.Y(); p1[2] = p.Z(); 
      p = geo->GetPoint(t.PNum(2));
      p2[0] = p.X(); p2[1] = p.Y(); p2[2] = p.Z(); 
      p = geo->GetPoint(t.PNum(3));
      p3[0] = p.X(); p3[1] = p.Y(); p3[2] = p.Z();
      normal = t.Normal();
      n[0] = normal.X(); n[1] = normal.Y(); n[2] = normal.Z();
      
      std::cout << "### mat1: " << t.Domain(0) << "  mat2: "<< t.Domain(1) << std::endl;
      Ng_STL_AddTriangle_rh(geo2, p1, p2, p3, n, t.Domain(0), t.Domain(1));
    }

  return geo2;
}






// generate new STL Geometry
Ng_STL_Geometry * Ng_STL_NewGeometry ()
{
  return (Ng_STL_Geometry*)(void*)new STLGeometry;
} 

// after adding triangles (and edges) initialize
Ng_Result Ng_STL_InitSTLGeometry (Ng_STL_Geometry * geom)
{
  STLGeometry* geo = (STLGeometry*)geom;


  geo->InitSTLGeometry(readtrias);
  readtrias.SetSize(0);
  if (readedges.Size() != 0)
    {
      int i;
      /*
      for (i = 1; i <= readedges.Size(); i+=2)
	{
	  cout << "e(" << readedges.Get(i) << "," << readedges.Get(i+1) << ")" << endl;
	}
      */
      geo->AddEdges(readedges);
    }


  //std::cout << "stl doctor.. " << std::endl;
  //geo->SetSelectTrig(1);
  //geo->OrientAfterTrig(geo->GetSelectTrig());
  //geo->FindNeighbourTrigs();
  //std::cout << "stl doctor.. " << std::endl;

  geo->Save("testme2.stl");
  //geo->TopologyChanged(); //do some things, if topology changed!
  //geo->FindNeighbourTrigs();


  if (geo->GetStatus() == STLTopology::STL_GOOD || geo->GetStatus() == STLTopology::STL_WARNING) return NG_OK;
  return NG_SURFACE_INPUT_ERROR;
}

// automatically generates edges:
//
Ng_Result Ng_STL_MakeEdges (Ng_STL_Geometry * geom,
		       Ng_Mesh* mesh,
		       Ng_Meshing_Parameters * mp)
{
  STLGeometry* stlgeometry = (STLGeometry*)geom;
  Mesh* me = (Mesh*)mesh;
  
  MeshingParameters mparam;

  mparam.maxh = mp->maxh;
  mparam.meshsizefilename = mp->meshsize_filename;

  me -> SetGlobalH (mparam.maxh);
  me -> SetLocalH (stlgeometry->GetBoundingBox().PMin() - Vec3d(10, 10, 10),
		   stlgeometry->GetBoundingBox().PMax() + Vec3d(10, 10, 10),
		   0.3);

  me -> LoadLocalMeshSize (mp->meshsize_filename);
  /*
  if (mp->meshsize_filename)
    {
      ifstream infile (mp->meshsize_filename);
      if (!infile.good()) return NG_FILE_NOT_FOUND;
      me -> LoadLocalMeshSize (infile);
    }
  */

  STLMeshing (*stlgeometry, *me);
  
  stlgeometry->edgesfound = 1;
  stlgeometry->surfacemeshed = 0;
  stlgeometry->surfaceoptimized = 0;
  stlgeometry->volumemeshed = 0;


  
  return NG_OK;
}

  
// generates mesh, empty mesh be already created.
//
Ng_Result Ng_STL_GenerateSurfaceMesh (Ng_STL_Geometry * geom,
				      Ng_Mesh* mesh,
				      Ng_Meshing_Parameters * mp)
{
  STLGeometry* stlgeometry = (STLGeometry*)geom;
  Mesh* me = (Mesh*)mesh;




  MeshingParameters mparam;

  mparam.maxh = mp->maxh;
  mparam.meshsizefilename = mp->meshsize_filename;

  /*
  me -> SetGlobalH (mparam.maxh);
  me -> SetLocalH (stlgeometry->GetBoundingBox().PMin() - Vec3d(10, 10, 10),
		   stlgeometry->GetBoundingBox().PMax() + Vec3d(10, 10, 10),
		   0.3);
  */
  /*
  STLMeshing (*stlgeometry, *me);
  
  stlgeometry->edgesfound = 1;
  stlgeometry->surfacemeshed = 0;
  stlgeometry->surfaceoptimized = 0;
  stlgeometry->volumemeshed = 0;
  */  



  int retval = STLSurfaceMeshing (*stlgeometry, *me);
  if (retval == MESHING3_OK)
    {
      std::cout << "Success !!!!" << endl;
      stlgeometry->surfacemeshed = 1;
      stlgeometry->surfaceoptimized = 0;
      stlgeometry->volumemeshed = 0;
    } 
  else if (retval == MESHING3_OUTERSTEPSEXCEEDED)
    {
      std::cout << "ERROR: Give up because of too many trials. Meshing aborted!" << endl;
    }
  else if (retval == MESHING3_TERMINATE)
    {
      std::cout << "Meshing Stopped!" << endl;
    }
  else
    {
      std::cout << "ERROR: Surface meshing not successful. Meshing aborted!" << endl;
    }


  STLSurfaceOptimization (*stlgeometry, *me, mparam);

  return NG_OK;
}


  // fills STL Geometry
  // positive orientation
  // normal vector may be null-pointer
void Ng_STL_AddTriangle (Ng_STL_Geometry * geom, 
			 double * p1, double * p2, double * p3, double * nv)
{
  Point<3> apts[3];
  apts[0] = Point<3>(p1[0],p1[1],p1[2]);
  apts[1] = Point<3>(p2[0],p2[1],p2[2]);
  apts[2] = Point<3>(p3[0],p3[1],p3[2]);

  Vec<3> n;
  if (!nv)
    n = Cross (apts[0]-apts[1], apts[0]-apts[2]);
  else
    n = Vec<3>(nv[0],nv[1],nv[2]);

  readtrias.Append(STLReadTriangle(apts,n));
}



  // fills STL Geometry
  // positive orientation
  // normal vector may be null-pointer
void Ng_STL_AddTriangle_rh (Ng_STL_Geometry * geom, 
			 double * p1, double * p2, double * p3, double * nv, long mat1, long mat2)
{
  Point<3> apts[3];
  apts[0] = Point<3>(p1[0],p1[1],p1[2]);
  apts[1] = Point<3>(p2[0],p2[1],p2[2]);
  apts[2] = Point<3>(p3[0],p3[1],p3[2]);

  Vec<3> n;
  if (!nv)
    n = Cross (apts[0]-apts[1], apts[0]-apts[2]);
  else
    n = Vec<3>(nv[0],nv[1],nv[2]);

  readtrias.Append(STLReadTriangle(apts,n, mat1, mat2));
}



  // add (optional) edges:
void Ng_STL_AddEdge (Ng_STL_Geometry * geom, 
		     double * p1, double * p2)
{
  readedges.Append(Point3d(p1[0],p1[1],p1[2]));
  readedges.Append(Point3d(p2[0],p2[1],p2[2]));
}



Ng_Meshing_Parameters :: Ng_Meshing_Parameters()
{
  maxh = 1000;
  fineness = 0.5;
  secondorder = 0;
  meshsize_filename = 0;
  quad_dominated = 0;
}

// [RH] transition from nginterface
//
void Ng_SetRefinementFlag (Ng_Mesh *mesh, int ei, int flag)
{
  if (((Mesh*)mesh)->GetDimension() == 3)
    {
      ((Mesh*)mesh)->VolumeElement(ei).SetRefinementFlag (flag != 0);
      ((Mesh*)mesh)->VolumeElement(ei).SetStrongRefinementFlag (flag >= 10);
    }
  else
    {
      ((Mesh*)mesh)->SurfaceElement(ei).SetRefinementFlag (flag != 0);
      ((Mesh*)mesh)->SurfaceElement(ei).SetStrongRefinementFlag (flag >= 10);
    }
}

void Ng_set_surface_element (Ng_Mesh *mesh, int ei, int flag)
{
  if (((Mesh*)mesh)->GetDimension() == 3)
    {
      ((Mesh*)mesh)->SurfaceElement(ei).SetRefinementFlag (flag != 0);
      ((Mesh*)mesh)->SurfaceElement(ei).SetStrongRefinementFlag (flag >= 10);
    }
}


void Ng_Refine (Ng_Mesh *mesh, NG_REFINEMENT_TYPE reftype)
{
  BisectionOptions biopt;
  biopt.usemarkedelements = 1;
  biopt.refine_p = 0;
  biopt.refine_hp = 0;
//   if (reftype == NG_REFINE_P)
//     biopt.refine_p = 1;
//   if (reftype == NG_REFINE_HP)
//     biopt.refine_hp = 1;


  Refinement * ref;
  ref =new Refinement();
  // RH tetrahedra bisection
  //
  ref -> Bisect (*((Mesh*)mesh), biopt);

  // RH .. another refinement method
  //
  //ref -> Refine (*((Mesh*)mesh));


  ((Mesh*)mesh) -> UpdateTopology();
  delete ref;
}


void Ng_RestrictMeshSizePoint (Ng_Mesh * mesh, double * p, double h)
{
  ((Mesh*)mesh)->RestrictLocalH(Point3d (p[0], p[1], p[2]), h);
  ((Mesh*)mesh)->SetGlobalH( h);

  }




void Ng_GetCellOnVertex(Ng_Mesh *mesh, int vertex_number, int **element_container, int& number_of_elements)
{
  ARRAY<int> element_container_local;    
  ((Mesh*)mesh)->get_elements_on_vertex(vertex_number, element_container_local);

  int *element_c_temp;
  number_of_elements = element_container_local.Size();
  element_c_temp = (int*)realloc (*element_container, number_of_elements* sizeof(int));
  if (element_c_temp !=0)
    *element_container = element_c_temp;
  else
    {
      element_container = NULL;
      return;
    }
  for (int i = 0; i < element_container_local.Size(); ++i)
    {
      //std::cout << "out: "<< element_container_local[i] << std::endl;
      (*element_container)[i] = element_container_local[i];
    }
}

} // namespace end....





// compatibility functions:

namespace vgmodeler 
{

  char geomfilename[255];

// void MyError (const char * ch)
// {
//   cerr << ch;
// }

//Destination for messages, errors, ...
void Ng_PrintDest(const char * s)
{
  std::cout << s << flush;
}

double GetTime ()
{
  return 0;
}

void ResetTime ()
{
  ;
}

void MyBeep (int i)
{
  ;
}

}

