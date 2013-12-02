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
          add_xml_text_child(xml_mesh_segment, "vertex_count", viennagrid::vertices(*sit).size());
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




      template<typename MeshT>
      bool write( const_parameter_handle const & mesh, string const & filename )
      {
        typename result_of::const_parameter_handle<MeshT>::type tmp = dynamic_handle_cast<const MeshT>( mesh );
        if (!tmp)
          tmp = mesh->get_converted<MeshT>();

        if (!tmp)
          return false;

        info(5) << "Found conversion to ViennaGrid mesh." << std::endl;

        string extension = filename.substr( filename.rfind(".")+1 );

        if (extension == "vtu" || extension == "pvd")
        {
          info(5) << "Found .vtu/.pvd extension, using ViennaGrid VTK Writer" << std::endl;
          viennagrid::io::vtk_writer<MeshT> vtk_writer;
          vtk_writer( tmp(), filename.substr(0, filename.rfind(".")) );
          return true;
        }

        error(1) << "Unsupported extension: " << extension << std::endl;

        return false;
      }


      template<typename MeshT, typename SegmentationT>
      bool write( const_parameter_handle const & mesh, string const & filename )
      {
        typedef viennagrid::segmented_mesh<MeshT, SegmentationT> WrappedMeshType;
        typename result_of::const_parameter_handle<WrappedMeshType>::type tmp = dynamic_handle_cast<const WrappedMeshType>( mesh );
        if (!tmp)
          tmp = mesh->get_converted<WrappedMeshType>();

        if (!tmp)
          return false;

        info(5) << "Found conversion to ViennaGrid mesh." << std::endl;

        string extension = filename.substr( filename.rfind(".")+1 );

        if (extension == "vtu" || extension == "pvd")
        {
          info(5) << "Found .vtu/.pvd extension, using ViennaGrid VTK Writer" << std::endl;
          viennagrid::io::vtk_writer<MeshT, SegmentationT> vtk_writer;
          vtk_writer( tmp().mesh, tmp().segmentation, filename.substr(0, filename.rfind(".")) );
          return true;
        }

        if (extension == "vmesh")
        {
          info(5) << "Found .vmesh extension, using vmesh writer" << std::endl;
          write_vmesh( tmp().mesh, tmp().segmentation, filename );
          return true;
        }

        error(1) << "Unsupported extension: " << extension << std::endl;

        return false;
      }







      string name() const { return "ViennaGrid Mesh Writer"; }

      bool run_impl()
      {
        const_parameter_handle mesh = get_required_input("default");
        const_string_parameter_handle filename = get_required_input<string>("filename");



        if (write<viennagrid::line_1d_mesh, viennagrid::line_1d_segmentation>(mesh, filename()))
          return true;
        if (write<viennagrid::line_1d_mesh>(mesh, filename()))
          return true;

        if (write<viennagrid::line_2d_mesh, viennagrid::line_2d_segmentation>(mesh, filename()))
          return true;
        if (write<viennagrid::line_2d_mesh>(mesh, filename()))
          return true;

        if (write<viennagrid::line_3d_mesh, viennagrid::line_3d_segmentation>(mesh, filename()))
          return true;
        if (write<viennagrid::line_3d_mesh>(mesh, filename()))
          return true;


        if (write<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation>(mesh, filename()))
          return true;
        if (write<viennagrid::triangular_2d_mesh>(mesh, filename()))
          return true;

        if (write<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation>(mesh, filename()))
          return true;
        if (write<viennagrid::triangular_3d_mesh>(mesh, filename()))
          return true;


        if (write<viennagrid::quadrilateral_2d_mesh, viennagrid::quadrilateral_2d_segmentation>(mesh, filename()))
          return true;
        if (write<viennagrid::quadrilateral_2d_mesh>(mesh, filename()))
          return true;

        if (write<viennagrid::quadrilateral_3d_mesh, viennagrid::quadrilateral_3d_segmentation>(mesh, filename()))
          return true;
        if (write<viennagrid::quadrilateral_3d_mesh>(mesh, filename()))
          return true;


        if (write<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>(mesh, filename()))
          return true;
        if (write<viennagrid::tetrahedral_3d_mesh>(mesh, filename()))
          return true;


        if (write<viennagrid::hexahedral_3d_mesh, viennagrid::hexahedral_3d_segmentation>(mesh, filename()))
          return true;
        if (write<viennagrid::hexahedral_3d_mesh>(mesh, filename()))
          return true;


        error(1) << "Input mesh is not convertable to any supported ViennaGrid mesh." << std::endl;

        return false;
      }

    private:

    };

  }

}



#endif
