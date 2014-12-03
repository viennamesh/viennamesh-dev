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

#include "viennamesh/utils/pugixml/pugixml.hpp"

#include "viennamesh/core.hpp"

namespace viennamesh
{
  struct vtk_writer_proxy
  {
    template<typename MeshT>
    void operator() (MeshT const & mesh,
                      std::string const & filename,
                      mesh_writer const &)
    {
      viennagrid::io::vtk_writer<MeshT> writer;
      writer(mesh, filename);
    }

//       template<typename MeshT, typename SegmentationT>
//       void operator() (viennagrid::segmented_mesh<MeshT, SegmentationT> const & segmented_mesh,
//                        std::string const & filename,
//                        mesh_writer const &)
//       {
//         viennagrid::io::vtk_writer<MeshT, SegmentationT> writer;
//         writer(segmented_mesh.mesh, segmented_mesh.segmentation, filename);
//       }
//
//       template<typename MeshT, typename SegmentationT, typename SegmentIDT, typename VertexKeyT, typename CellKeyT, typename ValueT>
//       void operator() (viennagrid::segmented_mesh<MeshT, SegmentationT> const & segmented_mesh,
//                        viennamesh::segmented_mesh_quantities<SegmentIDT, VertexKeyT, CellKeyT, ValueT> const & quantities,
//                        std::string const & filename,
//                        mesh_writer const &)
//       {
//         viennagrid::io::vtk_writer<MeshT, SegmentationT> writer;
//         quantities.template toWriter<MeshT>(writer);
//         writer(segmented_mesh.mesh, segmented_mesh.segmentation, filename);
//       }
  };





  void add_xml_text_child( pugi::xml_node & parent, std::string const & element_name, std::string const & value  )
  {
    pugi::xml_node tmp = parent.append_child( element_name.c_str() );
    tmp.append_child( pugi::node_pcdata ).set_value( value.c_str() );
  }

  template<typename TypeT>
  void add_xml_text_child( pugi::xml_node & parent, std::string const & element_name, TypeT const & value  )
  {
    add_xml_text_child( parent, element_name, lexical_cast<std::string>(value) );
  }

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





  struct mphtxt_writer_proxy
  {
    template<typename MeshT>
    void operator() (MeshT const & mesh,
                      std::string const & filename,
                      mesh_writer const &)
    {
      viennagrid::io::mphtxt_writer writer;
      writer(mesh, filename);
    }

//       template<typename MeshT, typename SegmentationT>
//       void operator() (viennagrid::segmented_mesh<MeshT, SegmentationT> const & segmented_mesh,
//                        std::string const & filename,
//                        mesh_writer const &)
//       {
//         viennagrid::io::mphtxt_writer writer;
//         writer(segmented_mesh.mesh, segmented_mesh.segmentation, filename);
//       }
//
//       template<typename MeshT, typename SegmentationT, typename SegmentIDT, typename VertexKeyT, typename CellKeyT, typename ValueT>
//       void operator() (viennagrid::segmented_mesh<MeshT, SegmentationT> const & segmented_mesh,
//                        viennamesh::segmented_mesh_quantities<SegmentIDT, VertexKeyT, CellKeyT, ValueT> const &,
//                        std::string const & filename,
//                        mesh_writer const &)
//       {
//         viennagrid::io::mphtxt_writer writer;
//         writer(segmented_mesh.mesh, segmented_mesh.segmentation, filename);
//       }
  };






  mesh_writer::mesh_writer() {}

  std::string mesh_writer::name() { return "mesh_writer"; }


  template<typename WriterProxyT>
  bool mesh_writer::write_all( std::string const & filename_ )
  {
    std::string filename = filename_.substr(0, filename_.rfind("."));

    MeshHandleType mesh_handle = get_input_mesh("mesh");
    if (mesh_handle)
    {
      MeshType mesh( mesh_handle() );
      WriterProxyT()(mesh, filename, *this);
      return true;
    }
    else
      return false;
  }

  bool mesh_writer::write_mphtxt( std::string const & filename )
  {
    MeshHandleType mesh_handle = get_input_mesh("mesh");
    if (mesh_handle)
    {
      MeshType mesh( mesh_handle() );

      if ( viennagrid::geometric_dimension(mesh) != 3 || viennagrid::cell_dimension(mesh) != 3)
        return false;

      mphtxt_writer_proxy()(mesh, filename, *this);
      return true;
    }
    else
      return false;
  }


  bool mesh_writer::run(viennamesh::algorithm_handle &)
  {
    data_handle<viennamesh_string> filename = get_input<viennamesh_string>("filename");
    data_handle<viennamesh_string> filetype = get_input<viennamesh_string>("filetype");

//     const_parameter_handle mesh = input_mesh.get();
    info(1) << "Writing mesh to file \"" << filename() << "\"" << std::endl;

    FileType ft;
    if (filetype.valid())
      ft = from_string( filetype() );
    else
      ft = from_filename( filename() );

    info(1) << "Using file type " << to_string(ft) << std::endl;

//     int geometric_dimension = lexical_cast<int>(mesh->get_property("geometric_dimension").first);
//     std::string cell_type = mesh->get_property("cell_type").first;
//     bool is_segmented = lexical_cast< stringtools::boolalpha_bool >(mesh->get_property("is_segmented").first);


//     info(1) << "Found geometric dimension: " << geometric_dimension << std::endl;
//     info(1) << "Found cell type: " << cell_type << std::endl;
//     info(1) << "Is segmented: " << std::boolalpha << is_segmented << std::endl;


    try
    {
      switch (ft)
      {
        case VTK:
          return write_all<vtk_writer_proxy>( filename() );
//         case VMESH:
//           return write_all<vmesh_writer_proxy>( mesh, filename(), geometric_dimension, cell_type, is_segmented );
        case COMSOL_MPHTXT:
          return write_mphtxt( filename() );
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
