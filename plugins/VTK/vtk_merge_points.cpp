#include "vtk_ug_mesh.hpp"
#include "vtk_merge_points.hpp"

#include <vtkMergePoints.h>
#include <vtkCellArray.h>

#include <unordered_map>

namespace viennamesh {

    namespace VTK_PolyData 
    {

        merge_points::merge_points() {}

        bool merge_points::run(viennamesh::algorithm_handle &) 
        {
            info(1) << "Merging duplicate vertices" << std::endl;

            // Get required input parameters
            data_handle<VTK_UnstructuredGrid::mesh> input_mesh = get_required_input<VTK_UnstructuredGrid::mesh>("mesh");

            info(2) << "  Input mesh has " << input_mesh().GetMesh()->GetNumberOfPoints() << " vertices." << std::endl;
            info(2) << "  Input mesh has " << input_mesh().GetMesh()->GetNumberOfCells() << " cells." << std::endl;

            VTK_UnstructuredGrid::mesh* my_mesh = new VTK_UnstructuredGrid::mesh();
           // my_mesh->GetMesh()->DeepCopy((vtkDataObject*)(input_mesh().GetMesh()));
            
            // Insert all of the points
            vtkSmartPointer<vtkMergePoints> mergePoints = vtkSmartPointer<vtkMergePoints>::New();
            mergePoints->InitPointInsertion(input_mesh().GetMesh()->GetPoints(), input_mesh().GetMesh()->GetBounds());

            vtkIdType id;
            double point[3];
            std::unordered_map<int, int> index_mapping;

            for (size_t i = 0; i < input_mesh().GetMesh()->GetNumberOfPoints(); ++i)
            {
                input_mesh().GetMesh()->GetPoint(i, point);
                int inserted = mergePoints->InsertUniquePoint(point, id);

                index_mapping.insert(std::make_pair<int,int>(i, id));
            }

            int max = 0;
            for (auto it : index_mapping)
            {
                if (it.second > max)
                    max = it.second;
            }

            //Create UnstructuredGrid using the merged points
            //vtkSmartPointer<vtkUnstructuredGrid> ug = vtkSmartPointer<vtkUnstructuredGrid>::New();

            //create new point set for the unstructured grid
            vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

            for (size_t i = 0; i <= max; ++i)
            {
                input_mesh().GetMesh()->GetPoint(i, point);
                points->InsertNextPoint(point);
            }

            my_mesh->SetPoints(points);

            //create grid cells using the index mapping
            vtkSmartPointer<vtkIdList> IdList = vtkSmartPointer<vtkIdList>::New();
            vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();

            for (size_t i = 0; i < input_mesh().GetMesh()->GetNumberOfCells(); ++i)
            {
                input_mesh().GetMesh()->GetCellPoints(i, IdList);

                vtkSmartPointer<vtkTetra> tetra = vtkSmartPointer<vtkTetra>::New();

                for (size_t j = 0; j < 4; ++j)
                {
                    tetra->GetPointIds()->SetId(j, index_mapping[IdList->GetId(j)]);
                }
                cellArray->InsertNextCell(tetra);
            }

            my_mesh->SetCells(cellArray);

            info(2) << "  Output mesh has " << my_mesh->GetMesh()->GetNumberOfPoints() << " vertices." << std::endl;
            info(2) << "  Output mesh has " << my_mesh->GetMesh()->GetNumberOfCells() << " cells." << std::endl;

            set_output("mesh", *my_mesh);
           
            return VIENNAMESH_SUCCESS;
        }
    }
}