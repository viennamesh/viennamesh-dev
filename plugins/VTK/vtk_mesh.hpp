#ifndef VIENNAMESH_ALGORITHM_VTK_MESH_HPP
#define VIENNAMESH_ALGORITHM_VTK_MESH_HPP

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

#include <list>
#include "viennameshpp/plugin.hpp"
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>


namespace viennamesh
{
  namespace vtk
  {
      class vtkPolyDataWrapper
      {
          public:
              vtkPolyDataWrapper() :
                      m_Mesh(vtkSmartPointer<vtkPolyData>::New()),
                      m_IsCopy(false)
              {
                  static int id = 0;
                  m_ID = id++;
                  debug(5) << "Create new vtk mesh with id: " << m_ID << std::endl;
              }
              ~vtkPolyDataWrapper()
              {
                  if (!m_IsCopy)
                  {
                      debug(5) << "Delete vtk mesh with id: " << m_ID << std::endl;
                  }
                  else
                  {
                      debug(5) << "Delete vtk poly data wrapper copy with id: " << m_ID << std::endl;
                  }
              }
              vtkPolyDataWrapper(const vtkPolyDataWrapper& cpy) :
                      m_Mesh(cpy.m_Mesh),
                      m_ID(cpy.m_ID),
                      m_IsCopy(true)
              {
                  debug(5) << "MESH GETS COPIED, id: " << m_ID << std::endl;
              }

              vtkPolyData * GetMesh() const { return m_Mesh; }
              void SetMesh(vtkPolyData * setToMesh) { m_Mesh = setToMesh; }

              void SetPoints(vtkPoints * points) { m_Mesh->SetPoints(points); }
              void SetPolys(vtkCellArray * polys) { m_Mesh->SetPolys(polys); }

              vtkPoints* GetPoints() const { return m_Mesh->GetPoints(); }
              vtkCellArray* GetPolys() const { return m_Mesh->GetPolys(); }

          private:
              vtkSmartPointer<vtkPolyData> m_Mesh;
              int m_ID;
              bool m_IsCopy;
      };

      typedef vtkPolyDataWrapper mesh;
  }

    viennamesh_error convert(viennagrid::mesh const & input, vtk::mesh & output);
    viennamesh_error convert(vtk::mesh const & input, viennagrid::mesh & output);

    template<>
    viennamesh_error internal_convert<viennagrid_mesh, vtk::mesh>(viennagrid_mesh const & input, vtk::mesh & output);
    template<>
    viennamesh_error internal_convert<vtk::mesh, viennagrid_mesh>(vtk::mesh const & input, viennagrid_mesh & output);

  namespace result_of
  {
    template<>
    struct data_information<vtk::mesh>
    {
      static std::string type_name() { return "vtk::mesh"; }
      static viennamesh_data_make_function make_function() { return generic_make<vtk::mesh>; }
      static viennamesh_data_delete_function delete_function() { return generic_delete<vtk::mesh>; }
    };
  }


}

#endif
