
#include "viennagrid/viennagrid.hpp"
#include "pragmatic_mesh.hpp"

namespace viennamesh
{
    viennamesh_error convert(viennagrid::mesh const & input, pragmatic_wrapper::mesh & output)
    {
        //std::cout << std::endl << "viennagrid to pragmatic" << std::endl;
/*
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
        }*/
/*
        if (geometric_dimension == 2)	
        {
            pragmatic::pragmatic_mesh mesh = new Mesh<double> (NNodes, NElements, &(ENList[0]), &(x[0]), &(y[0]));
            mesh->create_boundary(); 
            output = mesh;
        }
    /*
        else
        {		
            output = new Mesh<double> (NNodes, NElements, &(ENList[0]), &(x[0]), &(y[0]), &(z[0]));
            output->create_boundary();
        }
*/
        output.CreateMesh(input);

        return VIENNAMESH_SUCCESS;
    }

    viennamesh_error convert(pragmatic_wrapper::mesh const & input, viennagrid::mesh & output)
    {
        //std::cout << std::endl << "pragmatic to viennagrid" << std::endl;
/*
        //ViennaGrid typedefs
        typedef viennagrid::mesh                                                        MeshType;

        typedef viennagrid::result_of::element<MeshType>::type                          VertexType;

        //get basic mesh information
        size_t NNodes = input->get_number_nodes();
        size_t NElements = input->get_number_elements();

        //create empty vector of size NNodes containing viennagrid vertices
        std::vector<VertexType> vertex_handles(NNodes);

        //iterating all pragmatic vertices and store their coordinates in the viennagrid vertices
        for(size_t i = 0; i < NNodes; ++i)
        {
            vertex_handles[i] = viennagrid::make_vertex( output, viennagrid::make_point(input->get_coords(i)[0], input->get_coords(i)[1]) );
        } //end of for loop iterating all pragmatic vertices 

        //iterating all pragmatic elements and createg their corresponding viennagrid triangles
        for (size_t i = 0; i < NElements; ++i)
        {
            index_t const* ENList_ptr = input->get_element(i); 
            if (input->get_number_dimensions() == 2)
                viennagrid::make_triangle( output, vertex_handles[*(ENList_ptr++)], vertex_handles[*(ENList_ptr++)], vertex_handles[*(ENList_ptr++)] );

            else
            {
                std::cout << "3DIM" << std::endl;
                return 0;
            }
        } //end of iterating all pragmatic elements

        //delete input;*/

        return VIENNAMESH_SUCCESS;
    }

    template<>
    viennamesh_error internal_convert<viennagrid_mesh, pragmatic_wrapper::mesh>(viennagrid_mesh const & input, pragmatic_wrapper::mesh & output)
    {
        //std::cerr << std::endl << "internal convert viennagrid to pragmatic" << std::endl;
        return convert(viennagrid::mesh(input), output);
    }

    template<>
    viennamesh_error internal_convert<pragmatic_wrapper::mesh, viennagrid_mesh>(pragmatic_wrapper::mesh const & input, viennagrid_mesh & output)
    {
        //std::cerr << std::endl << "internal convert pragmatic to viennagrid" << std::endl;
        viennagrid::mesh output_pp(output);
        return convert(input, output_pp);
    }
}