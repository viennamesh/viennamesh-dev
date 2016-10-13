#include "pragmatic_smooth.hpp"

//standard includes
#include <vector>
#include <string>

//Pragmatic includes
#include "Smooth.h"
#include "pragmatic_mesh.hpp"

#ifdef HAVE_OPENMP
#include <omp.h>
#endif

typedef viennagrid::result_of::element<MeshType>::type      	    CellType;

namespace viennamesh
{
		pragmatic_smooth::pragmatic_smooth()	{}
		std::string pragmatic_smooth::name() {return "pragmatic_smooth";}

		void inline make_metric(Mesh<double> *mesh, size_t geometric_dimension)
		{
			if(geometric_dimension == 2)
			{
				MetricField<double,2> metric_field(*mesh);

    		  		size_t NNodes = mesh->get_number_nodes();
    		  		double eta=0.0001;

    		  		std::vector<double> psi(NNodes);

				for(size_t i=0; i<NNodes; i++) 
		  		{
					// Want x,y ranging from -1, 1
        				double x = 2*mesh->get_coords(i)[0]-1;
        				double y = 2*mesh->get_coords(i)[1]-1;

        				psi[i] = 0.100000000000000*sin(50*x) + atan2(-0.100000000000000, (double)(2*x - sin(5*y)));			
    		  		}

				metric_field.add_field(&(psi[0]), eta, 1);
    		 		metric_field.update_mesh();
			}

			else
			{
				MetricField<double,3> metric_field(*mesh);

    		  		size_t NNodes = mesh->get_number_nodes();
    		  		double eta=0.0001;

    		  		std::vector<double> psi(NNodes);

				for(size_t i=0; i<NNodes; i++) 
		  		{
					psi[i] = pow(mesh->get_coords(i)[0], 4) + pow(mesh->get_coords(i)[1], 4) + pow(mesh->get_coords(i)[2], 4);
				}

				metric_field.add_field(&(psi[0]), eta, 1);
    		 		metric_field.update_mesh();
			}
		}//end make_metric

		void make_smoothing(Mesh<double> *mesh, size_t geometric_dimension, std::string type)
		{
			if ( type.compare("laplacian") )
			{
				std::cout << "LAPLACIAN SMOOTHING" << std::endl;
			}

			else if ( type.compare("smart_laplacian") )
			{
				std::cout << "SMART LAPLACIAN SMOOTHING" << std::endl;
			}

			else if ( type.compare("optimisation_linf") )
			{
				std::cout << "OPTIMISATION LINF SMOOTHING" << std::endl;
			}
		}
		
		bool pragmatic_smooth::run(viennamesh::algorithm_handle &)
		{
		  double tic = omp_get_wtime();
		  //
		  // Define the necessary types:
		  //
		  typedef viennagrid::mesh                                          MeshType;
		  typedef viennagrid::result_of::region<MeshType>::type             RegionType;

		  typedef viennagrid::result_of::element<MeshType>::type            VertexType;
		  typedef viennagrid::result_of::point<MeshType>::type              PointType;
		  typedef viennagrid::result_of::element<MeshType>::type      	    CellType;

		  typedef viennagrid::result_of::cell_range<RegionType>::type       CellRange;
		  typedef viennagrid::result_of::iterator<CellRange>::type          CellIterator;
						
		  //
		  // Get the types for a global vertex range and the corresponding iterator.
		  // This allows to traverse all vertices in the mesh.
		  // The first template argument to *_range<> denotes the enclosing body (here: the mesh),
		  //
		  typedef viennagrid::result_of::vertex_range<MeshType>::type       VertexRange;
		  typedef viennagrid::result_of::iterator<VertexRange>::type        VertexIterator;
			
		  //create mesh_handle to read input mesh			
		  mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
		  string_handle smoothing_algorithm = get_input<string_handle>("smoothing_algorithm");
		  data_handle<int> smoothing_passes = get_input<int>("smoothing_passes");

		  //create string_handle to get input filename, used for benchmark purposes to store the output in a file (see at the end of this file!!!)
		  string_handle input_file = get_input<string_handle>("input_file");

		  //Create output
		  set_output("mesh", input_mesh());
	
		  return true;
		} //end run()
}
