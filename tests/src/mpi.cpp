

/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at
               Jorge Rodríguez                       rcabrera.jorge@gmail.com

   license:    see file LICENSE in the base directory
============================================================================= */

// *** system includes
#include <iostream>

// *** viennamesh includes
#include "viennamesh/common.hpp"
#include "viennamesh/adaptation.hpp"
#include "viennamesh/mpi.hpp"

// *** vienna includes
#include "viennagrid/domain.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/algorithm/refine.hpp"
#include "viennautils/file.hpp"

int main(int argc, char *argv[])
{
  // initial MPI configuration
  boost::mpi::environment env(argc, argv);
  boost::mpi::communicator world;

  if(argc != 2)
  {
    std::cerr << "## Error::Parameter - usage: " << argv[0] << " inputfile" << std::endl;
    std::cerr << "## shutting down .." << std::endl;
    return -1;
  }

  std::string inputfile(argv[1]);
  std::string extension  = viennautils::file_extension(inputfile);

  if(extension != "bnd")
  {
    std::cerr << "## Error::wrong input file type - only BND is supported .." << std::endl;
    std::cerr << "## shutting down .." << std::endl;
    return -1;
  }

  // read the input geometry
  //
  viennautils::io::bnd_reader bndreader;
  bndreader(inputfile); 

  // prepare the functors
  //
  typedef viennagrid::result_of::domain<viennagrid::config::triangular_3d>::type      HullDomain;
  typedef boost::shared_ptr<HullDomain>                                               HullDomainSP;
  typedef viennagrid::result_of::domain<viennagrid::config::tetrahedral_3d>::type     VolDomain;

  typedef viennamesh::wrapper<viennamesh::tag::bnd, viennautils::io::bnd_reader>      Wrapper;
  Wrapper     wrapped_data(bndreader);      
  typedef viennamesh::result_of::mesh_generator<viennamesh::tag::cervpt>::type        HullMesher;
  HullMesher  hullmesher;                     
  typedef viennamesh::result_of::mesh_adaptor<viennamesh::tag::orienter>::type        Orienter;
  Orienter    orienter;

  // generate some initial hull mesh
  //
  HullDomainSP hull_domainsp = orienter(hullmesher(wrapped_data));
  
  // uniformly refine the hull mesh
  //
  HullDomain refined_hull_domain1 = viennagrid::refine_uniformly(*hull_domainsp);
  HullDomain refined_hull_domain2 = viennagrid::refine_uniformly(refined_hull_domain1);
  HullDomain refined_hull_domain3 = viennagrid::refine_uniformly(refined_hull_domain2);

  // do parallel volume meshing 
  //
  VolDomain voldomain;
  viennamesh::mpi::mesh(world, refined_hull_domain3, voldomain);

  return 0;
}




