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

#ifndef MTL_REDUCTION_INCLUDE
#define MTL_REDUCTION_INCLUDE

#include <boost/static_assert.hpp>
#include <boost/numeric/meta_math/loop1.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/utility/category.hpp>
#include <boost/numeric/mtl/utility/range_generator.hpp>

namespace mtl { namespace vector {


    namespace impl {
	
	template <unsigned long Index0, unsigned long Max0, typename Functor>
	struct reduction
	{
	    typedef reduction<Index0+1, Max0, Functor>     next;

	    template <typename Value>
	    static inline void init(Value& tmp00, Value& tmp01, Value& tmp02, Value& tmp03, Value& tmp04, 
				    Value& tmp05, Value& tmp06, Value& tmp07)
	    {
		Functor::init(tmp00);
		next::init(tmp01, tmp02, tmp03, tmp04, tmp05, tmp06, tmp07, tmp00);
	    }

	    template <typename Value, typename Vector, typename Size>
	    static inline void update(Value& tmp00, Value& tmp01, Value& tmp02, Value& tmp03, Value& tmp04, 
				      Value& tmp05, Value& tmp06, Value& tmp07, const Vector& v, Size i)
	    {
		Functor::update(tmp00, v[ i + Index0-1 ]);
		next::update(tmp01, tmp02, tmp03, tmp04, tmp05, tmp06, tmp07, tmp00, v, i);
	    }

	    template <typename Value>
	    static inline void finish(Value& tmp00, Value& tmp01, Value& tmp02, Value& tmp03, Value& tmp04, 
				    Value& tmp05, Value& tmp06, Value& tmp07)
	    {
		next::finish(tmp01, tmp02, tmp03, tmp04, tmp05, tmp06, tmp07, tmp00);
		Functor::finish(tmp00, tmp01);
	    }
	};


	template <unsigned long Max0, typename Functor>
	struct reduction<Max0, Max0, Functor>
	{
	    template <typename Value>
	    static inline void init(Value& tmp00, Value&, Value&, Value&, Value&, Value&, Value&, Value&)
	    {
		Functor::init(tmp00);
	    }

	    template <typename Value, typename Vector, typename Size>
	    static inline void update(Value& tmp00, Value&, Value&, Value&, Value&, Value&, Value&, Value&, 
				      const Vector& v, Size i)
	    {
		Functor::update(tmp00, v[ i + Max0-1 ]);
	    }

	    template <typename Value>
	    static inline void finish(Value& tmp00, Value&, Value&, Value&, Value&, Value&, Value&, Value&) {}

	};

    } // namespace impl


// Will need distinction between dense and sparse in the future
template <unsigned long Unroll, typename Functor, typename Result>
struct reduction
{
    template <typename Vector>
    Result static inline apply(const Vector& v)
    {
	return apply(v, typename traits::category<Vector>::type());
    }


private:
    template <typename Vector>
    Result static inline apply(const Vector& v, tag::sparse)
    {
	Result tmp00;
	Functor::init(tmp00);

	typename traits::const_value<Vector>::type                        value(v); 
	typedef typename traits::range_generator<tag::nz, Vector>::type   cursor_type;

	for (cursor_type cursor = begin<tag::nz>(v), cend = end<tag::nz>(v); cursor != cend; ++cursor)
	    Functor::update(tmp00, value(*cursor));
	return tmp00;
    }

    template <typename Vector>
    Result static inline apply(const Vector& v, tag::dense)
    {
	BOOST_STATIC_ASSERT((Unroll >= 1));
	BOOST_STATIC_ASSERT((Unroll <= 8)); // Might be relaxed in future versions

	Result tmp00, tmp01, tmp02, tmp03, tmp04, tmp05, tmp06, tmp07;
	impl::reduction<1, Unroll, Functor>::init(tmp00, tmp01, tmp02, tmp03, tmp04, tmp05, tmp06, tmp07);

	typedef typename Collection<Vector>::size_type              size_type;
	size_type  i_max= size(v), i_block= Unroll * (i_max / Unroll);

	for (size_type i= 0; i < i_block; i+= Unroll)
	    impl::reduction<1, Unroll, Functor>::update(tmp00, tmp01, tmp02, tmp03, 
							     tmp04, tmp05, tmp06, tmp07, v, i);

	for (size_type i= i_block; i < i_max; i++) 
	    Functor::update(tmp00, v[i]);

	impl::reduction<1, Unroll, Functor>::finish(tmp00, tmp01, tmp02, tmp03, tmp04, tmp05, tmp06, tmp07);
	return tmp00;
    } 
};








}} // namespace mtl

#endif // MTL_REDUCTION_INCLUDE
