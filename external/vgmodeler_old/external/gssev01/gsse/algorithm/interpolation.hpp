/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_INTERPOLATORH_ID
#define GSSE_INTERPOLATORH_ID

// *** system includes
//
#include<iostream>
#include<vector>

// *** GSSE includes
//
#include "gsse/exceptions.hpp"



// ==============================


static inline double det3x3(const double c1[], const double c2[], 
			    const double c3[])
{
   return    c1[0]*c2[1]*c3[2] + c2[0]*c3[1]*c1[2]
      + c3[0]*c1[1]*c2[2] - c1[0]*c3[1]*c2[2]
      - c2[0]*c1[1]*c3[2] - c3[0]*c2[1]*c1[2];
}

static inline double det2x2(const double c1[], const double c2[])
{
   return c1[0]*c2[1] - c2[0]*c1[1];
}





template<typename GenericPoint, typename Container>
std::vector<double>  computeInterpolationWeights2d(const GenericPoint&  thepoint, 
                                                   const Container&     pt_v) 
{
   if (pt_v.size() !=3)
   {
      std::cout << " Compute interpolation weights.. error.. wrong number of points: " << pt_v.size() << std::endl;
      return std::vector<double>();
   }
   double  c1[2], c2[2];

   c1[0] = pt_v[1][0] - pt_v[0][0];
   c1[1] = pt_v[1][1] - pt_v[0][1];
        
   c2[0] = pt_v[2][0] - pt_v[0][0];
   c2[1] = pt_v[2][1] - pt_v[0][1];
                
   double  det(det2x2(c1, c2));

   if (det == 0.0)
   {
      throw gsse::numerical_calculation_error(":: interpolation :: 2D :: coefficients.. determinant to small.. ");
   }
        
   double  rhs[2];
   std::vector<double> weights(3);
        
   rhs[0] = thepoint[0] - pt_v[0][0];
   rhs[1] = thepoint[1] - pt_v[0][1];

   weights[1] = det2x2(rhs, c2)  / det;
   weights[2] = det2x2(c1,  rhs) / det;
   weights[0] = 1.0 - weights[1] - weights[2];

   return weights;   // here .. copy constructor // think about it.. [RH]

}


template<typename GenericPoint, typename Container>
std::vector<double>  computeInterpolationWeights3d(const GenericPoint&  thepoint, 
                                                   const Container&     pt_v) 
{
   if (pt_v.size() !=4)
   {
      std::cout << " Compute interpolation weights.. error.. wrong number of points: " << pt_v.size() << std::endl;
      return std::vector<double>();
   }


   double	c1[3], c2[3], c3[3];

   c1[0] = pt_v[1][0] - pt_v[0][0];
   c1[1] = pt_v[1][1] - pt_v[0][1];
   c1[2] = pt_v[1][2] - pt_v[0][2];
	
   c2[0] = pt_v[2][0] - pt_v[0][0];
   c2[1] = pt_v[2][1] - pt_v[0][1];
   c2[2] = pt_v[2][2] - pt_v[0][2];
	
   c3[0] = pt_v[3][0] - pt_v[0][0];
   c3[1] = pt_v[3][1] - pt_v[0][1];
   c3[2] = pt_v[3][2] - pt_v[0][2];


   double	det(det3x3(c1, c2, c3));
	
   if (det == 0.0)
   {
      throw gsse::numerical_calculation_error(":: interpolation :: 3D :: coefficients.. determinant to small.. ");
   }
	
   double	rhs[3];
   std::vector<double> weights(4);

   rhs[0] = thepoint[0] - pt_v[0][0];
   rhs[1] = thepoint[1] - pt_v[0][1];
   rhs[2] = thepoint[2] - pt_v[0][2];
	
   weights[1] = det3x3(rhs, c2,  c3)  / det;
   weights[2] = det3x3(c1,  rhs, c3)  / det;
   weights[3] = det3x3(c1,  c2,  rhs) / det;
   weights[0] = 1.0 - weights[1] - weights[2] - weights[3];

   return weights;   // here .. copy constructor // think about it.. [RH]
}

// ====================================

template<typename NumericT, unsigned long InterpolationDimension>  
struct LinDValInterpolator
{
   static const unsigned long interpolation_dimension = InterpolationDimension;

   bool matchesLimits(const NumericT) const
      {
         return(true);
      }
   
   template<typename Container1, typename Container2>
   NumericT operator()(const Container1& weights,
                       const Container2& wps) const
      {
         double	dval(0.0);
         for (unsigned long i(0); i < InterpolationDimension; ++i)
         {
            double	dv(wps[i]);
            
            dval += weights[i] * dv;
         }
         
         return NumericT(dval);
      }

   template<template <typename> class Cont>
   NumericT average(const Cont<NumericT>& vals) const
      {
         std::cout << "lin average " << std::endl;
         if (vals.size() == 0)
            return NumericT(0.);    // Concept:: neutral element
         
         double	dval(0.0);
         for (unsigned int i(0); i < vals.size(); ++i)
         {
            double	dv(*vals[i]);
            
            dval += dv;
         }
         
         dval = dval/(double(vals.size()));
         return NumericT(dval);	
      }

};






template<typename NumericT, unsigned long InterpolationDimension>  
struct LogDValInterpolator
{
   bool matchesLimits(const NumericT val) const
      {
         double	dv(val);
         if (dv < 0.)
         {
            return(false);
         }
         return(true);
      }         

   template<typename Container1, typename Container2>
   NumericT operator()(const Container1& weights,
                       const Container2& wps) const
      {
         double	dval(0.0);
         bool	neg(false);	// sign flag
         double  posShift(0.);   // shift to reach the positive value range
   
         for (unsigned int j(0); j < InterpolationDimension; j++)
         {
            double	dv(wps[j]);
            if (dv < 0.)
               neg = true;
            posShift = std::min(posShift, dv); // minimal value
         }
         if (neg)
         {
            posShift *= -1;  // positive offset
            posShift += 10;  // move away from 0
         }
   
         for (unsigned int i(0); i < InterpolationDimension; ++i)
         {
            double	dv(wps[i]);
      
// 		if (dv == 0.0)
// 		{
// 			throw InterpolatorError("LogDNumericTInterpolator: cannot"
// 					" compute log of 0.0");
// 		}
      
            if (dv != 0.0)
            {
//  			if (dv < 0.0)
//  			{
//  				wafLog[WARNING] << "WARNING: "
//  					"LogDNumericTInterpolator: negative"
//  					" value encountered" << endl;
         
//  				neg = true;
//  			       dv = fabs(dv);
//  			}
         
               dv = log(dv + posShift);
               dval += weights[i] * dv;
            }
         }
   
         if (dval != 0.0)
         {
            dval = exp(dval);
         }
         dval -= posShift;
   
         return NumericT(dval);
      }

   template<template <typename> class Cont>
   NumericT average(const Cont<NumericT>& vals) const
      {
         if (vals.size() == 0)
            return NumericT(0.);
   
         double	dval(0.0);
         int     dcnt(0);
         for (unsigned int i(0); i < vals.size(); ++i)
         {
            double	dv((vals[i]));
            if (dv <= 0.0)
               continue;
      
            dv = log(dv);
            dval += dv;
            dcnt++;
         }
   
         if (dcnt == 0)
            return NumericT(0.);
   
         dval = exp(dval/double(dcnt));
         return NumericT(dval);	
      }

};


// [RH] generic interpolation algorithms
//
template<typename InputIterator1, typename InputIterator2>
typename std::iterator_traits<InputIterator2>::value_type
interpolate_lin(InputIterator1 it_weights_b, InputIterator1 it_weights_e,   // weights
                InputIterator2 it_quan_b,    InputIterator2 it_quan_e )     // quan values
{
   typename std::iterator_traits<InputIterator2>::value_type dval(0);          // neutral element for addition

   for (; it_weights_b != it_weights_e; ++it_weights_b, ++it_quan_b)
   {
      dval += (*it_weights_b) * (*it_quan_b);
   }
   return dval;
}


template<typename InputIterator1, typename InputIterator2>
typename std::iterator_traits<InputIterator2>::value_type
interpolate_log(InputIterator1 it_weights_b, InputIterator1 it_weights_e,      // weights
                InputIterator2 it_quan_b,    InputIterator2 it_quan_e )        // quan values
{
   typename std::iterator_traits<InputIterator2>::value_type  dval(0);         // neutral element for addition
   typename std::iterator_traits<InputIterator2>::value_type  posShift(0.);    // shift to reach the positive value range
   bool	neg(false);	// sign flag
   
   InputIterator2  it_quan_b2 = it_quan_b;

   for (; it_quan_b2 != it_quan_e; ++it_quan_b2)
   {
      if ((*it_quan_b2) < 0.)
      {
         neg = true;
         posShift = std::min(posShift, (*it_quan_b2)); // minimal value
      }
   }
   if (neg)
   {
      posShift *= -1;  // positive offset
      posShift += 10;  // move away from 0
   }

   // use the other generic interpolation algorithm
   //
   it_quan_b2 = it_quan_b;

   for (; it_weights_b != it_weights_e; ++it_weights_b, ++it_quan_b2)
   {
      if ((*it_quan_b2) != 0.0)
      {
         dval += (*it_weights_b) * std::log((*it_quan_b2)+posShift);
      }
   }
   
   if (dval != 0.0)
   {
      dval = exp(dval);
   }
   dval -= posShift;


   return dval;
}







#endif
