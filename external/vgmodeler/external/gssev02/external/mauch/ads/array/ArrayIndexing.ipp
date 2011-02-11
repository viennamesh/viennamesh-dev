// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

#if !defined(__ArrayIndexing_ipp__)
#error This file is an implementation detail of the class ArrayIndexing.
#endif

BEGIN_NAMESPACE_ADS

//
// Constructors.
//

// Default constructor.
template<int N, typename T>
inline
ArrayIndexing<N,T>::
ArrayIndexing() :
  base_type(),
  _root(0)
{}

// Copy constructor.
template<int N, typename T>
inline
ArrayIndexing<N,T>::
ArrayIndexing(const ArrayIndexing& x) :
  base_type(x),
  _root(x._root)
{}

/* CONTINUE
// Copy constructor.
template<int N, typename T>
template<typename T2>
inline
ArrayIndexing<N,T>::
ArrayIndexing(const ArrayIndexing<N,T2>& x) :
  base_type(x),
  _root(x.root())
{}
*/

// Assignment operator.  Shallow assignment.
template<int N, typename T>
inline
ArrayIndexing<N,T>& 
ArrayIndexing<N,T>::
operator=(const ArrayIndexing& other) {
  if (&other != this) {
    base_type::operator=(other);
    _root = other._root;
  }
  return *this;
}

/* CONTINUE
// Assignment operator.  Shallow assignment.
template<int N, typename T>
template<typename T2>
inline
ArrayIndexing<N,T>& 
ArrayIndexing<N,T>::
operator=(const ArrayIndexing<N,T2>& x)
{
  base_type::operator=(x);
  _root = x.root();
  return *this;
}
*/

// Construct given the extents and a pointer to the data.
template<int N, typename T>
template<typename Type>
inline
ArrayIndexing<N,T>::
ArrayIndexing(const index_type& extents, Type* data) :
  base_type(extents),
  _root(0) {
  compute_root(data);
}

// Rebuild given the extents and a pointer to the data.
template<int N, typename T>
template<typename Type>
inline
void
ArrayIndexing<N,T>::
rebuild(const index_type& extents, Type* data) {
  base_type::rebuild(extents);
  compute_root(data);
}

// Construct given the index ranges and a pointer to the data.
template<int N, typename T>
template<typename Type>
inline
ArrayIndexing<N,T>::
ArrayIndexing(const range_type& ranges, Type* data) :
  base_type(ranges),
  _root(0) {
  compute_root(data);
}

// Rebuild given the index ranges and a pointer to the data.
template<int N, typename T>
template<typename Type>
inline
void
ArrayIndexing<N,T>::
rebuild(const range_type& ranges, Type* data) {
  base_type::rebuild(ranges);
  compute_root(data);
}

//
// Private member functions.
//

// Compute the strides and the root.
template<int N, typename T>
inline
void
ArrayIndexing<N,T>::
compute_root(const const_pointer data) {
  base_type::compute_strides();
  _root = const_cast<pointer>(data) - internal::dot(lbounds(), strides());
}

//
// I/O
//

namespace internal {

  template<typename T>
  inline
  void
  pretty_print(std::ostream& out, const ArrayIndexing<1,T>& x) {
    for (int i = x.lbound(0); i != x.ubound(0); ++i) {
      out << x(i) << '\n';
    }
  }

  template<typename T>
  inline
  void
  pretty_print(std::ostream& out, const ArrayIndexing<2,T>& x) {
    for (int j = x.ubound(1) - 1; j >= x.lbound(1); --j) {
      for (int i = x.lbound(0); i < x.ubound(0); ++i) {
	out << x(i, j) << " ";
      }
      out << '\n';
    }
  }

  template<typename T>
  inline
  void
  pretty_print(std::ostream& out, const ArrayIndexing<3,T>& x) {
    for (int k = x.ubound(2) - 1; k >= x.lbound(2); --k) {
      for (int j = x.ubound(1) - 1; j >= x.lbound(1); --j) {
	for (int i = x.lbound(0); i < x.ubound(0); ++i) {
	  out << x(i, j, k) << " ";
	}
	out << '\n';
      }
      out << '\n';
    }
  }

} // end namespace internal


template<int N, typename T>
inline
void
ArrayIndexing<N,T>::
pretty_print(std::ostream& out) const {
  internal::pretty_print(out, *this);
}

template<typename T>
inline
void
ArrayIndexing<1,T>::
pretty_print(std::ostream& out) const {
  internal::pretty_print(out, *this);
}

END_NAMESPACE_ADS

// End of file.
