#ifndef TRIANGLE_INTERFACE_H
#define TRIANGLE_INTERFACE_H

// defines for triangle library
#define REAL double
#define VOID int

#ifndef ANSI_DECLARATORS
  #define ANSI_DECLARATORS
#endif

#include "triangle.h"

extern int (*should_triangle_be_refined)(REAL * triorg, REAL * tridest, REAL * triapex, REAL area);


#endif
