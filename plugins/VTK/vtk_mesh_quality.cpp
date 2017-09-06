#include "vtk_mesh_quality.hpp"
#include "vtk_mesh.hpp"

#include <vtkMeshQuality.h>
#include <vtkDoubleArray.h>
#include <vtkFieldData.h>
#include <vtkSmartPointer.h>

namespace viennamesh {

    namespace vtk {

        mesh_quality::mesh_quality() {}

        bool mesh_quality::run(viennamesh::algorithm_handle &) {
            info(5) << "Running vtk_mesh_quality." << std::endl;

            // Get required input parameters
            data_handle<vtk::mesh> input_mesh = get_required_input<vtk::mesh>("mesh");

            vtkSmartPointer<vtkMeshQuality> meshQuality = vtkSmartPointer<vtkMeshQuality>::New();
            meshQuality->SetInputData(input_mesh().GetMesh());

            vtkSmartPointer<vtkDoubleArray> doubleArray;

            // Get area qualities
            meshQuality->SetTriangleQualityMeasureToArea();
            meshQuality->Update();

            /*
             * MeshQuality output array always contains 5 fields:
             * minimum, average, maximum, unbiased variance, number of cells of the given type
             */
            doubleArray = vtkDoubleArray::SafeDownCast(meshQuality->GetOutput()->GetFieldData()->GetArray("Mesh Triangle Quality"));
            if (doubleArray->GetSize() < 5)
            {
                cerr << "Not able to fetch triangle qualities!" << endl;
                exit(1);
            }

            set_output("min_area", doubleArray->GetValue(0));
            set_output("av_area", doubleArray->GetValue(1));
            set_output("max_area", doubleArray->GetValue(2));

            // Get angle qualities
            meshQuality->SetTriangleQualityMeasureToMinAngle();
            meshQuality->Update();
            doubleArray = vtkDoubleArray::SafeDownCast(meshQuality->GetOutput()->GetFieldData()->GetArray("Mesh Triangle Quality"));
            if (doubleArray->GetSize() < 5)
            {
                cerr << "Not able to fetch triangle qualities!" << endl;
                exit(1);
            }

            set_output("min_angle", doubleArray->GetValue(0));
            set_output("av_min_angle", doubleArray->GetValue(1));

            // Get aspect ratio qualities
            meshQuality->SetTriangleQualityMeasureToAspectRatio();
            meshQuality->Update();
            doubleArray = vtkDoubleArray::SafeDownCast(meshQuality->GetOutput()->GetFieldData()->GetArray("Mesh Triangle Quality"));
            if (doubleArray->GetSize() < 5)
            {
                cerr << "Not able to fetch triangle qualities!" << endl;
                exit(1);
            }

            set_output("av_aspect_ratio", doubleArray->GetValue(1));
            set_output("max_aspect_ratio", doubleArray->GetValue(2));
            set_output("cell_count", (int)(input_mesh().GetMesh()->GetNumberOfCells()));
            set_output("vertex_count", (int)(input_mesh().GetMesh()->GetNumberOfPoints()));

            return true;
        }
    }
}