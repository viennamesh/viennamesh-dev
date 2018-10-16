#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include "viennagrid/viennagrid.hpp"
#include "vtk_mesh.hpp"
#include <vtkDoubleArray.h>
#include <vtkTetra.h>

typedef viennagrid::mesh                                                        ViennaGridMeshType;
typedef viennagrid::result_of::const_vertex_range<ViennaGridMeshType>::type     ConstVertexRangeType;
typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type             ConstVertexIteratorType;
typedef viennagrid::result_of::const_element_range<ViennaGridMeshType, 2>::type ConstCellRangeType;
typedef viennagrid::result_of::iterator<ConstCellRangeType>::type               ConstCellIteratorType;

typedef viennagrid::mesh                                                        MeshType;
typedef viennagrid::result_of::element<MeshType>::type                          ElementType;
typedef viennagrid::result_of::const_element_range<ElementType>::type           ConstBoundaryElementRangeType;
typedef viennagrid::result_of::iterator<ConstBoundaryElementRangeType>::type    ConstBoundaryElementIteratorType;

namespace viennamesh {

    viennamesh_error convert(viennagrid::mesh const &input, VTK_PolyData::mesh &output) {

        //info(5) << "Converting from viennagrid to vtk. (2D)" << std::endl;

        viennagrid_dimension geometric_dimension = viennagrid::geometric_dimension( input );
        viennagrid_dimension cell_dimension = viennagrid::cell_dimension( input );

        ConstVertexRangeType    vertices(input);
        ConstCellRangeType      cells(input);

        vtkSmartPointer<vtkPoints> lVertices = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkCellArray> lCells = vtkSmartPointer<vtkCellArray>::New();

        //Hull mesh
        if (geometric_dimension == 3 && cell_dimension == 2)
        {

            for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit) {
                lVertices->InsertNextPoint(
                        viennagrid::get_point(input, *vit)[0],
                        viennagrid::get_point(input, *vit)[1],
                        viennagrid::get_point(input, *vit)[2]
                );
            }

            vtkIdType lVertexIndices[3] = {0, 0, 0};

            for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit) {
                if (!(*cit).is_triangle())
                {
                    error(1) << "vtk_simplify_mesh just operates on triangle meshes" << std::endl;
                    return false; // This plugin just operates on triangle meshes
                }

                ConstBoundaryElementRangeType boundary_vertices(*cit, 0);

                int lIndex = 0;
                for (ConstBoundaryElementIteratorType vit = boundary_vertices.begin(); vit != boundary_vertices.end(); ++vit, ++lIndex) {
                    lVertexIndices[lIndex] = (vtkIdType) (*vit).id().index();

                    // Looking for input cells which do refer to non existing vertices.
                    if (lVertexIndices[lIndex] >= lVertices->GetNumberOfPoints())
                        error(1) << "Cell refers to non existing vertex." << std::endl;
                }

                // Looking for degenerated cells after viennagrid removed duplicate vertices
                // TODO: Solve degenerated cells problem in viennagrid vtk reader
                if (lVertexIndices[0] != lVertexIndices[1]
                && lVertexIndices[0] != lVertexIndices[2]
                && lVertexIndices[1] != lVertexIndices[2])
                {
                    lCells->InsertNextCell(3, lVertexIndices);
                }
            }

            output.SetPoints(lVertices);
            output.SetPolys(lCells);
        } // end of if (geometric_dimension == 2)

        //3D Volume mesh
        else if (geometric_dimension == 3 && cell_dimension == 3)
        {
            std::cerr << "\nTODO: conversion to 3D volume mesh in VTK format" << std::endl;
            
            //create basic data
            auto output = vtkSmartPointer<vtkPolyData>::New();
            auto points = vtkSmartPointer<vtkPoints>::New();
            auto cells = vtkSmartPointer<vtkCellArray>::New();
            auto data = vtkSmartPointer<vtkDoubleArray>::New();
            data->SetNumberOfComponents(1);

            //get basic mesh information
            size_t NNodes = viennagrid::vertex_count(input);
            size_t NElements = viennagrid::cells(input).size();
            size_t cell_dimension = viennagrid::cell_dimension(input);
            size_t geometric_dimension = viennagrid::geometric_dimension(input);

            //Initialize x-, y-, and, z-coordinates
            //
            //create pointer to iterate over viennagrid_array
            viennagrid_numeric* ptr_coords = nullptr;

            //get pointer to coordinates array from the mesh
            viennagrid_mesh_vertex_coords_pointer(input.internal(), &ptr_coords);

            for (size_t i = 0; i < NNodes; ++i)
            {
                if (geometric_dimension == 2)
                {
                    //double xyz[3] = {*(ptr_coords++), *(ptr_coords++), 0.0};
                    double xyz[3] = {ptr_coords[0], ptr_coords[1], 0.0};
                    points->InsertNextPoint(xyz);
                }

                else
                {
                    //double xyz[3] = {*(ptr_coords++), *(ptr_coords++), *(ptr_coords++)};
                    double xyz[3] = {ptr_coords[0], ptr_coords[1], ptr_coords[2]};
                    points->InsertNextPoint(xyz);
                }
            } //end of for (size_t i = 0; i < NNodes; ++i)

            //Iterate over all tetrahedra in the mesh
            viennagrid_element_id * element_ids_begin;
            viennagrid_element_id * element_ids_end;
            viennagrid_dimension topological_dimension = geometric_dimension;		//produces segmentation fault if not set to 2 for 2d and to 3 for 3d case

            //get elements from mesh
            viennagrid_mesh_elements_get(input.internal(), topological_dimension, &element_ids_begin, &element_ids_end);

            viennagrid_element_id * boundary_vertex_ids_begin;
            viennagrid_element_id * boundary_vertex_ids_end;
            viennagrid_dimension boundary_topological_dimension = 0;
            viennagrid_element_id cell_id;

            //outer for loop iterates over all elements with dimension = topological_dimension (2 for triangles, 3 for tetrahedrons)
            //inner for loop iterates over all elements with dimension = boundary_topological_dimension (0 for vertices)
            for (viennagrid_element_id * vit = element_ids_begin; vit != element_ids_end; ++vit)
            {
                //get vertices of triangle
                cell_id = *vit;
                viennagrid_element_boundary_elements(input.internal(), cell_id, boundary_topological_dimension, &boundary_vertex_ids_begin, &boundary_vertex_ids_end);

                auto tmp_tetrahedron = vtkSmartPointer<vtkTetra>::New();

                int vid = 0;
                for (viennagrid_element_id * bit = boundary_vertex_ids_begin; bit != boundary_vertex_ids_end; ++bit)
                {
                    viennagrid_element_id vertex_id = *bit;

                    tmp_tetrahedron->GetPointIds()->SetId(vid, *bit);

                    ++vid;
                }

                cells->InsertNextCell(tmp_tetrahedron);
            } //end of for (viennagrid_element_id * vit = element_ids_begin; vit != element_ids_end; ++vit)

            output->SetPoints(points);
            output->SetPolys(cells);

            std::cout << "Vertices after conversion: " << output->GetNumberOfPoints() << " Elements after conversion: " << output->GetNumberOfCells() << std::endl;
            std::cout << output << std::endl;

            return true;
        } //end of if geometric_dimension == 3

        else
        {
            std::cerr << "geometric dimension is " << geometric_dimension << std::endl;
            return false;
        }

        //info(5) << "Finished converting from viennagrid to vtk.";
        return VIENNAMESH_SUCCESS;
    }

    viennamesh_error convert(VTK_PolyData::mesh const & input, viennagrid::mesh & output) {
        /*debug(5) << "Converting from vtk to viennagrid. (2D)" << std::endl;
        debug(5) << "Input has: " << input.GetMesh()->GetNumberOfCells() << " cells" << std::endl;*/

        typedef viennagrid::mesh                                MeshType;
        typedef viennagrid::result_of::element<MeshType>::type  VertexType;

        vtkSmartPointer<vtkPoints> lVertices = vtkSmartPointer<vtkPoints>::New();
        lVertices = input.GetPoints();

        vtkSmartPointer<vtkCellArray> lCells = vtkSmartPointer<vtkCellArray>::New();
        lCells = input.GetPolys();

        vtkIdType lNumberOfVertices = lVertices->GetNumberOfPoints();

        std::vector<VertexType> vertex_handles;
        vertex_handles.reserve(lNumberOfVertices);

        for(vtkIdType i = 0; i < lNumberOfVertices; ++i)
        {
            double* lVertex = lVertices->GetPoint(i);

            vertex_handles[i] = viennagrid::make_vertex
                                    (
                                    output,
                                    viennagrid::make_point(lVertex[0],
                                                           lVertex[1],
                                                           lVertex[2])
                                    );
        }

        vtkIdList *lVertexIndices = vtkIdList::New();
        while(lCells->GetNextCell(lVertexIndices)) {
            viennagrid::make_triangle(
                    output,
                    vertex_handles[lVertexIndices->GetId(0)],
                    vertex_handles[lVertexIndices->GetId(1)],
                    vertex_handles[lVertexIndices->GetId(2)]
            );
        }

        info(5) << "Finished converting from VTK_PolyData to viennagrid." << std::endl;
        return VIENNAMESH_SUCCESS;
    }

    template<>
    viennamesh_error internal_convert<viennagrid_mesh, VTK_PolyData::mesh>(viennagrid_mesh const & input, VTK_PolyData::mesh & output) {
        return convert(input, output);
    }

    template<>
    viennamesh_error internal_convert<VTK_PolyData::mesh, viennagrid_mesh>(VTK_PolyData::mesh const & input, viennagrid_mesh & output) {
        viennagrid::mesh output_pp(output);
        return convert(input, output_pp);
    }

}
