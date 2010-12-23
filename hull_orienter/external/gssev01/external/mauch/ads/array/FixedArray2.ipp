// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

//
// Specializations for 2-D.
//

#if !defined(__FixedArray2_ipp__)
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
    
  template <typename T>
  inline
  T
  dot(const FixedArray<2,T>& x, const FixedArray<2,T>& y) {
    return x[0] * y[0] + x[1] * y[1];
  }

}


//
// Math operators.
//

template <typename T>
inline
T
computeSum(const FixedArray<2,T>& x) {
  return x[0] + x[1];
}

template <typename T>
inline
T
computeProduct(const FixedArray<2,T>& x) {
  return x[0] * x[1];
}

template <typename T>
inline
T
computeMinimum(const FixedArray<2,T>& x) {
  return std::min(x[0], x[1]);
}

template <typename T>
inline
T
computeMaximum(const FixedArray<2,T>& x) {
  return std::max(x[0], x[1]);
}

//---------------------------------------------------------------------------
// Apply the standard math functions.
//---------------------------------------------------------------------------

template<typename T>
inline
void
applyAbs(FixedArray<2,T>* x) {
  (*x)[0] = std::abs((*x)[0]);
  (*x)[1] = std::abs((*x)[1]);
}

template<typename T>
inline
void
applyAcos(FixedArray<2,T>* x) {
  (*x)[0] = std::acos((*x)[0]);
  (*x)[1] = std::acos((*x)[1]);
}

template<typename T>
inline
void
applyAsin(FixedArray<2,T>* x) {
  (*x)[0] = std::asin((*x)[0]);
  (*x)[1] = std::asin((*x)[1]);
}

template<typename T>
inline
void
applyAtan(FixedArray<2,T>* x) {
  (*x)[0] = std::atan((*x)[0]);
  (*x)[1] = std::atan((*x)[1]);
}

template<typename T>
inline
void
applyCeil(FixedArray<2,T>* x) {
  (*x)[0] = std::ceil((*x)[0]);
  (*x)[1] = std::ceil((*x)[1]);
}

template<typename T>
inline
void
applyCos(FixedArray<2,T>* x) {
  (*x)[0] = std::cos((*x)[0]);
  (*x)[1] = std::cos((*x)[1]);
}

template<typename T>
inline
void
applyCosh(FixedArray<2,T>* x) {
  (*x)[0] = std::cosh((*x)[0]);
  (*x)[1] = std::cosh((*x)[1]);
}

template<typename T>
inline
void
applyExp(FixedArray<2,T>* x) {
  (*x)[0] = std::exp((*x)[0]);
  (*x)[1] = std::exp((*x)[1]);
}

template<typename T>
inline
void
applyFloor(FixedArray<2,T>* x) {
  (*x)[0] = std::floor((*x)[0]);
  (*x)[1] = std::floor((*x)[1]);
}

template<typename T>
inline
void
applyLog(FixedArray<2,T>* x) {
  (*x)[0] = std::log((*x)[0]);
  (*x)[1] = std::log((*x)[1]);
}

template<typename T>
inline
void
applyLog10(FixedArray<2,T>* x) {
  (*x)[0] = std::log10((*x)[0]);
  (*x)[1] = std::log10((*x)[1]);
}

template<typename T>
inline
void
applySin(FixedArray<2,T>* x) {
  (*x)[0] = std::sin((*x)[0]);
  (*x)[1] = std::sin((*x)[1]);
}

template<typename T>
inline
void
applySinh(FixedArray<2,T>* x) {
  (*x)[0] = std::sinh((*x)[0]);
  (*x)[1] = std::sinh((*x)[1]);
}

template<typename T>
inline
void
applySqrt(FixedArray<2,T>* x) {
  (*x)[0] = std::sqrt((*x)[0]);
  (*x)[1] = std::sqrt((*x)[1]);
}

template<typename T>
inline
void
applyTan(FixedArray<2,T>* x) {
  (*x)[0] = std::tan((*x)[0]);
  (*x)[1] = std::tan((*x)[1]);
}

template<typename T>
inline
void
applyTanh(FixedArray<2,T>* x) {
  (*x)[0] = std::tanh((*x)[0]);
  (*x)[1] = std::tanh((*x)[1]);
}



//
// Equality
//

template <typename T1, typename T2>
inline
bool
operator==(const FixedArray<2,T1>& a, const FixedArray<2,T2>& b) {
  return (a[0] == b[0] && a[1] == b[1]);
}

//
// Comparison
//

template <typename T1, typename T2>
inline
bool
operator<(const FixedArray<2,T1>& a, const FixedArray<2,T2>& b) {
  return (a[0] < b[0] || (a[0] == b[0] && a[1] < b[1]));
}

END_NAMESPACE_ADS

// End of file.
