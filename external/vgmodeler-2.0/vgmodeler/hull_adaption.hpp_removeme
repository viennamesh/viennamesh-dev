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
 
#ifndef VGMODELER_HULL_ADAPTION_HPP
#define VGMODELER_HULL_ADAPTION_HPP


namespace nglib {
#include "vgmodeler/hull_adaption/interface/nglib.h"
}
   
//namespace vgmodeler {

struct hull_adaptor
{
   hull_adaptor() 
   {
      nglib::Ng_Init();

      std::string filename = "hinge.msz";
      cfilename = (char *)malloc( filename.length() * sizeof(char) );
      std::strcpy(cfilename,filename.c_str());
      mp.maxh              = 100000;
      mp.fineness          = 0.5;
      mp.secondorder       = 2;
      mp.meshsize_filename = cfilename;

      geom = nglib::Ng_STL_NewGeometry();
   }
   
   ~hull_adaptor() 
   {
      free(cfilename);
      nglib::Ng_Exit();
   }
   
   template<typename TriangleT>
   void add_hull_element(TriangleT const& tri)
   {
      double p1[3];
      double p2[3];
      double p3[3];
      double n[3];   
   
//      const STLTriangle& t = geo->GetTriangle(i);
//      p = geo->GetPoint(t.PNum(1));
//      p1[0] = p.X(); p1[1] = p.Y(); p1[2] = p.Z(); 
//      p = geo->GetPoint(t.PNum(2));
//      p2[0] = p.X(); p2[1] = p.Y(); p2[2] = p.Z(); 
//      p = geo->GetPoint(t.PNum(3));
//      p3[0] = p.X(); p3[1] = p.Y(); p3[2] = p.Z();

//      normal = t.Normal();
//      n[0] = normal.X(); n[1] = normal.Y(); n[2] = normal.Z();
//      
//      Ng_STL_AddTriangle(geom, p1, p2, p3, n);
   }
   
   char                          * cfilename;   
   nglib::Ng_Mesh                * mesh;   
   nglib::Ng_STL_Geometry        * geom;
   nglib::Ng_Meshing_Parameters    mp;
};

//} // end namespace vgmodeler
   
#endif

