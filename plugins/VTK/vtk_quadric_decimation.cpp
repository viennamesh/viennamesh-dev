#include "vtk_quadric_decimation.hpp"
#include "vtk_mesh.hpp"

#include <vtkQuadricDecimation.h>

namespace viennamesh {

    namespace vtk {

        quadric_decimation::quadric_decimation() {}

        bool quadric_decimation::run(viennamesh::algorithm_handle &) {
            info(5) << "Running vtk_quadric_decimation." << std::endl;

            // Get required input parameters
            data_handle<vtk::mesh> input_mesh = get_required_input<vtk::mesh>("mesh");
            data_handle<double> reductionPercentage = get_required_input<double>("reduction_percentage");

            data_handle<vtk::mesh> output_mesh = make_data<vtk::mesh>();

            vtkQuadricDecimation * quadricDecimation = vtkQuadricDecimation::New();

            // Set required parameters
            quadricDecimation->SetInputData(input_mesh().GetMesh());
            quadricDecimation->SetTargetReduction(reductionPercentage());

            // Run algorithm
            info(5) << "Run quadric decimation." << std::endl;
            quadricDecimation->Update();

            info(5) << "Copy output mesh." << std::endl;
            output_mesh().GetMesh()->DeepCopy((vtkDataObject*)(quadricDecimation->GetOutput()));

            set_output("mesh", output_mesh());

            quadricDecimation->Delete();
            return true;
        }
    }
}