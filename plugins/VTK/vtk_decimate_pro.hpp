#ifndef VIENNAMESH_VTK_DECIMATE_PRO_HPP
#define VIENNAMESH_VTK_DECIMATE_PRO_HPP

#include "viennameshpp/plugin.hpp"

namespace viennamesh
{
    namespace vtk
    {
        class decimate_pro :
                public plugin_algorithm
        {
            public:
                decimate_pro();

                static std::string name() { return "vtk_decimate_pro"; }
                bool run(viennamesh::algorithm_handle&);
        };
    }
}

#endif //VIENNAMESH_VTK_DECIMATE_PRO_HPP
