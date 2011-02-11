// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 1999-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

#if !defined(__geom_Interval_ipp__)
#error This file is an implementation detail of the class Interval.
#endif

BEGIN_NAMESPACE_GEOM

//
// Constructors
//
  

template<int N, typename T>
inline
Interval<N,T>& 
Interval<N,T>::
operator=(const Interval& other) {
  // Avoid assignment to self
  if (&other != this) {
    _min = other._min;
    _max = other._max;
  }
  // Return *this so assignments can chain.
  return *this;
}


template<int N, typename T>
inline
void 
Interval<N,T>::
add(const Point& p) {
  for (int i = 0; i != N; ++i) {
    if (p[i] < _min[i]) {
      _min[i] = p[i];
    }
    if (p[i] > _max[i]) {
      _max[i] = p[i];
    }
  }
}


template<int N, typename T>
inline
void 
Interval<N,T>::
add(const Interval& x) {
  add(x.getLowerCorner());
  add(x.getUpperCorner());
}


//
// The bound operation.
//


template<int N, typename T>
template<typename InputIterator>
inline
void
Interval<N,T>::
bound(InputIterator first, InputIterator last) {
  if (first == last) {
    *this = Interval();
    return;
  }
    
  _min = _max = *first;
  ++first;
  while (first != last) {
    add(*first);
    ++first;
  }
}


template<int N, typename T>
inline
void 
Interval<N,T>::
bound(const Point& p) {
  _min = p;
  _max = p;
}


template<int N, typename T>
inline
void
Interval<N,T>::
bound(const Point& p, const Point& q) {
  for (int i = 0; i != N; ++i) {
    _min[i] = std::min(p[i], q[i]);
    _max[i] = std::max(p[i], q[i]);
  }
}


template<int N, typename T>
inline
void
Interval<N,T>::
bound(const Point& p, const Point& q, const Point& r) {
  for (int i = 0; i != N; ++i) {
    _min[i] = std::min(p[i], std::min(q[i], r[i]));
    _max[i] = std::max(p[i], std::max(q[i], r[i]));
  }
}


//
// Mathematical free functions
//


template<int N, typename T>
inline
bool 
doOverlap(const Interval<N,T>& a, const Interval<N,T>& b) {
  for (int i = 0; i != N; ++i) {
    if (std::max(a.getLowerCorner()[i], b.getLowerCorner()[i]) >= 
	std::min(a.getUpperCorner()[i], b.getUpperCorner()[i])) {
      return false;
    }
  }
  return true;
}


template<int N, typename T>
inline
Interval<N,T>
computeIntersection(const Interval<N,T>& a, const Interval<N,T>& b) {
  Interval<N,T> inter;
  computeIntersection(a, b, &inter);
  return inter;
}


template<int N, typename T>
inline
void
computeIntersection(const Interval<N,T>& a, const Interval<N,T>& b,
		    Interval<N,T>* x) {
  x->setLowerCorner(ads::computeMaximum(a.getLowerCorner(), 
					b.getLowerCorner()));
  x->setUpperCorner(ads::computeMinimum(a.getUpperCorner(), 
					b.getUpperCorner()));
}


//
// File I/O
//

//! Print in a nice format.
/*! \relates Interval */
template<int N, typename T>
inline
void
printFormatted(std::ostream& out, const Interval<N,T>& x) {
  for (int i = 0; i != N; ++i) {
    if (i != 0) {
      out << " x ";
    }
    out << "{ " << x.getLowerCorner()[i] << " ... " 
	<< x.getUpperCorner()[i] << " }";
  }
}

END_NAMESPACE_GEOM

// End of file.
