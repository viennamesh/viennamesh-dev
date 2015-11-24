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

#include "sentaurus_tdr_writer.hpp"

#include <fstream>

#include <boost/container/flat_map.hpp>
#include <boost/lexical_cast.hpp>

#include "viennagrid/core/range.hpp"

#include "H5Cpp.h"

namespace viennamesh
{
namespace
{
typedef viennagrid::const_mesh                                          MeshType;
typedef viennagrid::result_of::element<MeshType>::type                  ElementType;
typedef viennagrid::result_of::point<MeshType>::type                    PointType;
typedef viennagrid::result_of::region<MeshType>::type                   RegionType;
typedef viennagrid::result_of::id<ElementType>::type                    ElementId;
typedef viennagrid::result_of::id<RegionType>::type                     RegionId;

typedef viennagrid::result_of::const_vertex_range<MeshType>::type       MeshVertexRange;
typedef viennagrid::result_of::iterator<MeshVertexRange>::type          MeshVertexIterator;

typedef viennagrid::result_of::const_vertex_range<ElementType>::type    BoundaryVertexRange;
typedef viennagrid::result_of::iterator<BoundaryVertexRange>::type      BoundaryVertexIterator;

typedef viennagrid::result_of::const_region_range<MeshType>::type       RegionRange;
typedef viennagrid::result_of::iterator<RegionRange>::type              RegionIterator;

typedef viennagrid::result_of::const_vertex_range<RegionType>::type     RegionVertexRange;
typedef viennagrid::result_of::iterator<RegionVertexRange>::type        RegionVertexIterator;

typedef viennagrid::result_of::const_cell_range<RegionType>::type       CellRange;
typedef viennagrid::result_of::iterator<CellRange>::type                CellIterator;

void write_attribute(H5::H5Object & obj, std::string const & name, std::string const & value)
{
  H5::StrType strdatatype(H5::PredType::C_S1, value.size());
  H5::Attribute attr = obj.createAttribute(name, strdatatype, H5::DataSpace(H5S_SCALAR));
  attr.write(strdatatype, value);
}

void write_attribute(H5::H5Object & obj, std::string const & name, int value)
{
  H5::Attribute attr = obj.createAttribute(name, H5::PredType::NATIVE_INT, H5::DataSpace());
  attr.write(H5::PredType::NATIVE_INT, &value);
}

void write_attribute(H5::H5Object & obj, std::string const & name, double value)
{
  H5::Attribute attr = obj.createAttribute(name, H5::PredType::NATIVE_DOUBLE, H5::DataSpace());
  attr.write(H5::PredType::NATIVE_DOUBLE, &value);
}

template <typename T>
H5::DataSet write_dataset(H5::Group & group, std::string const & name, H5::DataType const & type, hsize_t length, std::vector<T> const & data)
{
  H5::DataSpace dataspace(1, &length);
  H5::DataSet dataset = group.createDataSet(name, type, dataspace);
  dataset.write(&data[0], type);
  return dataset;
}

} //end of anonymous namespace

void write_to_tdr(std::string const & filename, viennagrid::const_mesh const & mesh, std::vector<viennagrid::quantity_field> const & quantities)
{
  unsigned int dimension = viennagrid::geometric_dimension(mesh);
  if (dimension != 2)
  {
    throw viennautils::make_exception<tdr_writer_error>("TDR writer currently supports only two dimensional meshes");
  }

  try
  {
    H5::H5File file(filename, H5F_ACC_TRUNC);

    H5::Group collection = file.createGroup("collection");

    write_attribute(collection, "number of geometries", 1);
    write_attribute(collection, "number of plots", 0);

    H5::Group geometry = collection.createGroup("geometry_0");

    write_attribute(geometry, "type", 1);
    write_attribute(geometry, "dimension", static_cast<int>(dimension));
    write_attribute(geometry, "number of vertices", static_cast<int>(viennagrid::vertices(mesh).size()));
    write_attribute(geometry, "number of regions", static_cast<int>(mesh.region_count()));
    write_attribute(geometry, "number of states", 1);

    {
      //transformation Group
      //for some reason this is apparently always 3d...
      H5::Group transformation = geometry.createGroup("transformation");
      write_attribute(transformation, "type", 1);

      std::vector<double> identity_matrix(9, 0.0);
      identity_matrix[0] = identity_matrix[4] = identity_matrix[8] = 1.0;
      write_dataset(transformation, "A", H5::PredType::NATIVE_DOUBLE, 9, identity_matrix);

      std::vector<double> zero_vector(3, 0.0);
      write_dataset(transformation, "b", H5::PredType::NATIVE_DOUBLE, 3, zero_vector);
    }

    boost::container::flat_map<ElementId, int32_t> vertex_mapping; //should (and has to) be monotonous 'on both sides'
    vertex_mapping.reserve(viennagrid::vertices(mesh).size());

    {
      //vertex Dataset
      std::vector<double> vertex_coordinates;
      vertex_coordinates.reserve(dimension*viennagrid::vertices(mesh).size());
      MeshVertexRange mesh_vertices(mesh);
      int id_counter = 0;
      for (MeshVertexIterator it = mesh_vertices.begin(); it != mesh_vertices.end(); ++it, ++id_counter)
      {
        vertex_mapping[(*it).id()] = id_counter;

        PointType const & p = viennagrid::get_point(*it);
        for (PointType::size_type i = 0; i < dimension; ++i)
        {
          vertex_coordinates.push_back(p[i]);
        }
      }

      H5::CompType vertex_type( dimension*sizeof(double) );
      vertex_type.insertMember( "x", 0, H5::PredType::NATIVE_DOUBLE);
      if (dimension > 1)
      {
        vertex_type.insertMember( "y", sizeof(double), H5::PredType::NATIVE_DOUBLE);
      }
      if (dimension > 2)
      {
        vertex_type.insertMember( "z", 2*sizeof(double), H5::PredType::NATIVE_DOUBLE);
      }

      write_dataset(geometry, "vertex", vertex_type, vertex_coordinates.size()/dimension, vertex_coordinates);
    }

    RegionRange regions(mesh);
    {
      //region Groups
      int region_counter = 0;
      for (RegionIterator region_it = regions.begin(); region_it != regions.end(); ++region_it, ++region_counter)
      {
        //write name etc.
        H5::Group region_group = geometry.createGroup("region_" + boost::lexical_cast<std::string>(region_counter));
        write_attribute(region_group, "type", 0);
        write_attribute(region_group, "name", (*region_it).get_name());
        write_attribute(region_group, "material", "unknown"); //TODO
        write_attribute(region_group, "number of parts", 1);

        std::vector<int32_t> region_element_data;

        CellRange cells(*region_it);
        unsigned int num_elements = 0;
        for (CellIterator cell_it = cells.begin(); cell_it != cells.end(); ++cell_it, ++num_elements)
        {
          if (!(*cell_it).tag().is_triangle())
          {
            throw viennautils::make_exception<tdr_writer_error>("Only triangles are supported at the moment");
          }

          int32_t const triangle_id = 2;
          region_element_data.push_back(triangle_id);

          BoundaryVertexRange vertices(*cell_it);
          for (BoundaryVertexIterator vertex_it = vertices.begin(); vertex_it != vertices.end(); ++vertex_it)
          {
            region_element_data.push_back(vertex_mapping[(*vertex_it).id()]);
          }
        }

        H5::DataSet elements = write_dataset(region_group, "elements_0", H5::PredType::NATIVE_INT32, region_element_data.size(), region_element_data);
        write_attribute(elements, "number of elements", static_cast<int>(num_elements));
      }
    }

    if (!quantities.empty())
    {
      //datasets
      H5::Group state = geometry.createGroup("state_0");
      write_attribute(state, "name", "state_0");
      write_attribute(state, "number of plots", 0);
      write_attribute(state, "number of string streams", 0);
      write_attribute(state, "number of xy plots", 0);

      typedef boost::container::flat_map<RegionId, std::vector<ElementId> > RegionVertexIdMap;
      RegionVertexIdMap region_vert_ids;
      region_vert_ids.reserve(mesh.region_count());
      for (RegionIterator region_it = regions.begin(); region_it != regions.end(); ++region_it)
      {
        std::vector<ElementId> & vertex_ids = region_vert_ids[(*region_it).id()];
        RegionVertexRange vertices(*region_it);
        for (RegionVertexIterator vertex_it = vertices.begin(); vertex_it != vertices.end(); ++vertex_it)
        {
          vertex_ids.push_back((*vertex_it).id());
        }
      }

      int num_datasets = 0;
      for (unsigned int i = 0; i < quantities.size(); ++i)
      {
        viennagrid::quantity_field const & quantity = quantities[i];
        int region_num = 0;
        for (RegionVertexIdMap::const_iterator region_it = region_vert_ids.begin(); region_it != region_vert_ids.end(); ++region_it, ++region_num)
        {
          std::vector<ElementId> const & vertex_ids = region_it->second;
          std::vector<double> values;
          values.reserve(vertex_ids.size());
          for (unsigned int j = 0; j < vertex_ids.size(); ++j)
          {
            if (!quantity.valid(vertex_ids[j].index()))
            {
              break;
            }
            values.push_back(quantity.get(vertex_ids[j].index()));
          }

          if (values.size() == vertex_ids.size()) //only write quantities that are defined on the entire region
          {
            H5::Group dataset_group = state.createGroup("dataset_" + boost::lexical_cast<std::string>(num_datasets++));
            write_attribute(dataset_group, "number of values", static_cast<int>(values.size()));
            write_attribute(dataset_group, "location type", 0);
            write_attribute(dataset_group, "structure type", 0);
            write_attribute(dataset_group, "value type", 2);
            write_attribute(dataset_group, "name", quantity.get_name());
            write_attribute(dataset_group, "quantity", quantity.get_name());
            write_attribute(dataset_group, "conversion factor", 1.0);
            write_attribute(dataset_group, "region", region_num);
            write_attribute(dataset_group, "unit:name", "unknown"); //TODO
            write_dataset(dataset_group, "values", H5::PredType::NATIVE_DOUBLE, values.size(), values);
          }
        }
      }
      write_attribute(state, "number of datasets", num_datasets);
    }
  }
  catch(H5::Exception const & e)
  {
    throw viennautils::make_exception<tdr_writer_error>("caught HDF5 exception in HDF5 function: " + e.getFuncName() + " - with message: " + e.getDetailMsg());
  }
}

} //end of namespace viennagrid
