#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include "viennagrid/viennagrid.hpp"
#include "vtk_mesh.hpp"

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

    viennamesh_error convert(viennagrid::mesh const &input, vtk::mesh &output) {

        debug(5) << "Converting from viennagrid to vtk." << std::endl;

        ConstVertexRangeType    vertices(input);
        ConstCellRangeType      cells(input);

        vtkSmartPointer<vtkPoints> lVertices = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkCellArray> lCells = vtkSmartPointer<vtkCellArray>::New();

        for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit) {
            lVertices->InsertNextPoint(
                    viennagrid::get_point(input, *vit)[0],
                    viennagrid::get_point(input, *vit)[1],
                    viennagrid::get_point(input, *vit)[2]
            );
        }

        vtkIdType lVertexIndizes[3] = {0, 0, 0};

        for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit) {
            if (!(*cit).is_triangle())
            {
                error(1) << "vtk_simplify_mesh just operates on triangle meshes" << std::endl;
                return false; // This plugin just operates on triangle meshes
            }

            ConstBoundaryElementRangeType boundary_vertices(*cit, 0);

            int lIndex = 0;
            for (ConstBoundaryElementIteratorType vit = boundary_vertices.begin(); vit != boundary_vertices.end(); ++vit, ++lIndex) {
                lVertexIndizes[lIndex] = (vtkIdType) (*vit).id().index();

                // Looking for input cells which do refer to non existing vertices.
                if (lVertexIndizes[lIndex] >= lVertices->GetNumberOfPoints())
                    error(1) << "Cell refers to non existing vertex." << std::endl;
            }

            // Looking for degenerated cells after viennagrid removed duplicate vertices
            // TODO: Solve degenerated cells problem in viennagrid vtk reader
            if (lVertexIndizes[0] != lVertexIndizes[1]
             && lVertexIndizes[0] != lVertexIndizes[2]
             && lVertexIndizes[1] != lVertexIndizes[2])
            {
                lCells->InsertNextCell(3, lVertexIndizes);
            }
        }

        output.SetPoints(lVertices);
        output.SetPolys(lCells);

        debug(5) << "Finished converting from viennagrid to vtk." << std::endl;
        return VIENNAMESH_SUCCESS;
    }

    viennamesh_error convert(vtk::mesh const & input, viennagrid::mesh & output) {
        debug(5) << "Converting from vtk to viennagrid." << std::endl;
        debug(5) << "Input has: " << input.GetMesh()->GetNumberOfCells() << " cells" << std::endl;

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

        debug(5) << "Finished converting from vtk to viennagrid." << std::endl;
        return VIENNAMESH_SUCCESS;
    }

    template<>
    viennamesh_error internal_convert<viennagrid_mesh, vtk::mesh>(viennagrid_mesh const & input, vtk::mesh & output) {
        return convert(input, output);
    }

    template<>
    viennamesh_error internal_convert<vtk::mesh, viennagrid_mesh>(vtk::mesh const & input, viennagrid_mesh & output) {
        viennagrid::mesh output_pp(output);
        return convert(input, output_pp);
    }

}
