// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 1999-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

#if !defined(__geom_BBox_ipp__)
#error This file is an implementation detail of the class BBox.
#endif

BEGIN_NAMESPACE_GEOM

//
// Mathematical member functions
//


template<int N, typename T>
inline
bool
BBox<N,T>::
isEmpty() const {
  for (int i = 0; i != N; ++i) {
    if (getLowerCorner()[i] > getUpperCorner()[i]) {
      return true;
    }
  }
  return false;
}


template<int N, typename T>
template<typename PointType>
inline
bool
BBox<N,T>::
isIn(const PointType& p) const {
  for (int i = 0; i != N; ++i) {
    if (p[i] < getLowerCorner()[i] || getUpperCorner()[i] < p[i]) {
      return false;
    }
  }
  return true;
}


template<int N, typename T>
inline
bool
BBox<N,T>::
isIn(const BBox& x) const {
  for (int i = 0; i != N; ++i) {
    if (x.getLowerCorner()[i] < getLowerCorner()[i] || 
	getUpperCorner()[i] < x.getUpperCorner()[i]) {
      return false;
    }
  }
  return true;
}


template<int N, typename T>
inline
bool
BBox<N,T>::
isIn(const SemiOpenInterval<N,T>& x) const {
  for (int i = 0; i != N; ++i) {
    if (x.getLowerCorner()[i] < getLowerCorner()[i] || 
	getUpperCorner()[i] < x.getUpperCorner()[i]) {
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
doOverlap(const BBox<N,T>& a, const BBox<N,T>& b) {
  for (int i = 0; i != N; ++i) {
    if (std::max(a.getLowerCorner()[i], b.getLowerCorner()[i]) > 
	 std::min(a.getUpperCorner()[i], b.getUpperCorner()[i])) {
      return false;
    }
  }
  return true;
}


template<int N, typename T>
inline
bool 
doOverlap(const BBox<N,T>& a, const SemiOpenInterval<N,T>& b) {
  for (int i = 0; i != N; ++i) {
    if (a.getLowerCorner()[i] > a.getUpperCorner()[i] || 
	b.getLowerCorner()[i] >= b.getUpperCorner()[i] ||
	a.getLowerCorner()[i] >= b.getUpperCorner()[i] || 
	b.getLowerCorner()[i] > a.getUpperCorner()[i]) {
      return false;
    }
  }
  return true;
}


template<typename T>
inline
bool
isIn(const BBox<3,T>& bb, typename BBox<3,T>::point_type& p) {
  if (p[0] >= bb.getLowerCorner()[0] &&
      p[1] >= bb.getLowerCorner()[1] &&
      p[2] >= bb.getLowerCorner()[2] &&
      p[0] <= bb.getUpperCorner()[0] &&
      p[1] <= bb.getUpperCorner()[1] &&
      p[2] <= bb.getUpperCorner()[2]) {
    return true;
  }
  return false;
}


template<typename T>
inline
bool
isIn(const Interval<3,T>& a, const Interval<3,T>& b) {
  if (b.getLowerCorner()[0] < a.getLowerCorner()[0] || 
      a.getUpperCorner()[0] < b.getUpperCorner()[0] ||
      b.getLowerCorner()[1] < a.getLowerCorner()[1] || 
      a.getUpperCorner()[1] < b.getUpperCorner()[1] ||
      b.getLowerCorner()[2] < a.getLowerCorner()[2] || 
      a.getUpperCorner()[2] < b.getUpperCorner()[2]) {
    return false;
  }
  return true;
}


// CONTINUE: Try to get rid of floor and ceil.
template<typename MultiIndexOutputIterator, typename T>
inline
void
scanConvert(MultiIndexOutputIterator indices, const BBox<3,T>& box) {
  // Make the integer bounding box.
  BBox<3,int> ib(static_cast<int>(std::ceil(box.getLowerCorner()[0])),
		 static_cast<int>(std::ceil(box.getLowerCorner()[1])),
		 static_cast<int>(std::ceil(box.getLowerCorner()[2])),
		 static_cast<int>(std::floor(box.getUpperCorner()[0])),
		 static_cast<int>(std::floor(box.getUpperCorner()[1])),
		 static_cast<int>(std::floor(box.getUpperCorner()[2])));

  // Scan convert the integer bounding box.
  scanConvert(indices, ib);
}


template<typename MultiIndexOutputIterator>
inline
void
scanConvert(MultiIndexOutputIterator indices, const geom::BBox<3,int>& box) {
  const int iStart = box.getLowerCorner()[0];
  const int iEnd = box.getUpperCorner()[0];
  const int jStart = box.getLowerCorner()[1];
  const int jEnd = box.getUpperCorner()[1];
  const int kStart = box.getLowerCorner()[2];
  const int kEnd = box.getUpperCorner()[2];

  ads::FixedArray<3,int> index;
  for (index[2] = kStart; index[2] <= kEnd; ++index[2]) {
    for (index[1] = jStart; index[1] <= jEnd; ++index[1]) {
      for (index[0] = iStart; index[0] <= iEnd; ++index[0]) {
	*indices++ = index;
      }
    }
  }
}


template<typename MultiIndexOutputIterator, typename T>
inline
void
scanConvert(MultiIndexOutputIterator indices, const geom::BBox<3,T>& box,
	    const BBox<3,int>& domain) {
  // Make the integer bounding box.
  BBox<3,int> ib(static_cast<int>(std::ceil(box.getLowerCorner()[0])),
		 static_cast<int>(std::ceil(box.getLowerCorner()[1])),
		 static_cast<int>(std::ceil(box.getLowerCorner()[2])),
		 static_cast<int>(std::floor(box.getUpperCorner()[0])),
		 static_cast<int>(std::floor(box.getUpperCorner()[1])),
		 static_cast<int>(std::floor(box.getUpperCorner()[2])));

  // Scan convert the integer bounding box on the specified domain.
  scanConvert(indices, ib, domain);
}


template<typename MultiIndexOutputIterator>
inline
void
scanConvert(MultiIndexOutputIterator indices, const geom::BBox<3,int>& box,
	    const geom::BBox<3,int>& domain) {
  BBox<3,int> inter;
  computeIntersection(box, domain, &inter);
  if (! inter.isEmpty()) {
    scanConvert(indices, inter);
  }
}


// CONTINUE: Remove this.
#if 0
template<typename T, typename _IndexType>
inline
int 
scanConvert(std::vector<_IndexType>& indexSet, const BBox<3,T>& box)
{
  const int old_size = indexSet.size();

  const int iStart = int(std::ceil(box.getLowerCorner()[0]));
  const int iEnd = int(std::floor(box.getUpperCorner()[0]));
  const int jStart = int(std::ceil(box.getLowerCorner()[1]));
  const int jEnd = int(std::floor(box.getUpperCorner()[1]));
  const int kStart = int(std::ceil(box.getLowerCorner()[2]));
  const int kEnd = int(std::floor(box.getUpperCorner()[2]));

  _IndexType index;
  for (index[2] = kStart; index[2] <= kEnd; ++index[2]) {
    for (index[1] = jStart; index[1] <= jEnd; ++index[1]) {
      for (index[0] = iStart; index[0] <= iEnd; ++index[0]) {
	indexSet.push_back(index);
      }
    }
  }
  return indexSet.size() - old_size;
}
#endif


// CONTINUE: Remove this.
#if 0
template<typename T, typename _IndexType>
inline
int 
scanConvert(std::vector<_IndexType>& indexSet,
	      const BBox<3,T>& box, const BBox<3,T>& domain)
{
  BBox<3,T> inter = intersection(box, domain);
  if (!inter.isEmpty()) {
    return scanConvert(indexSet, inter);
  }
  return 0;
}
#endif


//
// File I/O
//


//! Print in a nice format.
/*! \relates BBox */
template<int N, typename T>
inline
void
printFormatted(std::ostream& out, const BBox<N,T>& x) {
  for (int i = 0; i != N; ++i) {
    if (i != 0) {
      out << " x ";
    }
    out << "[ " << x.getLowerCorner()[i] << " ... " 
	<< x.getUpperCorner()[i] << " ]";
  }
}


END_NAMESPACE_GEOM

// End of file.
