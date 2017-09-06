#ifndef VIENNAMESH_ZOLTAN_PARTITIONING_HPP
#define VIENNAMESH_ZOLTAN_PARTITIONING_HPP

#include "viennameshpp/plugin.hpp"

namespace viennamesh
{
    class zoltan_partitioning : public plugin_algorithm
    {
        public:
            zoltan_partitioning();

            static std::string name();
            bool run(viennamesh::algorithm_handle&);
    };
}


#endif //VIENNAMESH_ZOLTAN_PARTITIONING_HPP
