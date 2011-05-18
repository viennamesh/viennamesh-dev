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

#ifndef MTL_MATRIX_UMFPACK_SOLVE_INCLUDE
#define MTL_MATRIX_UMFPACK_SOLVE_INCLUDE

#ifdef MTL_HAS_UMFPACK

#include <iostream>


#include <cassert>
#include <algorithm>
#include <boost/type_traits.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/numeric/mtl/matrix/compressed2D.hpp>
#include <boost/numeric/mtl/matrix/parameter.hpp>
#include <boost/numeric/mtl/concept/collection.hpp>
#include <boost/numeric/mtl/utility/exception.hpp>
#include <boost/numeric/mtl/utility/make_copy_or_reference.hpp>
#include <boost/numeric/mtl/operation/merge_complex_vector.hpp>
#include <boost/numeric/mtl/operation/split_complex_vector.hpp>

extern "C" {
#  include <umfpack.h>
}

namespace mtl { namespace matrix {

    namespace umfpack {

	// conversion for value_type needed if not double or complex<double> (where possible)
	template <typename Value> struct value         {};
	template<> struct value<double>                { typedef double               type; };
	template<> struct value<float>                 { typedef double               type; };
	template<> struct value<std::complex<double> > { typedef std::complex<double> type; };
	template<> struct value<std::complex<float> >  { typedef std::complex<double> type; };

	template <typename Value> struct use_long { static const bool value= sizeof(Value) > sizeof(int); };

	template <bool Larger> struct index_aux   { typedef int     type; };
	template<> struct index_aux<true>         { typedef UF_long type; };

	template <typename Value> struct index 
          : index_aux<use_long<Value>::value> {};

	template <typename Matrix, typename Value, typename Orientation> 
	struct matrix_copy {};

	// If arbitrary compressed matrix -> copy
	template <typename Value, typename Parameters, typename Orientation>
	struct matrix_copy<compressed2D<Value, Parameters>, Value, Orientation>
	{
	    typedef typename value<Value>::type                      value_type;
	    typedef compressed2D<value_type, parameters<col_major> > matrix_type;
	    typedef compressed2D<Value, Parameters>                  in_matrix_type;

	    matrix_copy(const in_matrix_type& A) : matrix(A) {}
	    matrix_type matrix;
	};

	struct error : public domain_error
	{
	    error(const char *s, int code) : domain_error(s), code(code) {}
	    int code;
	};

	inline void check(int res, const char *s)
	{
	    MTL_THROW_IF(res != UMFPACK_OK, error(s, res));
	}

	/// Class for repeated Umfpack solutions
	/** Keeps symbolic and numeric preprocessing. Numeric part can be updated. 
	    Only defined for compressed matrices. **/
	template <typename T> class solver {};

	template <typename Parameters>
	class solver<compressed2D<double, Parameters> >
	{
	    typedef double                                    value_type;
	    typedef compressed2D<value_type, Parameters>      matrix_type;
	    typedef typename matrix_type::size_type           size_type;
	    typedef typename index<size_type>::type           index_type;

	    static const bool copy_indices= sizeof(index_type) != sizeof(size_type),
		              long_indices= use_long<size_type>::value;
	    typedef boost::mpl::bool_<long_indices>           blong;
	    typedef boost::mpl::true_                         true_;
	    typedef boost::mpl::false_                        false_;

	    // typedef parameters<col_major>     Parameters;

	    void assign_pointers()
	    {
		if (copy_indices) {
		    if (Apc == 0) Apc= new index_type[n + 1]; 
		    if (my_nnz != A.nnz() && Aic) { delete[] Aic; Aic= 0; }
		    if (Aic == 0) Aic= new index_type[A.nnz()];
		    std::copy(A.address_major(), A.address_major() + n + 1, Apc);
		    std::copy(A.address_minor(), A.address_minor() + A.nnz(), Aic);
		    Ap= Apc;
		    Ai= Aic;
		} else {
		    Ap= reinterpret_cast<const index_type*>(A.address_major());
		    Ai= reinterpret_cast<const index_type*>(A.address_minor());
		}
		Ax= A.address_data();
	    }

	    void init_aux(true_)
	    {
		check(umfpack_dl_symbolic(n, n, Ap, Ai, Ax, &Symbolic, Control, Info), "Error in dl_symbolic");
		check(umfpack_dl_numeric(Ap, Ai, Ax, Symbolic, &Numeric, Control, Info), "Error in dl_numeric");
	    }
	    
	    void init_aux(false_)
	    {
		check(umfpack_di_symbolic(n, n, Ap, Ai, Ax, &Symbolic, Control, Info), "Error in di_symbolic");
		check(umfpack_di_numeric(Ap, Ai, Ax, Symbolic, &Numeric, Control, Info), "Error in di_numeric");
	    }

	    void init()
	    {
		MTL_THROW_IF(num_rows(A) != num_cols(A), matrix_not_square());
		n= num_rows(A);
		assign_pointers();
		init_aux(blong());
	    }



	  public:
	    explicit solver(const matrix_type& A) 
	      : A(A), Apc(0), Aic(0), my_nnz(0), Symbolic(0), Numeric(0) 
	    {
		// Use default setings.
		if (long_indices)
		    umfpack_dl_defaults(Control);
		else
		    umfpack_di_defaults(Control);
		init(); 
	    }

	    ~solver()
	    {
		if (long_indices) {
		    umfpack_dl_free_numeric(&Numeric);
		    umfpack_dl_free_symbolic(&Symbolic);
		} else {
		    umfpack_di_free_numeric(&Numeric);
		    umfpack_di_free_symbolic(&Symbolic);
		}
		if (Apc) delete[] Apc; 
		if (Aic) delete[] Aic;
	    }

	    void update_numeric_aux(true_)
	    {
		umfpack_dl_free_numeric(&Numeric);
		check(umfpack_dl_numeric(Ap, Ai, Ax, Symbolic, &Numeric, Control, Info), "Error in dl_numeric");
	    }
	    
	    void update_numeric_aux(false_)
	    {
		umfpack_di_free_numeric(&Numeric);
		check(umfpack_di_numeric(Ap, Ai, Ax, Symbolic, &Numeric, Control, Info), "Error in di_numeric");
	    }

	    /// Update numeric part, for matrices that kept the sparsity and changed the values
	    void update_numeric()
	    {
		assign_pointers();
		update_numeric_aux(blong());
	    }

	    /// Update symbolic and numeric part
	    void update()
	    {
		if (long_indices) {
		    umfpack_dl_free_numeric(&Numeric);
		    umfpack_dl_free_symbolic(&Symbolic);
		} else {
		    umfpack_di_free_numeric(&Numeric);
		    umfpack_di_free_symbolic(&Symbolic);
		}
		init();
	    }

	    template <typename VectorX, typename VectorB>
	    void solve_aux(int sys, VectorX& xx, const VectorB& bb, true_)
	    {
		check(umfpack_dl_solve(sys, Ap, Ai, Ax, &xx.value[0], &bb.value[0], Numeric, Control, Info), "Error in dl_numeric");
	    }

	    template <typename VectorX, typename VectorB>
	    void solve_aux(int sys, VectorX& xx, const VectorB& bb, false_)
	    {
		check(umfpack_di_solve(sys, Ap, Ai, Ax, &xx.value[0], &bb.value[0], Numeric, Control, Info), "Error in di_numeric");
	    }

	    /// Solve double system
	    template <typename VectorX, typename VectorB>
	    int operator()(VectorX& x, const VectorB& b)
	    {
		MTL_THROW_IF(num_rows(A) != size(x) || num_rows(A) != size(b), incompatible_size());
		make_in_out_copy_or_reference<dense_vector<value_type>, VectorX> xx(x);
		make_in_copy_or_reference<dense_vector<value_type>, VectorB>     bb(b);
		int sys= mtl::traits::is_row_major<Parameters>::value ? UMFPACK_At : UMFPACK_A;
		solve_aux(sys, xx, bb, blong());
		return UMFPACK_OK;
	    }

	  private:
	    const matrix_type&  A;
	    int                 n;
	    const index_type    *Ap, *Ai;
	    index_type          *Apc, *Aic;
	    size_type           my_nnz;
	    const double        *Ax;
	    double              Control[UMFPACK_CONTROL], Info[UMFPACK_INFO];
	    void                *Symbolic, *Numeric;
	};

	template <typename Parameters>
	class solver<compressed2D<std::complex<double>, Parameters> >
	{
	    typedef std::complex<double>                      value_type;
	    typedef compressed2D<value_type, Parameters>      matrix_type;
	    typedef typename matrix_type::size_type           size_type;
	    typedef typename index<size_type>::type           index_type;

	    static const bool copy_indices= sizeof(index_type) != sizeof(size_type),
		              long_indices= use_long<size_type>::value;

	    typedef boost::mpl::bool_<long_indices>           blong;
	    typedef boost::mpl::true_                         true_;
	    typedef boost::mpl::false_                        false_;
	    

	    void assign_pointers()
	    {
		if (copy_indices) {
		    if (Apc == 0) Apc= new index_type[n + 1]; 
		    if (Aic == 0) Aic= new index_type[A.nnz()];
		    std::copy(A.address_major(), A.address_major() + n + 1, Apc);
		    std::copy(A.address_minor(), A.address_minor() + A.nnz(), Aic);
		    Ap= Apc;
		    Ai= Aic;
		} else {
		    Ap= reinterpret_cast<const index_type*>(A.address_major());
		    Ai= reinterpret_cast<const index_type*>(A.address_minor());
		}
		split_complex_vector(A.data, Ax, Az);
	    }

	    void init_aux(true_)
	    {
		check(umfpack_zl_symbolic(n, n, Ap, Ai, &Ax[0], &Az[0], &Symbolic, Control, Info), "Error in dl_symbolic");
		check(umfpack_zl_numeric(Ap, Ai, &Ax[0], &Az[0], Symbolic, &Numeric, Control, Info), "Error in dl_numeric");
	    }
	    
	    void init_aux(false_)
	    {
		check(umfpack_zi_symbolic(n, n, Ap, Ai, &Ax[0], &Az[0], &Symbolic, Control, Info), "Error in di_symbolic");
		check(umfpack_zi_numeric(Ap, Ai, &Ax[0], &Az[0], Symbolic, &Numeric, Control, Info), "Error in di_numeric");
	    }

	    void initialize()
	    {
		MTL_THROW_IF(num_rows(A) != num_cols(A), matrix_not_square());
		n= num_rows(A);
		assign_pointers();
		init_aux(blong());
	    }
	public:
	    explicit solver(const compressed2D<value_type, Parameters>& A) : A(A), Apc(0), Aic(0)
	    {
		// Use default setings.
		if (long_indices)
		    umfpack_zl_defaults(Control);
		else
		    umfpack_zi_defaults(Control);
		// umfpack_zi_defaults(Control);
		initialize();
	    }

	    ~solver()
	    {
		if (long_indices) {
		    umfpack_zl_free_numeric(&Numeric);
		    umfpack_zl_free_symbolic(&Symbolic);
		} else {
		    umfpack_zi_free_numeric(&Numeric);
		    umfpack_zi_free_symbolic(&Symbolic);
		}
		if (Apc) delete[] Apc; 
		if (Aic) delete[] Aic;
	    }

	    void update_numeric_aux(true_)
	    {
		umfpack_zl_free_numeric(&Numeric);
		check(umfpack_zl_numeric(Ap, Ai, &Ax[0], &Az[0], Symbolic, &Numeric, Control, Info), "Error in dl_numeric");
	    }
	    
	    void update_numeric_aux(false_)
	    {
		umfpack_zi_free_numeric(&Numeric);
		check(umfpack_zi_numeric(Ap, Ai, &Ax[0], &Az[0], Symbolic, &Numeric, Control, Info), "Error in di_numeric");
	    }

	    /// Update numeric part, for matrices that kept the sparsity and changed the values
	    void update_numeric()
	    {
		assign_pointers();
		update_numeric_aux(blong());
	    }

	    /// Update symbolic and numeric part
	    void update()
	    {
		Ax.change_dim(0); Az.change_dim(0);
		if (long_indices) {
		    umfpack_zl_free_numeric(&Numeric);
		    umfpack_zl_free_symbolic(&Symbolic);
		} else {
		    umfpack_zi_free_numeric(&Numeric);
		    umfpack_zi_free_symbolic(&Symbolic);
		}
		initialize();
	    }

	    template <typename VectorX, typename VectorB>
	    void solve_aux(int sys, VectorX& Xx, VectorX& Xz, const VectorB& Bx, const VectorB& Bz, true_)
	    {
		check(umfpack_zl_solve(sys, Ap, Ai, &Ax[0], &Az[0], &Xx[0], &Xz[0], &Bx[0], &Bz[0], Numeric, Control, Info), 
		      "Error in zi_solve");
	    }

	    template <typename VectorX, typename VectorB>
	    void solve_aux(int sys, VectorX& Xx, VectorX& Xz, const VectorB& Bx, const VectorB& Bz, false_)
	    {
		check(umfpack_zi_solve(sys, Ap, Ai, &Ax[0], &Az[0], &Xx[0], &Xz[0], &Bx[0], &Bz[0], Numeric, Control, Info), 
		      "Error in zi_solve");
	    }

	    /// Solve complex system
	    template <typename VectorX, typename VectorB>
	    int operator()(VectorX& x, const VectorB& b)
	    {
		MTL_THROW_IF(num_rows(A) != size(x) || num_rows(A) != size(b), incompatible_size());
		dense_vector<double> Xx(size(x)), Xz(size(x)), Bx, Bz;
		split_complex_vector(b, Bx, Bz);
		int sys= mtl::traits::is_row_major<Parameters>::value ? UMFPACK_Aat : UMFPACK_A;
		solve_aux(sys, Xx, Xz, Bx, Bz, blong());
		merge_complex_vector(Xx, Xz, x);
		return UMFPACK_OK;
	    }

	private:
	    const matrix_type&   A;
	    int                  n; 
	    const index_type     *Ap, *Ai;
	    index_type          *Apc, *Aic;
	    dense_vector<double> Ax, Az;
	    double               Control[UMFPACK_CONTROL], Info[UMFPACK_INFO];
	    void                 *Symbolic, *Numeric;
	};

	template <typename Value, typename Parameters>
	class solver<compressed2D<Value, Parameters> >
	  : matrix_copy<compressed2D<Value, Parameters>, Value, typename Parameters::orientation>,
	    public solver<typename matrix_copy<compressed2D<Value, Parameters>, Value, typename Parameters::orientation>::matrix_type >
	{
	    typedef matrix_copy<compressed2D<Value, Parameters>, Value, typename Parameters::orientation> copy_type;
	    typedef solver<typename matrix_copy<compressed2D<Value, Parameters>, Value, typename Parameters::orientation>::matrix_type > solver_type;
	public:
	    explicit solver(const compressed2D<Value, Parameters>& A) 
		: copy_type(A), solver_type(copy_type::matrix), A(A)
	    {}

	    void update()
	    {
		copy_type::matrix= A;
		solver_type::update();
	    }

	    void update_numeric()
	    {
		copy_type::matrix= A;
		solver_type::update_numeric();
	    }
	private:
	    const compressed2D<Value, Parameters>& A;
	};
    } // umfpack


template <typename Value, typename Parameters, typename VectorX, typename VectorB>
int umfpack_solve(const compressed2D<Value, Parameters>& A, VectorX& x, const VectorB& b)
{
    umfpack::solver<compressed2D<Value, Parameters> > solver(A);
    return solver(x, b);
}

}} // namespace mtl::matrix

#endif

#endif // MTL_MATRIX_UMFPACK_SOLVE_INCLUDE
