#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cstdlib>
using namespace std;

#include "gsse_meshing_includes.hpp"
#include "bnd2hin.h"

#define foreach(var, container) \
  for(typeof((container).begin()) var = (container).begin(); \
      var != (container).end(); \
      ++var)
#define findin(var,container) \
  (container.find(var)!=container.end())

vector<string> bnd_c::partition(const string &line, const string &delim)
{
	string tmp=line;
	vector<string> feld;
	int idx;
	idx=tmp.find_first_of(delim);
	if (idx==0)
	{
		idx=tmp.find_first_not_of(delim);
		tmp=tmp.substr(idx);
	}
	while (1)
	{
		idx=tmp.find_first_of(delim);
		feld.push_back(tmp.substr(0,idx));
		if (idx==tmp.npos)
			break;
		tmp=tmp.substr(idx);
		idx=tmp.find_first_not_of(delim);
		if (idx==tmp.npos)
			break;
		tmp=tmp.substr(idx);
	}
	return feld;
}
vector<string> bnd_c::partition(const string &line, const char *delim)
{
	string d=delim;
	return partition(line,d);
}
vector<string> bnd_c::split(const string &line)
{
	return partition(line," ");
}

pair<int,int> bnd_c::check_needle(face_c &face, points_c &points)
{
	double xmin=1e20;
	double ymin=1e20;
	double zmin=1e20;
	double xmax=-1e20;
	double ymax=-1e20;
	double zmax=-1e20;
	for (int i=0; i<face.size(); i++)
	{
		int ii=face[i];

		if (points.x[ii]<xmin)
			xmin=points.x[ii];
		if (points.y[ii]<ymin)
			ymin=points.y[ii];
		if (points.z[ii]<zmin)
			zmin=points.z[ii];
		if (points.x[ii]>xmax)
			xmax=points.x[ii];
		if (points.y[ii]>ymax)
			ymax=points.y[ii];
		if (points.z[ii]>zmax)
			zmax=points.z[ii];
	}

	double d2=((xmax-xmin)*(xmax-xmin)+(ymax-ymin)*(ymax-ymin)+(zmax-zmin)*(zmax-zmin));
	//print "d2 ",d2

	for (int i=0; i<face.size(); i++)
	{
		int ii=face[i];
		int jj=face[(i+1)%face.size()];

		double d3=( (points.x[ii]-points.x[jj])*(points.x[ii]-points.x[jj])+ (points.y[ii]-points.y[jj])*(points.y[ii]-points.y[jj])+ (points.z[ii]-points.z[jj])*(points.z[ii]-points.z[jj]) );
		//print "d3 ",d3

		if (d3*fac2<d2)
		{
			//print x[ii],y[ii],z[ii]
			//print x[jj],y[jj],z[jj]
			//print x[ii]-x[jj],y[ii]-y[jj],z[ii]-z[jj]
			//print ii,jj
			return pair<int,int>(ii,jj);
		}
	}
	return pair<int,int>(-1,-1);
}

void bnd_c::remove_from_elements(elements_c &theelements,int f)
{
	for (int i=0; i<theelements.size(); i++)
	{
		int removed=1;
		while (removed)
		{
		    removed=0;
		    for (int j=0; j<theelements[i].size(); j++)
		    {
			int k=theelements[i][j];
			if (k<0)
				k=-k-1;
			if (k==f or k==-f-1)
			{
				theelements[i].erase(theelements[i].begin()+j);
				removed=1;
				break;
			}
		    }
		    if (theelements[i].size()==0)
		    {
			    cerr << "Error: Element got empty\n";
			    exit(-1);
		    }

		//for j in range(0,len(theelements[i])):
		//	k=theelements[i][j]
		//	if (k<0 and k<-f-1):
		//		theelements[i][j]=k+1
		//	elif (k>f):
		//		theelements[i][j]=k-1
		}
	}
}	


void bnd_c::remove_needles(faces_c &tmpfaces, elements_c &elements, points_c &points)
{
	int changed=1;
	faces_c thetmpfaces=tmpfaces;
	elements_c theelements=elements;
	pair<int,int> s;
	cerr << "Faces1: " << thetmpfaces.size() << "\n";
	cerr << "Faces1: " << tmpfaces.size() << "\n";
	while (changed)
	{
		changed=0;
		for (int i=0; i<thetmpfaces.size(); i++)
		{
			//print i
			s=check_needle(thetmpfaces[i],points);

			if (s.first!=-1)
			{
				changed=1;
				break;
			}
		}

		if (s.first==-1)
			break;

		cerr << "Remove needle\n";
		//print s[0],s[1]

		int removedface=1;
		while (removedface)
		{
		    for (int i=0; i<thetmpfaces.size(); i++)
		    {
			removedface=0;
			int j1=-1;
			int j2=-1;
			for (int j=0; j<thetmpfaces[i].size(); j++)
			{
				if (thetmpfaces[i][j]==s.first)
					j1=j;
				if (thetmpfaces[i][j]==s.second)
					j2=j;
			}

			int mod=thetmpfaces[i].size();

			if (j1==-1 and j2==-1) // not found
				continue;
			if (j2==-1)		// second point not found, nothing
				continue;
			if (j1==-1)		// only second point found, move it to first
			{
				thetmpfaces[i][j2]=s.first;
				continue;
			}
			if ((j1-j2)*(j1-j2)!=1 and (j1-j2)*(j1-j2)!=(mod-1)*(mod-1))
			{
				cerr << j1 << " " << j2 << endl;
				cerr << "The needle cannot be removed, because the indices don't lie next in polygon\n";
				exit(-1);
			}

			//print thetmpfaces[i]
			thetmpfaces[i].erase(thetmpfaces[i].begin()+j2);
			//print thetmpfaces[i]
			cerr << "Remove point\n";

			if (thetmpfaces[i].size()>=3)
				continue;

			//del thetmpfaces[i]
			removedface=1;
			cerr << "Remove face\n";
			remove_from_elements(theelements,i);
			break;
		    }
		}
	}
		   
	elements=theelements;
	tmpfaces=thetmpfaces;
	cerr << "Faces2: " << thetmpfaces.size() << "\n";
	cerr << "Faces2: " << tmpfaces.size() << "\n";
}

void bnd_c::consistency_check(faces_c &tmpfaces, elements_c &elements, regions_c &regions, polyhedronlist_c &polyhedronlist)
{
	edgemap_c edgemap;
	int i;
	edge_c edge1,edge2;
  	for (int j=0; j<regions.size(); j++)
	{
		if (!findin(j,polyhedronlist))
			continue;
		for (int k=0; k<1; k++)
		{
		  foreach(l,elements[regions[j].elements[k]])
		  {
		    if (*l>=0)
			i=*l;
		    else
			i=-*l-1;
		    for (int j=0; j<tmpfaces[i].size(); j++)
		    {
			k=(j+1)%tmpfaces[i].size();

			if (*l>=0)
			{
				edge1=edge_c(tmpfaces[i][j],tmpfaces[i][k]);
				edge2=edge_c(tmpfaces[i][k],tmpfaces[i][j]);
			}
			else
			{
				edge2=edge_c(tmpfaces[i][j],tmpfaces[i][k]);
				edge1=edge_c(tmpfaces[i][k],tmpfaces[i][j]);
			}

			if (findin(edge1,edgemap))
			{
				cerr << "Edge " << edge1[0] << "," << edge1[1] << " already exists\n";
				exit(-1);
			}

			if (findin(edge2,edgemap))
			{
				if (edgemap[edge2]!=1)
				{
					cerr << "Edge " << edge1[0] << "," << edge1[1] << " already exists " << edgemap[edge2] << " times\n";
					exit(-1);
				}

				edgemap[edge2]=edgemap[edge2]+1;
			}
			else
				edgemap[edge1]=1;
		     }
		  }
		}
	}

	foreach(i,edgemap)
	{
		if (i->second!=2)
			cerr << i->first << " " << i->second << endl;
	}
}

//readit(int argc, char *argv[])
void bnd_c::readit(const char *fname)
{
ifstream fin(fname);
string line;
vector<string> l,p,q;
while (1)
{
	getline(fin,line);
	l=partition(line," ");
	if (l[0]=="Vertices")
		break;
}

int npts=atoi(partition(partition(line,"(")[1],")")[0].c_str());

int i=0;
while (1)
{
	getline(fin,line);
	p=split(line);
	_points.x.push_back(atof(p[0].c_str()));
	_points.y.push_back(atof(p[1].c_str()));
	_points.z.push_back(atof(p[2].c_str()));
	i=i+1;
	if (i==npts)
		break;
}

while (1)
{
	getline(fin,line);
	l=partition(line," ");
	if (l[0]=="Edges")
		break;
}

int nedges=atoi(partition(partition(line,"(")[1],")")[0].c_str());

i=0;
while (1)
{
	getline(fin,line);
	p=split(line);
	_edges.push_back(edge_c(atoi(p[0].c_str()),atoi(p[1].c_str())));
	i=i+1;
	if (i==nedges)
		break;
}

while (1)
{
	getline(fin,line);
	l=partition(line," ");
	if (l[0]=="Faces")
		break;
}

int nfaces=atoi(partition(partition(line,"(")[1],")")[0].c_str());

i=0;
while (1)
{
	getline(fin,line);
	p=split(line);

	int n=atoi(p[0].c_str());
	_bndfaces.push_back(face_c());
	for (int j=0; j<n; j++)
		_bndfaces[i].push_back(atoi(p[j+1].c_str()));
	i=i+1;
	if (i==nfaces)
		break;
}

while (1)
{
	getline(fin,line);
	l=partition(line," ");
	if (l[0]=="Elements")
		break;
}

int nelements=atoi(partition(partition(line,"(")[1],")")[0].c_str());

i=0;
while (1)
{
	getline(fin,line);
	p=split(line);

	if (p[0]=="10")
	{
		int n=atoi(p[1].c_str());
		while (n>p.size())
		{
			getline(fin,line);
			q=split(line);
			p.insert( p.end(), q.begin(), q.end());
		}
		_elements.push_back(element_c());
		for (int j=0; j<n; j++)
			_elements[i].push_back(atoi(p[j+2].c_str()));
		i=i+1;
		if (i==nelements)
			break;
	}
	else
	{
		cerr << "Elementtype not known\n";
		exit(-1);
	}
}

i=0;
while (1)
{
	while (1)
	{
		getline(fin,line);
		if (line.length()==0)
			break;
		l=partition(line," ");
		if (l[0]=="Region")
			break;
	}

	if (line.length()==0)
		break;
	string name=partition(partition(line,"\"")[1],"\"")[0];
	getline(fin,line);
	string material=partition(line,"=")[1];
	getline(fin,line);
	int nel=atoi(partition(partition(line,"(")[1],")")[0].c_str());

	_regions.push_back(region_c());
	_regions.back().name=name;
	_regions.back().material=material;
	
	int j=0;
	while (1)
	{
		getline(fin,line);
		p=split(line);
		foreach(e,p)
		{
			_regions.back().elements.push_back(atoi(partition(*e,"}")[0].c_str()));
			j=j+1;
			if (j==nel)
				break;
		}
		if (j==nel)
			break;
	}

	//for l in regionelements[i]:
	//	print elements[l]
	i=i+1;
}
}

void bnd_c::build_new_faces1(polyhedronlist_c *_polyhedronlist, vector<face_c> &_tmpfaces)
{
int i=0;
foreach (j,_bndfaces)
{
	_tmpfaces.push_back(face_c());
	foreach(k,*j)
	{
		if (*k>=0)
			_tmpfaces[i].push_back(_edges[*k][0]);
		else
			_tmpfaces[i].push_back(_edges[-*k-1][1]);
	}
	i=i+1;
}

if (_polyhedronlist->size())
	polyhedronlist=*_polyhedronlist;
else
	for (i=0; i<_regions.size(); i++)
		polyhedronlist[i]=1;
cerr << "polyhedronlist: " << polyhedronlist.size() << endl;
}

void bnd_c::build_new_faces2(vector<face_c> &_tmpfaces)
{
newelements=_elements;
if (1)
{
  int npct=0;
  for (int i=0; i<_regions.size(); i++)
  {
	if (findin(i,polyhedronlist))
	{
		r2newr[i]=newr2r.size();
		newr2r.push_back(i);
	}
	for (int k=0; k<1; k++)
	{
		int ll;
		if (!findin(i,polyhedronlist))
		{
			newelements[_regions[i].elements[k]].clear();
			continue;
		}
		newelements[_regions[i].elements[k]].clear();
		foreach(l,_elements[_regions[i].elements[k]])
		{
			if (*l>=0)
				ll=*l;
			else
				ll=-*l-1;

			if (!findin(ll,p2newp))
			{
				newp2p.push_back(ll);
				p2newp[ll]=npct;
				npct=npct+1;
			}
			if (*l>=0)
				newelements[_regions[i].elements[k]].push_back(p2newp[ll]);
			else
				newelements[_regions[i].elements[k]].push_back(-p2newp[ll]-1);
		}
	}
  }

  int j=0;
  for (int i=0; i<newp2p.size(); i++)
  {
	int ii=newp2p[i];

	newfaces.push_back(face_c());
	foreach(jj,_tmpfaces[ii])
	{
		if (!findin(*jj,v2newv))
		{
			newv2v.push_back(*jj);
			v2newv[*jj]=j;
			j=j+1;
		}
		newfaces[i].push_back(v2newv[*jj]);
	}
  }
}
}

void bnd_c::writeit(char *fname)
{
int j;
if (1)
{
  ofstream fout(fname);
  int npts=newv2v.size();
  fout << "Vertices[" << npts << "]{\n";
  for (int i=0; i<npts; i++)
  {
	//print x[newv2v[i]]*1000,y[newv2v[i]]*1000,z[newv2v[i]]*1000;
	fout << _points.x[newv2v[i]] << " " << _points.y[newv2v[i]] << " " << _points.z[newv2v[i]] << endl;
  }
  fout << "}\n";

  int nfaces=newp2p.size();
  fout << "Polygons[" << nfaces << "]{\n";
  for (j=0; j<nfaces; j++)
  {
	fout << "Polygon[" << newfaces[j].size() << "]{\n";
	int num=newfaces[j].size();
	if (0)
	{
		fout << newfaces[j][0];
		for(int k=1; k<newfaces[j].size(); k++)
			fout << " " << newfaces[j][k];
	}
	else
	{
		fout << newfaces[j][num-1];
		for (int k=1; k<newfaces[j].size(); k++)
			fout << " " << newfaces[j][num-1-k];
	}
	fout << "\n}\n";
  }
  fout << "}\n";

  fout << "Polyhedra[" << newr2r.size() << "]{\n";
  for (j=0; j<_regions.size(); j++)
  {
	if (!findin(j,polyhedronlist))
		continue;
	fout << "Polyhedron[" << newelements[_regions[j].elements[0]].size() << "]{\n";
	for (int k=0; k<1; k++)
	{
		//print regionelements[j][k],
		foreach(l,newelements[_regions[j].elements[k]])
		{
			if (*l>=0)
				fout << *l << " ";
			else
				fout << *l << " ";
		}
		fout << "}\n";
	}
  }
  fout << "}\n";

  fout << "Material[" << newr2r.size() << "]{\n";
  for (j=0; j<_regions.size(); j++)
  {
	if (!findin(j,polyhedronlist))
		continue;
	fout << j << " ";
  }
fout << "}\n";
}
}

//void bnd_c::doit(char *fnamein, char *fnameout, polyhedronlist_c *_polyhedronlist=NULL)
void bnd_c::doit(char *fnamein, char *fnameout, polyhedronlist_c *_polyhedronlist)
{
	readit(fnamein);
	vector<face_c> _tmpfaces;
	build_new_faces1(_polyhedronlist,_tmpfaces);
	cerr << "Faces: " << _tmpfaces.size() << endl;
	//consistency_check();
	remove_needles(_tmpfaces, _elements, _points);
	//consistency_check();
	cerr << "Faces: " << _tmpfaces.size() << "\n";
	//
	build_new_faces2(_tmpfaces);
	writeit(fnameout);
}

const double bnd_c::fac2=1000000;

int bnd_c::to_gsse(mesh_p_t mesh_p) 
{
   int i, j, k, m, h;
   int flipflag;
   int number;
   int elcount;
   int ctag;
   int stflag;
   int index;
   double minx;
   double maxx;
   double miny;
   double maxy;
   double minz;
   double maxz;
   double epsx;
   double epsy;
   double epsz;
   ppt dpp;
   ppt dppout;
   tri_pt starttri_p;
   tri_pt begintri_p;
   tri_pt tri_p2;

   struct arraydef vertex_list;
   int polygon_count;
   struct polygondef* polygon_p;
   int polyhedra_count;
   struct segmentdef* segment_p;
   int z;
   tri_pt tri_p;
   struct pointtype dpmin;
   struct pointtype dpmax;
   struct ls_clientdef ls_client;
   int nfms;
   int fm;
   int ret;

   array_init(&ls_client.ls_list, 20, sizeof(ppt)); 


   number = newv2v.size();

   array_init(&vertex_list, number, sizeof(ppt));
   vertex_list.n = number;
   msg_info1("## Reading %i vertices...\n", vertex_list.n); 
  
   for(i=0 ; i<vertex_list.n ; i++) 
   {
      create_point(dpp);
      ((ppt*)vertex_list.vp)[i] = dpp;

      dpp->x=_points.x[newv2v[i]];
      dpp->y=_points.y[newv2v[i]];
      dpp->z=_points.z[newv2v[i]];

      if(i==0) 
      {
         minx = dpp->x; 
         maxx = dpp->x;
         miny = dpp->y; 
         maxy = dpp->y;
         minz = dpp->z; 
         maxz = dpp->z;

         continue;
      }

      if(dpp->x > maxx) maxx = dpp->x;
      else if(dpp->x < minx) minx = dpp->x;

      if(dpp->y > maxy) maxy = dpp->y;
      else if(dpp->y < miny) miny = dpp->y;

      if(dpp->z > maxz) maxz = dpp->z;
      else if(dpp->z < minz) minz = dpp->z;
   }

   epsx = (minx==maxx)? (BBZEROBUF) : ((maxx - minx) * BBCUSHIONPERCENT);
   epsy = (miny==maxy)? (BBZEROBUF) : ((maxy - miny) * BBCUSHIONPERCENT);
   epsz = (minz==maxz)? (BBZEROBUF) : ((maxz - minz) * BBCUSHIONPERCENT);
   maxx += epsx;
   minx -= epsx;
   maxy += epsy;
   miny -= epsy;
   maxz += epsz;
   minz -= epsz;

   // octree 
   //
   if(mesh_p->global_octree.octree_counter>0) 
   {
      if(minx < mesh_p->global_octree.bbminimum.x || 
	 maxx > mesh_p->global_octree.bbmaximum.x ||
	 miny < mesh_p->global_octree.bbminimum.y || 
	 maxy > mesh_p->global_octree.bbmaximum.y ||
	 minz < mesh_p->global_octree.bbminimum.z || 
	 maxz > mesh_p->global_octree.bbmaximum.z  ) 
      {
         msg_error("+ERROR+: Cannot load data exceeding current "); 
         msg_error("octree bounding box\n");
      
         for(i=0 ; i<vertex_list.n ; i++) 
         {
            erase_point(((ppt*)vertex_list.vp)[i]);
         }
         free(vertex_list.vp);
         free(ls_client.ls_list.vp);
         return 0;
      }
   } else 
   { 
      mesh_p->global_octree.bbminimum.x = minx;
      mesh_p->global_octree.bbmaximum.x = maxx;
      mesh_p->global_octree.bbminimum.y = miny;
      mesh_p->global_octree.bbmaximum.y = maxy;
      mesh_p->global_octree.bbminimum.z = minz;
      mesh_p->global_octree.bbmaximum.z = maxz;

      octree_init(&mesh_p->global_octree);
   }

/*   do 
   {
      if(fscanf(fp, "%s", charin)==EOF) 
      {
         msg_dinfo("No Parameter section found in file\n");
         break;
      }
   } while( strncmp(charin, START_PARAMETERS, NUM_STARTPARS) != 0 );

   if(strncmp(charin, START_PARAMETERS, NUM_STARTPARS) == 0) 
   {
      if(strcmp(charin, START_PARAMETERS)==0) int ret = fscanf(fp, "%s", charin);
      msg_info("Reading the Parameter section \n");
      printf("Reading the Parameter section \n");

      param_init(mesh_p, fp);
   }

   rewind(fp);
*/
   for(i=0 ; i<vertex_list.n ; i++) 
   {
      dppout = oct_insert_point(((ppt*)vertex_list.vp)[i], &mesh_p->global_octree);
      if(dppout!=null) 
      {
         msg_dinfo("Point not inserted into octree, ");
         msg_dinfo("because same or similar point was found. \n");
         erase_point(((ppt*)vertex_list.vp)[i]);

         ((ppt*)vertex_list.vp)[i] = dppout;    //using the found octree point
      } 
   }

   // read in polygons
   //
   polygon_count = newp2p.size();
   msg_info1("## Reading %i polygons...\n", polygon_count); 

   polygon_p = (struct polygondef*) my_malloc(polygon_count * sizeof(struct polygondef));   // allocate memory for polygon array

   for(i=0 ; i<polygon_count ; i++) 
   {
      k = newfaces[i].size();
    
      array_init(&(polygon_p[i].vts), k, sizeof(ppt));
      polygon_p[i].vts.n = k;

      for(j=0 ; j<k ; j++) 
      {
	 h=newfaces[i][k-1-j];  // Reverse orientation
         ((ppt*)(polygon_p[i].vts.vp))[j] = ((ppt*)vertex_list.vp)[h];
      }

      k--;
      if(((ppt*)(polygon_p[i].vts.vp))[k] != ((ppt*)(polygon_p[i].vts.vp))[0]) 
      {
         k++;
      }

      polygon_p[i].vts.n = k;
      polygon_p[i].ctag = CTG_BLOCK;
      polygon_p[i].first_tri_p = null;
      polygon_p[i].facemark = init_facemark;
      polygon_p[i].number_of_indices = 0;  

      if(k<3) 
      { 
         msg_info1("%ith polygon has less than two points !\n", i);
         for(j=0 ; j<(i+1) ; j++) 
         {
            free(polygon_p[j].vts.vp);
         }
         free(polygon_p);
         free(vertex_list.vp);
         free(ls_client.ls_list.vp);
         return 0;
      }
   }
  
   // segment list
   //
   
      polyhedra_count = newr2r.size();
         msg_info1("## There are %i segments.\n", polyhedra_count); 

   segment_p = (struct segmentdef*) my_malloc(polyhedra_count * sizeof(struct segmentdef));

  
   // [MOD] .. additional material information
   //
   for(int ll=0 ; ll<_regions.size() ; ll++) 
   {
	int l=r2newr[ll];
	if (!findin(ll,polyhedronlist))
		continue;
         // number of polygons
         //
         number = newelements[_regions[ll].elements[0]].size();
         msg_info2("Segment %i contains %i polygons\n", l, number); 

      segment_p[l].n = number;
      segment_p[l].pgs_p = (int*) my_malloc(number * sizeof(int));

      // loop over all polygons of the l-th segment
      //
      for(m=0 ; m<number ; m++) 
      { 
            i = newelements[_regions[ll].elements[0]][m];

         segment_p[l].pgs_p[m] = i;
         if(i < 0) 
            i = ~i;

         if (polygon_p[i].number_of_indices == 0)    // additional material information
         {
            polygon_p[i].number_of_indices=1;
            polygon_p[i].material[0]=l;
            polygon_p[i].material[1]=-2;   // material is going to be incremented.. 
         }
         else if (polygon_p[i].number_of_indices == 1)
         {
            polygon_p[i].number_of_indices=2;
            polygon_p[i].material[1]=polygon_p[i].material[0];
            polygon_p[i].material[0]=l;

            if ( polygon_p[i].material[1] == polygon_p[i].material[0])
            {
               polygon_p[i].number_of_indices=1;
               polygon_p[i].material[1]=-2;
            }
         }
         else if (polygon_p[i].number_of_indices == 2)
         {
            polygon_p[i].number_of_indices=3;
            polygon_p[i].material[1]=-1;
            polygon_p[i].material[0]=-1;
         }


         if(polygon_p[i].ctag == CTG_BLOCK) 
         {
            polygon_p[i].ctag = CTG_BFACE + CTG_SEGBFACE;
         }
         else if(polygon_p[i].ctag == (CTG_BFACE + CTG_SEGBFACE)) 
         {
            polygon_p[i].ctag = CTG_IFACE + CTG_SEGIFACE;
         }
         else if( polygon_p[i].ctag == (CTG_IFACE + CTG_SEGIFACE) ) 
         {
            msg_warn("+WARNING+: Polygon is referenced by more than ");
            msg_warn("two segments\n");
         }
      }
   }    

   // all segments are read in
   //
   // now process all segments
   //
   elcount = 0;
   begintri_p = mesh_p->lasttri_p; 

   for(int l=0 ; l<polyhedra_count ; l++) 
   {
      stflag = 0;
      if(mesh_p->FLIP_GNORMALS) 
         flipflag = 1;
      else 
         flipflag = 0;


      number = segment_p[l].n;
      for(m=0 ; m<number ; m++) 
      { 
         i = segment_p[l].pgs_p[m];
      
         if(i < 0) i = ~i;
         k = polygon_p[i].vts.n;
         ctag = polygon_p[i].ctag;
      
         // [MOD] .. a polygon is going to be blocked here..
         //
         if(ctag & CTG_BLOCK) 
         {
            continue; 
         }
      
         if( (flipflag==1 && segment_p[l].pgs_p[m]>=0) || 
             (flipflag==0 && segment_p[l].pgs_p[m]<0) ) 
         {
            // switch the orientation of ith polygon (mth in lth segment)     
            //
            for(j=0 ; j<(k/2.0) ; j++) 
            {
               dpp = ((ppt*)(polygon_p[i].vts.vp))[j];

               ((ppt*)(polygon_p[i].vts.vp))[j] = 
                  ((ppt*)(polygon_p[i].vts.vp))[k-j-1];

               ((ppt*)(polygon_p[i].vts.vp))[k-j-1] = dpp;

            }
            if(polygon_p[i].facemark!=init_facemark && 
               polygon_p[i].facemark!=~init_facemark) 
            {
               polygon_p[i].facemark = ~polygon_p[i].facemark;
            }
         }
         starttri_p = mesh_p->lasttri_p; 
         polygon_p[i].first_tri_p = starttri_p;


         // NEW nonconvex_triangulate with material information
         //
         elcount += nonconvex_triangulate2(&(polygon_p[i].vts),  ctag,  polygon_p[i].facemark, &mesh_p->lasttri_p,
                                           polygon_p[i].material[0]+1, polygon_p[i].material[1]+1);  
         polygon_p[i].ctag = CTG_BLOCK; 

         polygon_p[i].last_tri_p = mesh_p->lasttri_p;
         if(stflag!=2) 
         {
            if(ctag==(CTG_BFACE + CTG_SEGBFACE)) 
            {
               stflag = 2; 
               mesh_p->lasttri_p->ctag |= CTG_SEED;
               if(mesh_p->seed_list.n == mesh_p->seed_list.max) 
               {
                  expand_ar(&mesh_p->seed_list, D_AR+polyhedra_count);
               }
               ((struct seed_def*) (mesh_p->seed_list.vp))[mesh_p->seed_list.n].seedtri_p      =   mesh_p->lasttri_p; 
               ((struct seed_def*) (mesh_p->seed_list.vp))[mesh_p->seed_list.n].material_index = l;
               mesh_p->seed_list.n++;

               polygon_p[i].ctag |= CTG_SEED; 

            } else 
            { 
               // interface / segment interface
               //
               stflag = 1; 
               index = i; 
            }
         } 
      } 

      if(stflag!=2) 
      { 
         if(stflag==1) 
         {
            msg_dinfo1("Segment %i contains no boundary faces \n", l);
            mesh_p->lasttri_p->ctag |= CTG_SEED;
            if(mesh_p->seed_list.n == mesh_p->seed_list.max) 
            {
               expand_ar(&mesh_p->seed_list, D_AR+polyhedra_count);
            }
            ((struct seed_def*)
             (mesh_p->seed_list.vp))[mesh_p->seed_list.n].seedtri_p = 
               mesh_p->lasttri_p; 
            ((struct seed_def*)
             (mesh_p->seed_list.vp))[mesh_p->seed_list.n].material_index = l;
            mesh_p->seed_list.n++;
	
            polygon_p[index].ctag |= CTG_SEED; 

         } else 
         { 
            msg_dinfo1("Segment %i contains no new triangles \n", l);
            for(m=0 ; m<number ; m++) 
            {
               i = segment_p[l].pgs_p[m];
               if(i < 0) i = ~i;
               if(! (polygon_p[i].ctag&CTG_SEED) ) 
               { 
                  /* avoid other starttris                                         */
                  polygon_p[i].last_tri_p->ctag |= CTG_SEED;
                  /* Manage seed_list and set material index                       */
                  if(mesh_p->seed_list.n == mesh_p->seed_list.max) 
                  {
                     expand_ar(&mesh_p->seed_list, D_AR+polyhedra_count);
                  }
                  ((struct seed_def*)
                   (mesh_p->seed_list.vp))[mesh_p->seed_list.n].seedtri_p = 
                     polygon_p[i].last_tri_p; 
                  ((struct seed_def*)
                   (mesh_p->seed_list.vp))[mesh_p->seed_list.n].material_index = l;
                  mesh_p->seed_list.n++;
                  polygon_p[i].ctag |= CTG_SEED;

                  /* if(flipflag==0) { FLIP in ANYCASE ASSUMING correct interface  */

                  /* flip all triangles of polygon containing starttri           */
                  /* when not interactive default for flipflag==0 should be      */
                  /* to *flip* the interface tri which already exists.           */
                  starttri_p = polygon_p[i].first_tri_p; 
                  do 
                  { 
                     /* there must be at least one triangle in a polygon          */
                     starttri_p = starttri_p->ntri_p;
                     dppout = starttri_p->dppp[0];
                     starttri_p->dppp[0] = starttri_p->dppp[1];
                     starttri_p->dppp[1] = dppout;
                     /* update facemark                                           */
                     if(starttri_p->facemark!=init_facemark &&
                        starttri_p->facemark!=~init_facemark) 
                     {
                        starttri_p->facemark = ~starttri_p->facemark;
                     }
                  } while(starttri_p!=polygon_p[i].last_tri_p);
                  /* only one starttri, no more polygons                           */
                  break;
               }
               /* next polygon                                                    */
            }
            /* check if a seed was found among all the polygons from this segment*/
            if(m==number) 
            {
               msg_warn1("+WARNING:+ Cannot create a seed for segment %i !\n", l);
               msg_warn("           This segment is bounded exclusively by other seeds!\n");
            }
         }
      }

      /* NEXT SEGMENT                                                          */
   } 


   /** PROCESS EXTRA MATERIAL INFORMATION *************************************/

   if(1) 
   {
      number = _regions.size();
    
      if(number!=polyhedra_count) 
      {
         msg_info("Not reading the material info (different number) \n");
      }
      else 
      {
         msg_info("Reading the material info ... \n");
         for(int l=0 ; l<polyhedra_count ; l++) 
         {
            j = newr2r[l];
            if(j>=0) 
            {
               ((struct seed_def*)(mesh_p->seed_list.vp))[l].material_index = j;
            }
            else 
            {
               ((struct seed_def*)(mesh_p->seed_list.vp))[l].material_index = j;

               dppout = 
                  ((struct seed_def*)(mesh_p->seed_list.vp))[l].seedtri_p->dppp[0];
               ((struct seed_def*)(mesh_p->seed_list.vp))[l].seedtri_p->dppp[0] = 
                  ((struct seed_def*)(mesh_p->seed_list.vp))[l].seedtri_p->dppp[1];
               ((struct seed_def*)(mesh_p->seed_list.vp))[l].seedtri_p->dppp[1] = 
                  dppout;

               /* update facemark                                                 */
               if(((struct seed_def*)(mesh_p->seed_list.vp))[l].seedtri_p->facemark != init_facemark &&
                  ((struct seed_def*)(mesh_p->seed_list.vp))[l].seedtri_p->facemark != ~init_facemark) 
               {
                  ((struct seed_def*)(mesh_p->seed_list.vp))[l].seedtri_p->facemark=
                     ~((struct seed_def*)(mesh_p->seed_list.vp))[l].seedtri_p->facemark;
               }

            }
         }
      }
   }

   // freeing allocated memory
   for(int l=0 ; l<polyhedra_count ; l++) 
   {
      free(segment_p[l].pgs_p);
   }
   free(segment_p);
   for(j=0 ; j<polygon_count ; j++) 
   {
      free(polygon_p[j].vts.vp);
   }
   free(polygon_p);
   free(vertex_list.vp);
   free(ls_client.ls_list.vp);

  
   /** COMPLETE DATASTRUCTURE FROM BEGINTRI_P ON ******************************/
   j = 0;
   m = 0; /* flip toggle                                                      */
   tri_p = begintri_p->ntri_p; 
   while(tri_p!=null) 
   {
      //    printf(" .. hierwrap.c:: complete data structure \n");
      /* just to make sure check if it exists already!                         */
      /* If yes, then could change ctag or do flipping for the findmaterials   */
      /* functionality!                                                        */
      if(exist_tri(tri_p->dppp[0], 
		   tri_p->dppp[1], 
		   tri_p->dppp[2],
		   &tri_p2) != 0) 
      {

         /* count double existing tris                                          */
         j++;
      
         /* CASE both seeds                                                     */
         if( (tri_p2->ctag&CTG_SEED) && (tri_p->ctag&CTG_SEED) ) 
         {
            /* both triangles were referenced by segment info and both are seeds */
            /* erase the uninserted (tri_p) also from the seedlist               */
            for(i=0 ; i < mesh_p->seed_list.n ; i++) 
            {
               if( ((struct seed_def*)(mesh_p->seed_list.vp))[i].seedtri_p==tri_p) 
               {
                  break;
               }
            }
            /* must be found !! (i<seed_list.n)                                  */
            mesh_p->seed_list.n--;
            ((struct seed_def*)(mesh_p->seed_list.vp))[i].seedtri_p = 
               ((struct seed_def*)(mesh_p->seed_list.vp))[mesh_p->seed_list.n].seedtri_p;
            ((struct seed_def*)(mesh_p->seed_list.vp))[i].material_index = 
               ((struct seed_def*)(mesh_p->seed_list.vp))[mesh_p->seed_list.n].material_index;
            msg_warn("+WARNING+: Erasing a seed triangle due to triangles which exist twice! \n");
	
            /* could have been BFACE and needs to be changed to IFACE            */
            /* could cause troubles when polys are not just triangles..          */
            /* or for flag checks when cocircular points..                       */
            /* or when just simply wrong that double tris.. and not interface    */
            tri_p2->ctag = CTG_SEED + CTG_IFACE + CTG_SEGIFACE;

            /* do not need to remove tri_p because is not yet inserted           */
            tri_p2 = tri_p; /* the one to be erased                              */
            tri_p = tri_p->ntri_p; /* can be null                                */
            erase_triangle(tri_p2, mesh_p->lasttri_p);
            elcount--;

            /* continue with next tri (tri_p)                                    */
            continue;
         }

         /* CASE one of them is a seed                                          */
         if(tri_p2->ctag&CTG_SEED) 
         {
            /* the inserted tri is a seed                                        */
	
            /* could have been BFACE and needs to be changed to IFACE            */
            /* could cause troubles when polys are not just triangles..          */
            /* or for flag checks when cocircular points..                       */
            /* or when just simply wrong that double tris.. and not interface    */
            tri_p2->ctag = CTG_SEED + CTG_IFACE + CTG_SEGIFACE;

            /* do not need to remove tri_p because is not yet inserted           */
            tri_p2 = tri_p; /* the one to be erased                              */
            tri_p = tri_p->ntri_p; /* can be null                                */
            erase_triangle(tri_p2, mesh_p->lasttri_p);
            elcount--;

            /* continue with next tri (tri_p)                                    */
            continue;
         }
         if(tri_p->ctag&CTG_SEED) 
         {
            /* the uninserted tri is a seed                                      */
	
            /* could have been BFACE and needs to be changed to IFACE            */
            /* could cause troubles when polys are not just triangles..          */
            /* or for flag checks when cocircular points..                       */
            /* or when just simply wrong that double tris.. and not interface    */
            tri_p->ctag = CTG_SEED + CTG_IFACE + CTG_SEGIFACE;

            /* need to remove tri_p2 because was inserted                        */
            sm_remove_triangle(tri_p2);
            erase_triangle(tri_p2, mesh_p->lasttri_p);
            elcount--;

            /* insert the seed                                                   */
            insert_triangle(tri_p);
            tri_p = tri_p->ntri_p; 

            /* continue with next tri (tri_p)                                    */
            continue;
         }

         /* CASE none of them is a seed and the inserted can always win         */
         /* alternately flip the inserted to make sure findmaterials works      */
         /* (necessary for segments which are surrounded by interfaces)         */

         /* could have been BFACE and needs to be changed to IFACE              */
         /* could cause troubles when polys are not just triangles..            */
         /* or for flag checks when cocircular points..                         */
         /* or when just simply wrong that double tris.. and not interface      */
         tri_p2->ctag = CTG_IFACE + CTG_SEGIFACE;

         /* alternate flip                                                      */
         if(m) 
         {
            dpp = tri_p2->dppp[0];
            tri_p2->dppp[0] = tri_p2->dppp[1];
            tri_p2->dppp[1] = dpp;
            /* update facemark                                                   */
            if(tri_p2->facemark!=init_facemark && 
               tri_p2->facemark!=~init_facemark) 
            {
               tri_p2->facemark = ~tri_p2->facemark;
            }
         }
         m = (m+1)%2;

         /* do not need to remove tri_p because is not yet inserted             */
         tri_p2 = tri_p; /* the one to be erased                                */
         tri_p = tri_p->ntri_p; /* can be null                                  */
         erase_triangle(tri_p2, mesh_p->lasttri_p);
         elcount--;

         /* continue with next tri (tri_p)                                      */
         continue;

      }
      /* unique triangle                                                       */
      insert_triangle(tri_p);
      tri_p = tri_p->ntri_p;
   }

   mesh_p->gltr += elcount;
  
   if(j) 
   {
      msg_info1("%i triangles have been erased (multiple existence) \n", j);
   }
   msg_info1("%i triangles have been inserted \n", elcount);
   msg_info1("%i triangles\n", mesh_p->gltr);
   msg_info1("%i points in global octree \n", 
             mesh_p->global_octree.octree_counter);
  
   return elcount;
}
