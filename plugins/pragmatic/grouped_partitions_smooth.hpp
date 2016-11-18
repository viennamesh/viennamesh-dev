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

  std::string output = "LaplaceSmoothing_";
  output += std::to_string(no_iterations);

  return true;
}
//end of Simple Laplace Smoother

//----------------------------------------------------------------------------------------------------------------------------------------------//
//                                                                     End                                                                      //
//----------------------------------------------------------------------------------------------------------------------------------------------//

#endif
