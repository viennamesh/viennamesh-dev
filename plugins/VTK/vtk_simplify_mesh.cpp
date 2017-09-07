// #ifdef VIENNAMESH_WITH_TETGEN

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
#include "vtk_mesh.hpp"
#include "vtk_simplify_mesh.hpp"

#include "vtkQuadricClustering.h"

namespace viennamesh {

    namespace vtk {

        simplify_mesh::simplify_mesh() {}

        bool simplify_mesh::run(viennamesh::algorithm_handle &) {
            info(5) << "Running VTK mesh reduction." << std::endl;

            info(5) << "Get input mesh." << std::endl;
            data_handle<vtk::mesh> input_mesh = get_required_input<vtk::mesh>("mesh");

            info(5) << "Create internal mesh." << std::endl;
            vtk::mesh * my_mesh = new vtk::mesh();
            my_mesh->GetMesh()->DeepCopy((vtkDataObject*)(input_mesh().GetMesh()));

            vtkQuadricClustering *quadricClustering = vtkQuadricClustering::New();
            quadricClustering->SetInputData(my_mesh->GetMesh());
            quadricClustering->UseFeatureEdgesOn();
            quadricClustering->Update();

            my_mesh->SetMesh(quadricClustering->GetOutput());

            debug(5) << "Input mesh cell count: " << my_mesh->GetMesh()->GetNumberOfCells() << std::endl;

            info(5) << "Set output mesh." << std::endl;
            set_output("mesh", *my_mesh);
            info(5) << "Output mesh set." << std::endl;

            return true;
        }
    }
}

// #endif
