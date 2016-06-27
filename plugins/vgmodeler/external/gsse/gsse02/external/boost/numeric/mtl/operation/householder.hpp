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

// With contributions from Cornelius Steinhardt

#ifndef MTL_MATRIX_HOUSEHOLDER_INCLUDE
#define MTL_MATRIX_HOUSEHOLDER_INCLUDE

#include <cmath>
#include <boost/numeric/mtl/concept/collection.hpp>


namespace mtl { namespace vector {


/// Computes Householder vector b of y 
/** such that identity_matrix(size(v))-b*v*v' projects the vector y 
    to a positive multiple of the first unit vector. **/
template <typename Vector>
std::pair<typename mtl::dense_vector<typename Collection<Vector>::value_type, parameters<> >, typename Collection<Vector>::value_type>
inline householder(Vector& y)
{
    typedef typename  Collection<Vector>::value_type   value_type;
    typedef typename  Collection<Vector>::size_type    size_type;
    size_type         n = size(y);
    const value_type  zero= math::zero(y[0]), one= math::one(y[0]);

    Vector            v(y);
    v[0]= one;

    irange            tail(1, imax); 
    value_type        s( dot(v[tail], v[tail]) ), b, v0;

    //evaluation of v and b
    if (s == zero)
        b= zero;
    else {
	value_type mu= sqrt(y[0] * y[0] + s);
	v0= v[0]= y[0] < zero ? y[0] - mu : -s / (y[0] + mu); // komplex < zero????
	b= 2 * v0 * v0 / (s + v0 * v0);                       // 2* komplex???????
	v/= v0;                                               // normalization of the first entry
    }
    return std::make_pair(v,b);
}


}} // namespace mtl::matrix

#endif // MTL_MATRIX_HOUSEHOLDER_INCLUDE

