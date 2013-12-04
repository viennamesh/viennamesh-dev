#include "viennamesh/algorithm/viennagrid.hpp"
#include "viennamesh/algorithm/io.hpp"
#include "viennamesh/algorithm/triangle.hpp"

#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/interface.hpp"
#include "viennagrid/algorithm/distance.hpp"
#include <boost/concept_check.hpp>






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
  upper = lower = viennagrid::point(*vit++);

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







int main()
{
  // creating an algorithm for reading a mesh from a file
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );

  // creating a hull extraction algorithm
  viennamesh::algorithm_handle mesher( new viennamesh::triangle::algorithm() );

//   // creating an algorithm for writing a mesh to a file
//   viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );





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

  viennamesh::sizing_function_2d sizing_function = my_sizing;
  mesher->set_input( "sizing_function", sizing_function );
  mesher->set_input( "min_angle", 0.35 );     // minimum angle in radiant, 0.35 are about 20 degrees
  mesher->set_input( "delaunay", true  );     // we want a Delaunay triangulation
  mesher->set_input( "algorithm_type", "incremental_delaunay" );  // incremental Delaunay algorithm is used

  // start the algorithms
  reader->run();
  mesher->run();



  MeshType & mesh = segmented_mesh.mesh;
  SegmentationType & segmentation = segmented_mesh.segmentation;



  SegmentHandleType oxide1 = segmentation.get_segment(0);  // oxide 1
  SegmentHandleType silicon = segmentation.get_segment(1);  // silicon
  SegmentHandleType oxide2 = segmentation.get_segment(2);  // oxide 2
  SegmentHandleType gate = segmentation.get_segment(3);  // gate contact
  SegmentHandleType drain = segmentation.get_segment(4);  // drain contact
  SegmentHandleType bulk = segmentation.get_segment(5);  // bulk contact
  SegmentHandleType source = segmentation.get_segment(6);  // source contact
  SegmentHandleType substrate = segmentation.get_segment(7);  // substrate contact

  // auflösen bei: contact-silicon-interfaces, über gate (im oxide und im silicon)


  std::pair<PointType, PointType> bb = bounding_box( mesh );
  double mesh_size = viennagrid::norm_2( bb.second - bb.first );

  std::cout << "Mesh size: " << mesh_size << std::endl;

  typedef viennagrid::result_of::vertex<MeshType>::type VertexType;
  typedef viennagrid::result_of::cell<MeshType>::type CellType;

  typedef viennagrid::result_of::accessor_container<CellType, double, viennagrid::std_map_tag>::type CellSizeContainerType;
  typedef viennagrid::result_of::field<CellSizeContainerType, CellType>::type CellSizeFieldType;

  typedef viennagrid::result_of::vertex_range<MeshType>::type VertexRangeType;
  typedef viennagrid::result_of::iterator<VertexRangeType>::type VertexIteratorType;

  typedef viennagrid::result_of::line_range<MeshType>::type LineRangeType;
  typedef viennagrid::result_of::iterator<LineRangeType>::type LineIteratorType;

  typedef viennagrid::result_of::cell_range<MeshType>::type CellRangeType;
  typedef viennagrid::result_of::iterator<CellRangeType>::type CellIteratorType;



  CellSizeContainerType sizes;
  CellSizeFieldType size_field(sizes);

  VertexRangeType vertices(mesh);
  LineRangeType lines(mesh);
  CellRangeType cells(mesh);



  for (CellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
    size_field(*cit) = mesh_size;


  for (CellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
  {
    if ( viennagrid::is_in_segment( oxide1, *cit ) )
    {
      bool connects_to_gate = connects_to_segment(gate, *cit);
      bool connects_to_silicon = connects_to_segment(silicon, *cit);
      if (connects_to_gate && connects_to_silicon)
        size_field(*cit) = size( bounding_box(*cit) );
    }


  }





//   for (LineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
//   {
//     if ( viennagrid::is_interface(seg0, seg3, *lit) )
//     {
//       double smallest_distance = 1000000.0;
//
//       for (LineIteratorType lit2 = lines.begin(); lit2 != lines.end(); ++lit2)
//       {
//         if ( viennagrid::is_interface(seg0, seg1, *lit2) )
//         {
//           double current_distance = viennagrid::distance( *lit, *lit2 );
//           if (current_distance < smallest_distance)
//             smallest_distance = current_distance;
//         }
//       }
//
//       size_field( viennagrid::vertices(*lit)[0] ) = std::min( size_field( viennagrid::vertices(*lit)[0] ), smallest_distance );
//       size_field( viennagrid::vertices(*lit)[1] ) = std::min( size_field( viennagrid::vertices(*lit)[1] ), smallest_distance );
//     }
//   }
//
//
//   for (LineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
//   {
//     if ( viennagrid::is_interface(seg0, seg1, *lit) )
//     {
//       double smallest_distance = 1000000.0;
//
//       for (LineIteratorType lit2 = lines.begin(); lit2 != lines.end(); ++lit2)
//       {
//         if ( viennagrid::is_interface(seg0, seg3, *lit2) )
//         {
//           double current_distance = viennagrid::distance( *lit, *lit2 );
//           if (current_distance < smallest_distance)
//             smallest_distance = current_distance;
//         }
//       }
//
//       size_field( viennagrid::vertices(*lit)[0] ) = std::min( size_field( viennagrid::vertices(*lit)[0] ), smallest_distance );
//       size_field( viennagrid::vertices(*lit)[1] ) = std::min( size_field( viennagrid::vertices(*lit)[1] ), smallest_distance );
//     }
//   }







  viennagrid::io::vtk_writer<MeshType, SegmentationType> writer;
  viennagrid::io::add_scalar_data_on_cells( writer, size_field, "size" );

  writer( mesh, segmentation, "meshed_nld" );




}
