/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2007-2008 Franz Stimpfl              franz@gsse.at
     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */


template<typename PointT>
void calc_normal_vector(const PointT vector, PointT& norm_vector)
{
  norm_vector[0] = vector[1];
  norm_vector[1] = vector[0] * -1.0;
}



template<typename PointT>
void set_transformation(const PointT p1, const PointT p2, PointT& reference_point, PointT& vector_x, PointT& vector_y, double& length)
{
  reference_point = p1;
  
  // calculate direction vector
  //
  vector_x = p2 - p1;

  // normalize the vector
  //
  length = vec_absolute(vector_x);
  vector_x /= length;

  // calculate the normal vector
  //
  calc_normal_vector(vector_x, vector_y);
  
}


template<typename PointT>
void transform_to_plain(PointT point_global, PointT& point_plain, PointT reference_point, PointT vector_x, PointT vector_y, double h)
{
  PointT dir_vec;
  dir_vec = reference_point - point_global;
  
  dir_vec /= h;

  point_plain[0] = vec_inner_product(dir_vec, vector_x);
  point_plain[1] = vec_inner_product(dir_vec, vector_y);
  point_plain[2] = 0.0;
} 


template<typename PointT>
void transform_from_plain(PointT plain_point, PointT& e_point, PointT reference_point, PointT vector_x, PointT vector_y, double h)
{
  PointT p1p; 
  p1p = plain_point[0] * vector_x + plain_point[1] * vector_y;
  p1p *= h;
  e_point = reference_point + p1p;
  e_point[2] = 0.0;
 
}
