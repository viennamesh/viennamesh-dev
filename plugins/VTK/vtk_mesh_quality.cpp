#include "vtk_mesh_quality.hpp"
#include "vtk_mesh.hpp"

#include <vtkMeshQuality.h>
#include <vtkDoubleArray.h>
#include <vtkFieldData.h>
#include <vtkSmartPointer.h>

namespace viennamesh {

    namespace VTK_PolyData {

        mesh_quality::mesh_quality() {}

        bool mesh_quality::run(viennamesh::algorithm_handle &) {
            info(5) << "Running vtk_mesh_quality." << std::endl;

            // Get required input parameters
            data_handle<VTK_PolyData::mesh> input_mesh = get_required_input<VTK_PolyData::mesh>("mesh");

            vtkPolyData * mesh = input_mesh().GetMesh();

            std::cout << mesh << std::endl;
            std::cout << input_mesh().GetMesh() << std::endl;

            mesh->SetPolys( input_mesh().GetMesh()->GetPolys() );

            std::cout << "NNodes   : " << mesh->GetNumberOfPoints() << std::endl;
            std::cout << "NElements: " << mesh->GetNumberOfCells() << std::endl;

            std::cout << "NNodes   : " << input_mesh().GetMesh()->GetNumberOfPoints() << std::endl;
            std::cout << "NElements: " << mesh->GetNumberOfCells() << std::endl;

            int geometric_dimension = 3;
            int cell_dimension = mesh->GetCellType(0);

            //Hull mesh
            if (geometric_dimension == 3 && cell_dimension == 2)
            {

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
            } //end of hull mesh

            //3D volume mesh
            else if (geometric_dimension == 3 && cell_dimension == 3)
            {
                ;
            } //end of 3D volume mesh

            else
            {
                viennamesh::error(1) << " Geometric dimension is " << geometric_dimension << " and cell dimension is " << cell_dimension << std::endl;
                return false;
            }

            return VIENNAMESH_SUCCESS;
        }
    }
}