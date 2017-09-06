#ifndef VIENNAMESH_ALGORITHM_PRAGMATIC_REFINE_MULTI_HPP
#define VIENNAMESH_ALGORITHM_PRAGMATIC_REFINE_MULTI_HPP

#include "viennameshpp/plugin.hpp"

typedef viennagrid::mesh                                          MeshType;

namespace viennamesh
{
	class pragmatic_refine_multi_region : public plugin_algorithm
  	{	
  		public:
    			pragmatic_refine_multi_region();

    			static std::string name();
    			bool run(viennamesh::algorithm_handle &);
  	};
}

#endif
