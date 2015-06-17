//20.11.1999 third part of stlgeom.cc, functions with chart and atlas

#include <mystdlib.h>

#include <myadt.hpp>
#include <linalg.hpp>
#include <gprim.hpp>

#include <meshing.hpp>

// [JW] viennagrid has to be included prior to
// stlgeom, as the viennagrid domain has to be used within
// stltopology.hpp
#include "viennagridpp/mesh/mesh.hpp"
// #include "viennagrid/config/simplex.hpp"


#include "stlgeom.hpp"

namespace vgmnetgen
{

int chartdebug = 0;



void STLGeometry :: MakeAtlas(Mesh & mesh)
{

  double h, h2;

  h = mparam.maxh;

#ifdef DEBUGALL
  std::cout << "[FS] .. in MakeAtlas.. GetNE: " << GetNE() << std::endl;
#endif

//  PushStatusF("Make Atlas");

  int i,j,k,l,m,ctl;

  double atlasminh = 5e-3 * Dist (boundingbox.PMin(), boundingbox.PMax());
//  PrintMessage(5, "atlasminh = ", atlasminh);

  //speedup for make atlas
  if (GetNT() > 50000)
    {
      mesh.SetGlobalH(0.05*Dist (boundingbox.PMin(), boundingbox.PMax()));
    }

//   atlas_t atlas;
  atlas.SetSize(0);
  ClearSpiralPoints();

  // [FS] why is this commented ???
  //
  //  BuildSmoothEdges();
#ifdef DEBUGALL
  std::cout << ".. GetNE(): " << GetNE() << std::endl;
  std::cout << ".. GetNP(): " << GetNP() << std::endl;
  std::cout << ".. GetNT(): " << GetNT() << std::endl;
#endif

  double chartangle = stlparam.chartangle;
  double outerchartangle = stlparam.outerchartangle;

  chartangle = chartangle/180.*M_PI;
  outerchartangle = outerchartangle/180.*M_PI;

  double coschartangle = cos(chartangle);
  double cosouterchartangle = cos(outerchartangle);
  double cosouterchartanglehalf = cos(0.5*outerchartangle);
  double sinchartangle = sin(chartangle);
  double sinouterchartangle = sin(outerchartangle);

  ARRAY<int> outermark(GetNT()); //marks all trigs form actual outer region
  ARRAY<int> outertested(GetNT()); //marks tested trigs for outer region
  ARRAY<int> pointstochart(GetNP()); //point in chart becomes chartnum
  ARRAY<int> innerpointstochart(GetNP()); //point in chart becomes chartnum
  ARRAY<int> chartpoints; //point in chart becomes chartnum
  ARRAY<int> innerchartpoints;
  ARRAY<int> dirtycharttrigs;
  ARRAY<int> chartpointchecked;

  ARRAY<int> chartdistacttrigs; //outercharttrigs
  chartdistacttrigs.SetSize(GetNT());
  for (i = 1; i <= GetNT(); i++)
    {
      chartdistacttrigs.Elem(i) = 0;
    }

  STLBoundary chartbound(this); //knows the actual chart boundary
  int chartboundarydivisions = 10;
  markedsegs.SetSize(0); //for testing!!!

  chartpointchecked.SetSize(GetNP()); //for dirty-chart-trigs

  outermark.SetSize(GetNT());
  outertested.SetSize(GetNT());
  pointstochart.SetSize(GetNP());
  innerpointstochart.SetSize(GetNP());
  chartmark.SetSize(GetNT());

  for (i = 1; i <= GetNP(); i++)
    {
      innerpointstochart.Elem(i) = 0;
      pointstochart.Elem(i) = 0;
      chartpointchecked.Elem(i) = 0;
    }

  double eps = 1e-12 * Dist (boundingbox.PMin(), boundingbox.PMax());

  int spiralcheckon = stldoctor.spiralcheck;

#ifdef DEBUGALL
  if (!spiralcheckon) {PrintWarning("++++++++++++\nspiral deactivated by user!!!!\n+++++++++++++++"); }
#endif

  for (i = 1; i <= GetNT(); i++)
    {
      chartmark.Elem(i) = 0;
    }

  for (i = 1; i <= GetNT(); i++)
    {
      outermark.Elem(i) = 0;
      outertested.Elem(i) = 0;
    }

  int markedtrigcnt = 0;
  int found = 1;
  double atlasarea = Area();
  double workedarea = 0;
  double showinc = 100.*5000./(double)GetNT();
  double nextshow = 0;
  Point<3> startp;
  int lastunmarked = 1;
  int prelastunmarked;

//#ifdef DEBUGALL
//  PrintMessage(5,"one dot per 5000 triangles: ");
//#endif

  while(markedtrigcnt < GetNT() && found)
    {
      if (multithread.terminate)  {PopStatus();return;}

      if (workedarea / atlasarea*100. >= nextshow)
      {
        //PrintDot(); // [JW] deactivated the 'dot' print ..
        nextshow+=showinc;
      }

      SetThreadPercent(100.0 * workedarea / atlasarea);

      /*
      for (j = 1; j <= GetNT(); j++)
	{
	  outermark.Elem(j) = 0;
	}
      */

#ifdef DEBUGALL
      std::cout << "[FS] .. before chart creation" << std::endl;
#endif

      STLChart * chart = new STLChart(this);
      atlas.Append(chart);

      //find unmarked trig
      prelastunmarked = lastunmarked;
      j = lastunmarked;
      found = 0;
      while (!found && j <= GetNT())
	{
	  if (!GetMarker(j))
	    {
	      found = 1;
	      lastunmarked = j;

#ifdef DEBUGALL
	      std::cout << "[FS] .. marker found .. j : " << j << std::endl;
#endif
	    }
	  else
	    {
	      j++;

#ifdef DEBUGALL
	      std::cout << "[FS] .. marker not found .. j : " << j << std::endl;
#endif
	    }
	}

      chartpoints.SetSize(0);
      innerchartpoints.SetSize(0);
      chartbound.Clear();
      chartbound.SetChart(chart);

#ifdef DEBUGALL
      if (!found) {PrintSysError("Make Atlas, no starttrig found"); return;}
#endif

      //find surrounding trigs
      int starttrig = j;

      double mindist, tdist;
      startp = GetPoint(GetTriangle(starttrig).PNum(1));

#ifdef DEBUGALL
      std::cout << ".. here 1 " << std::endl;
#endif

      int accepted;

      // [FS] .. save chart size in chartnum, which is the current chart id
      //
      int chartnum = GetNOCharts();

      // [FS] .. edit
      // int chartnum = atlas.Size();

#ifdef DEBUGALL
      std::cout << "[FS] .. number of charts: " << chartnum << std::endl;
#endif


      Vec<3> sn = GetTriangle(starttrig).Normal();
      chart->SetNormal (startp, sn);

#ifdef DEBUGALL
      std::cout << "[FS] .. starttrig: " << starttrig << " :: " << GetTriangle(starttrig) << std::endl;
#endif

      SetMarker(starttrig, chartnum);
      markedtrigcnt++;
      chart->AddChartTrig(starttrig);
      chartbound.AddTriangle(GetTriangle(starttrig));

      workedarea += GetTriangle(starttrig).Area(points);

      for (i = 1; i <= 3; i++)
	{
	  innerpointstochart.Elem(GetTriangle(starttrig).PNum(i)) = chartnum;
	  pointstochart.Elem(GetTriangle(starttrig).PNum(i)) = chartnum;
	  chartpoints.Append(GetTriangle(starttrig).PNum(i));
	  innerchartpoints.Append(GetTriangle(starttrig).PNum(i));
	}

      Vec<3> n2, n3;
      int changed = 1;
      int nt;
      // [FS] .. ic = inner chart ?
      int ic;
      int oldstartic = 1;
      int oldstartic2;
      int np1, np2;

#ifdef DEBUGALL
      std::cout << "[FS] .. before while .. " << std::endl;
#endif

      while (changed)
	{
	  changed = 0;
	  oldstartic2 = oldstartic;
	  oldstartic = chart->GetNT();

#ifdef DEBUGALL
	  std::cout << "[FS] .. oldstartic: " << oldstartic << std::endl;
#endif

	  //	      for (ic = oldstartic2; ic <= chart->GetNT(); ic++)
	  for (ic = oldstartic2; ic <= oldstartic; ic++)
	    {
	      i = chart->GetTrig(ic);

#ifdef DEBUGALL
	      std::cout << "..checkpoint A" << std::endl;
#endif

	      if (GetMarker(i) == chartnum)
		{
		  for (j = 1; j <= NONeighbourTrigs(i); j++)
		    {
		      nt = NeighbourTrig(i,j);

#ifdef DEBUGALL
		      std::cout << "trig1: " << GetTriangle(i) << " :: trig2: " << GetTriangle(nt) << std::endl;
#endif

		      GetTriangle(i).GetNeighbourPoints(GetTriangle(nt),np1,np2);

#ifdef DEBUGALL
		      std::cout << "..checkpoint B: np1: " << np1 << " :: np2: " << np2 << std::endl;
#endif

		      if (GetMarker(nt) == 0 && !IsEdge(np1,np2))
			{
			  n2 = GetTriangle(nt).Normal();

#ifdef DEBUGALL
			  std::cout << "..checkpoint C" << std::endl;
#endif

			  if ( (n2 * sn) >= coschartangle )
			    {
#ifdef DEBUGALL
			      std::cout << "..checkpoint D" << std::endl;
#endif
			      accepted = 1;

			      int nnp1, nnp2;
			      int nnt;

#ifdef DEBUGALL
			      std::cout << "[FS] .. find overlapping charts .. " <<std::endl;
#endif

			      //find overlapping charts exacter:
			      for (k = 1; k <= 3; k++)
				{
				  nnt = NeighbourTrig(nt,k);
				  if (GetMarker(nnt) != chartnum)
				    {
				      GetTriangle(nt).GetNeighbourPoints(GetTriangle(nnt),nnp1,nnp2);

				      accepted = chartbound.TestSeg(GetPoint(nnp1),
								    GetPoint(nnp2),
								    sn,sinchartangle,1 /*chartboundarydivisions*/ ,points, eps);

#ifdef DEBUGALL
				      std::cout << "[FS] .. after TestSeg " << std::endl;
#endif

				      n3 = GetTriangle(nnt).Normal();
				      if ( (n3 * sn) >= coschartangle  &&
					   IsSmoothEdge (nnp1, nnp2) )
					accepted = 1;
				    }
				  if (!accepted) {break;}
				}

			      // [FS][INFO] add the neighbour trigs to the chart
			      //
			      if (accepted)
				{
				  SetMarker(nt, chartnum);
				  changed = 1;
				  markedtrigcnt++;
				  workedarea += GetTriangle(nt).Area(points);
				  chart->AddChartTrig(nt);

				  chartbound.AddTriangle(GetTriangle(nt));

				  for (k = 1; k <= 3; k++)
				    {
				      if (innerpointstochart.Get(GetTriangle(nt).PNum(k))
					  != chartnum)
					{
					  innerpointstochart.Elem(GetTriangle(nt).PNum(k)) = chartnum;
					  pointstochart.Elem(GetTriangle(nt).PNum(k)) = chartnum;
					  chartpoints.Append(GetTriangle(nt).PNum(k));
					  innerchartpoints.Append(GetTriangle(nt).PNum(k));
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	}


      //find outertrigs

      //      chartbound.Clear();
      // warum, ic-bound auf edge macht Probleme js ???

#ifdef DEBUGALL
      std::cout << "[FS] .. between while .. " << GetNOCharts() << std::endl;
#endif

      outermark.Elem(starttrig) = chartnum;
      //chart->AddOuterTrig(starttrig);
      changed = 1;
      oldstartic = 1;
      while (changed)
	{
	  changed = 0;
	  oldstartic2 = oldstartic;
	  oldstartic = chart->GetNT();
	  //for (ic = oldstartic2; ic <= chart->GetNT(); ic++)

#ifdef DEBUGALL
	  std::cout << "..checkpoint E" <<std::endl;
#endif

	  for (ic = oldstartic2; ic <= oldstartic; ic++)
	    {
	      i = chart->GetTrig(ic);

#ifdef DEBUGALL
	      std::cout << "..checkpoint F" <<std::endl;
#endif

	      if (outermark.Get(i) == chartnum)
		{
		  for (j = 1; j <= NONeighbourTrigs(i); j++)
		    {
#ifdef DEBUGALL
		      std::cout << "..checkpoint G" <<std::endl;
#endif

		      nt = NeighbourTrig(i,j);
		      if (outermark.Get(nt) == chartnum)
			continue;

		      const STLTriangle & ntrig = GetTriangle(nt);
		      GetTriangle(i).GetNeighbourPoints(GetTriangle(nt),np1,np2);

		      if (IsEdge (np1, np2))
			continue;

#ifdef DEBUGALL
		      std::cout << "..checkpoint H" <<std::endl;
#endif
		      /*
		      if (outertested.Get(nt) == chartnum)
			continue;
		      */
		      outertested.Elem(nt) = chartnum;


		      n2 = GetTriangle(nt).Normal();

		      /*
			double ang;
			ang = Angle(n2,sn);
			if (ang < -M_PI*0.5) {ang += 2*M_PI;}

			(*testout) << "ang < ocharang = " << (fabs(ang) <= outerchartangle);
			(*testout) << " = " << ( (n2 * sn) >= cosouterchartangle) << endl;

			//			      if (fabs(ang) <= outerchartangle)
		      */

#ifdef DEBUGALL
		      std::cout << "..checkpoint I" <<std::endl;
#endif

		      //abfragen, ob noch im tolerierten Winkel
		      if ( (n2 * sn) >= cosouterchartangle )
			{
			  accepted = 1;

			  int isdirtytrig = 0;

#ifdef DEBUGALL
			  std::cout << "..checkpoint I 0 0" <<std::endl;
#endif

			  Vec<3> gn = GetTriangle(nt).GeomNormal(points);

#ifdef DEBUGALL
			  std::cout << "..checkpoint I 0 1" <<std::endl;
#endif

			  double gnlen = gn.Length();

			  if (n2 * gn <= cosouterchartanglehalf * gnlen)
			    {isdirtytrig = 1;}

			  //zurueckweisen, falls eine Spiralartige outerchart entsteht
			  int nnp1, nnp2;
			  int nnt;
			  //find overlapping charts exacter:
			  //do not check dirty trigs!


			  if (spiralcheckon && !isdirtytrig)
			    for (k = 1; k <= 3; k++)
			      {
				nnt = NeighbourTrig(nt,k);

				if (outermark.Elem(nnt) != chartnum)
				  {
#ifdef DEBUGALL
				    std::cout << "..checkpoint I 0 " << std::endl;
#endif

				    GetTriangle(nt).GetNeighbourPoints(GetTriangle(nnt),nnp1,nnp2);

#ifdef DEBUGALL
				    std::cout << "..checkpoint I 1 " << std::endl;
#endif

				    accepted =
				      chartbound.TestSeg(GetPoint(nnp1),GetPoint(nnp2),
							 sn,sinouterchartangle, 0 /*chartboundarydivisions*/ ,points, eps);

#ifdef DEBUGALL
				    std::cout << "..checkpoint I 2 .. accepted: " << accepted << std::endl;
#endif

				    n3 = GetTriangle(nnt).Normal();
				    if ( (n3 * sn) >= cosouterchartangle  &&
					 IsSmoothEdge (nnp1, nnp2) )
				      accepted = 1;
				  }
				if (!accepted) {break;}
			      }

			  //}

#ifdef DEBUGALL
			  std::cout << "..checkpoint J" <<std::endl;
#endif

			  // outer chart is only small environment of
			  //    inner chart:
			  if (accepted)
			    {
			      accepted = 0;

			      for (k = 1; k <= 3; k++)
				{
				  if (innerpointstochart.Get(ntrig.PNum(k)) == chartnum)
				    {
				      accepted = 1;
				      break;
				    }
				}

			      if (!accepted)
				for (k = 1; k <= 3; k++)
				  {
				    Point<3> pt = GetPoint(ntrig.PNum(k));
				    h2 = sqr(mesh.GetH(pt));

				    for (l = 1; l <= innerchartpoints.Size(); l++)
				      {
					tdist = Dist2(pt, GetPoint (innerchartpoints.Get(l)));
					if (tdist < 4 * h2)
					  {
					    accepted = 1;
					    break;
					  }
				      }
				    if (accepted) {break;}
				  }
			    }

#ifdef DEBUGALL
			  std::cout << "..checkpoint K" <<std::endl;
#endif

			  if (accepted)
			    {
			      changed = 1;
			      outermark.Elem(nt) = chartnum;

			      if (GetMarker(nt) != chartnum)
				{
				  chartbound.AddTriangle(GetTriangle(nt));
				  chart->AddOuterTrig(nt);
				  for (k = 1; k <= 3; k++)
				    {
				      if (pointstochart.Get(GetTriangle(nt).PNum(k))
					  != chartnum)
					{
					  pointstochart.Elem(GetTriangle(nt).PNum(k)) = chartnum;
					  chartpoints.Append(GetTriangle(nt).PNum(k));
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	}

#ifdef DEBUGALL
      std::cout << "..after while" <<std::endl;
#endif

      //end of while loop for outer chart
//       GetDirtyChartTrigs(chartnum, *chart, outermark, chartpointchecked, dirtycharttrigs);
      //dirtycharttrigs are local (chart) point numbers!!!!!!!!!!!!!!!!

#ifdef DEBUGALL
      std::cout << "..checkpoint L" <<std::endl;
      std::cout << "..checkpoint test: " << dirtycharttrigs.Size() << std::endl;
#endif

//       if (dirtycharttrigs.Size() != 0 &&
// 	  (dirtycharttrigs.Size() != chart->GetNChartT() || dirtycharttrigs.Size() != 1))
// 	{
// 	  std::cout << "..checkpoint L1" <<std::endl;

// 	  if (dirtycharttrigs.Size() == chart->GetNChartT() && dirtycharttrigs.Size() != 1)
// 	    {
// 	      std::cout << "..checkpoint L2" <<std::endl;

// 	      // if all trigs would be eliminated -> leave 1 trig!
// 	      dirtycharttrigs.SetSize(dirtycharttrigs.Size() - 1);
// 	    }
// 	  for (k = 1; k <= dirtycharttrigs.Size(); k++)
// 	    {
// 	      int tn = chart->GetChartTrig(dirtycharttrigs.Get(k));
// 	      outermark.Elem(tn) = 0; //not necessary, for later use
// 	      SetMarker(tn, 0);
// 	      markedtrigcnt--;
// 	      workedarea -= GetTriangle(tn).Area(points);
// 	    }
// 	  chart->MoveToOuterChart(dirtycharttrigs);
// 	  lastunmarked = 1;
// 	  lastunmarked = prelastunmarked;

// 	  std::cout << "..checkpoint L3" <<std::endl;
// 	}

#ifdef DEBUGALL
      std::cout << "..checkpoint L4" <<std::endl;
#endif

      // calculate an estimate meshsize, not to produce to large outercharts, with factor 2 larger!
      //
      RestrictHChartDistOneChart(chartnum, chartdistacttrigs, mesh, h, 0.5, atlasminh);

#ifdef DEBUGALL
      std::cout << "..checkpoint L5" <<std::endl;
#endif
    }

#ifdef DEBUGALL
  std::cout << "[FS] .. after while .. NO charts: " << atlas.Size() << std::endl;
#endif

#ifdef DEBUGALL
  PrintMessage(5,"");
  PrintMessage(5,"NO charts=", atlas.Size());
#endif

  int cnttrias = 0;
  //int found2;
  outerchartspertrig.SetSize(GetNT());

  for (i = 1; i <= atlas.Size(); i++)
    {
      int j;
      //found2 = 1;
      for (j = 1; j <= GetChart(i).GetNT(); j++)
	{
	  int tn = GetChart(i).GetTrig(j);
	  AddOCPT(tn,i);

	}

      cnttrias += GetChart(i).GetNT();
    }

#ifdef DEBUGALL
  PrintMessage(5, "NO outer chart trias=", cnttrias);
#endif

  //sort outerchartspertrig
  for (i = 1; i <= GetNT(); i++)
    {
      int j,k, swap;
      for (k = 1; k < GetNOCPT(i); k++)
	{

	  for (j = 1; j < GetNOCPT(i); j++)
	    {
	      swap = GetOCPT(i,j);
	      if (GetOCPT(i,j+1) < swap)
		{
		  SetOCPT(i,j,GetOCPT(i,j+1));
		  SetOCPT(i,j+1,swap);
		}
	    }
	}

#ifdef DEBUGALL
      // check make atlas
      if (GetChartNr(i) <= 0 || GetChartNr(i) > GetNOCharts())
	{
	  PrintSysError("Make Atlas: chartnr(", i, ")=0!!");
	};
#endif

    }

  mesh.SetGlobalH(mparam.maxh);
  mesh.SetMinimalH(mparam.minh);

#ifdef DEBUGALL
  std::cout << " .. before AddConeAndSpiralEdges .. GetNE: " << GetNE() << std::endl;
#endif

  // [INFO][FS][TODO] .. using this function the meshing process won't work ... check why !!!!!!!!!!!!!!!!!!!!!!!!!
  //
//   AddConeAndSpiralEdges();

#ifdef DEBUGALL
  std::cout << " .. after AddConeAndSpiralEdges .. GetNE: " << GetNE() << std::endl;
  PrintMessage(5,"Make Atlas finished");
#endif

  PopStatus();

#ifdef DEBUGALL
  std::cout << ".. adding atlas to atlases vector .. " << std::endl;
  std::cout << "[FS] .. leaving MakeAtlas.. GetNE: " << GetNE() << " :: charts: " << atlas.Size() << std::endl;
#endif

}


int STLGeometry::TrigIsInOC(int tn, int ocn) const
{
  if (tn < 1 || tn > GetNT())
    {
      // assert (1);
      abort ();
      PrintSysError("STLGeometry::TrigIsInOC illegal tn: ", tn);

      return 0;
    }

  /*
  int firstval = 0;
  int i;
  for (i = 1; i <= GetNOCPT(tn); i++)
    {
      if (GetOCPT(tn, i) == ocn) {firstval = 1;}
    }
  */

  int found = 0;

  int inc = 1;
  while (inc <= GetNOCPT(tn)) {inc *= 2;}
  inc /= 2;

  int start = inc;

  while (!found && inc > 0)
    {
      if (GetOCPT(tn,start) > ocn) {inc = inc/2; start -= inc;}
      else if (GetOCPT(tn,start) < ocn) {inc = inc/2; if (start+inc <= GetNOCPT(tn)) {start += inc;}}
      else {found = 1;}
    }

  return GetOCPT(tn, start) == ocn;
}

int STLGeometry :: GetChartNr(int i) const
{
  if (i > chartmark.Size())
    {
      PrintSysError("GetChartNr(", i, ") not possible!!!");
      i = 1;
    }
  return chartmark.Get(i);
}
/*
int STLGeometry :: GetMarker(int i) const
{
  return chartmark.Get(i);
}
*/
void STLGeometry :: SetMarker(int nr, int m)
{
  chartmark.Elem(nr) = m;
}
int STLGeometry :: GetNOCharts() const
{
  // [FS] .. edit
  return atlas.Size();
}
const STLChart& STLGeometry :: GetChart(int nr) const
{
  if (nr > atlas.Size())
    {
      PrintSysError("GetChart(", nr, ") not possible!!!");
      nr = 1;
    }
  return *(atlas.Get(nr));
}

int STLGeometry :: AtlasMade() const
{
  return chartmark.Size() != 0;
}


//return 1 if not exists
int AddIfNotExists(ARRAY<int>& list, int x)
{
  int i;
  for (i = 1; i <= list.Size(); i++)
    {
      if (list.Get(i) == x) {return 0;}
    }
  list.Append(x);
  return 1;
}

void STLGeometry :: GetInnerChartLimes(ARRAY<twoint>& limes, int chartnum)
{
  int j, k;

  int t, nt, np1, np2;
  STLTriangle tt;

  limes.SetSize(0);

  STLChart& chart = GetChart(chartnum);

  for (j = 1; j <= chart.GetNChartT(); j++)
    {
      t = chart.GetChartTrig(j);
      const STLTriangle& tt = GetTriangle(t);
      for (k = 1; k <= 3; k++)
	{
	  nt = NeighbourTrig(t,k);
	  if (GetChartNr(nt) != chartnum)
	    {
	      tt.GetNeighbourPoints(GetTriangle(nt),np1,np2);
	      if (!IsEdge(np1,np2))
		{
		  limes.Append(twoint(np1,np2));
		  /*
		  p3p1 = GetPoint(np1);
		  p3p2 = GetPoint(np2);
		  if (AddIfNotExists(limes,np1))
		    {
		      plimes1.Append(p3p1);
		      //plimes1trigs.Append(t);
		      //plimes1origin.Append(np1);
		    }
		  if (AddIfNotExists(limes1,np2))
		    {
		      plimes1.Append(p3p2);
		      //plimes1trigs.Append(t);
		      //plimes1origin.Append(np2);
		    }
		  //chart.AddILimit(twoint(np1,np2));

		  for (int di = 1; di <= divisions; di++)
		    {
		      double f1 = (double)di/(double)(divisions+1.);
		      double f2 = (divisions+1.-(double)di)/(double)(divisions+1.);

		      plimes1.Append(Point3d(p3p1.X()*f1+p3p2.X()*f2,
					     p3p1.Y()*f1+p3p2.Y()*f2,
					     p3p1.Z()*f1+p3p2.Z()*f2));
		      //plimes1trigs.Append(t);
		      //plimes1origin.Append(0);
		    }
		  */
		}
	    }
	}
    }
}



void STLGeometry :: GetDirtyChartTrigs(int chartnum, STLChart& chart,
				       const ARRAY<int>& outercharttrigs,
				       ARRAY<int>& chartpointchecked,
				       ARRAY<int>& dirtytrigs)
{
  std::cout << "[FS] .. in GetDirtyChartTrigs" << std::endl;

  dirtytrigs.SetSize(0);
  int j,k,n;

  int np1, np2, nt;
  int cnt = 0;

  std::cout << "..chart.GetNChartT: " << chart.GetNChartT() << std::endl;

  for (j = 1; j <= chart.GetNChartT(); j++)
    {
      int t = chart.GetChartTrig(j);
      const STLTriangle& tt = GetTriangle(t);

      for (k = 1; k <= 3; k++)
	{
	  nt = NeighbourTrig(t,k);
	  if (GetChartNr(nt) != chartnum && outercharttrigs.Get(nt) != chartnum)
	    {
	      tt.GetNeighbourPoints(GetTriangle(nt),np1,np2);

	      std::cout << "..checkpoint GDCT: A" << std::endl;

	      if (!IsEdge(np1,np2))
		{
		  dirtytrigs.Append(j); //local numbers!!!
		  cnt++;
		  break; //only once per trig!!!
		}
	    }
	}
    }
  cnt = 0;

  std::cout << "..checkpoint GDCT: B" << std::endl;

  int addedges = 0;
  int p1, p2, tn1, tn2, l, problem, pn;
  ARRAY<int> trigsaroundp;

  std::cout << ".. chart.GetNChartT: " << chart.GetNChartT() << std::endl;

  for (j = chart.GetNChartT(); j >= 1; j--)
    {
      int t = chart.GetChartTrig(j);
      const STLTriangle& tt = GetTriangle(t);

      std::cout << "..checkpoint GDCT: B1" << std::endl;

      for (k = 1; k <= 3; k++)
	{
	  pn = tt.PNum(k);
	  //if (chartpointchecked.Get(pn) == chartnum)
	  //{continue;}

	  std::cout << "..checkpoint GDCT: B2" << std::endl;

	  int checkpoint = 0;
	  for (n = 1; n <= trigsperpoint.EntrySize(pn); n++)
	    {
	      if (trigsperpoint.Get(pn,n) != t && //ueberfluessig???
		  GetChartNr(trigsperpoint.Get(pn,n)) != chartnum &&
		  outercharttrigs.Get(trigsperpoint.Get(pn,n)) != chartnum) {checkpoint = 1;};
	    }

	  std::cout << "..checkpoint GDCT: B3 .. checkpoint: " << checkpoint << std::endl;

	  if (checkpoint)
	    {
	      chartpointchecked.Elem(pn) = chartnum;

	      std::cout << "..checkpoint GDCT: B31" << std::endl;

	      int worked = 0;
	      GetSortedTrianglesAroundPoint(pn,t,trigsaroundp);
	      trigsaroundp.Append(t); //ring

	      std::cout << "..checkpoint GDCT: B4" << std::endl;

	      problem = 0;
	      //forward:
	      for (l = 2; l <= trigsaroundp.Size()-1; l++)
		{
		  tn1 = trigsaroundp.Get(l-1);
		  tn2 = trigsaroundp.Get(l);
		  const STLTriangle& t1 = GetTriangle(tn1);
		  const STLTriangle& t2 = GetTriangle(tn2);
		  t1.GetNeighbourPoints(t2, p1, p2);
		  if (IsEdge(p1,p2)) break;

		  if (GetChartNr(tn2) != chartnum && outercharttrigs.Get(tn2) != chartnum) {problem = 1;}
		}

	      std::cout << "..checkpoint GDCT: B5" << std::endl;

	      //backwards:
	      for (l = trigsaroundp.Size()-1; l >= 2; l--)
		{
		  tn1 = trigsaroundp.Get(l+1);
		  tn2 = trigsaroundp.Get(l);
		  const STLTriangle& t1 = GetTriangle(tn1);
		  const STLTriangle& t2 = GetTriangle(tn2);
		  t1.GetNeighbourPoints(t2, p1, p2);
		  if (IsEdge(p1,p2)) break;

		  if (GetChartNr(tn2) != chartnum && outercharttrigs.Get(tn2) != chartnum) {problem = 1;}
		}

	      std::cout << "..checkpoint GDCT: B6" << std::endl;

	      if (problem && !IsInArray(j,dirtytrigs))
		{
		  dirtytrigs.Append(j);
		  cnt++;
		  break; //only once per triangle
		}
	    }
	}
    }

  std::cout << "[FS] .. leaving GetDirtyChartTrigs" << std::endl;
}

}
