#include "vtk_quadric_clustering.hpp"
#include "vtk_mesh.hpp"

#include <vtkQuadricClustering.h>

namespace viennamesh {

    namespace vtk {

        quadric_clustering::quadric_clustering() {}

        bool quadric_clustering::run(viennamesh::algorithm_handle &) {
            info(5) << "Running vtk_quadric_clustering." << std::endl;

            // Get required input parameters
            data_handle<vtk::mesh> input_mesh = get_required_input<vtk::mesh>("mesh");
            data_handle<int> xDivision = get_required_input<int>("x_division");
            data_handle<int> yDivision = get_required_input<int>("y_division");
            data_handle<int> zDivision = get_required_input<int>("z_division");

            data_handle<vtk::mesh> output_mesh = make_data<vtk::mesh>();

            // Get optional input parameters
            data_handle<int> useInputPoints = get_input<int>("use_input_points");
            data_handle<int> useFeatureEdges = get_input<int>("use_feature_edges");
            data_handle<int> useFeaturePoints = get_input<int>("use_feature_points");
            data_handle<double> featurePointsAngle = get_input<double>("feature_points_angle");
            data_handle<int> useInternalTriangles = get_input<int>("use_internal_triangles");
            data_handle<int> copyCellData = get_input<int>("copy_cell_data");
            data_handle<int> preventDublicateCells = get_input<int>("prevent_dublicate_cells");

            vtkQuadricClustering* quadricClustering = vtkQuadricClustering::New();

            // Set required parameters
            quadricClustering->SetInputData(input_mesh().GetMesh());
            quadricClustering->SetNumberOfDivisions(xDivision(), yDivision(), zDivision());

            // Set optional parameters
            if (useInputPoints.valid())
            {
                quadricClustering->SetUseInputPoints(useInputPoints());
            }

            if (useFeatureEdges.valid())
            {
                quadricClustering->SetUseFeatureEdges(useFeatureEdges());
            }

            if (useFeaturePoints.valid())
            {
                quadricClustering->SetUseFeaturePoints(useFeaturePoints());
            }

            if (featurePointsAngle.valid())
            {
                quadricClustering->SetFeaturePointsAngle(featurePointsAngle());
            }

            if (useInternalTriangles.valid())
            {
                quadricClustering->SetUseInternalTriangles(useInternalTriangles());
            }

            if (copyCellData.valid())
            {
                quadricClustering->SetCopyCellData(copyCellData());
            }

            if (preventDublicateCells.valid())
            {
                quadricClustering->SetPreventDuplicateCells(preventDublicateCells());
            }

            // Run algorithm
            quadricClustering->Update();

            output_mesh().GetMesh()->DeepCopy((vtkDataObject*)quadricClustering->GetOutput());

            info(5) << "Set output mesh." << std::endl;
            set_output("mesh", output_mesh());

            quadricClustering->Delete();
            return true;
        }
    }
}
