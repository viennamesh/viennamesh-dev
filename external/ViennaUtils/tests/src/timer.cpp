/* =============================================================================
   Copyright (c) 2010, 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                ViennaUtils - The Vienna Utilities Library
                             -----------------

   authors:    Josef Weinbub                      weinbub@iue.tuwien.ac.at

   license:    see file LICENSE in the base directory
============================================================================= */

#include<vector>
#include<cmath>

#include"viennautils/timer.hpp"


int main(int argc, char * argv[])
{
   typedef std::vector<double>  cont_type;
   cont_type cont(1E6);

   for(size_t i = 0; i < cont.size(); i++)   
   {
      cont[i] = i*1.0;
   }

   viennautils::Timer timer;
   timer.start();

   double eval;
   for(size_t i = 0; i < cont.size(); i++)   
   {
      eval = sqrt(cont[i]*cont[i]*exp(cont[i]));
   }
   std::cout << "timer exec-time: " << timer.get() << std::endl;

   return 0;
}
