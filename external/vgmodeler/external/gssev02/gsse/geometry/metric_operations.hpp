/* ============================================================================
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GSSE_SPACE_METRICOPS_HH_ID
#define GSSE_SPACE_METRICOPS_HH_ID 

// *** system includes
#include <numeric>      /// inner product
#include <cmath>

// *** BOOST includes

// *** GSSE includes
#include "gsse/util/common.hpp"
#include "gsse/numerics/conversion.hpp"

// ============================================================================================

namespace gsse
{

template<typename MetricalSpace>
typename gsse::result_of::val<MetricalSpace>::type
length_square(const MetricalSpace& metrical_object)
{
   typedef typename gsse::result_of::val< MetricalSpace >::type        NumericT;
   typedef typename gsse::result_of::itr< MetricalSpace const >::type  ItT;

   NumericT length;
   gsse::numerical_conversion()( double(0.0), length );
  
   for (ItT it  = metrical_object.begin();
            it != metrical_object.end(); 
            ++it)
   {
      length += (*it) * (*it);
   }

   return (length);
}

template<typename MetricalSpace>
typename gsse::result_of::val<MetricalSpace>::type
length(const MetricalSpace& metrical_object)
{
   return std::sqrt ( length_square(metrical_object) );
}

template<typename MetricalSpace>
typename gsse::result_of::val<MetricalSpace>::type
norm_2(const MetricalSpace& metrical_object)
{
   return length(metrical_object);
}



template<typename MetricalSpace>
inline MetricalSpace
normalize_hp(const MetricalSpace& mo)
{
   double len = sqrt(mo[0] * mo[0] + mo[1] * mo[1] + mo[2] * mo[2]);
   MetricalSpace mp;
   mp[0] = mo[0] / len;
   mp[1] = mo[1] / len;
   mp[2] = mo[2] / len;

   return mp;
}



template<typename MetricalSpace>
MetricalSpace
normalize(const MetricalSpace& metrical_object)
{
   typedef typename gsse::result_of::val< MetricalSpace >::type        NumericT;

   NumericT local_len = length(metrical_object);
   
   MetricalSpace new_metrical_object;
      for (size_t i=0; i < metrical_object.size();i++)
      {
         new_metrical_object[i] = metrical_object[i] / local_len;
      }
   return new_metrical_object;
}

template<typename MetricalSpace>
MetricalSpace
normalize_safe(const MetricalSpace& metrical_object)
{
   typedef typename gsse::result_of::val< MetricalSpace >::type        NumericT;

   NumericT local_len = length(metrical_object);
   
   MetricalSpace new_metrical_object;
   if (  local_len != 0.0)   // [RH][TODO] use an epsilon depending on the space type
   {
      for (size_t i=0; i < metrical_object.size();i++)
      {
         new_metrical_object[i] = metrical_object[i] / local_len;
      }
   }
   return new_metrical_object;
}




template<typename MetricalSpace>
MetricalSpace ex(const MetricalSpace& metrical_object1, 
                 const MetricalSpace& metrical_object2 ) 
{
   MetricalSpace ex_point;
   ex_point[0] = metrical_object1[1] * metrical_object2[2] - metrical_object1[2] * metrical_object2[1];
   ex_point[1] = metrical_object1[2] * metrical_object2[0] - metrical_object1[0] * metrical_object2[2];
   ex_point[2] = metrical_object1[0] * metrical_object2[1] - metrical_object1[1] * metrical_object2[0];

   return ex_point;
}

template<typename MetricalSpace>
MetricalSpace ex2D(MetricalSpace& metrical_object1, MetricalSpace& metrical_object2 )
{
   // [RH] .. temp debug version with temp 3D point for ex product
   //
   typedef  metric_object<typename gsse::result_of::val<MetricalSpace>::type, 3>  temp3dpoint_t;
   temp3dpoint_t mo1, mo2, final_p;

   mo1[0] = metrical_object1[0];
   mo1[1] = metrical_object1[1];
   mo1[2] = 0.0;
   mo2[0] = metrical_object2[0];
   mo2[1] = metrical_object2[1];
   mo2[2] = 0.0;
//    std::cout << "GSSE:: EX:: mo1: "<< mo1 << std::endl;
//    std::cout << "GSSE:: EX:: mo2: "<< mo2 << std::endl;

   final_p = temp3dpoint_t(mo1[1] * mo2[2] - mo1[2] * mo2[1],
                           mo1[2] * mo2[0] - mo1[0] * mo2[2],
                           mo1[0] * mo2[1] - mo1[1] * mo2[0]);
   
   typename gsse::result_of::val<MetricalSpace>::type val1, val2;
   val1 = final_p[0];
   val2 = final_p[1];
//    std::cout << "GSSE:: EX::  final p: " << final_p << std::endl;
//    std::cout << "GSSE:: EX::  final p: " << MetricalSpace(val1, val2) << std::endl;
//    std::cout << std::endl;

   return MetricalSpace(val1, val2);
}


template<typename MetricalSpace>
typename gsse::result_of::val<MetricalSpace>::type
in(const MetricalSpace& metrical_object1, const MetricalSpace& metrical_object2 )
{
   typedef typename gsse::result_of::val<MetricalSpace>::type ValT;
   typename gsse::result_of::val<MetricalSpace>::type  ip 
      = std::inner_product(metrical_object1.begin(),
                           metrical_object1.end(), 
                           metrical_object2.begin(),ValT(0.0));
    return ip;
}

template<typename MetricalSpace1, typename MetricalSpace2>
double 
in_hp(const MetricalSpace1& mo1, const MetricalSpace2& mo2 )
{
   return mo1[0] * mo2[0] + mo1[1] * mo2[1] + mo1[2] * mo2[2];
}

template<typename MetricalSpace>
double 
in2_hp(const MetricalSpace& mo1)
{
   return mo1[0] * mo1[0] + mo1[1] * mo1[1] + mo1[2] * mo1[2];
}


}
#endif
