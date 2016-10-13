#ifndef VIENNAMESH_ALGORITHM_PRAGMATIC_SMOOTH_HPP
#define VIENNAMESH_ALGORITHM_PRAGMATIC_SMOOTH_HPP

#include "viennameshpp/plugin.hpp"

typedef viennagrid::mesh                                          MeshType;

namespace viennamesh
{
	class pragmatic_smooth : public plugin_algorithm
  	{	
  		public:
    			pragmatic_smooth();

    			static std::string name();
    			bool run(viennamesh::algorithm_handle &);
  	};
}

#endif
