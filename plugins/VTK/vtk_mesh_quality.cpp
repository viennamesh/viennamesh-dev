#include "vtk_mesh_quality.hpp"
#include "vtk_ug_mesh.hpp"

#include <vtkMeshQuality.h>
#include <vtkDoubleArray.h>
#include <vtkFieldData.h>
#include <vtkSmartPointer.h>

namespace viennamesh {

    namespace VTK_UnstructuredGrid {

        mesh_quality::mesh_quality() {}

        bool mesh_quality::run(viennamesh::algorithm_handle &) {
            info(5) << "Running vtk_mesh_quality." << std::endl;

            // Get required input parameters
            data_handle<VTK_UnstructuredGrid::mesh> input_mesh = get_required_input<VTK_UnstructuredGrid::mesh>("mesh");

            int geometric_dimension = 3;
            int cell_type = input_mesh().GetMesh()->GetCellType(0);
            int cell_dimension;

            if (cell_type == VTK_TRIANGLE)
            {
                cell_dimension = 2;
            }

            else if (cell_type == VTK_TETRA)
            {
                cell_dimension = 3;
            }

            else 
            {
                error(1) << "Unknown cell dimension" << std::endl;
                return false;
            }

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
                error(1) << "3D Volume Mesh Quality Analysis not yet implemented fully" << std::endl;

                //data handles for the tetrahedral mesh metrics implemented in VTK's MeshQuality
                data_handle<bool> EdgeRatio = get_input<bool>("EdgeRatio");
                data_handle<bool> AspectRatio = get_input<bool>("AspectRatio");
                data_handle<bool> RadiusRatio = get_input<bool>("RadiusRatio");
                data_handle<bool> FrobeniusNorm = get_input<bool>("FrobeniusNorm");
                data_handle<bool> MinimalDihedralAngle = get_input<bool>("MinimalDihedralAngle");
                data_handle<bool> CollapseRatio = get_input<bool>("CollapseRatio");
                data_handle<bool> AspectBeta = get_input<bool>("AspectBeta");
                data_handle<bool> AspectGamma = get_input<bool>("AspectGamma");
                data_handle<bool> Volume = get_input<bool>("Volume");
                data_handle<bool> Condition = get_input<bool>("Condition");
                data_handle<bool> Jacobian = get_input<bool>("Jacobian");
                data_handle<bool> ScaledJacobian = get_input<bool>("ScaledJacobian");
                data_handle<bool> Shape = get_input<bool>("Shape");
                data_handle<bool> RelSizeSquared = get_input<bool>("RelSizeSquared");
                data_handle<bool> ShapeSize = get_input<bool>("ShapeSize");
                data_handle<bool> Distortion = get_input<bool>("Distortion");

                //VTK MeshQuality object 
                vtkSmartPointer<vtkMeshQuality> quality = vtkSmartPointer<vtkMeshQuality>::New();

                quality->SetInputData( input_mesh().GetMesh() );
                quality->SaveCellQualityOn();

                //EdgeRatio
                if ( EdgeRatio.valid() && EdgeRatio() )
                {
                    std::cout << " Mesh Quality Metric : EdgeRatio" << std::endl;

                    quality->SetTetQualityMeasureToEdgeRatio();
                    quality->Update();

                    //Put data into quantity field
                    viennagrid::quantity_field edge_ratio_field(3,1);
                    edge_ratio_field.set_name("EdgeRatio");
                } //end of EdgeRatio

                //AspectRatio
                if ( AspectRatio.valid() && AspectRatio() )
                {
                    std::cout << " Mesh Quality Metric : AspectRatio" << std::endl;
                } //end of AspectRatio

                //RadiusRatio
                if ( RadiusRatio.valid() && RadiusRatio() )
                {
                    std::cout << " Mesh Quality Metric : RadiusRatio" << std::endl;
                } //end of RadiusRatio

                //FrobeniusNorm
                if ( FrobeniusNorm.valid() && FrobeniusNorm() )
                {
                    std::cout << " Mesh Quality Metric : FrobeniusNorm" << std::endl;
                } //end of FrobeniusNorm

                //MinimalDihedralAngle
                if ( MinimalDihedralAngle.valid() && MinimalDihedralAngle() )
                {
                    std::cout << " Mesh Quality Metric : MinimalDihedralAngle" << std::endl;
                } //end of MinimalDihedralAngle

                //CollapseRatio
                if ( CollapseRatio.valid() && CollapseRatio() )
                {
                    std::cout << " Mesh Quality Metric : CollapseRatio" << std::endl;
                } //end of CollapseRatio

                //AspectBeta
                if ( AspectBeta.valid() && AspectBeta() )
                {
                    std::cout << " Mesh Quality Metric : AspectBeta" << std::endl;
                } //end of AspectBeta

                //AspectGamma
                if ( AspectGamma.valid() && AspectGamma() )
                {
                    std::cout << " Mesh Quality Metric : AspectGamma" << std::endl;
                } //end of AspectGamma

                //Volume
                if ( Volume.valid() && Volume() )
                {
                    std::cout << " Mesh Quality Metric : Volume" << std::endl;
                } //end of Volume

                //Condition
                if ( Condition.valid() && Condition() )
                {
                    std::cout << " Mesh Quality Metric : Condition" << std::endl;
                } //end of Condition

                //Jacobian
                if ( Jacobian.valid() && Jacobian() )
                {
                    std::cout << " Mesh Quality Metric : Jacobian" << std::endl;
                } //end of Jacobian

                //ScaledJacobian
                if ( ScaledJacobian.valid() && ScaledJacobian() )
                {
                    std::cout << " Mesh Quality Metric : ScaledJacobian" << std::endl;
                } //end of ScaledJacobian

                //Shape
                if ( Shape.valid() && Shape() )
                {
                    std::cout << " Mesh Quality Metric : Shape" << std::endl;
                } //end of Shape

                //RelSizeSquared
                if ( RelSizeSquared.valid() && RelSizeSquared() )
                {
                    std::cout << " Mesh Quality Metric : RelSizeSquared" << std::endl;
                } //end of RelSizeSquared

                //ShapeSize
                if ( ShapeSize.valid() && ShapeSize() )
                {
                    std::cout << " Mesh Quality Metric : ShapeSize" << std::endl;
                } //end of ShapeSize

                //Distortion
                if ( Distortion.valid() && Distortion() )
                {
                    std::cout << " Mesh Quality Metric : Distortion" << std::endl;
                } //end of Distortion

            } //end of 3D volume mesh

            else
            {
                error(1) << " Geometric dimension is " << geometric_dimension << " and cell dimension is " << cell_dimension << std::endl;
                return false;
            }

            return VIENNAMESH_SUCCESS;
        }
    }
}