/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                     ---------------------------------------
                                 VGModeler
                     ---------------------------------------
                            
   authors:    
               Josef Weinbub                      weinbub@iue.tuwien.ac.at
               Johann Cervenka                   cervenka@iue.tuwien.ac.at
               Franz Stimpfl
               Rene Heinzl
               Philipp Schwaha

   license:    see file LICENSE in the base directory
   ======================================================================= */
   
#include <cstdlib>
#include <cstring>
#include <iostream>   
   
namespace vgmnetgen {
#include "interface/nglib.h"
}
   

struct hull_adaptor
{
   hull_adaptor();
   ~hull_adaptor();
   
   //template<typename TriangleT>
   void add_hull_element(double const& tri);
   
   char                             * cfilename;   
   vgmnetgen::Ng_Mesh                * mesh;   
   vgmnetgen::Ng_STL_Geometry        * geom;
   vgmnetgen::Ng_Meshing_Parameters    mp;
};


//template void hull_adaptor::add_hull_element<double>(double const&) ;


