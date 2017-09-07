#ifndef VIENNAMESH_VTK_QUADRIC_DECIMATION_HPP
#define VIENNAMESH_VTK_QUADRIC_DECIMATION_HPP

#include "viennameshpp/plugin.hpp"

namespace viennamesh
{
    namespace vtk
    {
        class quadric_decimation :
                public plugin_algorithm
        {
            public:
                quadric_decimation();

                static std::string name() { return "vtk_quadric_decimation"; }
                bool run(viennamesh::algorithm_handle&);
        };
    }
}


#endif //VIENNAMESH_VTK_QUADRIC_DECIMATION_HPP
