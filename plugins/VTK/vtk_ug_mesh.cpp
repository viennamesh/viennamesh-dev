#include "vtk_ug_mesh.hpp"
#include "viennagrid/viennagrid.hpp"

#include <vtkCellArray.h>

namespace viennamesh 
{
    typedef viennagrid::mesh                                                        ViennaGridMeshType;
    typedef viennagrid::result_of::const_vertex_range<ViennaGridMeshType>::type     ConstVertexRangeType;
    typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type             ConstVertexIteratorType;
    typedef viennagrid::result_of::const_element_range<ViennaGridMeshType, 3>::type ConstCellRangeType;
    typedef viennagrid::result_of::iterator<ConstCellRangeType>::type               ConstCellIteratorType;

    typedef viennagrid::mesh                                                        MeshType;
    typedef viennagrid::result_of::element<MeshType>::type                          ElementType;
    typedef viennagrid::result_of::const_element_range<ElementType>::type           ConstBoundaryElementRangeType;
    typedef viennagrid::result_of::iterator<ConstBoundaryElementRangeType>::type    ConstBoundaryElementIteratorType;

    //conversion from viennagrid to VTK_UnstructuredGrid
    viennamesh_error convert(viennagrid::mesh const &input, VTK_UnstructuredGrid::mesh &output) 
    {
        viennagrid_dimension geometric_dimension = viennagrid::geometric_dimension( input );
        viennagrid_dimension cell_dimension = viennagrid::cell_dimension( input );

        ConstVertexRangeType    vertices(input);
        ConstCellRangeType      cells(input);

        vtkSmartPointer<vtkPoints> lVertices = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkCellArray> lCells = vtkSmartPointer<vtkCellArray>::New();

        for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit) 
        {
            lVertices->InsertNextPoint( viennagrid::get_point(input, *vit)[0],
                                        viennagrid::get_point(input, *vit)[1],
                                        viennagrid::get_point(input, *vit)[2] );
        }

        output.SetPoints(lVertices);   

        //std::cout << std::endl << "NNodes: " << output.GetMesh()->GetNumberOfPoints() << std::endl;

        vtkIdType lVertexIndices[4] = {0, 0, 0, 0};  

        for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit) 
        {         
            ConstBoundaryElementRangeType boundary_vertices(*cit, 0);

            vtkSmartPointer<vtkTetra> tetra = vtkSmartPointer<vtkTetra>::New();

            int lIndex = 0;
            for (ConstBoundaryElementIteratorType vit = boundary_vertices.begin(); vit != boundary_vertices.end(); ++vit, ++lIndex) 
            {
                lVertexIndices[lIndex] = (vtkIdType) (*vit).id().index();
            }

            for (size_t j = 0; j < 4; ++j)
            {
                tetra->GetPointIds()->SetId(j, lVertexIndices[j]);
            }

            lCells->InsertNextCell(tetra);
        }

        output.SetCells(lCells);

        return VIENNAMESH_SUCCESS;
    } //end of conversion from viennagrid to VTK_UnstructuredGrid

    //conversion from VTK_UnstructuredGrid to ViennaGrid
    viennamesh_error convert(VTK_UnstructuredGrid::mesh const & input, viennagrid::mesh & output) 
    {
        //ViennaGrid typedefs
        typedef viennagrid::mesh                                                        MeshType;
        typedef viennagrid::result_of::element<MeshType>::type                          VertexType;
        typedef viennagrid::result_of::element<MeshType>::type							TetrahedronType;

        //create empty vector of size NumberOfVertices containing viennagrid vertices
        std::vector<TetrahedronType> tet_handles(input.GetMesh()->GetNumberOfPoints());        

        //Iterate all points and store their coordinates in the viennagrid tet_handles vector
        double point[3];
        for (size_t i = 0; i < input.GetMesh()->GetNumberOfPoints(); ++i)
        {
            input.GetMesh()->GetPoint(i, point);
            //std::cout << "vertex " << i << ": " << point[0] << " " << point[1] << " " << point[2] << std::endl;
            tet_handles[i] = viennagrid::make_vertex(output,
                                                     viennagrid::make_point( point[0],
                                                                             point[1],
                                                                             point[2]) 
                                                    );
        } //end of Iterate all points and store their coordinates in the viennagrid tet_handles vector

        //Iterating all elements and create their corresponding viennagrid tetrahedrons
        vtkSmartPointer<vtkIdList> IdList = vtkSmartPointer<vtkIdList>::New();

        for (size_t i = 0; i < input.GetMesh()->GetNumberOfCells(); ++i)
        {
            input.GetMesh()->GetCellPoints(i, IdList);
            viennagrid::make_tetrahedron(output,
                                         tet_handles[IdList->GetId(0)],
                                         tet_handles[IdList->GetId(1)],
                                         tet_handles[IdList->GetId(2)],
                                         tet_handles[IdList->GetId(3)]);
        }

        return VIENNAMESH_SUCCESS;
    } //end of conversion from VTK_UnstructuredGrid to ViennaGrid

    template<>
    viennamesh_error internal_convert<viennagrid_mesh, VTK_UnstructuredGrid::mesh>(viennagrid_mesh const & input, VTK_UnstructuredGrid::mesh & output) 
    {
        return convert(input, output);
    }

    template<>
    viennamesh_error internal_convert<VTK_UnstructuredGrid::mesh, viennagrid_mesh>(VTK_UnstructuredGrid::mesh const & input, viennagrid_mesh & output) 
    {
        viennagrid::mesh output_pp(output);
        return convert(input, output_pp);
    }

}