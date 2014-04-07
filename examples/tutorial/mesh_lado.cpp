#include "viennamesh/viennamesh.hpp"
#include "viennamesh/core/sizing_function.hpp"

#include "viennagrid/io/vtk_writer.hpp"

int main()
{
  // creating an algorithm for reading a mesh from a file
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );

  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::algorithm_handle scale_algo( new viennamesh::linear_transform::algorithm() );
  viennamesh::algorithm_handle mesher( new viennamesh::tetgen::algorithm() );

  // creating an algorithm for writing a mesh to a file
  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );



  reader->set_input( "filename", "/export/florian/work/projects/2014_01 Lado Meshes/samples/TSV_01.bnd" );
//   reader->set_input( "filename", "/export/florian/work/projects/2014_01 Lado Meshes/samples/test_boundary.bnd" );

  reader->run();


  double angle = 0.0;
  double scale = 10000.0;


  std::vector<double> matrix(9, 0.0);

  matrix[0] = std::cos(angle) * scale;
  matrix[1] = -std::sin(angle) * scale;
  matrix[3] = std::sin(angle) * scale;
  matrix[4] = std::cos(angle) * scale;
  matrix[8] = scale;

  scale_algo->link_input( "default", reader, "default" );
  scale_algo->set_input( "matrix", matrix );

  scale_algo->run();




  viennamesh::algorithm_handle no_optimziation_mesher( new viennamesh::tetgen::algorithm() );

  typedef viennagrid::tetrahedral_3d_mesh MeshType;
  typedef viennagrid::tetrahedral_3d_segmentation SegmentationType;
  typedef viennagrid::tetrahedral_3d_segment_handle SegmentType;

  typedef viennagrid::segmented_mesh<MeshType, SegmentationType> SegmentedMeshType;
  viennamesh::result_of::parameter_handle<SegmentedMeshType>::type segmented_mesh = viennamesh::make_parameter<SegmentedMeshType>();

  no_optimziation_mesher->reference_output( "default", segmented_mesh() );

  no_optimziation_mesher->link_input( "default", scale_algo, "default" );
  no_optimziation_mesher->set_input( "option_string", "zpVx10000000A" );

  no_optimziation_mesher->run();





  SegmentType middle_hole = segmented_mesh().segmentation.get_segment(0);
  SegmentType base = segmented_mesh().segmentation.get_segment(1);
  SegmentType middle_cirular_zylinder = segmented_mesh().segmentation.get_segment(2);
  SegmentType outer_hole = segmented_mesh().segmentation.get_segment(3);
  SegmentType upper_cirular_zylinder = segmented_mesh().segmentation.get_segment(4);
  SegmentType coat = segmented_mesh().segmentation.get_segment(5);
  SegmentType lower_cirular_zylinder = segmented_mesh().segmentation.get_segment(6);
  SegmentType inner_hole = segmented_mesh().segmentation.get_segment(7);




  // linking the output from the reader to the mesher
  mesher->link_input( "default", scale_algo, "default" );
  mesher->set_input( "option_string", "zpVO4/7x10000000A" );



  pugi::xml_document xml_element_size;
  xml_element_size.load_file("test.xml");

  viennamesh::sizing_function::sizing_function_3d_handle function = viennamesh::sizing_function::from_xml(xml_element_size.first_child(), segmented_mesh().mesh, segmented_mesh().segmentation);


//   viennamesh::sizing_function::sizing_function_3d_handle function =
//     viennamesh::sizing_function::linear_interpolate(
//       viennamesh::sizing_function::distance_to_segment_boundaries<viennagrid::line_tag>(base, upper_cirular_zylinder, outer_hole),
//       2.0*scale, 2.0*scale, 1.0*scale, 50.0*scale);


//       viennamesh::sizing_function::constant<viennamesh::point_3d>(10000.0);

  viennamesh::sizing_function_3d sizing_function = viennamesh::bind(viennamesh::sizing_function::get<viennamesh::sizing_function::base_sizing_function_3d>, function, _1);

  mesher->set_input( "sizing_function", sizing_function );


//   typedef viennagrid::segmented_mesh<MeshType, SegmentationType> SegmentedMeshType;
//   viennamesh::result_of::parameter_handle<SegmentedMeshType>::type segmented_optimized_mesh = viennamesh::make_parameter<SegmentedMeshType>();
//
//   mesher->reference_output( "default", segmented_optimized_mesh() );

  mesher->run();


//   typedef viennagrid::result_of::vertex<MeshType>::type VertexType;
//   typedef viennagrid::result_of::accessor_container<VertexType, double, viennagrid::std_map_tag>::type VertexDoubleContainerType;
//   typedef viennagrid::result_of::field<VertexDoubleContainerType, VertexType>::type VertexDoubleFieldType;
//
//   VertexDoubleContainerType element_size_container;
//   VertexDoubleFieldType element_size_field(element_size_container);
//
//   typedef viennagrid::result_of::const_vertex_range<MeshType>::type ConstVertexRangeType;
//   typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;
//
//   ConstVertexRangeType vertices( segmented_optimized_mesh().mesh );
//   for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
//     element_size_field(*vit) = function->get( viennagrid::point(*vit) );
//
//
//   {
//     viennagrid::io::vtk_writer<MeshType, SegmentationType> vtk_writer;
//
//     viennagrid::io::add_scalar_data_on_vertices( vtk_writer, element_size_field, "element_size" );
//
//     vtk_writer( segmented_optimized_mesh().mesh, segmented_optimized_mesh().segmentation, "tsv" );
//   }


  // linking the output from the mesher to the writer
  writer->link_input( "default", mesher, "default" );
  writer->set_input( "filename", "tsv.vtu" );

  writer->run();

}
