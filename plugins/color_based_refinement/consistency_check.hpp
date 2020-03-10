#ifndef VIENNAMESH_ALGORITHM_CONSISTENCY_CHECK_HPP
#define VIENNAMESH_ALGORITHM_CONSISTENCY_CHECK_HPP

#include "viennameshpp/plugin.hpp"

namespace viennamesh
{
    class consistency_check : public plugin_algorithm
    {
        public:
            consistency_check();
        
            static std::string name();
            bool run(viennamesh::algorithm_handle &);
    };
}

#endif