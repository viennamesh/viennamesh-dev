#include "triangle_interface.h"

int (*should_triangle_be_refined)(REAL * triorg, REAL * tridest, REAL * triapex, REAL area) = 0;

int triunsuitable(REAL * triorg, REAL * tridest, REAL * triapex, REAL area)
{
  if (should_triangle_be_refined)
    return should_triangle_be_refined(triorg, tridest, triapex, area);
  else
    return 0;
}
