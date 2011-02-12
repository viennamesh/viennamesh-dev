/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at
               Karl Rupp                                rupp@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#include <iostream>
#include <fstream>

namespace nglib {
#include "nglib.h"
}

int main(int argc, char * argv[])
{
  nglib::Ng_Mesh * mesh;

  nglib::Ng_Init();

  // creates mesh structure
  mesh = nglib::Ng_NewMesh ();  
  

  int i, np, nse, ne;
  double point[3];
  int trig[3], tet[4];


  // reads surface mesh from file
  std::ifstream in("../input/cube.surf");
  if(!in.is_open())
  {
    std::cerr<< "ERROR: input file could be opened .. " << std::endl;
  }

  in >> np;
  std::cout << "Reading " << np  << " points..."; std::cout.flush();
  for (i = 1; i <= np; i++)
    {
      in >> point[0] >> point[1] >> point[2];
      nglib::Ng_AddPoint (mesh, point);
    }
  std::cout << "done" << std::endl;

  in >> nse;
  std::cout << "Reading " << nse  << " faces..."; std::cout.flush();
  for (i = 1; i <= nse; i++)
    {
      in >> trig[0] >> trig[1] >> trig[2];
      nglib::Ng_AddSurfaceElement (mesh, nglib::NG_TRIG, trig);
    }
  std::cout << "done" << std::endl;


  // generate volume mesh
  nglib::Ng_Meshing_Parameters mp;
  mp.maxh = 1e6;
  mp.fineness = 1;
  mp.secondorder = 0;

  std::cout << "start meshing" << std::endl;
  nglib::Ng_GenerateVolumeMesh (mesh, &mp);
  std::cout << "meshing done" << std::endl;

  // volume mesh output
  np = nglib::Ng_GetNP(mesh);
  std::cout << "Points: " << np << std::endl;

  for (i = 1; i <= np; i++)
    {
      nglib::Ng_GetPoint (mesh, i, point);
      std::cout << i << ": " << point[0] << " " << point[1] << " " << point[2] << std::endl;
    }

  ne = nglib::Ng_GetNE(mesh);
  std::cout << "Elements: " << ne << std::endl;
  for (i = 1; i <= ne; i++)
    {
      nglib::Ng_GetVolumeElement (mesh, i, tet);
      std::cout << i << ": " << tet[0] << " " << tet[1] 
	   << " " << tet[2] << " " << tet[3] << std::endl;
    }

  nglib::Ng_SaveMesh(mesh,"test.vol");  
  
  return 0;
}
