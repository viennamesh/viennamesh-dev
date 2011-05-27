// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

//
// Specializations for 1-D.
//

#if !defined(__FixedArray1_ipp__)
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
    
  template<typename T>
  inline
  T
  dot(const FixedArray<1,T>& x, const FixedArray<1,T>& y) {
    return x[0] * y[0];
  }

}


//
// Math operators.
//

template<typename T>
inline
T
computeSum(const FixedArray<1,T>& x) {
  return x[0];
}

template<typename T>
inline
T
computeProduct(const FixedArray<1,T>& x) {
  return x[0];
}

template<typename T>
inline
T
computeMinimum(const FixedArray<1,T>& x) {
  return x[0];
}

template<typename T>
inline
T
computeMaximum(const FixedArray<1,T>& x) {
  return x[0];
}



//---------------------------------------------------------------------------
// Apply the standard math functions.
//---------------------------------------------------------------------------

template<typename T>
inline
void
applyAbs(FixedArray<1,T>* x) {
  (*x)[0] = std::abs((*x)[0]);
}

template<typename T>
inline
void
applyAcos(FixedArray<1,T>* x) {
  (*x)[0] = std::acos((*x)[0]);
}

template<typename T>
inline
void
applyAsin(FixedArray<1,T>* x) {
  (*x)[0] = std::asin((*x)[0]);
}

template<typename T>
inline
void
applyAtan(FixedArray<1,T>* x) {
  (*x)[0] = std::atan((*x)[0]);
}

template<typename T>
inline
void
applyCeil(FixedArray<1,T>* x) {
  (*x)[0] = std::ceil((*x)[0]);
}

template<typename T>
inline
void
applyCos(FixedArray<1,T>* x) {
  (*x)[0] = std::cos((*x)[0]);
}

template<typename T>
inline
void
applyCosh(FixedArray<1,T>* x) {
  (*x)[0] = std::cosh((*x)[0]);
}

template<typename T>
inline
void
applyExp(FixedArray<1,T>* x) {
  (*x)[0] = std::exp((*x)[0]);
}

template<typename T>
inline
void
applyFloor(FixedArray<1,T>* x) {
  (*x)[0] = std::floor((*x)[0]);
}

template<typename T>
inline
void
applyLog(FixedArray<1,T>* x) {
  (*x)[0] = std::log((*x)[0]);
}

template<typename T>
inline
void
applyLog10(FixedArray<1,T>* x) {
  (*x)[0] = std::log10((*x)[0]);
}

template<typename T>
inline
void
applySin(FixedArray<1,T>* x) {
  (*x)[0] = std::sin((*x)[0]);
}

template<typename T>
inline
void
applySinh(FixedArray<1,T>* x) {
  (*x)[0] = std::sinh((*x)[0]);
}

template<typename T>
inline
void
applySqrt(FixedArray<1,T>* x) {
  (*x)[0] = std::sqrt((*x)[0]);
}

template<typename T>
inline
void
applyTan(FixedArray<1,T>* x) {
  (*x)[0] = std::tan((*x)[0]);
}

template<typename T>
inline
void
applyTanh(FixedArray<1,T>* x) {
  (*x)[0] = std::tanh((*x)[0]);
}



//
// Equality
//

template<typename T1, typename T2>
inline
bool
operator==(const FixedArray<1,T1>& a, const FixedArray<1,T2>& b) {
  return a[0] == b[0];
}

//
// Comparison
//

template<typename T1, typename T2>
inline
bool
operator<(const FixedArray<1,T1>& a, const FixedArray<1,T2>& b) {
  return a[0] < b[0];
}

END_NAMESPACE_ADS

// End of file.
