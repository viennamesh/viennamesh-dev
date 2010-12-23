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

#ifndef MTL_DOT_INCLUDE
#define MTL_DOT_INCLUDE

#include <boost/numeric/mtl/concept/std_concept.hpp>
#include <boost/numeric/mtl/concept/collection.hpp>
#include <boost/numeric/mtl/operation/conj.hpp>
#include <boost/numeric/meta_math/loop1.hpp>
#include <boost/numeric/linear_algebra/identity.hpp>

namespace mtl { 

    namespace vector {

	namespace detail {
	    
	    // Result type of dot product
	    template <typename Vector1, typename Vector2>
	    struct dot_result
	    {
		typedef typename Multiplicable<typename Collection<Vector1>::value_type,
					       typename Collection<Vector2>::value_type>::result_type type;
	    };

	    // Whether or not conjugating first argument
	    struct without_conj
	    {
		template <typename Value>
		Value operator()(const Value& v) { return v; }
	    };

	    struct with_conj
	    {
		template <typename Value>
		typename mtl::sfunctor::conj<Value>::result_type
		operator() (const Value& v)
		{
		    using mtl::conj;
		    return conj(v);
		}
	    };
	}

	namespace sfunctor {
	    
	    template <unsigned long Index0, unsigned long Max0>
	    struct dot_aux
		: public meta_math::loop1<Index0, Max0>
	    {
		typedef meta_math::loop1<Index0, Max0>                                    base;
		typedef dot_aux<base::next_index0, Max0>                                  next_t;
		
		template <typename Value, typename Vector1, typename Vector2, typename Size, typename ConjOpt>
		static inline void 
		apply(Value& tmp00, Value& tmp01, Value& tmp02, Value& tmp03, Value& tmp04, 
		      Value& tmp05, Value& tmp06, Value& tmp07, 
		      const Vector1& v1, const Vector2& v2, Size i, ConjOpt conj_opt)
		{
		    tmp00+= conj_opt(v1[ i + base::index0 ]) * v2[ i + base::index0 ];
		    next_t::apply(tmp01, tmp02, tmp03, tmp04, tmp05, tmp06, tmp07, tmp00,
				  v1, v2, i, conj_opt);
		}
	    };


	    template <unsigned long Max0>
	    struct dot_aux<Max0, Max0>
	    {
		typedef meta_math::loop1<Max0, Max0>                                      base;
		
		template <typename Value, typename Vector1, typename Vector2, typename Size, typename ConjOpt>
		static inline void 
		apply(Value& tmp00, Value&, Value&, Value&, Value&, Value&, Value&, Value&, 
		      const Vector1& v1, const Vector2& v2, Size i, ConjOpt conj_opt)
		{
		    tmp00+= conj_opt(v1[ i + base::index0 ]) * v2[ i + base::index0 ];
		}
	    };


	    template <unsigned long Unroll>
	    struct dot
	    {
		template <typename Vector1, typename Vector2, typename ConjOpt>
		typename detail::dot_result<Vector1, Vector2>::type
		static inline apply(const Vector1& v1, const Vector2& v2, ConjOpt conj_opt)
		{
		    MTL_THROW_IF(size(v1) != size(v2), incompatible_size());
		    
		    typedef typename Collection<Vector1>::size_type              size_type;
		    typedef typename detail::dot_result<Vector1, Vector2>::type  value_type;
		    
		    value_type dummy, z= math::zero(dummy), tmp00= z, tmp01= z, tmp02= z, tmp03= z, tmp04= z,
			       tmp05= z, tmp06= z, tmp07= z;
		    size_type  i_max= size(v1), i_block= Unroll * (i_max / Unroll);
		    
		    for (size_type i= 0; i < i_block; i+= Unroll)
			dot_aux<1, Unroll>::apply(tmp00, tmp01, tmp02, tmp03, tmp04, tmp05, tmp06, tmp07, v1, v2, i, conj_opt);
		    
		    for (size_type i= i_block; i < i_max; i++) 
			tmp00+= conj_opt(v1[i]) * v2[i];
		    return ((tmp00 + tmp01) + (tmp02 + tmp03)) + ((tmp04 + tmp05) + (tmp06 + tmp07));
		}
	    };
	}

	/// Dot product defined as hermitian(v) * w
	/** Unrolled eight times by default **/
	template <typename Vector1, typename Vector2>
	typename detail::dot_result<Vector1, Vector2>::type
	inline dot(const Vector1& v1, const Vector2& v2)
	{
	    return sfunctor::dot<8>::apply(v1, v2, detail::with_conj());
	}

	/// Dot product with user-specified unrolling defined as hermitian(v) * w
	template <unsigned long Unroll, typename Vector1, typename Vector2>
	typename detail::dot_result<Vector1, Vector2>::type
	inline dot(const Vector1& v1, const Vector2& v2)
	{
	    return sfunctor::dot<Unroll>::apply(v1, v2, detail::with_conj());
	}

	/// Dot product without conjugate defined as trans(v) * w
	/** Unrolled eight times by default **/
	template <typename Vector1, typename Vector2>
	typename detail::dot_result<Vector1, Vector2>::type
	inline dot_real(const Vector1& v1, const Vector2& v2)
	{
	    return sfunctor::dot<8>::apply(v1, v2, detail::without_conj());
	}

	/// Dot product without conjugate with user-specified unrolling defined as trans(v) * w
	template <unsigned long Unroll, typename Vector1, typename Vector2>
	typename detail::dot_result<Vector1, Vector2>::type
	inline dot_real(const Vector1& v1, const Vector2& v2)
	{
	    return sfunctor::dot<Unroll>::apply(v1, v2, detail::without_conj());
	}


    } // namespace vector
    
    using vector::dot;
    using vector::dot_real;

} // namespace mtl

#endif // MTL_DOT_INCLUDE
