#include "viennagrid/viennagrid.hpp"
#include "consistency_check.hpp"

namespace viennamesh
{
    consistency_check::consistency_check() {}

    std::string consistency_check::name() { return "consistency_check"; }

    bool consistency_check::run(viennamesh::algorithm_handle &)
    {
        //
        // Define the mesh and segmentation types
        //
        typedef viennagrid::mesh                                             MeshType;

        //
        // Define the types of the elements in the mesh (derived from MeshType):
        //
        typedef viennagrid::result_of::element<MeshType>::type               VertexType;
        typedef viennagrid::result_of::element<MeshType>::type               FacetType;
        typedef viennagrid::result_of::element<MeshType>::type               CellType;

        info(1) << name() << std::endl;

        mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");

        //get basic information about mesh
        size_t cell_dimension = viennagrid::cell_dimension(input_mesh());
        size_t geometric_dimension = viennagrid::geometric_dimension(input_mesh());

        //2D consistency check
        if (cell_dimension == 2)
        {
            info(5) << "  " << cell_dimension << "D case " << std::endl;
        } //end of 2D consistency check

        //3D consistency check
        else
        {
            info(5) << "  " << cell_dimension << "D case " << std::endl;

            for (size_t mesh_id = 0 ; mesh_id < input_mesh.size(); ++mesh_id)
            {
                info(2) << "    Checking mesh number " << mesh_id << std::endl;

                //get basic info about mesh
                size_t NElements = viennagrid::cells(input_mesh(mesh_id)).size();
                size_t NNodes = viennagrid::vertices(input_mesh(mesh_id)).size();

                info(2) << "      Mesh contains " << NNodes << " vertices" << std::endl;
                info(2) << "      Mesh contains " << NElements << " elements" << std::endl;

                /****************************************************************/
                /*Consistency algorithm from "An algorithm to check the         */
                /*topological consistency of a general 3-D finite element mesh" */
                /* from K. Preiss, 1981.                                        */
                /****************************************************************/
                /*Step (1)                                                      */
                /* Check that each element is bounded.                          */
                /****************************************************************/


                for (size_t eid = 0; eid < NElements; ++eid)
                {
                    ;
                }
            }
/*
            //multi- or single mesh in input_mesh?
            if (input_mesh.size() == 1)
            {
                std::cout << "    Single mesh check" << std::endl;
            }

            else
            {
                std::cout << "    Multi mesh check" << std::endl;
            }*/
        } //end of 3D consistency check

        return VIENNAMESH_SUCCESS;
    }
}