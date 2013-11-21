#include "viennamesh/algorithm/seed_point_locator.hpp"
#include "viennamesh/algorithm/file_reader.hpp"
#include "viennamesh/algorithm/file_writer.hpp"


int main()
{
  typedef viennagrid::brep_2d_mesh GeometryMeshType;

  typedef viennagrid::result_of::point<GeometryMeshType>::type PointType;
  typedef viennagrid::result_of::vertex_handle<GeometryMeshType>::type GeometryVertexHandle;
  typedef viennagrid::result_of::line_handle<GeometryMeshType>::type GeometryLineHandle;

  // creating the geometry mesh
  viennamesh::result_of::parameter_handle< GeometryMeshType >::type geometry_handle = viennamesh::make_parameter<GeometryMeshType>();
  GeometryMeshType & geometry = geometry_handle->value();


  double s = 10.0;
  GeometryVertexHandle vtx[8];

  vtx[0] = viennagrid::make_vertex( geometry, PointType(0, 0) );
  vtx[1] = viennagrid::make_vertex( geometry, PointType(0, s) );
  vtx[2] = viennagrid::make_vertex( geometry, PointType(s, 0) );
  vtx[3] = viennagrid::make_vertex( geometry, PointType(s, s) );

  vtx[4] = viennagrid::make_vertex( geometry, PointType(3*s+0, 0) );
  vtx[5] = viennagrid::make_vertex( geometry, PointType(3*s+0, s) );
  vtx[6] = viennagrid::make_vertex( geometry, PointType(3*s+s, 0) );
  vtx[7] = viennagrid::make_vertex( geometry, PointType(3*s+s, s) );

  viennagrid::make_line( geometry, vtx[0], vtx[1] );
  viennagrid::make_line( geometry, vtx[1], vtx[3] );
  viennagrid::make_line( geometry, vtx[3], vtx[2] );
  viennagrid::make_line( geometry, vtx[2], vtx[0] );

  viennagrid::make_line( geometry, vtx[4], vtx[5] );
  viennagrid::make_line( geometry, vtx[5], vtx[7] );
  viennagrid::make_line( geometry, vtx[7], vtx[6] );
  viennagrid::make_line( geometry, vtx[6], vtx[4] );

  viennagrid::make_line( geometry, vtx[2], vtx[4] );
  viennagrid::make_line( geometry, vtx[3], vtx[5] );


  // creating the seed point locator algorithm
  viennamesh::algorithm_handle seed_point_locator( new viennamesh::seed_point_locator::algorithm() );


  seed_point_locator->set_input( "default", geometry_handle );

  viennamesh::point_2d_container hole_points;
  hole_points.push_back( PointType(s+s/2, s/2) );
  seed_point_locator->set_input( "hole_points", hole_points );

  seed_point_locator->run();

  typedef viennamesh::result_of::point_container<PointType>::type PointContainerType;
  viennamesh::result_of::parameter_handle<PointContainerType>::type point_container = seed_point_locator->get_output<PointContainerType>( "default" );
  if (point_container)
  {
    std::cout << "Number of extracted seed points: " << point_container->value().size() << std::endl;
    for (PointContainerType::iterator it = point_container->value().begin(); it != point_container->value().end(); ++it)
      std::cout << "  " << *it << std::endl;
  }
}
