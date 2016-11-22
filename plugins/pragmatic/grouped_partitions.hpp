#ifndef GROUPED_PARTITIONS_HPP
#define GROUPED_PARTITIONS_HPP

//pragmatic basic includes for mesh data structure
#include "ticker.h"
#include "VTKTools.h"
#include "Mesh.h"
#include "MetricField.h"
#include "Lock.h"
#include "ElementProperty.h"

//boost includes
#include <boost/timer/timer.hpp>
//----------------------------------------------------------------------------------------------------------------------------------------------//
//                                                                Declaration                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------------//
class GroupedPartitions
{
  //list of friend classes
  friend class GroupedPartitionsSmooth;

  public:
    GroupedPartitions(Mesh<double>* input_mesh, int region_count);              //Constructor
    ~GroupedPartitions();                                                       //Destructor
   
    void WriteMergedMesh();

  private:
    Mesh<double>* mesh = nullptr;                                               //input mesh (in pragmatic data structure)

    int num_partitions;
    int num_interfaces;

    std::vector<std::set<int>> _NEList;                                         //TODO: replace this, since its unnecessarily copying data!
    std::vector<index_t> _ENList;                                               //TODO: replace this, since its unnecessarily copying data!
    std::vector<std::set<index_t>> nodes_per_partition;
    std::vector<std::set<index_t>> initial_nodes_per_partition;
    std::vector< std::set<index_t>> elements_per_partition;
    std::vector<std::vector<std::set<index_t>>> interface_nodes;
    std::vector<std::vector<std::vector<index_t>>> interface_sets;
    std::vector<std::vector<std::set<index_t>>> interface_elements_sets;  

    std::vector<size_t> element_counter_interfaces;

    //pragmatic mesh containers
    std::vector<Mesh<double> * > pragmatic_partitions;
    std::vector<Mesh<double> * > pragmatic_interfaces;

    //index mappings for the partitions
    std::vector<std::unordered_map<index_t, index_t>> global_to_local_index_mappings_partitions;
    std::vector<std::unordered_map<index_t, index_t>> local_to_global_index_mappings_partitions;

    //vectors storing the mapping information for the interfaces
    std::vector<std::unordered_map<index_t, index_t>> global_to_local_index_mappings_interfaces;
    std::vector<std::unordered_map<index_t, index_t>> local_to_global_index_mappings_interfaces;

    //vectors storing the number of appearances of the global indices in the local meshes
    std::vector<index_t> element_appearances;
    std::vector<index_t> vertex_appearances;

    //vectors storing the boundaries of the whole mesh, the partitions and the interfaces
    std::vector<int> boundary_nodes_mesh;
    std::vector<std::vector<index_t>> boundary_nodes_partitions;
    std::vector<std::vector<index_t>> boundary_nodes_interfaces;
  
    //variables used by metis
    std::vector<idx_t> eptr;    //vectors storing the mesh as described in the Metis manual
    std::vector<idx_t> eind;    //vectors storing the mesh as described in the Metis manual
    idx_t num_nodes;
    idx_t num_elements;
    idx_t ncommon;  
    idx_t nparts;
    idx_t result;
    std::vector<idx_t> epart;
    std::vector<idx_t> npart;            //nparts not npart!!!

    //helper functions
    void CreateMetisPartitioning();
    void CreatePartitionsAndInterfaces();
    void CreatePragmaticDataStructures();
    void WritePartitionsAndInterfaces(std::string name = "pragmatic");
    void GetPartitionAndInterfaceBoundaries();
    int GetVertexPartitionOrInterface(index_t n, bool* partition, bool *interface);
    int GetElementPartitionOrInterface(index_t n);

    void GetCoords(index_t n, double *vertex);
    void SetCoords(index_t n, double *vertex);
    std::vector<index_t>* GetNNList(index_t n, int *partitions, int *interfaces);
}; //end of class

//----------------------------------------------------------------------------------------------------------------------------------------------//
//                                                                Helper Functions                                                              //
//----------------------------------------------------------------------------------------------------------------------------------------------//

//binomial_coefficient(int n, int k)
//TODO: implement a more efficient dynamic version (e.g., http://www.geeksforgeeks.org/dynamic-programming-set-9-binomial-coefficient/)
unsigned int binomial_coefficient (int n, int k)
{ 
  //check inputs
  if (k == 0 || k == n)
  {
    return 1;
  }

  //use recursion for computation
  return (binomial_coefficient(n-1, k-1) + binomial_coefficient(n-1, k));
} //end of binomial_coefficient(int n, int k)

//----------------------------------------------------------------------------------------------------------------------------------------------//
//                                                                Implementation                                                                //
//----------------------------------------------------------------------------------------------------------------------------------------------//

//TODO:Constructor
//TODO:conversion from ViennaMesh data structure into pragmatic data structure can be done here!?!?!?!?!?!?
//TODO: REPLACE _NEList and _ENList function, since it's copying data unnecessarily!!!
//TODO: use element initializer list!!!
GroupedPartitions::GroupedPartitions(Mesh<double>* input_mesh, int region_count) : num_nodes(input_mesh->get_number_nodes()), num_elements(input_mesh->get_number_elements()), ncommon(input_mesh->get_number_dimensions()), nparts(region_count), epart(input_mesh->get_number_elements()), npart(input_mesh->get_number_nodes()), nodes_per_partition(region_count), elements_per_partition(region_count), interface_nodes(region_count-1), interface_sets(region_count-1), _NEList(input_mesh->get_node_element()), _ENList(input_mesh->get_element_node()), interface_elements_sets(region_count-1), element_counter_interfaces(input_mesh->get_number_elements(), 0), global_to_local_index_mappings_partitions(region_count), local_to_global_index_mappings_partitions(region_count), element_appearances(input_mesh->get_number_elements(), 0), vertex_appearances(input_mesh->get_number_nodes(), 0), global_to_local_index_mappings_interfaces(binomial_coefficient(region_count, 2)), local_to_global_index_mappings_interfaces(binomial_coefficient(region_count, 2)), boundary_nodes_mesh(input_mesh->get_number_nodes(), 0), boundary_nodes_partitions(region_count), boundary_nodes_interfaces(binomial_coefficient(region_count, 2)), num_partitions(region_count), num_interfaces(binomial_coefficient(region_count, 2)), initial_nodes_per_partition(region_count)
{
  std::cout << "Grouped Partitions Object created" << std::endl;
  mesh = input_mesh;

  CreateMetisPartitioning();

  CreatePartitionsAndInterfaces();

  CreatePragmaticDataStructures();

  GetPartitionAndInterfaceBoundaries();

  WritePartitionsAndInterfaces();

}
//end of constructor

//TODO:Destructor
GroupedPartitions::~GroupedPartitions()
{
  std::cout << "Grouped Partitions Object deleted" << std::endl;
}
//end of destructor

//Create Metis Partitioning
void GroupedPartitions::CreateMetisPartitioning()
{
  std::cout << "Calling METISPartMeshNodal" << std::endl;

  //fill eptr and eind, as done in viennamesh plugin "metis", file "mesh_partitionig.cpp"  
  eptr.push_back(0);

  for (size_t i = 0; i < mesh->get_number_elements(); ++i)
  {
    const index_t* element_ptr = nullptr;
    element_ptr = mesh->get_element(i);

    for (size_t j = 0; j < (mesh->get_number_dimensions() + 1); ++j)
    {
       eind.push_back( *(element_ptr+j) );
    }               

    eptr.push_back( eind.size() );    
    
  }   

  //Call Metis Partitioning Function (see metis manual for details on the parameters and on the use of the metis API)
  /*METIS_PartMeshDual (&num_elements,
                      &num_nodes,
                      eptr.data(),
                      eind.data(),
                      NULL,
                      NULL,
                      &ncommon,
                      &nparts,
                      NULL,
                      NULL,
                      &result,
                      epart.data(),
                      npart.data());*/

  METIS_PartMeshNodal(&num_elements,
                      &num_nodes,
                      eptr.data(),
                      eind.data(),
                      NULL,
                      NULL,
                      &nparts,
                      NULL,
                      NULL,
                      &result,
                      epart.data(),
                      npart.data());
}
//end of GroupedPartitions::CreateMetisPartitioning()

void GroupedPartitions::CreatePartitionsAndInterfaces()
{
  std::cout << "Creating Partitions and Interfaces" << std::endl;

  //get the nodes per partition
  //this for-loop is not parallelizable, since set.insert() is not thread-safe
  for (size_t i = 0; i < num_elements; ++i)
  {
    //add nodes
    //TODO: the following hast to be updated for the 3D case!!
    initial_nodes_per_partition[ epart[i] ].insert(_ENList[i*3]);
    initial_nodes_per_partition[ epart[i] ].insert(_ENList[i*3+1]);
    initial_nodes_per_partition[ epart[i] ].insert(_ENList[i*3+2]);

    //add element
    elements_per_partition[ epart[i] ].insert(i);
  } 
  //end of get nodes per partition

  //get the interface nodes for all partition boundaries
  for (size_t i = 0; i < nparts; ++i)     
  {        
    for (size_t j = i+1; j < nparts; ++j)
    {   
      std::vector<index_t> tmp;

      set_intersection( initial_nodes_per_partition[i].begin(), initial_nodes_per_partition[i].end(),
                        initial_nodes_per_partition[j].begin(), initial_nodes_per_partition[j].end(),
                        inserter(tmp, tmp.begin()) );

      interface_sets[i].push_back(tmp);
    }      
  } 
  //end of get the interface nodes for all partition boundaries

  //remove interface nodes from initial_nodes_per_partition
  for (auto interface_nodes : interface_sets)
  {
    for (size_t i = 0; i < interface_nodes.size(); ++i)
    {
      for (size_t j = 0; j < initial_nodes_per_partition.size(); ++j)
      {
        for (auto it : interface_nodes[i])
        {
          initial_nodes_per_partition[j].erase(it);
        }
      }
    }
  } 
  //end of remove interface nodes from initial_nodes_per_partition
/*
  //get all elements per partition
  //TODO: seems unable to parallelize due to possibla data races with insert()-function 
  //TODO: incorporate into initial_nodes_per_partition loop - already done
  for(size_t i = 0; i < num_elements; ++i)
  {
    //elements_per_partition[ epart[i] ].insert(i); //incorporated in loop for initial_nodes_per_partition
  }
  //end of get all elements per partition
*/
  //get partition interface elements
  for (size_t i = 0; i < interface_sets.size(); ++i)
  {
    for (size_t j = 0; j < interface_sets[i].size(); ++j)
    {
      std::set<index_t> tmp;
      for (size_t k = 0; k < interface_sets[i][j].size(); ++k)
      {            
        for (auto NE_it : _NEList[interface_sets[i][j][k]])
        {  
          //element must not appear in more than one interface mesh!
          if (element_counter_interfaces[NE_it] == 0)
          {          
            tmp.insert(NE_it);
            ++element_counter_interfaces[NE_it];
          }
        }
      }
      interface_elements_sets[i].push_back(tmp);
      tmp.clear();
    }
  }
  //end of get partition interface elements

  //add the vertices of the partition interface elements to interface_sets
  //TODO: this seems to be overload, since a set in interface_sets storing the interface nodes is already available
  for (size_t i = 0; i < interface_elements_sets.size(); ++i)
  { 
    for (size_t j = 0; j < interface_elements_sets[i].size(); ++j)
    {
      std::set<index_t> tmp;

      for (auto interface_element : interface_elements_sets[i][j])
      {
        tmp.insert(_ENList[interface_element*3]);
        tmp.insert(_ENList[interface_element*3+1]);
        tmp.insert(_ENList[interface_element*3+2]);
      }
      interface_nodes[i].push_back(tmp);
    }
  }
  //end of add the vertices of the partition interface elements to interface_sets

  //remove partition interface elements from the actual partitions
  for (auto interface_elements : interface_elements_sets)
  {
    for (size_t i = 0; i < interface_elements.size(); ++i)
    {
      for (size_t j = 0; j < elements_per_partition.size(); ++j)
      {
        for (auto it : interface_elements[i])
        {
          elements_per_partition[j].erase(it);
        }
      }
    }
  } 
  //end of remove partition interface elements from the actual partitions

  //now get vertices from global ENList
  for (size_t i = 0; i < nparts; ++i)
  {
    for (auto element : elements_per_partition[i])
    {
      nodes_per_partition[i].insert( _ENList[3*element] );
      nodes_per_partition[i].insert( _ENList[3*element+1]);
      nodes_per_partition[i].insert( _ENList[3*element+2]);
    }
  }
  //end of get vertices from global ENList
}
//end of void GroupedPartitions::CreatePartitionsAndInterfaces()

void GroupedPartitions::CreatePragmaticDataStructures()
{
  std::cout << "Creating Pragmatic Data Structures" << std::endl;

  //Create Partitions

  //vectors storing the coordinate
  std::vector< std::vector<double>> x_coords(nparts);
  std::vector< std::vector<double>> y_coords(nparts);
  std::vector< std::vector<double>> z_coords(nparts);

  //vector storing the ENLists of each region
  std::vector<std::vector<index_t>> ENLists_partitions(nparts);

  //loop over all partitions
  for (size_t i = 0; i < nparts; ++i)
  {
    //get number of vertices and elements
    int num_points = nodes_per_partition[i].size();
    int num_cells = elements_per_partition[i].size();

    //get the vertex-to-index-mapping between old and new indices
    //and additionally the index-to-vertex-mapping
    //TODO: use unordered_map instead, to speed up the code
    std::unordered_map <index_t, index_t> global_to_local_index_map;
    std::unordered_map <index_t, index_t> local_to_global_index_map;

    index_t new_vertex_id = 0;
    for (auto it : nodes_per_partition[i])
    {
      global_to_local_index_map.insert( std::make_pair(it, new_vertex_id++) );
      ++vertex_appearances[it];
    }
    
    global_to_local_index_mappings_partitions[i] = global_to_local_index_map;

    //and get also the index-to-vertex mapping (opposite direction than vertex to index mapping)
    for (auto it : global_to_local_index_map)
    {
      local_to_global_index_map[it.second] = it.first;
    }

    local_to_global_index_mappings_partitions[i] = local_to_global_index_map;

    //pre-allocate memory
    x_coords[i].reserve(num_points);
    y_coords[i].reserve(num_points);
    ENLists_partitions[i].resize(3*num_cells);

    //get coordinates of each vertex
    int counter = 0;
    for (auto it : nodes_per_partition[i])
    {
      double p[2];
      mesh->get_coords( it, p);
      x_coords[i][counter] = p[0];
      y_coords[i][counter] = p[1];
      ++counter;
    }

    //create ENList with respect to the new vertex indices
    counter=0;        
    for (auto it : elements_per_partition[i])
    {      
      const index_t *element_ptr = nullptr;
      element_ptr = mesh->get_element(it);
       
      ENLists_partitions[i][counter++] = global_to_local_index_map[*(element_ptr++)];
      ENLists_partitions[i][counter++] = global_to_local_index_map[*(element_ptr++)];
      ENLists_partitions[i][counter++] = global_to_local_index_map[*(element_ptr++)];  
      ++element_appearances[it];       
    }

    //create pragmatic mesh 
    Mesh<double> *partition_mesh = nullptr;

    //TODO: change for 3D refinement
    //mesh = new Mesh<double> ( num_points, num_cells, &(ENLists_regions[region.id()][0]) ,&(x_coords[region.id()][0]), &(y_coords[region.id()][0]), &(z_coords[region.id()][0]) );        
    partition_mesh = new Mesh<double> ( num_points, num_cells, &(ENLists_partitions[i][0]), &(x_coords[i][0]), &(y_coords[i][0]) );
    partition_mesh->create_boundary();

    pragmatic_partitions.push_back(partition_mesh);
  
    //delete partial_mesh;        //TODO: creates segfault if comments are removed
  } 
  //end of loop over all partitions

  //Create Interfaces
  int num_interfaces = binomial_coefficient(nparts, 2);
  
  //vectors storing the coordinate
  std::vector< std::vector<double>> x_coords_interfaces(num_interfaces);
  std::vector< std::vector<double>> y_coords_interfaces(num_interfaces);
  std::vector< std::vector<double>> z_coords_interfaces(num_interfaces);

  //vector storing the ENLists of each region
  std::vector<std::vector<index_t>> ENLists_interfaces(num_interfaces);

  //loop over all interfaces
  int interface_counter = 0 ;
  for(size_t i = 0; i < interface_elements_sets.size(); ++i)
  {
    for (size_t j = 0; j < interface_elements_sets[i].size(); ++j, interface_counter++)
    {
      //get number of vertices and elements
      int num_points = interface_nodes[i][j].size();
      int num_cells = interface_elements_sets[i][j].size();

      //get the vertex-to-index-mapping between old and new indices
      std::unordered_map <index_t, index_t> interface_global_to_local_index_map;
      std::unordered_map <index_t, index_t> interface_local_to_global_index_map;

      if (num_points == 0 || num_cells == 0)
      {
         pragmatic_interfaces.push_back(nullptr);
      
         //if interface i does not exist, insert negative numbers into the mapping vectors to mark them as empty
         interface_global_to_local_index_map.insert( std::make_pair(-1, -1) );
         global_to_local_index_mappings_interfaces[interface_counter] = interface_global_to_local_index_map;
         local_to_global_index_mappings_interfaces[interface_counter] = interface_global_to_local_index_map;

        continue;
      }

      index_t new_vertex_id = 0;  
      for (auto it : interface_nodes[i][j])
      {
        interface_global_to_local_index_map.insert( std::make_pair(it, new_vertex_id++) );
        ++vertex_appearances[it]; //TODO: atomic increment for all appearances incrementations for parallelization???
      }

      global_to_local_index_mappings_interfaces[interface_counter] = interface_global_to_local_index_map;

      //and get also the index-to-vertex mapping (opposite direction than vertex to index mapping)
      for (auto it : interface_global_to_local_index_map)
      {
        interface_local_to_global_index_map[it.second] = it.first;
      }

      local_to_global_index_mappings_interfaces[interface_counter] = interface_local_to_global_index_map;

      //pre-allocate memory
      x_coords_interfaces[interface_counter].reserve(num_points);
      y_coords_interfaces[interface_counter].reserve(num_points);
      ENLists_interfaces[interface_counter].resize(3*num_cells);

      //get coordinates of each vertex
      //TODO: combine this loop with the next one!
      size_t counter = 0;
      for (auto it : interface_nodes[i][j])
      {
        double p[2];
        mesh->get_coords( it, p);
        x_coords_interfaces[interface_counter][counter] = p[0];
        y_coords_interfaces[interface_counter][counter] = p[1];           
        ++counter;
      }

      //create ENList with respect to the new vertex indices
      //TODO: combine this loop with the previous one!
      counter=0;        
      for (auto it : interface_elements_sets[i][j])
      {         
        const index_t *element_ptr = nullptr;
        element_ptr = mesh->get_element(it);
          
        ENLists_interfaces[interface_counter][counter++] = interface_global_to_local_index_map[*(element_ptr++)];
        ENLists_interfaces[interface_counter][counter++] = interface_global_to_local_index_map[*(element_ptr++)];
        ENLists_interfaces[interface_counter][counter++] = interface_global_to_local_index_map[*(element_ptr++)];      

        ++element_appearances[it];    
      }
          
      //create pragmatic mesh 
      Mesh<double> *interface_mesh = nullptr;

      //TODO: change for 3D refinement
      //mesh = new Mesh<double> ( num_points, num_cells, &(ENLists_regions[region.id()][0]) ,&(x_coords[region.id()][0]), &(y_coords[region.id()][0]), &(z_coords[region.id()][0]) );        
      interface_mesh = new Mesh<double> ( num_points, num_cells, &(ENLists_interfaces[interface_counter][0]), &(x_coords_interfaces[interface_counter][0]), &(y_coords_interfaces[interface_counter][0]) );
      interface_mesh->create_boundary();

      pragmatic_interfaces.push_back(interface_mesh);
    }
  }
  //end of loop over all interfaces
}
//end of void GroupedPartitions::CreatePragmaticDataStructures()

//GroupedPartitions::WritePartitionsAndInterfaces()
void GroupedPartitions::WritePartitionsAndInterfaces(std::string name)
{
  std::cout << "Using filename \033[1;36m" << name << "\033[0m" << std::endl;

  //write partitions
  for (size_t i = 0; i < pragmatic_partitions.size(); ++i)
  {
    std::cout << "Writing partition " << i << std::endl;
    std::cout << "  Vertex count = " << pragmatic_partitions[i]->get_number_nodes() << std::endl;
    std::cout << "  Cell count = " << pragmatic_partitions[i]->get_number_elements() << std::endl;
    
    std::string filename;
    filename += "examples/data/pragmatic_metis_partitioning/";
    filename += name;
    filename += "_partition_";
    filename += std::to_string( i );
  
    VTKTools<double>::export_vtu(filename.c_str(), pragmatic_partitions[i]);
  }
  //end of write partitions

  //write interfaces
  for (size_t i = 0; i < pragmatic_interfaces.size(); ++i)
  {
    if (pragmatic_interfaces[i] == nullptr)
    {
      continue;
    }

    std::cout << "Writing interface " << i << std::endl;
    std::cout << "  Vertex count = " << pragmatic_interfaces[i]->get_number_nodes() << std::endl;
    std::cout << "  Cell count = " << pragmatic_interfaces[i]->get_number_elements() << std::endl;

    std::string filename;
    filename += "examples/data/pragmatic_metis_partitioning/";
    filename += name;
    filename += "_interface_";
    filename += std::to_string( i );
  
    VTKTools<double>::export_vtu(filename.c_str(), pragmatic_interfaces[i]);
  }        
  //end of write interfaces
}
//end of GroupedPartitions::WritePartitionsAndInterfaces()

//GroupedPartitions::GetPartitionAndInterfaceBoundaries()
//TODO: replace function calls with lookups of class variables!
void GroupedPartitions::GetPartitionAndInterfaceBoundaries()
{
  std::cout << "Creating Boundary Information" << std::endl;
  //get global boundary nodes
  std::vector<int> global_boundary = mesh->copy_boundary_vector();

  for (size_t i = 0; i < mesh->get_number_elements(); ++i)
  {
    const int *n = mesh->get_element(i);

    for (size_t j = 0; j < 3; ++j)
    {
      boundary_nodes_mesh[n[(j+1)%3]] = std::max(boundary_nodes_mesh[n[(j+1)%3]], global_boundary[i*3+j]);
      boundary_nodes_mesh[n[(j+2)%3]] = std::max(boundary_nodes_mesh[n[(j+2)%3]], global_boundary[i*3+j]);
    }
  }
  //end of get global boundary nodes

  //get partition boundary nodes
  for (size_t mesh_number = 0; mesh_number < pragmatic_partitions.size(); mesh_number++)
  {
    std::vector<int> boundary_nodes_tmp(pragmatic_partitions[mesh_number]->get_number_nodes(), 0);
    std::vector<int> boundary_mesh = pragmatic_partitions[mesh_number]->copy_boundary_vector();

    for (size_t i = 0; i < pragmatic_partitions[mesh_number]->get_number_elements(); ++i)
    {
      const int *n = pragmatic_partitions[mesh_number]->get_element(i);

      for (size_t j = 0; j < 3; ++j)
      {
        boundary_nodes_tmp[n[(j+1)%3]] = std::max(boundary_nodes_tmp[n[(j+1)%3]], boundary_mesh[i*3+j]);
        boundary_nodes_tmp[n[(j+2)%3]] = std::max(boundary_nodes_tmp[n[(j+2)%3]], boundary_mesh[i*3+j]);
      }
    }
    boundary_nodes_partitions[mesh_number]=boundary_nodes_tmp;
  }
  //end of get partition boundary nodes

  //get interface boundary nodes
  for (size_t mesh_number = 0; mesh_number < pragmatic_interfaces.size(); mesh_number++)
  {
    if  (pragmatic_interfaces[mesh_number] == nullptr)
    {  
      continue;
    }
    std::vector<int> boundary_nodes_tmp(pragmatic_interfaces[mesh_number]->get_number_nodes(), 0);
    std::vector<int> boundary_mesh = pragmatic_interfaces[mesh_number]->copy_boundary_vector();

    for (size_t i = 0; i < pragmatic_interfaces[mesh_number]->get_number_elements(); ++i)
    {
      const int *n = pragmatic_interfaces[mesh_number]->get_element(i);

      for (size_t j = 0; j < 3; ++j)
      {
        boundary_nodes_tmp[n[(j+1)%3]] = std::max(boundary_nodes_tmp[n[(j+1)%3]], boundary_mesh[i*3+j]);
        boundary_nodes_tmp[n[(j+2)%3]] = std::max(boundary_nodes_tmp[n[(j+2)%3]], boundary_mesh[i*3+j]);
      }
    }
    boundary_nodes_interfaces[mesh_number]=boundary_nodes_tmp;
  }
  //end of get partition boundary nodes
} 
//end of GroupedPartitions::GetPartitionAndInterfaceBoundaries()

//GroupedPartitions::GetCoords()
void GroupedPartitions::GetCoords(index_t n, double *vertex)
{
  bool found = false;

  //search partitions
 
  //do a lookup in the global_to_local_index_mappings_partitions and then return the corresponding coordinates!
  for (size_t i = 0; i < global_to_local_index_mappings_partitions.size(); ++i)
  {
    std::unordered_map<index_t, index_t>::iterator position = global_to_local_index_mappings_partitions[i].find(n);
    
    if (position == global_to_local_index_mappings_partitions[i].end())
    {
      continue;
    }

    //std::cout << "found vertex " << n << " in partition " << i << std::endl;
    pragmatic_partitions[i]->get_coords(position->second, vertex);
    found = true;
    break;
  }
  //end of seach partitions

  //check interface
  if (found)
    return; 

  for (size_t i = 0; i < global_to_local_index_mappings_interfaces.size(); ++i)
  {
    std::unordered_map<index_t, index_t>::iterator position = global_to_local_index_mappings_interfaces[i].find(n);

    if(position == global_to_local_index_mappings_interfaces[i].end())
    {        
        continue;
    }

    //std::cout << "found vertex " << n << " in interface " << i << std::endl;
    pragmatic_interfaces[i]->get_coords(position->second, vertex);
    break;
  }
  //end of search interface
}
//end of GroupedPartitions::GetCoords()

//GroupedPartitions::SetCoords()
//TODO: implementation, and validation check, if the moved vertex is on a valid location!!!
//TODO: implement for 3D
void GroupedPartitions::SetCoords(index_t n, double *vertex)
{
  int to_do = vertex_appearances[n];
  int comitted = 0;

  //update partitions
  for(size_t i = 0; i < global_to_local_index_mappings_partitions.size(); ++i)
  {
    std::unordered_map<index_t, index_t>::iterator position = global_to_local_index_mappings_partitions[i].find(n);

    if (position == global_to_local_index_mappings_partitions[i].end())
    {
      continue;
    }

      pragmatic_partitions[i]->set_coords(position->second, vertex);
      ++comitted;

      if (to_do == comitted)
        break;
    
  }
  //end of update partitions

  if (to_do == comitted)
    return;

  //update interfaces
  for(size_t i = 0; i < global_to_local_index_mappings_interfaces.size(); ++i)
  {
    std::unordered_map<index_t, index_t>::iterator position = global_to_local_index_mappings_interfaces[i].find(n);
    
    if(position == global_to_local_index_mappings_interfaces[i].end())
    {        
        continue;
    }

    else
    {
      pragmatic_interfaces[i]->set_coords(position->second, vertex);
      ++comitted;

      if (to_do == comitted)
        break;
    }
  }
  
  //end of update interfaces
}
//end of GroupedPartitions:SetCoords()

//GroupedPartitions::GetNNList(index_t n)
std::vector<index_t>* GroupedPartitions::GetNNList(index_t n, int *partitions, int *interfaces)
{
  bool partition;
  bool interface;
  int mapping = GetVertexPartitionOrInterface(n, &partition, &interface);

  if (partition)
  {
    std::unordered_map<index_t, index_t>::iterator position = global_to_local_index_mappings_partitions[mapping].find(n);

    *partitions = mapping;
    return pragmatic_partitions[mapping]->get_nnlist( position->second );
  }

  else if (interface)
  {
    std::unordered_map<index_t, index_t>::iterator position = global_to_local_index_mappings_interfaces[mapping].find(n);

    *interfaces = mapping;
    return pragmatic_interfaces[mapping]->get_nnlist( position->second );
  }
  /*
  //check partitions
  for (size_t i = 0; i < nparts; ++i)
  {
    std::unordered_map<index_t, index_t>::iterator position = global_to_local_index_mappings_partitions[i].find(n);

    if (position == global_to_local_index_mappings_partitions[i].end())
    {
      continue;
    }

    *partitions = i;
    return pragmatic_partitions[i]->get_nnlist( position->second );
  }
  //end of check partitions

  //check interfaces
  for (size_t i = 0; i < global_to_local_index_mappings_interfaces.size(); ++i)
  {
    std::unordered_map<index_t, index_t>::iterator position = global_to_local_index_mappings_interfaces[i].find(n);

    if (position == global_to_local_index_mappings_interfaces[i].end())
    {
      continue;
    }

    *interfaces = i;
    return pragmatic_interfaces[i]->get_nnlist( position->second );
  }  
  //end of check interfaces
  */
  else
    std::cout << "\033[1;31mVertex " << n << " not found\033[0m" << std::endl;
}
//end of GroupedPartitions::GetNNList(index_t n)

//GroupedPartitions::WriteMergedMesh()
void GroupedPartitions::WriteMergedMesh()
{
  std::cout << "Write merged mesh" << std::endl;
/*
  //boost timer for collecting benchmark data
  boost::timer::cpu_timer t;
*/
  //create merged ENList
  //TODO: Update for 3D case (then num_elements has to be multiplied with 4)
  std::vector<index_t> merged_ENList(num_elements*3);

  //ierate over partitions
  int global_element_counter = 0;
  for (size_t i = 0; i < pragmatic_partitions.size(); ++i)  
  {
    for (size_t j = 0; j < pragmatic_partitions[i]->get_number_elements(); ++j)
    {
      const index_t *element_ptr = nullptr;
      element_ptr = pragmatic_partitions[i]->get_element(j);

      merged_ENList[3*global_element_counter] = local_to_global_index_mappings_partitions[i].at( *(element_ptr++) );
      merged_ENList[3*global_element_counter+1] = local_to_global_index_mappings_partitions[i].at( *(element_ptr++) );
      merged_ENList[3*global_element_counter+2] = local_to_global_index_mappings_partitions[i].at( *(element_ptr++) );  
      ++global_element_counter;
    }
  }

  //iterate over interfaces
  for (size_t i = 0; i < pragmatic_interfaces.size(); ++i)  
  {
    if  (pragmatic_interfaces[i] == nullptr)
    {  
      continue;
    }

    for (size_t j = 0; j < pragmatic_interfaces[i]->get_number_elements(); ++j)
    {
      const index_t *element_ptr = nullptr;
      element_ptr = pragmatic_interfaces[i]->get_element(j);

      merged_ENList[3*global_element_counter] = local_to_global_index_mappings_interfaces[i].at( *(element_ptr++) );
      merged_ENList[3*global_element_counter+1] = local_to_global_index_mappings_interfaces[i].at( *(element_ptr++) );
      merged_ENList[3*global_element_counter+2] = local_to_global_index_mappings_interfaces[i].at( *(element_ptr++) );  
      ++global_element_counter;
    }
  }
  //end of create merged ENList

  //ofstream object
  ofstream writer;
  writer.open("examples/data/pragmatic_metis_partitioning/merged_mesh.vtu", ios::out);

  //write header
  writer << "<?xml version=\"1.0\"?>" << std::endl;
  writer << "<VTKFile type=\"UnstructuredGrid\" version=\"1.0\" byte_order=\"LittleEndian\">" << std::endl;
  writer << " <UnstructuredGrid>" << std::endl;
  //end of write header

  writer << "  <Piece NumberOfPoints=\"" << num_nodes << "\" NumberOfCells=\"" << num_elements << "\">" << std::endl;    

  //write points into file
  writer << "   <Points>" << std::endl;
  writer << "    <DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;  

  //iterate over coordinates vector
  for (size_t i = 0; i < num_nodes; ++i)
  {
    //TODO: change for 3D case
    //double x[3];
    double x[2];
    GetCoords(i, x);
    writer << std::fixed << std::setprecision(8) << x[0] << " " << x[1] << " " << "0" << std::endl;      
    //TODO: change for 3D case
    //writer << x[0] << " " << x[1] << " " << x[2] << std::endl;
  }
  //writer << std::endl;
  writer << "    </DataArray>" << std::endl;
  writer << "   </Points> " << std::endl;
  //end of write points into file

  //write cells into file
  writer << "   <Cells> " << std::endl;
  writer << "    <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">" << std::endl;

  for (size_t i = 0; i < num_elements; ++i)
  {
    writer << merged_ENList[3*i] << " " << merged_ENList[3*i+1] << " " << merged_ENList[3*i+2] << " ";

    writer << std::endl;
  }

  writer << std::endl;
  writer << "    </DataArray>" << std::endl;
  //end of write cells into file

  //write offset into file
  writer << "    <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">" << std::endl;
    
  for (size_t i = 1; i <= num_elements; ++i)
  {
    //TODO: change for 3D case
    //writer << j*4 << " ";
    writer << i*3 << " ";

    if (i%6 == 0)
    {
      writer << std::endl;
    }
  }

  writer << std::endl;
  writer << "    </DataArray>" << std::endl;
  //end of write offset into file
  
  //write types into file
  writer << "    <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">" << std::endl;
  
  for (size_t j = 0; j < num_elements; ++j) 
  {
    //TODO: change for 3D case
    //writer << 10 << " ";
    writer << 5 << " ";
      
    if (j%6 == 0)
    {
      writer << std::endl;
    }
  }

  writer << std::endl;
  writer << "    </DataArray>" << std::endl;
  //end of write types into file

  writer << "   </Cells>" << std::endl;
  writer << "  </Piece>" << std::endl;

  //write footer
  writer << " </UnstructuredGrid>" << std::endl;
  writer << "</VTKFile>" << std::endl;
  //end of write footer
  
  //close ofstream object
  writer.close();
/*
  boost::timer::cpu_times timer = t.elapsed();

  //runtime benchmark
  ofstream runtime;
  runtime.open("mesh_merging_runtime.txt");
  runtime << "    wall    user    system" << std::endl; 
  runtime << std::fixed << std::setprecision(6) << static_cast<double>(timer.wall) / 1000000000.0 << " " << static_cast<double>(timer.user) / 1000000000.0 << " " << static_cast<double>(timer.system) / 1000000000.0 << std::endl;
  runtime.close();
*/
}
//end of GroupedPartitions::WriteMergedMesh()

//TODO: GroupedPartitions::FindVertex(index_t n)
int GroupedPartitions::GetVertexPartitionOrInterface(index_t n, bool *partition, bool *interface)
{
  //check partitions
  for (size_t i = 0; i < nparts; ++i)
  {
    std::unordered_map<index_t, index_t>::iterator position = global_to_local_index_mappings_partitions[i].find(n);

    if (position == global_to_local_index_mappings_partitions[i].end())
    {
      continue;
    }
    *partition = true;
    *interface = false;
    return i;
  }
  //end of check partitions

  //check interfaces
  for (size_t i = 0; i < global_to_local_index_mappings_interfaces.size(); ++i)
  {
    std::unordered_map<index_t, index_t>::iterator position = global_to_local_index_mappings_interfaces[i].find(n);

    if (position == global_to_local_index_mappings_interfaces[i].end())
    {
      continue;
    }
    *partition = false;
    *interface = true;
    return i;
  }  
  //end of check interfaces

  //error
  return -1;
}
//end of GroupedPartitions::FindVertex(index_t n)

//TODO: GroupedPartitions::FindElement(index_t n)
int GroupedPartitions::GetElementPartitionOrInterface(index_t n)
{
  return -1;
}
//end of GroupedPartitions::FindElement(index_t n)

//----------------------------------------------------------------------------------------------------------------------------------------------//
//                                                                     End                                                                      //
//----------------------------------------------------------------------------------------------------------------------------------------------//

#endif 
