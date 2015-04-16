#ifndef VIENNAMESH_ALGORITHM_SYMMETRY_SPHERICAL_HARMONICS_HPP
#define VIENNAMESH_ALGORITHM_SYMMETRY_SPHERICAL_HARMONICS_HPP

/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include "viennameshpp/plugin.hpp"





inline double power_mone(int k)
{
  if (k < 0)
    k = -k;
  return k%2==0 ? 1.0 : -1.0;
}

/** @brief Compute the factorial */
inline double factorial(long n)
{
  assert(n>=0);

  double result = 1;
  for (long i=n; i>1; --i)
      result *= i;

  return result;
}

inline double binomial(int n, int k)
{
  return factorial(n) / (factorial(k) * factorial(n-k));
}

/** @brief Compute the double factorial, i.e. n(n-2)(n-4)... */
inline double doublefactorial(long n)
{
    double result = 1;
    double loopindex = n;

    while (loopindex > 1)
    {
        result *= loopindex;
        loopindex -= 2;
    }

    return result;
}


/** @brief Associated Legendre polynomial */
class AssocLegendre
{
public:
  AssocLegendre(long l, long m) : l_(l), m_(m) {}

  double operator()(double x) const //evaluation is carried out via a recursion formula
  {
    assert(l_ >= 0);

    if (m_ < 0)
      return power_mone(m_) * factorial(l_+m_) / factorial(l_-m_) * AssocLegendre(l_,-m_)(x);
    else if (l_ == m_)
      return (l_ == 0) ? (1.0) : (power_mone(l_) * doublefactorial(2*l_ - 1) * pow(sqrt(1.0 - x*x), l_));
    else if (m_ == (l_ - 1))
      return x * (2.0 * l_ - 1.0) * AssocLegendre(m_, m_)(x);
    else if (l_ < std::abs(m_))
      return 0.0;
    else
      return (  (2.0 * l_ - 1.0) * x * AssocLegendre(l_ - 1, m_)(x)
              - (l_ + m_ - 1.0)      * AssocLegendre(l_ - 2, m_)(x) ) / static_cast<double>(l_ - m_);
  }

private:
  long l_;
  long m_;
};



/** @brief A spherical harmonic */
class SphericalHarmonic
{
public:
  SphericalHarmonic(int l, int m) : l_(l), m_(m)
  {
    assert(l >= 0);
//     const double pi = 3.1415926535897932384626433832795;
//     normalisation = sqrt( (2.0*l_ + 1.0) * factorial(l_ - abs(m_)) / (2.0 * pi * factorial(l_ + abs(m_))) );
  }

  double real(double theta, double phi) const
  {
    if (m_ > 0)
      return normalisation_real() * AssocLegendre(l_, m_)(cos(theta)) * cos(m_ * phi);
    else if (m_ == 0)
      return normalisation_real() * AssocLegendre(l_, m_)(cos(theta)) / sqrt(2.0);
    else
      return normalisation_real() * AssocLegendre(l_,-m_)(cos(theta)) * sin(-m_ * phi);
  }

  std::complex<double> complex(double theta, double phi) const
  {
    return power_mone(m_) * normalisation_complex() * AssocLegendre(l_, m_)(cos(theta)) *
            std::exp( std::complex<double>(0,1)*static_cast<double>(m_)*phi );
  }

//   http://en.wikipedia.org/wiki/Spherical_harmonics#Real_form
//   std::complex<double> real(double theta, double phi) const
//   {
//     if (m_ > 0)
//       return 1.0/std::sqrt(2.0) *
//               ( SphericalHarmonic(l_,-m_).complex(theta,phi) +
//                 power_mone(m_)*SphericalHarmonic(l_, m_).complex(theta,phi) );
//     else if (m_ == 0)
//       return complex(theta, phi);
//     else
//       return std::complex<double>(0,1)/std::sqrt(2.0) *
//               ( SphericalHarmonic(l_, m_).complex(theta,phi) -
//                 power_mone(m_)*SphericalHarmonic(l_,-m_).complex(theta,phi) );
//   }

//   double conj(double theta, double phi) const
//   {
//     return SphericalHarmonic(l_,-m_)(theta,phi) * power_mone(m_);
//   }

  std::complex<double> conj_complex(double theta, double phi) const
  {
    return power_mone(m_) * SphericalHarmonic(l_,-m_).complex(theta,phi);
  }

private:

  double normalisation_real() const
  {
    return sqrt( (2.0*l_ + 1.0) * factorial(l_ - abs(m_)) / (2.0 * M_PI * factorial(l_ + abs(m_))) );
  }

  double normalisation_complex() const
  {
    return sqrt( (2.0*l_ + 1.0) * factorial(l_-m_) / (4.0 * M_PI * factorial(l_+m_)) );
  }

  int l_;
  int m_;
};

#endif
