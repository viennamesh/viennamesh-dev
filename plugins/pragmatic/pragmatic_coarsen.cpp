#include "pragmatic_coarsen.hpp"

//standard includes
#include <vector>

//Pragmatic includes
#include "pragmatic_mesh.hpp"
#include "Coarsen.h"

#ifdef HAVE_OPENMP
#include <omp.h>
#endif

#include<fstream>

namespace viennamesh
{
		pragmatic_coarsen::pragmatic_coarsen()	{}
		std::string pragmatic_coarsen::name() {return "pragmatic_coarsen";}

		bool pragmatic_coarsen::run(viennamesh::algorithm_handle &)
		{	 
			//
			// Define the necessary types:
			//

			typedef viennagrid::mesh                                          MeshType;
			typedef viennagrid::result_of::region<MeshType>::type             RegionType;

			typedef viennagrid::result_of::element<MeshType>::type            VertexType;
			typedef viennagrid::result_of::point<MeshType>::type              PointType;
			typedef viennagrid::result_of::element<MeshType>::type      	  CellType;

			typedef viennagrid::result_of::cell_range<RegionType>::type       CellRange;
			typedef viennagrid::result_of::iterator<CellRange>::type          CellIterator;
			
			
			viennagrid_mesh my_mesh;
			viennagrid_mesh_create(&my_mesh);
			
			//
			// Get the types for a global vertex range and the corresponding iterator.
			// This allows to traverse all vertices in the mesh.
			// The first template argument to *_range<> denotes the enclosing body (here: the mesh),
			//
			typedef viennagrid::result_of::vertex_range<MeshType>::type       VertexRange;
			typedef viennagrid::result_of::iterator<VertexRange>::type        VertexIterator;
			
			//create mesh_handle to read input mesh			
			mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
			
			//create data_handle for optional input region_count
		 	data_handle<int> region_count = get_input<int>("region_count");
						
			//check if region_count has been provided
			if (region_count.valid())
				std::cout << "pragmatic plugin called with region count = " << region_count() << std::endl;

			else
				std::cout << "pragmatic plugin called with only 1 region" << std::endl;
			
			//get topologic_dimension and geometric_dimension as well as the number of vertices and elements in the mesh
			double tic = omp_get_wtime();
			
			size_t cell_dimension = viennagrid::cell_dimension( input_mesh() );
			size_t geometric_dimension = viennagrid::geometric_dimension( input_mesh() );
			size_t NNodes = viennagrid::vertex_count( input_mesh() );
			size_t NElements = viennagrid::cells( input_mesh() ).size();
			/*
			//output mesh information
			std::cout << "cell_dimension: " << cell_dimension << std::endl;
			std::cout << "geometric_dimension: " << geometric_dimension << std::endl;
			std::cout << "NNodes: " << NNodes << std::endl;
			std::cout << "NElements: " << NElements << std::endl;
			*/
			//set up vectors for ENList and x-, y- nad z-coordinates
			std::vector<index_t> ENList;
			std::vector<double> x, y, z;
			
						
			/*VertexRange vertices(input_mesh() );
			std::vector<PointType> points;
			
			for (VertexIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
			{
			  points.push_back(viennagrid::get_point(input_mesh(), *vit) );
			}
			
			std::cout << "Points vector size: " << points.size() << std::endl;	
			*/
			
			//Initialize x-, y- and z-coordinates
			//
			//create pointer to iterate over viennagrid_array
			viennagrid_numeric *ptr_coords = nullptr;
						
			//get pointer to coordinates array from the mesh
			viennagrid_mesh_vertex_coords_pointer(input_mesh().internal(), &ptr_coords);			
			
			//iterate over all nodes in the mesh and store the coordinates in the vectors needed by pragmatic
			//(coordinates are stored in a big array in the following scheme [x0 y0 y0 x1 y1 z1 ... xn yn zn])
			for (size_t i=0; i<NNodes; ++i)
			{
			  
			  x.push_back(*(ptr_coords++));
			  y.push_back(*(ptr_coords++));
			  
			  //depending on the dimension push_back data from the array or push_back 0 into the pragmatic data
			  if (geometric_dimension == 3)
			  {
			    z.push_back(*(ptr_coords++));
			  }
			  else
			  {
			    z.push_back(0);
			  }
			}
			
			//Initialize ENList
			/*
			viennagrid_element_type ptr_el_type;
			viennagrid_dimension topo_dim;
			
			viennagrid_element_type_get(input_mesh().internal(), 0, &ptr_el_type);
			viennagrid_mesh_cell_dimension_get(input_mesh().internal(), &topo_dim);
			
			size_t dim;
			dim = topo_dim;
			
			size_t el = ptr_el_type;
			
			std::cout << "dim: " << dim << std::endl;
			std::cout << "el: " << el << std::endl;
			*/
			//Iterate over all triangles in the mesh
			viennagrid_element_id * vertex_ids_begin;
			viennagrid_element_id * vertex_ids_end;
			viennagrid_dimension topological_dimension = cell_dimension;		//produces segmentation fault if not set to 2 for 2d and to 3 for 3d case
			//TODO:THE SOLUTION IN THE ROW ABOVE IS OPTIMIZABLE!!!
			//get elements from mesh
			viennagrid_mesh_elements_get(input_mesh().internal(), topological_dimension, &vertex_ids_begin, &vertex_ids_end);
			
			viennagrid_element_id * boundary_vertex_ids_begin;
			viennagrid_element_id * boundary_vertex_ids_end;
			viennagrid_dimension boundary_topological_dimension = 0;
			viennagrid_element_id triangle_id;
						
			int counter = 0;
			
			//outer for loop iterates over all elements with dimension = topological_dimension (2 for triangles)
			//inner for loop iterates over all elements with dimension = boundary_topological_dimension (0 for vertices)
			//this info is needed to build the NEList which is used to build the pragmatic data structure
			for (viennagrid_element_id * vit = vertex_ids_begin; vit != vertex_ids_end; ++vit)
			{
			  //get vertices of triangle
			  triangle_id = *vit;
			  viennagrid_element_boundary_elements(input_mesh().internal(), triangle_id, boundary_topological_dimension, &boundary_vertex_ids_begin, &boundary_vertex_ids_end);
			  			  
			  for (viennagrid_element_id * vit = boundary_vertex_ids_begin; vit != boundary_vertex_ids_end; ++vit)
			  {
			    viennagrid_element_id vertex_id = *vit;
			    ENList.push_back(vertex_id);
			  }
			  		  
			  ++counter;
			}
			
			//Create Pragmatic mesh data structure
			Mesh<double> *mesh = nullptr;
			
			 if (geometric_dimension == 2)
		  	{
		    		mesh = new Mesh<double> (NNodes, NElements, &(ENList[0]), &(x[0]), &(y[0]));
		  	}
		  	
			else 
		  	{
				std::cout << NNodes << " " << NElements << " " << ENList.size() << " " << x.size() << " " << y.size() << " " << y.size() << std::endl;
		    		mesh = new Mesh<double> (NNodes, NElements, &(ENList[0]), &(x[0]), &(y[0]), &(z[0]));
		  	}
		  	
			mesh->create_boundary();
			
			// create metric field		  
		 	MetricField<double,2> metric_field(*mesh); //if already defined in the if-else-construct, compiler throws an error
		  	//MetricField<double,3> metric_field3d(*mesh); //because call to metric_field.update_mesh() does not know about metric_field
					
			for (size_t i=0; i<NNodes; i++)
			{
			  //2d case
			  if (geometric_dimension == 2)
			  {
				double m[] = {0.5, 0.0, 0.5};
			    	metric_field.set_metric(m,i);
			  }
			  
			  //3d case
			  else
			  {
			   	double m[] = {0.5, 0.0, 0.0, 0.5, 0.0, 0.5 };
			  	//metric_field3d.set_metric(m,i);
			  }				  
			}
			
		        if (geometric_dimension == 2)
			{
				metric_field.update_mesh();
			}

			else
			{
				//metric_field3d.update_mesh();
			}
			
			std::cout << std::endl << "Initial Mesh" << std::endl;
			std::cout << "----------------------" << std::endl;
			std::cout << "NNodes: " << mesh->get_number_nodes() << std::endl;
			std::cout << "NElements: " << mesh->get_number_elements() << std::endl;
			std::cout << "----------------------" << std::endl;
			
			//VTKTools<double>::export_vtu("input_test_coarsen_2d", mesh);
			
			Coarsen<double,2> adapt2d(*mesh);
			//Coarsen<double,3> adapt3d(*mesh);	//same as with metric_field above!!!
			
			double L_up = sqrt(2.0);
			double L_low = L_up*0.5;
	
			double tic_coarsening;
			double toc_coarsening;
			  
			if (geometric_dimension == 2)
			{
				tic_coarsening = omp_get_wtime();
				adapt2d.coarsen(L_low, L_up);
				toc_coarsening = omp_get_wtime();
			}

			else
			{
				tic_coarsening = omp_get_wtime();
				//adapt3d.coarsen(L_low, L_up);
				toc_coarsening = omp_get_wtime();
			}
			
			mesh->defragment();
			
			double toc = omp_get_wtime();
			
			std::cout << std::endl << "Coarsened Mesh" << std::endl;
			std::cout << "----------------------" << std::endl;
			std::cout << "NNodes: " << mesh->get_number_nodes() << std::endl;
			std::cout << "NElements: " << mesh->get_number_elements() << std::endl;
			std::cout << "----------------------" << std::endl;
			std::cout << std::endl << "Overall time : " << toc - tic << std::endl;
			std::cout << "Time for coarsening: " << toc_coarsening - tic_coarsening << " seconds." << std::endl;
			
			//write coarsened mesh into vtu-file using pragmatic library and data structure
			//VTKTools<double>::export_vtu("examples/data/myfirsttask/pragmatic_coarsen_mesh", mesh);
			
			/*
			 * The code snippet below simply outputs the ENList and the coordinates into txt-files
			std::ofstream output;
			output.open("coarsened_mesh_ENList.txt");
			
			for(auto & it : ENList)
			{			  
			  output << it << std::endl;
			}
			
			output.close();
			
			std::ofstream output_coords;
			output_coords.open("coarsened_mesh_cooords.txt");
			
			for(size_t i=0; i<x.size(); ++i)
			{			  
			  output_coords << x[i] << std::endl << y[i] << std::endl << z[i]  << std::endl;
			}
			
			output_coords.close();
			*/

			//convert pragmatic mesh datastructure back into viennamesh data structure
		  	MeshType output_mesh;	
		  	std::vector<VertexType> vertex_handles(mesh->get_number_nodes());	  
		  	double *x_out;
		  	x_out = new double [3];
		  	const index_t *ptr_ENList=nullptr;

		  	for (size_t i = 0; i < mesh->get_number_nodes(); ++i)
		  	{
		        	mesh->get_coords(i, x_out);
				if (geometric_dimension==2)
			  		vertex_handles[i]=viennagrid::make_vertex(output_mesh, viennagrid::make_point(x_out[0], x_out[1]));
			
				else
			  		vertex_handles[i]=viennagrid::make_vertex(output_mesh, viennagrid::make_point(x_out[0], x_out[1], x_out[2]));
		  	} 

		  	for (size_t i = 0; i < mesh->get_number_elements(); ++i)
		  	{
				ptr_ENList = mesh->get_element(i);
			
				if (geometric_dimension==2)
			  		CellType cell = viennagrid::make_triangle(output_mesh, vertex_handles[ptr_ENList[0]], vertex_handles[ptr_ENList[1]], vertex_handles[ptr_ENList[2]]);
			
				else
			  		CellType cell = viennagrid::make_tetrahedron(output_mesh, vertex_handles[ptr_ENList[0]], vertex_handles[ptr_ENList[1]], vertex_handles[ptr_ENList[2]], vertex_handles[ptr_ENList[3]]);
		  	}

		  	//set output mesh
		  	set_output("mesh", output_mesh);

			delete mesh;
			delete x_out;
			
			return true;
		}
		
}
