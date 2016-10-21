#include "pragmatic_metis_partitioning.hpp"

//standard includes
#include <vector>
#include <string>
#include <cassert>

//Pragmatic includes
#include "pragmatic_mesh.hpp"
#include "Smooth.h"

#ifdef HAVE_OPENMP
#include <omp.h>
#endif

#include "metis.h"
#include <unordered_map>


//Defines
#define NUM_THREADS 2

//
// Define the necessary types:
//
typedef viennagrid::result_of::region<MeshType>::type                   RegionType;
typedef viennagrid::result_of::element<MeshType>::type                  VertexType;
typedef viennagrid::result_of::point<MeshType>::type                    PointType;
typedef viennagrid::result_of::element<MeshType>::type      	          CellType;

typedef viennagrid::result_of::cell_range<RegionType>::type             CellRange;
typedef viennagrid::result_of::iterator<CellRange>::type                CellIterator;

typedef viennagrid::result_of::const_cell_range<MeshType>::type         ConstCellRangeType;
typedef viennagrid::result_of::iterator<ConstCellRangeType>::type       ConstCellRangeIterator;

typedef viennagrid::result_of::element<MeshType>::type                  ElementType;
typedef viennagrid::result_of::const_element_range<ElementType>::type   ConstElementRangeType;
typedef viennagrid::result_of::iterator<ConstElementRangeType>::type    ConstElementRangeIterator;


typedef viennagrid::result_of::element<MeshType>::type      	          CellType;
typedef viennagrid::mesh                                                MeshType;

namespace viennamesh
{
		pragmatic_metis_partitioner::pragmatic_metis_partitioner()	{}
		std::string pragmatic_metis_partitioner::name() {return "pragmatic_metis_partitioner";}

    //
    //void laplace_smoother()
    //
    //TODO: implement a laplacian smoothing algorithm
    void laplace_smoother(Mesh<double>*& mesh, size_t max_iterations = 4)
    {
      std::cout << "Laplace smoother" << std::endl;
/*
      std::cout << &mesh << std::endl;
*/
      int NNodes = mesh->get_number_nodes();
      int NElements = mesh->get_number_elements();
      int nloc = mesh->get_nloc();

      std::vector<std::atomic<bool>> is_boundary(NNodes);
      std::vector<std::atomic<bool>> active_vertices(NNodes);

      //find all boundary nodes
      //TODO: find partition boundary nodes and parallalize this section
      
      //initalize is_boundary vector      
      for (size_t i = 0; i < NNodes; ++i)
      {
        is_boundary[i].store(false, std::memory_order_relaxed);
      }

      for(int n=0; n<NNodes; ++n) 
      {
        is_boundary[n].store(false, std::memory_order_relaxed);

        std::vector<index_t>* _NNList = mesh->get_nnlist(n);   
        std::vector<int>::iterator boundary_iterator = mesh->get_boundary();
        std::vector<int>* boundary = mesh->get_boundary_vector();
       // std::vector<std::vector<index_t>>::iterator NNList_iterator = mesh->get_nnlist_iterator();  
/*
        NNList_iterator += n;
       
        std::cout << n << ": " << std::endl;
        
        for (std::vector<index_t>::iterator it = NNList_iterator->begin(); it != NNList_iterator->end(); ++it)
        {
          std::cout << "  " << *it << std::endl;
        }

        //std::cout << n << ": " << (_NNList->size()) << std::endl << std::endl;
        std::cout << n << ": " << std::endl;

        for (size_t i = 0; i < _NNList->size(); ++i)
        {
          std::cout << "  " << _NNList->at(i) << std::endl;
        }
        std::cout << std::endl;
*/ 
/* 
        for (std::vector<index_t>::iterator)
        {
          
        }
*/      
        if(_NNList->empty())
        {
          active_vertices[n].store(false, std::memory_order_relaxed);
        } 

        else 
        {
          active_vertices[n].store(true, std::memory_order_relaxed);
        }
      
        for (size_t i = 0; i < NElements; ++i)
        {
          const int *n = mesh->get_element(i);

          if (n[0] < 0) //if element is marked for deletion (its first node in ENList is set to -1), we skip it
          {
            continue;
          }

          for (size_t j = 0; j < nloc; ++j)
          {
            boundary_iterator = mesh->get_boundary();
            std::advance(boundary_iterator, i*nloc+j);

            if (*boundary_iterator > 0)
            {
              for (size_t k = 1; k < nloc; ++k)
              {
                is_boundary[n[(j+k)%nloc]].store(true, std::memory_order_relaxed);
              }
            }
          }
        }
       }
      //end of find all boundary nodes section

      //output boundary vector for developing and debugging purposes
/*
      std::cout << "IS BOUNDARY" << std::endl;
      
      for (size_t i = 0; i < is_boundary.size(); ++i)
      {
        std::cout << i << ": " << is_boundary[i] << std::endl; 
      }
      std::cout << std::endl;
*/
      //TODO: now implement the laplacian smoothing algorithm here!!!!
      // in a first try, simply smooth the test mesh with all vertices and in a second try use the partition grouping 

      std::vector<double> coords(NNodes*2);
      std::vector<std::atomic<bool>> invalid_area(NNodes);
      //set new position according to the formula from "Glättung von Polygonnetzen in medizinischen Visualisierungen-Jens Haase", p.31
      for(size_t i = 0; i < NNodes; ++i)
      {
        if ( is_boundary[i].load(std::memory_order_relaxed) )
        { 
          continue;
        }
        const int *n = mesh->get_element(i);
        
        ElementProperty<double> *property;
        property = new ElementProperty<double>(mesh->get_coords(n[0]),
                                               mesh->get_coords(n[1]),
                                               mesh->get_coords(n[2]));

        double p[2] = {0.0, 0.0};

        std::vector<index_t>* _NNList = mesh->get_nnlist(i); //get Node-Node-adjacency list
       // const double *coords_ptr = mesh->get_coords(i);
        
        double q[2] = {0.0, 0.0};        

     //  std::cout << i << std::endl;

        for (size_t j = 0; j < _NNList->size()-1; ++j)
        {
          double q_tmp[2] = {0.0, 0.0};
          mesh->get_coords(_NNList->at(j), q_tmp);

          q[0] += q_tmp[0];
          q[1] += q_tmp[1];
        }
          
        double num_neighbors = _NNList->size();

        p[0] = (1/num_neighbors) * q[0];
        p[1] = (1/num_neighbors) * q[1];

        //check for validity
        
        double area;
        for (size_t j = 0; j < _NNList->size(); ++j)
        {
          const int *node = mesh->get_element(j);
          const double *x0 = mesh->get_coords(node[0]);
          const double *x1 = mesh->get_coords(node[1]);
          const double *x2 = mesh->get_coords(node[2]);
        
          if ( node[0] == i )
          {
            area = property->area(p, x1, x2);
          }

          else if ( node[1] == i )
          {
            area = property->area(x0, p, x2);
          }

          else if ( node[2] == i )
          {
            area = property->area(x0, x1, p);
          }

          if ( area < 0)
            {
              invalid_area[i].store(true, std::memory_order_relaxed);
              continue;
            }
          
          invalid_area[i].store(false, std::memory_order_relaxed);
        }

        coords[i*2]   = p[0];
        coords[i*2+1] = p[1];
     
      //  mesh->set_coords(i, p);
     //   std::cout << endl;
        //std::cout << i << ": " << _NNList->size() << std::endl;
      }

      for (size_t n = 0; n < NNodes; ++n)
      {
        double p[2];

        if ( is_boundary[n].load(std::memory_order_relaxed) || invalid_area[n].load(std::memory_order_relaxed))
        {
          mesh->get_coords(n, p);
          //std::cout << n << ": " << p[0] << ", " << p[1] << std::endl;
          continue;
        }

        p[0] = coords[n*2];
        p[1] = coords[n*2+1];
    
        //std::cout << n << ": " << p[0] << ", " << p[1] << std::endl;
        mesh->set_coords(n, p);
      }

      size_t iter = 0;
      while (iter < max_iterations)
      {
        ++iter;
      } //end of while loop
      //end of laplacian smoothing algorithm
    
      return;
    }

    //
    //binomial_coefficient(int n, int k)
    //
    //Returns the binomial coefficient n! / ( k! (n-k)! )
    //
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

    //
    //create_pragmatic_meshes()
    //
    //create pragmatic meshes for debugging purposes
    void create_pragmatic_meshes()
    {
    } //end of create_pragmatic_meshes()
  
    //
    //pragmatic_metis_partitioner::run(viennamesh::algorithm_handle &)
    //
    //Uses metis to partition the mesh (represented in pragmatic data structure)
    bool pragmatic_metis_partitioner::run(viennamesh::algorithm_handle &)
		{
      std::cout << name() << std::endl;

      //create mesh_handle to read input mesh			
		  mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
            
      //create data_handle for optional inputs
		  data_handle<int> region_count = get_input<int>("region_count");

      //convert viennamesh into pragmatic mesh data structure
      Mesh<double> *mesh = nullptr;

		  mesh = convert(input_mesh(), mesh);
      mesh->create_boundary();
      make_metric(mesh, 2);

      //pair of arrays storing the mesh as described in the metis manual
      std::vector<idx_t> eptr;
      std::vector<idx_t> eind;

      eptr.push_back(0);

      //fill eptr and eind, as done in viennamesh plugin "metis", file "mesh_partitionig.cpp"
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

      idx_t num_nodes = mesh->get_number_nodes();
      idx_t num_elements = mesh->get_number_elements();

      idx_t ncommon = mesh->get_number_dimensions();
      idx_t nparts = region_count();

      std::vector<idx_t> epart(num_elements);
      std::vector<idx_t> npart(num_nodes);                //nparts not npart!!!

      idx_t result;

      std::cout << "Calling METIS_PartMeshDual " << std::endl;

      //Call Metis Partitioning Function (see metis manual for details on the parameters and on the use of the metis API)
      METIS_PartMeshDual (&num_elements,
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
                          npart.data());
      
      /*METIS_PartMeshNodal(&num_elements,
                            &num_nodes,
                            eptr.data(),
                            eind.data(),
                            NULL,
                            NULL,
                            &nparts,
                            NULL,
                            NULL,
                            NULL,
                            epart.data(),
                            npart.data());
*/

      int nloc = 3;   //nlocal is the number of nodes of an element (3 for a triangle in 2D and 4 for a tetrahedron in 3D)
           
      ofstream output;
      output.open("data.txt");

      //TODO: get boundary elements (boundary is really the boundary of the mesh, not the partition interfaces!)
      //   
      //IDEA: use the is_owned_node function from pragmatic to determine partition boundary and build partition interface
      std::vector<std::set<int>> NEList = mesh->get_node_element(); //TODO: replace this, since its unnecessarily copying data!
      std::vector<index_t> _ENList = mesh->get_element_node();  //TODO: replace this, since its unnecessarily copying data!
      std::vector<int> boundary;
/*
      //get nodes per partition
      std::vector<std::set<index_t>> nodes_per_partition( nparts );

      for (size_t i = 0; i < num_elements; ++i)
      {
        //TODO: the following hast to be updated for the 3D case!!
        nodes_per_partition[ epart[i] ].insert(_ENList[i*3]);
        nodes_per_partition[ epart[i] ].insert(_ENList[i*3+1]);
        nodes_per_partition[ epart[i] ].insert(_ENList[i*3+2]);
      } 
      
      int counter =0;
      output << "NODES" << std::endl;
      for (auto it : nodes_per_partition)
      {
        output << "Partition " << counter << std::endl;
        output << it.size() << std::endl;
        for (auto node : it)
        {
          //output << node << std::endl;
        }      
        ++counter;
      }
 
     //get the interface nodes for all partition boundaries using vector<set<index_t>>
      std::vector<std::set<index_t>> interface_nodes( binomial_coefficient(nparts, 2) ); //We always intersect 2 partitions ==> k=2
      counter = 0;
      double int_tic = omp_get_wtime();
      //for (size_t i = 0; i < nparts; ++i)
      for (size_t i = 0; i < nparts; ++i)      
      {        
        for (size_t j = i+1; j < nparts; ++j)
        {   
          set_intersection( nodes_per_partition[i].begin(), nodes_per_partition[i].end(),
                            nodes_per_partition[j].begin(), nodes_per_partition[j].end(),
                            inserter(interface_nodes[counter], interface_nodes[counter].begin()) );
          ++counter;
        }      
      }
      double int_toc = omp_get_wtime();
      std::cout << "Time to get interface nodes serially: " << int_toc - int_tic << std::endl; 

      output << "INTERFACE NODES" << std::endl;
      counter =0;
      for (auto interfaces : interface_nodes)
      {
        //if (interfaces.size() != 0)
          output << "Interface " << counter << std::endl;
          output << interfaces.size() << std::endl;

        for (auto it : interfaces)
        {         
          //if (interfaces.size() == 0)
            //continue;m

          //output << it << std::endl;
        } 
        ++counter;
      }
      output << std::endl;
      //end of get partition interface using vector<set>

      //get boundary
/*
      boundary.resize(num_elements*nloc);
      std::fill(boundary.begin(), boundary.end(), -2);    

      for(size_t i=0; i<num_elements; i++) 
      {
        for(int j=0; j<nloc; j++) 
        {
          //TODO: the following has to be updated for the 3D case!!
          int n1 = _ENList[i*3+(j+1)%3];
          int n2 = _ENList[i*3+(j+2)%3];

          //change this if-clause to check if the node is in the actual partition //TODO is not really working!!!
          //if(npart[n1] == npart[n2]) 
          //if (true)
          //{
            std::set<int> neighbours;
            set_intersection(NEList[n1].begin(), NEList[n1].end(),
                             NEList[n2].begin(), NEList[n2].end(),
                             inserter(neighbours, neighbours.begin()));

            if(neighbours.size()==2) 
            {
              if(*neighbours.begin()==(int)i)
                boundary[i*3+j] = *neighbours.rbegin();
              else
                boundary[i*3+j] = *neighbours.begin();
            }
           //} 

           else 
           {
            // This is a halo facet.
            boundary[i*3+j] = -1;
           }
         }
      }
*/
      //end TODO get boundary
    
      //TODO: get partition interface elements
/*
      //create vector containing the cell indices of each element
      std::vector< std::set<index_t> > elements_per_region(nparts);
      for(size_t i = 0; i < num_elements; i++)
      {
        elements_per_region[ epart[i] ].insert(i);
      }  
  
      output << "NUMBER OF ELEMENTS PER REGION " << std::endl;
      counter = 0;
      for (auto it : elements_per_region)
      {
        output << "Region " << counter << ": " << it.size() << std::endl;
        ++counter;
      }
      output << std::endl;

      //TODO: get partition boundary elements
      counter = 0;
      std::vector< std::set<index_t> > partition_boundary_elements(interface_nodes.size());
      for(auto interfaces : interface_nodes)
      {
        //output << "Boundary No. " << counter << ":" << std::endl;
        for (auto it : interfaces)
        {            
          //output << it << ": " << std::endl;
          for(auto NE_it : NEList[it])
          {
            //output << "   " << NE_it << std::endl;
            partition_boundary_elements[counter].insert(NE_it);
          }
        } 
        ++counter;
      }

      output << "PARTITION INTERFACE ELEMENTS" << std::endl;  
      counter = 0;
      for (auto it : partition_boundary_elements)
      {
        output << "Interface " << counter << " containing " << it.size() << " elements" << std::endl;
        for (auto it2 : it)
        {
          //output << it2 << std::endl;
        }

        ++counter;
      }
      output << std::endl;
  
      output << "DUPLICATED INTERFACE ELEMENTS" << std::endl;
      std::set<index_t> degenerate_elements;
      for (size_t i = 0; i < partition_boundary_elements.size(); ++i)
      {
        std::vector<index_t> temp;
        for (size_t j = i+1; j < partition_boundary_elements.size(); ++j)
        {
          
          set_intersection( partition_boundary_elements[i].begin(), partition_boundary_elements[i].end(),
                            partition_boundary_elements[j].begin(), partition_boundary_elements[j].end(),
                            inserter(temp, temp.begin()) );

          for (auto it : temp)
          {
            degenerate_elements.insert(it);
          }
        }
      }

      for (auto it : degenerate_elements)
      {
        //output << it << std::endl;
      }
      output << std::endl;

      //TODO: remove partition interface elements from the actual partitions
      for (size_t i = 0; i < partition_boundary_elements.size(); ++i)
      {
        for (size_t j = 0; j < elements_per_region.size(); ++j)
        {
          for (auto it = partition_boundary_elements[i].begin(); it != partition_boundary_elements[i].end(); ++it)
          { 
            //std::cout << *it << std::endl; 
            elements_per_region[j].erase(*it);
          }
        }
      }

      //for developing and debugging purposes only
      output << "ELEMENTS PER REGION AFTER REMOVING INTERFACE ELEMENTS " << std::endl;
      counter = 0;
      for (auto it : elements_per_region)
      {
        output << "Region " << counter << " now consists of " << it.size() << " elements" << std::endl;
        for (auto it2 : it)
        {
          //output << it2 << std::endl;
        }
        ++counter;
      }
      output << std::endl;

      //count element appearances
      
      std::vector<size_t> num_of_appearances(num_elements, 0);
/*
      counter=0;

      double tic_appearances = omp_get_wtime();
        
      #pragma omp parallel for
      for (size_t i = 0; i < num_elements; ++i)
      {
        std::cout << i << " / " << num_elements << std::endl;
        for (auto it : elements_per_region)
        {
          num_of_appearances[i] += count(it.begin(), it.end(), i);
        }    
        for (auto it : partition_boundary_elements)
        {
          num_of_appearances[i] += count(it.begin(), it.end(), i);
        }
      }

      double toc_appearances = omp_get_wtime();
      std::cout << "Time for counting in parallel: " << toc_appearances - tic_appearances << std::endl;

      output << "NUMBER OF APPEARANCES" << std::endl;

      for (size_t i = 0; i < num_of_appearances.size(); ++i)
      {
        //output << i << ": " << num_of_appearances[i] << std::endl;
      } 
      output << "Accumulate: " << accumulate(num_of_appearances.begin(), num_of_appearances.end(), 0) << std::endl;

      output << std::endl;
      //end!
   */
      //output << std::endl;
      //end of get partition boundary elements
 /*     
      //for debugging and test purposes only
      output << "OLD COORDINATES" << std::endl;
      for (size_t i = 0; i<num_nodes; ++i)
      {
        double p[2];
        mesh->get_coords(i, p);
        //output << i << ": " << p[0] << ", " << p[1] << std::endl;
      }
      output << std::endl; //end of for debugging and test purposes only

      //laplace smoother
/*
      std::cout << "Plugin" << std::endl;
      std::cout << &mesh << std::endl;
*/
   //   laplace_smoother(mesh);

      //end of laplace smoother
/*
      //for debugging and test purposes only
      output << "NEW COORDINATES" << std::endl;
      for (size_t i = 0; i < num_nodes; ++i)
      {
        double p[2];
        mesh->get_coords(i, p);
        //output << i << ": " << p[0] << ", " << p[1] << std::endl;
      }
      output << std::endl; //end of for debugging and test purposes only
      
      //close output file (used for debugging and developing)
      output.close();

      //write pragmatic data file
      std::string filename;
      filename += "examples/data/pragmatic_metis_partitioning";
  
      VTKTools<double>::export_vtu(filename.c_str(), mesh);
*/
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//                                                                                                                                                                                      //
//TODO: IMPLEMENT ALL THE STUFF FROM BELOW IN A FAST AND COMPUTATIONALLY EFFICIENT WAY                                                                                                  //
//                                                                                                                                                                                      //
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
      //open second output file
      ofstream output2;
      output2.open("data2.txt");  

      //get the nodes present in each partition
      std::vector<std::set<index_t>> nodes_per_partition2( nparts );

      //this for-loop is not parallelizable, since set.insert() is not thread-safe
      for (size_t i = 0; i < num_elements; ++i)
      {
        //TODO: the following hast to be updated for the 3D case!!
        nodes_per_partition2[ epart[i] ].insert(_ENList[i*3]);
        nodes_per_partition2[ epart[i] ].insert(_ENList[i*3+1]);
        nodes_per_partition2[ epart[i] ].insert(_ENList[i*3+2]);
      }

      int counter = 0;
      output2 << "NODES" << std::endl;
      for (auto partition_iterator : nodes_per_partition2)
      {
        output2 << "Partition " << counter << std::endl;
        output2 << partition_iterator.size() << std::endl;
        ++counter;
        for (auto nit : partition_iterator)
        {
          //output2 << nit << std::endl;
        }
      }
   
      //get the interface nodes for all partition boundaries using vector<vector<vector<index_t>>>  
      std::vector<std::vector<std::vector<index_t>>> interface_sets (nparts-1);
 
      double tic = omp_get_wtime();
      //#pragma omp parallel for
      for (size_t i = 0; i < nparts; ++i)      
      {        
        for (size_t j = i+1; j < nparts; ++j)
        {   
          std::vector<index_t> tmp;

          set_intersection( nodes_per_partition2[i].begin(), nodes_per_partition2[i].end(),
                            nodes_per_partition2[j].begin(), nodes_per_partition2[j].end(),
                            inserter(tmp, tmp.begin()) );

          interface_sets[i].push_back(tmp);
        }      
      }
      double toc = omp_get_wtime();
      std::cout << "Time to get interface nodes in parallel: " << toc - tic << std::endl;

      output2 << "INTERFACE NODES" << std::endl;
      counter = 0;
      for (auto interface_vector : interface_sets)
      {
        for (auto it : interface_vector)
        {   
          output2 << "Interface " << counter << std::endl;   
          output2 << it.size() << std::endl;   
          for(auto it2 : it)
          {
            //output2 << it2 << std::endl;
          }
          ++counter;
        }         
      }
      output2 << std::endl;
      //end of get the interface nodes for all partition boundaries using vector<vector<vector<index_t>>>  

      //remove interface nodes from nodes_per_partition
      for (auto interface_nodes : interface_sets)
      {
        for (size_t i = 0; i < interface_nodes.size(); ++i)
        {
          for (size_t j = 0; j < nodes_per_partition2.size(); ++j)
          {
            for (auto it : interface_nodes[i])
            {
              nodes_per_partition2[j].erase(it);
            }
          }
        }
      }       
      //end of remove interface nodes from nodes_per_partition

      output2 << "NODES PER PARTITION AFTER REOMVING INTERFACE NODES" << std::endl;
      for (auto nip : nodes_per_partition2)
      {
        output2 << "New Partition containing " << nip.size() << " vertices" << std::endl;
        for (auto node : nip)
        { 
          output2 << node << std::endl;
        }
      }

      //get all elements per partition
      //TODO: seems unable to parallelize due to possibla data races with insert()-function 
      std::vector< std::set<index_t>> elements_per_partition(nparts);
      for(size_t i = 0; i < num_elements; i++)
      {
        elements_per_partition[ epart[i] ].insert(i);
      }  
  
      output2 << "NUMBER OF ELEMENTS PER PARTITION " << std::endl;
      counter = 0;
      for (auto it : elements_per_partition)
      {
        output2 << "Region " << counter << ": " << it.size() << std::endl;
        ++counter;
      }
      output2 << std::endl;
      //end of get all elements per partition

      //get partition interface elements
      counter = 0;
      std::vector<std::vector<std::set<index_t>>> interface_elements_sets(interface_sets.size());

      for (size_t i = 0; i < interface_sets.size(); ++i)
      {
        for (size_t j = 0; j < interface_sets[i].size(); ++j)
        {
          std::set<index_t> tmp;
          for (size_t k = 0; k < interface_sets[i][j].size(); k++)
          {            

            for (auto NE_it : NEList[interface_sets[i][j][k]])
            {
              tmp.insert(NE_it);
            }
          }
          interface_elements_sets[i].push_back(tmp);
          tmp.clear();
        }
      }

      output2 << "PARTITION INTERFACE ELEMENTS" << std::endl;  
      counter = 0;
      for (auto interface_set_iterator : interface_elements_sets)
      {
        for(auto interface_iterator : interface_set_iterator)
        {
          output2 << "Interface " << counter << " containing " << interface_iterator.size() << " elements" << std::endl;
          for (auto interface_element_iterator : interface_iterator)
          {
            //output2 << interface_element_iterator << std::endl;
          }
        }
        ++counter;
      }
      output2 << std::endl;
      //end of get partition interface elements    

      //add the vertices of the partition interface elements to interface_sets
      //TODO: this seems to be overload, since a interface_sets storing the interface nodes is already available
      std::vector<std::vector<std::set<index_t>>> interface_nodes(nparts-1);
      counter = 0;
      for (size_t i = 0; i < interface_elements_sets.size(); ++i)
      { 
        for (size_t j = 0; j < interface_elements_sets[i].size(); ++j)
        {
          std::set<index_t> tmp;

          for (auto interface_element : interface_elements_sets[i][j])
          {
            //std::cout << "Element " << interface_element << std::endl;
            //std::cout << _ENList[interface_element*3] << " "  << _ENList[interface_element*3+1] << " " << _ENList[interface_element*3+2] << std::endl;
            tmp.insert(_ENList[interface_element*3]);
            tmp.insert(_ENList[interface_element*3+1]);
            tmp.insert(_ENList[interface_element*3+2]);
          }
          interface_nodes[i].push_back(tmp);
        }
      }
      //end of add the vertices of the partition interface elements to interface_sets
      counter=0;
      for (auto interfaces : interface_nodes)
      {
        for(auto interface_iterator : interfaces)
        {
          //std::cout << "Interface " << counter << std::endl;
          for (auto interface_node_iterator : interface_iterator)
          {
            //std::cout << "  " << interface_node_iterator << std::endl;
          }
          ++counter;
        }
      }

      //get degenerated interface elements
      //TODO: this function is totally useless
/* 
     output2 << "DEGENERATED INTERFACE ELEMENTS" << std::endl;
      std::set<index_t> degenerate_elements2;
      for (size_t i = 0; i < partition_boundary_elements.size(); ++i)
      {
        std::vector<index_t> temp;
        for (size_t j = i+1; j < partition_boundary_elements.size(); ++j)
        {
          
          set_intersection( partition_boundary_elements[i].begin(), partition_boundary_elements[i].end(),
                            partition_boundary_elements[j].begin(), partition_boundary_elements[j].end(),
                            inserter(temp, temp.begin()) );

          for (auto it : temp)
          {
            degenerate_elements2.insert(it);
          }
        }
      }

      for (auto it : degenerate_elements2)
      {
        output2 << it << std::endl;
      }
      output2 << std::endl;  
      //end of get degenerated interface elements
*/
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
      } //end of remove partition interface elements from the actual partitions
    
      //print elements per partition after removing the interface elements
      output2 << "ELEMENTS PER PARTITION AFTER REMOVING INTERFACE ELEMENTS " << std::endl;
      counter = 0;
      for (auto it : elements_per_partition)
      {
        output2 << "Region " << counter << " now consists of " << it.size() << " elements" << std::endl;
        for (auto it2 : it)
        {
          //output << it2 << std::endl;
        }
        ++counter;
      }
      output2 << std::endl;
      //end of print elements per partition after removing the interface elements     
      
      //close second output file
      output2.close();

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//CREATE SEPARATED PRAGMATIC DATA STRUCTURES FOR EACH PARTITION AND EACH INTERFACE
//
      //vector storing the pragmatic meshes
      std::vector<Mesh<double>*> pragmatic_meshes;

      //vectors storing the coordinate
      std::vector< std::vector<double>> x_coords(nparts);
      std::vector< std::vector<double>> y_coords(nparts);
      std::vector< std::vector<double>> z_coords(nparts);

      //vector storing the ENLists of each region
      std::vector<std::vector<index_t>> ENLists_partitions(nparts);

      //vector storing the original vertices indices
     // std::vector<std::vector<index_t>> original_vertices_all_partitions(nparts);

      //loop over all partitions
      for (size_t i = 0; i < nparts; ++i)
      {
        //get number of vertices and elements
        int num_points = nodes_per_partition2[i].size();
        int num_cells = elements_per_partition[i].size();

        //get the vertex-to-index-mapping between old and new indices
        //TODO: use unordered_map instead, to speed up the code
        std::map <index_t, index_t> vertex_to_index_map;

        index_t new_vertex_id = 0;
        for (auto it : nodes_per_partition2[i])
        {
          vertex_to_index_map.insert( std::make_pair(it, new_vertex_id++) );
        }

        //pre-allocate memory
        x_coords[i].reserve(num_points);
        y_coords[i].reserve(num_points);
        ENLists_partitions[i].resize(3*num_cells);

        //get coordinates of each vertex
        counter = 0;
        for (auto it : nodes_per_partition2[i])
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
          
          ENLists_partitions[i][counter++] = vertex_to_index_map[*(element_ptr++)];
          ENLists_partitions[i][counter++] = vertex_to_index_map[*(element_ptr++)];
          ENLists_partitions[i][counter++] = vertex_to_index_map[*(element_ptr++)];          
        }

        //create pragmatic mesh 
        Mesh<double> *partial_mesh = nullptr;

        //TODO: change for 3D refinement
        //mesh = new Mesh<double> ( num_points, num_cells, &(ENLists_regions[region.id()][0]) ,&(x_coords[region.id()][0]), &(y_coords[region.id()][0]), &(z_coords[region.id()][0]) );        
        partial_mesh = new Mesh<double> ( num_points, num_cells, &(ENLists_partitions[i][0]), &(x_coords[i][0]), &(y_coords[i][0]) );
        partial_mesh->create_boundary();

/*      //TODO: it is not necessary to assign a metric to the pragmatic mesh
        std::cout << "pragmatic: " << partial_mesh->get_number_nodes() << " " << partial_mesh->get_number_elements() << std::endl;

        MetricField<double,2> metric_field(*partial_mesh);
        double h1 = 1.0/50;
        double h0 = 10.0/50;
        for (size_t j = 0; j < num_points; ++j)
        {
          double x = 2*partial_mesh->get_coords(i)[0] - 1;
          double y = 2*partial_mesh->get_coords(i)[1] - 1;
          double d = std::min(1-std::abs(x), 1-std::abs(y));

          double hx = h0 - (h1-h0)*(d-1);
          double m[] = {1.0/pow(hx, 2), 0, 1.0/pow(hx, 2)};
          metric_field.set_metric(m, j);
        }
        //metric_field.update_mesh();
*/
        pragmatic_meshes.push_back(partial_mesh);

        //delete partial_mesh;        //TODO: creates segfault if comments are removed
      } //end of loop over all partitions

      //write partitions    
      for (size_t i = 0; i < pragmatic_meshes.size(); ++i)
      {
        info(1) << "Writing partition " << i << std::endl;
        info(1) << "  Vertex count = " << pragmatic_meshes[i]->get_number_nodes() << std::endl;
        info(1) << "  Cell count = " << pragmatic_meshes[i]->get_number_elements() << std::endl;

        std::string filename;
        filename += "examples/data/pragmatic_partial_mesh_";
        filename += std::to_string( i );
  
        VTKTools<double>::export_vtu(filename.c_str(), pragmatic_meshes[i]);
      }
      //end of write partitions

      //DO THE SAME NOW WITH THE INTERFACES

      //vector storing the pragmatic meshes
      std::vector<Mesh<double>*> pragmatic_interface_meshes;

      //get number of interfaces
      int num_interfaces = binomial_coefficient(nparts,2);

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

          if (num_points == 0 || num_cells == 0)
          {
            pragmatic_interface_meshes.push_back(nullptr);
            continue;
          }

          //get the vertex-to-index-mapping between old and new indices
          std::map <index_t, index_t> interface_vertex_to_index_map;

          index_t new_vertex_id = 0;  
          for (auto it : interface_nodes[i][j])
          {
            interface_vertex_to_index_map.insert( std::make_pair(it, new_vertex_id++) );
          }

          //pre-allocate memory
          x_coords_interfaces[interface_counter].reserve(num_points);
          y_coords_interfaces[interface_counter].reserve(num_points);
          ENLists_interfaces[interface_counter].resize(3*num_cells);

          //get coordinates of each vertex
          //TODO: combine this loop with the next one!
          counter = 0;
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
          
            ENLists_interfaces[interface_counter][counter++] = interface_vertex_to_index_map[*(element_ptr++)];
            ENLists_interfaces[interface_counter][counter++] = interface_vertex_to_index_map[*(element_ptr++)];
            ENLists_interfaces[interface_counter][counter++] = interface_vertex_to_index_map[*(element_ptr++)];          
          }
          
          //create pragmatic mesh 
          Mesh<double> *interface_mesh = nullptr;

          //TODO: change for 3D refinement
          //mesh = new Mesh<double> ( num_points, num_cells, &(ENLists_regions[region.id()][0]) ,&(x_coords[region.id()][0]), &(y_coords[region.id()][0]), &(z_coords[region.id()][0]) );        
          interface_mesh = new Mesh<double> ( num_points, num_cells, &(ENLists_interfaces[interface_counter][0]), &(x_coords_interfaces[interface_counter][0]), &(y_coords_interfaces[interface_counter][0]) );
          interface_mesh->create_boundary();

          pragmatic_interface_meshes.push_back(interface_mesh);
        }
      }
      //end of loop over all interfaces

      //write interfaces
      for (size_t i = 0; i < pragmatic_interface_meshes.size(); ++i)
      {
        if (pragmatic_interface_meshes[i] == nullptr)
        {
          continue;
        }
        info(1) << "Writing interface " << i << std::endl;
        info(1) << "  Vertex count = " << pragmatic_interface_meshes[i]->get_number_nodes() << std::endl;
        info(1) << "  Cell count = " << pragmatic_interface_meshes[i]->get_number_elements() << std::endl;

        std::string filename;
        filename += "examples/data/pragmatic_partial_interface_";
        filename += std::to_string( i );
  
        VTKTools<double>::export_vtu(filename.c_str(), pragmatic_interface_meshes[i]);
      }        
      //end of write interfaces

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

      //COPIED FROM MESH_PARTITIONING.CPP INCLUDED IN METIS PLUGIN
      //ONLY HERE FOR DEBUGGING AND DEVELOPMENT PURPOSES
      data_handle<bool> multi_mesh_output = get_input<bool>("multi_mesh_output");
      mesh_handle output_mesh = make_data<mesh_handle>();

      ConstCellRangeType cells( input_mesh() );

      typedef viennagrid::result_of::element_copy_map<>::type ElementCopyMapType;

      if ( multi_mesh_output.valid() && multi_mesh_output() )
      {
        output_mesh.resize( region_count() );
        std::vector<ElementCopyMapType> copy_maps;

        for (int i = 0; i != region_count(); ++i)
        {
          ElementCopyMapType copy_map( output_mesh(i) );
          for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
          {
            int part = epart[(*cit).id().index()];
            if (part == i)
              copy_map( *cit );
          }
        }
      }
      else
      {
        ElementCopyMapType copy_map( output_mesh(), false );

        for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
        {
          ElementType cell = copy_map( *cit );
          viennagrid::add( output_mesh().get_or_create_region(epart[(*cit).id().index()]), cell );
        }
      }

      set_output( "mesh", output_mesh );
      //END OF DEBUGGING AND DEVELOPMENT CODE SNIPPET COPIED FROM METIS PLUGIN
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//


      return true;
    } //end of bool pragmatic_metis_partitioner::run(viennamesh::algorithm_handle &)
}
