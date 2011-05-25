/* ============================================================================
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GSSE_MATRIX_OPS_HH_ID
#define GSSE_MATRIX_OPS_HH_ID

// *** system includes
#include<vector> 
// *** BOOST includes
// *** GSSE includes
#include "gsse/util/size.hpp"
#include "gsse/geometry/metric_object.hpp"
#include "gsse/geometry/metric_operations.hpp"
#include "gsse/algorithm/fem/matrix.hpp"

// ############################################################

namespace gsse {  namespace math{
namespace matrix_op
{

   

template <typename MatrixT, typename ResultT, int i>
struct determinant_impl
{
  // BOOST_STATIC_ASSERT(false);
};


// determinant implementation for two dimensions
//
template <typename MatrixT, typename ResultT>
struct determinant_impl<MatrixT, ResultT, 3>
{
  ResultT operator()(MatrixT const& m)
  {
     // special case for a surface element (triangle) in 3D geometry space
     //
     if (gsse::num_rows(m) != gsse::num_cols(m))
     {
        gsse::metric_object<double, 3> v1, v2;
        gsse::metric_object<double, 3> vec_normal;

        v1[0] = m[0][0];
        v1[1] = m[1][0];
        v1[2] = m[2][0];

        v2[0] = m[0][1];
        v2[1] = m[1][1];
        v2[2] = m[2][1];

        vec_normal = gsse::ex ( v1, v2 );
        double det = gsse::norm_2 (vec_normal);  // [TODO] numericT

#ifdef GSSE_DEBUG_FULLOUTPUT
        std::cout << "v1: " << v1 << std::endl;
        std::cout << "v2: " << v2 << std::endl;
        std::cout << " vec_n: " << vec_normal << std::endl;
        std::cout << " det: " << det << std::endl;
#endif

        return det;
     }
     else
        return m(0,0) * m(1,1) * m(2,2) + m(1,0) * m(2,1) * m(0,2) + m(0,1) * m(1,2) * m(2,0) -
           (m(2,0) * m(1,1) * m(0,2) + m(0,0) * m(2,1) * m(1,2) + m(2,2) * m(1,0) * m(0,1));
  }
};

// determinant implementation for two dimensions
//
template <typename MatrixT, typename ResultT>
struct determinant_impl<MatrixT, ResultT, 2>
{
  ResultT operator()(MatrixT const& m)
  {
     // special case for a surface element (segment) in 2D geometry space
     //
     if (gsse::num_rows(m) != gsse::num_cols(m))
     {
        return sqrt ( (m(0,0)*m(0,0)) + (m(1,0)*m(1,0)));
     }
     else
        return m(0,0) * m(1,1) - m(0,1) * m(1,0);
  }
};

// determinant implementation for one dimension
//
template <typename MatrixT, typename ResultT>
struct determinant_impl<MatrixT, ResultT, 1>
{
  ResultT operator()(MatrixT const& m)
  {
#ifdef GSSE_DEBUG_FULLOUTPUT
     std::cout << "determinant M: " << m << std::endl;
#endif

     return sqrt ( (m(0,0)*m(0,0)) + (m(1,0)*m(1,0)));   // special surface treatment
//    return m(0,0);
  }
};



// Function object which encapsulates the partial specializations
// into one homogenous class

template <typename MatrixT, typename ResultT, int Size>
struct determinant_base
{
  typedef determinant_impl<MatrixT, ResultT, Size> det;
  
  ResultT operator()(MatrixT const& m)
  {
    return det()(m);
  }
};

// object generator for the determinant_base

template <typename ResultT, int Size, typename MatrixT>
ResultT determinant (MatrixT const& m)
{
  return determinant_base<MatrixT, ResultT, Size>()(m);
}


//  ################################## 

template <typename MatrixT, int i>
struct inverse_impl
{
  // BOOST_STATIC_ASSERT(false);
};


template <typename MatrixT>
struct inverse_impl<MatrixT, 3>
{
  MatrixT operator()(MatrixT const& m)
  {
     MatrixT result(3, 3);

     typedef typeof(result(0, 0)) value_t;    // [TODO]

     value_t det = determinant<typeof(result(0, 0)), 3>(m);

     result(0, 0) = (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) / det;
     result(1, 0) = (m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2)) / det;
     result(2, 0) = (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0)) / det;
     result(0, 1) = (m(0, 2) * m(2, 1) - m(0, 1) * m(2, 2)) / det;
     result(1, 1) = (m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0)) / det;
     result(2, 1) = (m(0, 1) * m(2, 0) - m(0, 0) * m(2, 1)) / det;
     result(0, 2) = (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) / det;
     result(1, 2) = (m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2)) / det;
     result(2, 2) = (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) / det;

     return result;
  }
};

template <typename MatrixT>
struct inverse_impl<MatrixT, 2>
{
   MatrixT operator()(MatrixT const& m)
      {
         MatrixT result(2, 2);

         typedef typeof(result(0, 0)) value_t;
         
         value_t det = determinant<typeof(result(0, 0)), 2>(m);
         
         result(0, 0) = (m(1, 1))  / det;
         result(1, 0) = (-m(1, 0)) / det;
         result(0, 1) = (-m(0, 1)) / det;
         result(1, 1) = (m(0, 0))  / det;

         return result;
      }
};

// object generator for the determinant_base

template <int Size, typename MatrixT>
MatrixT inverse (MatrixT const& m)
{
   return inverse_impl<MatrixT, Size>()(m);
}

template<typename MatrixT>
MatrixT transpose(const MatrixT& mat)
{
   MatrixT new_mat(gsse::num_cols(mat), gsse::num_rows(mat));

   for (unsigned int i1 = 0; i1 < gsse::num_rows(mat); ++i1)
   {
      for (unsigned int i2 = 0; i2 < gsse::num_cols(mat); ++i2)
      {
         new_mat(i2,i1) = mat(i1,i2);
      }
   }
   return new_mat;

}





template<typename Vector, typename MatrixV>
void convert_V2M(const Vector& v0, MatrixV& mx)
{
   for (long ci = 0; ci < gsse::num_rows(v0); ++ci)
   {
      mx[ci] = v0[ci];
   }
}

template<typename Geometry, typename CellT, typename Matrix>
void geom_2_mx(const Geometry& geometry,
               const CellT&    cell,
               Matrix&   mx_pc)
{
   if (geometry.size() == 0)
   {
      std::cout << " geometry does not contain any point .. " << std::endl;
      // [TODO] throw exception
      exit(-1);
   }

   long NB_pts = gsse::num_rows(mx_pc);
   long DIM    = gsse::num_cols(mx_pc);

   for (long cr = 0; cr < NB_pts; ++cr)
   {
      for (long cc = 0; cc < DIM; ++cc)   
      {
         mx_pc[cr][cc] = geometry[ cell[cr] ] [cc];
      }
   }
}










namespace compiletime
{


template <typename MatrixT, typename ResultT>
struct determinant_impl_ct
{
  ResultT operator()(MatrixT const& m)
  {
    return m(0,0) * m(1,1) * m(2,2) + m(1,0) * m(2,1) * m(0,2) + m(0,1) * m(1,2) * m(2,0) -
      (m(2,0) * m(1,1) * m(0,2) + m(0,0) * m(2,1) * m(1,2) + m(2,2) * m(1,0) * m(0,1));
  }
};

// Function object which encapsulates the partial specializations
// into one homogenous class

template <typename MatrixT, typename ResultT>
struct determinant_base
{
  typedef determinant_impl_ct<MatrixT, ResultT> det;
  
  ResultT operator()(MatrixT const& m)
  {
    return det()(m);
  }
};

// object generator for the determinant_base

template <typename ResultT, typename MatrixT>
ResultT determinant (MatrixT const& m)
{
  return determinant_base<MatrixT, ResultT>()(m);
}


  // ###########


template <typename MatrixT>
struct inverse_impl_ct
{
  //  MatrixT operator()(MatrixT const& m)
  void operator()(MatrixT const& m, MatrixT& result)
  {
    //     MatrixT result;

     typedef typeof(result(0, 0)) value_t;    // [TODO]   // internal numeric_t metafunction

     value_t det = determinant<typeof(result(0, 0))>(m);

     result(0, 0) = (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) / det;
     result(1, 0) = (m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2)) / det;
     result(2, 0) = (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0)) / det;
     result(0, 1) = (m(0, 2) * m(2, 1) - m(0, 1) * m(2, 2)) / det;
     result(1, 1) = (m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0)) / det;
     result(2, 1) = (m(0, 1) * m(2, 0) - m(0, 0) * m(2, 1)) / det;
     result(0, 2) = (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) / det;
     result(1, 2) = (m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2)) / det;
     result(2, 2) = (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) / det;

     //     return result;
  }
};


// object generator for the determinant_base

template <typename MatrixT>
void inverse (MatrixT const& m, MatrixT& inv)
{
  inverse_impl_ct<MatrixT>()(m,inv);
}





} // ////////////





} // namespace matrix_op

} // namespace math
} // namespace gsse

#endif
