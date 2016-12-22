#ifndef VIENNAMESH_ALGORITHM_PRAGMATIC_METIS_PARTITIONING_HPP
#define VIENNAMESH_ALGORITHM_PRAGMATIC_METIS_PARTITIONING_HPP

#include "viennameshpp/plugin.hpp"

typedef viennagrid::mesh                                          MeshType;

namespace viennamesh
{
	class pragmatic_metis_partitioner : public plugin_algorithm
  	{	
  		public:
    			pragmatic_metis_partitioner();

    			static std::string name();
    			bool run(viennamesh::algorithm_handle &);
  	};
}

#endif
