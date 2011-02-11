// Software License for MTL
// 
// Copyright (c) 2007 The Trustees of Indiana University.
//               2008 Dresden University of Technology and the Trustees of Indiana University.
// All rights reserved.
// Authors: Peter Gottschling and Andrew Lumsdaine
// 
// This file is part of the Matrix Template Library
// 
// See also license.mtl.txt in the distribution.

#ifndef MTL_SCALE_INCLUDE
#define MTL_SCALE_INCLUDE

#include <boost/numeric/mtl/mtl_fwd.hpp>
#include <boost/numeric/mtl/concept/std_concept.hpp>
#include <boost/numeric/mtl/matrix/map_view.hpp>
#include <boost/numeric/mtl/vector/map_view.hpp>
#include <boost/numeric/mtl/utility/enable_if.hpp>

namespace mtl { 

    namespace vector {
	
	template <typename Value1, typename Vector>
	typename traits::enable_if_vector<Vector, scaled_view<Value1, Vector> >::type
	inline scale(const Value1& value1, const Vector& vector)
	{
	    return scaled_view<Value1, Vector>(value1, vector);
	}
    }

    namespace matrix {

	template <typename Value1, typename Matrix>
	typename traits::enable_if_matrix<Matrix, scaled_view<Value1, Matrix> >::type
	inline scale(const Value1& value1, const Matrix& matrix)
	{
	    return scaled_view<Value1, Matrix> (value1, matrix);
	}
    }

    using vector::scale;
    using matrix::scale;


    namespace tfunctor {

	// AlgebraicCategory is by default tag::scalar
	template <typename Value1, typename Value2, typename AlgebraicCategory>
	struct scale
	{
	    typedef typename Multiplicable<Value1, Value2>::result_type result_type;	    
	    explicit scale(const Value1& v1) : v1(v1) {}
	    
	    result_type operator() (const Value2& v2) const
	    {
		return v1 * v2;
	    }
	private:
	    Value1 v1; 
	};


	template <typename Value1, typename Matrix>
	struct scale<Value1, Matrix, tag::matrix>
	{
	    typedef matrix::scaled_view<Value1, Matrix> result_type;	    
	    explicit scale(const Value1& v1) : v1(v1) {}
	
	    result_type operator() (const Matrix& matrix) const
	    {
		return result_type(v1, matrix);
	    }
	private:
	    Value1 v1; 
	};


	template <typename Value1, typename Vector>
	struct scale<Value1, Vector, tag::vector>
	{
	    typedef vector::scaled_view<Value1, Vector> result_type;
	    explicit scale(const Value1& v1) : v1(v1) {}
	
	    result_type operator() (const Vector& vector) const
	    {
		return result_type(v1, vector);
	    }
	private:
	    Value1 v1; 
	};

    } // namespace tfunctor

} // namespace mtl

#endif // MTL_SCALE_INCLUDE
