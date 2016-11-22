#ifndef GROUPED_PARTITIONS_SMOOTH_HPP
#define GROUPED_PARTITIONS_SMOOTH_HPP

#include "grouped_partitions.hpp"

//----------------------------------------------------------------------------------------------------------------------------------------------//
//                                                                Declaration                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------------//

class GroupedPartitionsSmooth
{
  public:
    //constructor and destructor
    GroupedPartitionsSmooth(GroupedPartitions &GP);
    ~GroupedPartitionsSmooth();

    //member functions
    bool SimpleLaplace(int iterations);
    bool SimpleLaplaceOnGroups(int iterations);

  private:
    GroupedPartitions &mesh;
};

//----------------------------------------------------------------------------------------------------------------------------------------------//
//                                                                Helper Functions                                                              //
//----------------------------------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------------------------------------//
//                                                                Implementation                                                                //
//----------------------------------------------------------------------------------------------------------------------------------------------//

//Constructor
GroupedPartitionsSmooth::GroupedPartitionsSmooth(GroupedPartitions &GP) : mesh(GP)
{
  std::cout << "Create GroupedPartitionsSmooth Object" << std::endl;
  std::cout << "Smooth " << mesh.num_partitions << " partitions and " << mesh.num_interfaces << " interfaces" << std::endl;
}
//end of Constructor

//Destructor
GroupedPartitionsSmooth::~GroupedPartitionsSmooth()
{
  std::cout << "Delete GroupedPartitionsSmooth Object" << std::endl;
}
//end of Destructor

//Simple Laplace Smoother
bool GroupedPartitionsSmooth::SimpleLaplace(int no_iterations)
{
  std::cout << no_iterations << " iterations of Simple Laplace Smoother" << std::endl;
  //for loop number of iterations
  for (size_t iter = 1; iter <= no_iterations; ++iter)
  {
    std::cout << "iteration " << iter << "/" << no_iterations << std::endl;

    //for loop over global indices
    for (index_t i = 0; i < mesh.num_nodes; ++i)
    {
      //skip vertex if it is on the global mesh boundary, or if it appears in more than 2 partitions/interfaces
      if ( mesh.boundary_nodes_mesh[i] || mesh.vertex_appearances[i] >= 2)
      {
        continue;
      } 

      double vertex[2] {0.0, 0.0};

      //partition and interface are set by the GetNNList function to the partition or interface containint the vertex i
      //and to know, from which partition/interface the NNList is coming from, to apply the correct index_mapping
      int partition = -1;
      int interface = -1;
    
      //get vertex coordinates
      mesh.GetCoords(i, vertex);  

      //calculate new position
      //equation from MSc-Thesis "Glättung von Polygonnetzen in medizinischen Visualisierungen - J. Haase, 2005, eq. 2.2"
      std::vector<index_t>* _NNList = mesh.GetNNList(i, &partition, &interface);
      int num_neighbors = _NNList->size();

      double q[2] {0.0, 0.0};
      
      for (size_t j = 0; j < _NNList->size(); ++j)
      {
        double q_tmp[2];

        if (partition >= 0)
        {
          mesh.GetCoords( mesh.local_to_global_index_mappings_partitions[partition].at(_NNList->at(j)), q_tmp);
          
          q[0] += q_tmp[0];
          q[1] += q_tmp[1];
        }

        else
        {
          mesh.GetCoords( mesh.local_to_global_index_mappings_interfaces[interface].at(_NNList->at(j)), q_tmp); 
          
          q[0] += q_tmp[0];
          q[1] += q_tmp[1];
        }
      } 
      
      double h1 = (1.0 / static_cast<double>(_NNList->size()) );

      vertex[0] = h1 * q[0];
      vertex[1] = h1 * q[1];  
      // end of calculate new position

      //set the new coordinates of vertex i
      mesh.SetCoords(i, vertex);

    } //end of for loop over global indices
  } //end of for loop number of iterations

  return true;
}
//end of Simple Laplace Smoother

//GroupedPartitionsSmooth::SimpleLaplaceOnGroups(int iterations)
bool GroupedPartitionsSmooth::SimpleLaplaceOnGroups(int iterations)
{
  //assign partitions and corresponding interface
  int part1 = 0;
  int part2 = 1;

  //get boundary vector of current partition
  int * bdry = mesh.pragmatic_partitions[0]->get_boundaryTags();

  //for loop number of iterations
  for (size_t iter = 0; iter < iterations; ++iter)
  {
    std::cout << "Iteration " << iter+1 << "/" << iterations << std::endl;
    
    std::cout << "boundary nodes" << std::endl;
    for (size_t node = 0; node < mesh.pragmatic_partitions[0]->get_number_nodes(); ++node)
    {
      //if vertex is on global mesh boundary do not touch it
      if ( mesh.boundary_nodes_mesh[mesh.local_to_global_index_mappings_partitions[0].at(node)] || mesh.vertex_apperances[ mesh.local_to_global_index_mappings_partitions[0].at(node) ] >= 2 )
      {
        continue;
      }      
    }

/*  TODO: idea: iterate over the corresponding local_to_global_index_mappings and update the vertices if necessary in more than 1 pragmatic mesh!    
    for (auto it : mesh.local_to_global_index_mappings_partitions[part1])  
    {
      if (mesh.boundary_nodes_mesh[it.second])
      {
        continue;
      }
    
      if (mesh.vertex_appearances[it.second] >= 2)
      {
        std::unordered_map<index_t, index_t>::iterator position = mesh.global_to_local_index_mappings_interfaces[0].find(it.second);
        if ( position != mesh.global_to_local_index_mappings_interfaces[0].end() )
        {
          std::cout << it.second << " is in interface 0" << std::endl;
        }        
      }
    }

    for (auto it : mesh.local_to_global_index_mappings_partitions[part2])  
    {
      //std::cout << it.second << std::endl;
    }

    for (auto it : mesh.local_to_global_index_mappings_interfaces[0])  
    {
      //std::cout << it.second << std::endl;
    }
*/
  }
}
//end of GroupedPartitionsSmooth::SimpleLaplaceOnGroups(int iterations)

//----------------------------------------------------------------------------------------------------------------------------------------------//
//                                                                     End                                                                      //
//----------------------------------------------------------------------------------------------------------------------------------------------//

#endif
