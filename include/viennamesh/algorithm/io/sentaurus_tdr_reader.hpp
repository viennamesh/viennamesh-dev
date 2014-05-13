#ifndef VIENNAMESH_ALGORITHM_IO_SENTAURUS_TDR_READER_HPP
#define VIENNAMESH_ALGORITHM_IO_SENTAURUS_TDR_READER_HPP

#include <string>
#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <typeinfo>
#include <cstdlib>

using std::string;

#include <iostream>
#include "H5Cpp.h"

#ifndef H5_NO_NAMESPACE
using namespace H5;
#endif
#define FALSE   0

#define mythrow(a) { std::cerr << a << std::endl; throw; }
#define mythrow1(a) { std::cerr << a << std::endl; throw; }


#include "viennagrid/mesh/element_creation.hpp"



namespace viennamesh
{

  // Operator function
  herr_t get_all_groups(hid_t loc_id, const char *name, void *opdata);
  herr_t file_info(hid_t loc_id, const char *name, void *opdata);


  typedef struct {
      int index;               // index of the current object
  } iter_info;

  typedef std::set<hid_t> child_t;
  struct node_t
  {
    string type;
    child_t child;
  };

  typedef std::map<hid_t,node_t> tree_t;
  extern tree_t tree;
  extern hid_t parent;


  void iterate_attrs(H5Object &element,int depth=0,string type="");

  void iterate_elements(CommonFG &element,int depth=0,string type="");

  void check_class(H5T_class_t t);

  int read_int(const H5Object &g, const string name);
  double read_double(const H5Object &g, const string name);
  //string read_string(const Group &g, const string name)
  string read_string(const H5Object &g, const string name);

  struct dataset_t
  {
    string name, quantity, unit;
    int nvalues;
    double conversion_factor;
    std::vector<double> values;
  };

  struct region_t
  {
    int regnr;
    string name,material;
    int nelements,npointidx;
    std::vector<std::vector<int> > elements;
    std::map<string,dataset_t> dataset;
  };

  struct attributeinfo_c
  {
    const dataset_t *firstattribute;
    std::map<int,double> values;
    attributeinfo_c() : firstattribute(NULL) {}
  };

  extern int globalattribute;
  extern std::map<string,attributeinfo_c> globalattributes;
  extern std::set<string> removeattribute;
  extern std::set<string> removesegment;
  extern double cutsegmentbelow;
  extern double cutsegmentabove;
  extern string addsegment;
  extern string addmaterial;


  struct tdr_geometry
  {
    int dim,nvertices,nregions,ndatasets;
    std::vector<double> vertex;
    std::map<string,region_t> region;
    double trans_matrix[9],trans_move[3];

    void read_transformation(const Group &trans)
    {
      const DataSet &A=trans.openDataSet("A");
      const DataSet &b=trans.openDataSet("b");
      A.read( trans_matrix, PredType::NATIVE_DOUBLE);
      b.read( trans_move, PredType::NATIVE_DOUBLE);
    }

    typedef struct coord2_t {
            double x[3];
        } coord2_t;


    void read_vertex(const DataSet &vert)
    {
      const DataSpace &dataspace = vert.getSpace();
      int rank = dataspace.getSimpleExtentNdims();
      hsize_t dims[10];
      int ndims = dataspace.getSimpleExtentDims( dims, NULL);
     std::cerr << "dataspace: " << rank << " " << ndims << " " << dims[0] << "," << dims[1] << std::endl;
     std::cerr << "nvertices: " << nvertices << std::endl;
      if (nvertices!=dims[0])
        mythrow("nvertices not equal vertices.dim");

      CompType mtype2( sizeof(coord2_t) );
      const H5std_string MEMBER1( "x" );
      const H5std_string MEMBER2( "y" );
      const H5std_string MEMBER3( "z" );
            mtype2.insertMember( "x", HOFFSET(coord2_t, x[0]), PredType::NATIVE_DOUBLE);
      if (dim>1)
      mtype2.insertMember( "y", HOFFSET(coord2_t, x[1]), PredType::NATIVE_DOUBLE);
      if (dim>2)
      mtype2.insertMember( "z", HOFFSET(coord2_t, x[2]), PredType::NATIVE_DOUBLE);
            /*
            * Read two fields x and y from s2 dataset. Fields in the file
            * are found by their names "x_name" and "y_name".
            */
            coord2_t s2[dims[0]];
            vert.read( s2, mtype2 );

      for (int i=0; i<dims[0]; i++)
      {
        vertex.push_back(s2[i].x[0]*10000.);
        if (dim>1)
        vertex.push_back(s2[i].x[1]*10000.);
        if (dim>2)
        vertex.push_back(s2[i].x[2]*10000.);
      }
    }

    void read_elements(region_t &region, const DataSet &elem)
    {
     std::cerr << "nelements: " << region.nelements << std::endl;

      const DataSpace &dataspace = elem.getSpace();
      int rank = dataspace.getSimpleExtentNdims();
      hsize_t dims[10];
      int ndims = dataspace.getSimpleExtentDims( dims, NULL);

      //cerr << "dataspace: " << rank << " " << ndims << " " << dims[0] << "," << dims[1] <<std::endl;
      if (rank!=1)
        mythrow("rank of elements in region " << region.name << " is not one");
      if (ndims!=1)
        mythrow("ndims of elements in region " << region.name << " is not one");

      int el[dims[0]];
      elem.read( el, PredType::NATIVE_INT);

      //for (int i=0; i<dims[0]; i++)
      // std::cerr << tmp_elements[i] << " ";
      //cerr <<std::endl;

      int elct=0,eldim;
      while (elct<dims[0])
      {
        region.elements.push_back(std::vector<int>());
        switch (el[elct++])
        {
          case 1: for (int i=0; i<2; i++)
              region.elements.back().push_back(el[elct++]);
            break;
          case 2: for (int i=0; i<3; i++)
              region.elements.back().push_back(el[elct++]);
            break;
          case 5: for (int i=0; i<4; i++)
              region.elements.back().push_back(el[elct++]);
            break;
          default:mythrow("Element type " << el[elct-1] << " in region " << region.name << " not known");
        }
      }

    }

    void read_region(const int regnr, const Group &reg)
    {
      string name0=read_string(reg,"name");
      string name;
      int i;
      while ((i=name0.find_first_of("_."))!=name0.npos)
      {
        name=name+name0.substr(0,i);
        name0=name0.substr(i+1);
      }
      name=name+name0;
      string material;
      const int typ=read_int(reg,"type");
      switch (typ)
      {
        case 0: // we have a nD region
          material=read_string(reg,"material");
          break;
        case 1: // we have a nD-1 region (contact... drain, source, ...)
          material="Contact";
          // attribute "part 0", no idea for what
          break;
        case 2: material="Interface"+read_int(reg,"bulk 0")+read_int(reg,"bulk 1");
          break;
        //default : mythrow(__LINE__ << ": Unknown type " << typ << " in region " << read_string(reg,"name"));
        default :std::cerr << __LINE__ << ": Unknown type " << typ << " in region " << read_string(reg,"name") << std::endl;
         std::cerr << "I do not throw, I only skip\n";
          return;
      }
     std::cerr << "typ: " << typ << std::endl;
      const int nparts=read_int(reg,"number of parts");
      if (nparts!=1)
      {
        //mythrow("Number of parts not one");
       std::cerr << "Number of parts not one\n";
       std::cerr << "I only skip this\n";
      }

      region[name].regnr=regnr;
      region[name].name=name;
      region[name].material=material;
      const int n=reg.getNumObjs();
      for (int i=0; i<n; i++)
      {
        if (reg.getObjnameByIdx(i)=="elements_0")
        {
          const DataSet &ds=reg.openDataSet("elements_0");
          region[name].nelements=read_int(ds,"number of elements");
          read_elements(region[name],ds);
          return;
        }
        if (reg.getObjnameByIdx(i)=="part_0")
        {
          const Group &part=reg.openGroup("part_0");
          region[name].nelements=read_int(part,"number of elements");
          read_elements(region[name],part.openDataSet("elements"));
          return;
        }
      }
      mythrow("No elements found on region " << name);
    }

    region_t &find_region(int regnr)
    {
      for (std::map<string,region_t>::iterator R=region.begin(); R!=region.end(); R++)
        if (R->second.regnr==regnr)
          return R->second;
      mythrow("Region " << regnr << " not found");
    }

    void read_values(dataset_t &dataset,const DataSet &values)
    {
      const DataSpace &dataspace = values.getSpace();
      int rank = dataspace.getSimpleExtentNdims();
      hsize_t dims[10];
      int ndims = dataspace.getSimpleExtentDims( dims, NULL);
     std::cerr << "dataspace: " << rank << " " << ndims << " " << dims[0] << "," << dims[1] <<std::endl;
     std::cerr << "nvalues: " << dataset.nvalues << std::endl;
      if (dataset.nvalues!=dims[0] || ndims!=1)
        mythrow("Dataset " << dataset.name << " should have " << dataset.nvalues << " values, but has " << dims[0] << " with dimension " << ndims);

      double v[dims[0]];
      values.read( v, PredType::NATIVE_DOUBLE);
      dataset.values.insert(dataset.values.end(),&v[0],&v[dims[0]]);
    }
    void read_dataset(const Group &dataset)
    {
            string name = read_string(dataset,"name");
      if (name.find("Stress")!=name.npos)
        return;

            string quantity = read_string(dataset,"quantity");
            int regnr = read_int(dataset,"region");
            int nvalues=read_int(dataset,"number of values");
            double conversion_factor = read_double(dataset,"conversion factor");
            if (read_int(dataset,"location type")!=0)
      {
        //mythrow("Dataset " << name << " location type not 0");
       std::cerr << "Dataset " << name << " location type not 0\n";
       std::cerr << "I only skip this dataset\n";
        return;
      }
            if (read_int(dataset,"structure type") != 0)
        mythrow("Dataset " << name << " structure type not 0");
            if (read_int(dataset,"value type") != 2)
              mythrow("Dataset " << name << " value type not 2");

     std::cerr << "Dataset: " << name << " " << nvalues <<std::endl;

      // In this dataset we have a group
      // tag_group_0???
      // units: take only the unit name
      // Dataset values: the actual values

      string unit;
      int n=dataset.getNumObjs(),i;
      for (i=0; i<n; i++)
      {
       std::cerr << dataset.getObjnameByIdx(i) <<std::endl;
        if (dataset.getObjnameByIdx(i)=="unit")
        {
          const Group &u=dataset.openGroup("unit");
          unit=read_string(u,"name");
          break;
        }
      }
      if (i==n)
      {
        unit=read_string(dataset,"unit:name");
      }

      region_t &region=find_region(regnr);
      region.dataset[name].name=name;
      region.dataset[name].quantity=quantity;
      region.dataset[name].nvalues=nvalues;
      region.dataset[name].conversion_factor=conversion_factor;
      region.dataset[name].unit=unit;

      read_values(region.dataset[name],dataset.openDataSet("values"));
    }

    void read_attribs0(const Group &state)
    {
      int i;
      ndatasets=read_int(state,"number of datasets");
      std::cout << "ndatasets: " << ndatasets <<std::endl;

      i=read_int(state,"number of plots");
      if (i!=0)
        mythrow("Numberofplots not equal 0");
      i=read_int(state,"number of string streams");
      if (i!=0)
        mythrow("Numberofstringstreams not equal 0");
      i=read_int(state,"number of xy plots");
      if (i!=0)
        mythrow("Numberofxyplots not equal 0");

      for (i=0; i<ndatasets; i++)
      {
        char a[100];
        sprintf(a,"dataset_%d",i);
       std::cerr << "Attribute: " << a <<std::endl;
        read_dataset(state.openGroup(a));
      }
    }

    void read_geometry(const Group &geometry)
    {
      int typ,dum;
      char name[100];
      typ=read_int(geometry,"type");
      switch (typ)
      {
        case 1: break;
        default : mythrow(__LINE__ << ": Unknown type " << typ << " in geometry");
      }
     std::cerr << "Typ: " << dum <<std::endl;

      dim=read_int(geometry,"dimension");
      std::cout << "Dimension: " << dim <<std::endl;

      nvertices=read_int(geometry,"number of vertices");
     std::cerr << "nvertices: " << nvertices <<std::endl;

      nregions=read_int(geometry,"number of regions");
     std::cerr << "nregions: " << nregions <<std::endl;

      dum=read_int(geometry,"number of states");
      if (dum!=1)
        mythrow("Number of states not one");
     std::cerr << "States: " << dum <<std::endl;

      for (int i=0; i<nregions; i++)
      {
        sprintf(name,"region_%d",i);
        const Group &reg=geometry.openGroup(name);
        read_region(i,reg);
      }

      const Group &trans=geometry.openGroup("transformation");
      read_transformation(trans);
      const DataSet &vert=geometry.openDataSet("vertex");
      read_vertex(vert);
      // What do the Units???

      read_attribs0(geometry.openGroup("state_0"));
    }

    void read_collection(const Group &collection)
    {
      int i;

      i=read_int(collection,"number of geometries");
      if (i!=1)
        mythrow("Not only one geometry");
      i=read_int(collection,"number of plots");
      if (i!=0)
        fprintf(stderr,"We have plots, skip them\n");

      read_geometry(collection.openGroup("geometry_0"));
    }

    std::map<std::vector<int>,int> edgeref,triangleref,tetref,elementref;
    std::vector<std::vector<int> > edges,triangles,tets,elements;

    int insert_edge(const int i1, const int i2,int check=0)
    {
      std::vector<int> e,e2;
      e.push_back(i1);
      e.push_back(i2);
      e2.push_back(i2);
      e2.push_back(i1);
      if (edgeref.find(e)!=edgeref.end())
      {
        if (check)
          mythrow("Edge should not be, dim=" << dim);
        return edgeref[e];
      }
      if (edgeref.find(e2)!=edgeref.end())
      {
        return -edgeref[e2]-1;
      }
      edgeref[e]=edges.size();
      edges.push_back(e);
      return edgeref[e];
    }
    int insert_tri(const int i1, const int i2, const int i3,int check=0)
    {
      int h;
      std::vector<int> idx(3),idxsort(3);
      std::vector<int> e(2);

      idx[0]=i1;
      idx[1]=i2;
      idx[2]=i3;

      int n=0;
      for (int i=1; i<3; i++)
        if (idx[i]<idx[n])
          n=i;

      for (int i=0; i<3; i++)
        idxsort[i]=idx[(n+i)%3];

      if (triangleref.find(idxsort)!=triangleref.end())
      {
        if (check)
          mythrow("Triangle should not be, dim=" << dim);
        return triangleref[idxsort];
      }

      idx[0]=idxsort[0];
      idx[1]=idxsort[2];
      idx[2]=idxsort[1];

      if (triangleref.find(idx)!=triangleref.end())
      {
        return -triangleref[idx]-1;
      }
      triangleref[idxsort]=triangles.size();
      triangles.push_back(idxsort);
      return triangleref[idxsort];
    }
    int insert_tet(const int i1, const int i2, const int i3, const int i4, int check=0)
    {
      int h;
      std::vector<int> idx(4),idxsort(4);
      std::vector<int> tri(3);

      idx[0]=i1;
      idx[1]=i2;
      idx[2]=i3;
      idx[3]=i4;
      int n=0;
      for (int i=1; i<4; i++)
        if (idx[i]<idx[n])
          n=i;

      for (int i=0; i<4; i++)
        idxsort[i]=idx[(n+i)%4];

      if (tetref.find(idxsort)!=tetref.end())
      {
        if (check)
          mythrow("Tet should not be, dim=" << dim);
        return tetref[idxsort];
      }

  /*    idx[0]=idxsort[0];
      idx[1]=idxsort[2];
      idx[2]=idxsort[1];
      idx[3]=idxsort[3];

      if (tetref.find(idx)!=tetref.end())
      {
        return -tetref[idx]-1;
      }*/
      tetref[idxsort]=tets.size();
      tets.push_back(idxsort);
      return tetref[idxsort];
    }

    void write_grd_header(std::ofstream &file)
    {
                  file << "DF-ISE text" <<std::endl;
                  file << "" <<std::endl;
                  file << "Info {" <<std::endl;
                  file << "  version = 1.0" <<std::endl;
                  file << "  type    = grid" <<std::endl;
                  file << "  dimension   = " << dim <<std::endl;
                  file << "  nb_vertices = " << nvertices <<std::endl;
                  file << "  nb_edges    = " << edges.size() <<std::endl;
      if (dim==3)
      {
        file << "  nb_faces    = " << triangles.size() <<std::endl;
      }
      else if (dim==2)
      {
        file << "  nb_faces    = " << 0 <<std::endl;
      }
      file << "  nb_elements = " << elements.size() <<std::endl;
                  file << "  nb_regions  = " << nregions <<std::endl;
                  file << "  regions     = [ ";
      for (std::map<string,region_t>::iterator S=region.begin(); S!=region.end(); S++)
        file << "\"" << S->second.name << "\" ";
      file << "]" <<std::endl;
                  file << "  materials   = [ ";
      for (std::map<string,region_t>::iterator S=region.begin(); S!=region.end(); S++)
        file << "" << S->second.material << " ";
      file << "]" <<std::endl;
                  file << "}" <<std::endl;
                  file << "" <<std::endl;

                  file << "Data {" <<std::endl;
                  file << "" <<std::endl;
                  file << "  CoordSystem {" <<std::endl;
                  file << "    translate = [ ";
      file << trans_move[0] << " ";
      file << trans_move[1] << " ";
      file << trans_move[2] << " ";
      file << "]" <<std::endl;

                  file << "    transform = [ ";
      file << trans_matrix[0] << " ";
      file << trans_matrix[1] << " ";
      file << trans_matrix[2] << " ";
      file << trans_matrix[3] << " ";
      file << trans_matrix[4] << " ";
      file << trans_matrix[5] << " ";
      file << trans_matrix[6] << " ";
      file << trans_matrix[7] << " ";
      file << trans_matrix[8] << " ";
      file << "]" <<std::endl;

                  file << "  }" <<std::endl;
    }

    void write_dat_header(std::ofstream &file)
    {
      file << "DF-ISE text" <<std::endl;
      file << "" <<std::endl;
      file << "Info {" <<std::endl;
      file << "  version = 1.0" <<std::endl;
      file << "  type    = dataset" <<std::endl;
      file << "  dimension   = " << dim <<std::endl;
      file << "  nb_vertices = " << nvertices <<std::endl;
      file << "  nb_edges    = " << edges.size() <<std::endl;
      if (dim==3)
      {
        file << "  nb_faces    = " << triangles.size() <<std::endl;
      }
      else if (dim==2)
      {
        file << "  nb_faces    = " << 0 <<std::endl;
      }
      file << "  nb_elements = " << elements.size() <<std::endl;
      file << "  nb_regions  = " << nregions <<std::endl;
      file << "  datasets    = [";
      if (!globalattribute)
      {
          for (std::map<string,region_t>::const_iterator R=region.begin(); R!=region.end(); R++)
        for (std::map<string,dataset_t>::const_iterator D=R->second.dataset.begin(); D!=R->second.dataset.end(); D++)
          if (removeattribute.find(D->second.name)==removeattribute.end())
            file << " \"" << D->second.name << "\"";
      }
      else
      {
          for (std::map<string,attributeinfo_c>::iterator A=globalattributes.begin(); A!=globalattributes.end(); A++)
          if (removeattribute.find(A->first)==removeattribute.end())
            file << " \"" << A->first << "\"";
      }
      file << " ]" <<std::endl;
      file << "  functions   = [";
      if (!globalattribute)
      {
          for (std::map<string,region_t>::const_iterator R=region.begin(); R!=region.end(); R++)
        for (std::map<string,dataset_t>::const_iterator D=R->second.dataset.begin(); D!=R->second.dataset.end(); D++)
          if (removeattribute.find(D->second.name)==removeattribute.end())
            file << " \"" << D->second.quantity << "\"";
      }
      else
      {
          for (std::map<string,attributeinfo_c>::iterator A=globalattributes.begin(); A!=globalattributes.end(); A++)
          if (removeattribute.find(A->first)==removeattribute.end())
            file << " \"" << A->second.firstattribute->quantity << "\"";
      }
      file << " ]" <<std::endl;
      file << "}" <<std::endl;
    }

    void remove_segments()
    {
      for (std::map<string,region_t>::iterator S=region.begin(); S!=region.end(); )
      {
        if (removesegment.find(S->first)==removesegment.end())
        { S++; continue; }

       std::cerr << "Remove Segment " << S->first <<std::endl;
        std::map<string,region_t>::iterator S2=S++;
        region.erase(S2);
      }
    }


    template<typename MeshT, typename SegmentationT>
    bool to_viennagrid(MeshT & mesh, SegmentationT & segmentation)
    {
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;
      typedef typename viennagrid::result_of::cell_tag<MeshT>::type CellTag;

      const int mesh_vertices_per_element = viennagrid::boundary_elements<CellTag, viennagrid::vertex_tag>::num;
      const int mesh_dimension = viennagrid::result_of::geometric_dimension<MeshT>::value;

      if (mesh_dimension != dim)
        return false;

      std::vector<VertexHandleType> vertex_handles( nvertices );

      for (int i=0; i<nvertices; i++)
      {
        PointType point;
        for (int j = 0; j < dim; ++j)
          point[j] = vertex[dim*i+j];

        vertex_handles[i] = viennagrid::make_vertex(mesh, point);
      }

      typedef typename viennagrid::result_of::segment_handle<SegmentationT>::type SegmentHandleType;

      for (std::map<string,region_t>::iterator S=region.begin(); S!=region.end(); S++)
      {
        string segment_name = S->second.name;
        SegmentHandleType & segment = segmentation(segment_name);

        std::vector<std::vector<int> > &elements=S->second.elements;
        for (std::vector<std::vector<int> >::iterator E=elements.begin(); E!=elements.end(); E++)
        {
          const std::vector<int> &e=*E;

          if (e.size() != mesh_vertices_per_element)
            continue;

          std::vector<VertexHandleType> handles(e.size());

          for (std::size_t i = 0; i < e.size(); ++i)
            handles[i] = vertex_handles[ e[i] ];

          viennagrid::make_cell( segment, handles.begin(), handles.end() );
        }
      }

      return true;
    }


    template<typename MeshT, typename SegmentationT, typename SegmentedMeshQuantitiesType>
    void to_mesh_quantities( MeshT const & mesh, SegmentationT const & segmentation,
                             SegmentedMeshQuantitiesType & quantites )
    {
      typedef typename viennagrid::result_of::vertex<MeshT>::type VertexType;
      typedef typename viennagrid::result_of::cell<MeshT>::type CellType;
      typedef typename viennagrid::result_of::segment_handle<SegmentationT>::type SegmentHandleType;


      for (std::map<string,region_t>::const_iterator R=region.begin(); R!=region.end(); R++)
      {
        std::set<int> pointmarkers;
        const std::vector<std::vector<int> > &elements=R->second.elements;
        for (int i=0; i<elements.size(); i++)
          for (int j=0; j<elements[i].size(); j++)
            pointmarkers.insert(elements[i][j]);

        for (std::map<string,dataset_t>::const_iterator D=R->second.dataset.begin(); D!=R->second.dataset.end(); D++)
        {
          if (globalattributes[D->first].firstattribute==NULL)
            globalattributes[D->first].firstattribute=&D->second;
          std::map<int,double> &attr=globalattributes[D->first].values;
          int validx=0;
          for (std::set<int>::const_iterator M=pointmarkers.begin(); M!=pointmarkers.end(); M++)
          {
            attr[*M]=D->second.conversion_factor*D->second.values[validx];
            validx++;
          }
        }
      }


      if (!globalattribute)
      {
        for (std::map<string,region_t>::const_iterator R=region.begin(); R!=region.end(); R++)
        {
          for (std::map<string,dataset_t>::const_iterator D=R->second.dataset.begin(); D!=R->second.dataset.end(); D++)
          {
            if (D->second.nvalues!=D->second.values.size())
              mythrow("Number of values for dataset " << D->second.name << " on region " << R->second.name << " not ok");

            string segment_name = R->second.name;
            string quantity_name = D->second.name;

            SegmentHandleType const & segment = segmentation(segment_name);

            typedef typename viennagrid::result_of::field<
                typename SegmentedMeshQuantitiesType::VertexValueContainerType,
                VertexType,
                viennagrid::base_id_unpack>::type FieldType;

            FieldType field = quantites.get_vertex_field( mesh, segment.id(), quantity_name );

            typedef typename viennagrid::result_of::const_vertex_range<SegmentHandleType>::type ConstVertexRangeType;
            typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

            ConstVertexRangeType vertices( segment );

            std::map<typename VertexType::id_type, double> tmp;

            int i = 0;
            for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++i)
              field(*vit) = D->second.values[i];
          }
        }
      }
      else
      {
        std::cout << "UNSUPPORTED" << std::endl;
      }
    }




    void write_grid(string filename)
    {
      std::ofstream file(filename.c_str());

      write_grd_header(file);

      if (vertex.size()/dim!=nvertices)
        mythrow("Number of vertices and number of coordinates read not ident");
      file << "Vertices(" << nvertices << ")" <<std::endl << "{" <<std::endl;
      for (int i=0; i<vertex.size(); i++)
      {
        file << vertex[i] << " ";
        if (i%dim==dim-1)
          file <<std::endl;
      }
      file << "}" <<std::endl;

      file << "Edges(" << edges.size() << ")" <<std::endl << "{" <<std::endl;
      for (int i=0; i<edges.size(); i++)
      {
        for (int j=0; j<edges[i].size(); j++)
          file << edges[i][j] << " ";
        file <<std::endl;
      }
      file << "}" <<std::endl;

      file << "Locations(" << edges.size() << ")" <<std::endl << "{" <<std::endl;
      for (int i=0; i<edges.size(); i++)
      {
        for (int j=0; j<edges[i].size(); j++)
        {
          file << "u ";
          if (j%20==19)
            file << std::endl;
        }
      }
      file <<std::endl;
      file << "}" <<std::endl;

      if (dim>2)
      {
      file << "Faces(" << triangles.size() << ")" <<std::endl << "{" <<std::endl;
      for (int i=0; i<triangles.size(); i++)
      {
        for (int j=0; j<triangles[i].size(); j++)
          file << triangles[i][j] << " ";
        file <<std::endl;
      }
      file << "}" <<std::endl;
      }

      file << "Elements(" << elements.size() << ")" <<std::endl << "{" <<std::endl;
      for (int i=0; i<elements.size(); i++)
      {
        file << elements[i].size()-1 << " ";
        for (int j=0; j<elements[i].size(); j++)
          file << elements[i][j] << " ";
        file <<std::endl;
      }
      file << "}" <<std::endl;

      for (std::map<string,region_t>::iterator S=region.begin(); S!=region.end(); S++)
      {
        file << "Region(\"" << S->second.name << "\")" <<std::endl << "{" <<std::endl;
        file << "material = " << S->second.material <<std::endl;

        std::vector<std::vector<int> > &elements=S->second.elements;
        file << "Elements(" << elements.size() << ")" <<std::endl << "{" <<std::endl;
        int j=0;

        for (std::vector<std::vector<int> >::iterator E=elements.begin(); E!=elements.end(); E++)
        {
          const std::vector<int> &e=*E;
          int idx=elementref[e];
          file << idx << " ";
          if (j++%20==19)
            file << std::endl;
        }
        file <<std::endl << "}" <<std::endl;
        file <<std::endl << "}" <<std::endl;
      }
      file <<std::endl << "}" <<std::endl;
      file.close();
    }

    void addsegment(string addsegment, string addmaterial)
    {
      double ymin=1e100;
      for (std::map<string,region_t>::iterator R=region.begin(); R!=region.end(); R++)
      {
        for (int j=0; j<R->second.elements.size(); j++)
        {
          for (int i=0; i<R->second.elements[j].size(); i++)
          {
            double val=-vertex[R->second.elements[j][i]*dim+1];
            if (val<ymin)
              ymin=val;
          }
        }
      }
     std::cerr << "add segment at " << ymin <<std::endl;
      region_t newseg;
      newseg.regnr=region.size();
      newseg.name=addsegment;
      newseg.material=addmaterial;

      for (std::map<string,region_t>::iterator R=region.begin(); R!=region.end(); R++)
      {
        for (int j=0; j<R->second.elements.size(); j++)
        {
          std::vector<int> tmpelement;
          for (int i=0; i<R->second.elements[j].size(); i++)
          {
            double val=-vertex[R->second.elements[j][i]*dim+1];
            if (val==ymin)
              tmpelement.push_back(R->second.elements[j][i]);
          }
          if (tmpelement.size()==dim)
          {
            newseg.elements.push_back(tmpelement);
          }
        }
      }

      newseg.nelements=newseg.elements.size();
      newseg.npointidx=0;
      region[addsegment]=newseg;
      nregions++;
    }

          void correct_vertices()
    {
      std::map<int,int> hakerl;
      std::vector<double> vertexsave;
      for (std::map<string,region_t>::iterator R=region.begin(); R!=region.end(); R++)
      {
        for (int j=0; j<R->second.elements.size(); j++)
        {
          for (int i=0; i<R->second.elements[j].size(); i++)
            hakerl[R->second.elements[j][i]];
        }
      }
      int ct=0;
      if (hakerl.size()*dim==vertex.size())
        return;
      vertexsave=vertex;
      vertex.clear();
      for (std::map<int,int>::iterator I=hakerl.begin(); I!=hakerl.end(); I++)
      {
        vertex.push_back(vertexsave[I->first*dim]);
        if (dim==2)
          vertex.push_back(vertexsave[I->first*dim+1]);
        if (dim==3)
          vertex.push_back(vertexsave[I->first*dim+2]);
        I->second=ct++;
      }
      for (std::map<string,region_t>::iterator R=region.begin(); R!=region.end(); R++)
      {
        for (int j=0; j<R->second.elements.size(); j++)
        {
          for (int i=0; i<R->second.elements[j].size(); i++)
          {
            int idx=hakerl[R->second.elements[j][i]];
            R->second.elements[j][i]=idx;
          }
        }
      }
      nvertices=vertex.size()/dim;
     std::cerr << "vertices: " << vertex.size()/dim <<std::endl;
    }
    void cut_segments(const double yval, const int below=1)
    {
      int cutfound=0;
      std::map<string,region_t>::iterator R,R2;
      for (R2=region.begin(); R2!=region.end(); )
      {
        R=R2++;
       std::cerr << "Region " << R->first <<std::endl;
        region_t &reg=R->second;

       std::cerr << "  Elements " << reg.elements.size() <<std::endl;
        std::set<int> eraseelements;
        int ptct=0;
        for (int j=0; j<reg.elements.size(); j++)
        {
          std::vector<int> belowct;
          for (int i=0; i<reg.elements[j].size(); i++)
          {
            if (vertex[reg.elements[j][i]*dim+1]*below>=-yval*below)
              belowct.push_back(reg.elements[j][i]);
          }
          if (belowct.size()==0)
          {
            continue;
          }
          if (belowct.size()==reg.elements[j].size())
          {
            eraseelements.insert(j);
            continue;
          }
          for (int i=0; i<belowct.size(); i++)
          {
            ptct++;
            vertex[belowct[i]*dim+1]=-yval;
          }
        }
       std::cerr << "  Points moved " << ptct <<std::endl;
        region_t savereg;
        if (eraseelements.size())
        {
          savereg=R->second;
          reg.elements.clear();
          for (int j=0; j<savereg.elements.size(); j++)
            if (eraseelements.find(j)==eraseelements.end())
              reg.elements.push_back(savereg.elements[j]);
        }
        else
        {
         std::cerr << "    Nothing to do" <<std::endl;
          continue;
        }
       std::cerr << "  Elements after remove " << reg.elements.size() <<std::endl;
        if (reg.elements.size()==0)
        {
         std::cerr << "    Remove region " << reg.name <<std::endl;
          nregions--;
          ndatasets-=R->second.dataset.size();
          region.erase(R);
          continue;
        }
        if (R->second.dataset.size()==0)
        {
         std::cerr << "    No datasets to move" <<std::endl;
          continue;
        }
       std::cerr << "  Move datasets" <<std::endl;
        std::map<int,int> savevalmap,valmap;
        std::map<int,int>::iterator saveI,I;
        std::vector<std::vector<int> >::iterator E;
        int ct=0;
        for (E=savereg.elements.begin(); E!=savereg.elements.end(); E++)
          for (int i=0; i<E->size(); i++)
            if (savevalmap.find((*E)[i])==savevalmap.end())
              savevalmap[(*E)[i]];
        for (saveI=savevalmap.begin(); saveI!=savevalmap.end(); saveI++)
          saveI->second=ct++;
        ct=0;
        for (E=reg.elements.begin(); E!=reg.elements.end(); E++)
          for (int i=0; i<E->size(); i++)
            if (valmap.find((*E)[i])==valmap.end())
              valmap[(*E)[i]];
        for (saveI=valmap.begin(); saveI!=valmap.end(); saveI++)
          saveI->second=ct++;
       std::cerr << "  Values prepared" <<std::endl;

        reg.nelements=reg.elements.size();
        reg.npointidx=valmap.size();
        std::map<string,dataset_t>::iterator D,saveD;
        for (D=reg.dataset.begin(); D!=reg.dataset.end(); D++)
        {
         std::cerr << "    Dataset " << D->first <<std::endl;
          if (D->second.values.size()<=0)
            continue;
          D->second.values.clear();
          D->second.values.reserve(valmap.size());
          saveD=savereg.dataset.find(D->first);
          for (saveI=valmap.begin(); saveI!=valmap.end(); saveI++)
          {
            double val=saveD->second.values[savevalmap[saveI->first]];
            D->second.values[saveI->second]=val;
          }
          D->second.nvalues=D->second.values.size();
         std::cerr << "      moved" <<std::endl;
        }
       std::cerr << "    Region done" <<std::endl;
      }
     std::cerr << "  cut done" <<std::endl;

     std::cerr << "nvertices: " << nvertices <<std::endl;
     std::cerr << "nregions: " << nregions <<std::endl;
     std::cerr << "ndatasets: " << ndatasets <<std::endl;
    }
    void write_dat(string filename)
    {
      std::ofstream file(filename.c_str());

      for (std::map<string,region_t>::const_iterator R=region.begin(); R!=region.end(); R++)
      {
        std::set<int> pointmarkers;
        const std::vector<std::vector<int> > &elements=R->second.elements;
        for (int i=0; i<elements.size(); i++)
          for (int j=0; j<elements[i].size(); j++)
            pointmarkers.insert(elements[i][j]);

        for (std::map<string,dataset_t>::const_iterator D=R->second.dataset.begin(); D!=R->second.dataset.end(); D++)
        {
          if (globalattributes[D->first].firstattribute==NULL)
            globalattributes[D->first].firstattribute=&D->second;
          std::map<int,double> &attr=globalattributes[D->first].values;
          int validx=0;
          for (std::set<int>::const_iterator M=pointmarkers.begin(); M!=pointmarkers.end(); M++)
          {
            attr[*M]=D->second.conversion_factor*D->second.values[validx];
            validx++;
          }
        }
      }

      write_dat_header(file);


      file << "Data {" <<std::endl;
      if (!globalattribute)
      {
          for (std::map<string,region_t>::const_iterator R=region.begin(); R!=region.end(); R++)
        for (std::map<string,dataset_t>::const_iterator D=R->second.dataset.begin(); D!=R->second.dataset.end(); D++)
        {
          if (removeattribute.find(D->second.name)!=removeattribute.end())
            continue;
          file << "  Dataset (\"" << D->second.name << "\") {" <<std::endl;
          file << "    function  = " << D->second.quantity <<std::endl;
          file << "    type      = scalar" <<std::endl;
          file << "    dimension = 1" <<std::endl;
          file << "    location  = vertex" <<std::endl;
          file << "    validity  = [ \"" << R->second.name << "\" ]" <<std::endl;
          file << "    Values (" << D->second.nvalues << ") {" <<std::endl;

         std::cerr << " Numberofvalues: npointidx=" << R->second.npointidx << " nvalues=" << D->second.nvalues <<std::endl;

          if (D->second.nvalues!=D->second.values.size())
            mythrow("Number of values for dataset " << D->second.name << " on region " << R->second.name << " not ok");
          for (int i=0; i<D->second.nvalues; i++)
            file << "      " << D->second.values[i] <<std::endl;
          file << "    }" <<std::endl;
          file << "  }" <<std::endl;
        }
      }
      else
      {
            for (std::map<string,attributeinfo_c>::iterator A=globalattributes.begin(); A!=globalattributes.end(); A++)
        {
          if (removeattribute.find(A->first)!=removeattribute.end())
            continue;
          file << "  Dataset (\"" << A->first << "\") {" <<std::endl;
          file << "    function  = " << A->second.firstattribute->quantity <<std::endl;
          file << "    type      = scalar" <<std::endl;
          file << "    dimension = 1" <<std::endl;
          file << "    location  = vertex" <<std::endl;
          file << "    validity  = [ ";
          for (std::map<string,region_t>::iterator S=region.begin(); S!=region.end(); S++)
            file << "\"" << S->first << "\" ";
          file << "]" <<std::endl;
          file << "    Values (" << nvertices << ") {" <<std::endl;

          //cerr << " Numberofvalues: npointidx=" << R->second.npointidx << " nvalues=" << D->second.nvalues <<std::endl;

          //if (D->second.nvalues!=D->second.values.size())
          //  mythrow("Number of values for dataset " << D->second.name << " on region " << R->second.name << " not ok");
          for (int i=0; i<nvertices; i++)
            file << "      " << A->second.values[i] <<std::endl;
          file << "    }" <<std::endl;
          file << "  }" <<std::endl;
        }
      }
      file << "}" <<std::endl;
      file.close();
    }

    void prepare_grid(string filename)
    {
      int i1,i2,i3,i4,i5,i6,trii1,trii2,trii3,trii4,teti;
      for (std::map<string,region_t>::iterator S=region.begin(); S!=region.end(); S++)
      {
        std::map<int,int> pointidx;
        for (std::vector<std::vector<int> >::iterator E=S->second.elements.begin(); E!=S->second.elements.end(); E++)
        {
          std::vector<int> &e=*E;
          if (e.size()==dim)
            continue;
          if (e.size()!=dim+1)
                mythrow("Element unknown");

          switch (dim)
          {
          case 1:
          {
            pointidx[e[0]];
            pointidx[e[1]];
            i1=insert_edge(e[0],e[1],1);
            elementref[e]=elements.size();
            elements.push_back(edges[i1]);
            continue;
          }
          case 2:
          {
            pointidx[e[0]];
            pointidx[e[1]];
            pointidx[e[2]];
            i1=insert_edge(e[0],e[1],1);
            i2=insert_edge(e[1],e[2],1);
            i3=insert_edge(e[2],e[0],1);
            trii1=insert_tri(i1,i2,i3,1);
            elementref[e]=elements.size();
            elements.push_back(triangles[trii1]);
            continue;
          }
          case 3:
          {
            pointidx[e[0]];
            pointidx[e[1]];
            pointidx[e[2]];
            pointidx[e[3]];
            i1=insert_edge(e[0],e[1],0);
            i2=insert_edge(e[1],e[2],0);
            i3=insert_edge(e[2],e[0],0);
            i4=insert_edge(e[0],e[3],0);
            i5=insert_edge(e[1],e[3],0);
            i6=insert_edge(e[2],e[3],0);
            trii1=insert_tri(i1,i2,i3,1);
            trii2=insert_tri(i4,-i5-1,-i1-1,1);
            trii3=insert_tri(i5,-i6-1,-i2-1,1);
            trii4=insert_tri(i6,-i4-1,-i3-1,1);
            teti=insert_tet(trii1,trii2,trii3,trii4,1);
            elementref[e]=elements.size();
            elements.push_back(tets[teti]);
            continue;
          }
          }
        }
        S->second.npointidx=pointidx.size();
      }
      for (std::map<string,region_t>::iterator S=region.begin(); S!=region.end(); S++)
      {
        for (std::vector<std::vector<int> >::iterator E=S->second.elements.begin(); E!=S->second.elements.end(); E++)
        {
          std::vector<int> &e=*E;
          if (e.size()==dim+1)
            continue;
          if (e.size()!=dim)
                mythrow("Element unknown");

          std::vector<int> tempel;

          switch (dim)
          {
          case 1:
          {
            tempel.push_back(e[0]);
            elementref[e]=elements.size();
            elements.push_back(tempel);
            continue;
          }
          case 2:
          {
            tempel.push_back(e[0]);
            tempel.push_back(e[1]);
            elementref[e]=elements.size();
            elements.push_back(tempel);
            continue;
          }
          case 3:
          {
            i1=insert_edge(e[0],e[1],0);
            i2=insert_edge(e[1],e[2],0);
            i3=insert_edge(e[2],e[0],0);
            tempel.push_back(i1);
            tempel.push_back(i2);
            tempel.push_back(i3);
            elementref[e]=elements.size();
            elements.push_back(tempel);
            continue;
          }
          }
        }
      }
    }
  };


}

#endif
