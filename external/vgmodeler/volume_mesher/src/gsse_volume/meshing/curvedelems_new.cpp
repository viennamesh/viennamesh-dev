#ifdef CURVEDELEMS_NEW
#include <mystdlib.h>

#include "meshing.hpp"

namespace netgen
{

   void ComputeGaussRule (int n, ARRAY<double> & xi, ARRAY<double> & wi)
  {
    xi.SetSize (n);
    wi.SetSize (n);
    
    int m = (n+1)/2;
    double p1, p2, p3;
    double pp, z, z1;
    for (int i = 1; i <= m; i++)
      {
	z = cos ( M_PI * (i - 0.25) / (n + 0.5));
	while(1)
	  {
	    p1 = 1; p2 = 0;
	    for (int j = 1; j <= n; j++)
	      {
		p3 = p2; p2 = p1;
		p1 = ((2 * j - 1) * z * p2 - (j - 1) * p3) / j;
	      }
	    // p1 is legendre polynomial
	    
	    pp = n * (z*p1-p2) / (z*z - 1);
	    z1 = z;
	    z = z1-p1/pp;
	    
	    if (fabs (z - z1) < 1e-14) break;
	  }
	
	xi[i-1] = 0.5 * (1 - z);
	xi[n-i] = 0.5 * (1 + z);
	wi[i-1] = wi[n-i] = 1.0 / ( (1  - z * z) * pp * pp);
      }
  }
  
  

  // compute edge bubbles up to order n, x \in (-1, 1)
   void CalcEdgeShape (int n, double x, double * shape)
  {
    double p1 = x, p2 = -1, p3 = 0;
    for (int j=2; j<=n; j++)
      {
	p3=p2; p2=p1;
	p1=( (2*j-3) * x * p2 - (j-3) * p3) / j;
	shape[j-2] = p1;
      }    
  }

   void CalcEdgeShapeDx (int n, double x, double * shape, double * dshape)
  {
    double p1 = x, p2 = -1, p3 = 0;
    double p1dx = 1, p2dx = 0, p3dx = 0;

    for (int j=2; j<=n; j++)
      {
	p3=p2; p2=p1;
	p3dx = p2dx; p2dx = p1dx;

	p1=( (2*j-3) * x * p2 - (j-3) * p3) / j;
	p1dx = ( (2*j-3) * (x * p2dx + p2) - (j-3) * p3dx) / j;

	shape[j-2] = p1;
	dshape[j-2] = p1dx;
      }    
  }

  // compute L_i(x/t) * t^i
   void CalcScaledEdgeShape (int n, double x, double t, double * shape)
  {
    double p1 = x, p2 = -1, p3 = 0;
    for (int j=0; j<=n-2; j++)
      {
	p3=p2; p2=p1;
	p1=( (2*j+1) * x * p2 - t*t*(j-1) * p3) / (j+2);
	shape[j] = p1;
      }    
  }

  template <int DIST>
   void CalcScaledEdgeShapeDxDt (int n, double x, double t, double * dshape)
  {
    double p1 = x, p2 = -1, p3 = 0;
    double p1dx = 1, p1dt = 0;
    double p2dx = 0, p2dt = 0;
    double p3dx = 0, p3dt = 0;
     
    for (int j=0; j<=n-2; j++)
      {
	p3=p2; p3dx=p2dx; p3dt = p2dt;
	p2=p1; p2dx=p1dx; p2dt = p1dt;

	p1   = ( (2*j+1) * x * p2 - t*t*(j-1) * p3) / (j+2);
	p1dx = ( (2*j+1) * (x * p2dx + p2) - t*t*(j-1) * p3dx) / (j+2);
	p1dt = ( (2*j+1) * x * p2dt - (j-1)* (t*t*p3dt+2*t*p3)) / (j+2);

	// shape[j] = p1;
	dshape[DIST*j  ] = p1dx;
	dshape[DIST*j+1] = p1dt;
      }    
  }


   void LegendrePolynomial (int n, double x, double * values)
  {
    switch (n)
      {
      case 0:
	values[0] = 1;
	break;
      case 1:
	values[0] = 1;
	values[1] = x;
	break;

      default:

	if (n < 0) return;

	double p1 = 1.0, p2 = 0.0, p3;
	
	values[0] = 1.0;
	for (int j=1; j<=n; j++)
	  {
	    p3 = p2; p2 = p1;
	    p1 = ((2.0*j-1.0)*x*p2 - (j-1.0)*p3) / j;
	    values[j] = p1;
	  }
      }
  }


   void ScaledLegendrePolynomial (int n, double x, double t, double * values)
  {
    switch (n)
      {
      case 0:
	values[0] = 1;
	break;

      case 1:
	values[0] = 1;
	values[1] = x;
	break;

      default:

	if (n < 0) return;

	double p1 = 1.0, p2 = 0.0, p3;
	values[0] = 1.0;
	for (int j=1; j<=n; j++)
	  {
	    p3 = p2; p2 = p1;
	    p1 = ((2.0*j-1.0)*x*p2 - t*t*(j-1.0)*p3) / j;
	    values[j] = p1;
	  }
      }
  }


  // compute face bubbles up to order n, 0 < y, y-x < 1, x+y < 1
   void CalcTrigShape (int n, double x, double y, double * shape)
  { 
    if (n < 3) return;
    double hx[20], hy[20];
    ScaledLegendrePolynomial (n-3, 2*x-1, 1-y, hx);
    LegendrePolynomial (n-3, 2*y-1, hy);

    int ii = 0;
    double bub = (1+x-y)*y*(1-x-y);
    for (int iy = 0; iy <= n-3; iy++)
      for (int ix = 0; ix <= n-3-iy; ix++)
	shape[ii++] = bub * hx[ix]*hy[iy];
  }

   void CalcTrigShapeDxDy (int n, double x, double y, double * dshape)
  { 
    if (n < 3) return;
    
    int ndof = (n-1)*(n-2)/2;
    double h1[20], h2[20];
    double eps = 1e-4;
  
    CalcTrigShape (n, x+eps, y, h1);
    CalcTrigShape (n, x-eps, y, h2);

    for (int i = 0; i < ndof; i++)
      dshape[2*i] = (h1[i]-h2[i])/(2*eps);

    CalcTrigShape (n, x, y+eps, h1);
    CalcTrigShape (n, x, y-eps, h2);

    for (int i = 0; i < ndof; i++)
      dshape[2*i+1] = (h1[i]-h2[i])/(2*eps);
  }


  // compute face bubbles up to order n, 0 < y, y-x < 1, x+y < 1
   void CalcScaledTrigShape (int n, double x, double y, double t, double * shape)
  {
    if (n < 3) return;

    double hx[20], hy[20];
    ScaledLegendrePolynomial (n-3, (2*x-1), t-y, hx);
    ScaledLegendrePolynomial (n-3, (2*y-1), t, hy);

    int ii = 0;
    double bub = (t+x-y)*y*(t-x-y);
    for (int iy = 0; iy <= n-3; iy++)
      for (int ix = 0; ix <= n-3-iy; ix++)
	shape[ii++] = bub * hx[ix]*hy[iy];
  }


  // compute face bubbles up to order n, 0 < y, y-x < 1, x+y < 1
   void CalcScaledTrigShapeDxDyDt (int n, double x, double y, double t, double * dshape)
  {
    if (n < 3) return;
    double hvl[100], hvr[100];
    int nd = (n-1)*(n-2)/2;
    
    double eps = 1e-6;

    CalcScaledTrigShape (n, x-eps, y, t, hvl);
    CalcScaledTrigShape (n, x+eps, y, t, hvr);
    for (int i = 0; i < nd; i++)
      dshape[3*i] = (hvr[i]-hvl[i])/(2*eps);

    CalcScaledTrigShape (n, x, y-eps, t, hvl);
    CalcScaledTrigShape (n, x, y+eps, t, hvr);
    for (int i = 0; i < nd; i++)
      dshape[3*i+1] = (hvr[i]-hvl[i])/(2*eps);

    CalcScaledTrigShape (n, x, y, t-eps, hvl);
    CalcScaledTrigShape (n, x, y, t+eps, hvr);
    for (int i = 0; i < nd; i++)
      dshape[3*i+2] = (hvr[i]-hvl[i])/(2*eps);
  }

    

  

  CurvedElements :: CurvedElements (const Mesh & amesh)
    : mesh (amesh)
  {
    order = 1;
  }


  CurvedElements :: ~CurvedElements()
  {
    ;
  }


  void CurvedElements :: BuildCurvedElements(Refinement * ref, int aorder)
  {
    if (mesh.coarsemesh)
      {
	mesh.coarsemesh->GetCurvedElements().BuildCurvedElements (ref, aorder);
	SetHighOrder();
	return;
      }
    
    PrintMessage (1, "Curve elements, order = ", aorder);
    const_cast<Mesh&> (mesh).UpdateTopology();
    const MeshTopology & top = mesh.GetTopology();

    ARRAY<int> edgenrs;

    edgeorder.SetSize (top.GetNEdges());
    faceorder.SetSize (top.GetNFaces());

    edgeorder = 1;
    faceorder = 1;

    if (aorder <= 1) return;

    if (mesh.GetDimension() == 3)
      for (SurfaceElementIndex i = 0; i < mesh.GetNSE(); i++)
	{
	  top.GetSurfaceElementEdges (i+1, edgenrs);
	  for (int j = 0; j < edgenrs.Size(); j++)
	    edgeorder[edgenrs[j]-1] = aorder;
	  faceorder[top.GetSurfaceElementFace (i+1)-1] = aorder;
	}
    for (SegmentIndex i = 0; i < mesh.GetNSeg(); i++)
      edgeorder[top.GetSegmentEdge (i+1)-1] = aorder;

    edgecoeffsindex.SetSize (top.GetNEdges()+1);
    int nd = 0;
    for (int i = 0; i < top.GetNEdges(); i++)
      {
	edgecoeffsindex[i] = nd;
	nd += max (0, edgeorder[i]-1);
      }
    edgecoeffsindex[top.GetNEdges()] = nd;

    edgecoeffs.SetSize (nd);
    edgecoeffs = Vec<3> (0,0,0);
    

    facecoeffsindex.SetSize (top.GetNFaces()+1);
    nd = 0;
    for (int i = 0; i < top.GetNFaces(); i++)
      {
	facecoeffsindex[i] = nd;
	if (top.GetFaceType(i+1) == TRIG)
	  nd += max (0, (faceorder[i]-1)*(faceorder[i]-2)/2);
	else
	  nd += max (0, sqr(faceorder[i]-1));
      }
    facecoeffsindex[top.GetNFaces()] = nd;

    facecoeffs.SetSize (nd);
    facecoeffs = Vec<3> (0,0,0);


    order = aorder;


    if (!ref || order <= 1) return;

    
    ARRAY<double> xi, weight;
    ComputeGaussRule (order+4, xi, weight);  // on (0,1)

    PrintMessage (3, "Curving edges");

    if (mesh.GetDimension() == 3)
    for (SurfaceElementIndex i = 0; i < mesh.GetNSE(); i++)
      {
	const Element2d & el = mesh[i];
	top.GetSurfaceElementEdges (i+1, edgenrs);
	for (int j = 0; j < edgenrs.Size(); j++)
	  edgenrs[j]--;
	const ELEMENT_EDGE * edges = MeshTopology::GetEdges (el.GetType());

	for (int i2 = 0; i2 < edgenrs.Size(); i2++)
	  {
	    PointIndex pi1 = edges[i2][0]-1;
	    PointIndex pi2 = edges[i2][1]-1;

	    bool swap = el[pi1] > el[pi2];

	    Point<3> p1 = mesh[el[pi1]];
	    Point<3> p2 = mesh[el[pi2]];

	    int order = edgeorder[edgenrs[i2]];
	    int ndof = max (0, order-1);
	    
	    Vector shape(ndof);
	    DenseMatrix mat(ndof, ndof), inv(ndof, ndof),
	      rhs(ndof, 3), sol(ndof, 3);
	    
	    rhs = 0.0;
	    mat = 0.0;
	    for (int j = 0; j < xi.Size(); j++)
	      {
		Point<3> p;
		Point3d pp;
		PointGeomInfo ppgi;
		
		if (swap)
		  {
		    p = p1 + xi[j] * (p2-p1);
		    ref -> PointBetween (p1, p2, xi[j], 
					 mesh.GetFaceDescriptor(el.GetIndex()).SurfNr(),
					 el.GeomInfoPi(edges[i2][0]),
					 el.GeomInfoPi(edges[i2][1]),
					 pp, ppgi);
		  }
		else
		  {
		    p = p2 + xi[j] * (p1-p2);
		    ref -> PointBetween (p2, p1, xi[j], 
					 mesh.GetFaceDescriptor(el.GetIndex()).SurfNr(),
					 el.GeomInfoPi(edges[i2][1]),
					 el.GeomInfoPi(edges[i2][0]),
					 pp, ppgi);
		  }
		
		Vec<3> dist = pp - p;
		
		CalcEdgeShape (order, 2*xi[j]-1, &shape(0));
		
		for (int k = 0; k < ndof; k++)
		  for (int l = 0; l < ndof; l++)
		mat(k,l) += weight[j] * shape(k) * shape(l);
		
		for (int k = 0; k < ndof; k++)
		  for (int l = 0; l < 3; l++)
		    rhs(k,l) += weight[j] * shape(k) * dist(l);
	      }
	    
	    CalcInverse (mat, inv);
	    Mult (inv, rhs, sol);
	    
	    int first = edgecoeffsindex[edgenrs[i2]];
	    for (int j = 0; j < ndof; j++)
	      for (int k = 0; k < 3; k++)
		edgecoeffs[first+j](k) = sol(j,k);
	  }
      }


    for (SegmentIndex i = 0; i < mesh.GetNSeg(); i++)
      {
	const Segment & seg = mesh[i];
	PointIndex pi1 = mesh[i].p1;
	PointIndex pi2 = mesh[i].p2;

	bool swap = (pi1 > pi2);

	Point<3> p1 = mesh[pi1];
	Point<3> p2 = mesh[pi2];

	int segnr = top.GetSegmentEdge (i+1)-1;

	int order = edgeorder[segnr];
	int ndof = max (0, order-1);

	Vector shape(ndof);
	DenseMatrix mat(ndof, ndof), inv(ndof, ndof),
	  rhs(ndof, 3), sol(ndof, 3);

	rhs = 0.0;
	mat = 0.0;
	for (int j = 0; j < xi.Size(); j++)
	  {
	    Point<3> p;

	    Point3d pp;
	    EdgePointGeomInfo ppgi;
	    
	    if (swap)
	      {
		p = p1 + xi[j] * (p2-p1);
		ref -> PointBetween (p1, p2, xi[j], 
				     seg.surfnr2, seg.surfnr1, 
				     seg.epgeominfo[0], seg.epgeominfo[1],
				     pp, ppgi);
	      }
	    else
	      {
		p = p2 + xi[j] * (p1-p2);
		ref -> PointBetween (p2, p1, xi[j], 
				     seg.surfnr2, seg.surfnr1, 
				     seg.epgeominfo[1], seg.epgeominfo[0],
				     pp, ppgi);
	      }
	    
	    testout -> precision (8);
	    Vec<3> dist = pp - p;

	    CalcEdgeShape (order, 2*xi[j]-1, &shape(0));

	    for (int k = 0; k < ndof; k++)
	      for (int l = 0; l < ndof; l++)
		mat(k,l) += weight[j] * shape(k) * shape(l);

	    for (int k = 0; k < ndof; k++)
	      for (int l = 0; l < 3; l++)
		rhs(k,l) += weight[j] * shape(k) * dist(l);
	  }

	CalcInverse (mat, inv);
	Mult (inv, rhs, sol);

	int first = edgecoeffsindex[segnr];
	for (int j = 0; j < ndof; j++)
	  for (int k = 0; k < 3; k++)
	    edgecoeffs[first+j](k) = sol(j,k);
      }




    PrintMessage (3, "Curving faces");

    if (mesh.GetDimension() == 3)
    for (SurfaceElementIndex i = 0; i < mesh.GetNSE(); i++)
      {
	const Element2d & el = mesh[i];
	int facenr = top.GetSurfaceElementFace (i+1)-1;

	if (el.GetType() == TRIG && order >= 3)
	  {
	    int fnums[] = { 0, 1, 2 };
	    if (el[fnums[0]] > el[fnums[1]]) swap (fnums[0], fnums[1]);
	    if (el[fnums[1]] > el[fnums[2]]) swap (fnums[1], fnums[2]);
	    if (el[fnums[0]] > el[fnums[1]]) swap (fnums[0], fnums[1]);

	    int order = faceorder[facenr];
	    int ndof = max (0, (order-1)*(order-2)/2);
	    
	    Vector shape(ndof);
	    DenseMatrix mat(ndof, ndof), inv(ndof, ndof),
	      rhs(ndof, 3), sol(ndof, 3);
	    
	    rhs = 0.0;
	    mat = 0.0;

	    for (int jx = 0; jx < xi.Size(); jx++)
	      for (int jy = 0; jy < xi.Size(); jy++)
		{
		  double y = xi[jy];
		  double x = (1-y) * xi[jx];
		  double lami[] = { x, y, 1-x-y };
		  double wi = weight[jx]*weight[jy]*(1-y);

		  Point<2> xi (x, y);
		  Point<3> p1, p2;
		  CalcSurfaceTransformation (xi, i, p1);
		  p2 = p1;
		  ref -> ProjectToSurface (p2, mesh.GetFaceDescriptor(el.GetIndex()).SurfNr());

		  Vec<3> dist = p2-p1;
		
		  CalcTrigShape (order, lami[fnums[1]]-lami[fnums[0]],
				 1-lami[fnums[1]]-lami[fnums[0]], &shape(0));

		  for (int k = 0; k < ndof; k++)
		    for (int l = 0; l < ndof; l++)
		      mat(k,l) += wi * shape(k) * shape(l);
		  
		  for (int k = 0; k < ndof; k++)
		    for (int l = 0; l < 3; l++)
		      rhs(k,l) += wi * shape(k) * dist(l);
		}

	    CalcInverse (mat, inv);
	    Mult (inv, rhs, sol);
	    
	    int first = facecoeffsindex[facenr];
	    for (int j = 0; j < ndof; j++)
	      for (int k = 0; k < 3; k++)
		facecoeffs[first+j](k) = sol(j,k);
	  }
      }
    PrintMessage (3, "Complete");


    // compress edge and face tables
    int newbase = 0;
    for (int i = 0; i < edgeorder.Size(); i++)
      {
	bool curved = 0;
	int oldbase = edgecoeffsindex[i];
	int nd = edgecoeffsindex[i+1] - edgecoeffsindex[i];

	for (int j = 0; j < nd; j++)
	  if (edgecoeffs[oldbase+j].Length() > 1e-10)
	    curved = 1;

	if (curved && newbase != oldbase)
	  for (int j = 0; j < nd; j++)
	    edgecoeffs[newbase+j] = edgecoeffs[oldbase+j];

	edgecoeffsindex[i] = newbase;
	if (!curved) edgeorder[i] = 1;
	if (curved) newbase += nd;
      }
    edgecoeffsindex.Last() = newbase;


    newbase = 0;
    for (int i = 0; i < faceorder.Size(); i++)
      {
	bool curved = 0;
	int oldbase = facecoeffsindex[i];
	int nd = facecoeffsindex[i+1] - facecoeffsindex[i];

	for (int j = 0; j < nd; j++)
	  if (facecoeffs[oldbase+j].Length() > 1e-10)
	    curved = 1;

	if (curved && newbase != oldbase)
	  for (int j = 0; j < nd; j++)
	    facecoeffs[newbase+j] = facecoeffs[oldbase+j];

	facecoeffsindex[i] = newbase;
	if (!curved) faceorder[i] = 1;
	if (curved) newbase += nd;
      }
    facecoeffsindex.Last() = newbase;


    // std::cout << "edgecoeffs = " << endl << edgecoeffs << endl;
    // std::cout << "facecoeffs = " << endl << facecoeffs << endl;
  }










  // ***********************  Transform edges *****************************

  
  void CurvedElements :: 
  CalcSegmentTransformation (double xi, SegmentIndex elnr,
			     Point<3> * x, Vec<3> * dxdxi)
  {
    if (mesh.coarsemesh)
      {
	const HPRefElement & hpref_el =
	  (*mesh.hpelements) [mesh[elnr].hp_elnr];
	
	// xi umrechnen
	double lami[2] = { xi, 1-xi };
	double dlami[2] = { 1, -1 };

	double coarse_xi = 0;
	double trans = 0;
	for (int i = 0; i < 2; i++)
	  {
	    coarse_xi += hpref_el.param[i][0] * lami[i];
	    trans += hpref_el.param[i][0] * dlami[i];
	  }
	
	mesh.coarsemesh->GetCurvedElements().CalcSegmentTransformation (coarse_xi, hpref_el.coarse_elnr, x, dxdxi);
	if (dxdxi) *dxdxi *= trans;
	
	return;
      }
    

    Vector shapes, dshapes;
    ARRAY<Vec<3> > coefs;

    SegmentInfo info;
    info.elnr = elnr;
    info.order = order;
    info.ndof = info.nv = 2;
    if (info.order > 1)
      {
	const MeshTopology & top = mesh.GetTopology();
	info.edgenr = top.GetSegmentEdge (elnr+1)-1;	
	info.ndof += edgeorder[info.edgenr]-1;
      }


    CalcElementShapes (info, xi, shapes);
    GetCoefficients (info, coefs);

    *x = 0;
    for (int i = 0; i < shapes.Size(); i++)
      *x += shapes(i) * coefs[i];

    if (dxdxi)
      {
	CalcElementDShapes (info, xi, dshapes);
	
	*dxdxi = 0;
	for (int i = 0; i < shapes.Size(); i++)
	  for (int j = 0; j < 3; j++)
	    (*dxdxi)(j) += dshapes(i) * coefs[i](j);
      }
  }




  void CurvedElements :: 
  CalcElementShapes (SegmentInfo & info, double xi, Vector & shapes) const
  {

    shapes.SetSize(info.ndof);
    shapes(0) = xi;
    shapes(1) = 1-xi;

    if (info.order >= 2)
      {
	int order = edgeorder[info.edgenr];

	if (mesh[info.elnr].p1 > mesh[info.elnr].p2)
	  xi = 1-xi;
	CalcEdgeShape (order, 2*xi-1, &shapes(2));
      }
  }

  void CurvedElements :: 
  CalcElementDShapes (SegmentInfo & info, double xi, Vector & dshapes) const
  {
    dshapes.SetSize(info.ndof);
    dshapes = 0;
    dshapes(0) = 1;
    dshapes(1) = -1;

    // int order = edgeorder[info.edgenr];
  }

  void CurvedElements :: 
  GetCoefficients (SegmentInfo & info, ARRAY<Vec<3> > & coefs) const
  {
    const Segment & el = mesh[info.elnr];

    coefs.SetSize(info.ndof);

    coefs[0] = Vec<3> (mesh[el.p1]);
    coefs[1] = Vec<3> (mesh[el.p2]);

    if (info.order >= 2)
      {
	int first = edgecoeffsindex[info.edgenr]; 
	int next = edgecoeffsindex[info.edgenr+1]; 
	for (int i = 0; i < next-first; i++)
	  coefs[i+2] = edgecoeffs[first+i];
      }
  }













  // ********************** Transform surface elements *******************

  void CurvedElements :: 
  CalcSurfaceTransformation (Point<2> xi, SurfaceElementIndex elnr,
			     Point<3> * x, Mat<3,2> * dxdxi)
  {
    if (mesh.coarsemesh)
      {
	const HPRefElement & hpref_el =
	  (*mesh.hpelements) [mesh[elnr].hp_elnr];
	
	  // xi umrechnen
	double lami[4];
	FlatVector vlami(4, lami);
	vlami = 0;
	mesh[elnr].GetShapeNew (xi, vlami);
	
	Mat<2,2> trans;
	Mat<3,2> dxdxic;
	if (dxdxi)
	  {
	    MatrixFixWidth<2> dlami(4);
	    dlami = 0;
	    mesh[elnr].GetDShapeNew (xi, dlami);	  
	    
	    trans = 0;
	    for (int k = 0; k < 2; k++)
	      for (int l = 0; l < 2; l++)
		for (int i = 0; i < hpref_el.np; i++)
		  trans(l,k) += hpref_el.param[i][l] * dlami(i, k);
	    }
	
	Point<2> coarse_xi(0,0);
	for (int i = 0; i < hpref_el.np; i++)
	  for (int j = 0; j < 2; j++)
	    coarse_xi(j) += hpref_el.param[i][j] * lami[i];
	
	mesh.coarsemesh->GetCurvedElements().CalcSurfaceTransformation (coarse_xi, hpref_el.coarse_elnr, x, &dxdxic);
	
	if (dxdxi)
	  *dxdxi = dxdxic * trans;
	
	return;
      }
    


    Vector shapes;
    DenseMatrix dshapes;
    ARRAY<Vec<3> > coefs;

    const Element2d & el = mesh[elnr];
    ELEMENT_TYPE type = el.GetType();

    SurfaceElementInfo info;
    info.elnr = elnr;
    info.order = order;
    info.ndof = info.nv = (type == TRIG) ? 3 : 4;
    if (info.order > 1)
      {
	const MeshTopology & top = mesh.GetTopology();
	
	top.GetSurfaceElementEdges (elnr+1, info.edgenrs);
	for (int i = 0; i < info.edgenrs.Size(); i++)
	  info.edgenrs[i]--;
	info.facenr = top.GetSurfaceElementFace (elnr+1)-1;

	for (int i = 0; i < info.edgenrs.Size(); i++)
	  info.ndof += edgecoeffsindex[info.edgenrs[i]+1] - edgecoeffsindex[info.edgenrs[i]];
	info.ndof += facecoeffsindex[info.facenr+1] - facecoeffsindex[info.facenr];
      }

    CalcElementShapes (info, xi, shapes);
    GetCoefficients (info, coefs);

    *x = 0;
    for (int i = 0; i < coefs.Size(); i++)
      *x += shapes(i) * coefs[i];

    if (dxdxi)
      {
	CalcElementDShapes (info, xi, dshapes);
	
	*dxdxi = 0;
	for (int i = 0; i < coefs.Size(); i++)
	  for (int j = 0; j < 3; j++)
	    for (int k = 0; k < 2; k++)
	      (*dxdxi)(j,k) += dshapes(i,k) * coefs[i](j);
      }
  }




  void CurvedElements :: 
  CalcElementShapes (SurfaceElementInfo & info, const Point<2> & xi, Vector & shapes) const
  {
    const Element2d & el = mesh[info.elnr];

    shapes.SetSize(info.ndof);
    shapes = 0;	  

    switch (el.GetType())
      {
      case TRIG:
	{
	  shapes(0) = xi(0);
	  shapes(1) = xi(1);
	  shapes(2) = 1-xi(0)-xi(1);

	  if (info.order == 1) return;

	  int ii = 3;
	  const ELEMENT_EDGE * edges = MeshTopology::GetEdges (TRIG);
	  
	  for (int i = 0; i < 3; i++)
	    {
	      int order = edgeorder[info.edgenrs[i]];
	      if (order >= 2)
		{
		  int vi1 = edges[i][0]-1, vi2 = edges[i][1]-1;
		  if (el[vi1] > el[vi2]) swap (vi1, vi2);

		  CalcScaledEdgeShape (order, shapes(vi1)-shapes(vi2), shapes(vi1)+shapes(vi2), &shapes(ii));
		  ii += order-1;
		}
	    }

	  int order = faceorder[info.facenr];
	  if (order >= 3)
	    {
	      int fnums[] = { 0, 1, 2 };
	      if (el[fnums[0]] > el[fnums[1]]) swap (fnums[0], fnums[1]);
	      if (el[fnums[1]] > el[fnums[2]]) swap (fnums[1], fnums[2]);
	      if (el[fnums[0]] > el[fnums[1]]) swap (fnums[0], fnums[1]);
	      
	      CalcTrigShape (order, 
			     shapes(fnums[1])-shapes(fnums[0]),
			     1-shapes(fnums[1])-shapes(fnums[0]), &shapes(ii));
	    }
	  break;
	}

      case QUAD:
	{
	  shapes(0) = (1-xi(0))*(1-xi(1));
	  shapes(1) =    xi(0) *(1-xi(1));
	  shapes(2) =    xi(0) *   xi(1) ;
	  shapes(3) = (1-xi(0))*   xi(1) ;

	  if (info.order == 1) return;
	  
	  double mu[4] = { 
	    1 - xi(0) + 1 - xi(1), 
	        xi(0) + 1 - xi(1), 
	        xi(0) +     xi(1), 
	    1 - xi(0) +     xi(1), 
	  };
	    
	  int ii = 4;
	  const ELEMENT_EDGE * edges = MeshTopology::GetEdges (QUAD);
	  
	  for (int i = 0; i < 4; i++)
	    {
	      int order = edgeorder[info.edgenrs[i]];
	      if (order >= 2)
		{
		  int vi1 = edges[i][0]-1, vi2 = edges[i][1]-1;
		  if (el[vi1] > el[vi2]) swap (vi1, vi2);

		  CalcEdgeShape (order, mu[vi1]-mu[vi2], &shapes(ii));
		  double lame = shapes(vi1)+shapes(vi2);
		  for (int j = 0; j < order-1; j++)
		    shapes(ii+j) *= lame;
		  ii += order-1;
		}
	    }
	  
	  for (int i = ii; i < info.ndof; i++)
	    shapes(i) = 0;

	  break;
	}
      };
  }


  void CurvedElements :: 
  CalcElementDShapes (SurfaceElementInfo & info, const Point<2> & xi, DenseMatrix & dshapes) const
  {
    const Element2d & el = mesh[info.elnr];
    ELEMENT_TYPE type = el.GetType();

    double lami[4];

    dshapes.SetSize(info.ndof,2);
    dshapes = 0;	  

    switch (type)
      {
      case TRIG:
	{
	  dshapes(0,0) = 1;
	  dshapes(1,1) = 1;
	  dshapes(2,0) = -1;
	  dshapes(2,1) = -1;
	  
	  if (info.order == 1) return;

	  lami[0] = xi(0);
	  lami[1] = xi(1);
	  lami[2] = 1-xi(0)-xi(1);

	  int ii = 3;
	  const ELEMENT_EDGE * edges = MeshTopology::GetEdges (TRIG);
	  
	  for (int i = 0; i < 3; i++)
	    {
	      int order = edgeorder[info.edgenrs[i]];
	      if (order >= 2)
		{
		  int vi1 = edges[i][0]-1, vi2 = edges[i][1]-1;
		  if (el[vi1] > el[vi2]) swap (vi1, vi2);

		  CalcScaledEdgeShapeDxDt<2> (order, lami[vi1]-lami[vi2], lami[vi1]+lami[vi2], &dshapes(ii,0));

		  Mat<2,2> trans;
		  for (int j = 0; j < 2; j++)
		    {
		      trans(0,j) = dshapes(vi1,j)-dshapes(vi2,j);
		      trans(1,j) = dshapes(vi1,j)+dshapes(vi2,j);
		    }
		  
		  for (int j = 0; j < order-1; j++)
		    {
		      double ddx = dshapes(ii+j,0);
		      double ddt = dshapes(ii+j,1);
		      dshapes(ii+j,0) = ddx * trans(0,0) + ddt * trans(1,0);
		      dshapes(ii+j,1) = ddx * trans(0,1) + ddt * trans(1,1);
		    }

		  ii += order-1;
		}
	    }

	  int order = faceorder[info.facenr];
	  if (order >= 3)
	    {
	      int fnums[] = { 0, 1, 2 };
	      if (el[fnums[0]] > el[fnums[1]]) swap (fnums[0], fnums[1]);
	      if (el[fnums[1]] > el[fnums[2]]) swap (fnums[1], fnums[2]);
	      if (el[fnums[0]] > el[fnums[1]]) swap (fnums[0], fnums[1]);
	      
	      CalcTrigShapeDxDy (order, 
				 lami[fnums[1]]-lami[fnums[0]],
				 1-lami[fnums[1]]-lami[fnums[0]], &dshapes(ii,0));

	      int nd = (order-1)*(order-2)/2;
	      Mat<2,2> trans;
	      for (int j = 0; j < 2; j++)
		{
		  trans(0,j) = dshapes(fnums[1],j)-dshapes(fnums[0],j);
		  trans(1,j) = -dshapes(fnums[1],j)-dshapes(fnums[0],j);
		}

	      for (int j = 0; j < nd; j++)
		{
		  double ddx = dshapes(ii+j,0);
		  double ddt = dshapes(ii+j,1);
		  dshapes(ii+j,0) = ddx * trans(0,0) + ddt * trans(1,0);
		  dshapes(ii+j,1) = ddx * trans(0,1) + ddt * trans(1,1);
		}
	    }

	  break;
	}
      case QUAD:
	{
	  dshapes(0,0) = -(1-xi(1));
	  dshapes(0,1) = -(1-xi(0));
	  dshapes(1,0) =  (1-xi(1));
	  dshapes(1,1) =    -xi(0);
	  dshapes(2,0) =     xi(1);
	  dshapes(2,1) =     xi(0);
	  dshapes(3,0) =    -xi(1);
	  dshapes(3,1) =  (1-xi(0));

	  if (info.order == 1) return;

	  double shapes[4] = {
	    (1-xi(0))*(1-xi(1)),
	       xi(0) *(1-xi(1)),
	       xi(0) *   xi(1) ,
	    (1-xi(0))*   xi(1) 
	  };

	  double mu[4] = { 
	    1 - xi(0) + 1 - xi(1), 
	        xi(0) + 1 - xi(1), 
	        xi(0) +     xi(1), 
	    1 - xi(0) +     xi(1), 
	  };

	  double dmu[4][2] = {
	    { -1, -1 },
	    { 1, -1 },
	    { 1, 1 },
	    { -1, 1 } };
	    
	  double hshapes[20], hdshapes[20];

	  int ii = 4;
	  const ELEMENT_EDGE * edges = MeshTopology::GetEdges (QUAD);
	  
	  for (int i = 0; i < 4; i++)
	    {
	      int order = edgeorder[info.edgenrs[i]];
	      if (order >= 2)
		{
		  int vi1 = edges[i][0]-1, vi2 = edges[i][1]-1;
		  if (el[vi1] > el[vi2]) swap (vi1, vi2);

		  CalcEdgeShapeDx (order, mu[vi1]-mu[vi2], hshapes, hdshapes);

		  double lame = shapes[vi1]+shapes[vi2];
		  double dlame[2] = {
		    dshapes(vi1, 0) + dshapes(vi2, 0),
		    dshapes(vi1, 1) + dshapes(vi2, 1) };
		    
		  for (int j = 0; j < order-1; j++)
		    for (int k = 0; k < 2; k++)
		      dshapes(ii+j, k) = 
			lame * hdshapes[j] * (dmu[vi1][k]-dmu[vi2][k])
			+ dlame[k] * hshapes[j];

		  ii += order-1;
		}
	    }

	  /*	  
	  std::cout << "quad, dshape = " << endl << dshapes << endl;
	  for (int i = 0; i < 2; i++)
	    {
	      Point<2> xil = xi, xir = xi;
	      Vector shapesl(dshapes.Height()), shapesr(dshapes.Height());
	      xil(i) -= 1e-6;
	      xir(i) += 1e-6;
	      CalcElementShapes (info, xil, shapesl);
	      CalcElementShapes (info, xir, shapesr);
	      
	      for (int j = 0; j < dshapes.Height(); j++)
		dshapes(j,i) = 1.0 / 2e-6 * (shapesr(j)-shapesl(j));
	    }
	  
	  std::cout << "quad, num dshape = " << endl << dshapes << endl;
	  */
	  break;
	}
      };
  }



  void CurvedElements :: 
  GetCoefficients (SurfaceElementInfo & info, ARRAY<Vec<3> > & coefs) const
  {
    const Element2d & el = mesh[info.elnr];

    coefs.SetSize (info.ndof);
    // coefs = Vec<3> (0,0,0);
    
    for (int i = 0; i < info.nv; i++)
      coefs[i] = Vec<3> (mesh[el[i]]);
    
    if (info.order == 1) return;

    int ii = info.nv;
	  
    for (int i = 0; i < info.edgenrs.Size(); i++)
      {
	int first = edgecoeffsindex[info.edgenrs[i]];
	int next = edgecoeffsindex[info.edgenrs[i]+1];
	for (int j = first; j < next; j++, ii++)
	  coefs[ii] = edgecoeffs[j];
      }
    
    int first = facecoeffsindex[info.facenr];
    int next = facecoeffsindex[info.facenr+1];
    for (int j = first; j < next; j++, ii++)
      coefs[ii] = facecoeffs[j];
  }







  // ********************** Transform volume elements *******************


  void CurvedElements :: 
  CalcElementTransformation (Point<3> xi, ElementIndex elnr,
			     Point<3> * x, Mat<3,3> * dxdxi)
  {
    if (mesh.coarsemesh)
      {
	  const HPRefElement & hpref_el =
	    (*mesh.hpelements) [mesh[elnr].hp_elnr];
	  
	  // xi umrechnen
	  double lami[8];
	  FlatVector vlami(8, lami);
	  vlami = 0;
	  mesh[elnr].GetShapeNew (xi, vlami);

	  Mat<3,3> trans, dxdxic;
	  if (dxdxi)
	    {
	      MatrixFixWidth<3> dlami(8);
	      dlami = 0;
	      mesh[elnr].GetDShapeNew (xi, dlami);	  
	      
	      trans = 0;
	      for (int k = 0; k < 3; k++)
		for (int l = 0; l < 3; l++)
		  for (int i = 0; i < hpref_el.np; i++)
		    trans(l,k) += hpref_el.param[i][l] * dlami(i, k);
	    }

	  Point<3> coarse_xi(0,0,0);
	  for (int i = 0; i < hpref_el.np; i++)
	    for (int j = 0; j < 3; j++)
	      coarse_xi(j) += hpref_el.param[i][j] * lami[i];

	  mesh.coarsemesh->GetCurvedElements().CalcElementTransformation (coarse_xi, hpref_el.coarse_elnr, x, &dxdxic);

	  if (dxdxi)
	    *dxdxi = dxdxic * trans;

	  return;
	}


    Vector shapes;
    DenseMatrix dshapes;
    ARRAY<Vec<3> > coefs;


    const Element & el = mesh[elnr];
    ELEMENT_TYPE type = el.GetType();

    ElementInfo info;
    info.elnr = elnr;
    info.order = order;
    info.ndof = info.nv = MeshTopology::GetNVertices (type);
    if (info.order > 1)
      {
	const MeshTopology & top = mesh.GetTopology();
	
	top.GetElementEdges (elnr+1, info.edgenrs);
	for (int i = 0; i < info.edgenrs.Size(); i++)
	  info.edgenrs[i]--;

	top.GetElementFaces (elnr+1, info.facenrs);
	for (int i = 0; i < info.facenrs.Size(); i++)
	  info.facenrs[i]--;

	for (int i = 0; i < info.edgenrs.Size(); i++)
	  info.ndof += edgecoeffsindex[info.edgenrs[i]+1] - edgecoeffsindex[info.edgenrs[i]];
	for (int i = 0; i < info.facenrs.Size(); i++)
	  info.ndof += facecoeffsindex[info.facenrs[i]+1] - facecoeffsindex[info.facenrs[i]];
	// info.ndof += facecoeffsindex[info.facenr+1] - facecoeffsindex[info.facenr];
      }



    CalcElementShapes (info, xi, shapes);
    GetCoefficients (info, coefs);

    *x = 0;
    for (int i = 0; i < shapes.Size(); i++)
      *x += shapes(i) * coefs[i];

    if (dxdxi)
      {
	CalcElementDShapes (info, xi, dshapes);
	
	*dxdxi = 0;
	for (int i = 0; i < shapes.Size(); i++)
	  for (int j = 0; j < 3; j++)
	    for (int k = 0; k < 3; k++)
	      (*dxdxi)(j,k) += dshapes(i,k) * coefs[i](j);
      }
  }




  void CurvedElements ::   CalcElementShapes (ElementInfo & info, const Point<3> & xi, Vector & shapes) const
  {
    const Element & el = mesh[info.elnr];

    shapes.SetSize(info.ndof);
    
    switch (el.GetType())
      {
      case TET:
	{
	  shapes(0) = xi(0);
	  shapes(1) = xi(1);
	  shapes(2) = xi(2);
	  shapes(3) = 1-xi(0)-xi(1)-xi(2);

	  if (info.order == 1) return;

	  int ii = 4;
	  const ELEMENT_EDGE * edges = MeshTopology::GetEdges (TET);
	  for (int i = 0; i < 6; i++)
	    {
	      int order = edgeorder[info.edgenrs[i]];
	      if (order >= 2)
		{
		  int vi1 = edges[i][0]-1, vi2 = edges[i][1]-1;
		  if (el[vi1] > el[vi2]) swap (vi1, vi2);

		  CalcScaledEdgeShape (order, shapes(vi1)-shapes(vi2), shapes(vi1)+shapes(vi2), &shapes(ii));
		  ii += order-1;
		}
	    }
	  const ELEMENT_FACE * faces = MeshTopology::GetFaces (TET);
	  for (int i = 0; i < 4; i++)
	    {
	      int order = faceorder[info.facenrs[i]];
	      if (order >= 3)
		{
		  int fnums[] = { faces[i][0]-1, faces[i][1]-1, faces[i][2]-1 }; 
		  if (el[fnums[0]] > el[fnums[1]]) swap (fnums[0], fnums[1]);
		  if (el[fnums[1]] > el[fnums[2]]) swap (fnums[1], fnums[2]);
		  if (el[fnums[0]] > el[fnums[1]]) swap (fnums[0], fnums[1]);

		  CalcScaledTrigShape (order, 
				       shapes(fnums[1])-shapes(fnums[0]), shapes(fnums[2]), 
				       shapes(fnums[0])+shapes(fnums[1])+shapes(fnums[2]), &shapes(ii));
		  ii += (order-1)*(order-2)/2;
		  // CalcScaledEdgeShape (order, shapes(vi1)-shapes(vi2), shapes(vi1)+shapes(vi2), &shapes(ii));
		  // ii += order-1;
		}
	    }


	  break;
	}
      case PRISM:
	{
	  double lami[3] = { xi(0), xi(1), 1-xi(0)-xi(1) };
	  for (int i = 0; i < 6; i++)
	    shapes(i) = lami[i%3] * ( (i < 3) ? (1-xi(2)) : xi(2) );
	  for (int i = 6; i < info.ndof; i++)
	    shapes(i) = 0;


	  int ii = 6;
	  const ELEMENT_EDGE * edges = MeshTopology::GetEdges (PRISM);
	  for (int i = 0; i < 6; i++)    // horizontal edges
	    {
	      int order = edgeorder[info.edgenrs[i]];
	      if (order >= 2)
		{
		  int vi1 = (edges[i][0]-1) % 3, vi2 = (edges[i][1]-1) % 3;
		  if (el[vi1] > el[vi2]) swap (vi1, vi2);

		  CalcScaledEdgeShape (order, lami[vi1]-lami[vi2], lami[vi1]+lami[vi2], &shapes(ii));
		  double facz = (i < 3) ? (1-xi(2)) : xi(2);
		  for (int j = 0; j < order-1; j++)
		    shapes(ii+j) *= facz;

		  ii += order-1;
		}
	    }


	  break;
	}

      case PYRAMID:
	{
	  shapes = 0.0;
	  double x = xi(0);
	  double y = xi(1);
	  double z = xi(2);
	  
	  if (z == 1.) z = 1-1e-10;
	  shapes[0] = (1-z-x)*(1-z-y) / (1-z);
	  shapes[1] = x*(1-z-y) / (1-z);
	  shapes[2] = x*y / (1-z);
	  shapes[3] = (1-z-x)*y / (1-z);
	  shapes[4] = z;
	  break;
	}
      };
  }


  void CurvedElements :: 
  CalcElementDShapes (ElementInfo & info, const Point<3> & xi, DenseMatrix & dshapes) const
  {
    const Element & el = mesh[info.elnr];

    dshapes.SetSize(info.ndof,3);
    dshapes = 0.0;

    switch (el.GetType())
      {
      case TET:
	{
	  dshapes(0,0) = 1;
	  dshapes(1,1) = 1;
	  dshapes(2,2) = 1;
	  dshapes(3,0) = -1;
	  dshapes(3,1) = -1;
	  dshapes(3,2) = -1;

	  if (info.order == 1) return;

	  double lami[] = { xi(0), xi(1), xi(2), 1-xi(0)-xi(1)-xi(2) };
	  int ii = 4;
	  const ELEMENT_EDGE * edges = MeshTopology::GetEdges (TET);
	  for (int i = 0; i < 6; i++)
	    {
	      int order = edgeorder[info.edgenrs[i]];
	      if (order >= 2)
		{
		  int vi1 = edges[i][0]-1, vi2 = edges[i][1]-1;
		  if (el[vi1] > el[vi2]) swap (vi1, vi2);

		  CalcScaledEdgeShapeDxDt<3> (order, lami[vi1]-lami[vi2], lami[vi1]+lami[vi2], &dshapes(ii,0));

		  Mat<2,3> trans;
		  for (int j = 0; j < 3; j++)
		    {
		      trans(0,j) = dshapes(vi1,j)-dshapes(vi2,j);
		      trans(1,j) = dshapes(vi1,j)+dshapes(vi2,j);
		    }
		  
		  for (int j = 0; j < order-1; j++)
		    {
		      double ddx = dshapes(ii+j,0);
		      double ddt = dshapes(ii+j,1);
		      dshapes(ii+j,0) = ddx * trans(0,0) + ddt * trans(1,0);
		      dshapes(ii+j,1) = ddx * trans(0,1) + ddt * trans(1,1);
		      dshapes(ii+j,2) = ddx * trans(0,2) + ddt * trans(1,2);
		    }

		  ii += order-1;
		}
	    }

	  const ELEMENT_FACE * faces = MeshTopology::GetFaces (TET);
	  for (int i = 0; i < 4; i++)
	    {
	      int order = faceorder[info.facenrs[i]];
	      if (order >= 3)
		{
		  int fnums[] = { faces[i][0]-1, faces[i][1]-1, faces[i][2]-1 }; 
		  if (el[fnums[0]] > el[fnums[1]]) swap (fnums[0], fnums[1]);
		  if (el[fnums[1]] > el[fnums[2]]) swap (fnums[1], fnums[2]);
		  if (el[fnums[0]] > el[fnums[1]]) swap (fnums[0], fnums[1]);

		  CalcScaledTrigShapeDxDyDt (order, 
					     lami[fnums[1]]-lami[fnums[0]], 
					     lami[fnums[2]], lami[fnums[0]]+lami[fnums[1]]+lami[fnums[2]],
					     &dshapes(ii,0));

		  Mat<3,3> trans;
		  for (int j = 0; j < 3; j++)
		    {
		      trans(0,j) = dshapes(fnums[1],j)-dshapes(fnums[0],j);
		      trans(1,j) = dshapes(fnums[2],j);
		      trans(2,j) = dshapes(fnums[0],j)+dshapes(fnums[1],j)+dshapes(fnums[2],j);
		    }
		  
		  int nfd = (order-1)*(order-2)/2;
		  for (int j = 0; j < nfd; j++)
		    {
		      double ddx = dshapes(ii+j,0);
		      double ddy = dshapes(ii+j,1);
		      double ddt = dshapes(ii+j,2);
		      dshapes(ii+j,0) = ddx * trans(0,0) + ddy * trans(1,0) + ddt * trans(2,0);
		      dshapes(ii+j,1) = ddx * trans(0,1) + ddy * trans(1,1) + ddt * trans(2,1);
		      dshapes(ii+j,2) = ddx * trans(0,2) + ddy * trans(1,2) + ddt * trans(2,2);
		    }

		  ii += nfd;
		}
	    }

	  break;
	}
      case PRISM:
	{
	  double lami[3] = { xi(0), xi(1), 1-xi(0)-xi(1) };
	  double dlami[3][2] = 
	    { { 1, 0, },
	      { 0, 1, },
	      { -1, -1 } };
	  for (int i = 0; i < 6; i++)
	    {
	      // shapes(i) = lami[i%3] * ( (i < 3) ? (1-xi(2)) : xi(2) );
	      dshapes(i,0) = dlami[i%3][0] * ( (i < 3) ? (1-xi(2)) : xi(2) );
	      dshapes(i,1) = dlami[i%3][1] * ( (i < 3) ? (1-xi(2)) : xi(2) );
	      dshapes(i,2) = lami[i%3] * ( (i < 3) ? -1 : 1 );
	    }

	  /*
	  int ii = 6;
	  const ELEMENT_EDGE * edges = MeshTopology::GetEdges (PRISM);
	  for (int i = 0; i < 6; i++)    // horizontal edges
	    {
	      int order = edgeorder[info.edgenrs[i]];
	      if (order >= 2)
		{
		  int vi1 = (edges[i][0]-1) % 3, vi2 = (edges[i][1]-1) % 3;
		  if (el[vi1] > el[vi2]) swap (vi1, vi2);

		  CalcScaledEdgeShape (order, lami[vi1]-lami[vi2], lami[vi1]+lami[vi2], &shapes(ii));
		  double facz = (i < 3) ? (1-xi(2)) : xi(2);
		  for (int j = 0; j < order-1; j++)
		    shapes(ii+j) *= facz;

		  ii += order-1;
		}
	    }
	  */
	  break;

	}

      case PYRAMID:
	{
	  dshapes = 0.0;
	  double x = xi(0);
	  double y = xi(1);
	  double z = xi(2);
	  
	  if (z == 1.) z = 1-1e-10;
	  double z1 = 1-z;
	  double z2 = z1*z1;
	  
	  dshapes(0,0) = -(z1-y)/z1;
	  dshapes(0,1) = -(z1-x)/z1;
	  dshapes(0,2) = ((x+y+2*z-2)*z1+(z1-y)*(z1-x))/z2;

	  dshapes(1,0) = (z1-y)/z1;
	  dshapes(1,1) = -x/z1;
	  dshapes(1,2) = (-x*z1+x*(z1-y))/z2;

	  dshapes(2,0) = y/z1;
	  dshapes(2,1) = x/z1;
	  dshapes(2,2) = x*y/z2;

	  dshapes(3,0) = -y/z1;
	  dshapes(3,1) = (z1-x)/z1;
	  dshapes(3,2) = (-y*z1+y*(z1-x))/z2;

	  dshapes(4,0) = 0;
	  dshapes(4,1) = 0;
	  dshapes(4,2) = 1;
		  /* old:
	  vdshape[0] = Vec<3>( -(z1-y)/z1, -(z1-x)/z1, ((x+y+2*z-2)*z1+(z1-y)*(z1-x))/z2 );
	  vdshape[1] = Vec<3>( (z1-y)/z1,  -x/z1,      (-x*z1+x*(z1-y))/z2 );
	  vdshape[2] = Vec<3>( y/z1,       x/z1,       x*y/z2 );
	  vdshape[3] = Vec<3>( -y/z1,      (z1-x)/z1,  (-y*z1+y*(z1-x))/z2 );
	  vdshape[4] = Vec<3>( 0, 0, 1 );
		  */
	  break;
	}
      }
    
    /*
    DenseMatrix dshapes2 (info.ndof, 3);
    Vector shapesl(info.ndof); 
    Vector shapesr(info.ndof); 
    
    double eps = 1e-6;
    for (int i = 0; i < 3; i++)
      {
	Point<3> xl = xi;
	Point<3> xr = xi;
	
	xl(i) -= eps;
	xr(i) += eps;
	CalcElementShapes (info, xl, shapesl);
	CalcElementShapes (info, xr, shapesr);
	
	for (int j = 0; j < info.ndof; j++)
	  dshapes2(j,i) = (shapesr(j)-shapesl(j)) / (2*eps);
      }
    std::cout << "dshapes = " << endl << dshapes << endl;
    std::cout << "dshapes2 = " << endl << dshapes2 << endl;
    dshapes2 -= dshapes;
    std::cout << "diff = " << endl << dshapes2 << endl;
    */
  }



  void CurvedElements :: 
  GetCoefficients (ElementInfo & info, ARRAY<Vec<3> > & coefs) const
  {
    const Element & el = mesh[info.elnr];

    coefs.SetSize (info.ndof);
    coefs = Vec<3> (0,0,0);

    for (int i = 0; i < info.nv; i++)
      coefs[i] = Vec<3> (mesh[el[i]]);

    int ii = info.nv;
	  
    for (int i = 0; i < info.edgenrs.Size(); i++)
      {
	int first = edgecoeffsindex[info.edgenrs[i]];
	int next = edgecoeffsindex[info.edgenrs[i]+1];
	for (int j = first; j < next; j++, ii++)
	  coefs[ii] = edgecoeffs[j];
      }
    for (int i = 0; i < info.facenrs.Size(); i++)
      {
	int first = facecoeffsindex[info.facenrs[i]];
	int next = facecoeffsindex[info.facenrs[i]+1];
	for (int j = first; j < next; j++, ii++)
	  coefs[ii] = facecoeffs[j];
      }
  }































  void CurvedElements :: 
  CalcMultiPointSegmentTransformation (ARRAY<double> * xi, SegmentIndex segnr,
				       ARRAY<Point<3> > * x,
				       ARRAY<Vec<3> > * dxdxi)
  {
    ;
  }

  void CurvedElements :: 
  CalcMultiPointSurfaceTransformation (ARRAY< Point<2> > * xi, SurfaceElementIndex elnr,
				       ARRAY< Point<3> > * x,
				       ARRAY< Mat<3,2> > * dxdxi)
  {
    Vector shapes;
    DenseMatrix dshapes;
    ARRAY<Vec<3> > coefs;


    const Element2d & el = mesh[elnr];
    ELEMENT_TYPE type = el.GetType();

    SurfaceElementInfo info;
    info.elnr = elnr;
    info.order = order;
    info.ndof = info.nv = (type == TRIG) ? 3 : 4;
    if (info.order > 1)
      {
	const MeshTopology & top = mesh.GetTopology();
	
	top.GetSurfaceElementEdges (elnr+1, info.edgenrs);
	for (int i = 0; i < info.edgenrs.Size(); i++)
	  info.edgenrs[i]--;
	info.facenr = top.GetSurfaceElementFace (elnr+1)-1;

	for (int i = 0; i < info.edgenrs.Size(); i++)
	  info.ndof += edgecoeffsindex[info.edgenrs[i]+1] - edgecoeffsindex[info.edgenrs[i]];
	info.ndof += facecoeffsindex[info.facenr+1] - facecoeffsindex[info.facenr];
      }

    GetCoefficients (info, coefs);

    if (x)
      {
	for (int j = 0; j < xi->Size(); j++)
	  {
	    CalcElementShapes (info, (*xi)[j], shapes);
	    (*x)[j] = 0;
	    for (int i = 0; i < coefs.Size(); i++)
	      (*x)[j] += shapes(i) * coefs[i];
	  }
      }

    if (dxdxi)
      {
	for (int ip = 0; ip < xi->Size(); ip++)
	  {
	    CalcElementDShapes (info, (*xi)[ip], dshapes);
	
	    (*dxdxi)[ip] = 0;
	    for (int i = 0; i < coefs.Size(); i++)
	      for (int j = 0; j < 3; j++)
		for (int k = 0; k < 2; k++)
		  (*dxdxi)[ip](j,k) += dshapes(i,k) * coefs[i](j);
	  }
      }
  }

  void CurvedElements :: 
  CalcMultiPointElementTransformation (ARRAY< Point<3> > * xi, ElementIndex elnr,
				       ARRAY< Point<3> > * x,
				       ARRAY< Mat<3,3> > * dxdxi)
  {
    if (mesh.coarsemesh)
      {
	const HPRefElement & hpref_el =
	  (*mesh.hpelements) [mesh[elnr].hp_elnr];
	
	  // xi umrechnen
	double lami[8];
	FlatVector vlami(8, lami);


	ArrayMem<Point<3>, 50> coarse_xi (xi->Size());
	
	for (int pi = 0; pi < xi->Size(); pi++)
	  {
	    vlami = 0;
	    mesh[elnr].GetShapeNew ( (*xi)[pi], vlami);
	    
	    Point<3> cxi(0,0,0);
	    for (int i = 0; i < hpref_el.np; i++)
	      for (int j = 0; j < 3; j++)
		cxi(j) += hpref_el.param[i][j] * lami[i];

	    coarse_xi[pi] = cxi;
	  }

	mesh.coarsemesh->GetCurvedElements().
	  CalcMultiPointElementTransformation (&coarse_xi, hpref_el.coarse_elnr, x, dxdxi);


	Mat<3,3> trans, dxdxic;
	if (dxdxi)
	  {
	    MatrixFixWidth<3> dlami(8);
	    dlami = 0;

	    for (int pi = 0; pi < xi->Size(); pi++)
	      {
		mesh[elnr].GetDShapeNew ( (*xi)[pi], dlami);	  
		
		trans = 0;
		for (int k = 0; k < 3; k++)
		  for (int l = 0; l < 3; l++)
		    for (int i = 0; i < hpref_el.np; i++)
		      trans(l,k) += hpref_el.param[i][l] * dlami(i, k);
		
		dxdxic = (*dxdxi)[pi];
		(*dxdxi)[pi] = dxdxic * trans;
	      }
	  }	

	return;
      }








    Vector shapes;
    DenseMatrix dshapes;
    ARRAY<Vec<3> > coefs;


    const Element & el = mesh[elnr];
    ELEMENT_TYPE type = el.GetType();

    ElementInfo info;
    info.elnr = elnr;
    info.order = order;
    info.ndof = info.nv = MeshTopology::GetNVertices (type);
    if (info.order > 1)
      {
	const MeshTopology & top = mesh.GetTopology();
	
	top.GetElementEdges (elnr+1, info.edgenrs);
	for (int i = 0; i < info.edgenrs.Size(); i++)
	  info.edgenrs[i]--;

	top.GetElementFaces (elnr+1, info.facenrs);
	for (int i = 0; i < info.facenrs.Size(); i++)
	  info.facenrs[i]--;

	for (int i = 0; i < info.edgenrs.Size(); i++)
	  info.ndof += edgecoeffsindex[info.edgenrs[i]+1] - edgecoeffsindex[info.edgenrs[i]];
	for (int i = 0; i < info.facenrs.Size(); i++)
	  info.ndof += facecoeffsindex[info.facenrs[i]+1] - facecoeffsindex[info.facenrs[i]];
	// info.ndof += facecoeffsindex[info.facenr+1] - facecoeffsindex[info.facenr];
      }

    GetCoefficients (info, coefs);
    if (x)
      {
	for (int j = 0; j < xi->Size(); j++)
	  {
	    CalcElementShapes (info, (*xi)[j], shapes);
	    (*x)[j] = 0;
	    for (int i = 0; i < coefs.Size(); i++)
	      (*x)[j] += shapes(i) * coefs[i];
	  }
      }
    if (dxdxi)
      {
	for (int ip = 0; ip < xi->Size(); ip++)
	  {
	    CalcElementDShapes (info, (*xi)[ip], dshapes);
	
	    (*dxdxi)[ip] = 0;
	    for (int i = 0; i < coefs.Size(); i++)
	      for (int j = 0; j < 3; j++)
		for (int k = 0; k < 3; k++)
		  (*dxdxi)[ip](j,k) += dshapes(i,k) * coefs[i](j);
	  }
      }
  }



};


#endif
