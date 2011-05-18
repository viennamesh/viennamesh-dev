/* ============================================================================
   Copyright (c) 2007-2010 Franz Stimpfl                          franz@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GSSE_UTIL_FILE_FORMATS_GSSE
#define GSSE_UTIL_FILE_FORMATS_GSSE

// *** GSSE includes
#include "gsse/datastructure/domain_algorithms.hpp"

// ============================================================================

namespace gsse {
   namespace converter {
      
      struct tag_gsse {};     
      
      struct gsse_impl
      {
         template<typename FullSpaceT>
         void operator()( std::string& filename, FullSpaceT& full_space)
         {
            std::cout << ".. reading GSSE format .. " << std::endl;
            std::ifstream file(filename.c_str());
            gsse::read(full_space, file);
         }
         
         template<typename FullSpaceT>
         void operator()(FullSpaceT& full_space, std::string& filename)
         {
            std::cout << ".. writing GSSE format .. " << std::endl;
            std::ofstream file(filename.c_str());
            gsse::write(full_space, file);
         }

         template<typename FullSpaceT>
         void read(FullSpaceT& full_space, std::istream& input = std::cin)
         {
            gsse::read(full_space, input);
         }

         template<typename FullSpaceT>
         void write(FullSpaceT& full_space, std::ostream& output = std::cout)
         {
            gsse::write(full_space, output);
         }
      };

   } // end namespace: converter
} // end namespace: gsse

#endif
