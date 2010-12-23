/* ============================================================================
   Copyright (c) 2004-2009 Philipp Schwaha                      philipp@gsse.at
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_ALGORITHM_INTERPOLATION_META_HH)
#define GSSE_ALGORITHM_INTERPOLATION_META_HH

/*
   preamble: how this should work and what works

   currently it works for an n-cube with sides of length 1 
   scaling needs to be added (maybe externally) -- :: accepted :: -- ;-)

   the multivariate interpolation provided here is based on the basic linear interpolation (it is consistent with it in 1D)
   in a n-cube located at the origin (with one corner) and a the other corners at (1,0,0, ...), (0,1, ...) 
   interpolation takes on the form (3D example):

      (1-x)(1-y)(1-z) f(0,0,0) + x(1-y)(1-z) f(1,0,0) + ... xyz f(1,1,1) 

   so there are basically two kinds of coefficients 'x' and '1-x' types (modelled by simple and composite in this implementation)
   this implementation generates all the combinations (at compile time) and calculates the coefficients for a location specified (at run time)
   it also expects a container of values for f(...), it requires 2^n values for f(...) (n being the dimension)
  
*/


// *** system includes
#include <iostream>
// *** BOOST includes
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/insert_range.hpp>
#include <boost/mpl/bitand.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/for_each.hpp>
// *** GSSE includes
#include "gsse/math/power.hpp"

// =============


namespace gsse {  namespace algorithm {

// tag structs
//
struct simple {};
struct complement {};

// return the correct factors -> x or (1-x)
//
template<typename a, typename b> struct factor_selector;
template<typename numeric_type>
struct factor_selector<simple, numeric_type>
{
   numeric_type operator()(numeric_type x)
   {
      return x;
   }
};

template<typename numeric_type>
struct factor_selector<complement, numeric_type>
{
   numeric_type operator()(numeric_type x)
   {
      return numeric_type(1) - x;
   }
};

// determine the factor combination for each of the input values
//
template<long key, long position>
struct determine_factors_impl
{
   typedef boost::mpl::integral_c<unsigned, (1 << position)> bit_wrap;
   typedef boost::mpl::integral_c<unsigned, key> key_wrap;
   typedef typename boost::mpl::if_<
      typename boost::mpl::bool_<boost::mpl::bitand_<key_wrap, bit_wrap>::value >::type,
      typename boost::mpl::vector<simple>::type,
      typename boost::mpl::vector<complement>::type
      >::type summary;

   typedef typename determine_factors_impl<key, position - 1>::type sub_type;   
   typedef typename boost::mpl::insert_range<summary,
                                             typename boost::mpl::end<summary>::type,
                                             sub_type>::type type;
};

template<long key>
struct determine_factors_impl<key, 0>
{
   typedef boost::mpl::integral_c<unsigned, 1> bit_wrap;
   typedef boost::mpl::integral_c<unsigned, key> key_wrap;
   typedef typename boost::mpl::if_<
      typename boost::mpl::bool_<boost::mpl::bitand_<key_wrap, bit_wrap>::value >::type,
      typename boost::mpl::vector<simple>::type,
      typename boost::mpl::vector<complement>::type
   >::type type;
};


// determine the factors and insert them into a given container (MPL function object)
//
template<long dimension>
struct determine_factors
{
   template<typename container, typename item> struct apply
   {
      typedef typename determine_factors_impl<item::value, dimension - 1>::type i;
      typedef typename boost::mpl::insert<container, 
                                          typename boost::mpl::end<container>::type,
                                          i>::type type;
   };
};


// evaluate a factor combination
// this function object stores the positon already evaluated, so that all coordinates of a point are traversed in sequence
//
template<typename metric_point_type>
struct eval
{
   typedef typename metric_point_type::value_type numeric_type;
   const metric_point_type& value;
   long counter;
   numeric_type& factor;
   eval(metric_point_type& value, numeric_type& result) : value(value), counter(0), factor(result) {};
   template<typename coefficient_type> void operator()(coefficient_type x)
   {
      factor *= factor_selector<coefficient_type, typename metric_point_type::value_type>()(value[counter]);
      counter++;
   }
};

// traverse a structural line of coefficients and assemble the whole factor
//
template<typename list, typename metric_point_type>
struct factor_single_line_eval
{
   typedef typename metric_point_type::value_type numeric_type;
   numeric_type operator()(metric_point_type point)
   {
      numeric_type result(1);
      eval<metric_point_type> e(point, result);
      boost::mpl::for_each< list >( e ) ;
      return result;
   }
};

template<long dimension, typename numeric_type>
struct multivariate_interpolant
{
   // the dimension to be interpolated
   //
   static const long DIM = dimension;

   // number of required function values
   //
   static const unsigned long num_values = gsse::power_ct<2,DIM>::value;

   // a range to be iterated during interpolant construction
   //
   typedef typename boost::mpl::range_c<long, 0, num_values> value_range;

   // the list of factors for each of the input points
   //
   typedef typename boost::mpl::fold<
      typename value_range::type,
      boost::mpl::vector<>,
      determine_factors<DIM>
      >::type factor_list;


   // combine the structure stored in factor_list (at compile time) with the specified data set 
   //   function values are in a container
   //   the metric point is a coordination of the point of evaluation
   //
   template<typename container_type, typename metric_point_type>
   struct combiner
   {
      const container_type& container;
      const metric_point_type& point;
      numeric_type& sum;

      combiner(const container_type& container, const metric_point_type& point, numeric_type& sum) : 
         container(container), point(point), sum(sum) {} 

      template<typename point_position>
      void operator()(point_position x)
      {
         typedef typename boost::mpl::at_c<factor_list, point_position::value>::type query_line;
         factor_single_line_eval<query_line, metric_point_type> factor;
         sum += factor(point) * container[point_position::value];
      }
   };

   // this is the runtime interface
   //
   template<typename input_container_type, typename metric_point_type>
   numeric_type operator()(const input_container_type& input_data, const metric_point_type& point)
   {
      // basic check, maybe throw exception here?
      //
      if (input_data.size() != num_values)
      {
         std::cout << "wrong number of values!  input size: " << input_data.size() << " and expected values: " << num_values  << std::endl;
         
         exit (-1);
      }

      // could not reliably extract values unless providing a reference to an external variable ... 
      // I consider this inelegant, but currently do not know how to address it [PS]
      //
      numeric_type result_sum(0);

      // traverse all the points (identified by value_range) and collect the individual contributions using the combiner (see above)
      //
      boost::mpl::for_each<value_range> ( combiner<input_container_type, metric_point_type>(input_data, point, result_sum) );

      return result_sum; 
   }
};


} // namespace algorithm 
} // namespace gsse

#endif
