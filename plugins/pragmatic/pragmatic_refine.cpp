#include "pragmatic_refine.hpp"

//standard includes
#include <vector>
#include <string>

//Pragmatic includes
#include "Refine.h"
#include "pragmatic_mesh.hpp"

#ifdef HAVE_OPENMP
#include <omp.h>
#endif

//
// Define the necessary types:
//
typedef viennagrid::result_of::region<MeshType>::type             RegionType;
typedef viennagrid::result_of::element<MeshType>::type            VertexType;
typedef viennagrid::result_of::point<MeshType>::type              PointType;
typedef viennagrid::result_of::element<MeshType>::type      	    CellType;

typedef viennagrid::result_of::cell_range<RegionType>::type       CellRange;
typedef viennagrid::result_of::iterator<CellRange>::type          CellIterator;

typedef viennagrid::result_of::element<MeshType>::type      	    CellType;
typedef viennagrid::mesh                                          MeshType;

namespace viennamesh
{
		pragmatic_refine::pragmatic_refine()	{}
		std::string pragmatic_refine::name() {return "pragmatic_refine";}

		void make_refinement(Mesh<double> *mesh, size_t geometric_dimension, size_t no_of_passes)
		{
			if (geometric_dimension == 2)
			{
				Refine<double,2> adapt(*mesh);
			
				for(int i=1; i<=no_of_passes; i++)
		  		{	
					std::cout << "Pass " << i << " of " << no_of_passes << std::endl;	//DEFAULT VALUE IS i<3; only 3 passes for refinement!
        				adapt.refine(sqrt(2.0));						//DEFAULT VALUE IS SQRT(2.0)
    		 		}				
			}

			else
			{
				Refine<double,3> adapt(*mesh);
			
				for(int i=1; i<=no_of_passes; i++)
		  		{	
					std::cout << "Pass " << i << " of " << no_of_passes << std::endl;	//DEFAULT VALUE IS i<3; only 3 passes for refinement!
        				adapt.refine(sqrt(2.0));						//DEFAULT VALUE IS SQRT(2.0)
    		 		}
			}
		} //end make_refinement

		bool pragmatic_refine::run(viennamesh::algorithm_handle &)
		{
		  double tic = omp_get_wtime();
		 						
		  //
		  // Get the types for a global vertex range and the corresponding iterator.
		  // This allows to traverse all vertices in the mesh.
		  // The first template argument to *_range<> denotes the enclosing body (here: the mesh),
		  //
		  typedef viennagrid::result_of::vertex_range<MeshType>::type       VertexRange;
		  typedef viennagrid::result_of::iterator<VertexRange>::type        VertexIterator;
			
		  //create mesh_handle to read input mesh			
		  mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
			
		  //create data_handle for optional inputsregio
		  data_handle<int> refinement_passes = get_input<int>("refinement_passes");

		  //create string_handle to get input filename, used for benchmark purposes to store the output in a file (see at the end of this file!!!)
		  string_handle input_file = get_input<string_handle>("input_file");

		  int no_of_passes;
		
		  //check if a value for refinement_passes has been provided, otherwise set it to a default value
		  if (refinement_passes.valid())
			no_of_passes = refinement_passes();
		  else
			no_of_passes = 10; 

		  Mesh<double> *mesh = nullptr;

		  mesh = convert(input_mesh(), mesh);
		
		  std::cout << std::endl << "Initial Mesh" << std::endl;
		  std::cout << "----------------------" << std::endl;
		  std::cout << "NNodes: " << mesh->get_number_nodes() << std::endl;
		  std::cout << "NElements: " << mesh->get_number_elements() << std::endl;
		  std::cout << "----------------------" << std::endl;

		  size_t geometric_dimension = viennagrid::geometric_dimension( input_mesh() );

		  //set up the metric
		  make_metric(mesh, geometric_dimension);

   		  // Refine<double,2> adapt(*mesh);
		  
    		  double tic_refine = 0;
	          double toc_refine = 0;    		  

		  tic_refine = omp_get_wtime();
		
		  //refine the mesh
		  make_refinement(mesh, geometric_dimension, no_of_passes);

		  toc_refine = omp_get_wtime();
		
   		mesh->defragment();	  

		  double toc_wo_reconversion = omp_get_wtime();

		  //write refined mesh into vtu-file using pragmatic library and data structure
		  //VTKTools<double>::export_vtu("examples/data/myfirsttask/pragmatic_refine", mesh);

		  //convert pragmatic mesh datastructure back into viennamesh data structure
		  //this is a first simple 2d example!!!
		  MeshType output_mesh;	
		  std::vector<VertexType> vertex_handles(mesh->get_number_nodes());	  
		  double *x_out;
		  x_out = new double [3];
		  const index_t *ptr_ENList=nullptr;
		 
		  size_t NNodes = mesh->get_number_nodes();
		  size_t NElements = mesh->get_number_elements();
	
		 /* float tmp_nn = (NNodes / 100) * 1;
		  float tmp_ne = (NElements / 100) * 1;
		  */
		 // std::cout << "Creating Vertices" << std::endl;

		  for (size_t i = 0; i < NNodes; ++i)
		  {	
		        mesh->get_coords(i, x_out);
			if (geometric_dimension==2)
			  vertex_handles[i]=viennagrid::make_vertex(output_mesh, viennagrid::make_point(x_out[0], x_out[1]));
			
			else
			  vertex_handles[i]=viennagrid::make_vertex(output_mesh, viennagrid::make_point(x_out[0], x_out[1], x_out[2]));

			/*if(i%(int)tmp_nn==0)
			  std::cout << (i/tmp_nn) << " %" << std::endl;*/
		  } 
	
		 // std::cout << std::endl << "Creating Elements" << std::endl;

		  for (size_t i = 0; i < NElements; ++i)
		  {
			//std::cout << "Element " << i << " of " << NElements << std::endl;
			ptr_ENList = mesh->get_element(i);
			
			if (geometric_dimension==2)
			  CellType cell = viennagrid::make_triangle(output_mesh, vertex_handles[ptr_ENList[0]], vertex_handles[ptr_ENList[1]], vertex_handles[ptr_ENList[2]]);
			
			else
			  CellType cell = viennagrid::make_tetrahedron(output_mesh, vertex_handles[ptr_ENList[0]], vertex_handles[ptr_ENList[1]], vertex_handles[ptr_ENList[2]], vertex_handles[ptr_ENList[3]]);

			/*if(i%(int)tmp_ne==0)
			  std::cout << (i/tmp_ne) << " %" << std::endl;*/
		  }

		  //set output mesh
		  set_output("mesh", output_mesh);	  

		  double toc_w_reconversion = omp_get_wtime();

		  //output results
		  double overall_time_w_reconversion = toc_w_reconversion-tic;
		  double overall_time_wo_reconversion = toc_wo_reconversion-tic;
		  double refine_time = toc_refine - tic_refine;

		  std::cout << std::endl << "Refined Mesh" << std::endl;
		  std::cout << "----------------------" << std::endl;
		  std::cout << "Number of Passes: " << no_of_passes << std::endl;
		  std::cout << "NNodes: " << mesh->get_number_nodes() << std::endl;
		  std::cout << "NElements: " << mesh->get_number_elements() << std::endl;
		  std::cout << "----------------------" << std::endl;
		/*  std::cout << "Number of Vertices: " << mesh->get_number_nodes() << std::endl;
		  std::cout << "Number of Elements: " << mesh->get_number_elements() << std::endl; 
		  std::cout << "Overall time with reconversion : " << overall_time_w_reconversion << std::endl;
		  std::cout << "Overall time without reconversion : " << overall_time_wo_reconversion << std::endl;
		  std::cout << "Time for refining: " << refine_time << " seconds." << std::endl;*/
		  
		  std::ofstream output;
		  std::string filename;
		 
		  std::size_t pos1 = input_file().find_last_of("/");
		  std::size_t pos2 = input_file().find(".vtu");
		  std::string tmp = input_file().substr(pos1+1,(pos2-pos1-1));
		  
		  filename="/home/lgnam/Desktop/benchmark_pragmatic_refine_";
		  filename+=tmp;
	   	  filename+=".txt";
		  
		  output.open(filename.c_str(), std::ofstream::out | std::ios_base::app);
		  output << static_cast<int>(no_of_passes) << " " << static_cast<int>(mesh->get_number_nodes()) << " " << static_cast<int>(mesh->get_number_elements()) << " " << overall_time_w_reconversion <<  \
		  " " << overall_time_wo_reconversion << " " << refine_time << std::endl; 

		  /*output  << "----------------------------------------" << std::endl << "Number of Passes: " << no_of_passes << std::endl << "NNodes: " << mesh->get_number_nodes() << std::endl \
		  << "NElements: " << mesh->get_number_elements() << std::endl << "Overall time : " << overall_time << " s. " << std::endl << "Time for refining: " << refine_time << " s." << std::endl;
		  std::cout  << "----------------------------------------" << std::endl << "Number of Passes: " << no_of_passes << std::endl << "NNodes: " << mesh->get_number_nodes() << std::endl \
		  << "NElements: " << mesh->get_number_elements() << std::endl << "Overall time : " << overall_time << " s. " << std::endl << "Time for refining: " << refine_time << " s." << std::endl;*/

		  output.close();

		  delete mesh;
		  delete x_out;

		  return true;
		} //end run()
}
