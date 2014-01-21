#include "viennamesh/algorithm/io/mesh_reader.hpp"

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/tetgen_poly_reader.hpp"
#include "viennamesh/algorithm/io/gts_deva_geometry_reader.hpp"

template<typename CellTagT, unsigned int GeometricDimensionV>
bool is_mesh_type( unsigned int vtk_cell_type, unsigned int geometric_dimension )
{
  return (viennagrid::io::ELEMENT_TAG_TO_VTK_TYPE<CellTagT>::value == vtk_cell_type) && (geometric_dimension == GeometricDimensionV);
}

template<typename CellTagT, unsigned int GeometricDimensionV>
bool is_mesh_type( std::string const & cell_type, unsigned int geometric_dimension )
{
  return (CellTagT::name() == cell_type) && (geometric_dimension == GeometricDimensionV);
}


template<typename CellTagT, unsigned int GeometricDimensionV>
bool viennamesh::io::mesh_reader::generic_read_vtk( string const & filename )
{
  typedef typename viennamesh::result_of::full_config<CellTagT, GeometricDimensionV>::type ConfigType;
  typedef typename viennagrid::mesh<ConfigType> MeshType;
  typedef typename viennagrid::result_of::segmentation<MeshType>::type SegmentationType;

  output_parameter_proxy< viennagrid::segmented_mesh<MeshType, SegmentationType> > output_mesh = output_proxy< viennagrid::segmented_mesh<MeshType, SegmentationType> >("default");

  viennagrid::io::vtk_reader<MeshType, SegmentationType> vtk_reader;
  vtk_reader(output_mesh().mesh, output_mesh().segmentation, filename);
  return true;
}




template<int GeometricDimensionV>
bool viennamesh::io::mesh_reader::read_seed_points( pugi::xml_document const & xml )
{
  typedef typename viennamesh::result_of::point<GeometricDimensionV>::type PointType;
  typedef typename viennamesh::result_of::seed_point_container<PointType>::type SeedPointContainerType;

  output_parameter_proxy<SeedPointContainerType> output_seed_points = output_proxy<SeedPointContainerType>("seed_points");

  pugi::xpath_node_set xml_segments = xml.select_nodes( "/mesh/segment" );
  for (pugi::xpath_node_set::const_iterator xsit = xml_segments.begin(); xsit != xml_segments.end(); ++xsit)
  {
    int segment_id;
    pugi::xpath_node_set xml_segment_id = xsit->node().select_nodes( "/id" );
    if (!xml_segment_id.empty())
      segment_id = atoi( xml_segment_id.first().node().text().get() );
    else
      continue;

    pugi::xpath_node_set xml_seed_points = xsit->node().select_nodes( "/seed_point" );
    for (pugi::xpath_node_set::const_iterator xspit = xml_seed_points.begin(); xspit != xml_seed_points.end(); ++xspit)
    {
      std::stringstream ss( xspit->node().text().get() );
      PointType point;
      for (std::size_t i = 0; i < point.size(); ++i)
        ss >> point[i];

      info(5) << "Found seed point for segment " << segment_id << ": " << point << std::endl;

      output_seed_points().push_back( std::make_pair(point, segment_id) );
    }
  }

  return true;
}




bool viennamesh::io::mesh_reader::read_vmesh( string const & filename )
{
  int geometric_dimension = -1;
  string mesh_filename;

  pugi::xml_document xml;
  xml.load_file( filename.c_str() );

  {
    pugi::xpath_node_set nodes = xml.select_nodes( "/mesh/dimension" );
    if (!nodes.empty())
    {
      geometric_dimension = atoi( nodes.first().node().text().get() );
      info(5) << "geometric dimension = " << geometric_dimension << std::endl;
    }
  }


  {
    pugi::xpath_node_set nodes = xml.select_nodes( "/mesh/file" );
    if (!nodes.empty())
    {
      mesh_filename = nodes.first().node().text().get();
      info(5) << "vmesh inner mesh filename = " << mesh_filename << std::endl;
    }
  }

  load(mesh_filename);

  if (geometric_dimension == 1)
    read_seed_points<1>(xml);
  else if (geometric_dimension == 2)
    read_seed_points<2>(xml);
  else if (geometric_dimension == 3)
    read_seed_points<3>(xml);

  return true;
}



bool viennamesh::io::mesh_reader::load( string const & filename )
{
  string path = stringtools::extract_path( filename );
  string extension = filename.substr( filename.rfind(".")+1 );

  parameter_handle result;

  if (extension == "mesh")
  {
    info(5) << "Found .mesh extension, using ViennaGrid Netgen Reader" << std::endl;
    typedef viennagrid::segmented_mesh<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> MeshType;

    output_parameter_proxy<MeshType> output_mesh = output_proxy<MeshType>("default");

    viennagrid::io::netgen_reader reader;
    reader(output_mesh().mesh, output_mesh().segmentation, filename);

    return true;
  }
  else if (extension == "poly")
  {
    info(5) << "Found .poly extension, using ViennaGrid Tetgen poly Reader" << std::endl;

    viennagrid::io::tetgen_poly_reader reader;

    try
    {
      typedef viennagrid::brep_3d_mesh MeshType;

      output_parameter_proxy<MeshType> output_mesh = output_proxy<MeshType>("default");
      output_parameter_proxy<point_3d_container> output_hole_points = output_proxy<point_3d_container>("hole_points");
      output_parameter_proxy<seed_point_3d_container> output_seed_points = output_proxy<seed_point_3d_container>("seed_points");

      point_3d_container hole_points;
      seed_point_3d_container seed_points;

      output_mesh().clear();
      reader(output_mesh(), filename, hole_points, seed_points);

      if (!hole_points.empty())
      {
        info(1) << "Found hole points (" << hole_points.size() << ")" << std::endl;
        output_hole_points() = hole_points;
      }
      else
        unset_output("hole_points");


      if (!seed_points.empty())
      {
        info(1) << "Found seed points (" << seed_points.size() << ")" << std::endl;
        output_seed_points() = seed_points;
      }
      else
        unset_output("seed_points");

      return true;
    }
    catch (viennagrid::io::bad_file_format_exception const & ) {}

//         try
//         {
//           typedef viennagrid::brep_2d_mesh MeshType;
//
//           output_parameter_proxy<MeshType> output_mesh = output_proxy<MeshType>("default");
//           output_parameter_proxy<point_2d_container> output_hole_points = output_proxy<point_2d_container>("hole_points");
//           output_parameter_proxy<seed_point_2d_container> output_seed_points = output_proxy<seed_point_2d_container>("seed_points");
//
//
//           point_2d_container hole_points;
//           seed_point_2d_container seed_points;
//
//           reader(output_mesh(), filename, hole_points, seed_points);
//
//
//           if (!hole_points.empty())
//           {
//             info(1) << "Found hole points (" << hole_points.size() << ")" << std::endl;
//             output_hole_points() = hole_points;
//           }
//
//           if (!seed_points.empty())
//           {
//             info(1) << "Found seed points (" << seed_points.size() << ")" << std::endl;
//             output_seed_points() = seed_points;
//           }
//
//           return true;
//         }
//         catch (viennagrid::io::bad_file_format_exception const & ) {}

    return false;
  }
  else if (extension == "deva")
  {
    info(5) << "Found .deva extension, using ViennaMesh GTS deva Reader" << std::endl;
    typedef viennagrid::segmented_mesh<viennagrid::line_2d_mesh, viennagrid::line_2d_segmentation> MeshType;

    output_parameter_proxy<MeshType> output_mesh = output_proxy<MeshType>("default");

    viennamesh::io::gts_deva_geometry_reader reader;
    reader(output_mesh().mesh, output_mesh().segmentation, filename);

    return true;
  }
  else if (extension == "vtu" || extension == "pvd")
  {
    info(5) << "Found .vtu/.pvd extension, using ViennaGrid VTK Reader" << std::endl;


    int geometric_dimension = 0;
    unsigned int vtk_cell_type = 0; // hexahedron=12, tetrahedron=10, quadrilateral=9, triangle=5, line=3

    pugi::xml_document xml;
    if (extension == "vtu")
      xml.load_file( filename.c_str() );
    else
    {
      pugi::xml_document pvd;
      pvd.load_file( filename.c_str() );

      pugi::xpath_node_set nodes = pvd.select_nodes( "/VTKFile/Collection/DataSet" );

      if (!nodes.empty())
      {
//             pugi::xpath_node const & node = nodes.first();
//             pugi::xml_attribute attrib = node.node().attribute("file");
//             std::cout << attrib.value() << std::endl;

        string vtu_file = path + nodes.first().node().attribute("file").value();
        xml.load_file( vtu_file.c_str() );
      }
    }

    {
      pugi::xpath_node_set nodes = xml.select_nodes( "/VTKFile/UnstructuredGrid/Piece/Points/DataArray" );
      if (!nodes.empty())
      {
        geometric_dimension = atoi( nodes.first().node().attribute("NumberOfComponents").value() );
        info(5) << "geometric dimension = " << geometric_dimension << std::endl;
      }
    }

    {
      pugi::xpath_node_set nodes = xml.select_nodes( "/VTKFile/UnstructuredGrid/Piece/Cells/DataArray[@Name='types']" );
      if (!nodes.empty())
      {
        std::string types = nodes.first().node().text().get();
        std::istringstream stream( types );
        stream >> vtk_cell_type;
        info(5) << "VTK cell type = " << vtk_cell_type << " (supported: hexahedron=12, tetrahedron=10, quadrilateral=9, triangle=5, line=3)" << std::endl;
      }
    }

    if (is_mesh_type<viennagrid::line_tag, 1>(vtk_cell_type, geometric_dimension))
      return generic_read_vtk<viennagrid::line_tag, 1>(filename);
    if (is_mesh_type<viennagrid::line_tag, 2>(vtk_cell_type, geometric_dimension))
      return generic_read_vtk<viennagrid::line_tag, 2>(filename);
    if (is_mesh_type<viennagrid::line_tag, 3>(vtk_cell_type, geometric_dimension))
      return generic_read_vtk<viennagrid::line_tag, 3>(filename);

    if (is_mesh_type<viennagrid::triangle_tag, 2>(vtk_cell_type, geometric_dimension))
      return generic_read_vtk<viennagrid::triangle_tag, 2>(filename);
    if (is_mesh_type<viennagrid::triangle_tag, 3>(vtk_cell_type, geometric_dimension))
      return generic_read_vtk<viennagrid::triangle_tag, 3>(filename);

    if (is_mesh_type<viennagrid::quadrilateral_tag, 2>(vtk_cell_type, geometric_dimension))
      return generic_read_vtk<viennagrid::quadrilateral_tag, 2>(filename);
    if (is_mesh_type<viennagrid::quadrilateral_tag, 3>(vtk_cell_type, geometric_dimension))
      return generic_read_vtk<viennagrid::quadrilateral_tag, 3>(filename);

    if (is_mesh_type<viennagrid::tetrahedron_tag, 3>(vtk_cell_type, geometric_dimension))
      return generic_read_vtk<viennagrid::tetrahedron_tag, 3>(filename);

    error(1) << "Combination of geometric and topologic dimension is not supported" << std::endl;
    return false;
  }
  else
  {
    error(1) << "Unsupported extension: " << extension << std::endl;
    return false;
  }

  return false;
}




bool viennamesh::io::mesh_reader::run_impl()
{
  const_string_parameter_handle param = get_required_input<string>("filename");
  return load(param());
}
