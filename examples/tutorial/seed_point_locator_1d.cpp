#include "viennamesh/algorithm/seed_point_locator.hpp"


int main()
{
  typedef viennagrid::vertex_1d_mesh GeometryMeshType;
  // Typedefing vertex handle and point type for geometry creation
  typedef viennagrid::result_of::point<GeometryMeshType>::type PointType;
  typedef viennagrid::result_of::vertex_handle<GeometryMeshType>::type GeometryVertexHandle;

  // creating the geometry mesh
  viennamesh::result_of::parameter_handle< GeometryMeshType >::type geometry_handle = viennamesh::make_parameter<GeometryMeshType>();
  GeometryMeshType & geometry = geometry_handle();

  double s = 10.0;
  viennagrid::make_vertex( geometry, PointType(s) );
  viennagrid::make_vertex( geometry, PointType(-s) );
  viennagrid::make_vertex( geometry, PointType(0) );
  viennagrid::make_vertex( geometry, PointType(0) );
  viennagrid::make_vertex( geometry, PointType(0) );
  viennagrid::make_vertex( geometry, PointType(2*s) );
  viennagrid::make_vertex( geometry, PointType(3*s) );




  // creating the seed point locator algorithm
  viennamesh::algorithm_handle seed_point_locator( new viennamesh::seed_point_locator::algorithm() );


  seed_point_locator->set_input( "default", geometry_handle );

  viennamesh::point_1d_container hole_points;
  hole_points.push_back( PointType(s/2) );
  seed_point_locator->set_input( "hole_points", hole_points );

  seed_point_locator->run();

  typedef viennamesh::result_of::point_container<PointType>::type PointContainerType;
  viennamesh::result_of::parameter_handle<PointContainerType>::type point_container = seed_point_locator->get_output<PointContainerType>( "default" );
  if (point_container)
  {
    std::cout << "Number of extracted seed points: " << point_container().size() << std::endl;
    for (PointContainerType::iterator it = point_container().begin(); it != point_container().end(); ++it)
      std::cout << "  " << *it << std::endl;
  }
}
