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

#ifndef MTL_ZIPPED_SORT_INCLUDE
#define MTL_ZIPPED_SORT_INCLUDE

// Designed for pointers so far and not tested for general iterators
// For internal use only

#include <iterator>

namespace mtl { namespace utility {

template <typename T, typename U> struct zip_ref;
template <typename T, typename U> struct zip_it;
template <typename T, typename U> struct zip_value;

struct less_0
{
    template <typename T, typename U>
    bool operator()(const zip_ref<T, U>& x, const zip_ref<T, U>& y) const
    {
	return x.a[x.p] < y.a[y.p];
    }
    
    template <typename T, typename U>
    bool operator()(const zip_ref<T, U>& x, const zip_value<T, U>& y) const
    {
	return x.a[x.p] < y.x;
    }
    
    template <typename T, typename U>
    bool operator()(const zip_value<T, U>& x, const zip_ref<T, U>& y) const
    {
	return x.x < y.a[y.p];
    }
};

template <typename T, typename U>
struct zip_it
{
    typedef zip_ref<T, U> ref_type;

    explicit zip_it(T* a, U* v, int p) : a(a), v(v), p(p) {}

    ref_type operator*() { return ref_type(a, v, p); }
    zip_it& operator++() { p++; return *this;}
    zip_it& operator--() { p--; return *this;}
    zip_it operator--(int) { zip_it tmp(a, v, p); p--; return tmp;}

    void check(const zip_it& other) const { assert(a == other.a); assert(v == other.v); }

    bool operator==(const zip_it& other) const { check(other); return p == other.p; }
    bool operator!=(const zip_it& other) const { check(other); return p != other.p; }
    bool operator<(const zip_it& other) const { check(other); return p < other.p; }
    int operator-(const zip_it& other) const { check(other); return p - other.p; }
    zip_it operator+(int i) const { return zip_it(a, v, p+i); }
    zip_it operator-(int i) const { return zip_it(a, v, p-i); }
    zip_it& operator=(const zip_it& other) { check(other); p= other.p; return *this; }

    T*            a;
    U*            v;
    int           p;
};


template <typename T, typename U>
struct zip_ref
{
    typedef zip_ref       self;

    zip_ref(T* a, U* v, int p) : a(a), v(v), p(p) {}

    void check(const zip_ref& other) const { assert(a == other.a); assert(v == other.v); }

    bool operator<(const zip_ref& r) const { check(r); return a[p] < r.a[r.p]; }
    zip_ref& operator=(const zip_ref& r) 
    { 
	check(r);
	if (p == r.p) 
	    return *this;
	a[p]= r.a[r.p];	v[p]= r.v[r.p];
	p= r.p; 
	return *this;
    }

    zip_ref& operator=(const zip_value<T, U>& zv) 
    { 
	a[p]= zv.x; v[p]= zv.y;
	return *this;
    }

    friend inline void swap(self x, self y)
    {
	swap(x.a[x.p], y.a[y.p]);
	swap(x.v[x.p], y.v[y.p]);
    }    

    T *a;
    U *v;
    int           p;

};

template <typename T, typename U>
struct zip_value
{
    zip_value(const zip_ref<T, U>& r) : x(r.a[r.p]), y(r.v[r.p]) {}

    T x;
    U y;
};

}} // namespace mtl::utility

namespace std {
    template <typename T, typename U>
    struct iterator_traits<mtl::utility::zip_it<T, U> >
    {
	typedef mtl::utility::zip_ref<T, U>    ref_type;
	typedef mtl::utility::zip_value<T, U>  value_type;
	typedef ref_type&             reference;
	typedef ref_type*             pointer;
	typedef int                   difference_type;
	typedef random_access_iterator_tag iterator_category;
    };
}

// usage:
// sort(zip_it<T, U>(a, v, 0), zip_it<T, U>(a, v, S), less_0());
// where a and v are pointers or arrays and S the size of both arrays

#endif // MTL_ZIPPED_SORT_INCLUDE
