#ifndef VIENNAMESH_ALGORITHM_IO_MESH_WRITER_HPP
#define VIENNAMESH_ALGORITHM_IO_MESH_WRITER_HPP

#include "viennamesh/core/algorithm.hpp"

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/vtk_writer.hpp"

#include "pugixml/pugixml.hpp"




namespace viennamesh
{
  namespace io
  {

    class mesh_writer : public base_algorithm
    {
    public:




      void add_xml_text_child( pugi::xml_node & parent, string const & element_name, string const & value  )
      {
        pugi::xml_node tmp = parent.append_child( element_name.c_str() );
        tmp.append_child( pugi::node_pcdata ).set_value( value.c_str() );
      }

      template<typename TypeT>
      void add_xml_text_child( pugi::xml_node & parent, string const & element_name, TypeT const & value  )
      {
        add_xml_text_child( parent, element_name, lexical_cast<string>(value) );
      }


      template<typename MeshT, typename SegmentationT>
      void write_vmesh( MeshT const & mesh,  SegmentationT const & segmentation, string const & filename )
      {
        typedef typename viennagrid::result_of::point<MeshT>::type PointType;
        typedef typename viennamesh::result_of::seed_point_container<PointType>::type SeedPointContainer;

        typename result_of::const_parameter_handle<SeedPointContainer>::type seed_points = get_input<SeedPointContainer>("seed_points");


        string filename_without_extension = filename.substr(0, filename.rfind(".vmesh"));

        viennagrid::io::vtk_writer<MeshT, SegmentationT> vtk_writer;
        vtk_writer( mesh, segmentation, filename_without_extension );

        pugi::xml_document xml;

        pugi::xml_node xml_mesh = xml.append_child("mesh");

        add_xml_text_child(xml_mesh, "file", (filename_without_extension + (segmentation.size() > 1 ? "_main.pvd" : ".vtu")).c_str() );
        add_xml_text_child(xml_mesh, "dimension", typename viennagrid::result_of::point<MeshT>::type().size() );
        add_xml_text_child(xml_mesh, "vertex_count", viennagrid::vertices(mesh).size() );
        add_xml_text_child(xml_mesh, "cell_count", viennagrid::cells(mesh).size() );
        add_xml_text_child(xml_mesh, "segment_count", segmentation.size() );

        pugi::xml_node xml_mesh_topology = xml_mesh.append_child("topology");
        add_xml_text_child(xml_mesh_topology, "celltype", viennagrid::result_of::cell_tag<MeshT>::type::name() );

        pugi::xml_node xml_mesh_segmentation = xml_mesh.append_child("segmentation");
        for ( typename SegmentationT::const_iterator sit = segmentation.begin(); sit != segmentation.end(); ++sit )
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
      }




      template<typename MeshT, typename ParameterHandleT>
      bool write( ParameterHandleT const & mesh, string const & filename )
      {
        info(5) << "Found conversion to ViennaGrid mesh." << std::endl;
        string extension = filename.substr( filename.rfind(".")+1 );

        if (extension == "vtu" || extension == "pvd")
        {
          info(5) << "Found .vtu/.pvd extension, using ViennaGrid VTK Writer" << std::endl;
          viennagrid::io::vtk_writer<MeshT> vtk_writer;
          vtk_writer( mesh(), filename.substr(0, filename.rfind(".")) );
          return true;
        }

        error(1) << "Unsupported extension: " << extension << std::endl;

        return false;
      }


      template<typename MeshT, typename SegmentationT, typename ParameterHandleT>
      bool write( ParameterHandleT const & mesh, string const & filename )
      {
        typedef viennagrid::segmented_mesh<MeshT, SegmentationT> WrappedMeshType;

        info(5) << "Found conversion to ViennaGrid mesh." << std::endl;
        string extension = filename.substr( filename.rfind(".")+1 );

        if (extension == "vtu" || extension == "pvd")
        {
          info(5) << "Found .vtu/.pvd extension, using ViennaGrid VTK Writer" << std::endl;
          viennagrid::io::vtk_writer<MeshT, SegmentationT> vtk_writer;
          vtk_writer( mesh().mesh, mesh().segmentation, filename.substr(0, filename.rfind(".")) );
          return true;
        }

        if (extension == "vmesh")
        {
          info(5) << "Found .vmesh extension, using vmesh writer" << std::endl;
          write_vmesh( mesh().mesh, mesh().segmentation, filename );
          return true;
        }

        error(1) << "Unsupported extension: " << extension << std::endl;

        return false;
      }



      template<typename TagT, int DimensionT>
      bool generic_run_nonsegmented( const_parameter_handle const & mesh, string const & filename )
      {
        typedef typename viennamesh::result_of::full_config<TagT, DimensionT>::type FullConfigType;
        typedef typename viennamesh::result_of::thin_config<TagT, DimensionT>::type ThinConfigType;

        typedef viennagrid::mesh<FullConfigType> FullMeshType;
        typedef viennagrid::mesh<ThinConfigType> ThinMeshType;

        // full mesh
        if (mesh->is_type<FullMeshType>())
          return write<FullMeshType>( dynamic_handle_cast<const FullMeshType>(mesh), filename );

        // thin mesh
        if (mesh->is_type<ThinMeshType>())
          return write<ThinMeshType>( dynamic_handle_cast<const ThinMeshType>(mesh), filename );

        typename result_of::const_parameter_handle<ThinMeshType>::type tmp = mesh->get_converted<ThinMeshType>();
        return write<ThinMeshType>(tmp, filename);
      }



      template<typename TagT, int DimensionT>
      bool generic_run_segmented( const_parameter_handle const & mesh, string const & filename )
      {
        typedef typename viennamesh::result_of::full_config<TagT, DimensionT>::type FullConfigType;
        typedef typename viennamesh::result_of::thin_config<TagT, DimensionT>::type ThinConfigType;

        typedef viennagrid::mesh<FullConfigType> FullMeshType;
        typedef viennagrid::mesh<ThinConfigType> ThinMeshType;

        typedef typename viennagrid::result_of::segmentation<FullMeshType>::type FullSegmentationOfFullMeshType;
        typedef typename viennagrid::result_of::segmentation<ThinMeshType>::type FullSegmentationOfThinMeshType;

        typedef typename viennagrid::result_of::cell_only_segmentation<FullMeshType>::type CellOnlySegmentationOfFullMeshType;
        typedef typename viennagrid::result_of::cell_only_segmentation<ThinMeshType>::type CellOnlySegmentationOfThinMeshType;

        // full mesh, full segmentation
        if (mesh->is_type< viennagrid::segmented_mesh<FullMeshType, FullSegmentationOfFullMeshType> >())
          return write<FullMeshType, FullSegmentationOfFullMeshType>(dynamic_handle_cast< const viennagrid::segmented_mesh<FullMeshType, FullSegmentationOfFullMeshType> >(mesh), filename);

        // full mesh, cell only segmentation
        if (mesh->is_type< viennagrid::segmented_mesh<FullMeshType, CellOnlySegmentationOfFullMeshType> >())
          return write<FullMeshType, CellOnlySegmentationOfFullMeshType>(dynamic_handle_cast< const viennagrid::segmented_mesh<FullMeshType, CellOnlySegmentationOfFullMeshType> >(mesh), filename);

        // thin mesh, full segmentation
        if (mesh->is_type< viennagrid::segmented_mesh<ThinMeshType, FullSegmentationOfThinMeshType> >())
          return write<ThinMeshType, FullSegmentationOfThinMeshType>(dynamic_handle_cast< const viennagrid::segmented_mesh<ThinMeshType, FullSegmentationOfThinMeshType> >(mesh), filename);

        // thin mesh, full segmentation
        if (mesh->is_type< viennagrid::segmented_mesh<ThinMeshType, CellOnlySegmentationOfThinMeshType> >())
          return write<ThinMeshType, CellOnlySegmentationOfThinMeshType>(dynamic_handle_cast< const viennagrid::segmented_mesh<ThinMeshType, CellOnlySegmentationOfThinMeshType> >(mesh), filename);


        typename result_of::const_parameter_handle< viennagrid::segmented_mesh<ThinMeshType, CellOnlySegmentationOfThinMeshType> >::type tmp = mesh->get_converted< viennagrid::segmented_mesh<ThinMeshType, CellOnlySegmentationOfThinMeshType> >();
        return write<ThinMeshType, CellOnlySegmentationOfThinMeshType>(tmp, filename);
      }



      template<typename TagT, int DimensionT>
      bool generic_run( const_parameter_handle const & mesh, string const & filename )
      {
        bool is_segmented = boost::lexical_cast< stringtools::locale_bool >(mesh->get_property("is_segmented").first);

        if ( is_segmented )
          return generic_run_segmented<viennagrid::tetrahedron_tag, 3>( mesh, filename );
        else
          return generic_run_nonsegmented<viennagrid::tetrahedron_tag, 3>( mesh, filename );
      }





      string name() const { return "ViennaGrid Mesh Writer"; }

      bool run_impl()
      {
        const_parameter_handle mesh = get_required_input("default");
        const_string_parameter_handle filename = get_required_input<string>("filename");

        int geometric_dimension = boost::lexical_cast<int>(mesh->get_property("geometric_dimension").first);
        string cell_type = mesh->get_property("cell_type").first;




        if ( cell_type == "1-simplex" && geometric_dimension == 1 )
          return generic_run<viennagrid::line_tag, 1>( mesh, filename() );

        if ( cell_type == "1-simplex" && geometric_dimension == 2 )
          return generic_run<viennagrid::line_tag, 2>( mesh, filename() );

        if ( cell_type == "1-simplex" && geometric_dimension == 3 )
          return generic_run<viennagrid::line_tag, 3>( mesh, filename() );



        if ( cell_type == "2-simplex" && geometric_dimension == 2 )
          return generic_run<viennagrid::triangle_tag, 2>( mesh, filename() );

        if ( cell_type == "2-simplex" && geometric_dimension == 3 )
          return generic_run<viennagrid::triangle_tag, 3>( mesh, filename() );



        if ( cell_type == "quadrilateral" && geometric_dimension == 2 )
          return generic_run<viennagrid::quadrilateral_tag, 2>( mesh, filename() );

        if ( cell_type == "quadrilateral" && geometric_dimension == 3 )
          return generic_run<viennagrid::quadrilateral_tag, 3>( mesh, filename() );



        if ( cell_type == "3-simplex" && geometric_dimension == 3 )
          return generic_run<viennagrid::tetrahedron_tag, 3>( mesh, filename() );



        if ( cell_type == "hexahedron" && geometric_dimension == 3 )
          return generic_run<viennagrid::hexahedron_tag, 3>( mesh, filename() );


        error(1) << "Input mesh \"" << mesh->type_name() << "\"is not convertable to any supported ViennaGrid mesh." << std::endl;

        return false;
      }

    private:

    };

  }

}



#endif
