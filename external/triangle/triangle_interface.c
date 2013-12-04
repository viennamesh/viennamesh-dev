#include "triangle_interface.h"

int (*triangle_okay)(REAL * triorg, REAL * tridest, REAL * triapex, REAL area) = 0;

int triunsuitable(REAL * triorg, REAL * tridest, REAL * triapex, REAL area)
{
  if (triangle_okay)
    return triangle_okay(triorg, tridest, triapex, area);
  else
    return 0;
}
