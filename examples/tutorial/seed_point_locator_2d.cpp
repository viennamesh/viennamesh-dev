#include "viennamesh/algorithm/seed_point_locator.hpp"
#include "viennamesh/algorithm/file_reader.hpp"
#include "viennamesh/algorithm/file_writer.hpp"


int main()
{
  typedef viennagrid::plc_2d_mesh GeometryMeshType;

  typedef viennagrid::result_of::point<GeometryMeshType>::type PointType;
  typedef viennagrid::result_of::vertex_handle<GeometryMeshType>::type GeometryVertexHandle;
  typedef viennagrid::result_of::line_handle<GeometryMeshType>::type GeometryLineHandle;

  // creating the geometry mesh
  viennamesh::result_of::parameter_handle< GeometryMeshType >::type geometry = viennamesh::make_parameter<GeometryMeshType>();

  double s = 10.0;
  GeometryVertexHandle vtx[4];
  GeometryLineHandle lines[4];

  vtx[0] = viennagrid::make_vertex( geometry->get(), PointType(0, 0) );
  vtx[1] = viennagrid::make_vertex( geometry->get(), PointType(0, s) );
  vtx[2] = viennagrid::make_vertex( geometry->get(), PointType(s, 0) );
  vtx[3] = viennagrid::make_vertex( geometry->get(), PointType(s, s) );

  lines[0] = viennagrid::make_line( geometry->get(), vtx[0], vtx[1] );
  lines[1] = viennagrid::make_line( geometry->get(), vtx[1], vtx[3] );
  lines[2] = viennagrid::make_line( geometry->get(), vtx[3], vtx[2] );
  lines[3] = viennagrid::make_line( geometry->get(), vtx[2], vtx[0] );

  viennagrid::make_plc( geometry->get(), lines+0, lines+4 );



  // creating the seed point locator algorithm
  viennamesh::algorithm_handle seed_point_locator( new viennamesh::seed_point_locator::algorithm() );


  seed_point_locator->set_input( "default", geometry );
  seed_point_locator->run();

  viennamesh::result_of::parameter_handle<PointType>::type point = seed_point_locator->get_output<PointType>( "default" );
  if (point)
    std::cout << point->get() << std::endl;
}
