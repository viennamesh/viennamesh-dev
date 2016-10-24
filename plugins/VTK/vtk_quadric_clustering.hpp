#ifndef VIENNAMESH_VTK_QUADRIC_CLUSTERING_HPP
#define VIENNAMESH_VTK_QUADRIC_CLUSTERING_HPP

#include "viennameshpp/plugin.hpp"

namespace viennamesh
{
    namespace vtk
    {
        class quadric_clustering :
                public plugin_algorithm
        {
            public:
                quadric_clustering();

                static std::string name() { return "vtk_quadric_clustering"; }
                bool run(viennamesh::algorithm_handle&);
        };
    }
}


#endif //VIENNAMESH_VTK_QUADRIC_CLUSTERING_HPP
