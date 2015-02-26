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

#include "mesh_reader.hpp"

#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/tetgen_poly_reader.hpp"
// #include "viennagrid/io/bnd_reader.hpp"
// #include "viennagrid/io/neper_tess_reader.hpp"
#include "viennagrid/io/stl_reader.hpp"

// #include "viennamesh/algorithm/io/gts_deva_geometry_reader.hpp"
// #include "viennamesh/algorithm/io/silvaco_str_reader.hpp"

// #include "viennamesh/core/mesh_quantities.hpp"
// #include "viennamesh/utils/pugixml/pugixml.hpp"

#include "viennamesh/core.hpp"

namespace viennamesh
{
  mesh_reader::mesh_reader() {}
  std::string mesh_reader::name() { return "mesh_reader"; }


//   template<int GeometricDimensionV>
//   bool mesh_reader::read_seed_points( pugi::xml_document const & xml )
//   {
//     typedef typename viennamesh::result_of::point<GeometricDimensionV>::type PointType;
//     typedef typename viennamesh::result_of::seed_point_container<PointType>::type SeedPointContainerType;
//
//     output_parameter_proxy<SeedPointContainerType> ospp(output_seed_points);
//
//     pugi::xpath_node_set xml_segments = xml.select_nodes( "/mesh/segment" );
//     for (pugi::xpath_node_set::const_iterator xsit = xml_segments.begin(); xsit != xml_segments.end(); ++xsit)
//     {
//       int segment_id;
//       pugi::xpath_node_set xml_segment_id = xsit->node().select_nodes( "/id" );
//       if (!xml_segment_id.empty())
//         segment_id = atoi( xml_segment_id.first().node().text().get() );
//       else
//         continue;
//
//       pugi::xpath_node_set xml_seed_points = xsit->node().select_nodes( "/seed_point" );
//       for (pugi::xpath_node_set::const_iterator xspit = xml_seed_points.begin(); xspit != xml_seed_points.end(); ++xspit)
//       {
//         std::stringstream ss( xspit->node().text().get() );
//         PointType point;
//         for (std::size_t i = 0; i < point.size(); ++i)
//           ss >> point[i];
//
//         info(5) << "Found seed point for segment " << segment_id << ": " << point << std::endl;
//
//         ospp().push_back( std::make_pair(point, segment_id) );
//       }
//     }
//
//     return true;
//   }




//   bool mesh_reader::read_vmesh( std::string const & filename )
//   {
//     int geometric_dimension = -1;
//     std::string mesh_filename;
//
//     pugi::xml_document xml;
//     xml.load_file( filename.c_str() );
//
//     {
//       pugi::xpath_node_set nodes = xml.select_nodes( "/mesh/dimension" );
//       if (!nodes.empty())
//       {
//         geometric_dimension = atoi( nodes.first().node().text().get() );
//         info(5) << "geometric dimension = " << geometric_dimension << std::endl;
//       }
//     }
//
//
//     {
//       pugi::xpath_node_set nodes = xml.select_nodes( "/mesh/file" );
//       if (!nodes.empty())
//       {
//         mesh_filename = nodes.first().node().text().get();
//         info(5) << "vmesh inner mesh filename = " << mesh_filename << std::endl;
//       }
//     }
//
//     FileType file_type = from_filename( mesh_filename );
//
//     load(mesh_filename, file_type);
//
//     if (geometric_dimension == 1)
//       read_seed_points<1>(xml);
//     else if (geometric_dimension == 2)
//       read_seed_points<2>(xml);
//     else if (geometric_dimension == 3)
//       read_seed_points<3>(xml);
//
//     return true;
//   }



  bool mesh_reader::load( std::string const & filename, FileType filetype )
  {
    std::string path = stringtools::extract_path( filename );

    info(1) << "Reading mesh from file \"" << filename << "\"" << std::endl;

    mesh_handle output_mesh = make_data<mesh_handle>();
    bool success = false;

    switch (filetype)
    {
//     case SYNOPSIS_BND:
//       {
//         info(5) << "Found .bnd extension, using ViennaGrid BND Reader" << std::endl;
//         typedef viennagrid::mesh_t MeshType;
//
//         output_parameter_proxy<MeshType> omp(output_mesh);
//
//         viennagrid::io::bnd_reader reader;
//         reader(omp().mesh, omp().segmentation, filename);
//
//         return true;
//       }
    case NETGEN_MESH:
      {
        info(5) << "Found .mesh extension, using ViennaGrid Netgen Reader" << std::endl;

        viennagrid::io::netgen_reader reader;
        reader(output_mesh(), filename);

        success = true;
        break;
      }
    case TETGEN_POLY:
      {
        info(5) << "Found .poly extension, using ViennaGrid Tetgen poly Reader" << std::endl;

        PointContainerType tmp_hole_points;
        SeedPointContainerType tmp_seed_points;

        viennagrid::io::tetgen_poly_reader reader;
        reader(output_mesh(), filename, tmp_hole_points, tmp_seed_points);

//         viennagrid_int point_dim = viennagrid::geometric_dimension(output_mesh());

        if (!tmp_hole_points.empty())
        {
          point_container_handle hole_points = make_data<point_container_handle>();
          convert(tmp_hole_points, hole_points());
          set_output("hole_points", hole_points);
        }

        if (!tmp_seed_points.empty())
        {
          seed_point_container_handle seed_points = make_data<seed_point_container_handle>();
          convert(tmp_seed_points, seed_points());
          set_output("seed_points", seed_points);
        }

        success = true;
        break;
      }


//     case NEPER_TESS:
//       {
//         info(5) << "Found .tess extension, using ViennaGrid Neper tess Reader" << std::endl;
//         typedef viennagrid::brep_3d_mesh MeshType;
//
//         output_parameter_proxy<MeshType> omp(output_mesh);
//         output_parameter_proxy<seed_point_3d_container> ospp(output_seed_points);
//
//         seed_point_3d_container seed_points;
//
//         viennagrid::io::neper_tess_reader reader;
//         reader(omp(), filename, seed_points);
//
//         if (!seed_points.empty())
//         {
//           info(1) << "Found seed points (" << seed_points.size() << ")" << std::endl;
//           ospp() = seed_points;
//         }
//         else
//           unset_output("seed_points");
//
//
//         return true;
//       }

    case STL:
    case STL_ASCII:
    case STL_BINARY:
      {
        info(5) << "Found .stl extension, using ViennaGrid STL Reader" << std::endl;

        data_handle<double> vertex_tolerance = get_input<double>("vertex_tolerance");

        viennagrid::io::stl_reader<> reader;
        if (vertex_tolerance)
          reader = viennagrid::io::stl_reader<>( vertex_tolerance() );

        if (filetype == STL)
          reader(output_mesh(), filename);
        else if (filetype == STL_ASCII)
          reader.read_ascii(output_mesh(), filename);
        else if (filetype == STL_BINARY)
          reader.read_binary(output_mesh(), filename);

        success = true;
        break;
      }

//     case SILVACO_STR:
//       {
//         info(5) << "Found .str extension, using ViennaGrid Silvaco str Reader" << std::endl;
//         typedef viennagrid::triangular_3d_mesh MeshType;
//
//         output_parameter_proxy<MeshType> omp(output_mesh);
//
//         viennagrid::io::silvaco_str_reader reader;
//         reader(omp(), filename);
//
//         return true;
//       }

//     case GTS_DEVA:
//       {
//         info(5) << "Found .deva extension, using ViennaMesh GTS deva Reader" << std::endl;
//         typedef viennagrid::segmented_mesh<viennagrid::line_2d_mesh, viennagrid::line_2d_segmentation> MeshType;
//
//         output_parameter_proxy<MeshType> omp(output_mesh);
//
//         gts_deva_geometry_reader reader;
//         reader(omp().mesh, omp().segmentation, filename);
//
//         return true;
//       }

    case VTK:
      {
        info(5) << "Found .vtu/.pvd extension, using ViennaGrid VTK Reader" << std::endl;

        viennagrid::io::vtk_reader<viennagrid::mesh_t> reader;

        data_handle<bool> use_local_points = get_input<bool>("use_local_points");
        if (use_local_points)
          reader.set_use_local_points( use_local_points() );

        reader(output_mesh(), filename);


        std::vector<viennagrid::quantity_field> quantity_fields;

        for (auto quantity_name : reader.scalar_vertex_data_names())
          quantity_fields.push_back( reader.scalar_vertex_quantity_field(quantity_name) );

        for (auto quantity_name : reader.vector_vertex_data_names())
          quantity_fields.push_back( reader.vector_vertex_quantity_field(quantity_name) );

        for (auto quantity_name : reader.scalar_cell_data_names())
          quantity_fields.push_back( reader.scalar_cell_quantity_field(quantity_name) );

        for (auto quantity_name : reader.vector_cell_data_names())
          quantity_fields.push_back( reader.vector_cell_quantity_field(quantity_name) );


        for (auto it = quantity_fields.begin(); it != quantity_fields.end(); )
        {
          if ( !(*it).is_valid() )
            it = quantity_fields.erase(it);

          info(1) << "Found quantity for topologic dimension " << (*it).topologic_dimension() <<
            " with name \"" << (*it).name() << "\"" << std::endl;

//           for (int i = 0; i != (*it).size(); ++i)
//             std::cout << "  " << (*it).get(i) << std::endl;

          ++it;
        }

        set_output_vector( "quantities", quantity_fields );



        success = true;
        break;
      }
    default:
      {
        error(1) << "Unsupported extension: " << to_string(filetype) << std::endl;
        return false;
      }
    }

    if (success)
    {
      info(1) << "Successfully read a mesh with " << viennagrid::vertices(output_mesh()).size() << " vertices and " <<
        viennagrid::cells(output_mesh()).size() << " cells" << std::endl;

      set_output("mesh", output_mesh);
    }

    return success;
  }




  bool mesh_reader::run(viennamesh::algorithm_handle &)
  {
    string_handle filename = get_required_input<string_handle>("filename");
    string_handle filetype = get_input<string_handle>("filetype");

    FileType ft;
    if (filetype)
      ft = from_string( filetype() );
    else
      ft = from_filename( filename() );
    info(1) << "Using file type " << to_string(ft) << std::endl;

    std::string path = base_path();

    if (!path.empty())
    {
      info(1) << "Using base path: " << path << std::endl;
      return load( path + "/" + filename(), ft);
    }
    else
      return load(filename(), ft);
  }

}
