#ifndef TRIANGLE_INTERFACE_H
#define TRIANGLE_INTERFACE_H

/* defines for triangle library */
#define REAL double
#define VOID int

#ifndef ANSI_DECLARATORS
  #define ANSI_DECLARATORS
#endif

#include "external/triangle.h"
#include "viennamesh/viennamesh.h"

extern int (*should_triangle_be_refined)(REAL * triorg, REAL * tridest, REAL * triapex, REAL area);

typedef struct triangulateio * triangle_mesh;
viennamesh_error triangle_make_mesh(triangle_mesh * mesh);
viennamesh_error triangle_delete_mesh(triangle_mesh mesh);

#endif
