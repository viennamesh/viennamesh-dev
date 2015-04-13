#ifndef _VIENNAMESH_TYPES_H_
#define _VIENNAMESH_TYPES_H_

#include "viennamesh/forwards.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                Special Data Types
//////////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct viennamesh_string_t * viennamesh_string;
DYNAMIC_EXPORT enum viennamesh_error viennamesh_string_make(viennamesh_string * string);
DYNAMIC_EXPORT enum viennamesh_error viennamesh_string_delete(viennamesh_string string);
DYNAMIC_EXPORT enum viennamesh_error viennamesh_string_set(viennamesh_string string, const char * cstr);
DYNAMIC_EXPORT enum viennamesh_error viennamesh_string_get(viennamesh_string string, const char ** cstr);


typedef struct viennamesh_point_t * viennamesh_point;
DYNAMIC_EXPORT enum viennamesh_error viennamesh_point_make(viennamesh_point * point);
DYNAMIC_EXPORT enum viennamesh_error viennamesh_point_delete(viennamesh_point point);
DYNAMIC_EXPORT enum viennamesh_error viennamesh_point_set(viennamesh_point point,
                                                     viennagrid_numeric * values,
                                                     int size);
DYNAMIC_EXPORT enum viennamesh_error viennamesh_point_get(viennamesh_point point,
                                                     viennagrid_numeric ** values,
                                                     int * size);


typedef struct viennamesh_seed_point_t * viennamesh_seed_point;
DYNAMIC_EXPORT enum viennamesh_error viennamesh_seed_point_make(viennamesh_seed_point * seed_point);
DYNAMIC_EXPORT enum viennamesh_error viennamesh_seed_point_delete(viennamesh_seed_point seed_point);
DYNAMIC_EXPORT enum viennamesh_error viennamesh_seed_point_set(viennamesh_seed_point seed_point,
                                                          viennagrid_numeric * values, int size, int region);
DYNAMIC_EXPORT enum viennamesh_error viennamesh_seed_point_get(viennamesh_seed_point seed_point,
                                                          viennagrid_numeric ** values, int * size, int * region);


#endif
