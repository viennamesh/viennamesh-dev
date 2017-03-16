#ifndef VIENNAMESH_VTK_MESH_QUALITY_HPP
#define VIENNAMESH_VTK_MESH_QUALITY_HPP

#include "viennameshpp/plugin.hpp"

namespace viennamesh
{
    namespace vtk
    {
        class mesh_quality :
                public plugin_algorithm
        {
            public:
                mesh_quality();

                static std::string name() { return "vtk_mesh_quality"; }
                bool run(viennamesh::algorithm_handle&);
        };
    }
}


#endif //VIENNAMESH_VTK_MESH_QUALITY_HPP
