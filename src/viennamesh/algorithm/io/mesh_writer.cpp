#include "viennamesh/algorithm/io/mesh_writer.hpp"
#include "viennamesh/core/mesh_quantities.hpp"

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/mphtxt_writer.hpp"

#include "pugixml/pugixml.hpp"



namespace viennamesh
{
  namespace io
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

      template<typename MeshT, typename SegmentationT>
      void operator() (viennagrid::segmented_mesh<MeshT, SegmentationT> const & segmented_mesh,
                       std::string const & filename,
                       mesh_writer const &)
      {
        viennagrid::io::vtk_writer<MeshT, SegmentationT> writer;
        writer(segmented_mesh.mesh, segmented_mesh.segmentation, filename);
      }

      template<typename MeshT, typename SegmentationT, typename SegmentIDT, typename VertexKeyT, typename CellKeyT, typename ValueT>
      void operator() (viennagrid::segmented_mesh<MeshT, SegmentationT> const & segmented_mesh,
                       viennamesh::segmented_mesh_quantities<SegmentIDT, VertexKeyT, CellKeyT, ValueT> const & quantities,
                       std::string const & filename,
                       mesh_writer const &)
      {
        viennagrid::io::vtk_writer<MeshT, SegmentationT> writer;
        quantities.template toWriter<MeshT>(writer);
        writer(segmented_mesh.mesh, segmented_mesh.segmentation, filename);
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
      void operator() (MeshT const & mesh,
                       std::string const & filename,
                       mesh_writer const & algorithm)
      {
        std::string filename_without_extension = filename.substr(0, filename.rfind(".vmesh"));

        vtk_writer_proxy()(mesh, filename, algorithm);

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
      }

      template<typename MeshT, typename SegmentationT>
      void operator() (viennagrid::segmented_mesh<MeshT, SegmentationT> const & segmented_mesh,
                       std::string const & filename,
                       mesh_writer const & algorithm)
      {
        typedef typename viennagrid::result_of::point<MeshT>::type PointType;
        typedef typename viennamesh::result_of::seed_point_container<PointType>::type SeedPointContainer;

        typename result_of::const_parameter_handle<SeedPointContainer>::type spp = algorithm.input_seed_points.get<SeedPointContainer>();


        std::string filename_without_extension = filename.substr(0, filename.rfind(".vmesh"));

        vtk_writer_proxy()(segmented_mesh, filename, algorithm);

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

          if (spp)
          {
            for (typename SeedPointContainer::const_iterator spit = spp().begin(); spit != spp().end(); ++spit)
            {
              if (spit->second == sit->id())
              {
                add_xml_text_child(xml_mesh_segment, "seed_point", spit->first );
              }
            }
          }
        }

        xml.save_file( (filename_without_extension + ".vmesh").c_str() );
      }





      template<typename MeshT, typename SegmentationT, typename SegmentIDT, typename VertexKeyT, typename CellKeyT, typename ValueT>
      void operator() (viennagrid::segmented_mesh<MeshT, SegmentationT> const & segmented_mesh,
                       viennamesh::segmented_mesh_quantities<SegmentIDT, VertexKeyT, CellKeyT, ValueT> const & quantities,
                       std::string const & filename,
                       mesh_writer const & algorithm)
      {
        typedef typename viennagrid::result_of::point<MeshT>::type PointType;
        typedef typename viennamesh::result_of::seed_point_container<PointType>::type SeedPointContainer;

        typename result_of::const_parameter_handle<SeedPointContainer>::type spp = algorithm.input_seed_points.get<SeedPointContainer>();


        std::string filename_without_extension = filename.substr(0, filename.rfind(".vmesh"));

        vtk_writer_proxy()(segmented_mesh, quantities, filename, algorithm);

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

          if (spp)
          {
            for (typename SeedPointContainer::const_iterator spit = spp().begin(); spit != spp().end(); ++spit)
            {
              if (spit->second == sit->id())
              {
                add_xml_text_child(xml_mesh_segment, "seed_point", spit->first );
              }
            }
          }
        }

        xml.save_file( (filename_without_extension + ".vmesh").c_str() );
      }
    };





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

      template<typename MeshT, typename SegmentationT>
      void operator() (viennagrid::segmented_mesh<MeshT, SegmentationT> const & segmented_mesh,
                       std::string const & filename,
                       mesh_writer const &)
      {
        viennagrid::io::mphtxt_writer writer;
        writer(segmented_mesh.mesh, segmented_mesh.segmentation, filename);
      }

      template<typename MeshT, typename SegmentationT, typename SegmentIDT, typename VertexKeyT, typename CellKeyT, typename ValueT>
      void operator() (viennagrid::segmented_mesh<MeshT, SegmentationT> const & segmented_mesh,
                       viennamesh::segmented_mesh_quantities<SegmentIDT, VertexKeyT, CellKeyT, ValueT> const &,
                       std::string const & filename,
                       mesh_writer const &)
      {
        viennagrid::io::mphtxt_writer writer;
        writer(segmented_mesh.mesh, segmented_mesh.segmentation, filename);
      }
    };






    mesh_writer::mesh_writer() :
      input_mesh(*this, parameter_information("mesh","mesh","The mesh to be written")),
      filename(*this, parameter_information("filename","string","The filename of the mesh to be written")),
      filetype(*this, parameter_information("filetype","string","The filetype of the mesh to be written, Supported filetypes: VTK, VMESH, COMSOL_MPHTXT")),
      input_seed_points(*this, parameter_information("seed_points","seed_point_1d_container|seed_point_2d_container|seed_point_3d_container","The seed points to be written")),
//       input_hole_points(*this, "hole_points")
      quantities(*this, parameter_information("quantities","segmented_mesh_quantities|mesh_quantities","The mesh quantities to be written")) {}

    std::string mesh_writer::name() const { return "ViennaGrid Mesh Writer"; }
    std::string mesh_writer::id() const { return "mesh_writer"; }





    template<typename WriterProxyT, typename MeshT>
    bool mesh_writer::basic_nonsegmented_write( const_parameter_handle const & mesh, std::string const & filename )
    {
      if (mesh->is_type<MeshT>())
      {
        WriterProxyT()(dynamic_handle_cast<const MeshT>(mesh)(), filename, *this);
        return true;
      }

      return false;
    }

    template<typename WriterProxyT, typename MeshT, typename SegmentationT>
    bool mesh_writer::basic_segmented_write( const_parameter_handle const & mesh, std::string const & filename )
    {
      // full mesh, full segmentation
      if (mesh->is_type< viennagrid::segmented_mesh<MeshT, SegmentationT> >())
      {
        typedef typename viennamesh::result_of::segmented_mesh_quantities<MeshT, SegmentationT>::type SegmentedMeshQuantitiesType;
        typename viennamesh::result_of::const_parameter_handle<SegmentedMeshQuantitiesType>::type qp = quantities.get<SegmentedMeshQuantitiesType>();

        if (qp)
          WriterProxyT()(dynamic_handle_cast< const viennagrid::segmented_mesh<MeshT, SegmentationT> >(mesh)(), qp(), filename, *this);
        else
          WriterProxyT()(dynamic_handle_cast< const viennagrid::segmented_mesh<MeshT, SegmentationT> >(mesh)(), filename, *this);

        return true;
      }

      return false;
    }


    template<typename WriterProxyT, typename TagT, int DimensionV>
    bool mesh_writer::generic_nonsegmented_write( const_parameter_handle const & mesh, std::string const & filename )
    {
      typedef typename viennamesh::result_of::full_config<TagT, DimensionV>::type FullConfigType;
      typedef typename viennamesh::result_of::thin_config<TagT, DimensionV>::type ThinConfigType;

      typedef viennagrid::mesh<FullConfigType> FullMeshType;
      typedef viennagrid::mesh<ThinConfigType> ThinMeshType;

      // full mesh
      if (basic_nonsegmented_write<WriterProxyT, FullMeshType>(mesh, filename))
        return true;

      // thin mesh
      if (basic_nonsegmented_write<WriterProxyT, ThinMeshType>(mesh, filename))
        return true;

      typename result_of::const_parameter_handle<ThinMeshType>::type tmp = mesh->get_converted<ThinMeshType>();
      if (tmp)
      {
        WriterProxyT()(tmp(), filename, *this);
        return true;
      }
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
      if (basic_segmented_write<WriterProxyT, FullMeshType, FullSegmentationOfFullMeshType>(mesh, filename))
        return true;

      // full mesh, cell only segmentation
      if (basic_segmented_write<WriterProxyT, FullMeshType, CellOnlySegmentationOfFullMeshType>(mesh, filename))
        return true;

      // thin mesh, full segmentation
      if (basic_segmented_write<WriterProxyT, ThinMeshType, FullSegmentationOfThinMeshType>(mesh, filename))
        return true;

      // thin mesh, cell only segmentation
      if (basic_segmented_write<WriterProxyT, ThinMeshType, CellOnlySegmentationOfThinMeshType>(mesh, filename))
        return true;

      typename result_of::const_parameter_handle< viennagrid::segmented_mesh<ThinMeshType, CellOnlySegmentationOfThinMeshType> >::type tmp = mesh->get_converted< viennagrid::segmented_mesh<ThinMeshType, CellOnlySegmentationOfThinMeshType> >();

      if (tmp)
      {
        WriterProxyT()(tmp(), filename, *this);
        return true;
      }
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
    bool mesh_writer::write_all( const_parameter_handle const & mesh, std::string const & filename_,
                                 int geometric_dimension, std::string cell_type, bool is_segmented )
    {
      std::string filename = filename_.substr(0, filename_.rfind("."));

      if ( cell_type == "1-simplex" && geometric_dimension == 1 )
        return generic_write<WriterProxyT, viennagrid::line_tag, 1>( mesh, filename, is_segmented );

      if ( cell_type == "1-simplex" && geometric_dimension == 2 )
        return generic_write<WriterProxyT, viennagrid::line_tag, 2>( mesh, filename, is_segmented );

      if ( cell_type == "1-simplex" && geometric_dimension == 3 )
        return generic_write<WriterProxyT, viennagrid::line_tag, 3>( mesh, filename, is_segmented );



      if ( cell_type == "2-simplex" && geometric_dimension == 2 )
        return generic_write<WriterProxyT, viennagrid::triangle_tag, 2>( mesh, filename, is_segmented );

      if ( cell_type == "2-simplex" && geometric_dimension == 3 )
      {
        if (generic_write<WriterProxyT, viennagrid::triangle_tag, 3>( mesh, filename, is_segmented ))
          return true;

        if (is_segmented)
        {
          if (basic_segmented_write<WriterProxyT, viennagrid::triangular_3d_mesh, viennagrid::triangular_hull_3d_segmentation>(mesh, filename))
            return true;
        }

        return false;
      }



      if ( cell_type == "quadrilateral" && geometric_dimension == 2 )
        return generic_write<WriterProxyT, viennagrid::quadrilateral_tag, 2>( mesh, filename, is_segmented );

      if ( cell_type == "quadrilateral" && geometric_dimension == 3 )
        return generic_write<WriterProxyT, viennagrid::quadrilateral_tag, 3>( mesh, filename, is_segmented );



      if ( cell_type == "3-simplex" && geometric_dimension == 3 )
        return generic_write<WriterProxyT, viennagrid::tetrahedron_tag, 3>( mesh, filename, is_segmented );



      if ( cell_type == "hexahedron" && geometric_dimension == 3 )
        return generic_write<WriterProxyT, viennagrid::hexahedron_tag, 3>( mesh, filename, is_segmented );


      error(1) << "Input mesh \"" << mesh->type_name() << "\" is not convertable to any supported ViennaGrid mesh." << std::endl;

      return false;
    }

    bool mesh_writer::write_mphtxt( const_parameter_handle const & mesh, std::string const & filename,
                                    int geometric_dimension, std::string cell_type )
    {
      if (geometric_dimension != 3 || cell_type != "3-simplex")
        return false;

      typedef viennamesh::result_of::full_config<viennagrid::tetrahedron_tag, 3>::type FullConfigType;

      typedef viennagrid::mesh<FullConfigType> FullMeshType;

      typedef viennagrid::result_of::segmentation<FullMeshType>::type FullSegmentationOfFullMeshType;

      // full mesh, full segmentation
      if (mesh->is_type< viennagrid::segmented_mesh<FullMeshType, FullSegmentationOfFullMeshType> >())
      {
        mphtxt_writer_proxy()(dynamic_handle_cast< const viennagrid::segmented_mesh<FullMeshType, FullSegmentationOfFullMeshType> >(mesh)(), filename, *this);
        return true;
      }

      result_of::const_parameter_handle< viennagrid::segmented_mesh<FullMeshType, FullSegmentationOfFullMeshType> >::type tmp = mesh->get_converted< viennagrid::segmented_mesh<FullMeshType, FullSegmentationOfFullMeshType> >();

      if (tmp)
      {
        mphtxt_writer_proxy()(tmp(), filename, *this);
        return true;
      }
      else
        return false;
    }


    bool mesh_writer::run_impl()
    {
      const_parameter_handle mesh = input_mesh.get();
      info(1) << "Writing mesh to file \"" << filename() << "\"" << std::endl;

      FileType ft;
      if (filetype.valid())
        ft = from_string( filetype() );
      else
        ft = from_filename( filename() );

      info(1) << "Using file type " << to_string(ft) << std::endl;

      int geometric_dimension = lexical_cast<int>(mesh->get_property("geometric_dimension").first);
      std::string cell_type = mesh->get_property("cell_type").first;
      bool is_segmented = lexical_cast< stringtools::boolalpha_bool >(mesh->get_property("is_segmented").first);


      info(1) << "Found geometric dimension: " << geometric_dimension << std::endl;
      info(1) << "Found cell type: " << cell_type << std::endl;
      info(1) << "Is segmented: " << std::boolalpha << is_segmented << std::endl;


      try
      {
        switch (ft)
        {
          case VTK:
            return write_all<vtk_writer_proxy>( mesh, filename(), geometric_dimension, cell_type, is_segmented );
          case VMESH:
            return write_all<vmesh_writer_proxy>( mesh, filename(), geometric_dimension, cell_type, is_segmented );
          case COMSOL_MPHTXT:
            return write_mphtxt( mesh, filename(), geometric_dimension, cell_type );
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
}
