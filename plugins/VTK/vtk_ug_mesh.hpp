#ifndef VIENNAMESH_ALGORITHM_VTK_UG_MESH_HPP
#define VIENNAMESH_ALGORITHM_VTK_UG_MESH_HPP

/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include "viennameshpp/plugin.hpp"
#include <vtkUnstructuredGrid.h>
#include <vtkSmartPointer.h>
#include <vtkTetra.h>

namespace viennamesh
{
    namespace VTK_UnstructuredGrid
    {
        class VTK_UG_Wrapper
        {
            public:
                VTK_UG_Wrapper() : m_mesh(vtkSmartPointer<vtkUnstructuredGrid>::New()) {};
                vtkUnstructuredGrid * GetMesh() const {return m_mesh;}

                void SetPoints(vtkPoints * points) { m_mesh->SetPoints(points); }
                void SetCells(vtkCellArray * cells) { m_mesh->SetCells(VTK_TETRA, cells); }
            
                vtkPoints* GetPoints() const { return m_mesh->GetPoints(); }
                vtkCellArray* GetCells() const { return m_mesh->GetCells(); }
            private:
                vtkSmartPointer<vtkUnstructuredGrid> m_mesh;
        };

        typedef VTK_UG_Wrapper mesh;
    }

    viennamesh_error convert(viennagrid::mesh const & input, VTK_UnstructuredGrid::mesh & output);
    viennamesh_error convert(VTK_UnstructuredGrid::mesh const & input, viennagrid::mesh & output);

    template<>
    viennamesh_error internal_convert<viennagrid_mesh, VTK_UnstructuredGrid::mesh>(viennagrid_mesh const & input, VTK_UnstructuredGrid::mesh & output);
    template<>
    viennamesh_error internal_convert<VTK_UnstructuredGrid::mesh, viennagrid_mesh>(VTK_UnstructuredGrid::mesh const & input, viennagrid_mesh & output);

    namespace result_of
    {
        template<>
        struct data_information<VTK_UnstructuredGrid::mesh>
        {
            static std::string type_name() { return "VTK_UnstructuredGrid::mesh"; }
            static viennamesh_data_make_function make_function() { return generic_make<VTK_UnstructuredGrid::mesh>; }
            static viennamesh_data_delete_function delete_function() { return generic_delete<VTK_UnstructuredGrid::mesh>; }
        };
    }
}
#endif