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

#include "mesh_writer.hpp"

#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/mphtxt_writer.hpp"

#include "pugixml.hpp"

#include "viennamesh/core.hpp"

namespace viennamesh
{
//   void add_xml_text_child( pugi::xml_node & parent, std::string const & element_name, std::string const & value  )
//   {
//     pugi::xml_node tmp = parent.append_child( element_name.c_str() );
//     tmp.append_child( pugi::node_pcdata ).set_value( value.c_str() );
//   }
//
//   template<typename TypeT>
//   void add_xml_text_child( pugi::xml_node & parent, std::string const & element_name, TypeT const & value  )
//   {
//     add_xml_text_child( parent, element_name, lexical_cast<std::string>(value) );
//   }

//     struct vmesh_writer_proxy
//     {
//       template<typename MeshT>
//       void operator() (MeshT const & mesh,
//                        std::string const & filename,
//                        mesh_writer const & algorithm)
//       {
//         std::string filename_without_extension = filename.substr(0, filename.rfind(".vmesh"));
//
//         vtk_writer_proxy()(mesh, filename, algorithm);
//
//         pugi::xml_document xml;
//
//         pugi::xml_node xml_mesh = xml.append_child("mesh");
//
//         add_xml_text_child(xml_mesh, "file", (filename_without_extension + ".vtu").c_str());
//         add_xml_text_child(xml_mesh, "dimension", typename viennagrid::result_of::point<MeshT>::type().size() );
//         add_xml_text_child(xml_mesh, "vertex_count", viennagrid::vertices(mesh).size() );
//         add_xml_text_child(xml_mesh, "cell_count", viennagrid::cells(mesh).size() );
//         add_xml_text_child(xml_mesh, "segment_count", 0 );
//
//         pugi::xml_node xml_mesh_topology = xml_mesh.append_child("topology");
//         add_xml_text_child(xml_mesh_topology, "celltype", viennagrid::result_of::cell_tag<MeshT>::type::name() );

//         pugi::xml_node xml_mesh_segmentation = xml_mesh.append_child("segmentation");
//         for ( typename SegmentationT::const_iterator sit = segmentation.begin(); sit != segmentation.end(); ++sit )
//         {
//           pugi::xml_node xml_mesh_segment = xml_mesh_segmentation.append_child("segment");
//
//           add_xml_text_child(xml_mesh_segment, "id", sit->id() );
//           // TODO: determine vertex count
//       //           add_xml_text_child(xml_mesh_segment, "vertex_count", viennagrid::vertices(*sit).size());
//           add_xml_text_child(xml_mesh_segment, "cell_count", viennagrid::cells(*sit).size());
//
//           if (seed_points)
//           {
//             for (typename SeedPointContainer::const_iterator spit = seed_points().begin(); spit != seed_points().end(); ++spit)
//             {
//               if (spit->second == sit->id())
//               {
//                 add_xml_text_child(xml_mesh_segment, "seed_point", spit->first );
//               }
//             }
//           }
//         }

//         xml.save_file( (filename_without_extension + ".vmesh").c_str() );
//       }

//       template<typename MeshT, typename SegmentationT>
//       void operator() (viennagrid::segmented_mesh<MeshT, SegmentationT> const & segmented_mesh,
//                        std::string const & filename,
//                        mesh_writer const & algorithm)
//       {
//         typedef typename viennagrid::result_of::point<MeshT>::type PointType;
//         typedef typename viennamesh::result_of::seed_point_container<PointType>::type SeedPointContainer;
//
//         typename result_of::const_parameter_handle<SeedPointContainer>::type spp = algorithm.input_seed_points.get<SeedPointContainer>();
//
//
//         std::string filename_without_extension = filename.substr(0, filename.rfind(".vmesh"));
//
//         vtk_writer_proxy()(segmented_mesh, filename, algorithm);
//
//         pugi::xml_document xml;
//
//         pugi::xml_node xml_mesh = xml.append_child("mesh");
//
//         add_xml_text_child(xml_mesh, "file", (filename_without_extension + (segmented_mesh.segmentation.size() > 1 ? "_main.pvd" : ".vtu")).c_str() );
//         add_xml_text_child(xml_mesh, "dimension", typename viennagrid::result_of::point<MeshT>::type().size() );
//         add_xml_text_child(xml_mesh, "vertex_count", viennagrid::vertices(segmented_mesh.mesh).size() );
//         add_xml_text_child(xml_mesh, "cell_count", viennagrid::cells(segmented_mesh.mesh).size() );
//         add_xml_text_child(xml_mesh, "segment_count", segmented_mesh.segmentation.size() );
//
//         pugi::xml_node xml_mesh_topology = xml_mesh.append_child("topology");
//         add_xml_text_child(xml_mesh_topology, "celltype", viennagrid::result_of::cell_tag<MeshT>::type::name() );
//
//         pugi::xml_node xml_mesh_segmentation = xml_mesh.append_child("segmentation");
//         for ( typename SegmentationT::const_iterator sit = segmented_mesh.segmentation.begin(); sit != segmented_mesh.segmentation.end(); ++sit )
//         {
//           pugi::xml_node xml_mesh_segment = xml_mesh_segmentation.append_child("segment");
//
//           add_xml_text_child(xml_mesh_segment, "id", sit->id() );
//           // TODO: determine vertex count
//       //           add_xml_text_child(xml_mesh_segment, "vertex_count", viennagrid::vertices(*sit).size());
//           add_xml_text_child(xml_mesh_segment, "cell_count", viennagrid::cells(*sit).size());
//
//           if (spp)
//           {
//             for (typename SeedPointContainer::const_iterator spit = spp().begin(); spit != spp().end(); ++spit)
//             {
//               if (spit->second == sit->id())
//               {
//                 add_xml_text_child(xml_mesh_segment, "seed_point", spit->first );
//               }
//             }
//           }
//         }
//
//         xml.save_file( (filename_without_extension + ".vmesh").c_str() );
//       }
//
//
//
//
//
//       template<typename MeshT, typename SegmentationT, typename SegmentIDT, typename VertexKeyT, typename CellKeyT, typename ValueT>
//       void operator() (viennagrid::segmented_mesh<MeshT, SegmentationT> const & segmented_mesh,
//                        viennamesh::segmented_mesh_quantities<SegmentIDT, VertexKeyT, CellKeyT, ValueT> const & quantities,
//                        std::string const & filename,
//                        mesh_writer const & algorithm)
//       {
//         typedef typename viennagrid::result_of::point<MeshT>::type PointType;
//         typedef typename viennamesh::result_of::seed_point_container<PointType>::type SeedPointContainer;
//
//         typename result_of::const_parameter_handle<SeedPointContainer>::type spp = algorithm.input_seed_points.get<SeedPointContainer>();
//
//
//         std::string filename_without_extension = filename.substr(0, filename.rfind(".vmesh"));
//
//         vtk_writer_proxy()(segmented_mesh, quantities, filename, algorithm);
//
//         pugi::xml_document xml;
//
//         pugi::xml_node xml_mesh = xml.append_child("mesh");
//
//         add_xml_text_child(xml_mesh, "file", (filename_without_extension + (segmented_mesh.segmentation.size() > 1 ? "_main.pvd" : ".vtu")).c_str() );
//         add_xml_text_child(xml_mesh, "dimension", typename viennagrid::result_of::point<MeshT>::type().size() );
//         add_xml_text_child(xml_mesh, "vertex_count", viennagrid::vertices(segmented_mesh.mesh).size() );
//         add_xml_text_child(xml_mesh, "cell_count", viennagrid::cells(segmented_mesh.mesh).size() );
//         add_xml_text_child(xml_mesh, "segment_count", segmented_mesh.segmentation.size() );
//
//         pugi::xml_node xml_mesh_topology = xml_mesh.append_child("topology");
//         add_xml_text_child(xml_mesh_topology, "celltype", viennagrid::result_of::cell_tag<MeshT>::type::name() );
//
//         pugi::xml_node xml_mesh_segmentation = xml_mesh.append_child("segmentation");
//         for ( typename SegmentationT::const_iterator sit = segmented_mesh.segmentation.begin(); sit != segmented_mesh.segmentation.end(); ++sit )
//         {
//           pugi::xml_node xml_mesh_segment = xml_mesh_segmentation.append_child("segment");
//
//           add_xml_text_child(xml_mesh_segment, "id", sit->id() );
//           // TODO: determine vertex count
//       //           add_xml_text_child(xml_mesh_segment, "vertex_count", viennagrid::vertices(*sit).size());
//           add_xml_text_child(xml_mesh_segment, "cell_count", viennagrid::cells(*sit).size());
//
//           if (spp)
//           {
//             for (typename SeedPointContainer::const_iterator spit = spp().begin(); spit != spp().end(); ++spit)
//             {
//               if (spit->second == sit->id())
//               {
//                 add_xml_text_child(xml_mesh_segment, "seed_point", spit->first );
//               }
//             }
//           }
//         }
//
//         xml.save_file( (filename_without_extension + ".vmesh").c_str() );
//       }
//     };



  mesh_writer::mesh_writer() {}
  std::string mesh_writer::name() { return "mesh_writer"; }


  bool mesh_writer::run(viennamesh::algorithm_handle &)
  {
    string_handle filename = get_required_input<string_handle>("filename");
    string_handle filetype = get_input<string_handle>("filetype");
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");

    info(1) << "Number of vertices of mesh to write " << viennagrid::vertices(input_mesh()).size() << std::endl;
    info(1) << "Number of cells of mesh to write " << viennagrid::cells(input_mesh()).size() << std::endl;

    if (!filename.valid())
    {
      error(1) << "Input parameter \"filename\" not found" << std::endl;
      return false;
    }

    if (!input_mesh.valid())
    {
      error(1) << "Input parameter \"mesh\" not found" << std::endl;
      return false;
    }

    info(1) << "Writing mesh to file \"" << filename() << "\"" << std::endl;

    FileType ft;
    if (filetype.valid())
      ft = from_string( filetype() );
    else
      ft = from_filename( filename() );

    info(1) << "Using file type " << to_string(ft) << std::endl;

    viennagrid_dimension geometric_dimension = viennagrid::geometric_dimension( input_mesh() );
    viennagrid_dimension cell_dimension = viennagrid::topologic_dimension( input_mesh() );

    info(1) << "Found geometric dimension: " << geometric_dimension << std::endl;
    info(1) << "Found cell dimension: " << cell_dimension << std::endl;

    try
    {
      switch (ft)
      {
        case VTK:
        {
          viennagrid::io::vtk_writer<viennagrid::mesh_t> writer;

          quantity_field_handle quantity_field = get_input<viennagrid::quantity_field>("quantities");
          if (quantity_field.valid())
          {
            int quantity_field_count = quantity_field.size();
            for (int i = 0; i != quantity_field_count; ++i)
            {
              viennagrid::quantity_field current = quantity_field(i);

              info(1) << "Found quantity field \"" << current.name() << "\" with cell dimension " << current.topologic_dimension() << " and values dimension " << current.values_dimension() << std::endl;

              if ( (current.topologic_dimension() != 0) && (current.topologic_dimension() != cell_dimension) )
              {
                error(1) << "Values dimension " << current.values_dimension() << " for quantitiy field \"" << current.name() << "\" not supported -> skipping" << std::endl;
                continue;
              }

              if ( (current.values_dimension() != 1) && (current.values_dimension() != 3) )
              {
                error(1) << "Values dimension " << current.values_dimension() << " for quantitiy field \"" << current.name() << "\" not supported -> skipping" << std::endl;
                continue;
              }


              if ( current.topologic_dimension() == 0 )
              {
                if ( current.values_dimension() == 1 )
                {
                  writer.add_scalar_data_on_vertices( current, current.name() );
                }
                else
                {
                  writer.add_vector_data_on_vertices( current, current.name() );
                }
              }
              else if ( current.topologic_dimension() == cell_dimension )
              {
                if ( current.values_dimension() == 1 )
                {
                  writer.add_scalar_data_on_cells( current, current.name() );
                }
                else
                {
                  writer.add_vector_data_on_cells( current, current.name() );
                }
              }

            }
          }

          std::string fn = filename().substr(0, filename().rfind("."));
          writer( input_mesh(), fn );
          return true;
        }
//         case VMESH:
//         {
//           return write_all<vmesh_writer_proxy>( mesh, filename(), geometric_dimension, cell_type, is_segmented );
//         }
        case COMSOL_MPHTXT:
        {
          if ( geometric_dimension != 3 || cell_dimension != 3)
          {
            error(1) << "MPHTXT writer not supported for geometric dimension " << geometric_dimension << " and cell dimension " << cell_dimension << std::endl;
            return false;
          }

          viennagrid::io::mphtxt_writer writer;
          writer(input_mesh(), filename());
          return true;
        }
        default:
          return false;
      }
    }
    catch (std::exception const & ex)
    {
      error(1) << "File reading failed" << std::endl;
      error(1) << ex.what() << std::endl;
      return false;
    }
  }
}
