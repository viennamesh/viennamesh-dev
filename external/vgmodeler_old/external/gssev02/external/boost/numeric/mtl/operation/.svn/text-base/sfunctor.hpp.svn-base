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

#ifndef MTL_SFUNCTOR_INCLUDE
#define MTL_SFUNCTOR_INCLUDE

#include <cmath>
#include <boost/numeric/mtl/concept/std_concept.hpp>
#include <boost/numeric/mtl/concept/magnitude.hpp>
#include <boost/numeric/mtl/concept/static_functor.hpp>

namespace mtl { namespace sfunctor {

template <typename Value1, typename Value2>
struct plus
{
    typedef const Value1&                                 first_argument_type;
    typedef const Value2&                                 second_argument_type;
    typedef typename Addable<Value1, Value2>::result_type result_type;

    static inline result_type apply(const Value1& v1, const Value2& v2)
    {
	return v1 + v2;
    }

    result_type operator() (const Value1& v1, const Value2& v2) const
    {
	return v1 + v2;
    }
};
    
template <typename Value1, typename Value2>
struct minus
{
    typedef const Value1&                                 first_argument_type;
    typedef const Value2&                                 second_argument_type;
    typedef typename Subtractable<Value1, Value2>::result_type result_type;

    static inline result_type apply(const Value1& v1, const Value2& v2)
    {
	return v1 - v2;
    }

    result_type operator() (const Value1& v1, const Value2& v2) const
    {
	return v1 - v2;
    }
};

template <typename Value1, typename Value2>
struct times
{
    typedef const Value1&                                 first_argument_type;
    typedef const Value2&                                 second_argument_type;
    typedef typename Multiplicable<Value1, Value2>::result_type result_type;

    static inline result_type apply(const Value1& v1, const Value2& v2)
    {
	return v1 * v2;
    }

    result_type operator() (const Value1& v1, const Value2& v2) const
    {
	return v1 * v2;
    }
};

template <typename Value1, typename Value2>
struct divide
{
    typedef const Value1&                                 first_argument_type;
    typedef const Value2&                                 second_argument_type;
    typedef typename Divisible<Value1, Value2>::result_type result_type;

    static inline result_type apply(const Value1& v1, const Value2& v2)
    {
	return v1 / v2;
    }

    result_type operator() (const Value1& v1, const Value2& v2) const
    {
	return v1 / v2;
    }
};

template <typename Value1, typename Value2>
struct assign
{
    typedef Value1&                                       first_argument_type;
    typedef const Value2&                                 second_argument_type;
    typedef Value1&                                       result_type;

    static inline result_type apply(Value1& v1, const Value2& v2)
    {
	return v1= v2;
    }

    result_type operator() (Value1& v1, const Value2& v2) const
    {
	return v1= v2;
    }
};
    
template <typename Value1, typename Value2>
struct plus_assign
{
    typedef Value1&                                       first_argument_type;
    typedef const Value2&                                 second_argument_type;
    typedef Value1&                                       result_type;

    static inline result_type apply(Value1& v1, const Value2& v2)
    {
	return v1+= v2;
    }

    result_type operator() (Value1& v1, const Value2& v2) const
    {
	return v1+= v2;
    }
};
    
template <typename Value1, typename Value2>
struct minus_assign
{
    typedef Value1&                                       first_argument_type;
    typedef const Value2&                                 second_argument_type;
    typedef Value1&                                       result_type;

    static inline result_type apply(Value1& v1, const Value2& v2)
    {
	return v1-= v2;
    }

    result_type operator() (Value1& v1, const Value2& v2) const
    {
	return v1-= v2;
    }
};

template <typename Value1, typename Value2>
struct times_assign
{
    typedef Value1&                                       first_argument_type;
    typedef const Value2&                                 second_argument_type;
    typedef Value1&                                       result_type;

    static inline result_type apply(Value1& v1, const Value2& v2)
    {
	return v1*= v2;
    }

    result_type operator() (Value1& v1, const Value2& v2) const
    {
	return v1*= v2;
    }
};

template <typename Value1, typename Value2>
struct divide_assign
{
    typedef Value1&                                       first_argument_type;
    typedef const Value2&                                 second_argument_type;
    typedef Value1&                                       result_type;

    static inline result_type apply(Value1& v1, const Value2& v2)
    {
	return v1/= v2;
    }

    result_type operator() (Value1& v1, const Value2& v2) const
    {
	return v1/= v2;
    }
};


// Might be helpful for surplus functor arguments
template <typename Value>
struct identity
{
    typedef const Value&                                  argument_type;
    typedef Value                                         result_type;

    static inline result_type apply(const Value& v)
    {
	return v;
    }

    result_type operator() (const Value& v) const
    {
	return v;
    }
};


template <typename Value>
struct negate
{
    typedef const Value&                                  argument_type;
    typedef Value                                         result_type;

    static inline result_type apply(const Value& v) { return -v;  }
    result_type operator() (const Value& v) const { return -v; }
};


template <typename Value>
struct abs
{
    typedef const Value&                                  argument_type;
    typedef typename Magnitude<Value>::type               result_type;

    static inline result_type apply(const Value& v)
    {						
	using std::abs;
	return abs(v);
    }

    result_type operator() (const Value& v) { return apply(v); }
};

template <typename Value>
struct sqrt
{
    typedef const Value&                                  argument_type;
    typedef Value                                         result_type;

    static inline result_type apply(const Value& v)
    {						
	using std::sqrt;
	return sqrt(v);
    }

    result_type operator() (const Value& v) { return apply(v); }
};

template <typename Value>
struct square
{
    typedef const Value&                                  argument_type;
    typedef Value                                         result_type;

    static inline result_type apply(const Value& v)
    {						
	return v * v;
    }

    result_type operator() (const Value& v) { return apply(v); }
};

/// Compose functors \p F and \p G, i.e. compute f(g(x)).
/** Functors must be models of StaticUnaryFunctor,
    StaticUnaryFunctor<G>::result_type must be convertible to
    StaticUnaryFunctor<F>::argument_type.
    Under these conditions compose<F, G> will be a model of StaticUnaryFunctor.
**/
template <typename F, typename G>
struct compose
{
    typedef typename StaticUnaryFunctor<G>::argument_type argument_type;
    typedef typename StaticUnaryFunctor<F>::result_type   result_type;
    
    static inline result_type apply(argument_type x)
    {
	return F::apply(G::apply(x));
    }

    result_type operator()(argument_type x) { return apply(x); }
};


/// Compose functors \p F and \p G with G in F's first argument, i.e. compute f(g(x), y).
/** F/G must be models of StaticBinaryFunctor/StaticUnaryFunctor,
    StaticUnaryFunctor<G>::result_type must be convertible to
    StaticBinaryFunctor<F>::first_argument_type.
    Under these conditions compose_first<F, G> will be a model of StaticBinaryFunctor.
**/
template <typename F, typename G>
struct compose_first
{
    typedef typename StaticUnaryFunctor<G>::argument_type         first_argument_type;
    typedef typename StaticBinaryFunctor<F>::second_argument_type second_argument_type;
    typedef typename StaticBinaryFunctor<F>::result_type          result_type;
    
    static inline result_type apply(first_argument_type x, second_argument_type y)
    {
	return F::apply(G::apply(x), y);
    }

    result_type operator()(first_argument_type x, second_argument_type y) { return apply(x, y); }
};


/// Compose functors \p F and \p G with G in F's second argument, i.e. compute f(x, g(y)).
/** F/G must be models of StaticBinaryFunctor/StaticUnaryFunctor,
    StaticUnaryFunctor<G>::result_type must be convertible to
    StaticBinaryFunctor<F>::second_argument_type.
    Under these conditions compose_second<F, G> will be a model of StaticBinaryFunctor.
**/
template <typename F, typename G>
struct compose_second
{
    typedef typename StaticBinaryFunctor<F>::first_argument_type  first_argument_type;
    typedef typename StaticUnaryFunctor<G>::argument_type         second_argument_type;
    typedef typename StaticBinaryFunctor<F>::result_type          result_type;
    
    static inline result_type apply(first_argument_type x, second_argument_type y)
    {
	return F::apply(x, G::apply(y));
    }

    result_type operator()(first_argument_type x, second_argument_type y) { return apply(x, y); }
};

/// Compose functors \p F, \p G, and \p H with G/H in F's first/second argument, i.e. compute f(g(x), h(y)).
/** F/G must be models of StaticBinaryFunctor/StaticUnaryFunctor,
    StaticUnaryFunctor<G>::result_type must be convertible to
    StaticBinaryFunctor<F>::first_argument_type and
    StaticUnaryFunctor<H>::result_type must be convertible to
    StaticBinaryFunctor<F>::second_argument_type.
    Under these conditions compose_both<F, G, H> will be a model of StaticBinaryFunctor.
**/
template <typename F, typename G, typename H>
struct compose_both
{
    typedef typename StaticUnaryFunctor<G>::argument_type         first_argument_type;
    typedef typename StaticUnaryFunctor<H>::argument_type         second_argument_type;
    typedef typename StaticBinaryFunctor<F>::result_type          result_type;
    
    static inline result_type apply(first_argument_type x, second_argument_type y)
    {
	return F::apply(G::apply(x), H::apply(y));
    }

    result_type operator()(first_argument_type x, second_argument_type y) { return apply(x, y); }
};

/// Compose unary functor \p F with binary functor \p G, i.e. compute f(g(x, y)).
/** F/G must be models of StaticUnaryFunctor/StaticBinaryFunctor,
    StaticBinaryFunctor<G>::result_type must be convertible to
    StaticUnaryFunctor<F>::argument_type.
    Under these conditions compose_binary<F, G> will be a model of StaticBinaryFunctor.
**/
template <typename F, typename G>
struct compose_binary
{
    typedef typename StaticBinaryFunctor<G>::first_argument_type  first_argument_type;
    typedef typename StaticBinaryFunctor<G>::second_argument_type second_argument_type;
    typedef typename StaticUnaryFunctor<F>::result_type           result_type;
    
    static inline result_type apply(first_argument_type x, second_argument_type y)
    {
	return F::apply(G::apply(x, y));
    }

    result_type operator()(first_argument_type x, second_argument_type y) { return apply(x, y); }
};


/// Templatized example of composition, computes l_2 norm in 2D, i.e. sqrt(abs(x*x + y*y))
template <typename T>
struct l_2_2D
  : public compose_binary<sqrt<typename abs<T>::result_type>, 
			  compose_binary<abs<T>, 
					 compose_both<plus<T, T>, 
						      square<T>, 
						      square<T>  > 
                                        > 
                         >
{};

}} // namespace mtl::sfunctor

#endif // MTL_SFUNCTOR_INCLUDE
