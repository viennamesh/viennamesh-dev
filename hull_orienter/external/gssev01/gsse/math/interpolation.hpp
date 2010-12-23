/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 René Heinzl         rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha
     Copyright (c) 2005-2006 Michael Spevak
     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_INTERPOLATION_HH
#define GSSE_INTERPOLATION_HH

#include <boost/mpl/vector.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <iostream>
#include <typeinfo>
#include <vector>
#include <numeric>
#include <boost/spirit/phoenix.hpp>

#include "gsse/base/gsse_quantity.hh"
#include "gsse/base/gsse_segment.hh"


namespace gsse
{
   ////////////////////////////////////////////////////////////
   //
   //  Direct helper classes of the Interpolation:
   //
   //  The class EvalAnsatzInPoint evaluates a certain ansatz
   //  function which is only passes by a type in a point which is
   //  passes at the construction time. The value will be stored in
   //  a container. 
   //
   ////////////////////////////////////////////////////////////

   template<typename PointT, typename IteratorT>
   class eval_ansatz_in_point
   {
      typedef typename PointT::value_type numeric_t;

      PointT     point;
      IteratorT& iter;

   public:
      eval_ansatz_in_point(PointT point, IteratorT& iter) : point(point), iter(iter) {}

      template <typename AnsatzFunc>
      void operator()(AnsatzFunc) const
      {
         AnsatzFunc func;    
         *iter = func(point);  
         ++iter;
      }
   };

   ////////////////////////////////////////////////////////////////
   //
   // This function is an object generator for the
   // eval_in_ansatz_point. 
   //
   ////////////////////////////////////////////////////////////////


   template <typename PointT, typename IteratorT>
   eval_ansatz_in_point<PointT, IteratorT>
   eval_ansatz(PointT point, IteratorT& iter)
   {
      eval_ansatz_in_point<PointT, IteratorT> eval(point, iter);
      return eval;
   }

   ////////////////////////////////////////////////////////////////
   //
   //  Interpolator class:
   //
   //  functionality: The interpolator class takes the vector of
   //  ansatz function weights (got by WeightAccessor) and a
   //  mpl::vector of ansatz functions. At construction time the
   //  Weight Access calculation object is given. At execution time
   //  the cell element as well as the geometrical Point of
   //  interpolation is given.
   //
   ////////////////////////////////////////////////////////////////


   template <typename AnsatzFuncSet, typename SegmentT, typename PointT>
   class interpolator
   {
     typedef double numeric_t;
     typedef PointT point_t;  
     typedef typename segment_traits<SegmentT>::storage_type storage_type;
     typedef std::vector<numeric_t>                            vector_t;
     typedef typename segment_traits<SegmentT>::cell           cell_t;
     typedef typename segment_traits<SegmentT>::quan_key_t     quan_key_t;     
     quan_key_t key;

   public:

      template <typename TupleT>
      struct result
      {
         typedef typename segment_traits<SegmentT>::storage_type type;
      };

     interpolator(const quan_key_t& key) : key(key) {}

      template <typename TupleT>
      typename result<TupleT>::type
      eval(TupleT const& args) const
      {

         const PointT    point = args[phoenix::tuple_index<TupleT::length - 1>()];
         const cell_t    cell  = args[phoenix::tuple_index<TupleT::length - 2>()];
         SegmentT&       seg   = args[phoenix::tuple_index<TupleT::length - 3>()];

         vector_t ansatz_vector;          
         gsse_matrix<numeric_t> weight_quan;  

         //////////////////////////////////////////////////////////////
         //
         // Q_{int}(P) = \sum_{i} f_i(P) \cdot c_i
         //
         //////////////////////////////////////////////////////////////

         typedef eval_ansatz_in_point<point_t, std::back_insert_iterator<vector_t> > eval_t;
	 std::back_insert_iterator<vector_t> iter = std::back_inserter(ansatz_vector);
         eval_t eval = eval_ansatz(point, iter);

         // evaluate the ansatz function in a point and 
         boost::mpl::for_each<AnsatzFuncSet>(eval);           
         // -> vector [f_1(P), f_n(P)]                                               
         // get the vector of all ansatz funtions evaluated
         // in this point

         // load the weights into a std::vector (perhaps use iterators)
         weight_quan = seg(cell.handle(), key);               
	 //	 std::cout << "WEIGHTS: " << key << weight_quan << std::endl;

         // -> vector [c_1, c_n]


         // ### debug output
         // std::cout << weight_quan << std::endl;
         // std::for_each(ansatz_vector.begin(), ansatz_vector.end(), std::cout << phoenix::arg1 << "   ");
         // std::cout << std::endl;
         // ###


#warning HACK. This interpolation only works for scalar data types and gsse internal types. [MS]

         // For the real solution of this problem there are some things necessary:
         // 1. A data type of order n+1 where n is the order of the interpolated type
         // 2. Matrix multiplication or the respective tensor operation
         // 
         // should look like this:
         // 
         //  result = mult(weight_quan * ansatz_vector)

         // ??? this is specific for scalar types ??? [MS]


         numeric_t result = 0.0;

         // ### debug output for this error
         // std::cout << "(" << weight_quan.size_1() << ", " <<  weight_quan.size_2() << ")" <<  std::endl;
         // ###



         // ??? get a sensible error handling for this
         if (weight_quan.size_1() != 1)
         {
            // ### debug output for this error
	   //std::cout << "Error when accessing a weight quantity 1" << std::endl;
	   //std::cout << "x";
            return storage_type(1, 1, 1.0E-300);
            // ###
         }

         if (weight_quan.size_2() == 0)
         {
            // ### debug output for this error
            //std::cout << "Error when accessing a weight quantity 2" << std::endl;
	   //std::cout << "X";
            return storage_type(1, 1, 1.0E-300);
            // ###
         }

         // ??? 

         for (int i=0; i<ansatz_vector.size() && i < weight_quan.size_2(); i++)
         {
            result += ansatz_vector.at(i) * weight_quan(0, i);
         }

         // ??? end of HACK ???

         return storage_type(1, 1, result);
      }       
   };

   ///////////////////////////////////////////////////////////////////
   //
   // GSSE specific object generator abstracts the generic
   // Interpolator class from the "gory" GSSE segments uses Cells
   // and the NumericType for Quantities for specialization
   //
   //////////////////////////////////////////////////////////////////


   template <typename AnsatzFuncSet, typename SegmentT, typename PointT>
   phoenix::actor<interpolator<AnsatzFuncSet, SegmentT, PointT> >
   get_interpolator(const typename segment_traits<SegmentT>::quan_key_t& key)
   {
     return phoenix::actor<interpolator<AnsatzFuncSet, SegmentT, PointT> >
       (interpolator<AnsatzFuncSet, SegmentT, PointT>(key));
   }

   template <typename AnsatzFuncSet, typename SegmentT, typename PointT>
   struct get_interpolator_t
   {
      typedef phoenix::actor<interpolator<AnsatzFuncSet, SegmentT, PointT> > type;
   };


   //////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////


   ///////////////////////////////////////////////////////////////////
   //
   //  Interpolation class. This class obtains the ansatz function
   //  weights from the Ansatz function and the quantity values. 
   //
   ///////////////////////////////////////////////////////////////////

   template <typename AnsatzFuncSet, typename CoordFuncT, typename QuanFuncT, 
   typename Solver, typename VertexIterator, typename CellType>
   class interpolation
   {
   public:

      template<typename TupleT>
      struct internal_traits
      {
         typedef CellType                                                 cell;
         typedef VertexIterator                                           vertex_iterator;

         typedef typename phoenix::actor_result<CoordFuncT, TupleT>::type point_t;
         typedef typename phoenix::actor_result<QuanFuncT, TupleT>::type  numeric_t;
         typedef typename boost::remove_reference<numeric_t>::type        numeric_noref_t;
	 typedef typename boost::remove_cv<numeric_noref_t>::type         numeric_nocv_t;   
         typedef typename std::vector<std::vector<numeric_nocv_t> >       matrix_t;
         typedef typename std::vector<numeric_nocv_t>                     vector_t;

	//	BOOST_STATIC_ASSERT(( boost::is_same<numeric_nocv_t, double>::type ));

         typedef typename matrix_t::iterator                              matrix_iterator;
         typedef typename vector_t::iterator                              vector_iterator;

	
      };

      template<typename TupleT>
      struct result 
      {
         typedef typename phoenix::actor_result<QuanFuncT, TupleT>::type  numeric_t;
         typedef typename boost::remove_reference<numeric_t>::type        numeric_noref_t;
         typedef typename boost::remove_cv<numeric_noref_t>::type         numeric_nocv_t;
         typedef typename gsse::gsse_matrix<numeric_noref_t>              type;
      };

   public:

      interpolation(CoordFuncT coord_func, QuanFuncT quan_func) : coord_func(coord_func), quan_func(quan_func) {}

      template<typename TupleT>
      typename result<TupleT>::type
      eval(TupleT const& args) const
      {
         typedef typename internal_traits<TupleT>::cell cell;
         typedef typename internal_traits<TupleT>::point_t point_t;
         typedef typename internal_traits<TupleT>::matrix_t matrix_t;
         typedef typename internal_traits<TupleT>::vector_t vector_t;
         typedef typename internal_traits<TupleT>::matrix_iterator matrix_iterator;
         typedef typename internal_traits<TupleT>::vector_iterator vector_iterator;
         typedef typename internal_traits<TupleT>::vertex_iterator vertex_iterator;

         cell C = args[phoenix::tuple_index<TupleT::length - 1>()];

         matrix_t geo_matrix;                   // matrix of ansatzvalues in points
         vector_t geo_vector;                  
         vector_t quan_vector;                  // vector of Quantities in points

         matrix_iterator geo_iter;              // some iterators
         vector_iterator vec_iter;
         vector_iterator quan_iter;

         vertex_iterator vocit(C);
         while (vocit.valid())                                                  // iterate over the while cell
         {

            typename vertex_iterator::value_type v = *vocit;

            // get point from the vertex
            point_t point = coord_func(v);                      

            // get an ansatz function evaluation object      

            typedef eval_ansatz_in_point<point_t, std::back_insert_iterator<vector_t> > eval_t;
	    std::back_insert_iterator<vector_t> iter = std::back_inserter(geo_vector);
	    eval_t eval = eval_ansatz(point, iter);

            // evaluate the ansatz function in a point and 
            boost::mpl::for_each<AnsatzFuncSet>(eval);           
            // get the vector of all ansatz funtions evaluated
            // in this point

            // get the quantity which is stored in the point
            geo_matrix.push_back(geo_vector); 

            // store the vector in the matrix
            quan_vector.push_back(quan_func(v)); 

            // just for debugging reasons, please comment in final version ###
            //std::cout.precision(3);                                
            //for(vec_iter = geo_vector.begin(); vec_iter != geo_vector.end(); vec_iter++)
            //  std::cout << *vec_iter << "  \t";                                  
            std::for_each(geo_vector.begin(),geo_vector.end(), std::cout << phoenix::arg1 << "  ");
	    std::cout << std::endl;                                                       
            // end of debugging section, please comment in final version ###

            geo_vector.clear();
            vocit++;
         }

	 // ###
         std::for_each(quan_vector.begin(),quan_vector.end(), std::cout << phoenix::arg1 << "\n");
	 // ###

         // get a solver object
         Solver solver;                  


         // and solve the equation system
	 try
	   {
	     solver.solve(geo_matrix, quan_vector);   
	   }
	 catch(...)
	   {
	     std::cout << "interpolation caught an exception while solving the interpolant equation system" << std::endl;
	   }


         //// just for debugging reasons, please comment in final version ###
         // vector_t ansatz_vector;                // result vector of ansatz function weights
         // std::copy(solver.solution_begin(), solver.solution_end(), std::back_inserter(ansatz_vector));
         //std::cout.precision(3);                                
         //for(vec_iter = quan_vector.begin(); vec_iter != quan_vector.end(); vec_iter++)
         //  std::cout << *vec_iter << "  \t";                                  
         //std::cout << std::endl;                                           
         //// end of debugging section, please comment in final version ###


         //// just for debugging reasons, please comment in final version ###
         //std::cout.precision(3);                                
         //for(vec_iter = ansatz_vector.begin(); vec_iter != ansatz_vector.end(); vec_iter++)
         //  std::cout << *vec_iter << "  \t";                                  
         //std::cout << std::endl;                                           
         //// end of debugging section, please comment in final version ###

         // return the vector of ansatz functions

         typename result<TupleT>::type 
         my_result(1, solver.solution_size(), solver.solution_begin(), solver.solution_end());

         return my_result; 
      }

   private:
      CoordFuncT coord_func;
      QuanFuncT  quan_func;
   };

   ///////////////////////////////////////////////////////////////////
   //
   // This metafunction returns the return type if the
   // get_interpolation function. As a second type the interior
   // function object type interpolation_t is returned. These
   // metafunction is used for the get_interpolaiton function as
   // well as for type generation in the test program. 
   //
   //////////////////////////////////////////////////////////////////


   template <typename AnsatzFuncSet, typename SegmentT, typename Solver, 
   typename CoordFuncT, typename QuanFuncT>                   
   struct get_interpolation_t
   {
      typedef interpolation < 
      AnsatzFuncSet, 
      CoordFuncT, 
      QuanFuncT, 
      Solver, 
      typename segment_traits<SegmentT>::vertex_on_cell_iterator, 
      typename segment_traits<SegmentT>::cell
      >
      interpolation_t;

      typedef phoenix::actor< interpolation_t > type;
   };


   template <typename AnsatzFuncSet, typename SegmentT, typename Solver,       // essential
   typename CoordFuncT, typename QuanFuncT>                           // derived
   typename get_interpolation_t<AnsatzFuncSet, SegmentT, Solver, CoordFuncT, QuanFuncT> :: type
   get_interpolation (CoordFuncT const& coord_func, QuanFuncT const& quan_func)
   {
      return typename get_interpolation_t<AnsatzFuncSet, SegmentT, Solver, CoordFuncT, QuanFuncT>::type
      (typename get_interpolation_t<AnsatzFuncSet, SegmentT, Solver, CoordFuncT, QuanFuncT>::interpolation_t
       (coord_func, quan_func)
      );
   }

   ///////////////////////////////////////////////////////////////
   //
   //  In the following we present the ansatz functions for the
   //  interpolation mechanism. This only depends on the PointType
   //  and the numeric type of the calculation.
   //
   ///////////////////////////////////////////////////////////////

   template <typename NumericT, typename PointT>
   struct ansatz_func_constant
   {
      NumericT operator()(PointT point){return 1.0;}
   };

   template<unsigned int N, typename NumericT, typename PointT>
   struct ansatz_func_linear
   {
      NumericT operator()(PointT point) { return point[N]; }
   };

   template<unsigned int N, unsigned int M, typename NumericT, typename PointT>
   struct ansatz_func_bilinear
   {
      NumericT operator()(PointT point) {return point[N] * point[M];}
   };

}

#endif 
