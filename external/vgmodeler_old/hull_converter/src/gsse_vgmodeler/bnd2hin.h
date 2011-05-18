#ifndef BND2HIN_H
#define BND2HIN_H


struct edge_c : std::pair<int,int>
{
	typedef std::pair<int,int> BASE;
	edge_c(int i1=0, int i2=0) : BASE(i1,i2) {}
	int operator[](int i) const
	{
		if (i==0)
			return first;
		if (i==1)
			return second;
		throw;
	}
};
inline std::ostream &operator<<(std::ostream &of, const edge_c &e) { of << "(" << e[0] << "," << e[1] << ")"; return of; }
class bnd_c
{
private:
    struct points_c
    {
	    std::vector<double> x,y,z;
    };

    typedef std::map<edge_c,int> edgemap_c;
    typedef std::vector<edge_c> edges_c;

    typedef std::vector<int> element_c;
    typedef std::vector<int> face_c;

    struct polyhedron_c
    {
    };

    typedef std::vector<face_c> faces_c;
    typedef std::vector<element_c> elements_c;
    struct region_c
    {
	std::string name,material;
	std::vector<int> elements;
    };
    typedef std::vector<region_c> regions_c;

    static const double fac2;

    points_c _points;
    edges_c _edges;
    faces_c _bndfaces;
    elements_c _elements;
    regions_c _regions;

    typedef std::map<int,int> polyhedronlist_c;
    polyhedronlist_c polyhedronlist;
    elements_c newelements;
    faces_c newfaces;

    std::vector<int> newr2r;
    std::vector<int> newp2p;
//    std::vector<int> newf2f;
    std::vector<int> newv2v;

    std::map<int,int> r2newr;
    std::map<int,int> p2newp;
//    std::map<int,int> f2newf;
    std::map<int,int> v2newv;

public:
    void doit(char *fnamein, char *fnameout, polyhedronlist_c *_polyhedronlist=NULL);

private:
    void writeit(char *fname);
    std::vector<std::string> partition(const std::string &line, const std::string &delim);
    std::vector<std::string> partition(const std::string &line, const char *delim);
    std::vector<std::string> split(const std::string &line);
    std::pair<int,int> check_needle(face_c &face, points_c &points);
    void remove_from_elements(elements_c &theelements,int f);
    void remove_needles(faces_c &faces, elements_c &elements, points_c &points);
    void consistency_check(faces_c &faces, elements_c &elements, regions_c &regions, polyhedronlist_c &polyhedronlist);
    void readit(const char *fname);
    void build_new_faces1(polyhedronlist_c *_polyhedronlist, std::vector<face_c> &_faces);
    void build_new_faces2(std::vector<face_c> &_faces);
public:
    int to_gsse(mesh_p_t mesh_p);
    int bnd2surface(const std::string& filename_in)
    {
  	bnd_c::polyhedronlist_c _polyhedronlist;
	std::vector<face_c> _tmpfaces;

  	//gsse_surface.readGeometry(filename_in);
  	readit(filename_in.c_str());
  	build_new_faces1(&_polyhedronlist,_tmpfaces);
  	std::cerr << "Faces: " << _tmpfaces.size() << std::endl;
  	//consistency_check();
  	remove_needles(_tmpfaces, _elements, _points);
  	//consistency_check();
  	std::cerr << "Faces: " << _tmpfaces.size() << "\n";
  	//
  	build_new_faces2(_tmpfaces);
  	//writeit(fnameout);
	//
  	return 0;
    }

};

#endif
