/* =======================================================================

                     ---------------------------------------
                     ViennaMesh - The Vienna Meshing Library
                     ---------------------------------------
                            
   maintainers:    
               Josef Weinbub                      weinbub@iue.tuwien.ac.at
               Johann Cervenka                   cervenka@iue.tuwien.ac.at

   developers:
               Franz Stimpfl, Rene Heinzl, Philipp Schwaha

   license:    
               LGPL, see file LICENSE in the ViennaMesh base directory

   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at

   ======================================================================= */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

namespace nglib {
#include "libsrc/interface/nglib.h"
}

using namespace nglib;

static void printHeader()
{
  std::cout << "  " << std::endl;
  std::cout << "-------------------------------------------------------------------" << std::endl;
  std::cout << "-- ViennaMesh: VGModeler Hull Adaptor 1.0,  2010                 --" << std::endl;
  std::cout << "-------------------------------------------------------------------" << std::endl;
  std::cout << "--  developed by:                                                --" << std::endl;
  std::cout << "--    Franz Stimpfl, Rene Heinzl, Philipp Schwaha                --" << std::endl;
  std::cout << "--  maintained by:                                               --" << std::endl;
  std::cout << "--    Josef Weinbub, Johann Cervenka                             --" << std::endl;
  std::cout << "--  Institute for Microelectronics, TU Wien, Austria             --" << std::endl;
  std::cout << "--  http://www.iue.tuwien.ac.at                                  --" << std::endl;
  std::cout << "-------------------------------------------------------------------" << std::endl;
  std::cout << "  " << std::endl;
}

int main (int argc, char ** argv)
{
  printHeader();


  if (argc < 2)
    {
       std::cout << "Usage: " << argv[0] << " input_file.gau32" << std::endl;
       return 1;
    }

  std::cout << "## Starting surface meshing " << std::endl;

  int i, j, rv;

  Ng_Mesh * mesh;
  Ng_STL_Geometry * geom;


  Ng_Meshing_Parameters mp;
  mp.maxh=100000;
  mp.fineness = 0.5;
  mp.secondorder = 2;
  mp.meshsize_filename = "hinge.msz";

  Ng_Init();

  std::cout << "## Loading geometry " << std::endl;

  geom = Ng_STL_LoadGeometry (argv[1]);
  if (!geom)
    {
//       std::cout << "Ng_STL_LoadGeometry return NULL" << std::endl;
       exit(1);
    }

  std::cout << "## Starting surface meshing " << std::endl;

  rv = Ng_STL_InitSTLGeometry(geom, argv[1]);
//  std::cout << "InitSTLGeometry: NG_result=" << rv << std::endl;

  mesh = Ng_NewMesh ();


  std::cout << "## Remeshing surface ";

  Ng_FS_SurfaceMesh(geom, mesh, &mp);

  std::cout  << std::endl;
  std::cout << "## Saving surface mesh: surface_mesh.gau32" << std::endl;
  
//  rv = Ng_STL_MakeEdges (geom, mesh, &mp);
//  std::cout << "Make Edges: Ng_result=" << rv << std::endl;

//  rv = Ng_STL_GenerateSurfaceMesh (geom, mesh, &mp);
//  std::cout << "Generate Surface Mesh: Ng_result=" << rv << std::endl;

//  Ng_SaveMesh (mesh, "surface.vol");
//  
//  rv = Ng_GenerateVolumeMesh(mesh,&mp);
//  std::cout << "Generate Volume Mesh: Ng_result=" << rv << std::endl;

//  Ng_SaveMesh (mesh, "volume.vol");

  Ng_Exit();
}
