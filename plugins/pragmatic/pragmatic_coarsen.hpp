#ifndef VIENNAMESH_ALGORITHM_PRAGMATIC_COARSEN_HPP
#define VIENNAMESH_ALGORITHM_PRAGMATIC_COARSEN_HPP

#include "viennameshpp/plugin.hpp"

namespace viennamesh
{
	class pragmatic_coarsen : public plugin_algorithm
  	{	
  		public:
    			pragmatic_coarsen();

    			static std::string name();
    			bool run(viennamesh::algorithm_handle &);
  	};
}

#endif
