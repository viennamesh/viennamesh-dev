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
// #include "viennagrid/io/stl_reader.hpp"

// #include "viennamesh/algorithm/io/gts_deva_geometry_reader.hpp"
// #include "viennamesh/algorithm/io/silvaco_str_reader.hpp"
// #ifdef VIENNAMESH_WITH_TDR
//   #include "viennamesh/algorithm/io/sentaurus_tdr_reader.hpp"
// #endif

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
        MeshHandleType mesh_handle = make_mesh();
        MeshType mesh( mesh_handle() );

        viennagrid::io::netgen_reader reader;
        reader(mesh, filename);
        set_output("mesh", mesh_handle);

        return true;
      }
    case TETGEN_POLY:
      {
        info(5) << "Found .poly extension, using ViennaGrid Tetgen poly Reader" << std::endl;

        MeshHandleType mesh_handle = make_mesh();
        MeshType mesh( mesh_handle() );

        viennagrid::io::tetgen_poly_reader reader;
        reader(mesh, filename);
        set_output("mesh", mesh_handle);

        return true;
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

//     case STL:
//     case STL_ASCII:
//     case STL_BINARY:
//       {
//         info(5) << "Found .stl extension, using ViennaGrid STL Reader" << std::endl;
//         typedef viennagrid::triangular_3d_mesh MeshType;
//
//         output_parameter_proxy<MeshType> omp(output_mesh);
//
//         viennagrid::io::stl_reader reader;
//         if (filetype == STL)
//           reader(omp(), filename);
//         else if (filetype == STL_ASCII)
//           reader.read_ascii(omp(), filename);
//         else if (filetype == STL_BINARY)
//           reader.read_binary(omp(), filename);
//
//         return true;
//       }

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

// #ifdef VIENNAMESH_WITH_TDR
//     case SENTAURUS_TDR:
//       {
//         info(5) << "Found .tdr extension, using Sentaurus TDR Reader" << std::endl;
//
//         shared_ptr<H5File> file( new H5File(filename.c_str(), H5F_ACC_RDWR) );
//
//         if (file->getNumObjs()!=1)
//         {
//           error(1) << "File has not only collection" << std::endl;
//           return false;
//         }
//
//         tdr_geometry geometry;
//         geometry.read_collection(file->openGroup("collection"));
//
//         geometry.correct_vertices();
//
//         if (geometry.dim == 2)
//         {
//           typedef viennagrid::triangular_2d_mesh MeshType;
//           typedef viennagrid::triangular_2d_segmentation SegmentationType;
//           typedef viennagrid::segmented_mesh<MeshType, SegmentationType> SegmentedMeshType;
//
//           output_parameter_proxy<SegmentedMeshType> omp(output_mesh);
//           geometry.to_viennagrid( omp().mesh, omp().segmentation );
//
//           typedef viennamesh::result_of::segmented_mesh_quantities<MeshType, SegmentationType>::type SegmentedMeshQuantitiesType;
//           output_parameter_proxy<SegmentedMeshQuantitiesType> oqp(output_quantities);
//           geometry.to_mesh_quantities( omp().mesh, omp().segmentation, oqp() );
//
//           return true;
//         }
//         else if (geometry.dim == 3)
//         {
//           typedef viennagrid::tetrahedral_3d_mesh MeshType;
//           typedef viennagrid::tetrahedral_3d_segmentation SegmentationType;
//           typedef viennagrid::segmented_mesh<MeshType, SegmentationType> SegmentedMeshType;
//
//           output_parameter_proxy<SegmentedMeshType> omp(output_mesh);
//           geometry.to_viennagrid( omp().mesh, omp().segmentation );
//
//           typedef viennamesh::result_of::segmented_mesh_quantities<MeshType, SegmentationType>::type SegmentedMeshQuantitiesType;
//           output_parameter_proxy<SegmentedMeshQuantitiesType> oqp(output_quantities);
//           geometry.to_mesh_quantities( omp().mesh, omp().segmentation, oqp() );
//
//           return true;
//         }
//
//         error(1) << "Dimension not supported" << std::endl;
//
//         return false;
//       }
// #endif

    case VTK:
      {
        info(5) << "Found .vtu/.pvd extension, using ViennaGrid VTK Reader" << std::endl;

        MeshHandleType mesh_handle = make_mesh();
        MeshType mesh( mesh_handle() );

        viennagrid::io::vtk_reader<MeshType> reader;
        reader(mesh, filename);
        set_output("mesh", mesh_handle);

        return true;
      }
    default:
      {
        error(1) << "Unsupported extension: " << to_string(filetype) << std::endl;
        return false;
      }
    }

    return false;
  }




  bool mesh_reader::run(viennamesh::algorithm_handle &)
  {
    data_handle<char*> filename = get_input<char*>("filename");
    data_handle<char*> filetype = get_input<char*>("filetype");
    data_handle<char*> base_path = get_input<char*>("base_path");

    FileType ft;
    if (filetype)
      ft = from_string( filetype() );
    else
      ft = from_filename( filename() );
    info(1) << "Using file type " << to_string(ft) << std::endl;

    std::string path;
    if (base_path)
      path = base_path();

    if (!path.empty())
    {
      info(1) << "Using base path: " << path << std::endl;
      return load( path + "/" + filename(), ft);
    }
    else
      return load(filename(), ft);
  }

}
