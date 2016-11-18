#ifndef VIENNAMESH_ALGORITHM_PRAGMATIC_MESH_HPP
#define VIENNAMESH_ALGORITHM_PRAGMATIC_MESH_HPP

//pragmatic basic includes for mesh data structure
#include "ticker.h"
#include "VTKTools.h"
#include "Mesh.h"
#include "MetricField.h"
#include "Lock.h"
#include "ElementProperty.h"

//vienangrid includes
#include "viennagrid/io/vtk_writer.hpp"

//viennamesh includes
#include "viennameshpp/core.hpp"

typedef viennagrid::mesh                                          MeshType;

//make metric
inline void make_metric(Mesh<double> *mesh, size_t geometric_dimension)
		{
			if(geometric_dimension == 2)
			{
				MetricField<double,2> metric_field(*mesh);

    		size_t NNodes = mesh->get_number_nodes();
    		double eta=0.0001;

    		std::vector<double> psi(NNodes);

				for(size_t i=0; i<NNodes; i++) 
		  		{
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

//convert vienangrid to pragmatic data structure
inline Mesh<double>* convert(MeshType input_mesh, Mesh<double>* mesh)
		{
			  size_t cell_dimension = viennagrid::cell_dimension( input_mesh );
		  	size_t geometric_dimension = viennagrid::geometric_dimension( input_mesh );
		  	size_t NNodes = viennagrid::vertex_count( input_mesh );
		  	size_t NElements = viennagrid::cells( input_mesh ).size();

			  //set up vectors for ENList and x-, y- and z-coordinates
		  	std::vector<index_t> ENList;
		  	std::vector<double> x, y, z;

			  //Initialize x-, y- and z-coordinates
		  	//
		  	//create pointer to iterate over viennagrid_array
		  	viennagrid_numeric *ptr_coords = nullptr;
						
		  	//get pointer to coordinates array from the mesh
		  	viennagrid_mesh_vertex_coords_pointer(input_mesh.internal(), &ptr_coords);			
			
		  	//iterate over all nodes in the mesh and store the coordinates in the vectors needed by pragmatic
		  	//(coordinates are stored in a big array in the following scheme [x0 y0 z0 x1 y1 z1 ... xn yn zn])
		  	for (size_t i=0; i<NNodes; ++i)
		  	{			  
		   	  x.push_back(*(ptr_coords++));
		    	y.push_back(*(ptr_coords++));
		  
		    	  //depending on the dimension push_back data from the array or push_back 0 into the pragmatic data
		    	  if (geometric_dimension == 3)
		      	    z.push_back(*(ptr_coords++));
		    
		    	  else
		      	    z.push_back(0);
		  	}
		  
		  	//Iterate over all triangles in the mesh
		  	viennagrid_element_id * vertex_ids_begin;
		  	viennagrid_element_id * vertex_ids_end;
		  	viennagrid_dimension topological_dimension = geometric_dimension;		//produces segmentation fault if not set to 2 for 2d and to 3 for 3d case
												//TODO:THE SOLUTION IN THE ROW ABOVE IS OPTIMIZABLE!!!
		  	//get elements from mesh
		  	viennagrid_mesh_elements_get(input_mesh.internal(), topological_dimension, &vertex_ids_begin, &vertex_ids_end);
			
		 	  viennagrid_element_id * boundary_vertex_ids_begin;
		  	viennagrid_element_id * boundary_vertex_ids_end;
		  	viennagrid_dimension boundary_topological_dimension = 0;
		  	viennagrid_element_id triangle_id;
						
		  	int counter = 0;
		  			
		  	//outer for loop iterates over all elements with dimension = topological_dimension (2 for triangles, 3 for tetrahedrons)
		  	//inner for loop iterates over all elements with dimension = boundary_topological_dimension (0 for vertices)
		  	//this info is needed to build the NEList which is used to build the pragmatic data structure
		  	for (viennagrid_element_id * vit = vertex_ids_begin; vit != vertex_ids_end; ++vit)
		  	{
		    		//get vertices of triangle
		    		triangle_id = *vit;
		    		viennagrid_element_boundary_elements(input_mesh.internal(), triangle_id, boundary_topological_dimension, &boundary_vertex_ids_begin, &boundary_vertex_ids_end);
			  			  
		    		for (viennagrid_element_id * bit = boundary_vertex_ids_begin; bit != boundary_vertex_ids_end; ++bit)
		    		{
		      			viennagrid_element_id vertex_id = *bit;
		      			ENList.push_back(vertex_id);
		    		}
			  		  
		    		++counter;
		  	}
		  
		  	if (geometric_dimension == 2)	
		  	{
          mesh = new Mesh<double> (NNodes, NElements, &(ENList[0]), &(x[0]), &(y[0]));
				  return mesh;
		  	}
		
		 	else
		  	{		
				  mesh = new Mesh<double> (NNodes, NElements, &(ENList[0]), &(x[0]), &(y[0]), &(z[0]));
				  return mesh;
		  	}
		}//end of convert(MeshType input_mesh, Mesh<double> *mesh)

//export_to_viennagrid_vtu: converts the pragmatic data structure into viennagrid data structure 
inline bool export_to_viennagrid_vtu(std::vector<Mesh<double>*> meshes)
{
  std::cout << "export_to_viennagrid_vtu" << std::endl;

  int no_of_regions = meshes.size();

  //iterate over all regions  
  for (size_t i = 0; i < no_of_regions; ++i)
  {
    std::string filename;
    filename += "examples/data/export_vtu_to_viennagrid_xml_";
    filename += std::to_string(i);
    filename += ".vtu";
  
    //ofstream object
    ofstream writer;
    writer.open(filename.c_str(), ios::out);

    //write header
    writer << "<?xml version=\"1.0\"?>" << std::endl;
    writer << "<VTKFile type=\"UnstructuredGrid\" version=\"1.0\" byte_order=\"LittleEndian\">" << std::endl;
    writer << " <UnstructuredGrid>" << std::endl;
    //end of write header

    int num_points = meshes[i]->get_number_nodes();
    int num_cells = meshes[i]->get_number_elements();

    writer << "  <Piece NumberOfPoints=\"" << num_points << "\" NumberOfCells=\"" << num_cells << "\">" << std::endl;    

    //write points into file
    writer << "   <Points>" << std::endl;
    writer << "    <DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;  

    //iterate over coordinates vector
    for (size_t j = 0; j < num_points; ++j)
    {
      //TODO: change for 3D case
      //double x[3];
      double x[2];
      meshes[i]->get_coords(j, x);
      writer << x[0] << " " << x[1] << " " << "0" << std::endl;      
      //TODO: change for 3D case
      //writer << x[0] << " " << x[1] << " " << x[2] << std::endl;
    }
    writer << std::endl;
    writer << "    </DataArray>" << std::endl;
    writer << "   </Points> " << std::endl;
    //end of write points into file

    //write cells into file
    writer << "   <Cells> " << std::endl;
    writer << "    <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">" << std::endl;

    for (size_t j = 0; j < num_cells; ++j)
    {    
      //get pointer to element-node-list
      index_t const * ENList_pointer = meshes[i]->get_element(j);

      //TODO: change for 3D case
      //for (size_t k = 0; k < 4; ++k)
      for (size_t k = 0; k < 3; ++k)
      {
        writer << *(ENList_pointer++) << " ";
      }
      writer << std::endl;
    } 

    writer << std::endl;
    writer << "    </DataArray>" << std::endl;
    //end of write cells into file
  
    //write offset into file
    writer << "    <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">" << std::endl;
    
    for (size_t j = 1; j <= num_cells; ++j)
    {
      //TODO: change for 3D case
      //writer << j*4 << " ";
      writer << j*3 << " ";

      if (j%6 == 0)
      {
        writer << std::endl;
      }
    }

    writer << std::endl;
    writer << "    </DataArray>" << std::endl;
    //end of write offset into file

    //write types into file
    writer << "    <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">" << std::endl;
  
    for (size_t j = 0; j < num_cells; ++j) 
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
  } //end of iterate over all regions

  return true;
} //end of export_to_viennagrid_vtu(std::vector<Mesh<double>*> meshes)
#endif
