#include "viennamesh/viennamesh.hpp"


int main()
{
  // creating an algorithm for reading a mesh from a file
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );

  // Setting the filename for the reader
  reader->set_input( "filename", "/export/florian/work/projects/2013_11 ViennaSHE Yannick/mesh/ridiculously_fine_ortho_for_florian_out.devbz.vtu_main.pvd" );

  // start the algorithm
  reader->run();


  // creating a hull extraction algorithm
  viennamesh::algorithm_handle extract_hull( new viennamesh::extract_hull() );
  extract_hull->set_default_source(reader);

  // start the algorithm
  extract_hull->run();


  // creating an algorithm for writing a mesh to a file
  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );
  writer->set_default_source(extract_hull);

  // Setting the filename for writer
  writer->set_input( "filename", "nld_mosfet_hull.vtu" );

  // start the algorithm
  writer->run();

  typedef viennamesh::point_3d PointType;
  typedef viennamesh::result_of::seed_point_container<PointType>::type SeedPointContainerType;
  viennamesh::result_of::parameter_handle<SeedPointContainerType>::type point_container = extract_hull->get_output<SeedPointContainerType>( "seed_points" );
  if (point_container)
  {
    std::cout << "Number of extracted seed points: " << point_container().size() << std::endl;
    for (SeedPointContainerType::iterator it = point_container().begin(); it != point_container().end(); ++it)
      std::cout << "  " << it->first << " " << it->second << std::endl;
  }


}
