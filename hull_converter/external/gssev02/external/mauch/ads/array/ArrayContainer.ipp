// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

#if !defined(__ArrayContainer_ipp__)
#error This file is an implementation detail of the class ArrayContainer.
#endif

BEGIN_NAMESPACE_ADS

//
// Assignment operators with scalar operand.
//

template<typename T, bool A>
inline
ArrayContainer<T,A>& 
ArrayContainer<T,A>::
operator=(parameter_type x) {
  std::fill(begin(), end(), x);
  return *this;
}

// CONTINUE
#if 0
template<typename T, bool A>
inline
ArrayContainer<T,A>& 
ArrayContainer<T,A>::
operator+=(parameter_type x) {
  for (iterator i = begin(); i != end(); ++i) {
    *i += x;
  }
  return *this;
}

template<typename T, bool A>
inline
ArrayContainer<T,A>& 
ArrayContainer<T,A>::
operator-=(parameter_type x) {
  for (iterator i = begin(); i != end(); ++i) {
    *i -= x;
  }
  return *this;
}

template<typename T, bool A>
inline
ArrayContainer<T,A>& 
ArrayContainer<T,A>::
operator*=(parameter_type x) {
  for (iterator i = begin(); i != end(); ++i) {
    *i *= x;
  }
  return *this;
}

template<typename T, bool A>
inline
ArrayContainer<T,A>& 
ArrayContainer<T,A>::
operator/=(parameter_type x) {
#ifdef DEBUG_ArrayContainer
  assert(x != 0);
#endif
  for (iterator i = begin(); i != end(); ++i) {
    *i /= x;
  }
  return *this;
}

template<typename T, bool A>
inline
ArrayContainer<T,A>& 
ArrayContainer<T,A>::
operator%=(parameter_type x) {
#ifdef DEBUG_ArrayContainer
  assert(x != 0);
#endif
  for (iterator i = begin(); i != end(); ++i) {
    *i %= x;
  }
  return *this;
}
#endif




// To be used for arithmetic types.
template<typename T, bool A>
inline
ArrayContainer<T,A>& 
operator+=(ArrayContainer<T,A>& array, 
	   typename ArrayContainer<T,A>::parameter_type x) {
  for (typename ArrayContainer<T,A>::iterator i = array.begin(); 
       i != array.end(); ++i) {
    *i += x;
  }
  return array;
}

// To be used for arithmetic types.
template<typename T, bool A>
inline
ArrayContainer<T,A>& 
operator-=(ArrayContainer<T,A>& array, 
	   typename ArrayContainer<T,A>::parameter_type x) {
  for (typename ArrayContainer<T,A>::iterator i = array.begin(); 
       i != array.end(); ++i) {
    *i -= x;
  }
  return array;
}

// To be used for arithmetic types.
template<typename T, bool A>
inline
ArrayContainer<T,A>& 
operator*=(ArrayContainer<T,A>& array, 
	   typename ArrayContainer<T,A>::parameter_type x) {
  for (typename ArrayContainer<T,A>::iterator i = array.begin(); 
       i != array.end(); ++i) {
    *i *= x;
  }
  return array;
}

// To be used for arithmetic types.
template<typename T, bool A>
inline
ArrayContainer<T,A>& 
operator/=(ArrayContainer<T,A>& array, 
	   typename ArrayContainer<T,A>::parameter_type x) {
#ifdef DEBUG_ArrayContainer
  assert(x != 0);
#endif
  for (typename ArrayContainer<T,A>::iterator i = array.begin(); 
       i != array.end(); ++i) {
    *i /= x;
  }
  return array;
}

// To be used for arithmetic types.
template<typename T, bool A>
inline
ArrayContainer<T,A>& 
operator%=(ArrayContainer<T,A>& array, 
	   typename ArrayContainer<T,A>::parameter_type x) {
#ifdef DEBUG_ArrayContainer
  assert(x != 0);
#endif
  for (typename ArrayContainer<T,A>::iterator i = array.begin(); 
       i != array.end(); ++i) {
    *i %= x;
  }
  return array;
}

// To be used for pointer types.
template<typename T, bool A>
inline
ArrayContainer<T*,A>& 
operator+=(ArrayContainer<T*,A>& array, 
	   typename ArrayContainer<T*,A>::difference_type x) {
  for (typename ArrayContainer<T*,A>::iterator i = array.begin(); 
       i != array.end(); ++i) {
    *i += x;
  }
  return array;
}

// To be used for pointer types.
template<typename T, bool A>
inline
ArrayContainer<T*,A>& 
operator-=(ArrayContainer<T*,A>& array, 
	   typename ArrayContainer<T*,A>::difference_type x) {
  for (typename ArrayContainer<T*,A>::iterator i = array.begin(); 
       i != array.end(); ++i) {
    *i -= x;
  }
  return array;
}


//
// Assignment operators with array operand.
//

template<typename T1, bool A1, typename T2, bool A2>
inline
ArrayContainer<T1,A1>& 
operator+=(ArrayContainer<T1,A1>& x, const ArrayContainer<T2,A2> & y) {
  assert(x.size() == y.size());
  typename ArrayContainer<T1,A1>::iterator i = x.begin();
  typename ArrayContainer<T2,A2>::const_iterator j = y.begin();
  for (; i != x.end(); ++i, ++j) {
    *i += *j;
  }
  return x;
}

template<typename T1, bool A1, typename T2, bool A2>
inline
ArrayContainer<T1,A1>& 
operator-=(ArrayContainer<T1,A1>& x, const ArrayContainer<T2,A2> & y) {
  assert(x.size() == y.size());
  typename ArrayContainer<T1,A1>::iterator i = x.begin();
  typename ArrayContainer<T2,A2>::const_iterator j = y.begin();
  for (; i != x.end(); ++i, ++j) {
    *i -= *j;
  }
  return x;
}

template<typename T1, bool A1, typename T2, bool A2>
inline
ArrayContainer<T1,A1>& 
operator*=(ArrayContainer<T1,A1>& x, const ArrayContainer<T2,A2> & y) {
  assert(x.size() == y.size());
  typename ArrayContainer<T1,A1>::iterator i = x.begin();
  typename ArrayContainer<T2,A2>::const_iterator j = y.begin();
  for (; i != x.end(); ++i, ++j) {
    *i *= *j;
  }
  return x;
}

template<typename T1, bool A1, typename T2, bool A2>
inline
ArrayContainer<T1,A1>& 
operator/=(ArrayContainer<T1,A1>& x, const ArrayContainer<T2,A2> & y) {
  assert(x.size() == y.size());
  typename ArrayContainer<T1,A1>::iterator i = x.begin();
  typename ArrayContainer<T2,A2>::const_iterator j = y.begin();
  for (; i != x.end(); ++i, ++j) {
#ifdef DEBUG_ArrayContainer
    assert(*j != 0);
#endif
    *i /= *j;
  }
  return x;
}

template<typename T1, bool A1, typename T2, bool A2>
inline
ArrayContainer<T1,A1>& 
operator%=(ArrayContainer<T1,A1>& x, const ArrayContainer<T2,A2> & y) {
  assert(x.size() == y.size());
  typename ArrayContainer<T1,A1>::iterator i = x.begin();
  typename ArrayContainer<T2,A2>::const_iterator j = y.begin();
  for (; i != x.end(); ++i, ++j) {
#ifdef DEBUG_ArrayContainer
    assert(*j != 0);
#endif
    *i %= *j;
  }
  return x;
}


//
// Manipulators.
//

namespace internal {

  // Negate for number arrays.
  template<typename T, bool A>
  inline
  void
  negate(ArrayContainer<T,A>& x) {
    typename ArrayContainer<T,A>::iterator i = x.begin();
    for (; i != x.end(); ++i) {
      *i = -*i;
    }
  }

  // Negate for boolean arrays.
  template<bool A>
  inline
  void
  negate(ArrayContainer<bool,A>& x) {
    typename ArrayContainer<bool,A>::iterator i = x.begin();
    for (; i != x.end(); ++i) {
      *i = !*i;
    }
  }

}

template<typename T, bool A>
inline
void
ArrayContainer<T,A>::
negate() {
  internal::negate(*this);
}

template<typename T, bool A>
inline
void
ArrayContainer<T,A>::
fill(parameter_type value) {
  std::fill(begin(), end(), value);
}


// CONTINUE: This is a dangerous function.  I think I should get rid of it.
#if 0
template<typename T, bool A>
template<typename InputIterator>
inline
void
ArrayContainer<T,A>::
copy(InputIterator start, InputIterator finish) {
#ifdef DEBUG_ArrayContainer
  iterator last = std::copy(start, finish, begin());
  assert(last == end());
#else
  std::copy(start, finish, begin());
#endif
}
#endif

END_NAMESPACE_ADS

// End of file.
