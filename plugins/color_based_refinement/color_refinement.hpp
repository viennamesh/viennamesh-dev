#ifndef VIENNAMESH_ALGORITHM_COLOR_REFINEMENT_HPP
#define VIENNAMESH_ALGORITHM_COLOR_REFINEMENT_HPP

#include "viennameshpp/plugin.hpp"

namespace viennamesh
{
	class color_refinement : public plugin_algorithm
  	{	
  		public:
    			color_refinement();

				static std::string name();
				bool run (viennamesh::algorithm_handle &);
  	};
}

#endif