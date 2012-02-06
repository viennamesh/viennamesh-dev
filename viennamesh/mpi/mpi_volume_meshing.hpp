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

#ifndef VIENNAMESH_MPI_MPIVOLUMEMESHING_HPP
#define VIENNAMESH_MPI_MPIVOLUMEMESHING_HPP


#include <iostream>
#include <fstream>
#include <functional>
#include <vector>
#include <numeric>

#include <boost/mpi.hpp>
#include <boost/serialization/vector.hpp>

#include "viennamesh/common.hpp"
#include "viennamesh/wrapper.hpp"
#include "viennamesh/generation.hpp"

#include "viennautils/timer.hpp"
#include "viennagrid/domain.hpp"
#include "viennagrid/config/simplex.hpp"

// Constants for the MPI_Packet CMD field
#define CMD_INITIAL     -1    // Initial message
#define CMD_SEGINITIAL  -2    // Segment information
#define CMD_GEO         -3    // Geometric information
#define CMD_TOPO        -4    // Topographical information
#define CMD_FINISH      -5    // Communication finished
#define CMD_INVALID     -10   // Invalid CMD

/*
////////////////////
//
// TO DO:
// * - revisit segment merger in root: make it faster(possible?!) 
// * - make optional geometry unification at segment merger
// * - Merge segmentSend and volume_segmentSend (only difference: config_type)
//
////////////////////
*/

#define VMPI_DEBUG

namespace viennamesh {
namespace mpi {
namespace detail {

namespace mpi = boost::mpi;

//// Datastructure used to send and receive communications
template <typename T>
struct MPI_Packet {
  int cmd;                    // Type of communication
  std::vector<T> container;   // Container with the information
 
  // Constructor
  MPI_Packet(int cmd = 0) { setCMD(cmd); }  
  
  // Public methods
  void setCMD(int cmd) { this->cmd = cmd; }
  void clear() { this->container.clear(); this->cmd = CMD_INVALID; }
  
  // Alias to access the information
  T & operator[] (int index) { return this->container[index]; }
  
  // Serialization using Boost.serialization
  private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & container;
        ar & cmd;
    }
};

// Auxiliar function to check if two points are the same
template <typename T>
inline bool isSamePoint(T p1, T p2)
{
  return (fabs(p2[0]-p1[0]) < std::numeric_limits<double>::epsilon() &&
          fabs(p2[1]-p1[1]) < std::numeric_limits<double>::epsilon() &&
          fabs(p2[2]-p1[2]) < std::numeric_limits<double>::epsilon()
          );
}


// It can be much more optimized!
// Parameter type to be changed!! (we should only pass segments container, not the whole domain)
void sortIndices(int *indArray, viennagrid::result_of::domain<viennagrid::config::triangular_3d>::type& domain)
{
  typedef viennagrid::config::triangular_3d                                                           config_type;
  typedef config_type::cell_tag							  											                                  cell_tag;
  
  int backup = -1;
  for (int i=0; i<domain.segments().size(); i++) indArray[i] = i;
  
  for (int out=domain.segments().size()-1; out>1; out--) {
    for (int in=0; in<out; in++) {
    
    int ncells1 = viennagrid::ncells<cell_tag::dim>(domain.segments()[indArray[in]]).size();
    int ncells2 = viennagrid::ncells<cell_tag::dim>(domain.segments()[indArray[in+1]]).size();
    
      if (ncells1 < ncells2) {
        backup = indArray[in];
        indArray[in] = indArray[in+1];
        indArray[in+1] = backup;
      }
      
    }
  }
}

// Auxiliar function to send segments (root --> nodes)
void segmentSend(mpi::communicator &comm,
                  int destination,
                  viennagrid::result_of::domain<viennagrid::config::triangular_3d>::type& domain,
                  size_t si)
{	
  typedef viennagrid::config::triangular_3d                                                           config_type;
  typedef viennagrid::result_of::domain<config_type>::type                                            domain_type;
  
	typedef config_type::cell_tag							  											                                  cell_tag;
	typedef domain_type::segment_type                                             						          segment_type;

	typedef viennagrid::result_of::ncell<config_type, cell_tag::dim>::type				      CellType;
	typedef viennagrid::result_of::ncell<config_type, 0>::type										                  VertexType;

	typedef viennagrid::result_of::const_ncell_range<segment_type, cell_tag::dim>::type  CellContainer;
	typedef viennagrid::result_of::iterator<CellContainer>::type               							            CellIterator;
	
	typedef viennagrid::result_of::const_ncell_range<segment_type, 0>::type  							          VertexContainer;
	typedef viennagrid::result_of::iterator<VertexContainer>::type               							          VertexIterator;

	typedef viennagrid::result_of::const_ncell_range<CellType, 0>::type  							              VertexOnCellContainer;
	typedef viennagrid::result_of::iterator<VertexOnCellContainer>::type         					              VertexOnCellIterator;

  // Datastructure with information to send
  MPI_Packet<double> MPI_info(CMD_INVALID);

  // Extract the current segment object
  segment_type const & seg = domain.segments()[si];

  // Extract the segment specific cells and vertices
  CellContainer cells = viennagrid::ncells<cell_tag::dim>(seg);
  VertexContainer vertices = viennagrid::ncells<0>(seg);
  
  // [JW] added number of cells output to verify that the segment with the largest 
  // number of cells is processed first
#ifdef VMPI_DEBUG
  std::cout << "[0] sending segment: " << si << " - vertices: " << viennagrid::ncells<0>(seg).size() << std::endl;
  std::cout << "[0] sending segment: " << si << " - cells: " << viennagrid::ncells<cell_tag::dim>(seg).size() << std::endl;
#endif

  // Firstly, add type of the communication and the number of the segment
  MPI_info.clear();
  MPI_info.setCMD(CMD_SEGINITIAL);
  MPI_info.container.push_back(si);
  
  // Add geometrical info of every vertex
  MPI_info.container.push_back(CMD_GEO);
  for (VertexIterator vrt = vertices.begin(); vrt != vertices.end(); vrt++) {
    MPI_info.container.push_back(vrt->id());         // Vertex ID
    MPI_info.container.push_back(vrt->point()[0]);   // X
    MPI_info.container.push_back(vrt->point()[1]);   // Y
    MPI_info.container.push_back(vrt->point()[2]);   // Z
  }
  
  // Add topological info (cells)
  MPI_info.container.push_back(CMD_TOPO);
  for(CellIterator cll = cells.begin(); cll != cells.end(); cll++)
  {    
    // Extract the cell specific vertices
    VertexOnCellContainer cellVertices = viennagrid::ncells<0>(*cll);	  
    
    // Traverse the vertices and save their IDs in the MPI packet
    for (VertexOnCellIterator vrt = cellVertices.begin(); vrt != cellVertices.end(); vrt++) {
      MPI_info.container.push_back(vrt->id());			      
    }
  }
  
  // Send the segment
  comm.send(destination, 0, MPI_info);
}

// Auxiliar function to send segments (nodes --> root)
void volume_segmentSend(mpi::communicator &comm,
                  int destination,
                  viennagrid::result_of::domain<viennagrid::config::tetrahedral_3d>::type& domain,
                  size_t si)
{
	typedef viennagrid::config::tetrahedral_3d                                                           config_type;
  typedef viennagrid::result_of::domain<config_type>::type                                                             domain_type;
  
	typedef config_type::cell_tag							  											                                  cell_tag;
	typedef viennagrid::result_of::segment<config_type>::type        segment_type;

	typedef viennagrid::result_of::ncell<config_type, cell_tag::dim>::type				      CellType;
	typedef viennagrid::result_of::ncell<config_type, 0>::type										                  VertexType;

	typedef viennagrid::result_of::const_ncell_range<segment_type, cell_tag::dim>::type  CellContainer;
	typedef viennagrid::result_of::iterator<CellContainer>::type               							            CellIterator;
	
	typedef viennagrid::result_of::const_ncell_range<segment_type, 0>::type  							          VertexContainer;
	typedef viennagrid::result_of::iterator<VertexContainer>::type               							          VertexIterator;

	typedef viennagrid::result_of::const_ncell_range<CellType, 0>::type  							              VertexOnCellContainer;
	typedef viennagrid::result_of::iterator<VertexOnCellContainer>::type         					              VertexOnCellIterator;

  // Datastructure with information to send
  MPI_Packet<double> MPI_info(CMD_INVALID);

  // Extract the current segment object
  segment_type & seg = domain.segments()[0];

  // Extract the segment specific cells and vertices
  CellContainer cells = viennagrid::ncells<cell_tag::dim>(seg);
  VertexContainer vertices = viennagrid::ncells<0>(seg);
  
  // Firstly, add type of the communication and the number of the segment
  MPI_info.clear();
  MPI_info.setCMD(CMD_SEGINITIAL);
  MPI_info.container.push_back(si);
  
  // Add geometrical info of every vertex
  MPI_info.container.push_back(CMD_GEO);
  for (VertexIterator vrt = vertices.begin(); vrt != vertices.end(); vrt++) {
    MPI_info.container.push_back(vrt->id());         // Vertex ID
    MPI_info.container.push_back(si);                   // Segment ID
    MPI_info.container.push_back(vrt->point()[0]);   // X
    MPI_info.container.push_back(vrt->point()[1]);   // Y
    MPI_info.container.push_back(vrt->point()[2]);   // Z
  }
  
  // Add topological info (cells)
  MPI_info.container.push_back(CMD_TOPO);
  for(CellIterator cll = cells.begin(); cll != cells.end(); cll++)
  {    
    // Extract the cell specific vertices
    VertexOnCellContainer cellVertices = viennagrid::ncells<0>(*cll);	  
    
    // Traverse the vertices and save their IDs in the MPI packet
    for (VertexOnCellIterator vrt = cellVertices.begin(); vrt != cellVertices.end(); vrt++) {
      MPI_info.container.push_back(vrt->id());			      
    }
  }
  
  // Send the segment
  comm.send(destination, 0, MPI_info);
}

// Process information received from a node
void processInformation(MPI_Packet<double> MPI_info, std::vector<double> & GeoContainer, std::vector<double> & TopoContainer)
{
  assert(MPI_info[1] == CMD_GEO && "Second element of MPI_info is not CMD_GEO");

  int index=2;

  // Geometrical information
  for (; MPI_info[index] != CMD_TOPO; index += 5) {
    GeoContainer.push_back(MPI_info[index]);    // Vertex ID
    GeoContainer.push_back(MPI_info[index+1]);  // Segment ID
    GeoContainer.push_back(MPI_info[index+2]);  // X
    GeoContainer.push_back(MPI_info[index+3]);  // Y
    GeoContainer.push_back(MPI_info[index+4]);  // Z
  }
  
  // Topological information
  int CELLSIZE = viennagrid::config::tetrahedral_3d::cell_tag::dim+1;

  for (++index; index < MPI_info.container.size(); index += CELLSIZE) {      
    // Add IDs of the vertices which compose the cell
    for (int i=0; i < CELLSIZE; i++) {  
      TopoContainer.push_back(MPI_info[index+i]);
    }      
  }
}


void root_tasks(mpi::communicator& world, 
                  viennagrid::result_of::domain<viennagrid::config::triangular_3d>::type&   domain, 
                  viennagrid::result_of::domain<viennagrid::config::tetrahedral_3d>::type&  domainres)
{

    // Timer
    viennautils::Timer timer;
    timer.start();

	////////////////////// Hull mesh //////////////////////
  typedef viennagrid::config::triangular_3d                                                                           config_type;
  typedef viennagrid::result_of::domain<config_type>::type                                                                             domain_type;
  
	typedef config_type::cell_tag							  											                                                  cell_tag;
	typedef domain_type::segment_type                                             						                          segment_type;

	typedef viennagrid::result_of::ncell<config_type, cell_tag::dim>::type				                      CellType;
	typedef viennagrid::result_of::ncell<config_type, 0>::type										                                  VertexType;

	typedef viennagrid::result_of::const_ncell_range<segment_type, cell_tag::dim>::type                  CellContainer;
	typedef viennagrid::result_of::iterator<CellContainer>::type               							                            CellIterator;
	
	typedef viennagrid::result_of::const_ncell_range<segment_type, 0>::type  							                          VertexContainer;
	typedef viennagrid::result_of::iterator<VertexContainer>::type               							                          VertexIterator;

	typedef viennagrid::result_of::const_ncell_range<CellType, 0>::type  							                              VertexOnCellContainer;
	typedef viennagrid::result_of::iterator<VertexOnCellContainer>::type         					                              VertexOnCellIterator;

	typedef viennagrid::result_of::point<config_type>::type										                                    PointType;	

  ////////////////////// Volume mesh //////////////////////
  typedef viennamesh::result_of::mesh_generator<viennamesh::tag::netgen>::type                                        volume_mesh_generator_type;
  
  typedef viennagrid::config::tetrahedral_3d                                                                          volume_config_type;
  typedef volume_mesh_generator_type::result_type::value_type                                                         volume_domain_type;
  
  typedef volume_config_type::cell_tag							  											                                          volume_cell_tag;
	typedef volume_domain_type::segment_type                                                                            volume_segment_type;
	
	typedef viennagrid::result_of::ncell<volume_config_type, volume_cell_tag::dim>::type		            volume_CellType;
	typedef viennagrid::result_of::ncell<volume_config_type, 0>::type										                          volume_VertexType;
	
	typedef viennagrid::result_of::const_ncell_range<volume_segment_type, volume_cell_tag::dim>::type    volume_CellContainer;
	typedef viennagrid::result_of::iterator<volume_CellContainer>::type               							                    volume_CellIterator;
	
	typedef viennagrid::result_of::const_ncell_range<volume_segment_type, 0>::type      	                          volume_VertexContainer;
	typedef viennagrid::result_of::iterator<volume_VertexContainer>::type             				                          volume_VertexIterator;

	typedef viennagrid::result_of::const_ncell_range<volume_CellType, 0>::type  							                      volume_VertexOnCellContainer;
	typedef viennagrid::result_of::iterator<volume_VertexOnCellContainer>::type         					                      volume_VertexOnCellIterator;

  typedef viennagrid::result_of::point<volume_config_type>::type							                                    volume_PointType;


  // --------------------------------------------------------------------------
  //
  // ROOT
  //
  // -------------------------------------------------------------------------- 
  
  //////////////////
  ////////////////// Communication stage //////////////////
  //////////////////
  
  // Prepare first packet to broadcast the total number of segments among nodes
  int segmentInd = domain.segments().size();
  //mpi::broadcast(world, segmentInd, 0);  
  std::cout << "[" << world.rank() << "] Number of segments: " << domain.segments().size() << std::endl;

  // ENHANCEMENT: Segment reordering -- Bigger ones first
  // -------------
  int sortedSegs[domain.segments().size()];  
  sortIndices(sortedSegs, domain);

  // Send a segment to each available node (when there are enough segments for it)
  std::size_t si=0;
  for(std::size_t destNode=1; si < domain.segments().size() && si < world.size()-1; si++, destNode++)
  {
      std::cout << "[" << world.rank() << "] sending segment: " << si << " to process: " << destNode << std::endl;
      segmentSend(world, destNode, domain, sortedSegs[si]);
  }
    
  mpi::request reqs[world.size()-1];        // Stores state of a communication  
  MPI_Packet<double> msj[world.size()-1];   // Stores information received asynchronusly from a communication
  
  std::cout << "[" << world.rank() << "] waiting for responses ... " << std::endl;
  // Asynchronous wait for 'job done' confirmation from nodes
  // But, firstly, check if we have already sent every segment (number of segments >= worker nodes)
  if (domain.segments().size() >= world.size()-1)
  { // Not every segment has been sent. Wait for confirmation from every worker node.
    for (int i=0; i<world.size()-1; i++) reqs[i] = world.irecv(i+1, 0, msj[i]);
  }
  else
  { // Every segment has been sent. Wait for confirmation from first $number_of_segments worker nodes.
    for (int i=0; i<domain.segments().size(); i++) reqs[i] = world.irecv(i+1, 0, msj[i]);
  }

  // Container with data to be sent or received over the network
  MPI_Packet<double> MPI_info(CMD_INVALID);
  
  // Containers where geometrical and topological information received from nodes will be stored
  std::vector<double> GeoContainer, TopoContainer[domain.segments().size()]; 
   
  // Rest of the segments will be sent as soon as a node becomes available
  for (; si < domain.segments().size(); si++)
  {      
    // Wait until receive, at least, one confirmation
    // The node who sent the confirmation will be stored in info.source()
    mpi::status info = mpi::wait_any(reqs, reqs+world.size()-1).first;
    
    std::cout << "[" << world.rank() << "] sending segment: " << si << " to process: " << info.source() << std::endl;    
    // Send another segment to the available node (info.source()) and wait for its confirmation.
    segmentSend(world, info.source(), domain, sortedSegs[si]);         
    reqs[info.source()-1] = world.irecv(info.source(), 0, msj[info.source()-1]);
    
    processInformation(msj[info.source()-1], GeoContainer, TopoContainer[(int) msj[info.source()-1][0]]);    
  }
  
  // Every segment has been sent. Receiving last confirmations
  int upperLimit = world.size() > domain.segments().size() ? domain.segments().size() : world.size()-1;  
  for (int i=0; i < world.size()-1 && i < domain.segments().size(); i++) { 
  
    std::pair<mpi::status, mpi::request*> pair = mpi::wait_any(reqs, reqs+upperLimit-i);
    int ind = pair.second - reqs;
    int source = pair.first.source();
    
    
    // Remove received request and reordering reqs array
    for (; ind < upperLimit-1-i; ind++) {
      reqs[ind] = reqs[ind+1];
    }
    
    processInformation(msj[source-1], GeoContainer, TopoContainer[(int) msj[source-1][0]]);
  } 
  
  
  // Terminate communications with every worker node
  MPI_info.clear();
  MPI_info.setCMD(CMD_FINISH);  
  for (size_t i=0; i<world.size()-1; i++) world.isend(i+1, 0, MPI_info);
  
  std::ofstream hfile("results_distribut_collect_volume_mesh.txt", std::ios::out | std::fstream::app);
  hfile << world.size() << " " << timer.get() << std::endl;
  hfile.close();
  
  std::ofstream vfile("results_multisegment_domain_setup.txt", std::ios::out | std::fstream::app);
  timer.start();
  
  std::cout << "[" << world.rank() << "] Volume mesh information received from nodes. Building multi-segment domain..." << std::endl;
  
  //////////////////
  ////////////////// Building volume domain stage //////////////////
  //////////////////
  
  // Containers with indices in order to build the multi-segment volume domain
  std::map<std::size_t, std::size_t> indicesRelation[domain.segments().size()];  // Segment-Specific vertex ID --> Domain ID
  //std::map<std::size_t, std::size_t> uniqueVertices;	                        // Domain ID                  --> GeoContainer ID
  
  // Reserve required segments
  //domainres.create_segments(domain.segments().size());
  domain.segments().resize(domain.segments().size());
  std::size_t ind=0; // Index for new IDs inserted
  
  ////////////////// Build geometrical information //////////////////
    
  // [JW] defining one point object, and hold it in memory
  // this way it is not created for each new point ...
  volume_PointType  p;
  //volume_VertexType v;
  // Traverse GeoContainer
  for (int i=0; i < GeoContainer.size(); i += 5) {
  
    // Compare if current point is already inserted
    p[0] = GeoContainer[i+2];
    p[1] = GeoContainer[i+3];
    p[2] = GeoContainer[i+4];
    //volume_PointType p(GeoContainer[i+2], GeoContainer[i+3], GeoContainer[i+4]);
    bool wasInserted = false;
  
    /*// Traversing through uniqueVertices searching for similar points
    for (std::map<std::size_t, std::size_t>::iterator it2 = uniqueVertices.begin(); it2 != uniqueVertices.end(); it2++)
    {
      volume_PointType p2(GeoContainer[(it2->second)+2], GeoContainer[(it2->second)+3], GeoContainer[(it2->second)+4]);
      
      if (isSamePoint<volume_PointType>(p1, p2))
      { // Similar point detected
        indicesRelation[(int) GeoContainer[i+1]][(int) GeoContainer[i]] = it2->first;
        
        wasInserted = true;
        break;
      }
    }*/      
    
    /*if (!wasInserted)
    { // It is a new vertex
      uniqueVertices[ind] = i;                                                  // Domain ID                  --> GeoContainer ID*/
    indicesRelation[(int) GeoContainer[i+1]][(int) GeoContainer[i]] = ind++;  // Segment-specific vertex ID --> Domain ID      
    //v.point() = p;
    //domainres.add(v);  
    domainres.push_back(p);
    //}
    
  }
  // Adding vertices included in 'uniqueVertices' into the domain	  
  /*for (std::map<std::size_t, std::size_t>::iterator it=uniqueVertices.begin(); it != uniqueVertices.end(); it++) {
    volume_PointType p(GeoContainer[it->second+2], GeoContainer[it->second+3], GeoContainer[it->second+4]);
    volume_VertexType v;
    v.point() = p;
    domainres.add(v);
  }*/

  // clear geocontainer to release some memory ..
  GeoContainer.clear();


  ////////////////// Build topological information //////////////////
  
  // Traverse topological information
  int CELLSIZE = volume_cell_tag::dim+1;

  domainres.segments().resize(domain.segments().size());

  // [JW] defining the objects just once, and reuse them during the loop
  volume_VertexType *cell_vertices[CELLSIZE];
  volume_CellType cell;

  for (segmentInd=0; segmentInd < domain.segments().size(); segmentInd++) {

    volume_segment_type & seg = domainres.segments()[segmentInd];

    // Traverse segment-specific vertices
    for (int it=0; it < TopoContainer[segmentInd].size(); it += CELLSIZE) {     
  	  
  	  // Traverse vertices in the cell
     // [JW] using loop unrolling for the static loop (should be done by the compiler, 
     // but one cannot be sure ..
     cell_vertices[0] = &(viennagrid::ncells<0>(domainres)[ indicesRelation[segmentInd][(int) TopoContainer[segmentInd][it]] ]);
     cell_vertices[1] = &(viennagrid::ncells<0>(domainres)[ indicesRelation[segmentInd][(int) TopoContainer[segmentInd][it+1]] ]);
     cell_vertices[2] = &(viennagrid::ncells<0>(domainres)[ indicesRelation[segmentInd][(int) TopoContainer[segmentInd][it+2]] ]);
     cell_vertices[3] = &(viennagrid::ncells<0>(domainres)[ indicesRelation[segmentInd][(int) TopoContainer[segmentInd][it+3]] ]);               

//     vertices[0] = &(domainres.vertex( indicesRelation[segmentInd][(int) TopoContainer[segmentInd][it]] ));
//     vertices[1] = &(domainres.vertex( indicesRelation[segmentInd][(int) TopoContainer[segmentInd][it+1]] ));
//     vertices[2] = &(domainres.vertex( indicesRelation[segmentInd][(int) TopoContainer[segmentInd][it+2]] ));     
//     vertices[3] = &(domainres.vertex( indicesRelation[segmentInd][(int) TopoContainer[segmentInd][it+3]] ));                    
  	  //for (int ind=0; ind < CELLSIZE; ind++) {
        // Add vertex to the vertices vector
      //  vertices[ind] = &(domainres.vertex( indicesRelation[segmentInd][(int) TopoContainer[segmentInd][it+ind]] ));
      //}
      
      // Build the cell and add it into the domain
      cell.vertices(cell_vertices);
      //domainres.segment(segmentInd).add(cell);
      seg.push_back(cell);
    }
    
  }
  
  vfile << world.size() << " " << timer.get() << std::endl;
  vfile.close();  
}


void worker_tasks(mpi::communicator& world)
{
  ////////////////////// Hull mesh //////////////////////
  typedef viennagrid::config::triangular_3d                                                                           config_type;
  typedef viennagrid::result_of::domain<config_type>::type                                                                             domain_type;
  
	typedef config_type::cell_tag							  											                                                  cell_tag;
	typedef domain_type::segment_type                                             						                          segment_type;

	typedef viennagrid::result_of::ncell<config_type, cell_tag::dim>::type				                      CellType;
	typedef viennagrid::result_of::ncell<config_type, 0>::type										                                  VertexType;

	typedef viennagrid::result_of::const_ncell_range<segment_type, cell_tag::dim>::type                  CellContainer;
	typedef viennagrid::result_of::iterator<CellContainer>::type               							                            CellIterator;
	
	typedef viennagrid::result_of::const_ncell_range<segment_type, 0>::type  							                          VertexContainer;
	typedef viennagrid::result_of::iterator<VertexContainer>::type               							                          VertexIterator;

	typedef viennagrid::result_of::const_ncell_range<CellType, 0>::type  							                              VertexOnCellContainer;
	typedef viennagrid::result_of::iterator<VertexOnCellContainer>::type         					                              VertexOnCellIterator;

	typedef viennagrid::result_of::point<config_type>::type										                                    PointType;	

  ////////////////////// Volume mesh //////////////////////
  typedef viennamesh::result_of::mesh_generator<viennamesh::tag::netgen>::type                                        volume_mesh_generator_type;
  
  typedef viennagrid::config::tetrahedral_3d                                                                          volume_config_type;
  typedef volume_mesh_generator_type::result_type::value_type                                                         volume_domain_type;
  
  typedef volume_config_type::cell_tag							  											                                          volume_cell_tag;
	typedef volume_domain_type::segment_type                                                                            volume_segment_type;
	
	typedef viennagrid::result_of::ncell<volume_config_type, volume_cell_tag::dim>::type		            volume_CellType;
	typedef viennagrid::result_of::ncell<volume_config_type, 0>::type										                          volume_VertexType;
	
	typedef viennagrid::result_of::const_ncell_range<volume_segment_type, volume_cell_tag::dim>::type    volume_CellContainer;
	typedef viennagrid::result_of::iterator<volume_CellContainer>::type               							                    volume_CellIterator;
	
	typedef viennagrid::result_of::const_ncell_range<volume_segment_type, 0>::type      	                          volume_VertexContainer;
	typedef viennagrid::result_of::iterator<volume_VertexContainer>::type             				                          volume_VertexIterator;

	typedef viennagrid::result_of::const_ncell_range<volume_CellType, 0>::type  							                      volume_VertexOnCellContainer;
	typedef viennagrid::result_of::iterator<volume_VertexOnCellContainer>::type         					                      volume_VertexOnCellIterator;

  typedef viennagrid::result_of::point<volume_config_type>::type							                                    volume_PointType;


  // --------------------------------------------------------------------------
  //
  // WORKER NODE
  //
  // -------------------------------------------------------------------------- 
  
  // Container with data to be sent or received over the network
  MPI_Packet<double> MPI_info(CMD_INVALID);
  
  // Segment to work with (received from root)
  int segmentInd;
  
  // Wait for initial message with total amount of segments
  //mpi::broadcast(world, segmentInd, 0);

///  domain_type domainArray[segmentInd];
  
  // Receive segment info
  MPI_info.clear();
  world.recv(0, 0, MPI_info);
  
  // Looping while segments are received
  while (MPI_info.cmd != CMD_FINISH)
  { 	// An iteration per segment

    domain_type domain;
  
    //////////////////
    ////////////////// Communication stage //////////////////
    //////////////////
  	  
  	// Stores which segment we're working with now
    segmentInd = MPI_info[0];  
  	  
    // Container where vertices indexes are stored
    std::map<size_t, size_t> verticesIndexes;	      // vertex ID --> domain ID
    
    //domainArray[segmentInd].create_segments(1); 
    //domainArray[segmentInd].segments().resize(1);
    domain.segments().resize(1);
  #ifdef VMPI_DEBUG
    std::cout << "\t[" << world.rank() << "] Segment " << segmentInd << " received." << std::endl;
  #endif
   
    assert(MPI_info[1] == CMD_GEO && "Fourth element of MPI_info is not CMD_GEO");
    
    // Geometrical information           
    int index = 2;
    std::size_t id = 0;
    for (; MPI_info[index] != CMD_TOPO; index += 4) {
      PointType p(MPI_info[index+1], MPI_info[index+2], MPI_info[index+3]);
//      VertexType v;
//      v.point() = p;
      
      // Adding vertex to the domain and storing the ID
      //verticesIndexes[MPI_info[index]] = domainArray[segmentInd].add(v)->id();
      //domainArray[segmentInd].push_back( p );
      domain.push_back( p );
      verticesIndexes[MPI_info[index]] = id++;
    }
    
    // Topological information
    ///segment_type & seg = domainArray[segmentInd].segments()[0];      
    segment_type & seg = domain.segments()[0];      
    int size = cell_tag::dim+1;
    for (++index; index < MPI_info.container.size(); index += size) {        
      VertexType *cell_vertices[size];          
      CellType cell;

      // We add every vertex 
      for (int i=0; i < size; i++) {
        // Add vertex to the vertices vector
        ///cell_vertices[i] = &( viennagrid::ncells<0>(domainArray[segmentInd])[verticesIndexes[MPI_info[index+i]]] );
        cell_vertices[i] = &( viennagrid::ncells<0>(domain)[verticesIndexes[MPI_info[index+i]]] );
        //vertices[i] = &(domainArray[segmentInd].vertex( verticesIndexes[MPI_info[index+i]] ));
      }
      
      // Build the cell
      cell.vertices(cell_vertices);           
      //domainArray[segmentInd].segment(0).add(cell);
      seg.push_back(cell);
    }	    
    
    //////////////////
    ////////////////// Volume meshing stage //////////////////
    ////////////////// Generating a volume mesh from the hull mesh
    
    //// Functors    
    // Volume mesh generator
    volume_mesh_generator_type          volume_mesher;
    
    // Generate volume mesh applying functors
    //volume_mesh_generator_type::result_type domainres = volume_mesher(cell_normals(orienter(domainArray[segmentInd])));
    ///volume_mesh_generator_type::result_type domainres = volume_mesher(domainArray[segmentInd]);
    volume_mesh_generator_type::result_type domainres = volume_mesher(domain);

  #ifdef VMPI_DEBUG
    std::cout << "\t[" << world.rank() << "] finished - sending result .." << std::endl;
  #endif
  
    // Send single-segment volume domain result back to the root process
    volume_segmentSend(world, 0, *domainres, segmentInd);
    
    // Receive next packet
    MPI_info.clear();
    world.recv(0, 0, MPI_info);
    
  }
  
#ifdef VMPI_DEBUG
  std::cout << "\t[" << world.rank() << "] shutting down .." << std::endl;
#endif  
  
}

} // end namespace detail

void mesh(boost::mpi::communicator&                                                 comm, 
          viennagrid::result_of::domain<viennagrid::config::triangular_3d>::type&   hull_domain, 
          viennagrid::result_of::domain<viennagrid::config::tetrahedral_3d>::type&  volume_domain)
{
  if (comm.rank() == 0)
  {
    viennamesh::mpi::detail::root_tasks(comm, hull_domain, volume_domain);
  }
  else
  {
    viennamesh::mpi::detail::worker_tasks(comm);
  }
}

} // end namespace mpi
} // end namespace viennamesh
#endif


