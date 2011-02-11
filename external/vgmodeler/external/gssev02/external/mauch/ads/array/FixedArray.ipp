// -*- C++ -*-
// ----------------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// This software is provided "as is" and any express or implied warranties
// are disclaimed.  In no event shall the author be liable for any damages
// arising from the use of this software.
// ----------------------------------------------------------------------------

#if !defined(__FixedArray_ipp__)
#error This file is an implementation detail of the class FixedArray.
#endif

BEGIN_NAMESPACE_ADS


namespace internal {

  //
  // Math operators.
  // Here I define the dot product.  This is the same as that defined in
  // the geom library.  I duplicate the code here to avoid dependence
  // on geom.
  //
    
  template<int N, typename T>
  inline
  T
  dot(const FixedArray<N,T>& x, const FixedArray<N,T>& y) {
    T d = 0;
    for (int i = 0; i < N; ++i) {
      d += x[i] * y[i];
    }
    return d;
  }

}


//
// Constructors.
//

// Constructor.  Specify the value of the components.
template<int N, typename T> 
inline 
FixedArray<N,T>::
FixedArray(parameter_type x) {
  LOKI_STATIC_CHECK(N > 3, dimension_greater_than_3);
  fill(x);
}

// Constructor.  Specify the four components.
template<int N, typename T> 
inline 
FixedArray<N,T>::
FixedArray(parameter_type x0, parameter_type x1, 
	   parameter_type x2, parameter_type x3) {
  LOKI_STATIC_CHECK(N == 4, dimension_must_be_4);
  _data[0] = x0;
  _data[1] = x1;
  _data[2] = x2;
  _data[3] = x3;
}

// Constructor.  Specify the five components.
template<int N, typename T> 
inline 
FixedArray<N,T>::
FixedArray(parameter_type x0, parameter_type x1, 
	    parameter_type x2, parameter_type x3,
	    parameter_type x4) {
  LOKI_STATIC_CHECK(N == 5, dimension_must_be_5);
  _data[0] = x0;
  _data[1] = x1;
  _data[2] = x2;
  _data[3] = x3;
  _data[4] = x4;
}

// Constructor.  Specify the six components.
template<int N, typename T> 
inline 
FixedArray<N,T>::
FixedArray(parameter_type x0, parameter_type x1, 
	    parameter_type x2, parameter_type x3,
	    parameter_type x4, parameter_type x5) {
  LOKI_STATIC_CHECK(N == 6, dimension_must_be_6);
  _data[0] = x0;
  _data[1] = x1;
  _data[2] = x2;
  _data[3] = x3;
  _data[4] = x4;
  _data[5] = x5;
}

// Constructor.  Specify the seven components.
template<int N, typename T> 
inline 
FixedArray<N,T>::
FixedArray(parameter_type x0, parameter_type x1, 
	    parameter_type x2, parameter_type x3,
	    parameter_type x4, parameter_type x5, 
	    parameter_type x6) {
  LOKI_STATIC_CHECK(N == 7, dimension_must_be_7);
  _data[0] = x0;
  _data[1] = x1;
  _data[2] = x2;
  _data[3] = x3;
  _data[4] = x4;
  _data[5] = x5;
  _data[6] = x6;
}

// Constructor.  Specify the eight components.
template<int N, typename T> 
inline 
FixedArray<N,T>::
FixedArray(parameter_type x0, parameter_type x1, 
	    parameter_type x2, parameter_type x3,
	    parameter_type x4, parameter_type x5, 
	    parameter_type x6, parameter_type x7) {
  LOKI_STATIC_CHECK(N == 8, dimension_must_be_8);
  _data[0] = x0;
  _data[1] = x1;
  _data[2] = x2;
  _data[3] = x3;
  _data[4] = x4;
  _data[5] = x5;
  _data[6] = x6;
  _data[7] = x7;
}

//
// Assignment operators
//

template<int N, typename T> 
inline 
FixedArray<N,T>&
FixedArray<N,T>::
operator=(const FixedArray& x) {
  if (this != &x) {
    copy(x.begin(), x.end());
  }
  return *this;
}

template<int N, typename T> 
inline 
FixedArray<N,T>&
FixedArray<N,T>::
operator=(parameter_type x) {
  fill(x);
  return *this;
}

template<int N, typename T> 
template< typename T2 >
inline 
FixedArray<N,T>&
FixedArray<N,T>::
operator=(const FixedArray<N,T2>& x) {
  copy(x.begin(), x.end());
  return *this;
}


template<int N, typename T> 
template<typename T2, bool A>
inline 
FixedArray<N,T>&
FixedArray<N,T>::
operator=(const Array<1, T2, A>& x) {
  copy(x.begin(), x.end());
  return *this;
}

//
// Manipulators.
//

template<int N, typename T>
inline
void
FixedArray<N,T>::
negate() {
  iterator i = begin(), i_end = end();
  for (; i != i_end; ++i) {
    *i = -*i;
  }
}

template<int N, typename T>
inline
void
FixedArray<N,T>::
fill(parameter_type value) {
  std::fill(begin(), end(), value);
}


template<int N, typename T>
template<typename InputIterator>
inline
void
FixedArray<N,T>::
copy(InputIterator start, InputIterator finish) {
#ifdef DEBUG_FixedArray
  iterator last = std::copy(start, finish, begin());
  assert(last == end());
#else
  std::copy(start, finish, begin());
#endif
}

template<int N, typename T>
inline
void
FixedArray<N,T>::
sort() {
  std::sort(begin(), end());
}

template<int N, typename T>
template<class StrictWeakOrdering>
inline
void
FixedArray<N,T>::
sort(StrictWeakOrdering comp) {
  std::sort(begin(), end(), comp);
}

template<int N, typename T>
inline
bool
FixedArray<N,T>::
is_sorted() const {
  return ads::is_sorted(begin(), end());
}

template<int N, typename T>
template<class StrictWeakOrdering>
inline
bool
FixedArray<N,T>::
is_sorted(StrictWeakOrdering comp) const {
  return ads::is_sorted(begin(), end(), comp);
}

template<int N, typename T>
inline
int
FixedArray<N,T>::
min_index() const {
  return int(std::min_element(begin(), end()) - begin());
}

template<int N, typename T>
template<class StrictWeakOrdering>
inline
int
FixedArray<N,T>::
min_index(StrictWeakOrdering comp) const {
  return int(std::min_element(begin(), end(), comp) - begin());
}

template<int N, typename T>
inline
int
FixedArray<N,T>::
max_index() const {
  return int(std::max_element(begin(), end()) - begin());
}

template<int N, typename T>
template<class StrictWeakOrdering>
inline
int
FixedArray<N,T>::
max_index(StrictWeakOrdering comp) const {
  return int(std::max_element(begin(), end(), comp) - begin());
}


//
// Assignment operators with scalar operand
//

template<int N, typename T>
inline 
FixedArray<N,T>&
FixedArray<N,T>::
operator+=(parameter_type x) {
  iterator i = begin(), i_end = end();
  for (; i != i_end; ++i) {
    *i += x;
  }
  return *this;
}

template<int N, typename T>
inline 
FixedArray<N,T>&
FixedArray<N,T>::
operator-=(parameter_type x) {
  iterator i = begin(), i_end = end();
  for (; i != i_end; ++i) {
    *i -= x;
  }
  return *this;
}

template<int N, typename T>
inline 
FixedArray<N,T>&
FixedArray<N,T>::
operator*=(parameter_type x) {
  iterator i = begin(), i_end = end();
  for (; i != i_end; ++i) {
    *i *= x;
  }
  return *this;
}

template<int N, typename T>
inline 
FixedArray<N,T>&
FixedArray<N,T>::
operator/=(parameter_type x) {
#ifdef DEBUG_FixedArray
  assert(x != 0);
#endif
  iterator i = begin(), i_end = end();
  for (; i != i_end; ++i) {
    *i /= x;
  }
  return *this;
}

template<int N, typename T>
inline 
FixedArray<N,T>&
FixedArray<N,T>::
operator%=(parameter_type x) {
#ifdef DEBUG_FixedArray
  assert(x != 0);
#endif
  iterator i = begin(), i_end = end();
  for (; i != i_end; ++i) {
    *i %= x;
  }
  return *this;
}

template<int N, typename T>
inline 
FixedArray<N,T>&
FixedArray<N,T>::
operator<<=(const int offset) {
  for (int n = 0; n != N; ++n) {
    _data[n] <<= offset;
  }
  return *this;
}

template<int N, typename T>
inline 
FixedArray<N,T>&
FixedArray<N,T>::
operator>>=(const int offset) {
  for (int n = 0; n != N; ++n) {
    _data[n] >>= offset;
  }
  return *this;
}

//
// Assignment operators with FixedArray operand
//

template<int N, typename T> 
template< typename T2 >
inline 
FixedArray<N,T>&
FixedArray<N,T>::
operator+=(const FixedArray<N,T2>& x) {
  iterator i = begin(), i_end = end();
  typename FixedArray<N,T2>::const_iterator j = x.begin();
  for (; i != i_end; ++i, ++j) {
    *i += *j;
  }
  return *this;
}

template<int N, typename T> 
template< typename T2 >
inline 
FixedArray<N,T>&
FixedArray<N,T>::
operator-=(const FixedArray<N,T2>& x) {
  iterator i = begin(), i_end = end();
  typename FixedArray<N,T2>::const_iterator j = x.begin();
  for (; i != i_end; ++i, ++j) {
    *i -= *j;
  }
  return *this;
}

template<int N, typename T> 
template< typename T2 >
inline 
FixedArray<N,T>&
FixedArray<N,T>::
operator*=(const FixedArray<N,T2>& x) {
  iterator i = begin(), i_end = end();
  typename FixedArray<N,T2>::const_iterator j = x.begin();
  for (; i != i_end; ++i, ++j) {
    *i *= *j;
  }
  return *this;
}

template<int N, typename T> 
template< typename T2 >
inline 
FixedArray<N,T>&
FixedArray<N,T>::
operator/=(const FixedArray<N,T2>& x) {
  iterator i = begin(), i_end = end();
  typename FixedArray<N,T2>::const_iterator j = x.begin();
  for (; i != i_end; ++i, ++j) {
#ifdef DEBUG_FixedArray
    assert(*j != 0);
#endif
    *i /= *j;
  }
  return *this;
}

template<int N, typename T> 
template< typename T2 >
inline 
FixedArray<N,T>&
FixedArray<N,T>::
operator%=(const FixedArray<N,T2>& x) {
  iterator i = begin(), i_end = end();
  typename FixedArray<N,T2>::const_iterator j = x.begin();
  for (; i != i_end; ++i, ++j) {
#ifdef DEBUG_FixedArray
    assert(*j != 0);
#endif
    *i %= *j;
  }
  return *this;
}

//
// Math operators.
//

// Return the sum of the components.
template<int N, typename T>
inline
T
computeSum(const FixedArray<N,T>& x) {
  LOKI_STATIC_CHECK(N > 3, dimension_greater_than_3);
  return std::accumulate(x.begin(), x.end(), T(0));
}

// Return the product of the components.
template<int N, typename T>
inline
T
computeProduct(const FixedArray<N,T>& x) {
  LOKI_STATIC_CHECK(N > 3, dimension_greater_than_3);
  return std::accumulate(x.begin(), x.end(), T(1), std::multiplies<T>());
}

// Return the minimum component.  Use < for comparison.
template<int N, typename T>
inline
T
computeMinimum(const FixedArray<N,T>& x) {
  LOKI_STATIC_CHECK(N > 3, dimension_greater_than_3);
  return *std::min_element(x.begin(), x.end());
}
    
// Return the maximum component.  Use > for comparison.
template<int N, typename T>
inline
T
computeMaximum(const FixedArray<N,T>& x) {
  LOKI_STATIC_CHECK(N > 3, dimension_greater_than_3);
  return *std::max_element(x.begin(), x.end());
}
    
//
// Equality
//

// Return true if the arrays are equal.
template<typename T1, typename T2, int N>
inline
bool
operator==(const FixedArray<N,T1>& a, const FixedArray<N,T2>& b) {
  // CONTINUE: For some reason, xlC calls this function with N == 3.
#ifndef __IBM_ATTRIBUTES
  LOKI_STATIC_CHECK(N > 3, dimension_greater_than_3);
#endif
  return std::equal(a.begin(), a.end(), b.begin());
}

//
// Comparison
//

// Lexicographical less than comparison.
template<typename T1, typename T2, int N>
inline
bool
operator<(const FixedArray<N,T1>& a, const FixedArray<N,T2>& b) {
  LOKI_STATIC_CHECK(N > 3, dimension_greater_than_3);
  return std::lexicographical_compare(a.begin(), a.end(), 
				       b.begin(), b.end());
}

//
// I/O
//
    
template<int N, typename T>
inline
std::ostream&
operator<<(std::ostream& out, const FixedArray<N,T>& x) {
  typename FixedArray<N,T>::const_iterator i = x.begin(), i_end = x.end();
  if (i != i_end) {
    out << *i;
    ++i;
  }
  for (; i != i_end; ++i) {
    out << " " << *i;
  }
  return out;
}
    
template<int N, typename T>
inline
std::istream&
operator>>(std::istream& in, FixedArray<N,T>& x) {
  typename FixedArray<N,T>::iterator i = x.begin(), i_end = x.end();
  for (; i != i_end; ++i) {
    in >> *i;
  }
  return in;
}

//! Write the array elements in binary format.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
write_elements_binary(std::ostream& out, const FixedArray<N,T>& x) {
  out.write(reinterpret_cast<const char*>(x.data()), N * sizeof(T));
}
    
//! Read the array elements in binary format.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
read_elements_binary(std::istream& in, FixedArray<N,T>& x) {
  in.read(reinterpret_cast<char*>(x.data()), N * sizeof(T));
}

END_NAMESPACE_ADS

// End of file.
