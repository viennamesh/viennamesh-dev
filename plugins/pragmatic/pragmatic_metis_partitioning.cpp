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

//TODO: use boost bimap to evade using two unordered maps per mesh for local-to-global- and global-to-local-indexing
#include <boost/bimap.hpp>

#include "grouped_partitions.hpp"
#include "grouped_partitions_smooth.hpp"

//Defines
#define NUM_THREADS 2
#define MAX_NUM_LOOPS 1

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
            
      //create data_handle for optional inputs
		  data_handle<int> region_count = get_input<int>("region_count");

      //convert viennamesh into pragmatic mesh data structure
      Mesh<double> *mesh = nullptr;

		  mesh = convert(input_mesh(), mesh);
      mesh->create_boundary();

      //make_metric(mesh, 2); //it is not necessary to create a metric!

      GroupedPartitions Mesh1(mesh, region_count());

      GroupedPartitionsSmooth Smoother1(Mesh1);
      Smoother1.SimpleLaplace(2);

      Mesh1.WriteMergedMesh();

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

      set_output("mesh", input_mesh);
      
      //delete pointer created at the beginning of pragmatic_metis_partitioner::run(viennamesh::algorithm_handle &)
      delete mesh;
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

      return true;
    } //end of bool pragmatic_metis_partitioner::run(viennamesh::algorithm_handle &)
}
