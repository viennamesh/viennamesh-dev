#include "color_refinement.hpp"
#include "pragmatic_mesh.hpp"
#include "mesh_partitions.hpp"
#include <fstream>
#include <chrono>

namespace viennamesh
{
		color_refinement::color_refinement()	{}
		std::string color_refinement::name() {return "color_refinement";}

		bool color_refinement::run(viennamesh::algorithm_handle &)
		{	
			//viennamesh::info(1) << "Color based mesh refinement" << std::endl;

			data_handle<pragmatic_wrapper::mesh> input_mesh = get_required_input<pragmatic_wrapper::mesh>("mesh");
			data_handle<int> num_partitions = get_required_input<int>("num_partitions");
			data_handle<int> num_threads = get_input<int>("num_threads");
			data_handle<bool> single_mesh_output = get_input<bool>("single_mesh_output");
			string_handle input_file = get_input<string_handle>("filename");
			string_handle algorithm = get_input<string_handle>("algorithm");
			string_handle coloring_algorithm = get_input<string_handle>("coloring");
			data_handle<int> max_num_iterations = get_required_input<int>("max_num_iterations");

			Mesh<double> * in_mesh = input_mesh().mesh;

			//input_mesh().mesh->multiply_coords(100);
		
			info(1) << name() << std::endl;
			
			size_t found = input_file().find_last_of("/");
			size_t find_vtu = input_file().find_last_of(".");
			info(1) << input_file().substr(found+1) << std::endl;

			//info(1) << "  Number of vertices: " << input_mesh().mesh->get_number_nodes() << std::endl;
			info(1) << "  Partitions: " << num_partitions() << std::endl;
      		info(1) << "  Dimension: " << input_mesh().mesh->get_number_dimensions() << std::endl;
			info(1) << "  Threads: " << num_threads() << std::endl;
			info(1) << "  Number of Iterations: " << max_num_iterations() << std::endl;
			/*
			std::ofstream file;
			file.open("matrix.mtx");

			file << "%%MatrixMarket matrix coordinate real general" << std::endl;
			file << input_mesh().mesh->get_number_nodes() << " " << input_mesh().mesh->get_number_nodes() << " 0" << std::endl;

			for (size_t i = 0; i < input_mesh().mesh->get_number_nodes(); ++i)
			{
				std::vector<int> nnlist = input_mesh().mesh->get_nnlist(i);

				for(size_t neigh = 0; neigh < nnlist.size(); ++neigh)
				{
					file << i+1 << " " << nnlist[neigh]+1 << " 0" << std::endl;
				} 
			}

			file.close();//*/

			//DEBUG
			//output .node and .ele files for tetgen
			ofstream node_file;
			std::string node_file_name = "examples/data/color_refinement/output/tetgen_";
			node_file_name += input_file().substr(found+1);
			node_file_name+=".node";

			node_file.open(node_file_name.c_str());

			node_file << "# Node count, " << in_mesh->get_number_dimensions() << " dim, no attribute, no boundary marker" << std::endl;
			node_file << in_mesh->get_number_nodes() << " " << in_mesh->get_number_dimensions() << " 0 0" << std::endl;
			
			for (size_t vid = 0; vid < in_mesh->get_number_nodes(); ++vid)
			{
				node_file << vid+1 << " " << input_mesh().mesh->get_coords(vid)[0] << " " << input_mesh().mesh->get_coords(vid)[1];
				node_file << " " << input_mesh().mesh->get_coords(vid)[2] << std::endl;
				/*
				node_file << vid+1 << " " << input_mesh().mesh->_coords[3*vid] << " " << input_mesh().mesh->_coords[3*vid+1];
				node_file << " " << input_mesh().mesh->_coords[3*vid+2] << std::endl;*/
			}
			node_file.close();

			ofstream ele_file;
			std::string ele_file_name = "examples/data/color_refinement/output/tetgen_";
			ele_file_name+= input_file().substr(found+1);
			ele_file_name+=".ele";

			ele_file.open(ele_file_name.c_str());

			ele_file << "# Tetrahedra count, 4 nodes per tetrahedron, no region attribute" << std::endl;
			ele_file << in_mesh->get_number_elements() << " 4 0" << std::endl;
			
			for (size_t vid = 0; vid < in_mesh->get_number_elements(); ++vid)
			{
				index_t const* eid = in_mesh->get_element(vid); 
				if (eid[0] == -1)
				{
					std::cerr << " ERROR: no defragmentation of tetrahedronlist detected!" << std::endl;
					continue;
				}

				ele_file << vid+1 << " " << eid[0]+1 << " " << eid[1]+1;
				ele_file << " " << eid[2]+1 << " " << eid[3]+1 << std::endl;

				/*
				ele_file << vid+1 << " " << in_mesh->_ENList[4*vid]+1 << " " << in_mesh->_ENList[4*vid+1]+1;
				ele_file << " " << in_mesh->_ENList[4*vid+2]+1 << " " << in_mesh->_ENList[4*vid+3]+1 << std::endl;*/
			}
			ele_file.close();

			//end of output .node and .ele files for tetgen
			//END OF DEBUG*/
			
			//check chosen algorithm and chosen coloring method
			std::string algo;
			std::string options;
			std::string coloring;
			
			if (!algorithm.valid())
			{
				algo = "pragmatic";

			}	

			else if (algorithm() == "pragmatic" || algorithm() == "triangle" || algorithm() == "tetgen" || algorithm() == "pragmaticcavity")
			{
				algo = algorithm();	

				viennamesh::info(1) << "Algorithm: " << algo << std::endl;
				/*
				if (algo == "triangle" || algo == "tetgen")
				{
					string_handle tri_options = get_input<string_handle>("options"); //standard-options for triangle: zpq
					options = tri_options();
				}*/
				string_handle options_handle = get_input<string_handle>("options");
				options = options_handle();

				if(algorithm()=="tetgen")
				{
					#ifdef USE_CGAL_PREDICATES
						viennamesh::info(3) << "  Using CGAL's exact arithmetic for Tetgen" << std::endl;
					#else
						viennamesh::info(3) << "  Using Shewchuk's exact arithmetic for Tetgen" << std::endl;
					#endif
				}
			}	

			else 
			{
				viennamesh::error(1) << "'" << algorithm() << "'" << " is not a valid algorithm!" << std::endl;
				return false;
			}		
			
			if (!coloring_algorithm.valid())
			{
				coloring = "greedy-lu";
			}

			else if (coloring_algorithm()=="greedy-lu" || coloring_algorithm()=="greedy" 
					|| coloring_algorithm() == "greedy-sched" || coloring_algorithm()=="parallel"
					|| coloring_algorithm() == "catalyurek" || coloring_algorithm()=="parallel-test"
					|| coloring_algorithm() == "catalyurek-test")
			{
				coloring = coloring_algorithm();

				if (coloring_algorithm() == "parallel-test" || coloring_algorithm() == "catalyurek-test")
				{
					string_handle col_options = get_input<string_handle>("options");
					options = col_options();
				}
			}

			else 
			{
				viennamesh::error(1) << "'" << coloring_algorithm() << "'" << " is not a valid coloring algorithm!" << std::endl;
				return false;
			}	

			MeshPartitions InputMesh(input_mesh().mesh, num_partitions(), input_file().substr(found+1), num_threads(), algorithm()); 

			//SERIAL PART
			auto overall_tic = std::chrono::system_clock::now();
			
			auto wall_tic = std::chrono::system_clock::now();
				InputMesh.MetisPartitioning();
			std::chrono::duration<double> partitioning_duration = std::chrono::system_clock::now() - wall_tic;
			viennamesh::info(1) << "  Partitioning time " << partitioning_duration.count() << std::endl;

			wall_tic = std::chrono::system_clock::now();
				InputMesh.CreateNeighborhoodInformation(max_num_iterations());
			std::chrono::duration<double> adjacency_duration = std::chrono::system_clock::now() - wall_tic;
			viennamesh::info(1) << "  Creating adjacency information time " << adjacency_duration.count() << std::endl;

			wall_tic = std::chrono::system_clock::now();
				InputMesh.ColorPartitions(coloring, input_file().substr(found+1, find_vtu-found-1));
			std::chrono::duration<double> coloring_duration = std::chrono::system_clock::now() - wall_tic;
			viennamesh::info(1) << "  Coloring time " << coloring_duration.count() << std::endl;

			/*InputMesh.GetMeshStats();
/*
			wall_tic = std::chrono::system_clock::now();
			bool valid_coloring = true;
			/*if ( !InputMesh.CheckPartitionColoring() )
			{
				viennamesh::error(1) << "Invalid Partition Coloring" << std::endl;
				valid_coloring = false;
			}//*/
			/*
			if ( !InputMesh.CheckVertexColoring() )
			{
				viennamesh::error(1) << "Invalid Vertex Coloring" << std::endl;
				valid_coloring = false;
			}//*//*
			std::chrono::duration<double> coloring_check_time = std::chrono::system_clock::now() - wall_tic;
			viennamesh::info(1) << "  Coloring Validation time " << coloring_check_time.count() << std::endl;//*/

			//PARALLEL PART
			std::chrono::duration<double> pragmatic_duration;
			std::vector<double> threads_log;
			std::vector<double> heal_log;
			std::vector<double> metric_log;
			std::vector<double> call_refine_log;
			std::vector<double> refine_log;
			std::vector<double> mesh_log;
			std::vector<double> nodes_log;
			std::vector<double> enlist_log;
			std::vector<size_t> workload;
			std::vector<size_t> workload_elements;
			std::vector<double> get_interfaces_log;
			std::vector<double> defrag_log;
			std::vector<double> refine_boundary_log;
			std::vector<double> smooth_log;
			std::vector<double> swap_log;
			auto for_time = 0.0;
			auto prep_time = 0.0;

			
			wall_tic = std::chrono::system_clock::now();
			/*InputMesh.CreatePragmaticDataStructures_par(threads_log, refine_times, l2g_build, l2g_access, g2l_build, g2l_access, 
														algo, options, triangulate_log, int_check_log);//, build_tri_ds); //*/
			InputMesh.CreatePragmaticDataStructures_par(algo, threads_log, heal_log, metric_log, call_refine_log, refine_log, mesh_log,
														for_time, prep_time, nodes_log, enlist_log, options, workload, workload_elements,
														max_num_iterations(), get_interfaces_log, defrag_log, refine_boundary_log,
														smooth_log, swap_log);//*/
														
			std::chrono::duration<double> cpds_duration = std::chrono::system_clock::now() - wall_tic;
			viennamesh::info(1) << 	"  Adaptation time " << cpds_duration.count() << std::endl;
			viennamesh::info(1) <<  "     Refinement time " << *std::max_element(call_refine_log.begin(), call_refine_log.end()) << std::endl;
			if (algo == "pragmatic" || algo == "pragmaticcavity")
			{				
				viennamesh::info(1) <<  "     Swapping time   " << *std::max_element(swap_log.begin(), swap_log.end()) << std::endl;
				viennamesh::info(1) <<  "     Smoothing time  " << *std::max_element(smooth_log.begin(), smooth_log.end()) << std::endl;
			}
			viennamesh::info(1) <<  "     Misc. time      " << cpds_duration.count() - *std::max_element(call_refine_log.begin(), call_refine_log.end()) - *std::max_element(swap_log.begin(), swap_log.end()) - *std::max_element(smooth_log.begin(), smooth_log.end()) << std::endl;

			std::chrono::duration<double> overall_duration = std::chrono::system_clock::now() - overall_tic;

			//InputMesh.ConsistencyCheck();
			
			int r_vertices =0;
			int r_elements =0;
			
			InputMesh.GetRefinementStats(&r_vertices, &r_elements, algo);

			std::ofstream csv;
			std::string csv_name;
			//std::string csv_name = "times_vertices_";
			csv_name+= input_file().substr(found+1, find_vtu-found-1);
			csv_name+="_";
			csv_name+=algo;
			csv_name+=".csv";

			//check if file is already existing
			ifstream ifile(csv_name.c_str());

			//write file header if file is not existing
			if ( !(bool) ifile )
			{
				csv.open(csv_name.c_str(), ios::app);
				csv << "File, Threads, Iterations, Vertices, Elements, Partitions, Colors, Partitioning [s], Adjacency [s], Coloring [s], CPDS [s],";
				csv << "Final Vertices, Final Elements, Total [s],";

				for (size_t i =0; i < heal_log.size(); ++i)
					csv << "Heal " << i << " [s],";

				for (size_t i =0; i < get_interfaces_log.size(); ++i)
					csv << "NNInterfaces " << i << " [s],";
				
				for (size_t i = 0; i < refine_boundary_log.size(); ++i)
					csv << "Refine Boundary " << i << " [s],";

				for (size_t i=0; i < defrag_log.size(); ++i)
					csv << "Defrag " << i << " [s],";

				for (size_t i =0; i < call_refine_log.size(); ++i)
					csv << "Call Refine " << i << " [s],";

				for (size_t i =0; i < refine_log.size(); ++i)
					csv << "Refine " << i << " [s],";

				for (size_t i =0; i < threads_log.size(); ++i)
					csv << "Threads " << i << " [s],";

				for (size_t i = 0; i < smooth_log.size(); ++i)
					csv << "Smooth " << i << " [s],";

				for (size_t i = 0; i < swap_log.size(); ++i)
					csv << "Swap " << i << " [s],";
				
				csv << std::endl;
			}

			else
			{
				csv.open(csv_name.c_str(), ios::app);
			}

			//csv << "File, Threads, Vertices, Elements, Desired Partitions, Created Partitions, Colors, Metis [s], Adjacency Info [s], 
			//Coloring [s], Parallel DSs [s], Prep [s], Nodes [s], g2l [s], l2g [s], Coords [s], ENList [s], new Mesh [s], Boundary [s], Metric [s],
			// Update Metric [s], Interface Check [s],  Refine [s], Create Refine [s], R-Vertices, R-Elements, Total [s], Thread Times in Color Loop [s]" << std::endl;
			csv << input_file().substr(found+1) << ", " << num_threads() << ", " << max_num_iterations() << ", " << in_mesh->get_number_nodes() << ", ";
			csv << in_mesh->get_number_elements() << ", "  << num_partitions();// << ", " << InputMesh.get_max()+1;
			csv << ", " << InputMesh.get_colors() << ", ";			
			
			csv << std::fixed << std::setprecision(8) << partitioning_duration.count() << ", ";
			csv << adjacency_duration.count() << ", ";
			csv << coloring_duration.count() << ", ";

			csv << cpds_duration.count() << ", ";
			csv << r_vertices << ", ";
	 		csv << r_elements << ", ";
			csv << overall_duration.count() << ", ";

			/*if (valid_coloring)
				csv << "valid, ";
			else
				csv << "invalid, ";
			csv << coloring_check_time.count() << ", ";
			/*
			for (size_t i = 0; i < l2g_build.size(); ++i)
			{
				csv << l2g_build[i] << ", ";
			}

			for (size_t i = 0; i < g2l_build.size(); ++i)
			{
				csv << g2l_build[i] << ", ";
			}

			for (size_t i = 0; i < l2g_access.size(); ++i)
			{
				csv << l2g_access[i] << ", ";
			}

			for (size_t i = 0; i < g2l_access.size(); ++i)
			{
				csv << g2l_access[i] << ", ";
			}
			*/
			/*
			csv << cpds_duration.count() << ", ";
			csv << prep_time << ", ";
			csv << for_time << ", ";
			csv << r_vertices << ", ";
	 		csv << r_elements << ", ";
			csv << overall_duration.count() << ", ";
/*
			for (size_t i =0; i < threads_log.size(); ++i)
				csv << threads_log[i] << ", ";

			for (size_t i = 0; i < nodes_log.size(); ++i)
				csv << nodes_log[i] << ", ";

			for (size_t i = 0; i < enlist_log.size(); ++i)
				csv << enlist_log[i] << ", ";

			for (size_t i =0; i < mesh_log.size(); ++i)
				csv << mesh_log[i] << ", ";
*/
			for (size_t i =0; i < heal_log.size(); ++i)
				csv << heal_log[i] << ", ";
/*
			for (size_t i =0; i < metric_log.size(); ++i)
				csv << metric_log[i] << ", ";
*/
			for (size_t i=0; i < get_interfaces_log.size(); ++i)
				csv << get_interfaces_log[i] << ", ";

			for (size_t i = 0; i < refine_boundary_log.size(); ++i)
				csv << refine_boundary_log[i] << ", ";

			for (size_t i=0; i < defrag_log.size(); ++i)
				csv << defrag_log[i] << ", ";

			for (size_t i =0; i < call_refine_log.size(); ++i)
				csv << call_refine_log[i] << ", ";

			for (size_t i =0; i < refine_log.size(); ++i)
				csv << refine_log[i] << ", ";
/*			
			for (size_t i = 0; i < workload.size(); ++i)
				csv << workload[i] << ", ";
/*
			for (size_t i = 0; i < workload.size(); ++i)
				std::cout << i << ": " << workload[i] << std::endl;
*/				/*
			for (size_t i = 0; i < workload_elements.size(); ++i)
				csv << workload_elements[i] << ", ";//*/
/*
			csv << std::endl;
			csv.close();//*/
/*
			for (size_t i =0; i < refine_times.size(); ++i)
				csv << refine_times[i] << ", ";

			for (size_t i = 0; i < triangulate_log.size(); ++i)
			{
				csv << triangulate_log[i] << ", ";
			}
			*/
			for (size_t i =0; i < threads_log.size(); ++i)
				csv << threads_log[i] << ", ";//*/

			for (size_t i = 0; i < smooth_log.size(); ++i)
				csv << smooth_log[i] << ", ";

			for (size_t i = 0; i < swap_log.size(); ++i)
				csv << swap_log[i] << ", ";
/*
			for (size_t i = 0; i < int_check_log.size(); ++i)
				csv << int_check_log[i] << ","; */
/*
			for (size_t i = 0; i < build_tri_ds.size(); ++i)
				csv << build_tri_ds[i] << ",";
*/
							
			for (size_t i = 0; i < workload_elements.size(); ++i)
				csv << workload_elements[i] << ", ";//*/

			csv << std::endl;
			csv.close();//*/
	
			//InputMesh.WritePartitions();
			//InputMesh.WriteMergedMesh("output.vtu");

			//set_output("mesh", input_mesh());

			ofstream final_num_eles;
			final_num_eles.open("final_eles_per_partition.txt");
			std::vector<int> ele_data;
			//DEBUG
			for (size_t i = 0; i < InputMesh.pragmatic_partitions.size(); ++i)
			{
				final_num_eles << InputMesh.pragmatic_partitions[i]->get_number_elements() << std::endl;
				ele_data.push_back(InputMesh.pragmatic_partitions[i]->get_number_elements());
			}
			final_num_eles.close();
			std::cout << "num_elements max " << *std::max_element(ele_data.begin(), ele_data.end()) << " min " << *std::min_element(ele_data.begin(), ele_data.end());
			std::cout << " average " << std::accumulate(ele_data.begin(), ele_data.end(), 0) / InputMesh.pragmatic_partitions.size() << std::endl;
			//END OF DEBUG

			//Convert to ViennaGrid
			//auto convert_tic = std::chrono::system_clock::now();

			//ViennaGrid typedefs
			typedef viennagrid::mesh                                                        MeshType;
			typedef viennagrid::result_of::element<MeshType>::type                          VertexType;
			typedef viennagrid::result_of::element<MeshType>::type							TetrahedronType;

			mesh_handle output_mesh = make_data<mesh_handle>();
			int vertices = 0;
			int elements = 0;

			auto dimension = in_mesh->get_number_dimensions();

			//output_mesh.resize(num_partitions());

			//TODO: REPLACE WITH TEMPLATED VERSION!!!
			//convert pragmatic to viennagrid output
			if (algo=="pragmatic"/* || algo == "tetgen"*/ || algo == "pragmaticcavity")
			{
				viennamesh::info(1) << "Converting Pragmatic to ViennaGrid data structure" << std::endl;
				viennamesh::info(5) << "  REPLACE THIS WITH IMPLICIT CONVERSION!!!" << std::endl;

				if (dimension == 2)
				{	
					//output mesh in a single file
					if ( single_mesh_output.valid() && single_mesh_output() )
					{
						for (size_t i = 0; i != InputMesh.pragmatic_partitions.size(); ++i)
						{
							//get basic mesh information
							size_t NNodes = InputMesh.pragmatic_partitions[i]->get_number_nodes();
							size_t NElements = InputMesh.pragmatic_partitions[i]->get_number_elements(); 

							//create empty vector of size NNodes containing viennagrid vertices
							std::vector<VertexType> vertex_handles(NNodes);

							//std::vector<int> vertex_mapping(NNodes, -1);
							
							//iterating all pragmatic vertices and store their coordinates in the viennagrid vertices
							for(size_t j = 0; j < NNodes; ++j)
							{
								//if vertex is already in the mesh, than put the local index into the vertex_mapping vector use this index for the correct element creation
								//if ()

								vertex_handles[j] = viennagrid::make_vertex( output_mesh(), viennagrid::make_point(InputMesh.pragmatic_partitions[i]->get_coords(j)[0], 
																			InputMesh.pragmatic_partitions[i]->get_coords(j)[1]) );
							} //end of for loop iterating all pragmatic vertices 

							//iterating all pragmatic elements and createg their corresponding viennagrid triangles
							for (size_t j = 0; j < NElements; ++j)
							{
								index_t const* ENList_ptr = InputMesh.pragmatic_partitions[i]->get_element(j); 
								
								if (ENList_ptr[0] < 0)
								{
									--elements;
									continue;
								}

								viennagrid::make_triangle( output_mesh(), vertex_handles[ENList_ptr[0]], vertex_handles[ENList_ptr[1]], vertex_handles[ENList_ptr[2]] );
								//viennagrid::make_triangle( output_mesh(), vertex_handles[*(ENList_ptr++)], vertex_handles[*(ENList_ptr++)], vertex_handles[*(ENList_ptr++)] );
							} //end of iterating all pragmatic elements

							//Update total number of vertices and elements
							vertices += InputMesh.pragmatic_partitions[i]->get_number_nodes();
							elements += InputMesh.pragmatic_partitions[i]->get_number_elements();

							//delete InputMesh.pragmatic_partitions[i];
						}
					} //end of single mesh output
					
					//output each mesh partition in a single file
					else
					{
						output_mesh.resize(num_partitions());

						for (size_t i = 0; i != InputMesh.pragmatic_partitions.size(); ++i)
						{
							//get basic mesh information
							size_t NNodes = InputMesh.pragmatic_partitions[i]->get_number_nodes();
							size_t NElements = InputMesh.pragmatic_partitions[i]->get_number_elements(); 

							//create empty vector of size NNodes containing viennagrid vertices
							std::vector<VertexType> vertex_handles(NNodes);
							
							//iterating all pragmatic vertices and store their coordinates in the viennagrid vertices
							for(size_t j = 0; j < NNodes; ++j)
							{
								vertex_handles[j] = viennagrid::make_vertex( output_mesh(i), viennagrid::make_point(InputMesh.pragmatic_partitions[i]->get_coords(j)[0], 
																			InputMesh.pragmatic_partitions[i]->get_coords(j)[1]) );
							} //end of for loop iterating all pragmatic vertices 

							//iterating all pragmatic elements and createg their corresponding viennagrid triangles
							for (size_t j = 0; j < NElements; ++j)
							{
								index_t const* ENList_ptr = InputMesh.pragmatic_partitions[i]->get_element(j); 

								if (ENList_ptr[0] < 0)
								{
									--elements;
									continue;
								}

								viennagrid::make_triangle( output_mesh(i), vertex_handles[ENList_ptr[0]], vertex_handles[ENList_ptr[1]], vertex_handles[ENList_ptr[2]] );
								//viennagrid::make_triangle( output_mesh(i), vertex_handles[*(ENList_ptr++)], vertex_handles[*(ENList_ptr++)], vertex_handles[*(ENList_ptr++)] );
							} //end of iterating all pragmatic elements

							//Update total number of vertices and elements
							vertices += InputMesh.pragmatic_partitions[i]->get_number_nodes();
							elements += InputMesh.pragmatic_partitions[i]->get_number_elements();
			
						}
					} //end of else ( multi mesh output )
				} //end of if(dim == 2)

				//dim == 3
				else
				{
					//output mesh in a single file
					if ( single_mesh_output.valid() && single_mesh_output() )
					{
						for (size_t i = 0; i != InputMesh.pragmatic_partitions.size(); ++i)
						{
							//get basic mesh information
							size_t NNodes = InputMesh.pragmatic_partitions[i]->get_number_nodes();
							size_t NElements = InputMesh.pragmatic_partitions[i]->get_number_elements(); 

							//create empty vector of size NNodes containing viennagrid vertices
							std::vector<TetrahedronType> tet_handles(NNodes);
							
							//iterating all pragmatic vertices and store their coordinates in the viennagrid vertices
							for(size_t j = 0; j < NNodes; ++j)
							{
								tet_handles[j] = viennagrid::make_vertex( output_mesh(), viennagrid::make_point(InputMesh.pragmatic_partitions[i]->get_coords(j)[0], 
																			InputMesh.pragmatic_partitions[i]->get_coords(j)[1], InputMesh.pragmatic_partitions[i]->get_coords(j)[2]) );
							} //end of for loop iterating all pragmatic vertices 

							//iterating all pragmatic elements and create their corresponding viennagrid triangles
							for (size_t j = 0; j < NElements; ++j)
							{
								index_t const* ENList_ptr = InputMesh.pragmatic_partitions[i]->get_element(j);

								if (ENList_ptr[0] < 0)
								{
									--elements;
									continue;
								}
								
								viennagrid::make_tetrahedron( output_mesh(), tet_handles[ENList_ptr[0]], tet_handles[ENList_ptr[1]], tet_handles[ENList_ptr[2]], tet_handles[ENList_ptr[3]] );
								//viennagrid::make_tetrahedron( output_mesh(), tet_handles[*(ENList_ptr++)], tet_handles[*(ENList_ptr++)], tet_handles[*(ENList_ptr++)], tet_handles[*(ENList_ptr++)] );
							} //end of iterating all pragmatic elements

							//Update total number of vertices and elements
							vertices += InputMesh.pragmatic_partitions[i]->get_number_nodes();
							elements += InputMesh.pragmatic_partitions[i]->get_number_elements();							
						}
					} //end of if (single mesh output)

					//output each mesh partition into a single file (multi mesh output)
					else
					{
						output_mesh.resize(num_partitions());
						
						for (size_t i = 0; i != InputMesh.pragmatic_partitions.size(); ++i)
						{
							//get basic mesh information
							size_t NNodes = InputMesh.pragmatic_partitions[i]->get_number_nodes();
							size_t NElements = InputMesh.pragmatic_partitions[i]->get_number_elements(); 

							//create empty vector of size NNodes containing viennagrid vertices
							std::vector<TetrahedronType> tet_handles(NNodes);

							//iterating all pragmatic vertices and store their coordinates in the viennagrid vertices
							for(size_t j = 0; j < NNodes; ++j)
							{
								tet_handles[j] = viennagrid::make_vertex( output_mesh(i), viennagrid::make_point(InputMesh.pragmatic_partitions[i]->get_coords(j)[0], 
																			InputMesh.pragmatic_partitions[i]->get_coords(j)[1], InputMesh.pragmatic_partitions[i]->get_coords(j)[2]) );
							} //end of for loop iterating all pragmatic vertices 

							//iterating all pragmatic elements and createg their corresponding viennagrid triangles
							for (size_t j = 0; j < NElements; ++j)
							{
								index_t const* ENList_ptr = InputMesh.pragmatic_partitions[i]->get_element(j); 
								if (ENList_ptr[0] < 0)
								{
									--elements;
									continue;
								}
								
								viennagrid::make_tetrahedron( output_mesh(i), tet_handles[ENList_ptr[0]], tet_handles[ENList_ptr[1]], tet_handles[ENList_ptr[2]], tet_handles[ENList_ptr[3]] );
								//viennagrid::make_tetrahedron( output_mesh(i), tet_handles[*(ENList_ptr++)], tet_handles[*(ENList_ptr++)], tet_handles[*(ENList_ptr++)], tet_handles[*(ENList_ptr++)] );
							} //end of iterating all pragmatic elements

							//Update total number of vertices and elements
							vertices += InputMesh.pragmatic_partitions[i]->get_number_nodes();
							elements += InputMesh.pragmatic_partitions[i]->get_number_elements();
						}
					} //end of multi mesh output
				} //end of else (dim == 3)*/
			} //end of convert pragmatic to viennagrid output*/

			//convert triangle to viennagrid output
			else if (algo == "triangle")
			{
				viennamesh::info(1) << "Converting Triangle to ViennaGrid data structure" << std::endl;

				//output mesh in a single file
				if ( single_mesh_output.valid() && single_mesh_output() )
				{
					for (size_t i = 0; i != InputMesh.triangle_partitions.size(); ++i)
					{
						//get basic mesh information
						size_t NNodes = InputMesh.triangle_partitions[i].numberofpoints;
						size_t NElements = InputMesh.triangle_partitions[i].numberoftriangles;

						//create empty vector of size NNodes containing viennagrid vertices
						std::vector<VertexType> vertex_handles(NNodes);

						//std::cerr << " vertices " << NNodes << std::endl;

						//iterating all triangle vertices and store their coordinates in the viennagrid vertices
						for(size_t j = 0; j < NNodes; ++j)
						{
							//std::cout << "  " << j+1 << "/" << NNodes << std::endl;
							vertex_handles[j] = viennagrid::make_vertex( output_mesh(), viennagrid::make_point(InputMesh.triangle_partitions[i].pointlist[2*j], 
																		InputMesh.triangle_partitions[i].pointlist[2*j+1]));

							//std::cout << j << " " << InputMesh.triangle_partitions[i].pointmarkerlist[j] << std::endl;
						} //end of for loop iterating all pragmatic vertices 
	/*
						for (size_t e = 0; e < InputMesh.triangle_partitions[i].numberofedges; ++e)
						{
							if(InputMesh.triangle_partitions[i].edgemarkerlist[e])
							{
								std::cout << "edge " << e+1 << std::endl;
								std::cout << e << " " << InputMesh.triangle_partitions[i].edgemarkerlist[e] << std::endl;
								//std::cout << "  " << InputMesh.triangle_partitions[i].
							}
						}
	*/
						//std::cerr << " triangles " << NElements << std::endl;

						//iterating all triangle elements and createg their corresponding viennagrid triangles
						for (size_t j = 0; j < NElements; ++j)
						{
							//std::cout << "  " << j+1 << "/" << NElements << std::endl;
							viennagrid::make_triangle( output_mesh(), 
													vertex_handles[InputMesh.triangle_partitions[i].trianglelist[3*j]], 
													vertex_handles[InputMesh.triangle_partitions[i].trianglelist[3*j+1]], 
													vertex_handles[InputMesh.triangle_partitions[i].trianglelist[3*j+2]] );
						} //end of iterating all triangle elements

						//Update total number of vertices and elements
						vertices += InputMesh.triangle_partitions[i].numberofpoints;
						elements += InputMesh.triangle_partitions[i].numberoftriangles;
					}
				} //end of output mesh in a single file

				//multi mesh output
				else
				{
					output_mesh.resize(num_partitions());
				
					for (size_t i = 0; i != InputMesh.triangle_partitions.size(); ++i)
					{
						//get basic mesh information
						size_t NNodes = InputMesh.triangle_partitions[i].numberofpoints;
						size_t NElements = InputMesh.triangle_partitions[i].numberoftriangles;

						//create empty vector of size NNodes containing viennagrid vertices
						std::vector<VertexType> vertex_handles(NNodes);

						//std::cerr << " vertices " << NNodes << std::endl;

						//iterating all triangle vertices and store their coordinates in the viennagrid vertices
						for(size_t j = 0; j < NNodes; ++j)
						{
							//std::cout << "  " << j+1 << "/" << NNodes << std::endl;
							vertex_handles[j] = viennagrid::make_vertex( output_mesh(i), viennagrid::make_point(InputMesh.triangle_partitions[i].pointlist[2*j], 
																		InputMesh.triangle_partitions[i].pointlist[2*j+1]));

							//std::cout << j << " " << InputMesh.triangle_partitions[i].pointmarkerlist[j] << std::endl;
						} //end of for loop iterating all pragmatic vertices 
	/*
						for (size_t e = 0; e < InputMesh.triangle_partitions[i].numberofedges; ++e)
						{
							if(InputMesh.triangle_partitions[i].edgemarkerlist[e])
							{
								std::cout << "edge " << e+1 << std::endl;
								std::cout << e << " " << InputMesh.triangle_partitions[i].edgemarkerlist[e] << std::endl;
								//std::cout << "  " << InputMesh.triangle_partitions[i].
							}
						}
	*/
						//std::cerr << " triangles " << NElements << std::endl;

						//iterating all triangle elements and createg their corresponding viennagrid triangles
						for (size_t j = 0; j < NElements; ++j)
						{
							//std::cout << "  " << j+1 << "/" << NElements << std::endl;
							viennagrid::make_triangle( output_mesh(i), 
													vertex_handles[InputMesh.triangle_partitions[i].trianglelist[3*j]], 
													vertex_handles[InputMesh.triangle_partitions[i].trianglelist[3*j+1]], 
													vertex_handles[InputMesh.triangle_partitions[i].trianglelist[3*j+2]] );
						} //end of iterating all triangle elements

						//Update total number of vertices and elements
						vertices += InputMesh.triangle_partitions[i].numberofpoints;
						elements += InputMesh.triangle_partitions[i].numberoftriangles;
					} 
				}	//end of multi mesh output	

				//free used memory
				for (size_t i = 0; i < output_mesh.size(); ++i)
				{
					free(InputMesh.triangle_partitions[i].pointlist);				//if pointer in triangulateio creation is used, this list is free
																				   	//pragmaticwrapper's destructor
					free(InputMesh.triangle_partitions[i].pointattributelist);
					free(InputMesh.triangle_partitions[i].pointmarkerlist);
					//free(InputMesh.triangle_partitions[i].numberofpoints);
					//free(InputMesh.triangle_partitions[i].numberofpointattributes);

					free(InputMesh.triangle_partitions[i].trianglelist);         	//if pointer in triangulateio creation is used, this list is free
																				   	//pragmaticwrapper's destructor
					//free(InputMesh.triangle_partitions[i].triangleattributelist);
					//free(InputMesh.triangle_partitions[i].trianglearealist);
					//free(InputMesh.triangle_partitions[i].neighborlist);
					//free(InputMesh.triangle_partitions[i].numberoftriangles);
					//free(InputMesh.triangle_partitions[i].numberofcorners);
					//free(InputMesh.triangle_partitions[i].numberoftriangleattributes);

				/*	free(InputMesh.triangle_partitions[i].segmentlist);
					free(InputMesh.triangle_partitions[i].segmentmarkerlist);
					//free(InputMesh.triangle_partitions[i].numberofsegments);

					free(InputMesh.triangle_partitions[i].holelist);
					//free(InputMesh.triangle_partitions[i].numberofholes);

					free(InputMesh.triangle_partitions[i].regionlist);
					//free(InputMesh.triangle_partitions[i].numberofregions);

					free(InputMesh.triangle_partitions[i].edgelist);
					free(InputMesh.triangle_partitions[i].edgemarkerlist);
					free(InputMesh.triangle_partitions[i].normlist);
					//free(InputMesh.triangle_partitions[i].numberofedges);
*/
					//delete InputMesh.pragmatic_partitions[i];
				}
				//end of free used memory*/
			} //end of convert triangle to viennagrid output

			//Tetgen to ViennaMesh
			else
			{
				viennamesh::info(1) << "Converting Tetgen to ViennaGrid data structure" << std::endl;
				
				//output mesh into a single file
				if ( single_mesh_output.valid() && single_mesh_output() )
				{
					for (size_t i = 0; i != InputMesh.tetgen_partitions.size(); ++i)
					{
						//std::cout << "  Converting mesh number " << i << std::endl;
						//get basic mesh information
						size_t NNodes = InputMesh.tetgen_partitions[i].numberofpoints;
						size_t NElements = InputMesh.tetgen_partitions[i].numberoftetrahedra;
/*
						std::cout << "    Vertices: " << NNodes << std::endl;
						std::cout << "    Elements: " << NElements << std::endl;
*/
						//create empty vector of size NNodes containing viennagrid vertices
						std::vector<VertexType> vertex_handles(NNodes);

						//iterating all pragmatic vertices and store their coordinates in the viennagrid vertices
						for(size_t j = 0; j < NNodes; ++j)
						{
							vertex_handles[j] = viennagrid::make_vertex(output_mesh(), 
																		viennagrid::make_point(InputMesh.tetgen_partitions[i].pointlist[3*j],
																						       InputMesh.tetgen_partitions[i].pointlist[3*j+1],
																						       InputMesh.tetgen_partitions[i].pointlist[3*j+2]));										
						} //end of for loop iterating all pragmatic vertices 

						//iterating all pragmatic elements and create their corresponding viennagrid triangles
						for (size_t j = 0; j < NElements; ++j)
						{ 
							viennagrid::make_tetrahedron( output_mesh(), vertex_handles[ InputMesh.tetgen_partitions[i].tetrahedronlist[4*j] ],
																		 vertex_handles[ InputMesh.tetgen_partitions[i].tetrahedronlist[4*j+1] ], 
																		 vertex_handles[ InputMesh.tetgen_partitions[i].tetrahedronlist[4*j+2] ], 
																		 vertex_handles[ InputMesh.tetgen_partitions[i].tetrahedronlist[4*j+3] ] );
						} //end of iterating all pragmatic elements

						//Update total number of vertices and elements
						vertices += InputMesh.tetgen_partitions[i].numberofpoints;
						elements += InputMesh.tetgen_partitions[i].numberoftetrahedra;

						//InputMesh.tetgen_partitions[i].deinitialize();
					}	
				}//end of output mesh into a single file*/

				//multi mesh output
				else
				{
					output_mesh.resize(num_partitions());

					for (size_t i = 0; i != InputMesh.tetgen_partitions.size(); ++i)
					{
						//get basic mesh information
						size_t NNodes = InputMesh.tetgen_partitions[i].numberofpoints;
						size_t NElements = InputMesh.tetgen_partitions[i].numberoftetrahedra;

						//create empty vector of size NNodes containing viennagrid vertices
						std::vector<VertexType> vertex_handles(NNodes);

						//iterating all pragmatic vertices and store their coordinates in the viennagrid vertices
						for(size_t j = 0; j < NNodes; ++j)
						{
							vertex_handles[j] = viennagrid::make_vertex(output_mesh(i), 
																		viennagrid::make_point(InputMesh.tetgen_partitions[i].pointlist[3*j],
																						   	   InputMesh.tetgen_partitions[i].pointlist[3*j+1],
																						       InputMesh.tetgen_partitions[i].pointlist[3*j+2]));										
						} //end of for loop iterating all pragmatic vertices 

						//iterating all pragmatic elements and create their corresponding viennagrid triangles
						for (size_t j = 0; j < NElements; ++j)
						{ 
							viennagrid::make_tetrahedron( output_mesh(i), vertex_handles[ InputMesh.tetgen_partitions[i].tetrahedronlist[4*j] ],
																		  vertex_handles[ InputMesh.tetgen_partitions[i].tetrahedronlist[4*j+1] ], 
																		  vertex_handles[ InputMesh.tetgen_partitions[i].tetrahedronlist[4*j+2] ], 
																		  vertex_handles[ InputMesh.tetgen_partitions[i].tetrahedronlist[4*j+3] ] );
						} //end of iterating all pragmatic elements

						//Update total number of vertices and elements
						vertices += InputMesh.tetgen_partitions[i].numberofpoints;
						elements += InputMesh.tetgen_partitions[i].numberoftetrahedra;

						//InputMesh.tetgen_partitions[i].deinitialize();
					}	
				} //end of multi mesh output	
			} //end of Tetgen

			//std::chrono::duration<double> convert_dur = std::chrono::system_clock::now() - convert_tic;

		/*	csv << convert_dur.count();
			csv << std::endl;
			csv.close();*/
			
			viennamesh::info(5) << "  Number of Vertices after conversion: " << vertices << std::endl;
			viennamesh::info(5) << "  Number of Elements after conversion: " << elements << std::endl;
			set_output("mesh", output_mesh);
			set_output("colors", InputMesh.get_colors());

			//delete in_mesh;

			return true;
		} //end run()		
}
