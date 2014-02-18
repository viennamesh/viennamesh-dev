#include "viennamesh/algorithm/viennagrid.hpp"
#include "viennamesh/algorithm/io.hpp"
#include "viennamesh/algorithm/triangle.hpp"

#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"

#include "viennamesh/core/sizing_function.hpp"


double my_sizing( viennamesh::point_2d const & pt )
{
  return (pt[0]+1.0)/40.0;
}




template<typename SomethingT>
std::pair< typename viennagrid::result_of::point<SomethingT>::type, typename viennagrid::result_of::point<SomethingT>::type > bounding_box( SomethingT const & something )
{
  typedef typename viennagrid::result_of::point<SomethingT>::type PointType;
  typedef typename viennagrid::result_of::const_vertex_range<SomethingT>::type ConstVertexRangeType;
  typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

  PointType lower;
  PointType upper;

  ConstVertexRangeType vertices(something);
  if (vertices.empty())
    return std::make_pair(lower, upper);

  ConstVertexIteratorType vit = vertices.begin();
  upper = lower = viennagrid::point( *(vit++) );

  for (; vit != vertices.end(); ++vit)
  {
    lower = viennagrid::min(lower, viennagrid::point(*vit));
    upper = viennagrid::max(upper, viennagrid::point(*vit));
  }

  return std::make_pair(lower, upper);
}


template<typename PointT>
typename viennagrid::result_of::coord<PointT>::type size( std::pair<PointT, PointT> const & bounding_box )
{
  return viennagrid::norm_2(bounding_box.second - bounding_box.first);
}








template<typename SegmentT>
typename viennagrid::result_of::coord<SegmentT>::type interface_volume( SegmentT const & seg0, SegmentT const & seg1 )
{
  typedef typename viennagrid::result_of::const_facet_range<SegmentT>::type ConstFacetRangeType;
  typedef typename viennagrid::result_of::iterator<ConstFacetRangeType>::type ConstFacetIteratorType;

  typedef typename viennagrid::result_of::coord<SegmentT>::type CoordType;

  ConstFacetRangeType facets(seg0);
  CoordType length = 0;

  for (ConstFacetIteratorType fit = facets.begin(); fit != facets.end(); ++fit)
    if (is_boundary(seg1, *fit))
      length += viennagrid::volume(*fit);

  return length;
}







template<typename ConnectorElementTypeOrTagT, typename SegmentHandleT, typename ElementTagT, typename WrappedConfigT>
bool connects_to_segment( SegmentHandleT const & segment, viennagrid::element<ElementTagT, WrappedConfigT> const & element )
{
  typedef viennagrid::element<ElementTagT, WrappedConfigT> ElementType;
  typedef typename viennagrid::result_of::const_element_range<ElementType, ConnectorElementTypeOrTagT>::type ConstBoundaryElementRangeType;
  typedef typename viennagrid::result_of::iterator<ConstBoundaryElementRangeType>::type ConstBoundaryElementIteratorType;

  ConstBoundaryElementRangeType boundary_elements(element);
  for (ConstBoundaryElementIteratorType beit = boundary_elements.begin(); beit != boundary_elements.end(); ++beit)
  {
    if (viennagrid::is_in_segment(segment, *beit))
      return true;
  }

  return false;
}

template<typename SegmentHandleT, typename ElementTagT, typename WrappedConfigT>
bool connects_to_segment( SegmentHandleT const & segment, viennagrid::element<ElementTagT, WrappedConfigT> const & element )
{
  return connects_to_segment<viennagrid::vertex_tag>(segment, element);
}






template<typename SegmentT, typename FieldT>
typename FieldT::value_type arithmetic_average( SegmentT const & seg0, SegmentT const & seg1, FieldT const & field )
{
  typedef typename FieldT::access_type ElementType;
  typedef typename viennagrid::result_of::const_element_range<SegmentT, ElementType>::type ConstElementRangeType;
  typedef typename viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

  typename FieldT::value_type value = 0;
  int count = 0;

  ConstElementRangeType elements( seg0 );
  for (ConstElementIteratorType eit = elements.begin(); eit != elements.end(); ++eit)
  {
    if (viennagrid::is_interface(seg0, seg1, *eit))
    {
      value += field(*eit);
      ++count;
    }
  }

  return value/count;
}





template<typename SegmentT, typename FieldT, typename NumericT>
void set_on_interface( SegmentT const & seg0, SegmentT const & seg1, FieldT & field, NumericT value )
{
  typedef typename FieldT::access_type ElementType;
  typedef typename viennagrid::result_of::const_element_range<SegmentT, ElementType>::type ConstElementRangeType;
  typedef typename viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

  ConstElementRangeType elements( seg0 );
  for (ConstElementIteratorType eit = elements.begin(); eit != elements.end(); ++eit)
  {
    if (viennagrid::is_interface(seg0, seg1, *eit))
      field(*eit) = value;
  }
}







int main()
{
  // creating an algorithm for reading a mesh from a file
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );

  // creating a hull extraction algorithm
  viennamesh::algorithm_handle mesher( new viennamesh::triangle::algorithm() );





  typedef viennagrid::triangular_2d_mesh MeshType;
  typedef viennagrid::triangular_2d_segmentation SegmentationType;
  typedef viennagrid::triangular_2d_segment_handle SegmentHandleType;
  typedef viennagrid::result_of::point<MeshType>::type PointType;


  viennagrid::segmented_mesh<MeshType, SegmentationType> segmented_mesh;
  mesher->reference_output( "default", segmented_mesh );

  // linking the output from the reader to the mesher
  mesher->link_input( "default", reader, "default" );


  // Setting the filename for the reader and writer
  reader->set_input( "filename", "/export/florian/work/projects/2013_11 ViennaSHE Yannick/geometry.deva" );

//   mesher->set_input( "min_angle", 0.52 );     // minimum angle in radiant, 0.35 are about 20 degrees
//   mesher->set_input( "delaunay", true  );     // we want a Delaunay triangulation
//   mesher->set_input( "algorithm_type", "incremental_delaunay" );  // incremental Delaunay algorithm is used





  // start the algorithms
  reader->run();
  mesher->run();



  {
    viennagrid::io::vtk_writer<MeshType, SegmentationType> vtk_writer;
    vtk_writer( segmented_mesh.mesh, segmented_mesh.segmentation, "low_res_nld_mesh" );
  }








  MeshType & mesh = segmented_mesh.mesh;
  SegmentationType & segmentation = segmented_mesh.segmentation;


  int oxide1_id = 0;
  int silicon_id = 1;
//   int oxide2_id = 2;
  int gate_id = 3;
  int drain_id = 4;
  int bulk_id = 5;
  int source_id = 6;
  int substrate_id = 7;

  SegmentHandleType oxide1 = segmentation.get_segment(oxide1_id);  // oxide 1
  SegmentHandleType silicon = segmentation.get_segment(silicon_id);  // silicon
//   SegmentHandleType oxide2 = segmentation.get_segment(oxide2_id);  // oxide 2
  SegmentHandleType gate = segmentation.get_segment(gate_id);  // gate contact
  SegmentHandleType drain = segmentation.get_segment(drain_id);  // drain contact
  SegmentHandleType bulk = segmentation.get_segment(bulk_id);  // bulk contact
  SegmentHandleType source = segmentation.get_segment(source_id);  // source contact
  SegmentHandleType substrate = segmentation.get_segment(substrate_id);  // substrate contact

  // auflösen bei: contact-silicon-interfaces, über gate (im oxide und im silicon)


  std::pair<PointType, PointType> bb = bounding_box( mesh );
  double mesh_size = viennagrid::norm_2( bb.second - bb.first );

  double distance_gate_silicon = viennagrid::boundary_distance( gate, silicon );
//   double min_cell_size = distance_gate_silicon/2;

  std::cout << "Mesh size: " << mesh_size << std::endl;
  std::cout << "Distance gate-silicon: " << distance_gate_silicon << std::endl;

  typedef viennagrid::result_of::vertex<MeshType>::type VertexType;
  typedef viennagrid::result_of::cell<MeshType>::type CellType;

  typedef viennagrid::result_of::vertex_range<MeshType>::type VertexRangeType;
  typedef viennagrid::result_of::iterator<VertexRangeType>::type VertexIteratorType;

  typedef viennagrid::result_of::cell_range<MeshType>::type CellRangeType;
  typedef viennagrid::result_of::iterator<CellRangeType>::type CellIteratorType;




  viennamesh::algorithm_handle mesher_sizing_function( new viennamesh::triangle::algorithm() );

  mesher_sizing_function->link_input( "default", reader, "default" );

  viennagrid::segmented_mesh<MeshType, SegmentationType> segmented_sized_mesh;
  mesher_sizing_function->reference_output( "default", segmented_sized_mesh );







  viennagrid::triangular_2d_mesh doping_mesh;
  viennagrid::triangular_2d_segmentation doping_segmentation(doping_mesh);

  typedef viennagrid::result_of::accessor_container<VertexType, double, viennagrid::std_map_tag>::type VertexDoubleContainerType;
  typedef viennagrid::result_of::field<VertexDoubleContainerType, VertexType>::type VertexDoubleFieldType;

  VertexDoubleContainerType builtin_container;
  VertexDoubleFieldType builtin_field(builtin_container);

  VertexDoubleContainerType doping_n_container;
  VertexDoubleFieldType doping_n_field(doping_n_container);
  VertexDoubleContainerType doping_p_container;
  VertexDoubleFieldType doping_p_field(doping_p_container);

  VertexDoubleContainerType p_container;
  VertexDoubleFieldType p_field(p_container);
  VertexDoubleContainerType n_container;
  VertexDoubleFieldType n_field(n_container);
  VertexDoubleContainerType potential_container;
  VertexDoubleFieldType potential_field(potential_container);


  typedef viennagrid::result_of::accessor_container<CellType, double, viennagrid::std_map_tag>::type CellDoubleContainerType;
  typedef viennagrid::result_of::field<CellDoubleContainerType, CellType>::type CellDoubleFieldType;

  viennagrid::io::vtk_reader<viennagrid::triangular_2d_mesh> vtk_reader;
  viennagrid::io::add_scalar_data_on_vertices( vtk_reader, builtin_field, "builtin" );
  viennagrid::io::add_scalar_data_on_vertices( vtk_reader, doping_n_field, "doping_n" );
  viennagrid::io::add_scalar_data_on_vertices( vtk_reader, doping_p_field, "doping_p" );
  viennagrid::io::add_scalar_data_on_vertices( vtk_reader, p_field, "p" );
  viennagrid::io::add_scalar_data_on_vertices( vtk_reader, n_field, "n" );
  viennagrid::io::add_scalar_data_on_vertices( vtk_reader, potential_field, "potential" );

  vtk_reader(doping_mesh, doping_segmentation, "/export/florian/work/projects/2013_11 ViennaSHE Yannick/doping_mesh/Vg2,508_Vd0,1.devbz.vtu_main.pvd");

//   vtk_reader(doping_mesh, doping_segmentation, "/export/florian/work/projects/2013_11 ViennaSHE Yannick/doping_mesh_2/n53_0,85acc_out.devbz.vtu_main.pvd");


  double max_gradient = -1;

  CellRangeType cells(mesh);
  for (CellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
  {
    double current_gradient = viennamesh::gradient(*cit, builtin_field);
//     gradient_field(*cit) = current_gradient;
    max_gradient = std::max( max_gradient, current_gradient );
  }

  std::cout << "Max gradient = " << max_gradient << std::endl;



  double drain_silicon_interface_length = interface_volume( drain, silicon );
  double source_silicon_interface_length = interface_volume( source, silicon );
  double bulk_silicon_interface_length = interface_volume( bulk, silicon );

  viennamesh::sizing_function::sizing_function_2d_handle function =
    viennamesh::sizing_function::min(
      viennamesh::sizing_function::min(
        viennamesh::sizing_function::min(
          viennamesh::sizing_function::is_in_segments(
            viennamesh::sizing_function::linear_interpolate( viennamesh::sizing_function::mesh_gradient( doping_mesh, builtin_field ), 0, 2e5, mesh_size, distance_gate_silicon*10),
            silicon
          ),
          viennamesh::sizing_function::is_in_segments(
            viennamesh::sizing_function::is_near_double_interface( viennamesh::sizing_function::constant<PointType>(distance_gate_silicon), gate, silicon, 5*distance_gate_silicon),
            oxide1, silicon
          )
        ),

        viennamesh::sizing_function::min(
          viennamesh::sizing_function::linear_interpolate( viennamesh::sizing_function::distance_to_interface( drain, silicon ), distance_gate_silicon*2, mesh_size, drain_silicon_interface_length/10, mesh_size),
          viennamesh::sizing_function::min(
            viennamesh::sizing_function::linear_interpolate( viennamesh::sizing_function::distance_to_interface( source, silicon ), distance_gate_silicon*2, mesh_size, source_silicon_interface_length/10, mesh_size),
            viennamesh::sizing_function::linear_interpolate( viennamesh::sizing_function::distance_to_interface( bulk, silicon ), distance_gate_silicon*2, mesh_size, bulk_silicon_interface_length/10, mesh_size)
          )
        )
      ),
      viennamesh::sizing_function::constant<PointType>(distance_gate_silicon*100)
    );


  viennamesh::sizing_function_2d sizing_function = viennamesh::bind(viennamesh::sizing_function::get<viennamesh::sizing_function::base_sizing_function_2d>, function, _1);

  mesher_sizing_function->set_input( "sizing_function", sizing_function );

  mesher_sizing_function->set_input( "min_angle", 0.56 );     // minimum angle in radiant, 0.35 are about 20 degrees
  mesher_sizing_function->set_input( "delaunay", true  );     // we want a Delaunay triangulation
  mesher_sizing_function->set_input( "algorithm_type", "incremental_delaunay" );  // incremental Delaunay algorithm is used

  mesher_sizing_function->run();









  VertexDoubleContainerType sized_doping_n_container;
  VertexDoubleFieldType sized_doping_n_field(sized_doping_n_container, 0.0);
  VertexDoubleContainerType sized_doping_p_container;
  VertexDoubleFieldType sized_doping_p_field(sized_doping_p_container, 0.0);
  VertexDoubleContainerType sized_netto_doping_container;
  VertexDoubleFieldType sized_netto_doping_field(sized_netto_doping_container, 0.0);

  VertexDoubleContainerType sized_n_container;
  VertexDoubleFieldType sized_n_field(sized_n_container, 0.0);
  VertexDoubleContainerType sized_p_container;
  VertexDoubleFieldType sized_p_field(sized_p_container, 0.0);
  VertexDoubleContainerType sized_potential_container;
  VertexDoubleFieldType sized_potential_field(sized_potential_container, 0.0);

  typedef viennagrid::result_of::vertex_range<SegmentHandleType>::type VertexOnSegmentRangeType;
  typedef viennagrid::result_of::iterator<VertexOnSegmentRangeType>::type VertexOnSegmentIteratorType;



  viennamesh::sizing_function::sizing_function_2d_handle doping_n_interpolate = viennamesh::sizing_function::mesh_interpolate( doping_mesh, doping_n_field );
  viennamesh::sizing_function::sizing_function_2d_handle doping_p_interpolate = viennamesh::sizing_function::mesh_interpolate( doping_mesh, doping_p_field );

  viennamesh::sizing_function::sizing_function_2d_handle n_interpolate = viennamesh::sizing_function::mesh_interpolate( doping_mesh, n_field );
  viennamesh::sizing_function::sizing_function_2d_handle p_interpolate = viennamesh::sizing_function::mesh_interpolate( doping_mesh, p_field );
  viennamesh::sizing_function::sizing_function_2d_handle potential_interpolate = viennamesh::sizing_function::mesh_interpolate( doping_mesh, potential_field );


  VertexRangeType vertices( segmented_sized_mesh.mesh );
  for (VertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
    sized_potential_field(*vit) = potential_interpolate->get( viennagrid::point(*vit) );

  VertexOnSegmentRangeType vertices_on_silicon( segmented_sized_mesh.segmentation(1) );
  for (VertexOnSegmentIteratorType vit = vertices_on_silicon.begin(); vit != vertices_on_silicon.end(); ++vit)
  {
    double doping_n = doping_n_interpolate->get( viennagrid::point(*vit) );
    double doping_p = doping_p_interpolate->get( viennagrid::point(*vit) );

    sized_doping_n_field(*vit) = doping_n;
    sized_doping_p_field(*vit) = doping_p;
//     sized_netto_doping_field(*vit) = doping_p - doping_n;

    sized_n_field(*vit) = n_interpolate->get( viennagrid::point(*vit) );
    sized_p_field(*vit) = p_interpolate->get( viennagrid::point(*vit) );
  }


  set_on_interface( source, silicon, sized_doping_n_field, arithmetic_average(source, silicon, sized_doping_n_field) );
  set_on_interface( source, silicon, sized_doping_p_field, arithmetic_average(source, silicon, sized_doping_p_field) );
  set_on_interface( source, silicon, sized_potential_field, arithmetic_average(source, silicon, sized_potential_field) );

  set_on_interface( drain, silicon, sized_doping_n_field, arithmetic_average(drain, silicon, sized_doping_n_field) );
  set_on_interface( drain, silicon, sized_doping_p_field, arithmetic_average(drain, silicon, sized_doping_p_field) );
  set_on_interface( drain, silicon, sized_potential_field, arithmetic_average(drain, silicon, sized_potential_field) );

  set_on_interface( bulk, silicon, sized_doping_n_field, arithmetic_average(bulk, silicon, sized_doping_n_field) );
  set_on_interface( bulk, silicon, sized_doping_p_field, arithmetic_average(bulk, silicon, sized_doping_p_field) );
  set_on_interface( bulk, silicon, sized_potential_field, arithmetic_average(bulk, silicon, sized_potential_field) );

  set_on_interface( substrate, silicon, sized_doping_n_field, arithmetic_average(substrate, silicon, sized_doping_n_field) );
  set_on_interface( substrate, silicon, sized_doping_p_field, arithmetic_average(substrate, silicon, sized_doping_p_field) );
  set_on_interface( substrate, silicon, sized_potential_field, arithmetic_average(substrate, silicon, sized_potential_field) );



  for (VertexOnSegmentIteratorType vit = vertices_on_silicon.begin(); vit != vertices_on_silicon.end(); ++vit)
    sized_netto_doping_field(*vit) = sized_doping_p_field(*vit) - sized_doping_n_field(*vit);




  {
    viennagrid::io::vtk_writer<MeshType, SegmentationType> vtk_writer;

    viennagrid::io::add_scalar_data_on_vertices( vtk_writer, sized_doping_n_field, "doping_n" );
    viennagrid::io::add_scalar_data_on_vertices( vtk_writer, sized_doping_p_field, "doping_p" );
    viennagrid::io::add_scalar_data_on_vertices( vtk_writer, sized_netto_doping_field, "netto_doping_field" );

    viennagrid::io::add_scalar_data_on_vertices( vtk_writer, sized_n_field, "n" );
    viennagrid::io::add_scalar_data_on_vertices( vtk_writer, sized_p_field, "p" );
    viennagrid::io::add_scalar_data_on_vertices( vtk_writer, sized_potential_field, "potential" );

    vtk_writer( segmented_sized_mesh.mesh, segmented_sized_mesh.segmentation, "meshed_nld_sizing_field" );
  }




}
