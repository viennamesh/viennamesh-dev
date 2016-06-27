#include "triangle_interface.h"
#include <stdlib.h>

int (*should_triangle_be_refined)(REAL * triorg, REAL * tridest, REAL * triapex, REAL area) = 0;

int triunsuitable(REAL * triorg, REAL * tridest, REAL * triapex, REAL area)
{
  if (should_triangle_be_refined)
    return should_triangle_be_refined(triorg, tridest, triapex, area);
  else
    return 0;
}


viennamesh_error triangle_make_mesh(triangle_mesh * mesh)
{
  *mesh = (triangle_mesh)malloc( sizeof(struct triangulateio) );

  (*mesh)->pointlist = NULL;
  (*mesh)->pointattributelist = NULL;
  (*mesh)->pointmarkerlist = NULL;
  (*mesh)->numberofpoints = 0;
  (*mesh)->numberofpointattributes = 0;

  (*mesh)->trianglelist = NULL;
  (*mesh)->triangleattributelist = NULL;
  (*mesh)->trianglearealist = NULL;
  (*mesh)->neighborlist = NULL;
  (*mesh)->numberoftriangles = 0;
  (*mesh)->numberofcorners = 0;
  (*mesh)->numberoftriangleattributes = 0;

  (*mesh)->segmentlist = NULL;
  (*mesh)->segmentmarkerlist = NULL;
  (*mesh)->numberofsegments = 0;

  (*mesh)->holelist = NULL;
  (*mesh)->numberofholes = 0;

  (*mesh)->regionlist = NULL;
  (*mesh)->numberofregions = 0;

  (*mesh)->edgelist = NULL;
  (*mesh)->edgemarkerlist = NULL;
  (*mesh)->normlist = NULL;
  (*mesh)->numberofedges = 0;

  return VIENNAMESH_SUCCESS;
}

viennamesh_error triangle_delete_mesh(triangle_mesh mesh)
{
  if (mesh->pointlist) free(mesh->pointlist);
  if (mesh->pointattributelist) free(mesh->pointattributelist);
  if (mesh->pointmarkerlist) free(mesh->pointmarkerlist);

  if (mesh->trianglelist) free(mesh->trianglelist);
  if (mesh->triangleattributelist) free(mesh->triangleattributelist);
  if (mesh->trianglearealist) free(mesh->trianglearealist);
  if (mesh->neighborlist) free(mesh->neighborlist);

  if (mesh->segmentlist) free(mesh->segmentlist);
  if (mesh->segmentmarkerlist) free(mesh->segmentmarkerlist);

  if (mesh->edgelist) free(mesh->edgelist);
  if (mesh->edgemarkerlist) free(mesh->edgemarkerlist);
  if (mesh->normlist) free(mesh->normlist);

  if (mesh->holelist) free(mesh->holelist);
  if (mesh->regionlist) free(mesh->regionlist);

  free(mesh);

  return VIENNAMESH_SUCCESS;
}

