#ifndef VIENNAMESH_ALGORITHM_PRAGMATIC_SWAPPING_HPP
#define VIENNAMESH_ALGORITHM_PRAGMATIC_SWAPPING_HPP

#include "viennameshpp/plugin.hpp"

typedef viennagrid::mesh                                          MeshType;

namespace viennamesh
{
	class pragmatic_swapping : public plugin_algorithm
  	{	
  		public:
    			pragmatic_swapping();

    			static std::string name();
    			bool run(viennamesh::algorithm_handle &);
  	};
}

#endif
