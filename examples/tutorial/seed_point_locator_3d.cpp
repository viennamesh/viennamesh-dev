#include "viennamesh/algorithm/seed_point_locator.hpp"
#include "viennamesh/algorithm/file_reader.hpp"
#include "viennamesh/algorithm/file_writer.hpp"


int main()
{
  typedef viennagrid::plc_3d_mesh GeometryMeshType;

  typedef viennagrid::result_of::point<GeometryMeshType>::type PointType;
  typedef viennagrid::result_of::vertex_handle<GeometryMeshType>::type GeometryVertexHandle;
  typedef viennagrid::result_of::line_handle<GeometryMeshType>::type GeometryLineHandle;

  // creating the geometry mesh
  viennamesh::result_of::parameter_handle< GeometryMeshType >::type geometry = viennamesh::make_parameter<GeometryMeshType>();

  double s = 10.0;
  GeometryVertexHandle vtx[8];
  GeometryLineHandle lines[12];

  vtx[0] = viennagrid::make_vertex( geometry->get(), PointType(0, 0, 0) );
  vtx[1] = viennagrid::make_vertex( geometry->get(), PointType(0, s, 0) );
  vtx[2] = viennagrid::make_vertex( geometry->get(), PointType(s, 0, 0) );
  vtx[3] = viennagrid::make_vertex( geometry->get(), PointType(s, s, 0) );

  vtx[4] = viennagrid::make_vertex( geometry->get(), PointType(0, 0, s) );
  vtx[5] = viennagrid::make_vertex( geometry->get(), PointType(0, s, s) );
  vtx[6] = viennagrid::make_vertex( geometry->get(), PointType(s, 0, s) );
  vtx[7] = viennagrid::make_vertex( geometry->get(), PointType(s, s, s) );


  // bottom 4 lines
  lines[0] = viennagrid::make_line( geometry->get(), vtx[0], vtx[1] );
  lines[1] = viennagrid::make_line( geometry->get(), vtx[1], vtx[3] );
  lines[2] = viennagrid::make_line( geometry->get(), vtx[3], vtx[2] );
  lines[3] = viennagrid::make_line( geometry->get(), vtx[2], vtx[0] );

  // top 4 lines
  lines[4] = viennagrid::make_line( geometry->get(), vtx[4], vtx[5] );
  lines[5] = viennagrid::make_line( geometry->get(), vtx[5], vtx[7] );
  lines[6] = viennagrid::make_line( geometry->get(), vtx[7], vtx[6] );
  lines[7] = viennagrid::make_line( geometry->get(), vtx[6], vtx[4] );

  // columns
  lines[8] = viennagrid::make_line( geometry->get(), vtx[0], vtx[4] );
  lines[9] = viennagrid::make_line( geometry->get(), vtx[1], vtx[5] );
  lines[10] = viennagrid::make_line( geometry->get(), vtx[2], vtx[6] );
  lines[11] = viennagrid::make_line( geometry->get(), vtx[3], vtx[7] );



  viennagrid::make_plc( geometry->get(), lines+0, lines+4 );
  viennagrid::make_plc( geometry->get(), lines+4, lines+8 );

  {
    GeometryLineHandle cur_lines[4];
    cur_lines[0] = lines[0];
    cur_lines[1] = lines[9];
    cur_lines[2] = lines[4];
    cur_lines[3] = lines[8];
    viennagrid::make_plc( geometry->get(), cur_lines+0, cur_lines+4 );
  }

  {
    GeometryLineHandle cur_lines[4];
    cur_lines[0] = lines[2];
    cur_lines[1] = lines[11];
    cur_lines[2] = lines[6];
    cur_lines[3] = lines[10];
    viennagrid::make_plc( geometry->get(), cur_lines+0, cur_lines+4 );
  }

  {
    GeometryLineHandle cur_lines[4];
    cur_lines[0] = lines[3];
    cur_lines[1] = lines[10];
    cur_lines[2] = lines[7];
    cur_lines[3] = lines[8];
    viennagrid::make_plc( geometry->get(), cur_lines+0, cur_lines+4 );
  }

  {
    GeometryLineHandle cur_lines[4];
    cur_lines[0] = lines[1];
    cur_lines[1] = lines[11];
    cur_lines[2] = lines[5];
    cur_lines[3] = lines[9];
    viennagrid::make_plc( geometry->get(), cur_lines+0, cur_lines+4 );
  }



  // creating the seed point locator algorithm
  viennamesh::algorithm_handle seed_point_locator( new viennamesh::seed_point_locator::algorithm() );


  seed_point_locator->set_input( "default", geometry );
  seed_point_locator->run();

  typedef viennamesh::result_of::point_container<PointType>::type PointContainerType;
  viennamesh::result_of::parameter_handle<PointContainerType>::type point_container = seed_point_locator->get_output<PointContainerType>( "default" );
  if (point_container)
  {
    std::cout << "Number of extracted seed points: " << point_container->get().size() << std::endl;
    for (PointContainerType::iterator it = point_container->get().begin(); it != point_container->get().end(); ++it)
      std::cout << "  " << *it << std::endl;
  }
}
