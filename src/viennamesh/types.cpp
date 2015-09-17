#include "viennamesh/viennamesh.h"
#include "viennagrid/viennagrid.hpp"
#include <string>



struct viennamesh_string_t
{
  std::string string;
};



viennamesh_error viennamesh_string_make(viennamesh_string * string)
{
  *string = new viennamesh_string_t;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_string_delete(viennamesh_string string)
{
  delete string;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_string_set(viennamesh_string string, const char * cstr)
{
  string->string = std::string(cstr);
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_string_get(viennamesh_string string, const char ** cstr)
{
  *cstr = string->string.c_str();
  return VIENNAMESH_SUCCESS;
}




struct viennamesh_point_t
{
  viennagrid::point point;
};

viennamesh_error viennamesh_point_make(viennamesh_point * point)
{
  *point = new viennamesh_point_t;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_point_delete(viennamesh_point point)
{
  delete point;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_point_set(viennamesh_point point, viennagrid_numeric * values, int size)
{
  point->point.resize(size);
  std::copy(values, values + size, point->point.begin());
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_point_get(viennamesh_point point, viennagrid_numeric ** values, int * size)
{
  if (point->point.empty())
    *values = NULL;
  else
    *values = &point->point[0];

  *size = point->point.size();
  return VIENNAMESH_SUCCESS;
}




struct viennamesh_seed_point_t
{
  viennagrid::point point;
  int region;
};

viennamesh_error viennamesh_seed_point_make(viennamesh_seed_point * seed_point)
{
  *seed_point = new viennamesh_seed_point_t;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_seed_point_delete(viennamesh_seed_point seed_point)
{
  delete seed_point;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_seed_point_set(viennamesh_seed_point seed_point,
                                           viennagrid_numeric * values, int size, int region)
{
  seed_point->point.resize(size);
  std::copy(values, values + size, seed_point->point.begin());
  seed_point->region = region;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_seed_point_get(viennamesh_seed_point seed_point,
                                           viennagrid_numeric ** values, int * size,  int * region)
{
  if (seed_point->point.empty())
    *values = NULL;
  else
    *values = &seed_point->point[0];

  *size = seed_point->point.size();
  *region = seed_point->region;
  return VIENNAMESH_SUCCESS;
}
