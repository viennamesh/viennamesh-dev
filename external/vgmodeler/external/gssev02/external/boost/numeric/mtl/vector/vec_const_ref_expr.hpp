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

#ifndef MTL_VECTOR_VEC_CONST_REF_EXPR_INCLUDE
#define MTL_VECTOR_VEC_CONST_REF_EXPR_INCLUDE

namespace mtl { namespace vector {


/// Class for providing interface for a vector given as reference
template <typename Vector>
struct vec_const_ref_expr
    : vec_expr< vec_const_ref_expr<Vector> >
{
    typedef vec_const_ref_expr                   self;
    typedef typename Vector::size_type           size_type;
    typedef typename Vector::value_type          value_type;
    typedef value_type                           const_dereference_type ;

    vec_const_ref_expr(const Vector& ref) : ref(ref) {}

    void delay_assign() const {}

    size_type size() const
    {
	return ref.size();
    }

    const_dereference_type operator() ( size_type i ) const
    {
        return ref(i);
    }

    const_dereference_type operator[] ( size_type i ) const
    {
        return ref[i];
    }

    

private:
    const Vector& ref;
};


}} // namespace mtl::vector

#endif // MTL_VECTOR_VEC_CONST_REF_EXPR_INCLUDE
