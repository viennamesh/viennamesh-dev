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

#ifndef VGMODELER_HULLADAPTION_HPP
#define VGMODELER_HULLADAPTION_HPP

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "interface/nglib.h"


namespace vgmodeler {


  struct hull_adaptor
  {
    typedef viennagrid::mesh_t               MeshType;

    hull_adaptor();

    ~hull_adaptor();

    double& maxsize()          { return mp.maxh; }
    double& minsize()          { return mp.minh; }
    double& grading()          { return mp.grading; }
    char*&  meshsizefilename() { return mp.meshsize_filename; }
    char*&  optimize()         { return mp.optimize2d; }
    int&    optsteps()         { return mp.optsteps2d; }
    int&    uselocalh()        { return mp.uselocalh; }
    int&    delaunay()         { return mp.delaunay; }
    int&    checkoverlap()     { return mp.checkoverlap; }
    double& curvaturesafety()  { return mp.curvaturesafety; }
    int&    threads()          { return mp.parthread; }
    double& maxangle()         { return mp.badellimit; }
    double& curvfac()          { return mp.resthsurfcurvfac; }
    int&    curvenable()       { return mp.resthsurfcurvenable; }


    void process(MeshType const & domain, MeshType const & new_domain);

    char                              * cfilename;
    vgmnetgen::Ng_Mesh                * mesh;
    vgmnetgen::Ng_STL_Geometry        * geom;
    vgmnetgen::Ng_Meshing_Parameters    mp;
  };

}

#endif


