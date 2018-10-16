#ifndef VIENNAMESH_VTK_MERGE_POINTS_HPP
#define VIENNAMESH_VTK_MERGE_POINTS_HPP
#include "viennameshpp/plugin.hpp"

namespace viennamesh
{
    namespace VTK_PolyData
    {
        class merge_points :
                public plugin_algorithm
        {
            public:
                merge_points();

                static std::string name() { return "vtk_merge_points"; }
                bool run(viennamesh::algorithm_handle&);
        };
    }
}
#endif