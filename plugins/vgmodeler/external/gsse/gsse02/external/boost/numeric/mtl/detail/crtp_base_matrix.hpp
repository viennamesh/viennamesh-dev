// Software License for MTL
// 
// Copyright (c) 2007 The Trustees of Indiana University. All rights reserved.
// Authors: Peter Gottschling and Andrew Lumsdaine
// 
// This file is part of the Matrix Template Library
// 
// See also license.mtl.txt in the distribution.

#ifndef MTL_CRTP_BASE_MATRIX_INCLUDE
#define MTL_CRTP_BASE_MATRIX_INCLUDE

#include <iostream>
#include <boost/utility/enable_if.hpp>
#include <boost/numeric/mtl/operation/print.hpp>

#include <boost/numeric/mtl/mtl_fwd.hpp>
#include <boost/numeric/mtl/operation/matrix_bracket.hpp>
#include <boost/numeric/mtl/operation/copy.hpp>
#include <boost/numeric/mtl/operation/mult.hpp>
#include <boost/numeric/mtl/operation/right_scale_inplace.hpp>
#include <boost/numeric/mtl/matrix/all_mat_expr.hpp>
#include <boost/numeric/mtl/matrix/diagonal_setup.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/utility/ashape.hpp>
#include <boost/numeric/mtl/operation/mult_assign_mode.hpp>
#include <boost/numeric/mtl/operation/compute_factors.hpp>

namespace mtl { namespace detail {

/// Base class to provide matrix assignment operators generically 
template <typename Matrix, typename ValueType, typename SizeType>
struct crtp_matrix_assign
{
    /// Assign scalar to a matrix by setting the matrix to a multiple of unity matrix
    /** Uses internally \sa diagonal_setup, for details see there. **/
    template <typename Value>
    typename boost::enable_if<typename boost::is_same<typename ashape::ashape<Value>::type,
						      ashape::scal>,
			      Matrix&>::type
    operator=(const Value& value)
    {
	matrix::diagonal_setup(static_cast<Matrix&>(*this), value);
	return static_cast<Matrix&>(*this);
    }

    /// Assign matrix expressions by copying except for some special expressions
    template <typename MatrixSrc>
    Matrix& operator=(const matrix::mat_expr<MatrixSrc>& src)
    {
	// std::cout << "In assignment\n";
#ifdef  _MSC_VER
		// For other compilers there is an assign operator for the same type
		// and only this one checks for self-assignment.
		// Self-assignment between different types shouldn't happen.
		if (static_cast<const void*>(this) == static_cast<const void*>(&src))
			return static_cast<Matrix&>(*this);
#endif

	matrix_copy(src.ref, static_cast<Matrix&>(*this));
	return static_cast<Matrix&>(*this);
    }

    /// Assign sum by assigning first argument and adding second
    /** Note that this is more special then assigning arbitrary expressions including matrices itself
	because matrix::mat_mat_plus_expr <E1, E2> is a derived class from matrix::mat_expr < MatrixSrc >. **/
    template <typename E1, typename E2>
    Matrix& operator=(const matrix::mat_mat_plus_expr<E1, E2>& src)
    {
	static_cast<Matrix&>(*this)= src.first;
	static_cast<Matrix&>(*this)+= src.second;

	return static_cast<Matrix&>(*this);
    }

    /// Assign difference by assigning first argument and subtracting second
    /** Note that this is more special then assigning arbitrary expressions including matrices itself
	because matrix::mat_mat_minus_expr <E1, E2> is a derived class from matrix::mat_expr < MatrixSrc >. **/
    template <typename E1, typename E2>
    Matrix& operator=(const matrix::mat_mat_minus_expr<E1, E2>& src)
    {
	static_cast<Matrix&>(*this)= src.first;
	static_cast<Matrix&>(*this)-= src.second;

	return static_cast<Matrix&>(*this);
    }

    /// Assign product by calling mult
    /** Note that this does not work for arbitrary expressions. **/
    template <typename E1, typename E2>
    Matrix& operator=(const matrix::mat_mat_times_expr<E1, E2>& src)
    {
	operation::compute_factors<Matrix, matrix::mat_mat_times_expr<E1, E2> > factors(src);
	mult(factors.first, factors.second, static_cast<Matrix&>(*this));

	return static_cast<Matrix&>(*this);
    }

    /// Assign-add matrix expressions by incrementally copying except for some special expressions
    template <typename MatrixSrc>
    Matrix& operator+=(const matrix::mat_expr<MatrixSrc>& src)
    {
	matrix_copy_plus(src.ref, static_cast<Matrix&>(*this));
	return static_cast<Matrix&>(*this);
    }

    /// Assign-add sum by adding both arguments
    /** Note that this is more special then assigning arbitrary expressions including matrices itself
	because matrix::mat_mat_plus_expr <E1, E2> is a derived class from 
	matrix::mat_expr < MatrixSrc >. **/
    template <typename E1, typename E2>
    Matrix& operator+=(const matrix::mat_mat_plus_expr<E1, E2>& src)
    {
	static_cast<Matrix&>(*this)+= src.first;
	static_cast<Matrix&>(*this)+= src.second;

	return static_cast<Matrix&>(*this);
    }

    /// Assign-add difference by adding first argument and subtracting the second one
    /** Note that this is more special then assigning arbitrary expressions including matrices itself
	because matrix::mat_mat_minus_expr <E1, E2> is a derived class from 
	matrix::mat_expr < MatrixSrc >. **/
    template <typename E1, typename E2>
    Matrix& operator+=(const matrix::mat_mat_minus_expr<E1, E2>& src)
    {
	static_cast<Matrix&>(*this)+= src.first;
	static_cast<Matrix&>(*this)-= src.second;

	return static_cast<Matrix&>(*this);
    }

    /// Assign-add product by calling gen_mult
    /** Note that this does not work for arbitrary expressions. **/
    template <typename E1, typename E2>
    Matrix& operator+=(const matrix::mat_mat_times_expr<E1, E2>& src)
    {
	operation::compute_factors<Matrix, matrix::mat_mat_times_expr<E1, E2> > factors(src);
	gen_mult(factors.first, factors.second, static_cast<Matrix&>(*this), 
		 assign::plus_sum(), tag::matrix(), tag::matrix(), tag::matrix());

	return static_cast<Matrix&>(*this);
    }

    /// Assign-subtract matrix expressions by decrementally copying except for some special expressions
    template <typename MatrixSrc>
    Matrix& operator-=(const matrix::mat_expr<MatrixSrc>& src)
    {
	matrix_copy_minus(src.ref, static_cast<Matrix&>(*this));
	return static_cast<Matrix&>(*this);
    }

    /// Assign-subtract sum by adding both arguments
    /** Note that this is more special then assigning arbitrary expressions including matrices itself
	because matrix::mat_mat_plus_expr <E1, E2> is a derived class from 
	matrix::mat_expr < MatrixSrc >. **/
    template <typename E1, typename E2>
    Matrix& operator-=(const matrix::mat_mat_plus_expr<E1, E2>& src)
    {
	static_cast<Matrix&>(*this)-= src.first;
	static_cast<Matrix&>(*this)-= src.second;

	return static_cast<Matrix&>(*this);
    }

    /// Assign-subtracting difference by subtracting first argument and adding the second one
    /** Note that this is more special then assigning arbitrary expressions including matrices itself
	because matrix::mat_mat_minus_expr <E1, E2> is a derived class from 
	matrix::mat_expr < MatrixSrc >. **/
    template <typename E1, typename E2>
    Matrix& operator-=(const matrix::mat_mat_minus_expr<E1, E2>& src)
    {
	static_cast<Matrix&>(*this)-= src.first;
	static_cast<Matrix&>(*this)+= src.second;

	return static_cast<Matrix&>(*this);
    }

    /// Assign-subtract product by calling gen_mult
    /** Note that this does not work for arbitrary expressions. **/
    template <typename E1, typename E2>
    Matrix& operator-=(const matrix::mat_mat_times_expr<E1, E2>& src)
    {
	operation::compute_factors<Matrix, matrix::mat_mat_times_expr<E1, E2> > factors(src);
	gen_mult(factors.first, factors.second, static_cast<Matrix&>(*this), 
		 assign::minus_sum(), tag::matrix(), tag::matrix(), tag::matrix());

	return static_cast<Matrix&>(*this);
    }

    /// Scale matrix (in place) with scalar value or other matrix
    template <typename Factor>
    Matrix& operator*=(const Factor& alpha)
    {
	right_scale_inplace(static_cast<Matrix&>(*this), alpha);
	return static_cast<Matrix&>(*this);
    }	
    
};



template <typename Matrix, typename ValueType, typename SizeType>
struct const_crtp_matrix_bracket
{    
    operations::bracket_proxy<Matrix, const Matrix&, ValueType>
    operator[] (SizeType row) const
    {
	return operations::bracket_proxy<Matrix, const Matrix&, ValueType>(static_cast<const Matrix&>(*this), row);
    }
};

template <typename Matrix, typename ValueType, typename SizeType>
struct crtp_matrix_bracket 
{    
    operations::bracket_proxy<Matrix, const Matrix&, const ValueType&>
    operator[] (SizeType row) const
    {
        return operations::bracket_proxy<Matrix, const Matrix&, const ValueType&>(static_cast<const Matrix&>(*this), row);
    }

    operations::bracket_proxy<Matrix, Matrix&, ValueType&>
    operator[] (SizeType row)
    {
        return operations::bracket_proxy<Matrix, Matrix&, ValueType&>(static_cast<Matrix&>(*this), row);
    }
};

template <typename Matrix, typename ValueType, typename SizeType>
struct const_crtp_base_matrix
    : public const_crtp_matrix_bracket<Matrix, ValueType, SizeType>
{};

template <typename Matrix, typename ValueType, typename SizeType>
struct crtp_base_matrix 
    : public crtp_matrix_bracket<Matrix, ValueType, SizeType>,
      public crtp_matrix_assign<Matrix, ValueType, SizeType>
{};



}} // namespace mtl::detail

#endif // MTL_CRTP_BASE_MATRIX_INCLUDE
