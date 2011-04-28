


#define myvector vector

#include <cmath>
#include <vector>
#include <map>
#include <fstream>

using namespace std;

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Constrained_triangulation_plus_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/intersections.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel   KK;
typedef KK::Point_3                                         Point3;
typedef KK::Vector_3                                        Vector3;
typedef KK::FT                                              Double3;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_2<K>                Vb;
typedef CGAL::Delaunay_mesh_face_base_2<K>                  Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb>        Tds;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds>  CDTone;
typedef CGAL::Constrained_triangulation_plus_2<CDTone>      CDT;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT>            Criteria;
typedef CGAL::Delaunay_mesher_2<CDT, Criteria>              Mesher;
typedef CDT::Point                                          Point;

struct point_c 
{
	double c[3];
	point_c()
	{}
	point_c(double x, double y, double z)
	{
		c[0]=x;
		c[1]=y;
		c[2]=z;
	}
	double &operator[](const int i) { return c[i]; }
	double operator[](const int i) const { return c[i]; }

	point_c operator-(const point_c &p) const
	{
		point_c q=*this;
		q[0]-=p[0];
		q[1]-=p[1];
		q[2]-=p[2];
		return q;
	}
	point_c operator+(const point_c &p) const
	{
		point_c q=*this;
		q[0]+=p[0];
		q[1]+=p[1];
		q[2]+=p[2];
		return q;
	}
	point_c operator*(const double d) const
	{
		point_c q;
		q[0]=((*this)[0]*d);
		q[1]=((*this)[1]*d);
		q[2]=((*this)[2]*d);
		return q;
	}
	point_c operator/(const double d) const
	{
		point_c q;
		q[0]=((*this)[0]/d);
		q[1]=((*this)[1]/d);
		q[2]=((*this)[2]/d);
		return q;
	}
	double operator*(const point_c &p) const
	{ return (*this)[0]* p[0]+ (*this)[1]* p[1]+ (*this)[2]* p[2]; }
	point_c ex(const point_c &p) const
	{ 
		point_c q;
		q[0]=((*this)[1]* p[2]-(*this)[2]* p[1]); 
		q[1]=((*this)[2]* p[0]-(*this)[0]* p[2]); 
		q[2]=((*this)[0]* p[1]-(*this)[1]* p[0]); 
		return q;
	}
	double fabs() const
	{ return sqrt((*this)[0]* (*this)[0]+ (*this)[1]* (*this)[1]+ (*this)[2]* (*this)[2]); }
	double fabs2() const
	{ return ((*this)[0]* (*this)[0]+ (*this)[1]* (*this)[1]+ (*this)[2]* (*this)[2]); }
	size_t size() { return 3; }
	void normalize()
	{
		const double len=this->fabs();
		(*this)[0]/=len;	
		(*this)[1]/=len;	
		(*this)[2]/=len;	
	}
};

double fabs(const point_c &p)
{
	return p.fabs();
}


struct poly_c : myvector<int>
{
	poly_c(const myvector<int> &m) : myvector<int>(m) {}
	struct const_circulator
	{
		const poly_c *father;
		myvector<int>::const_iterator I;
		const_circulator() : father(NULL) {}
		const_circulator(const poly_c *father_, myvector<int>::const_iterator I_) : father(father_), I(I_) {}
		const_circulator operator--()
		{
			if (this->I==father->begin())
				this->I=father->end();
			--this->I;
			return *this;
		}
		const_circulator operator++(int)
		{
			const_circulator D=*this;
			++this->I;
			if (this->I==father->end())
				this->I=father->begin();
			return D;
		}
		const_circulator operator++()
		{
			++this->I;
			if (this->I==father->end())
				this->I=father->begin();
			return *this;
		}
		int operator*() const { return *I; }
		bool operator!=(const const_circulator &c) const { return I!=c.I; }
	};
	const_circulator circulator_begin() const { return const_circulator(this,this->begin()); }
	poly_c(unsigned int i=0) : myvector<int>(i) {}
	void erase(const int i) { myvector<int>::erase(myvector<int>::begin()+i); }
};


typedef vector<point_c> points_c; 
typedef vector<poly_c > polys_c;


void solve(
		double &l, const point_c &a,
		double &m, const point_c &b,
		double &n, const point_c &c,
		const point_c &d)
{
	const double det=a*(b.ex(c));
	const double detl=d*(b.ex(c));
	const double detm=a*(d.ex(c));
	const double detn=a*(b.ex(d));
	l=detl/det;
	m=detm/det;
	n=detn/det;
}

void correct_polys(polys_c &polys, const myvector <int> &poly, const int iaa, const int iee)
{
	const unsigned int ia=(iaa+poly.size())%poly.size();
	const unsigned int ie=iee%poly.size();
	for (unsigned int i=0; i<polys.size(); i++)
	{
		for (unsigned int j=0; j<polys[i].size(); j++)
		{
			if (polys[i][j]==poly[ia])
				if (polys[i][(j+1)%polys[i].size()]==poly[ie])
				{

					for (int k=ia+1; k<iee; k++, j++)
						//polys[i][j+1]=poly[k];
						polys[i].insert(polys[i].begin()+j+1,poly[k]);
					continue;
				}
				else if (polys[i][(j-1+polys[i].size())%polys[i].size()]==poly[ie])
				{
					for (int k=ia+1; k<iee; k++)
						//polys[i][j]=poly[k];
						polys[i].insert(polys[i].begin()+j,poly[k]);
					continue;
				}
		}
	}
}



void insert_point_cgal(myvector<Point> &cgal_poly,map<Point,int> &find_cgal_poly, const int i, const Point &p)
{
	myvector<Point>::iterator V=cgal_poly.begin()+i;
	cgal_poly.insert(V,p);
	for (map<Point,int>::iterator I=find_cgal_poly.begin(); I!=find_cgal_poly.end(); I++)
	{
		if (I->second>=i)
			I->second++;
	}
	find_cgal_poly[p]=i;
}

int insert_point(points_c &points, poly_c &poly, const int iaa, const int iee, const int ii, const Point &pa, const Point &pe, const Point &pi)
{
	// l*(pe-pa) + m*(pe-pa)T = pi-pa
	//
	const int ia=(iaa+poly.size())%poly.size();
	const int ie=(iee)%poly.size();
	
	const point_c v1((pe.x()-pa.x()),(pe.y()-pa.y()),0);
	const point_c v2((pa.y()-pe.y()),(pe.x()-pa.x()),0);
	const point_c v3(0,0,1);
	const point_c v4((pi.x()-pa.x()),(pi.y()-pa.y()),0);
	double l,m,n;
	solve(l,v1,m,v2,n,v3,v4);

	point_c p=(points[poly[ie]]-points[poly[ia]])*l+points[poly[ia]];
	points.push_back(p);
	poly.insert(poly.begin()+ii,points.size()-1);
	return points.size()-1;
}

void insert_point(points_c &points, myvector<Point> &cgal_poly, map<Point,int> &find_cgal_poly, Point &p2d,
		point_c &v1, point_c &v1n, point_c &vn, point_c &p0, map<int,int> &addpoints)
{
	double l=(p2d.x())/fabs(v1),m=(p2d.y())/fabs(v1n);

	point_c p(v1*l + v1n*m + p0);

	find_cgal_poly[p2d]=cgal_poly.size();
	cgal_poly.push_back(p2d);

	points.push_back(p);
	assert(points[points.size()-1].size()==3);
	addpoints[cgal_poly.size()-1]=points.size()-1;
}



inline point_c project(const point_c &v1, const point_c &v1n, const point_c &vn, const point_c &p0, const point_c p)
{
	double l,m,n;
	solve(l,v1,m,v1n,n,vn,p-p0);
	point_c q(l*fabs(v1),m*fabs(v1n),n*fabs(vn));
	return q;
}

void project_poly(const points_c &points, const poly_c &poly, map<int, point_c> &flatpoly, point_c &v1, point_c &v1n, point_c &vn, point_c &p0)
{
    const int d0=max(int((poly.size()-3))/2,1);
    for (int d2=d0; d2>0; d2--)
    for (int d1=d0; d1>0; d1--)
    for (unsigned int i=0; i<poly.size(); i++)
    {
	p0=points[poly[i]];
	v1=points[poly[(i+d1)%poly.size()]]-points[poly[i]];
	point_c v2=points[poly[(i+d1+d2)%poly.size()]]-points[poly[i]];

	vn=v1.ex(v2);
	v1n=vn.ex(v1);
	if (vn.fabs()>v1.fabs()*v1.fabs()/100)
	{

		//cerr << "project: " << v1.fabs() << " " << v2.fabs() << " " << v1.ex(v2).fabs() << " " << v1.fabs()*v1.fabs()/100 << endl;
		goto found;
	}
	if (poly.size()==3)
		goto found;
    }
    cerr << "No good vector found¿n";
	throw;

    found:
	flatpoly.clear();
	for (unsigned int i=0; i<poly.size(); i++)
	{
		point_c q(project(v1,v1n,vn,p0,points[poly[i]]));
		flatpoly[i]=q;
	}
}




int triangulate_poly(points_c &points, polys_c &polys, poly_c &poly, myvector<myvector<int> > &tris, int criterion)
{
	int inserted=0;
	map<int,point_c> flatpoly;
	myvector<Point> cgal_poly;
	map<Point,int> find_cgal_poly;
	point_c v1,v1n,vnref,p0;
	project_poly(points,poly,flatpoly,v1,v1n,vnref,p0);

	CDT cdt;

	const int n=flatpoly.size();
    //try 
    {
	cgal_poly.clear();
	cgal_poly.resize(n);
	//std::cout << "Inserting a grid of constraints " << std::endl;

	for (int i=0; i<n; i++)
	{
		cgal_poly[i]=Point(flatpoly[i][0],flatpoly[i][1]);
		find_cgal_poly[cgal_poly[i]]=i;
	}

	for (int i=0; i<n; i++)
		cdt.insert_constraint(cgal_poly[i],cgal_poly[(i+1)%n]);

	//CGAL::make_conforming_Delaunay_2(cdt) ;
	//CGAL::refine_Delaunay_mesh_2(cdt);
	//
  	//std::cout << "The number of resulting points is  " << cdt.number_of_vertices() << " and was " << n << endl;
  	//std::cout << "The number of resulting triangles is  " << cdt.number_of_faces() << endl;

	Criteria crit(0,0);
	Mesher m(cdt,crit);

	m.init();
	if (criterion)
	{
		m.refine_mesh();
	}

	assert(cdt.is_valid());

	ofstream dump2("dump2");
	for (int i=0; i<cgal_poly.size()+1; i++)
	{
		//dump2 << cgal_poly[i%cgal_poly.size()].x() << " " <<  cgal_poly[i%cgal_poly.size()].y() << endl;
		dump2 << points[poly[i%cgal_poly.size()]][0] << " " <<  points[poly[i%cgal_poly.size()]][1] << endl;
	}
	dump2.close();
/*
  	int count = 0;
  	for (CDT::Finite_edges_iterator eit = cdt.finite_edges_begin();
       		eit != cdt.finite_edges_end();
       		++eit)
  	{
    		CDT::Edge &e=*eit;
    		cout << eit->second << " ";
    		CDT::Face &f=*eit->first;
    		cout << *f.vertex(f.ccw(eit->second)) << " ";
    		cout << *f.vertex(f.cw(eit->second)) << endl;
    		if (cdt.is_constrained(*eit)) ++count;
  	}
  	std::cout << "The number of resulting constrained edges is  ";
  	std::cout <<  count << std::endl;*/

  	//std::cout << "The number of resulting points is  " << cdt.number_of_vertices() << " and was " << n << endl;
  	//std::cout << "The number of resulting triangles is  " << cdt.number_of_faces() << endl;

	for (CDT::Constraint_iterator C=cdt.constraints_begin(); C!=cdt.constraints_end(); C++)
	{
		const Point &pa=C->first.first->point();
		const Point &pe=C->first.second->point();
		int i;
		int direction=0;
		for (i=0; i<cgal_poly.size(); i++)
		{
			if (pa==cgal_poly[i])
				break;
		}
		if (i==cgal_poly.size())
		{
			cerr << "Constraint start in Poly not found\n";
			throw;
		}
		if (cgal_poly[(i+1)%cgal_poly.size()]==pe)
			direction=1;
		else if (cgal_poly[(i+cgal_poly.size()-1)%cgal_poly.size()]==pe)
			direction=-1;
		else
		{
			cerr << "Constraint end in Poly not found\n";
			throw;
		}
		//cerr << "SUBCONSTRAINT: " << C->second->size() << endl;
		if (C->second->size()==2)
			continue;

		CDT::Vertices_in_constraint_iterator V=++C->second->begin();
		int j=i,k=i,pidx;
		if (direction==1)
		{ 
			j++; k++;
		}
		else
		{
			i--;
		}
		for (; V!=C->second->end(); V++)
		{
			if (*V==C->first.second)
				break;

			if (direction==1)
			{
				insert_point_cgal(cgal_poly,find_cgal_poly,k,(*V)->point());
				inserted=1;
				pidx=insert_point(points,poly,i,j,k,pa,pe,(*V)->point());
				j++; k++;
			}
			else
			{
				insert_point_cgal(cgal_poly,find_cgal_poly,k,(*V)->point());
				inserted=1;
				pidx=insert_point(points,poly,i,j,k,pe,pa,(*V)->point());
				j++;
			}
			cerr << "Boundary insert " << pidx << endl;
		}
		correct_polys(polys,poly,i,j);
		if (V==C->second->end())
		{
			cerr << "Endpoint of constraint not in poly\n";
			throw;
		}
	}

	if (criterion)
		return inserted;

	/*ofstream dump3("dump3");
	for (int i=0; i<cgal_poly.size()+1; i++)
	{
		//dump3 << cgal_poly[i%cgal_poly.size()].x() << " " <<  cgal_poly[i%cgal_poly.size()].y() << endl;
		dump3 << points[poly[i%poly.size()]][0] << " " <<  points[poly[i%poly.size()]][1] << endl;
	}
	dump3.close();*/




	//assert(cdt.number_of_vertices()==n);

	//ofstream dump("dump",ios_base::app);

	//for (int i=0; i<flatpoly.size(); i++)
	//{
	//	dump << flatpoly[i][0] << " " << flatpoly [i][1] << endl;
	//}
	//dump << flatpoly[0][0] << " " << flatpoly [0][1] << endl << endl;

	//points_c dum;
	int direction=0;
	int trict=0;
	map<int,int> addpoints;
  	for (CDT::Finite_faces_iterator fit = cdt.finite_faces_begin();
       		fit != cdt.finite_faces_end();
       		++fit)
  	{
		const CDT::Face &f=*fit;

		//cerr << f.is_in_domain();
		if (!f.is_in_domain())
			continue;

		tris.resize(trict+1);
		for (int i=0; i<3; i++)
		{
			const CDT::Vertex_handle ph=f.vertex(i);

			//if (find_cgal_poly.find(ph->point())==find_cgal_poly.end())
			//{
			//	insert_cgal_point(points,cgal_poly,find_cgal_poly,ph->point());
			//}

			if (find_cgal_poly.find(ph->point())==find_cgal_poly.end())
			{
				insert_point(points,cgal_poly,find_cgal_poly,ph->point(),v1,v1n,vnref,p0,addpoints);
				cerr << "Inside insert " << find_cgal_poly[ph->point()] << endl;
			}
			const int idx=find_cgal_poly[ph->point()];

			//dum.push_back(point_c());
			//dum[dum.size()-1].resize(3);
			//dum[dum.size()-1][0]=ph->point()[0];
			//dum[dum.size()-1][1]=ph->point()[1];

			// for checking, first insert only the index in poly
			//tris[trict].push_back(poly[idx]);
			tris[trict].push_back(idx);

			/*
			if (tris[trict][i]<poly.size())
				cerr << "idx: " << idx << " pidx: " << poly[tris[trict][tris[trict].size()-1]] << " psize: " << points.size() << endl;
			else
				cerr << "idx: " << idx << " pidx: " << addpoints[tris[trict][tris[trict].size()-1]] << " psize: " << points.size() << endl;
			*/
		}

		for (int i=0; i<3; i++)
		{
			if (tris[trict][i]>=poly.size() || tris[trict][(i+1)%3]>=poly.size())
				continue;
			if ((tris[trict][i]+1)%poly.size()==tris[trict][(i+1)%3])
			{
				if (!direction)
					direction=1;
				if (direction==-1)
					direction=-2;
			}
			if (tris[trict][i]==(tris[trict][(i+1)%3]+1)%poly.size())
			{
				if (!direction)
					direction=-1;
				if (direction==1)
					direction=2;
			}
		}
		trict++;
	}
	if (direction==2 || direction==-2 || direction==0)
	{
		cerr << "Direction changes " << direction << "\n";
		throw;
	}

	//ofstream dump("dump");
	myvector<int> h;
	for (int idx=0; idx<tris.size(); idx++)
	{
		h=tris[idx];
		for (int i=0; i<3; i++)
		{
			if (direction==1)
				if (h[i]<poly.size())
					tris[idx][i]=poly[h[i]];
				else
					tris[idx][i]=addpoints[h[i]];
			else
				if (h[2-i]<poly.size())
					tris[idx][i]=poly[h[2-i]];
				else
					tris[idx][i]=addpoints[h[2-i]];
		}
		/*dump << points[tris[idx][0]][0] << " "
		     << points[tris[idx][0]][1] << " "
		     << points[tris[idx][0]][2] << endl;
		dump << points[tris[idx][1]][0] << " "
		     << points[tris[idx][1]][1] << " "
		     << points[tris[idx][1]][2] << endl;
		dump << points[tris[idx][2]][0] << " "
		     << points[tris[idx][2]][1] << " "
		     << points[tris[idx][2]][2] << endl;
		dump << points[tris[idx][0]][0] << " "
		     << points[tris[idx][0]][1] << " "
		     << points[tris[idx][0]][2] << endl;
		dump << endl;*/
		
/*		for (int i=0; i<3; i++)
		{
			for (double j=0; j<=1.001; j+=0.1)
			{
				point_c p1=dum[idx*3+(i+1)%3]*j+dum[idx*3+i]*(1-j);
				point_c p2=dum[idx*3+(i+2)%3]*j+dum[idx*3+i]*(1-j);
				dump << p1[0] << " " << p1[1] << endl;
				dump << p2[0] << " " << p2[1] << endl << endl;
			}
		}*/
		
		if (idx==0)
		{
			point_c v1=points[tris[idx][1]]-points[tris[idx][0]];
			point_c v2=points[tris[idx][2]]-points[tris[idx][0]];
			point_c vn=v1.ex(v2);
			//cerr << "Normaldir: " << vnref*vn << endl;
		}
	}

	//dump.close();
	//system("gnuplot");
    }

    return inserted;
}


void triangulate_polys(points_c &points, polys_c &polys, map<int,myvector<myvector<int> > > &tris)
{
	map<int,myvector<int> > innerpoints;
	tris.clear();
	int inserted=1,run=0;
	while (inserted)
	{
		inserted=0;
		for (unsigned int i=0; i<polys.size(); i++)
			if (polys[i].size()!=3)
			{
				inserted+=triangulate_poly(points, polys, polys[i], tris[i], 0);

			}
		run++;
	}
}


