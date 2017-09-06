#ifndef PRAGMATIC_WRAPPER_H
#define PRAGMATIC_WRAPPER_H

#include <memory>

class PragmaticWrapper
{
    public:

        PragmaticWrapper() 
        {
            //std::cout << "Constructor" << std::endl;
            mesh = nullptr;
        }
        ~PragmaticWrapper() 
        {
            //std::cout << "Destructor" << std::endl;
            if (mesh != nullptr)
            {
                //std::cout << "deleting mesh at " << mesh << std::endl;
                delete mesh;
                mesh = nullptr;
            }
        }


        void CreateMesh(viennagrid::mesh const & input) 
        {            
            //std::cout << "Create Mesh"<< std::endl;

            //get basic information about mesh
            size_t cell_dimension = viennagrid::cell_dimension(input);
            size_t geometric_dimension = viennagrid::geometric_dimension(input);
            size_t NNodes = viennagrid::vertex_count(input);
            size_t NElements = viennagrid::cells(input).size();

            //set up vectors for ENList and x-, y- and z-coordinates
            std::vector<index_t> ENList;
            std::vector<double> x,y,z;

            //
            //create pointer to iterate over viennagrid_array
            viennagrid_numeric* ptr_coords = nullptr;

            //get pointer to coordinates array from the mesh
            viennagrid_mesh_vertex_coords_pointer(input.internal(), &ptr_coords);

            //iterate over all nodes in the mesh and store the coordinates in the vectors needed by pragmatic
            for (size_t i = 0; i < NNodes; ++i)
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

            //Iterate over all triangles in the mesh
            viennagrid_element_id * vertex_ids_begin;
            viennagrid_element_id * vertex_ids_end;
            viennagrid_dimension topological_dimension = geometric_dimension;		//produces segmentation fault if not set to 2 for 2d and to 3 for 3d case
                                                                                    //TODO:THE SOLUTION IN THE ROW ABOVE IS OPTIMIZABLE!!!
            //get elements from mesh
            viennagrid_mesh_elements_get(input.internal(), topological_dimension, &vertex_ids_begin, &vertex_ids_end);
            
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
                    viennagrid_element_boundary_elements(input.internal(), triangle_id, boundary_topological_dimension, &boundary_vertex_ids_begin, &boundary_vertex_ids_end);
                            
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
                /*mesh = std::make_unique< Mesh<double> >(NNodes, NElements, &(ENList[0]), &(x[0]), &(y[0]));*/
                mesh->create_boundary(); 
            }
        
            else
            {		
                mesh = new Mesh<double> (NNodes, NElements, &(ENList[0]), &(x[0]), &(y[0]), &(z[0]));
                mesh->create_boundary();
            }
            //*/

            /*std::cout << "Created Mesh at " << mesh << std::endl;//*/
            
        } //end of CreateMesh(viennagrid::mesh const & input)

    //Members    
    Mesh<double>* mesh;
    //std::unique_ptr<Mesh<double>> mesh;
};
#endif