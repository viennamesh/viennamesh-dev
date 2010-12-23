/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */


#ifndef GSSE_GEOMETRICBARYCENTER_HH_ID
#define GSSE_GEOMETRICBARYCENTER_HH_ID 


// *** system includes
//
#include<iostream>

// *** GSSE includes
//
#include "gsse/math/geometric_point.hpp"


namespace gsse
{

template<typename GenericPointT>
GenericPointT barycenter(const GenericPointT& p1,
                         const GenericPointT& p2,
                         const GenericPointT& p3)
{
  GenericPointT center_point;


  center_point = GenericPointT( (p1[0] + p2[0] + p3[0])/3,
				(p1[1] + p2[1] + p3[1])/3,
				(p1[2] + p2[2] + p3[2])/3) ;
   

  return center_point;
}

template<typename GenericPointT>
GenericPointT barycenter(const GenericPointT& p1,
                         const GenericPointT& p2,
                         const GenericPointT& p3,
                         const GenericPointT& p4 )
{
  GenericPointT center_point;


  center_point = GenericPointT( (p1[0] + p2[0] + p3[0] + p4[0])/4,
				(p1[1] + p2[1] + p3[1] + p4[1])/4,
				(p1[2] + p2[2] + p3[2] + p4[2])/4) ;
   

  return center_point;
}


template<typename InputIterator>
typename std::iterator_traits<InputIterator>::value_type
barycenter(InputIterator begin, InputIterator end)
{
   typename std::iterator_traits<InputIterator>::value_type  resultp(*begin);
   size_t cnt =0;

   while (++begin != end)
   {
      resultp += (*begin);
//       std::cout << "result: " << resultp << std::endl;

      ++cnt;
   }
   resultp /= (cnt + 1);
   return resultp;
}


}
#endif
