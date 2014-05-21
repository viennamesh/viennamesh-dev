#ifndef VIENNAMESH_ALGORITHM_IO_SENTAURUS_TDR_READER_HPP
#define VIENNAMESH_ALGORITHM_IO_SENTAURUS_TDR_READER_HPP

/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

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

#include "viennagrid/algorithm/cross_prod.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/inclusion.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/algorithm/norm.hpp"


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


  int read_int(const H5Object &g, const string name);
  double read_double(const H5Object &g, const string name);
  string read_string(const H5Object &g, const string name);

  struct dataset_t
  {
    string name, quantity, unit;
    unsigned int nvalues;
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


  template<typename VertexHandleT>
  struct region_contacts
  {
    string segment_name;
    std::vector< std::vector<VertexHandleT> > vertex_handles;
  };

  struct tdr_geometry
  {
    unsigned int nvertices;
    int dim,nregions,ndatasets;
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
      hsize_t dims[10];
      dataspace.getSimpleExtentDims( dims, NULL);
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
      coord2_t * s2 = new coord2_t[dims[0]];
      vert.read( s2, mtype2 );

      for (unsigned int i=0; i<dims[0]; i++)
      {
        vertex.push_back(s2[i].x[0]*10000.);
        if (dim>1)
        vertex.push_back(s2[i].x[1]*10000.);
        if (dim>2)
        vertex.push_back(s2[i].x[2]*10000.);
      }

      delete[] s2;
    }

    void read_elements(region_t &region, const DataSet &elem)
    {
      const DataSpace &dataspace = elem.getSpace();
      int rank = dataspace.getSimpleExtentNdims();
      hsize_t dims[10];
      int ndims = dataspace.getSimpleExtentDims( dims, NULL);

      if (rank!=1)
        mythrow("rank of elements in region " << region.name << " is not one");
      if (ndims!=1)
        mythrow("ndims of elements in region " << region.name << " is not one");

      int * el = new int[dims[0]];
      elem.read( el, PredType::NATIVE_INT);

      unsigned int elct=0;
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

      delete[] el;
    }

    void read_region(const int regnr, const Group &reg)
    {
      string name0=read_string(reg,"name");
      string name;
      string::size_type i;
      while ((i=name0.find_first_of("_."))!=name0.npos)
      {
        name=name+name0.substr(0,i);
        name0=name0.substr(i+1);
      }
      name=name+name0;
      std::stringstream ss;
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
        case 2:
          ss << "Interface" << read_int(reg,"bulk 0") << read_int(reg,"bulk 1");
          material = ss.str();
          break;
        //default : mythrow(__LINE__ << ": Unknown type " << typ << " in region " << read_string(reg,"name"));
        default :std::cerr << __LINE__ << ": Unknown type " << typ << " in region " << read_string(reg,"name") << std::endl;
         std::cerr << "I do not throw, I only skip\n";
          return;
      }

//       const int nparts=read_int(reg,"number of parts");
//       if (nparts!=1)
//       {
//         //mythrow("Number of parts not one");
//        std::cerr << "Number of parts not one (" << nparts << ")\n";
//        std::cerr << "I only skip this\n";
//       }

      region[name].regnr=regnr;
      region[name].name=name;
      region[name].material=material;
      const int n=reg.getNumObjs();
      for (int i=0; i<n; i++)
      {
        std::string name = reg.getObjnameByIdx(i);
        if (name.find("elements_") == 0)
        {
          const DataSet &ds=reg.openDataSet(name);
          region[name].nelements=read_int(ds,"number of elements");
          read_elements(region[name],ds);
        }

        if (name.find("part_") == 0)
        {
          const Group &part=reg.openGroup(name);
          region[name].nelements=read_int(part,"number of elements");
          read_elements(region[name],part.openDataSet("elements"));
        }
      }

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
//       int rank = dataspace.getSimpleExtentNdims();
      hsize_t dims[10];
      int ndims = dataspace.getSimpleExtentDims( dims, NULL);
      if (dataset.nvalues!=dims[0] || ndims!=1)
        mythrow("Dataset " << dataset.name << " should have " << dataset.nvalues << " values, but has " << dims[0] << " with dimension " << ndims);

      double * v = new double[dims[0]];
      values.read( v, PredType::NATIVE_DOUBLE);
      dataset.values.insert(dataset.values.end(),&v[0],&v[dims[0]]);
      delete[] v;
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
        mythrow("Dataset " << name << " location type not 0");
        return;
      }

      if (read_int(dataset,"structure type") != 0)
        mythrow("Dataset " << name << " structure type not 0");
      if (read_int(dataset,"value type") != 2)
        mythrow("Dataset " << name << " value type not 2");

      // In this dataset we have a group
      // tag_group_0???
      // units: take only the unit name
      // Dataset values: the actual values

      string unit;
      int n=dataset.getNumObjs(),i;
      for (i=0; i<n; i++)
      {
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
      dim=read_int(geometry,"dimension");

      nvertices=read_int(geometry,"number of vertices");

      nregions=read_int(geometry,"number of regions");

      dum=read_int(geometry,"number of states");
      if (dum!=1)
        mythrow("Number of states not one");

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


    template<typename PointT>
    PointT normal_vector(PointT const & p0, PointT const & p1)
    {
      PointT line = p1-p0;
      std::swap(line[0], line[1]);
      line[0] = -line[0];
      return line;
    }

    template<typename PointT>
    PointT normal_vector(PointT const & p0, PointT const & p1, PointT const & p2)
    {
      return viennagrid::cross_prod( p1-p0, p2-p0 );
    }

    template<typename MeshT, typename SegmentationT>
    bool to_viennagrid(MeshT & mesh, SegmentationT & segmentation, bool extrude_contacts = true)
    {
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef typename viennagrid::result_of::vertex<MeshT>::type VertexType;
      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;
      typedef typename viennagrid::result_of::cell_tag<MeshT>::type CellTag;

      const int mesh_vertices_per_element = viennagrid::boundary_elements<CellTag, viennagrid::vertex_tag>::num;
      const int mesh_dimension = viennagrid::result_of::geometric_dimension<MeshT>::value;

      if (mesh_dimension != dim)
        return false;

      std::vector<VertexHandleType> vertex_handles( nvertices );

      for (unsigned int i=0; i<nvertices; i++)
      {
        PointType point;
        for (int j = 0; j < dim; ++j)
          point[j] = vertex[dim*i+j];

        vertex_handles[i] = viennagrid::make_vertex(mesh, point);
      }

      typedef typename viennagrid::result_of::segment_handle<SegmentationT>::type SegmentHandleType;

      typedef typename viennagrid::result_of::vertex_id<MeshT>::type VertexIDType;



      std::map<string, region_contacts<VertexHandleType> > contact_elements;


      for (std::map<string,region_t>::iterator S=region.begin(); S!=region.end(); S++)
      {
        string segment_name = S->second.name;

        std::vector<std::vector<int> > &elements=S->second.elements;
        for (std::vector<std::vector<int> >::iterator E=elements.begin(); E!=elements.end(); E++)
        {
          const std::vector<int> &e=*E;

          std::vector<VertexHandleType> handles(e.size());
          for (std::size_t i = 0; i < e.size(); ++i)
            handles[i] = vertex_handles[e[i]];

          if (e.size() == mesh_vertices_per_element)
            viennagrid::make_cell( segmentation(segment_name), handles.begin(), handles.end() );
          else
          {
            contact_elements[segment_name].segment_name = segment_name + "_contact";
            contact_elements[segment_name].vertex_handles.push_back(handles);
          }
        }
      }

      if (extrude_contacts)
      {
        for (typename std::map<string, region_contacts<VertexHandleType> >::iterator rc = contact_elements.begin(); rc != contact_elements.end(); ++rc)
        {
          for (std::size_t i = 0; i < rc->second.vertex_handles.size(); ++i)
          {
            std::vector<VertexHandleType> handles = rc->second.vertex_handles[i];

            PointType center;
            PointType normal;
            double size = 0;

            if (handles.size() == 2)
            {
              VertexType const & v0 = viennagrid::dereference_handle(mesh, handles[0]);
              VertexType const & v1 = viennagrid::dereference_handle(mesh, handles[1]);

              PointType const & p0 = viennagrid::point(v0);
              PointType const & p1 = viennagrid::point(v1);

              center = (p0+p1)/2;
              normal = normal_vector(p0, p1);

              size = std::max(size, viennagrid::distance(center, p0));
              size = std::max(size, viennagrid::distance(center, p1));
            }
            else if (handles.size() == 3)
            {
              VertexType const & v0 = viennagrid::dereference_handle(mesh, handles[0]);
              VertexType const & v1 = viennagrid::dereference_handle(mesh, handles[1]);
              VertexType const & v2 = viennagrid::dereference_handle(mesh, handles[2]);

              PointType const & p0 = viennagrid::point(v0);
              PointType const & p1 = viennagrid::point(v1);
              PointType const & p2 = viennagrid::point(v2);

              center = (p0+p1+p2)/3;
              normal = normal_vector(p0, p1, p2);

              size = std::max(size, viennagrid::distance(center, p0));
              size = std::max(size, viennagrid::distance(center, p1));
              size = std::max(size, viennagrid::distance(center, p2));
            }
            else
            {
              std::cout << "NOT SUPPORTED" << std::endl;
            }

            normal /= viennagrid::norm_2(normal);
            normal *= size;

            PointType other_vertex = center + normal;

            typedef typename viennagrid::result_of::cell_range<MeshT>::type CellRangeType;
            typedef typename viennagrid::result_of::iterator<CellRangeType>::type CellIteratorType;

            CellRangeType cells(mesh);
            CellIteratorType cit = cells.begin();
            for (; cit != cells.end(); ++cit)
            {
              if ( viennagrid::is_inside(*cit, other_vertex) )
                break;
            }

            if (cit != cells.end())
              other_vertex = center - normal;

            typedef typename viennagrid::result_of::cell_handle<MeshT>::type CellHandleType;

            handles.push_back( viennagrid::make_vertex(mesh, other_vertex) );
            viennagrid::make_cell( segmentation( rc->second.segment_name ), handles.begin(), handles.end() );
          }
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

          FieldType field = quantites.template get_vertex_field<MeshT>( segment.id(), quantity_name );

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

    void correct_vertices()
    {
      std::map<int,int> hakerl;
      std::vector<double> vertexsave;
      for (std::map<string,region_t>::iterator R=region.begin(); R!=region.end(); R++)
      {
        for (unsigned int j=0; j<R->second.elements.size(); j++)
        {
          for (unsigned int i=0; i<R->second.elements[j].size(); i++)
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
        for (unsigned int j=0; j<R->second.elements.size(); j++)
        {
          for (unsigned int i=0; i<R->second.elements[j].size(); i++)
          {
            int idx=hakerl[R->second.elements[j][i]];
            R->second.elements[j][i]=idx;
          }
        }
      }
      nvertices=vertex.size()/dim;
    }

  };


}

#endif
