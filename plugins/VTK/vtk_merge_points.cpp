#include "vtk_mesh.hpp"
#include "vtk_merge_points.hpp"

namespace viennamesh {

    namespace VTK_PolyData 
    {

        merge_points::merge_points() {}

        bool merge_points::run(viennamesh::algorithm_handle &) 
        {
            info(5) << "Running vtk_merge_points." << std::endl;
            std::cout << "NOT YET IMPLEMENTED" << std::endl;

            // Get required input parameters
            data_handle<VTK_PolyData::mesh> input_mesh = get_required_input<VTK_PolyData::mesh>("mesh");

            int geometric_dimension;
            int cell_dimension;

            return VIENNAMESH_SUCCESS;
        }
    }
}