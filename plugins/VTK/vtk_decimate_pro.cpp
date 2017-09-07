#include "vtk_decimate_pro.hpp"
#include "vtk_mesh.hpp"

#include <vtkDecimatePro.h>
#include <vtkPLYReader.h>
#include <vtkPLYWriter.h>

namespace viennamesh {

    namespace vtk {

        decimate_pro::decimate_pro() {}

        bool decimate_pro::run(viennamesh::algorithm_handle &) {
            info(5) << "Running vtk_decimate_pro." << std::endl;

            // Get required input parameters
            data_handle<vtk::mesh> input_mesh = get_required_input<vtk::mesh>("mesh");
            data_handle<double> reductionPercentage = get_required_input<double>("reduction_percentage");

            data_handle<vtk::mesh> output_mesh = make_data<vtk::mesh>();

            // Get optional input parameters
            data_handle<int> preserveTopology = get_input<int>("preserve_topology");
            data_handle<double> featureAngle = get_input<double>("feature_angle");
            data_handle<int> splitting = get_input<int>("splitting");
            data_handle<double> splitAngle = get_input<double>("split_angle");
            data_handle<int> preSplitMesh = get_input<int>("pre_spilt_mesh");
            data_handle<double> maximumError = get_input<double>("maximum_error");
            data_handle<int> accumulateError = get_input<int>("accumulate_error");
            data_handle<int> errorIsAbsolute = get_input<int>("error_is_absolute");
            data_handle<double> absoluteError = get_input<double>("absulute_error");
            data_handle<int> boundaryVertexDeletion = get_input<int>("boudary_vertex_deletion");
            data_handle<int> degree = get_input<int>("degree");
            data_handle<double> inflectionPointRatio = get_input<double>("inflection_point_ratio");

            vtkDecimatePro *decimatePro = vtkDecimatePro::New();

            // Set required parameters
            debug(5) << "Set required parameters." << std::endl;
            decimatePro->SetInputData(input_mesh().GetMesh());
            decimatePro->SetTargetReduction(reductionPercentage());

            // Set optional parameters
            debug(5) << "Set optional parameters." << std::endl;
            if (preserveTopology.valid())
            {
                decimatePro->SetPreserveTopology(preserveTopology());
            }

            if (featureAngle.valid())
            {
                decimatePro->SetFeatureAngle(featureAngle());
            }

            if (splitting.valid())
            {
                decimatePro->SetSplitting(splitting());
            }

            if (splitAngle.valid())
            {
                decimatePro->SetSplitAngle(splitAngle());
            }

            if (preSplitMesh.valid())
            {
                decimatePro->SetPreSplitMesh(preSplitMesh());
            }

            if (maximumError.valid())
            {
                decimatePro->SetMaximumError(maximumError());
            }

            if (accumulateError.valid())
            {
                decimatePro->SetAccumulateError(accumulateError());
            }

            if (errorIsAbsolute.valid())
            {
                decimatePro->SetErrorIsAbsolute(errorIsAbsolute());
            }

            if (absoluteError.valid())
            {
                decimatePro->SetAbsoluteError(absoluteError());
            }

            if (boundaryVertexDeletion.valid())
            {
                decimatePro->SetBoundaryVertexDeletion(boundaryVertexDeletion());
            }

            if (degree.valid())
            {
                decimatePro->SetDegree(degree());
            }

            if (inflectionPointRatio.valid())
            {
                decimatePro->SetInflectionPointRatio(inflectionPointRatio());
            }

            // Run algorithm
            debug(5) << "Run algorithm." << std::endl;
            decimatePro->Update();

            debug(5) << "Copy algorithm output." << std::endl;
            output_mesh().GetMesh()->DeepCopy(decimatePro->GetOutput());
            set_output("mesh", output_mesh());

            decimatePro->Delete();
            return true;
        }
    }
}
