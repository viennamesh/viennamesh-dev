/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2008-2009 Philipp Schwaha                      philipp@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_MATH_INTEGRATION_HH)
#define GSSE_MATH_INTEGRATION_HH

// *** system includes
#include <limits>
#if (__GNUC__ == 4 && __GNUC_MINOR__ > 2)
 #include <tr1/cmath>
#endif
// *** BOOST includes
// *** GSSE includes
#include "gsse/math/legrende_table.hpp"
#include "gsse/algorithm/modification.hpp"

// =============

namespace gsse {
// M_PI is the name used in the cmath include we can reuse it [PS]
//
#ifndef M_PI
        #define M_PI 3.1415926535897932384626433832795028841971693993751
#endif
namespace math{

template<typename ContainerT>
void gauss_legendre_tr1(long     point_count,
                        ContainerT& xi, 
                        ContainerT& wi,
                        typename ContainerT::value_type start = typename ContainerT::value_type(0), 
                        typename ContainerT::value_type  stop = typename ContainerT::value_type(1))
{
   typedef typename ContainerT::value_type NumericT;

   // use numerical limits based on the used data type
   NumericT error_eps(std::numeric_limits<NumericT>::epsilon() );

   gsse::resize(point_count) (xi);
   gsse::resize(point_count) (wi);

   // ------
   NumericT mean((stop + start) / NumericT(2));
   NumericT half_length((stop - start) / NumericT(2));

#ifdef GSSE_DEBUG
   std::cout << "error eps: " << error_eps << std::endl;
   std::cout << "mean: " << mean << std::endl;
   std::cout << "half_l: " << half_length << std::endl;
#endif


#if !(__GNUC__ == 4 && __GNUC_MINOR__ > 2)
            std::cout << "### std::tr1::legendre requires >=  gcc 4.3  ## " << std::endl;
#endif

   NumericT p1, p2, p3;  // legendre polynomial values
   // pp: legendre polyinomial derivatives
   // x0: initial guess for root/zero
   // x1: newton iteration step value
   NumericT pp, x0, x1;


   // find i-th root of legrende polynomial
   //
   long cnt_half((point_count + 1) / 2);
   for (int i = 1; i <= cnt_half; i++)
   {
      // initial guess
      //
      x0 = cos ( M_PI * (i - 0.25) / (point_count + 0.5));
      
      // newton iteration
      //
      while(1)
      {
         p1 = 1;
         p2 = 0;
         for (int cj = 1; cj <= point_count; cj++)
         {
            p3 = p2;
            p2 = p1;
            
#if (__GNUC__ == 4 && __GNUC_MINOR__ > 2)
            p2 = (std::tr1::legendre(point_count-1, x0));
            p1 = (std::tr1::legendre(point_count, x0));
#endif
         }

	 // compute legendre polynomial derivative at x0
         //
         pp = point_count * (x0 * p1 - p2) / (x0*x0 - 1.);


#ifdef GSSE_DEBUG
         std::cout << "\t\t  root: " << x0 << " p1: " << p1 << "  p2: " << p2 << "  pp: " << pp << std::endl;
#endif
         x1 = x0;

         // newton step
         //
         x0 = x1-p1/pp;

	 
         if (fabs (x0 - x1) < error_eps) break;
      }

#ifdef GSSE_DEBUG
      std::cout << "  p1: " << p1 << "  p2: " << p2 << "  pp: " << pp << std::endl;
#endif      
      // coordinates
      //
      xi[i-1]           = mean - half_length * x0;
      xi[point_count-i] = mean + half_length * x0;


      // weights
      //
      wi[i-1] = wi[point_count-i] = NumericT(2) * half_length / ( (1  - x0 * x0 ) * pp * pp);
   }
  }


template<typename ContainerT>
void gauss_legendre_gcc(long     point_count,
                        ContainerT& xi, 
                        ContainerT& wi,
                        typename ContainerT::value_type start = typename ContainerT::value_type(0), 
                        typename ContainerT::value_type  stop = typename ContainerT::value_type(1))
{
   typedef typename ContainerT::value_type NumericT;

   // use numerical limits based on the used data type
   NumericT error_eps(std::numeric_limits<NumericT>::epsilon());

   gsse::resize(point_count) (xi);
   gsse::resize(point_count) (wi);

   // ------
   NumericT mean((stop + start) / NumericT(2));
   NumericT half_length((stop - start) / NumericT(2));

#ifdef GSSE_DEBUG
   std::cout << "error eps: " << error_eps << std::endl;
   std::cout << "mean: " << mean << std::endl;
   std::cout << "half_l: " << half_length << std::endl;
#endif

   NumericT p1, p2, p3;  // legendre polynomial values
   // pp: legendre polyinomial derivatives
   // x0: initial guess for root/zero
   // x1: newton iteration step value
   NumericT pp, x0, x1;


   // find i-th root of legrende polynomial
   //
   long cnt_half((point_count + 1) / 2);
   for (int i = 1; i <= cnt_half; i++)
   {
      // initial guess
      //
      x0 = cos ( M_PI * (i - 0.25) / (point_count + 0.5));
      
      // newton iteration
      //
      while(1)
      {
         p1 = 1;
         p2 = 0;
         for (int cj = 1; cj <= point_count; cj++)
         {
            p3 = p2;
            p2 = p1;

            // [RH][info]  gcc 4.4 source:: legendre 
            //  This arrangement is supposed to be better for roundoff
            //  protection, Arfken, 2nd Ed, Eq 12.17a.
            //
            p1 = 2 * x0 * p2 - p3    - ( x0 * p2 - p3 ) / cj;
            

            // (c) Joachim Schoeberl:: ngsolve
            //
            //p1 = ((2 * cj - 1) * x0 * p2 - (cj - 1) * p3) / cj;

            
            // (c) Pavel Holoborodko
            //NumericT p1_1 = x0 * p2;

            // inexakt calculation
            //
            //NumericT t3 = (cj - 1.) / (cj);
            
            // "exact" lookup
            //NumericT t3 = gsse::math::detail::legrende_tbl[cj];
            //p1 = p1_1 + t3 * (p1_1 - p3);

//            std::cout << " p1: " << p1 << "  p2: " << p2 << "  p3: " << p3 << std::endl;
         }

	 // compute legendre polynomial derivative at x0
         //
         pp = point_count * (x0 * p1 - p2) / (x0*x0 - 1.);


#ifdef GSSE_DEBUG
         std::cout << "\t\t  root: " << x0 << " p1: " << p1 << "  p2: " << p2 << "  pp: " << pp << std::endl;
#endif
         x1 = x0;

         // newton step
         //
         x0 = x1-p1/pp;

	 
         if (fabs (x0 - x1) < error_eps) break;
      }

#ifdef GSSE_DEBUG
      std::cout << "  p1: " << p1 << "  p2: " << p2 << "  pp: " << pp << std::endl;
#endif      
      // coordinates
      //
      xi[i-1]           = mean - half_length * x0;
      xi[point_count-i] = mean + half_length * x0;


      // weights
      //
      wi[i-1] = wi[point_count-i] = NumericT(2) * half_length / ( (1  - x0 * x0 ) * pp * pp);
   }
  }

// ##################################################################



template<typename ContainerT>
void gauss_legendre_js(long     point_count,
                       ContainerT& xi, 
                       ContainerT& wi,
                       typename ContainerT::value_type start = typename ContainerT::value_type(0), 
                       typename ContainerT::value_type  stop = typename ContainerT::value_type(1))
{
   typedef typename ContainerT::value_type NumericT;

   // use numerical limits based on the used data type
   NumericT error_eps(std::numeric_limits<NumericT>::epsilon() );

   gsse::resize(point_count) (xi);
   gsse::resize(point_count) (wi);

   // ------
   NumericT mean((stop + start) / NumericT(2));
   NumericT half_length((stop - start) / NumericT(2));

#ifdef GSSE_DEBUG
   std::cout << "error eps: " << error_eps << std::endl;
   std::cout << "mean: " << mean << std::endl;
   std::cout << "half_l: " << half_length << std::endl;
#endif

   NumericT p1, p2, p3;  // legendre polynomial values
   // pp: legendre polyinomial derivatives
   // x0: initial guess for root/zero
   // x1: newton iteration step value
   NumericT pp, x0, x1;


   // find i-th root of legrende polynomial
   //
   long cnt_half((point_count + 1) / 2);
   for (int i = 1; i <= cnt_half; i++)
   {
      // initial guess
      //
      x0 = cos ( M_PI * (i - 0.25) / (point_count + 0.5));
      
      // newton iteration
      //
      while(1)
      {
         p1 = 1;
         p2 = 0;
         for (int cj = 1; cj <= point_count; cj++)
         {
            p3 = p2;
            p2 = p1;

            // (c) Joachim Schoeberl:: ngsolve
            //
            p1 = ((2 * cj - 1) * x0 * p2 - (cj - 1) * p3) / cj;
//            std::cout << " p1: " << p1 << "  p2: " << p2 << "  p3: " << p3 << std::endl;
         }

	 // compute legendre polynomial derivative at x0
         //
         pp = point_count * (x0 * p1 - p2) / (x0*x0 - 1.);

#ifdef GSSE_DEBUG
         std::cout << "\t\t  root: " << x0 << " p1: " << p1 << "  p2: " << p2 << "  pp: " << pp << std::endl;
#endif         
         x1 = x0;

         // newton step
         //
         x0 = x1-p1/pp;

	 
         if (fabs (x0 - x1) < error_eps) break;
      }

#ifdef GSSE_DEBUG
      std::cout << "  p1: " << p1 << "  p2: " << p2 << "  pp: " << pp << std::endl;
#endif      
      // coordinates
      //
      xi[i-1]           = mean - half_length * x0;
      xi[point_count-i] = mean + half_length * x0;


      // weights
      //
      wi[i-1] = wi[point_count-i] = NumericT(2) * half_length / ( (1  - x0 * x0 ) * pp * pp);
   }
  }

// ##################################################################



template<typename ContainerT>
void gauss_legendre_ph_calc(long     point_count,
                            ContainerT& xi, 
                            ContainerT& wi,
                            typename ContainerT::value_type start = typename ContainerT::value_type(0), 
                            typename ContainerT::value_type  stop = typename ContainerT::value_type(1))
{
   typedef typename ContainerT::value_type NumericT;

   // use numerical limits based on the used data type
   NumericT error_eps(std::numeric_limits<NumericT>::epsilon() );

   gsse::resize(point_count) (xi);
   gsse::resize(point_count) (wi);

   // ------
   NumericT mean((stop + start) / NumericT(2));
   NumericT half_length((stop - start) / NumericT(2));

#ifdef GSSE_DEBUG
   std::cout << "error eps: " << error_eps << std::endl;
   std::cout << "mean: " << mean << std::endl;
   std::cout << "half_l: " << half_length << std::endl;
#endif

   NumericT p1, p2, p3;  // legendre polynomial values
   // pp: legendre polyinomial derivatives
   // x0: initial guess for root/zero
   // x1: newton iteration step value
   NumericT pp, x0, x1;


   // find i-th root of legrende polynomial
   //
   long cnt_half((point_count + 1) / 2);
   for (int i = 1; i <= cnt_half; i++)
   {
      // initial guess
      //
      x0 = cos ( M_PI * (i - 0.25) / (point_count + 0.5));
      
      // newton iteration
      //
      while(1)
      {
         p1 = 1;
         p2 = 0;
         for (int cj = 1; cj <= point_count; cj++)
         {
            p3 = p2;
            p2 = p1;
            
            // (c) Pavel Holoborodko
            NumericT p1_1 = x0 * p2;
            // inexakt calculation
            //
            NumericT t3 = (cj - 1.) / (cj);
            p1 = p1_1 + t3 * (p1_1 - p3);

//            std::cout << " p1: " << p1 << "  p2: " << p2 << "  p3: " << p3 << std::endl;
         }

	 // compute legendre polynomial derivative at x0
         //
         pp = point_count * (x0 * p1 - p2) / (x0*x0 - 1.);
#ifdef GSSE_DEBUG
         std::cout << "\t\t  root: " << x0 << " p1: " << p1 << "  p2: " << p2 << "  pp: " << pp << std::endl;
#endif         
         x1 = x0;

         // newton step
         //
         x0 = x1-p1/pp;

	 
         if (fabs (x0 - x1) < error_eps) break;
      }
#ifdef GSSE_DEBUG
      std::cout << "  p1: " << p1 << "  p2: " << p2 << "  pp: " << pp << std::endl;
#endif      
      // coordinates
      //
      xi[i-1]           = mean - half_length * x0;
      xi[point_count-i] = mean + half_length * x0;


      // weights
      //
      wi[i-1] = wi[point_count-i] = NumericT(2) * half_length / ( (1  - x0 * x0 ) * pp * pp);
   }
  }

// ##################################################################



template<typename ContainerT>
void gauss_legendre_ph_tbl(long     point_count,
                           ContainerT& xi, 
                           ContainerT& wi,
                           typename ContainerT::value_type start = typename ContainerT::value_type(0), 
                           typename ContainerT::value_type  stop = typename ContainerT::value_type(1))
{
   typedef typename ContainerT::value_type NumericT;

   // use numerical limits based on the used data type
   NumericT error_eps(std::numeric_limits<NumericT>::epsilon() );

   gsse::resize(point_count) (xi);
   gsse::resize(point_count) (wi);

   // ------
   NumericT mean((stop + start) / NumericT(2));
   NumericT half_length((stop - start) / NumericT(2));

#ifdef GSSE_DEBUG
   std::cout << "error eps: " << error_eps << std::endl;
   std::cout << "mean: " << mean << std::endl;
   std::cout << "half_l: " << half_length << std::endl;
#endif

   NumericT p1, p2, p3;  // legendre polynomial values
   // pp: legendre polyinomial derivatives
   // x0: initial guess for root/zero
   // x1: newton iteration step value
   NumericT pp, x0, x1;


   // find i-th root of legrende polynomial
   //
   long cnt_half((point_count + 1) / 2);
   for (int i = 1; i <= cnt_half; i++)
   {
      // initial guess
      //
      x0 = cos ( M_PI * (i - 0.25) / (point_count + 0.5));
      
      // newton iteration
      //
      while(1)
      {
         p1 = 1;
         p2 = 0;
         for (int cj = 1; cj <= point_count; cj++)
         {
            p3 = p2;
            p2 = p1;

            // (c) Pavel Holoborodko
            NumericT p1_1 = x0 * p2;
            NumericT t3 = gsse::math::detail::legrende_tbl[cj];

            p1 = p1_1 + t3 * (p1_1 - p3);
//            std::cout << " p1: " << p1 << "  p2: " << p2 << "  p3: " << p3 << std::endl;
         }

	 // compute legendre polynomial derivative at x0
         //
         pp = point_count * (x0 * p1 - p2) / (x0*x0 - 1.);

#ifdef GSSE_DEBUG
         std::cout << "\t\t  root: " << x0 << " p1: " << p1 << "  p2: " << p2 << "  pp: " << pp << std::endl;
#endif
         
         x1 = x0;

         // newton step
         //
         x0 = x1-p1/pp;

	 
         if (fabs (x0 - x1) < error_eps) break;
      }
#ifdef GSSE_DEBUG
      std::cout << "  p1: " << p1 << "  p2: " << p2 << "  pp: " << pp << std::endl;
#endif      
      // coordinates
      //
      xi[i-1]           = mean - half_length * x0;
      xi[point_count-i] = mean + half_length * x0;


      // weights
      //
      wi[i-1] = wi[point_count-i] = NumericT(2) * half_length / ( (1  - x0 * x0 ) * pp * pp);
   }
  }

// ##################################################################



template<typename ContainerT>
void gauss_legendre(long     point_count,
                    ContainerT& xi, 
                    ContainerT& wi,
                    typename ContainerT::value_type start = typename ContainerT::value_type(0), 
                    typename ContainerT::value_type  stop = typename ContainerT::value_type(1))
{
   typedef typename ContainerT::value_type NumericT;

   // use numerical limits based on the used data type
   NumericT error_eps(std::numeric_limits<NumericT>::epsilon() );

   gsse::resize(point_count) (xi);
   gsse::resize(point_count) (wi);

   // ------
   NumericT mean((stop + start) / NumericT(2));
   NumericT half_length((stop - start) / NumericT(2));

#ifdef GSSE_DEBUG
   std::cout << "error eps: " << error_eps << std::endl;
   std::cout << "mean: " << mean << std::endl;
   std::cout << "half_l: " << half_length << std::endl;
#endif

   NumericT p1, p2, p3;  // legendre polynomial values
   // pp: legendre polyinomial derivatives
   // x0: initial guess for root/zero
   // x1: newton iteration step value
   NumericT pp, x0, x1;


   // find i-th root of legrende polynomial
   //
   long cnt_half((point_count + 1) / 2);
   for (int i = 1; i <= cnt_half; i++)
   {
      // initial guess
      //
      x0 = cos ( M_PI * (i - 0.25) / (point_count + 0.5));
      
      // newton iteration
      //
      while(1)
      {
         p1 = 1;
         p2 = 0;
         for (int cj = 1; cj <= point_count; cj++)
         {
            p3 = p2;
            p2 = p1;

            // [RH][info]  gcc 4.4 source:: legendre 
            //  This arrangement is supposed to be better for roundoff
            //  protection, Arfken, 2nd Ed, Eq 12.17a.
            //
            // p1 = 2 * x0 * p2 - p3    - ( x0 * p2 - p3 ) / cj;
            

            // (c) Joachim Schoeberl:: ngsolve
            //
            //p1 = ((2 * cj - 1) * x0 * p2 - (cj - 1) * p3) / cj;

            
            // (c) Pavel Holoborodko
            NumericT p1_1 = x0 * p2;

            // inexakt calculation
            //
            NumericT t3 = (cj - 1.) / (cj);
            
            // "exact" lookup
            //NumericT t3 = gsse::math::detail::legrende_tbl[cj];

            p1 = p1_1 + t3 * (p1_1 - p3);

//            std::cout << " p1: " << p1 << "  p2: " << p2 << "  p3: " << p3 << std::endl;
         }

	 // compute legendre polynomial derivative at x0
         //
         pp = point_count * (x0 * p1 - p2) / (x0*x0 - 1.);
#ifdef GSSE_DEBUG
         std::cout << "\t\t  root: " << x0 << " p1: " << p1 << "  p2: " << p2 << "  pp: " << pp << std::endl;
#endif
         
         x1 = x0;

         // newton step
         //
         x0 = x1-p1/pp;

	 
         if (fabs (x0 - x1) < error_eps) break;
      }
#ifdef GSSE_DEBUG
      std::cout << "  p1: " << p1 << "  p2: " << p2 << "  pp: " << pp << std::endl;
#endif      
      // coordinates
      //
      xi[i-1]           = mean - half_length * x0;
      xi[point_count-i] = mean + half_length * x0;


      // weights
      //
      wi[i-1] = wi[point_count-i] = NumericT(2) * half_length / ( (1  - x0 * x0 ) * pp * pp);
   }
  }


// ####################################


template<typename NumericT, typename FunctionT, typename InterpolationT>
NumericT symmetric_integrator(const NumericT lower, 
                              const NumericT upper, 
                              long samples, 
                              FunctionT& function, 
                              const InterpolationT& interpolation)
{
   NumericT x_mean(0.5 * (upper + lower));
   NumericT x_range(0.5 * (upper - lower));
   NumericT sum(0);

   for (long j(1); j <= samples; j++ )
   {
      NumericT dx(x_range * interpolation.x[j]);
      sum  += interpolation.weight[j] * (function(x_mean + dx) + function(x_mean - dx));
   }
   return sum * x_range;
}


template<typename NumericT, typename FunctionT, typename InterpolationT>
NumericT integrator(const NumericT lower, 
                    const NumericT upper, 
                    long samples, 
                    const FunctionT& function, 
                    const InterpolationT& interpolation)
{
//   NumericT x_mean(0.5 * (upper + lower));
//   NumericT x_range(0.5 * (upper - lower));
   NumericT sum(0);

   for (long j(0); j < samples; j++ )
   {
//        std::cout << "integrator eval at: " << interpolation.xi[j] 
//                  << " in rad: " << interpolation.xi[j]  * 2 * M_PI
//                  << "\t result: " << function( interpolation.xi[j] )
//                  << "\t wi: " << interpolation.wi[j] 
//                  << std::endl;
      sum  += interpolation.wi[j] * (function( interpolation.xi[j] ));
   }

   return sum ;
}


} // namespace math
} // namespace gsse
#endif
