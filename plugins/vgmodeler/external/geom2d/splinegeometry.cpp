#ifndef TESTCOMMENT2
#define TESTCOMMENT2
/*

2d Spline curve for Mesh generator

*/
#include <mystdlib.h>
#include <csg.hpp>
#include <geometry2d.hpp>
#include "meshing.hpp"

namespace vgmnetgen
{

  using namespace vgmnetgen;

template<int D>
void SplineGeometry<D> :: LoadDataV2 ( ifstream & infile )
{
  cout << "Load Geometry V2" << endl;
  int nump, numseg, leftdom, rightdom;
  Point<D> x;
  int hi1, hi2, hi3;
  double hd;
  char buf[50], ch;
  int pointnr;

  string keyword;

  ARRAY < GeomPoint<2> > infilepoints (0);
  ARRAY <int> pointnrs (0);
  nump = 0;
  int numdomains = 0;


  TestComment ( infile );
  // refinement factor
  infile >> elto0;
  TestComment ( infile );
      

  // test if next ch is a letter, i.e. new keyword starts
  bool ischar = false;

  while ( infile.good() )
    {
      infile >> keyword;

      ischar = false;

      if ( keyword == "points" )
	{
          cout << "load points" << endl;
	  infile.get(ch);
	  infile.putback(ch);

	  // test if ch is a letter
	  if ( int(ch) >= 65 && int(ch) <=90 )
	    ischar = true;
	  if ( int(ch) >= 97 && int(ch) <= 122 )
	    ischar = true;

	  while ( ! ischar )
	    {
	      TestComment ( infile );
	      infile >> pointnr;
	      // pointnrs 1-based
	      if ( pointnr > nump ) nump = pointnr; 
	      pointnrs.Append(pointnr);
	      
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

	      infilepoints.Append ( GeomPoint<D>(x, hd) );
	      infilepoints.Last().hpref = flags.GetDefineFlag ("hpref");

	      TestComment(infile);
	      infile.get(ch);
	      infile.putback(ch);

	      // test if letter
	      if ( int(ch) >= 65 && int(ch) <=90 )
		ischar = true;
	      if ( int(ch) >= 97 && int(ch) <= 122 )
		ischar = true;
	    }

	  //	  infile.putback (ch);

	  geompoints.SetSize(nump);
	  for ( int i = 0; i < nump; i++ )
	    {
	      geompoints[pointnrs[i] - 1] = infilepoints[i];
	      geompoints[pointnrs[i] - 1].hpref = infilepoints[i].hpref; 
	    }
	  TestComment(infile);
	}

      else if ( keyword == "segments" )
	{
          cout << "load segments" << endl;

          bcnames.SetSize(0);
	  infile.get(ch);
	  infile.putback(ch);
	  int i = 0;

	  // test if ch is a letter
	  if ( int(ch) >= 65 && int(ch) <=90 )
	    ischar = true;
	  if ( int(ch) >= 97 && int(ch) <= 122 )
	    ischar = true;

	  while ( !ischar ) //ch != 'p' && ch != 'm' )
	    {
	      i++;
	      TestComment ( infile );

	      SplineSegment * spline = 0;
	      TestComment ( infile );
		  
	      infile >> leftdom >> rightdom;
	      
	      if ( leftdom > numdomains ) numdomains = leftdom;
	      if ( rightdom > numdomains ) numdomains = rightdom;

	      
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
		      break;
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
	      //infile.get (ch);
	      // if another int-value, set refinement flag to this value
	      // (corresponding to old files)

	      if ( int (ch) >= 48 && int(ch) <= 57 )
		{
		  infile.putback(ch);
		  infile >> hd;
		  infile >> ch ;
		}

	      // get flags, 
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
	      if ( hd != 1 )
		splines.Last()->reffak = hd;

	      if ( flags.StringFlagDefined("bcname") )
		{
		  int mybc = splines.Last()->bc-1;
		  for ( int ii = bcnames.Size(); ii <= mybc; ii++ )
		    bcnames.Append ( new string ("default"));
		  if ( bcnames[mybc] ) delete bcnames[mybc];
		  bcnames[mybc] = new string (flags.GetStringFlag("bcname","") );
		}

	      TestComment(infile);
	      infile.get(ch);
	      infile.putback(ch);

	      // test if ch is a letter
	      if ( int(ch) >= 65 && int(ch) <=90 )
		ischar = true;
	      if ( int(ch) >= 97 && int(ch) <= 122 )
		ischar = true;

	    }
	  
	  infile.get(ch);
	  infile.putback(ch);
	

	}
      else if ( keyword == "materials" )
	{
	  TestComment ( infile );
	  int domainnr;
	  char material[100];
	  
	  if ( !infile.good() ) 
	    return;
	  
	  materials.SetSize(numdomains) ;
	  maxh.SetSize ( numdomains ) ;
	  for ( int i = 0; i < numdomains; i++)
	    maxh[i] = 1000;
          quadmeshing.SetSize ( numdomains );
          quadmeshing = false;
          tensormeshing.SetSize ( numdomains );
          tensormeshing = false;

	  
	  TestComment ( infile );
	  
	  for ( int i=0; i<numdomains; i++)
	    materials [ i ] = new char[100];
	  
	  for ( int i=0; i<numdomains && infile.good(); i++)
	    {
	      TestComment ( infile );
	      infile >> domainnr;
	      infile >> material;

	      strcpy (materials[domainnr-1], material);
	      
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
              if (flags.GetDefineFlag("quad")) quadmeshing[domainnr-1] = true;
              if (flags.GetDefineFlag("tensor")) tensormeshing[domainnr-1] = true;
	    }
	}
    }
  return;
}


  template class SplineGeometry<2>;
  //  template class SplineGeometry<3>;
}


#endif
