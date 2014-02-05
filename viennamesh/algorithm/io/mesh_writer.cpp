#include "viennamesh/algorithm/io/mesh_writer.hpp"

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/mphtxt_writer.hpp"

#include "pugixml/pugixml.hpp"
#include "common.hpp"


namespace viennamesh
{
  namespace io
  {


    struct vtk_writer_proxy
    {
      template<typename MeshT>
      bool operator() (MeshT const & mesh,
                       string const & filename,
                       algorithm_handle const & algorithm)
      {
        viennagrid::io::vtk_writer<MeshT> writer;
        return (writer(mesh, filename) == EXIT_SUCCESS);
      }

      template<typename MeshT, typename SegmentationT>
      bool operator() (viennagrid::segmented_mesh<MeshT, SegmentationT> const & segmented_mesh,
                       string const & filename,
                       algorithm_handle const & algorithm)
      {
        viennagrid::io::vtk_writer<MeshT, SegmentationT> writer;
        return (writer(segmented_mesh.mesh, segmented_mesh.segmentation, filename) == EXIT_SUCCESS);
      }
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

    struct vmesh_writer_proxy
    {
      template<typename MeshT>
      bool operator() (MeshT const & mesh,
                       string const & filename,
                       algorithm_handle const & algorithm)
      {
        typedef typename viennagrid::result_of::point<MeshT>::type PointType;
        typedef typename viennamesh::result_of::seed_point_container<PointType>::type SeedPointContainer;

//         typename result_of::const_parameter_handle<SeedPointContainer>::type seed_points = algorithm->get_input<SeedPointContainer>("seed_points");


        string filename_without_extension = filename.substr(0, filename.rfind(".vmesh"));

        if (!vtk_writer_proxy()(mesh, filename, algorithm))
          return false;

        pugi::xml_document xml;

        pugi::xml_node xml_mesh = xml.append_child("mesh");

        add_xml_text_child(xml_mesh, "file", (filename_without_extension + ".vtu").c_str());
        add_xml_text_child(xml_mesh, "dimension", typename viennagrid::result_of::point<MeshT>::type().size() );
        add_xml_text_child(xml_mesh, "vertex_count", viennagrid::vertices(mesh).size() );
        add_xml_text_child(xml_mesh, "cell_count", viennagrid::cells(mesh).size() );
        add_xml_text_child(xml_mesh, "segment_count", 0 );

        pugi::xml_node xml_mesh_topology = xml_mesh.append_child("topology");
        add_xml_text_child(xml_mesh_topology, "celltype", viennagrid::result_of::cell_tag<MeshT>::type::name() );

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

        xml.save_file( (filename_without_extension + ".vmesh").c_str() );

        return true;
      }

      template<typename MeshT, typename SegmentationT>
      bool operator() (viennagrid::segmented_mesh<MeshT, SegmentationT> const & segmented_mesh,
                       string const & filename,
                       algorithm_handle const & algorithm)
      {
        typedef typename viennagrid::result_of::point<MeshT>::type PointType;
        typedef typename viennamesh::result_of::seed_point_container<PointType>::type SeedPointContainer;

        typename result_of::const_parameter_handle<SeedPointContainer>::type seed_points = algorithm->get_input<SeedPointContainer>("seed_points");


        string filename_without_extension = filename.substr(0, filename.rfind(".vmesh"));

        if (!vtk_writer_proxy()(segmented_mesh, filename, algorithm))
          return false;

        pugi::xml_document xml;

        pugi::xml_node xml_mesh = xml.append_child("mesh");

        add_xml_text_child(xml_mesh, "file", (filename_without_extension + (segmented_mesh.segmentation.size() > 1 ? "_main.pvd" : ".vtu")).c_str() );
        add_xml_text_child(xml_mesh, "dimension", typename viennagrid::result_of::point<MeshT>::type().size() );
        add_xml_text_child(xml_mesh, "vertex_count", viennagrid::vertices(segmented_mesh.mesh).size() );
        add_xml_text_child(xml_mesh, "cell_count", viennagrid::cells(segmented_mesh.mesh).size() );
        add_xml_text_child(xml_mesh, "segment_count", segmented_mesh.segmentation.size() );

        pugi::xml_node xml_mesh_topology = xml_mesh.append_child("topology");
        add_xml_text_child(xml_mesh_topology, "celltype", viennagrid::result_of::cell_tag<MeshT>::type::name() );

        pugi::xml_node xml_mesh_segmentation = xml_mesh.append_child("segmentation");
        for ( typename SegmentationT::const_iterator sit = segmented_mesh.segmentation.begin(); sit != segmented_mesh.segmentation.end(); ++sit )
        {
          pugi::xml_node xml_mesh_segment = xml_mesh_segmentation.append_child("segment");

          add_xml_text_child(xml_mesh_segment, "id", sit->id() );
          // TODO: determine vertex count
      //           add_xml_text_child(xml_mesh_segment, "vertex_count", viennagrid::vertices(*sit).size());
          add_xml_text_child(xml_mesh_segment, "cell_count", viennagrid::cells(*sit).size());

          if (seed_points)
          {
            for (typename SeedPointContainer::const_iterator spit = seed_points().begin(); spit != seed_points().end(); ++spit)
            {
              if (spit->second == sit->id())
              {
                add_xml_text_child(xml_mesh_segment, "seed_point", spit->first );
              }
            }
          }
        }

        xml.save_file( (filename_without_extension + ".vmesh").c_str() );

        return true;
      }
    };















    struct mphtxt_writer_proxy
    {
      template<typename MeshT>
      bool operator() (MeshT const & mesh,
                       string const & filename,
                       algorithm_handle const & algorithm)
      {
        viennagrid::io::mphtxt_writer writer;
        return (writer(mesh, filename) == EXIT_SUCCESS);
      }

      template<typename MeshT, typename SegmentationT>
      bool operator() (viennagrid::segmented_mesh<MeshT, SegmentationT> const & segmented_mesh,
                       string const & filename,
                       algorithm_handle const & algorithm)
      {
        viennagrid::io::mphtxt_writer writer;
        return (writer(segmented_mesh.mesh, segmented_mesh.segmentation, filename) == EXIT_SUCCESS);
      }
    };













//     template<typename MeshT, typename SegmentationT>
//     void mesh_writer::write_vmesh( MeshT const & mesh,  SegmentationT const & segmentation, string const & filename )
//     {
//       typedef typename viennagrid::result_of::point<MeshT>::type PointType;
//       typedef typename viennamesh::result_of::seed_point_container<PointType>::type SeedPointContainer;
//
//       typename result_of::const_parameter_handle<SeedPointContainer>::type seed_points = get_input<SeedPointContainer>("seed_points");
//
//
//       string filename_without_extension = filename.substr(0, filename.rfind(".vmesh"));
//
//       viennagrid::io::vtk_writer<MeshT, SegmentationT> vtk_writer;
//       vtk_writer( mesh, segmentation, filename_without_extension );
//
//       pugi::xml_document xml;
//
//       pugi::xml_node xml_mesh = xml.append_child("mesh");
//
//       add_xml_text_child(xml_mesh, "file", (filename_without_extension + (segmentation.size() > 1 ? "_main.pvd" : ".vtu")).c_str() );
//       add_xml_text_child(xml_mesh, "dimension", typename viennagrid::result_of::point<MeshT>::type().size() );
//       add_xml_text_child(xml_mesh, "vertex_count", viennagrid::vertices(mesh).size() );
//       add_xml_text_child(xml_mesh, "cell_count", viennagrid::cells(mesh).size() );
//       add_xml_text_child(xml_mesh, "segment_count", segmentation.size() );
//
//       pugi::xml_node xml_mesh_topology = xml_mesh.append_child("topology");
//       add_xml_text_child(xml_mesh_topology, "celltype", viennagrid::result_of::cell_tag<MeshT>::type::name() );
//
//       pugi::xml_node xml_mesh_segmentation = xml_mesh.append_child("segmentation");
//       for ( typename SegmentationT::const_iterator sit = segmentation.begin(); sit != segmentation.end(); ++sit )
//       {
//         pugi::xml_node xml_mesh_segment = xml_mesh_segmentation.append_child("segment");
//
//         add_xml_text_child(xml_mesh_segment, "id", sit->id() );
//         // TODO: determine vertex count
//     //           add_xml_text_child(xml_mesh_segment, "vertex_count", viennagrid::vertices(*sit).size());
//         add_xml_text_child(xml_mesh_segment, "cell_count", viennagrid::cells(*sit).size());
//
//         if (seed_points)
//         {
//           for (typename SeedPointContainer::const_iterator spit = seed_points().begin(); spit != seed_points().end(); ++spit)
//           {
//             if (spit->second == sit->id())
//             {
//               add_xml_text_child(xml_mesh_segment, "seed_point", spit->first );
//             }
//           }
//         }
//       }
//
//       xml.save_file( (filename_without_extension + ".vmesh").c_str() );
//     }



/*
    template<typename MeshT, typename ParameterHandleT>
    bool mesh_writer::write( ParameterHandleT const & mesh, std::string const & filename, FileType file_type )
    {
      info(5) << "Found conversion to ViennaGrid mesh." << std::endl;

      switch (file_type)
      {
        case VTK:
        {
          info(5) << "Found .vtu/.pvd extension, using ViennaGrid VTK Writer" << std::endl;
          viennagrid::io::vtk_writer<MeshT> vtk_writer;
          vtk_writer( mesh(), filename.substr(0, filename.rfind(".")) );
          return true;
        }
//         case COMSOL_MPHTXT:
//         {
//           info(5) << "Found .mphtxt extension, using ViennaGrid MPHTXT Writer" << std::endl;
//           viennagrid::io::mphtxt_writer writer;
//           writer( mesh(), filename );
//           return true;
//         }
        default:
        {
          error(1) << "Unsupported extension: " << to_string(file_type) << std::endl;
          return false;
        }
      }

    }


    template<typename MeshT, typename SegmentationT, typename ParameterHandleT>
    bool mesh_writer::write( ParameterHandleT const & mesh, std::string const & filename, FileType file_type )
    {
      typedef viennagrid::segmented_mesh<MeshT, SegmentationT> WrappedMeshType;

      info(5) << "Found conversion to ViennaGrid mesh." << std::endl;


      switch (file_type)
      {
        case VTK:
        {
          info(5) << "Found .vtu/.pvd extension, using ViennaGrid VTK Writer" << std::endl;
          viennagrid::io::vtk_writer<MeshT, SegmentationT> vtk_writer;
          vtk_writer( mesh().mesh, mesh().segmentation, filename.substr(0, filename.rfind(".")) );
          return true;
        }
        case VMESH:
        {
          info(5) << "Found .vmesh extension, using vmesh writer" << std::endl;
          write_vmesh( mesh().mesh, mesh().segmentation, filename );
          return true;
        }
//         case COMSOL_MPHTXT:
//         {
//           info(5) << "Found .mphtxt extension, using ViennaGrid MPHTXT Writer" << std::endl;
//           viennagrid::io::mphtxt_writer writer;
//           writer( mesh().mesh, mesh().segmentation, filename );
//           return true;
//         }
        default:
        {
          error(1) << "Unsupported extension: " << to_string(file_type) << std::endl;
          return false;
        }
      }
    }*/







    template<typename WriterProxyT, typename TagT, int DimensionV>
    bool mesh_writer::generic_nonsegmented_write( const_parameter_handle const & mesh, std::string const & filename )
    {
      typedef typename viennamesh::result_of::full_config<TagT, DimensionV>::type FullConfigType;
      typedef typename viennamesh::result_of::thin_config<TagT, DimensionV>::type ThinConfigType;

      typedef viennagrid::mesh<FullConfigType> FullMeshType;
      typedef viennagrid::mesh<ThinConfigType> ThinMeshType;

      // full mesh
      if (mesh->is_type<FullMeshType>())
        return WriterProxyT()(dynamic_handle_cast<const FullMeshType>(mesh)(), filename, handle());

      // thin mesh
      if (mesh->is_type<ThinMeshType>())
        return WriterProxyT()(dynamic_handle_cast<const ThinMeshType>(mesh)(), filename, handle());

      typename result_of::const_parameter_handle<ThinMeshType>::type tmp = mesh->get_converted<ThinMeshType>();
      if (tmp)
        return WriterProxyT()(tmp(), filename, handle());
      else
        return false;
    }



    template<typename WriterProxyT, typename TagT, int DimensionV>
    bool mesh_writer::generic_segmented_write( const_parameter_handle const & mesh, std::string const & filename )
    {
      typedef typename viennamesh::result_of::full_config<TagT, DimensionV>::type FullConfigType;
      typedef typename viennamesh::result_of::thin_config<TagT, DimensionV>::type ThinConfigType;

      typedef viennagrid::mesh<FullConfigType> FullMeshType;
      typedef viennagrid::mesh<ThinConfigType> ThinMeshType;

      typedef typename viennagrid::result_of::segmentation<FullMeshType>::type FullSegmentationOfFullMeshType;
      typedef typename viennagrid::result_of::segmentation<ThinMeshType>::type FullSegmentationOfThinMeshType;

      typedef typename viennagrid::result_of::cell_only_segmentation<FullMeshType>::type CellOnlySegmentationOfFullMeshType;
      typedef typename viennagrid::result_of::cell_only_segmentation<ThinMeshType>::type CellOnlySegmentationOfThinMeshType;

      // full mesh, full segmentation
      if (mesh->is_type< viennagrid::segmented_mesh<FullMeshType, FullSegmentationOfFullMeshType> >())
        return WriterProxyT()(dynamic_handle_cast< const viennagrid::segmented_mesh<FullMeshType, FullSegmentationOfFullMeshType> >(mesh)(), filename, handle());

      // full mesh, cell only segmentation
      if (mesh->is_type< viennagrid::segmented_mesh<FullMeshType, CellOnlySegmentationOfFullMeshType> >())
        return WriterProxyT()(dynamic_handle_cast< const viennagrid::segmented_mesh<FullMeshType, CellOnlySegmentationOfFullMeshType> >(mesh)(), filename, handle());

      // thin mesh, full segmentation
      if (mesh->is_type< viennagrid::segmented_mesh<ThinMeshType, FullSegmentationOfThinMeshType> >())
        return WriterProxyT()(dynamic_handle_cast< const viennagrid::segmented_mesh<ThinMeshType, FullSegmentationOfThinMeshType> >(mesh)(), filename, handle());

      // thin mesh, full segmentation
      if (mesh->is_type< viennagrid::segmented_mesh<ThinMeshType, CellOnlySegmentationOfThinMeshType> >())
        return WriterProxyT()(dynamic_handle_cast< const viennagrid::segmented_mesh<ThinMeshType, CellOnlySegmentationOfThinMeshType> >(mesh)(), filename, handle());


      typename result_of::const_parameter_handle< viennagrid::segmented_mesh<ThinMeshType, CellOnlySegmentationOfThinMeshType> >::type tmp = mesh->get_converted< viennagrid::segmented_mesh<ThinMeshType, CellOnlySegmentationOfThinMeshType> >();

      if (tmp)
        return WriterProxyT()(tmp(), filename, handle());
      else
        return false;
    }



    template<typename WriterProxyT, typename TagT, int DimensionV>
    bool mesh_writer::generic_write( const_parameter_handle const & mesh, std::string const & filename, bool is_segmented )
    {
      if ( is_segmented )
        return generic_segmented_write<WriterProxyT, TagT, DimensionV>( mesh, filename );
      else
        return generic_nonsegmented_write<WriterProxyT, TagT, DimensionV>( mesh, filename );
    }



    template<typename WriterProxyT>
    bool mesh_writer::write_all( const_parameter_handle const & mesh, string const & filename_,
                                 int geometric_dimension, string cell_type, bool is_segmented )
    {
      string filename = filename_.substr(0, filename_.rfind("."));

      if ( cell_type == "1-simplex" && geometric_dimension == 1 )
        return generic_write<WriterProxyT, viennagrid::line_tag, 1>( mesh, filename, is_segmented );

      if ( cell_type == "1-simplex" && geometric_dimension == 2 )
        return generic_write<WriterProxyT, viennagrid::line_tag, 2>( mesh, filename, is_segmented );

      if ( cell_type == "1-simplex" && geometric_dimension == 3 )
        return generic_write<WriterProxyT, viennagrid::line_tag, 3>( mesh, filename, is_segmented );



      if ( cell_type == "2-simplex" && geometric_dimension == 2 )
        return generic_write<WriterProxyT, viennagrid::triangle_tag, 2>( mesh, filename, is_segmented );

      if ( cell_type == "2-simplex" && geometric_dimension == 3 )
        return generic_write<WriterProxyT, viennagrid::triangle_tag, 3>( mesh, filename, is_segmented );



      if ( cell_type == "quadrilateral" && geometric_dimension == 2 )
        return generic_write<WriterProxyT, viennagrid::quadrilateral_tag, 2>( mesh, filename, is_segmented );

      if ( cell_type == "quadrilateral" && geometric_dimension == 3 )
        return generic_write<WriterProxyT, viennagrid::quadrilateral_tag, 3>( mesh, filename, is_segmented );



      if ( cell_type == "3-simplex" && geometric_dimension == 3 )
        return generic_write<WriterProxyT, viennagrid::tetrahedron_tag, 3>( mesh, filename, is_segmented );



      if ( cell_type == "hexahedron" && geometric_dimension == 3 )
        return generic_write<WriterProxyT, viennagrid::hexahedron_tag, 3>( mesh, filename, is_segmented );


      error(1) << "Input mesh \"" << mesh->type_name() << "\"is not convertable to any supported ViennaGrid mesh." << std::endl;

      return false;
    }

    bool mesh_writer::write_mphtxt( const_parameter_handle const & mesh, string const & filename,
                                    int geometric_dimension, string cell_type, bool is_segmented )
    {
      if (geometric_dimension != 3 || cell_type != "3-simplex")
        return false;

      typedef viennamesh::result_of::full_config<viennagrid::tetrahedron_tag, 3>::type FullConfigType;
//       typedef typename viennamesh::result_of::thin_config<viennagrid::tetrahedron_tag, 3>::type ThinConfigType;

      typedef viennagrid::mesh<FullConfigType> FullMeshType;
//       typedef viennagrid::mesh<ThinConfigType> ThinMeshType;

      typedef viennagrid::result_of::segmentation<FullMeshType>::type FullSegmentationOfFullMeshType;
//       typedef typename viennagrid::result_of::segmentation<ThinMeshType>::type FullSegmentationOfThinMeshType;

      typedef viennagrid::result_of::cell_only_segmentation<FullMeshType>::type CellOnlySegmentationOfFullMeshType;
//       typedef typename viennagrid::result_of::cell_only_segmentation<ThinMeshType>::type CellOnlySegmentationOfThinMeshType;

      // full mesh, full segmentation
      if (mesh->is_type< viennagrid::segmented_mesh<FullMeshType, FullSegmentationOfFullMeshType> >())
        return mphtxt_writer_proxy()(dynamic_handle_cast< const viennagrid::segmented_mesh<FullMeshType, FullSegmentationOfFullMeshType> >(mesh)(), filename, handle());

      // full mesh, cell only segmentation
//       if (mesh->is_type< viennagrid::segmented_mesh<FullMeshType, CellOnlySegmentationOfFullMeshType> >())
//         return mphtxt_writer_proxy()(dynamic_handle_cast< const viennagrid::segmented_mesh<FullMeshType, CellOnlySegmentationOfFullMeshType> >(mesh)(), filename, handle());

      // thin mesh, full segmentation
//       if (mesh->is_type< viennagrid::segmented_mesh<ThinMeshType, FullSegmentationOfThinMeshType> >())
//         return WriterProxyT()(dynamic_handle_cast< const viennagrid::segmented_mesh<ThinMeshType, FullSegmentationOfThinMeshType> >(mesh)(), filename, handle());

      // thin mesh, full segmentation
//       if (mesh->is_type< viennagrid::segmented_mesh<ThinMeshType, CellOnlySegmentationOfThinMeshType> >())
//         return WriterProxyT()(dynamic_handle_cast< const viennagrid::segmented_mesh<ThinMeshType, CellOnlySegmentationOfThinMeshType> >(mesh)(), filename, handle());


      result_of::const_parameter_handle< viennagrid::segmented_mesh<FullMeshType, FullSegmentationOfFullMeshType> >::type tmp = mesh->get_converted< viennagrid::segmented_mesh<FullMeshType, FullSegmentationOfFullMeshType> >();

      if (tmp)
        return mphtxt_writer_proxy()(tmp(), filename, handle());
      else
        return false;
    }


    bool mesh_writer::run_impl()
    {
      const_parameter_handle mesh = get_required_input("default");
      const_string_parameter_handle filename = get_required_input<string>("filename");

      FileType file_type;
      const_string_parameter_handle file_type_param = get_input<string>("file_type");
      if (file_type_param)
        file_type = from_string( file_type_param() );
      else
        file_type = from_filename( filename() );

      info(1) << "Using file type " << to_string(file_type) << std::endl;

      int geometric_dimension = lexical_cast<int>(mesh->get_property("geometric_dimension").first);
      string cell_type = mesh->get_property("cell_type").first;
      bool is_segmented = lexical_cast< stringtools::boolalpha_bool >(mesh->get_property("is_segmented").first);


      info(1) << "Found geometric dimension: " << geometric_dimension << std::endl;
      info(1) << "Found cell type: " << cell_type << std::endl;
      info(1) << "Is segmented: " << std::boolalpha << is_segmented << std::endl;


      switch (file_type)
      {
        case VTK:
          return write_all<vtk_writer_proxy>( mesh, filename(), geometric_dimension, cell_type, is_segmented );
        case VMESH:
          return write_all<vmesh_writer_proxy>( mesh, filename(), geometric_dimension, cell_type, is_segmented );
        case COMSOL_MPHTXT:
          return write_mphtxt( mesh, filename(), geometric_dimension, cell_type, is_segmented );
        default:
          return false;
      }
    }
  }
}
