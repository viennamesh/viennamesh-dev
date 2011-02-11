// Copyright 2006. Peter Gottschling, Matthias Troyer, Rolf Bonderer
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

#ifndef MATH_IDENTITY_INCLUDE
#define MATH_IDENTITY_INCLUDE

#include <boost/numeric/linear_algebra/operators.hpp>
#include <limits>
#include <string>
#include <functional>

namespace math {

template <typename Operation, typename Element>
struct identity_t {};

// TBD: Do we the case that the return type is different? Using std::unary_function?

// Additive identity of Element type is by default a converted 0
// However, for vectors one needs to know the dimension
// (and in parallel eventually also the distribution).
// Therefore, an element is passed as reference.
// It is strongly recommended to specialize this functor
// for better efficiency.
template <typename Element>
struct identity_t< add<Element>, Element > 
  : public std::binary_function<add<Element>, Element, Element>
{ 
    Element operator() (const add<Element>&, const Element& ref) const
    {
	Element tmp(ref);
	tmp= 0;
	return tmp;
    }
};

template <>
struct identity_t< add<std::string>, std::string > 
  : public std::binary_function<add<std::string>, std::string, std::string>
{ 
    std::string operator() (const add<std::string>&, const std::string&) const
    {
	return std::string();
    }
};

// Multiplicative identity of Element type is by default a converted 1
// Same comments as above.
// In contrast to additive identity, this default more likely to be wrong (e.g. matrices with all 1s)
template <typename Element>
struct identity_t< mult<Element>, Element > 
  : public std::binary_function<mult<Element>, Element, Element>
{ 
    Element operator() (const mult<Element>&, const Element& ref) const
    {
	Element tmp(ref);
	tmp= 1;
	return tmp;
    }
};


// Identity of max is minimal representable value, for standard types defined in numeric_limits
template <typename Element>
struct identity_t< max<Element>, Element > 
  : public std::binary_function<max<Element>, Element, Element>
{ 
    Element operator() (const max<Element>&, const Element& ref) const
    {
	using std::numeric_limits;
	return numeric_limits<Element>::min();
    }
};


// Identity of min is maximal representable value, for standard types defined in numeric_limits
template <typename Element>
struct identity_t< min<Element>, Element > 
  : public std::binary_function<min<Element>, Element, Element>
{ 
    Element operator() (const min<Element>&, const Element& ref) const
    {
	using std::numeric_limits;
	return numeric_limits<Element>::max();
    }
};

// Identity of bit-wise and
template <typename Element>
struct identity_t< bitwise_and<Element>, Element > 
  : public std::binary_function<bitwise_and<Element>, Element, Element>
{ 
    Element operator() (const bitwise_and<Element>&, const Element& ref) const
    {
	return 0;
    }
};

// Identity of bit-wise or
template <typename Element>
struct identity_t< bitwise_or<Element>, Element > 
  : public std::binary_function<bitwise_or<Element>, Element, Element>
{ 
    Element operator() (const bitwise_or<Element>&, const Element& ref) const
    {
	return 0 - 1;
    }
};

// Function is shorter than typetrait-like functor
template <typename Operation, typename Element>
inline Element identity(const Operation& op, const Element& v)
{
    return identity_t<Operation, Element>() (op, v);
}


// Short-cut for additive identity
template <typename Element>
inline Element zero(const Element& v)
{
    return identity_t<math::add<Element>, Element>() (math::add<Element>(), v);
}


// Short-cut for multiplicative identity
template <typename Element>
inline Element one(const Element& v)
{
    return identity_t<math::mult<Element>, Element>() (math::mult<Element>(), v);
}


} // namespace math

#endif // MATH_IDENTITY_INCLUDE
