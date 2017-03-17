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
#include <time.h>

//TODO: use boost bimap to evade using two unordered maps per mesh for local-to-global- and global-to-local-indexing
#include <boost/bimap.hpp>

#include "grouped_partitions.hpp"
#include "grouped_partitions_smooth.hpp"


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
    //pragmatic_metis_partitioner::run(viennamesh::algorithm_handle &)
    //
    //Uses metis to partition the mesh (represented in pragmatic data structure)
    bool pragmatic_metis_partitioner::run(viennamesh::algorithm_handle &)
		{
      std::cout << name() << std::endl;

      //create mesh_handle to read input mesh			
		  mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
      string_handle filename = get_input<string_handle>("filename");
            
      //create data_handle for optional inputs
		  data_handle<int> region_count = get_input<int>("region_count");
/*
      output << "Benchmarking " << filename() << " using " << region_count()/2 << " threads " << std::endl;
      output  << "==================================================" << std::endl;
*/  
      //output-file
      ofstream output;
      std::string tmp = filename();      
      size_t pos = filename().find_last_of("/\\");
      size_t pos2 = filename().find(".vtu");
      std::string input_filename = filename().substr(pos+1, pos2-(pos+1));
      input_filename += "_";
      input_filename += std::to_string(region_count()/2);
      input_filename += ".txt";      
      output.open(input_filename.c_str(), ofstream::app);
      output << region_count()/2 << " ";

      //convert viennamesh into pragmatic mesh data structure
      Mesh<double> *mesh = nullptr;

		  mesh = convert(input_mesh(), mesh);
      mesh->create_boundary();

      //convert viennamesh into pragmatic mesh data structure
      Mesh<double> *serial_mesh = nullptr;

		  serial_mesh = convert(input_mesh(), serial_mesh);
      serial_mesh->create_boundary();

      //make_metric(mesh, 2); //it is not necessary to create a metric!
      //output << "SimpleLaplaceOnGroups_sections" << std::endl << "==================================================" << std::endl;
      GroupedPartitions Mesh1(mesh, region_count());  
      clock_t tic = clock();
      GroupedPartitionsSmooth Smoother1(Mesh1);
      clock_t toc = clock();
      //output << "Create Smoothing Object: " << static_cast<double>(toc - tic) / CLOCKS_PER_SEC << std::endl;

      //ProfilerStart("profile_simplelaplaceongroups.log");
      tic = clock();
      Smoother1.SimpleLaplaceOnGroups_sections(2);
      toc = clock();
      //ProfilerStop();
      //output << "SimpleLaplaceOnGroups: " << static_cast<double>(toc - tic) / CLOCKS_PER_SEC << std::endl;
      output << static_cast<double>(toc - tic) / CLOCKS_PER_SEC << std::endl;

      Smoother1.Evaluate();
/*
      tic = clock();
      Mesh1.WriteMergedMesh();
      toc = clock();
*/
      //output << "WriteMergedMesh: " << static_cast<double>(toc - tic) / CLOCKS_PER_SEC << std::endl  << "==================================================" << std::endl;
/*
      output << "Serial" << std::endl << "==================================================" << std::endl;
      GroupedPartitions Mesh2(serial_mesh, region_count());
      tic = clock();
      GroupedPartitionsSmooth Smoother2(Mesh2);
      toc = clock();
      output << "Create Smoothing Object: " << static_cast<double>(toc - tic) / CLOCKS_PER_SEC << std::endl;

      tic = clock();
      Smoother2.SimpleLaplace(2);
      toc = clock();
      output << "SimpleLaplace: " << static_cast<double>(toc - tic) / CLOCKS_PER_SEC << std::endl;

      tic = clock();
      Mesh2.WriteMergedMesh("examples/data/pragmatic_metis_partitioning/SimpleLaplace.vtu");
      toc = clock();
      output << "WriteMergedMesh: " << static_cast<double>(toc - tic) / CLOCKS_PER_SEC << std::endl << std::endl;
*/
      //Mesh1.PrintQuality();
      output.close();

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

      set_output("mesh", input_mesh);
      
      //delete pointer created at the beginning of pragmatic_metis_partitioner::run(viennamesh::algorithm_handle &)
      delete mesh;
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

      return true;
    } //end of bool pragmatic_metis_partitioner::run(viennamesh::algorithm_handle &)
}
