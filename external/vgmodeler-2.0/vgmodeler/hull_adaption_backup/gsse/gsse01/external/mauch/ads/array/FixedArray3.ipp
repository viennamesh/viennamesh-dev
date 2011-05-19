// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

//
// Specializations for 3-D.
//

#if !defined(__FixedArray3_ipp__)
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
  dot(const FixedArray<3,T>& x, const FixedArray<3,T>& y) {
    return x[0] * y[0] + x[1] * y[1] + x[2] * y[2];
  }

}


//
// Math operators.
//

template <typename T>
inline
T
computeSum(const FixedArray<3,T>& x) {
  return x[0] + x[1] + x[2];
}

template <typename T>
inline
T
computeProduct(const FixedArray<3,T>& x) {
  return x[0] * x[1] * x[2];
}

template <typename T>
inline
T
computeMinimum(const FixedArray<3,T>& x) {
  return std::min(std::min(x[0], x[1]), x[2]);
}

template <typename T>
inline
T
computeMaximum(const FixedArray<3,T>& x) {
  return std::max(std::max(x[0], x[1]), x[2]);
}


//---------------------------------------------------------------------------
// Apply the standard math functions.
//---------------------------------------------------------------------------

template<typename T>
inline
void
applyAbs(FixedArray<3,T>* x) {
  (*x)[0] = std::abs((*x)[0]);
  (*x)[1] = std::abs((*x)[1]);
  (*x)[2] = std::abs((*x)[2]);
}

template<typename T>
inline
void
applyAcos(FixedArray<3,T>* x) {
  (*x)[0] = std::acos((*x)[0]);
  (*x)[1] = std::acos((*x)[1]);
  (*x)[2] = std::acos((*x)[2]);
}

template<typename T>
inline
void
applyAsin(FixedArray<3,T>* x) {
  (*x)[0] = std::asin((*x)[0]);
  (*x)[1] = std::asin((*x)[1]);
  (*x)[2] = std::asin((*x)[2]);
}

template<typename T>
inline
void
applyAtan(FixedArray<3,T>* x) {
  (*x)[0] = std::atan((*x)[0]);
  (*x)[1] = std::atan((*x)[1]);
  (*x)[2] = std::atan((*x)[2]);
}

template<typename T>
inline
void
applyCeil(FixedArray<3,T>* x) {
  (*x)[0] = std::ceil((*x)[0]);
  (*x)[1] = std::ceil((*x)[1]);
  (*x)[2] = std::ceil((*x)[2]);
}

template<typename T>
inline
void
applyCos(FixedArray<3,T>* x) {
  (*x)[0] = std::cos((*x)[0]);
  (*x)[1] = std::cos((*x)[1]);
  (*x)[2] = std::cos((*x)[2]);
}

template<typename T>
inline
void
applyCosh(FixedArray<3,T>* x) {
  (*x)[0] = std::cosh((*x)[0]);
  (*x)[1] = std::cosh((*x)[1]);
  (*x)[2] = std::cosh((*x)[2]);
}

template<typename T>
inline
void
applyExp(FixedArray<3,T>* x) {
  (*x)[0] = std::exp((*x)[0]);
  (*x)[1] = std::exp((*x)[1]);
  (*x)[2] = std::exp((*x)[2]);
}

template<typename T>
inline
void
applyFloor(FixedArray<3,T>* x) {
  (*x)[0] = std::floor((*x)[0]);
  (*x)[1] = std::floor((*x)[1]);
  (*x)[2] = std::floor((*x)[2]);
}

template<typename T>
inline
void
applyLog(FixedArray<3,T>* x) {
  (*x)[0] = std::log((*x)[0]);
  (*x)[1] = std::log((*x)[1]);
  (*x)[2] = std::log((*x)[2]);
}

template<typename T>
inline
void
applyLog10(FixedArray<3,T>* x) {
  (*x)[0] = std::log10((*x)[0]);
  (*x)[1] = std::log10((*x)[1]);
  (*x)[2] = std::log10((*x)[2]);
}

template<typename T>
inline
void
applySin(FixedArray<3,T>* x) {
  (*x)[0] = std::sin((*x)[0]);
  (*x)[1] = std::sin((*x)[1]);
  (*x)[2] = std::sin((*x)[2]);
}

template<typename T>
inline
void
applySinh(FixedArray<3,T>* x) {
  (*x)[0] = std::sinh((*x)[0]);
  (*x)[1] = std::sinh((*x)[1]);
  (*x)[2] = std::sinh((*x)[2]);
}

template<typename T>
inline
void
applySqrt(FixedArray<3,T>* x) {
  (*x)[0] = std::sqrt((*x)[0]);
  (*x)[1] = std::sqrt((*x)[1]);
  (*x)[2] = std::sqrt((*x)[2]);
}

template<typename T>
inline
void
applyTan(FixedArray<3,T>* x) {
  (*x)[0] = std::tan((*x)[0]);
  (*x)[1] = std::tan((*x)[1]);
  (*x)[2] = std::tan((*x)[2]);
}

template<typename T>
inline
void
applyTanh(FixedArray<3,T>* x) {
  (*x)[0] = std::tanh((*x)[0]);
  (*x)[1] = std::tanh((*x)[1]);
  (*x)[2] = std::tanh((*x)[2]);
}



//
// Equality
//

template <typename T1, typename T2>
inline
bool
operator==(const FixedArray<3,T1>& a, const FixedArray<3,T2>& b) {
  return (a[0] == b[0] && a[1] == b[1] && a[2] == b[2]);
}

//
// Comparison
//

template <typename T1, typename T2>
inline
bool
operator<(const FixedArray<3,T1>& a, const FixedArray<3,T2>& b) {
  return (a[0] < b[0] || 
	  (a[0] == b[0] && (a[1] < b[1] || 
			    (a[1] == b[1] && a[2] < b[2]))));
}

END_NAMESPACE_ADS

// End of file.
