// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 1999-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

#if !defined(__geom_SemiOpenInterval_ipp__)
#error This file is an implementation detail of the class SemiOpenInterval.
#endif

BEGIN_NAMESPACE_GEOM

//
// Mathematical member functions
//

template<int N, typename T>
inline
bool
SemiOpenInterval<N,T>::
isEmpty() const {
  for (int i = 0; i != N; ++i) {
    if (getLowerCorner()[i] >= getUpperCorner()[i]) {
      return true;
    }
  }
  return false;
}


template<int N, typename T>
template<typename PointType>
inline
bool
SemiOpenInterval<N,T>::
isIn(const PointType& p) const {
  for (int i = 0; i != N; ++i) {
    if (p[i] < getLowerCorner()[i] || p[i] >= getUpperCorner()[i]) {
      return false;
    }
  }
  return true;
}


//
// Mathematical free functions
//


template<int N, typename T>
inline
bool 
doOverlap(const SemiOpenInterval<N,T>& a, const SemiOpenInterval<N,T>& b) {
  for (int i = 0; i != N; ++i) {
    if (std::max(a.getLowerCorner()[i], b.getLowerCorner()[i]) >= 
	 std::min(a.getUpperCorner()[i], b.getUpperCorner()[i])) {
      return false;
    }
  }
  return true;
}


//
// File I/O
//


// Print in a nice format.
template<int N, typename T>
inline
void
printFormatted(std::ostream& out, const SemiOpenInterval<N,T>& x) {
  for (int i = 0; i != N; ++i) {
    if (i != 0) {
      out << " x ";
    }
    out << "[ " << x.getLowerCorner()[i] << " ... " 
	<< x.getUpperCorner()[i] << " }";
  }
}


END_NAMESPACE_GEOM

// End of file.
