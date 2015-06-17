#ifndef _FILE_SPLINEGEOMETRY
#define _FILE_SPLINEGEOMETRY
#include "../csg/csgparser.hpp"

/// 
extern void LoadBoundarySplines (const char * filename,
				 ARRAY < GeomPoint<2> > & geompoints,
				 ARRAY < SplineSeg<2>* > & splines, 
				 double & elto0);
///
extern void PartitionBoundary (const ARRAY < SplineSeg<2>* > & splines,
			       double h, double elto0,
			       Mesh & mesh2d);


// allow to turn off messages: cover all couts !!
extern int printmessage_importance;

template < int D >
class SplineGeometry
{
  ARRAY < GeomPoint<D> > geompoints;
  ARRAY < SplineSeg<D>* > splines;
  double elto0;
  ARRAY<char*> materials;
  ARRAY<string*> bcnames;
  ARRAY<double> maxh;
  ARRAY<bool> quadmeshing;
  ARRAY<bool> tensormeshing;

private:
  void AppendSegment(SplineSeg<D> * spline, const int leftdomain, const int rightdomain,
		     const int bc,
		     const double reffac, const bool hprefleft, const bool hprefright,
		     const int copyfrom);

public:
  ~SplineGeometry();

  int Load (const ARRAY<double> & raw_data, const int startpos = 0);
  void Load (const char * filename);
  void CSGLoad (CSGScanner & scan);

  void LoadData( ifstream & infile );
  void LoadDataNew ( ifstream & infile );
  void LoadDataV2 ( ifstream & infile );

  void PartitionBoundary (double h, Mesh & mesh2d);

  void GetRawData (ARRAY<double> & raw_data) const;

  void CopyEdgeMesh (int from, int to, Mesh & mesh2d, Point3dTree & searchtree);

  const ARRAY<SplineSeg<D>*> & GetSplines () const
  { return splines; }

  int GetNSplines (void) const { return splines.Size(); }
  string GetSplineType (const int i) const { return splines[i]->GetType(); }
  SplineSeg<D> & GetSpline (const int i) {return *splines[i];}
  const SplineSeg<D> & GetSpline (const int i) const {return *splines[i];}

  void GetBoundingBox (Box<D> & box) const;
  Box<D> GetBoundingBox () const 
  { Box<D> box; GetBoundingBox (box); return box; }

  int GetNP () const { return geompoints.Size(); }
  const GeomPoint<D> & GetPoint(int i) const { return geompoints[i]; }

  void SetGrading (const double grading);
  void AppendPoint (const double x, const double y, const double reffac = 1., const bool hpref = false);
  void AppendPoint (const Point<D> & p, const double reffac = 1., const bool hpref = false);
  
  void AppendLineSegment (const int n1, const int n2,
			  const int leftdomain, const int rightdomain, const int bc = -1,
			  const double reffac = 1.,
			  const bool hprefleft = false, const bool hprefright = false,
			  const int copyfrom = -1);
  void AppendSplineSegment (const int n1, const int n2, const int n3,
			    const int leftdomain, const int rightdomain, const int bc = -1,
			    const double reffac = 1.,
			    const bool hprefleft = false, const bool hprefright = false,
			    const int copyfrom = -1);
  void AppendCircleSegment (const int n1, const int n2, const int n3,
			    const int leftdomain, const int rightdomain, const int bc = -1,
			    const double reffac = 1.,
			    const bool hprefleft = false, const bool hprefright = false,
			    const int copyfrom = -1);
  void AppendDiscretePointsSegment (const ARRAY< Point<D> > & points, 
				    const int leftdomain, const int rightdomain, const int bc = -1,
				    const double reffac = 1.,
				    const bool hprefleft = false, const bool hprefright = false,
				    const int copyfrom = -1);
  void TestComment ( ifstream & infile ) ;
  void	GetMaterial( const int  domnr, char* & material );

  double GetDomainMaxh ( const int domnr );
  bool GetDomainQuadMeshing ( int domnr ) 
  { 
    if ( quadmeshing.Size() ) return quadmeshing[domnr-1]; 
    else return false;
  }
  bool GetDomainTensorMeshing ( int domnr ) 
  { 
    if ( tensormeshing.Size() ) return tensormeshing[domnr-1]; 
    else return false;
  }

  string GetBCName ( const int bcnr ) const;

  string * BCNamePtr ( const int bcnr );
};


void MeshFromSpline2D (SplineGeometry<2> & geometry,
		       Mesh *& mesh, 
		       MeshingParameters & mp);



// check if comments in a .in2d file...
template <int D>
void SplineGeometry<D> :: TestComment ( ifstream & infile )
{
  bool comment = true;
  char ch;
  infile.get(ch);
  infile.putback(ch);
  int ii = 0;
  while ( comment == true && ii < 100)
    {
      infile.get(ch);
      if ( ch == '#' )
	while (  ch != '\n')
	  {
	    infile.get(ch);
	    comment = false;
	  }
      else if ( ch == '\n' )
	{
	  comment = true;
	  ii ++;
	}
      else
	{
	  infile.putback(ch);
	  comment = false;
	}

      infile.get(ch) ;
      if ( ch == '\n' || ch == '#' )
	{
	  comment = true;
	}
     infile.putback(ch);
      if ( !comment ) break;
    }
  return;

}



template<int D>
SplineGeometry<D> :: ~SplineGeometry()
{
  for(int i=0; i<splines.Size(); i++)
    {
      delete splines[i];
    }
  splines.DeleteAll();
  geompoints.DeleteAll();
  for (int i=0; i<materials.Size(); i++)
    delete materials[i];
  for ( int i = 0; i < bcnames.Size(); i++ )
    if ( bcnames[i] ) delete bcnames[i];
}



template<int D>
int SplineGeometry<D> :: Load (const ARRAY<double> & raw_data, const int startpos)
{
  int pos = startpos;
  if(raw_data[pos] != D)
    throw NgException("wrong dimension of spline raw_data");

  pos++;

  elto0 = raw_data[pos]; pos++;

  splines.SetSize(int(raw_data[pos]));
  pos++;

  ARRAY< Point<D> > pts(3);

  for(int i=0; i<splines.Size(); i++)
    {
      int type = int(raw_data[pos]);
      pos++;
      
      for(int j=0; j<type; j++)
	for(int k=0; k<D; k++)
	  {
	    pts[j](k) = raw_data[pos];
	    pos++;
	  }

      if (type == 2)
	{
	  splines[i] = new LineSeg<D>(GeomPoint<D>(pts[0],1),
				      GeomPoint<D>(pts[1],1));
	  //(*testout) << "appending line segment "
	  //	     << pts[0] << " -- " << pts[1] << endl;
	}
      else if (type == 3)
	{
	  splines[i] = new SplineSeg3<D>(GeomPoint<D>(pts[0],1),
					 GeomPoint<D>(pts[1],1),
					 GeomPoint<D>(pts[2],1));
	  //(*testout) << "appending spline segment "
	  //     << pts[0] << " -- " << pts[1] << " -- " << pts[2] << endl;

	}
      else
	throw NgException("something wrong with spline raw data");

    }
  return pos;
}

template<int D>
void SplineGeometry<D> :: GetRawData (ARRAY<double> & raw_data) const
{
  raw_data.Append(D);
  raw_data.Append(elto0);


  raw_data.Append(splines.Size());
  for(int i=0; i<splines.Size(); i++)
    splines[i]->GetRawData(raw_data);
    
  
}

template<int D>
void SplineGeometry<D> :: CSGLoad (CSGScanner & scan)
{
  double hd;
  Point<D> x;
  int nump, numseg;
 
   //scan.ReadNext();
  scan >> nump >> ';';

  hd = 1;
  geompoints.SetSize(nump);
  for(int i = 0; i<nump; i++)
    {
      if(D==2)
	scan >> x(0) >> ',' >> x(1) >> ';';
      else if(D==3)
	scan >> x(0) >> ',' >> x(1) >> ',' >> x(2) >> ';';

      geompoints[i] = GeomPoint<D>(x,hd);
    }

  scan >> numseg;// >> ';';

  splines.SetSize(numseg);

  int pnums,pnum1,pnum2,pnum3;
    

  for(int i = 0; i<numseg; i++)
    {
      scan >> ';' >> pnums >> ',';
      if (pnums == 2)
	{
	  scan >> pnum1 >> ',' >> pnum2;// >> ';';
	  splines[i] = new LineSeg<D>(geompoints[pnum1-1],
				      geompoints[pnum2-1]);
	}
      else if (pnums == 3)
	{
	  scan >> pnum1 >> ',' >> pnum2 >> ',' 
	       >> pnum3;// >> ';';
	  splines[i] = new SplineSeg3<D>(geompoints[pnum1-1],
					 geompoints[pnum2-1],
					 geompoints[pnum3-1]);
	}
      else if (pnums == 4)
	{
	  scan >> pnum1 >> ',' >> pnum2 >> ',' 
	       >> pnum3;// >> ';';
	  splines[i] = new CircleSeg<D>(geompoints[pnum1-1],
					geompoints[pnum2-1],
					geompoints[pnum3-1]);

	}

    }

    
    
    
}




template<int D>
void SplineGeometry<D> :: Load (const char * filename)
{

  ifstream infile;
  int nump, numseg, leftdom, rightdom;
  Point<D> x;
  int hi1, hi2, hi3;
  double hd;
  char buf[50], ch;
  int pointnr;


  infile.open (filename);
  
  if ( ! infile.good() )
    throw NgException(string ("Input file '") + 
		      string (filename) +
		      string ("' not available!"));

  TestComment ( infile );
  
  infile >> buf;   // file recognition

  tensormeshing.SetSize(0);
  quadmeshing.SetSize(0);

  TestComment ( infile );
  if ( strcmp (buf, "splinecurves2dnew") == 0 )
    {
      LoadDataNew ( infile );
    }
  else if ( strcmp (buf, "splinecurves2dv2") == 0 )
    {
      LoadDataV2 ( infile );
    }
  else
    {
      cout << "load fileversion 1" << endl;
      LoadData(infile );
    }
  infile.close();
}


template<int D>
void SplineGeometry<D> :: LoadDataNew ( ifstream & infile )
{

  int nump, numseg, leftdom, rightdom;
  Point<D> x;
  int hi1, hi2, hi3;
  double hd;
  char buf[50], ch;
  int pointnr;


  TestComment ( infile );
  infile >> elto0;
  TestComment ( infile );
      
  infile >> nump;
  geompoints.SetSize(nump);
      
  for (int i = 0; i < nump; i++)
    {
      TestComment ( infile );
      infile >> pointnr;
      if ( pointnr > nump )
	{
	  throw NgException(string ("Point number greater than total number of points") );
	}
      for(int j=0; j<D; j++)
	infile >> x(j);


      // hd is now optional, default 1
      //  infile >> hd;
      hd = 1;

      Flags flags;


      // get flags, 
      ch = 'a';
      // infile >> ch;
      do 
	{

	  infile.get (ch);
	  // if another int-value, set refinement flag to this value
	  // (corresponding to old files)
	  if ( int (ch) >= 48 && int(ch) <= 57 )
	    {
	      infile.putback(ch);
	      infile >> hd;
	      infile.get(ch);
	    }
	} 
      while (isspace(ch) && ch != '\n');
      while (ch == '-')
	{
	  char flag[100];
	  flag[0]='-';
	  infile >> (flag+1);
	  flags.SetCommandLineFlag (flag);
	  ch = 'a';
	  do {
	    infile.get (ch);
	  } while (isspace(ch) && ch != '\n');
	}
    
      if (infile.good())
	infile.putback (ch);

      if ( hd == 1 )
	hd = flags.GetNumFlag ( "ref", 1.0);
      //       geompoints.Append (GeomPoint<D>(x, hd));
      geompoints[pointnr-1] = GeomPoint<D>(x, hd);
      geompoints[pointnr-1].hpref = flags.GetDefineFlag ("hpref");
    }

  TestComment ( infile );

  infile >> numseg;
  bcnames.SetSize(numseg);
  for ( int i = 0; i < numseg; i++ )
    bcnames[i] = 0;//new"default";

  SplineSegment * spline = 0;
  for (int i = 0; i < numseg; i++)
    {
      TestComment ( infile );
      
      infile >> leftdom >> rightdom;

      // cout << "add spline " << i << ", left = " << leftdom << endl;

      infile >> buf;
      // type of spline segement
      if (strcmp (buf, "2") == 0)
	{ // a line
	  infile >> hi1 >> hi2;
	  spline = new LineSegment(geompoints[hi1-1],
				   geompoints[hi2-1]);
	}
      else if (strcmp (buf, "3") == 0)
	{ // a rational spline
	  infile >> hi1 >> hi2 >> hi3;
	  spline = new SplineSegment3 (geompoints[hi1-1],
				       geompoints[hi2-1],
				       geompoints[hi3-1]);
	}
      else if (strcmp (buf, "4") == 0)
	{ // an arc
	  infile >> hi1 >> hi2 >> hi3;
	  spline = new CircleSegment (geompoints[hi1-1],
				      geompoints[hi2-1],
				      geompoints[hi3-1]);
// 	  break;
	}
      else if (strcmp (buf, "discretepoints") == 0)
	{
	  int npts;
	  infile >> npts;
	  ARRAY< Point<D> > pts(npts);
	  for (int j = 0; j < npts; j++)
	    for(int k=0; k<D; k++)
	      infile >> pts[j](k);

	  spline = new DiscretePointsSegment (pts);
          if (printmessage_importance>0)
            cout << "pts = " << pts << endl;
	}
    
      //      infile >> spline->reffak;
      spline -> leftdom = leftdom;
      spline -> rightdom = rightdom;
      splines.Append (spline);

      // hd is now optional, default 1
      //  infile >> hd;
      hd = 1;
      infile >> ch;
      
      // get refinement parameter, if it is there
      // infile.get (ch);
      // if another int-value, set refinement flag to this value
      // (corresponding to old files)
      if ( int (ch) >= 48 && int(ch) <= 57 )
	{
	  infile.putback(ch);
	  infile >> hd;
	  infile >> ch ;
	}
      
      Flags flags;
      while (ch == '-')
	{
	  char flag[100];
	  flag[0]='-';
	  infile >> (flag+1);
	  flags.SetCommandLineFlag (flag);
	  ch = 'a';
	  infile >> ch;
	}
    
      if (infile.good())
	infile.putback (ch);
    
      splines.Last()->bc = int (flags.GetNumFlag ("bc", i+1));
      splines.Last()->hpref_left = int (flags.GetDefineFlag ("hpref")) || 
	int (flags.GetDefineFlag ("hprefleft"));
      splines.Last()->hpref_right = int (flags.GetDefineFlag ("hpref")) || 
	int (flags.GetDefineFlag ("hprefright"));
      splines.Last()->copyfrom = int (flags.GetNumFlag ("copy", -1));
      splines.Last()->reffak = flags.GetNumFlag ("ref", 1 );

      if ( flags.StringFlagDefined("bcname") )
	{
	  int mybc = splines.Last()->bc-1;
	  if ( bcnames[mybc] ) delete bcnames[mybc];
	  bcnames[mybc] = new string (flags.GetStringFlag("bcname","") );
	}

      if ( hd != 1 )
	splines.Last()->reffak = hd;
    }
  if ( !infile.good() )
    return;
  TestComment ( infile );
  int numdomains;
  int domainnr;
  char material[100];

  if ( !infile.good() ) 
    return;

  infile >> numdomains;
  materials.SetSize(numdomains) ;
  maxh.SetSize ( numdomains ) ;
  for ( int i = 0; i < numdomains; i++)
    maxh[i] = 1000;

  TestComment ( infile );

  for ( int i=0; i<numdomains; i++)
    materials [ i ] = new char (100);

  for ( int i=0; i<numdomains && infile.good(); i++)
    {
      TestComment ( infile );
      infile >> domainnr;
      infile >> material;
      strcpy(materials[domainnr-1], material);

      Flags flags;
      ch = 'a';
      infile >> ch;
      while (ch == '-')
	{
	  char flag[100];
	  flag[0]='-';
	  infile >> (flag+1);
	  flags.SetCommandLineFlag (flag);
	  ch = 'a';
	  infile >> ch;
	}
    
      if (infile.good())
	infile.putback (ch);
	 
      maxh[domainnr-1] = flags.GetNumFlag ( "maxh", 1000);
    }
  return;
}



template<int D>
void SplineGeometry<D> :: LoadData ( ifstream & infile )
{      

  int nump, numseg, leftdom, rightdom;
  Point<D> x;
  int hi1, hi2, hi3;
  double hd;
  char buf[50], ch;
  int pointnr;

  materials.SetSize(0);
  maxh.SetSize(0);
  infile >> elto0;

  TestComment ( infile );

  infile >> nump;
  for (int i = 0; i < nump; i++)
    {
      TestComment ( infile );
      for(int j=0; j<D; j++)
	infile >> x(j);
      infile >> hd;

      Flags flags;

      ch = 'a';
      // infile >> ch;
      do {
	infile.get (ch);
      } while (isspace(ch) && ch != '\n');
      while (ch == '-')
	{
	  char flag[100];
	  flag[0]='-';
	  infile >> (flag+1);
	  flags.SetCommandLineFlag (flag);
	  ch = 'a';
	  do {
	    infile.get (ch);
	  } while (isspace(ch) && ch != '\n');
	}
    
      if (infile.good())
	infile.putback (ch);

      geompoints.Append (GeomPoint<D>(x, hd));
      geompoints.Last().hpref = flags.GetDefineFlag ("hpref");

      cout << "p = " << x << endl;
    }

  cout << nump << " points loaded" << endl;
  TestComment ( infile );

  infile >> numseg;
  bcnames.SetSize(numseg);
  for ( int i = 0; i < numseg; i++ )
    bcnames[i] = 0; // "default";

  SplineSegment * spline = 0;
  cout << "numseg = " << numseg << endl;
  for (int i = 0; i < numseg; i++)
    {
      TestComment ( infile );
      
      infile >> leftdom >> rightdom;

      cout << "add spline " << i << ", left = " << leftdom << ", right = " << rightdom << endl;
      
      infile >> buf;
      // type of spline segement
      if (strcmp (buf, "2") == 0)
	{ // a line
	  infile >> hi1 >> hi2;
	  spline = new LineSegment(geompoints[hi1-1],
				   geompoints[hi2-1]);
	}
      else if (strcmp (buf, "3") == 0)
	{ // a rational spline
	  infile >> hi1 >> hi2 >> hi3;
	  spline = new SplineSegment3 (geompoints[hi1-1],
				       geompoints[hi2-1],
				       geompoints[hi3-1]);
	}
      else if (strcmp (buf, "4") == 0)
	{ // an arc
	  infile >> hi1 >> hi2 >> hi3;
	  spline = new CircleSegment (geompoints[hi1-1],
				      geompoints[hi2-1],
				      geompoints[hi3-1]);
// 	  break;
	}
      else if (strcmp (buf, "discretepoints") == 0)
	{
	  int npts;
	  infile >> npts;
	  ARRAY< Point<D> > pts(npts);
	  for (int j = 0; j < npts; j++)
	    for(int k=0; k<D; k++)
	      infile >> pts[j](k);

	  spline = new DiscretePointsSegment (pts);
          if (printmessage_importance>0)
            cout << "pts = " << pts << endl;
	}
    
      infile >> spline->reffak;
      spline -> leftdom = leftdom;
      spline -> rightdom = rightdom;
      splines.Append (spline);


      Flags flags;
      ch = 'a';
      infile >> ch;
      while (ch == '-')
	{
	  char flag[100];
	  flag[0]='-';
	  infile >> (flag+1);
	  flags.SetCommandLineFlag (flag);
	  ch = 'a';
	  infile >> ch;
	}
    
      if (infile.good())
	infile.putback (ch);
    
      splines.Last()->bc = int (flags.GetNumFlag ("bc", i+1));
      splines.Last()->hpref_left = int (flags.GetDefineFlag ("hpref")) || 
	int (flags.GetDefineFlag ("hprefleft"));
      splines.Last()->hpref_right = int (flags.GetDefineFlag ("hpref")) || 
	int (flags.GetDefineFlag ("hprefright"));
      splines.Last()->copyfrom = int (flags.GetNumFlag ("copy", -1));
      if ( flags.StringFlagDefined("bcname") )
	{
	  int mybc = splines.Last()->bc-1;
	  if ( bcnames[mybc] ) delete bcnames[mybc];
	  bcnames[mybc] = new string (flags.GetStringFlag("bcname","") );
	}
    }
  cout << numseg << " segments loaded" << endl;
}



template<int D>
void SplineGeometry<D> :: PartitionBoundary (double h, Mesh & mesh2d)
{
  Box<D> bbox;
  GetBoundingBox (bbox);
  double dist = Dist (bbox.PMin(), bbox.PMax());
  Point<D+1> pmin;
  Point<D+1> pmax;

  for(int j=0;j<D;j++)
    {
      pmin(j) = bbox.PMin()(j);
      pmax(j) = bbox.PMax()(j);
    }
  pmin(D) = -dist; pmax(D) = dist;


  if (printmessage_importance>0)
    cout << "searchtree from " << pmin << " to " << pmax << endl;
  Point3dTree searchtree (pmin, pmax);

  for (int i = 0; i < splines.Size(); i++)
    if (splines[i]->copyfrom == -1)
      {
	// astrid - set boundary meshsize to  domain meshsize h
	// if no domain mesh size is given, the max h value from the bounding box is used
 	double minimum = min2 ( GetDomainMaxh ( splines[i]->leftdom ), GetDomainMaxh ( splines[i]->rightdom ) );
 	double maximum = max2 ( GetDomainMaxh ( splines[i]->leftdom ), GetDomainMaxh ( splines[i]->rightdom ) );
	minimum = min2 ( minimum, h );
	maximum = min2 ( maximum, h);
 	if ( minimum > 0 )
 	  splines[i]->Partition(minimum, elto0, mesh2d, searchtree, i+1);
 	else if ( maximum > 0 )
 	  splines[i]->Partition(maximum, elto0, mesh2d, searchtree, i+1);
 	else
	  splines[i]->Partition(h, elto0, mesh2d, searchtree, i+1);
      }
    else
      {
	CopyEdgeMesh (splines[i]->copyfrom, i+1, mesh2d, searchtree);
      }
}


template<int D>
void SplineGeometry<D> :: CopyEdgeMesh (int from, int to, Mesh & mesh, Point3dTree & searchtree)
{
  int i, j, k;

  ARRAY<int, PointIndex::BASE> mappoints (mesh.GetNP());
  ARRAY<double, PointIndex::BASE> param (mesh.GetNP());
  mappoints = -1;
  param = 0;

  Point3d pmin, pmax;
  mesh.GetBox (pmin, pmax);
  double diam2 = Dist2(pmin, pmax);

  if (printmessage_importance>0)
    cout << "copy edge, from = " << from << " to " << to << endl;
  
  for (i = 1; i <= mesh.GetNSeg(); i++)
    {
      const Segment & seg = mesh.LineSegment(i);
      if (seg.edgenr == from)
	{
	  mappoints.Elem(seg.p1) = 1;
	  param.Elem(seg.p1) = seg.epgeominfo[0].dist;

	  mappoints.Elem(seg.p2) = 1;
	  param.Elem(seg.p2) = seg.epgeominfo[1].dist;
	}
    }

  bool mapped = false;
  for (i = 1; i <= mappoints.Size(); i++)
    {
      if (mappoints.Get(i) != -1)
	{
	  Point<D> newp = splines.Get(to)->GetPoint (param.Get(i));
	  Point<3> newp3;
	  for(int j=0; j<min2(D,3); j++)
	    newp3(j) = newp(j);
	  for(int j=min2(D,3); j<3; j++)
	    newp3(j) = 0;
	  
	  int npi = -1;
	  
	  for (PointIndex pi = PointIndex::BASE; 
	       pi < mesh.GetNP()+PointIndex::BASE; pi++)
	    if (Dist2 (mesh.Point(pi), newp3) < 1e-12 * diam2)
	      npi = pi;
	  
	  if (npi == -1)
	    {
	      npi = mesh.AddPoint (newp3);
	      searchtree.Insert (newp3, npi);
	    }

	  mappoints.Elem(i) = npi;

	  mesh.GetIdentifications().Add (i, npi, to);
	  mapped = true;
	}
    }
  if(mapped)
    mesh.GetIdentifications().SetType(to,Identifications::PERIODIC);

  // copy segments
  int oldnseg = mesh.GetNSeg();
  for (i = 1; i <= oldnseg; i++)
    {
      const Segment & seg = mesh.LineSegment(i);
      if (seg.edgenr == from)
	{
	  Segment nseg;
	  nseg.edgenr = to;
	  nseg.si = splines.Get(to)->bc;
	  nseg.p1 = mappoints.Get(seg.p1);
	  nseg.p2 = mappoints.Get(seg.p2);
	  nseg.domin = splines.Get(to)->leftdom;
	  nseg.domout = splines.Get(to)->rightdom;
	  
	  nseg.epgeominfo[0].edgenr = to;
	  nseg.epgeominfo[0].dist = param.Get(seg.p1);
	  nseg.epgeominfo[1].edgenr = to;
	  nseg.epgeominfo[1].dist = param.Get(seg.p2);
	  mesh.AddSegment (nseg);
	}
    }
}
  

template<int D>
void SplineGeometry<D> :: GetBoundingBox (Box<D> & box) const
{
  if (!splines.Size())
    {
      Point<D> auxp = 0.;
      box.Set (auxp);
      return;
    }

  ARRAY<Point<D> > points;
  for (int i = 0; i < splines.Size(); i++)
    {
      splines[i]->GetPoints (20, points);

      if (i == 0) box.Set(points[0]);
      for (int j = 0; j < points.Size(); j++)
	box.Add (points[j]);
    }
}

template<int D>
void SplineGeometry<D> :: SetGrading (const double grading)
{ elto0 = grading;}

template<int D>
void SplineGeometry<D> :: AppendPoint (const double x, const double y, const double reffac, const bool hpref)
{
  geompoints.Append (GeomPoint<D>(x, y, reffac));
  geompoints.Last().hpref = hpref;
}

template<int D>
void SplineGeometry<D> :: AppendPoint (const Point<D> & p, const double reffac, const bool hpref)
{
  geompoints.Append (GeomPoint<D>(p, reffac));
  geompoints.Last().hpref = hpref;
}




template<int D>
void SplineGeometry<D> :: AppendSegment(SplineSeg<D> * spline, const int leftdomain, const int rightdomain, 
					const int bc, 
					const double reffac, const bool hprefleft, const bool hprefright,
					const int copyfrom)
{
  spline -> leftdom = leftdomain;
  spline -> rightdom = rightdomain;
  spline -> bc = (bc >= 0) ? bc : (splines.Size()+1);
  spline -> reffak = reffac;
  spline -> hpref_left = hprefleft;
  spline -> hpref_right = hprefright;
  spline -> copyfrom = copyfrom;
  
  splines.Append(spline);
}

template<int D>
void SplineGeometry<D> :: AppendLineSegment (const int n1, const int n2, const int leftdomain, const int rightdomain,
					     const int bc, 
					     const double reffac, const bool hprefleft, const bool hprefright,
					     const int copyfrom)
{
  SplineSeg<D> * spline = new LineSeg<D>(geompoints[n1],geompoints[n2]);
  AppendSegment(spline,leftdomain,rightdomain,bc,reffac,hprefleft,hprefright,copyfrom);  
}

template<int D>
void SplineGeometry<D> :: AppendSplineSegment (const int n1, const int n2, const int n3, const int leftdomain, const int rightdomain, 
					       const int bc,
					       const double reffac, const bool hprefleft, const bool hprefright,
					       const int copyfrom)
{
  SplineSeg<D> * spline = new SplineSeg3<D>(geompoints[n1],geompoints[n2],geompoints[n3]);
  AppendSegment(spline,leftdomain,rightdomain,bc,reffac,hprefleft,hprefright,copyfrom);
}

template<int D>
void SplineGeometry<D> :: AppendCircleSegment (const int n1, const int n2, const int n3, const int leftdomain, const int rightdomain,
					       const int bc,  
					       const double reffac, const bool hprefleft, const bool hprefright,
					       const int copyfrom)
{
  SplineSeg<D> * spline = new CircleSeg<D>(geompoints[n1],geompoints[n2],geompoints[n3]);
  AppendSegment(spline,leftdomain,rightdomain,bc,reffac,hprefleft,hprefright,copyfrom);
}

template<int D>
void SplineGeometry<D> :: AppendDiscretePointsSegment (const ARRAY< Point<D> > & points, const int leftdomain, const int rightdomain, 
						       const int bc, 
						       const double reffac, const bool hprefleft, const bool hprefright,
						       const int copyfrom)
{
  SplineSeg<D> * spline = new DiscretePointsSeg<D>(points);
  AppendSegment(spline,leftdomain,rightdomain,bc,reffac,hprefleft,hprefright,copyfrom);
}


 template<int D>
 void	SplineGeometry<D> :: GetMaterial( const int  domnr, char* & material )
 {
    if ( materials.Size() >= domnr)
      material =  materials[domnr-1];
    else material = 0;
   return;
 }



 template<int D>
 double	SplineGeometry<D> :: GetDomainMaxh( const int  domnr )
 {
   if ( maxh.Size() >= domnr  && domnr > 0)
     return maxh[domnr-1];
   else
     return -1;
 }


 template<int D>
 string	SplineGeometry<D> :: GetBCName( const int  bcnr ) const
 {
   string bcname;
   if ( bcnames.Size() >= bcnr)
     if ( bcnames[bcnr-1] )
       bcname =  *bcnames[bcnr-1];
     else bcname = "default";
   return bcname;
 }

template<int D>
string * SplineGeometry<D> :: BCNamePtr( const int bcnr ) 
{
  if ( bcnr > bcnames.Size() )
    return 0;
  else
    return bcnames[bcnr-1];
}

typedef SplineGeometry<2> SplineGeometry2d;


#endif // _FILE_SPLINEGEOMETRY
