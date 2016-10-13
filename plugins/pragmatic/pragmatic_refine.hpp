#ifndef VIENNAMESH_ALGORITHM_PRAGMATIC_REFINE_HPP
#define VIENNAMESH_ALGORITHM_PRAGMATIC_REFINE_HPP

#include "viennameshpp/plugin.hpp"

typedef viennagrid::mesh                                          MeshType;

namespace viennamesh
{
	class pragmatic_refine : public plugin_algorithm
  	{	
  		public:
    			pragmatic_refine();

    			static std::string name();
    			bool run(viennamesh::algorithm_handle &);
  	};
}

#endif
