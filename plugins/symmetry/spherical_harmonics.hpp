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
#include "common.hpp"

namespace viennamesh
{

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
        return (l_ == 0) ? (1.0) : (power_mone(l_) * doublefactorial(2*l_ - 1) * std::pow(std::sqrt(1.0 - x*x), l_));
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



  template<typename T>
  class SphericalHarmonic;

  template<>
  class SphericalHarmonic<double>
  {
  public:
    typedef double result_type;

    SphericalHarmonic(int l, int m) : l_(l), m_(m) { assert(l >= 0); }

    result_type operator()(double theta, double phi) const
    {
      if (m_ > 0)
        return normalization() * AssocLegendre(l_, m_)(std::cos(theta)) * std::cos(m_ * phi);
      else if (m_ == 0)
        return normalization() * AssocLegendre(l_, m_)(std::cos(theta)) / sqrt(2.0);
      else
        return normalization() * AssocLegendre(l_,-m_)(std::cos(theta)) * std::sin(-m_ * phi);
    }

  private:

    double normalization() const
    {
      return sqrt( (2.0*l_ + 1.0) * factorial(l_ - abs(m_)) / (2.0 * M_PI * factorial(l_ + abs(m_))) );
    }

    int l_;
    int m_;
  };


  template<>
  class SphericalHarmonic< std::complex<double> >
  {
  public:
    typedef std::complex<double> result_type;

    SphericalHarmonic(int l, int m) : l_(l), m_(m) { assert(l >= 0); }

    result_type operator()(double theta, double phi) const
    {
      return power_mone(m_) * normalization() * AssocLegendre(l_, m_)(cos(theta)) *
          std::exp( std::complex<double>(0,1)*static_cast<double>(m_)*phi );
    }

  private:

    double normalization() const
    {
      return sqrt( (2.0*l_ + 1.0) * factorial(l_-m_) / (4.0 * M_PI * factorial(l_+m_)) );
    }

    int l_;
    int m_;
  };

  template<typename T>
  T D_to_integrate(int l, int m, int m_,
                  ublas::matrix<double> const & rot,
                  double theta, double phi)
  {
    std::pair<double, double> rotated = prod(rot, theta, phi);

    return SphericalHarmonic<T>(l, m)(rotated.first, rotated.second) *
          std::conj(SphericalHarmonic<T>(l, m_)(theta, phi)) *
          std::sin(theta);

  //   return SphericalHarmonic< std::complex<double> >(l, m).complex(rotated.first, rotated.second) *
  //           SphericalHarmonic< std::complex<double> >(l, m_).conj_complex(theta, phi) *
  //           std::sin(theta);
  }

  template<typename T>
  T D_integrate(int l, int m, int m_,
                ublas::matrix<double> const & rot)
  {
    int theta_count = 500;
    int phi_count = 1000;

    double theta_step = M_PI / theta_count;
    double phi_step = 2.0*M_PI / phi_count;

    T result = 0.0;

    for (int theta_index = 0; theta_index != theta_count; ++theta_index)
      for (int phi_index = 0; phi_index != phi_count; ++phi_index)
      {
        double theta = theta_index * theta_step + theta_step*0.5;
        double phi = phi_index * phi_step + phi_step*0.5;

        result += D_to_integrate<T>(l, m, m_, rot, theta, phi);
      }

    return result * theta_step * phi_step;
  }

  template<typename T>
  T D_integrate(int l, int m, int m_, double beta)
  {
    return D_integrate<T>(l, m, m_, rotation_y(-beta));
  }

  template<typename T>
  T D_integrate(int l, int m, int m_,
                double alpha, double beta, double gamma)
  {
    ublas::matrix<double> rot = euler_rotation_zyz(alpha, beta, gamma);
    return D_integrate<T>(l, m, m_, rot);
  }






  template<typename T>
  T D(int l, int m, int m_,
      double alpha, double beta, double gamma);





  class Jacobi
  {
  public:

    Jacobi(int n_, int a_, int b_) : n(n_), a(a_), b(b_) {}

    template<typename T>
    T operator()(T const & x) const
    {
      T sum = 0;

      for (int s = 0; s <= std::min(n,n+a); ++s)
      {
        sum += 1.0 / (factorial(s) * factorial(n+a-s) * factorial(b+s) * factorial(n-s)) *
               std::pow((x-1)/2, n-s) * std::pow((x+1)/2, s);
      }

      return factorial(n+a) * factorial(n+b) * sum;
    }

  private:
    int n;
    int a;
    int b;
  };



  template<typename CoefficientT>
  polynom<CoefficientT> jacobi_polynom(int n, int a, int b)
  {
    typedef polynom<CoefficientT> PolynomType;

    PolynomType result;

    for (int s = 0; s <= std::min(n,n+a); ++s)
    {
      PolynomType tmp0 = pow( (PolynomType::X-static_cast<CoefficientT>(1))/static_cast<CoefficientT>(2), n-s );
      PolynomType tmp1 = pow( (PolynomType::X+static_cast<CoefficientT>(1))/static_cast<CoefficientT>(2), s );


      result += static_cast<CoefficientT>(1) / static_cast<CoefficientT>(factorial(s) * factorial(n+a-s) * factorial(b+s) * factorial(n-s)) * tmp0 * tmp1;
    }

    return static_cast<CoefficientT>(factorial(n+a) * factorial(n+b)) * result;
  }




  double d(int l, int m, int m_, double beta)
  {
    if (m < 0 && m_ < 0)
      return power_mone(m-m_) * d(l, -m, -m_, beta);
    if (m_ < 0)
      return power_mone(l+m) * d(l, m, -m_, M_PI - beta);
    if (m < 0)
      return power_mone(m-m_) * d(l, -m, -m_, beta);
    if (m > m_)
      return power_mone(m-m_) * d(l, m_, m, beta);

    double tmp = std::sqrt( (factorial(l+m_) * factorial(l-m_)) / (factorial(l+m) * factorial(l-m)) ) *
           std::pow(std::sin(beta/2), m_-m) * std::pow(std::cos(beta/2), m_+m) *
           Jacobi(l-m_, m_-m, m_+m)(std::cos(beta));

    return tmp;
  }

  template<>
  std::complex<double> D< std::complex<double> >(int l, int m, int m_,
                                                 double alpha, double beta, double gamma)
  {
    return std::conj(d(l,m,m_,beta) *
           std::exp( std::complex<double>(0, m*alpha) ) *
           std::exp( std::complex<double>(0, m_*gamma) ));
  }






  template<>
  double D<double>(int l, int m, int m_,
                   double alpha, double beta, double gamma)
  {
    typedef std::complex<double> ComplexType;

    std::complex<double> result;
    if (m == 0)
    {
      if (m_ < 0)
      {
        result = (power_mone(m_) * D<ComplexType>(l,m,-m_,alpha,beta,gamma) - D<ComplexType>(l,m,m_,alpha,beta,gamma)) *
                std::complex<double>(0,1) / std::sqrt(2);
      }
      else if (m_ == 0)
      {
        result = D<ComplexType>(l,m,m_,alpha,beta,gamma);
      }
      else // (m_ > 0)
      {
        result = (power_mone(m_) * D<ComplexType>(l,m,m_,alpha,beta,gamma) + D<ComplexType>(l,m,-m_,alpha,beta,gamma)) / std::sqrt(2);
      }
    }
    else if (m_ == 0)
    {
      if (m < 0)
      {
        result = (- power_mone(m) * D<ComplexType>(l,-m,m_,alpha,beta,gamma) + D<ComplexType>(l,m,m_,alpha,beta,gamma)) * std::complex<double>(0,1) / std::sqrt(2);
      }
      else if (m > 0)
      {
        result = (power_mone(m) * D<ComplexType>(l,m,m_,alpha,beta,gamma) + D<ComplexType>(l,-m,m_,alpha,beta,gamma)) / std::sqrt(2);
      }
    }
    else
    {
      if (m > 0 && m_ > 0)
      {
        m = std::abs(m);
        m_ = std::abs(m_);
        result = 1.0 / 2.0 *
                 (+ D<ComplexType>(l,-m,-m_,alpha,beta,gamma)
                  + power_mone(m_) * D<ComplexType>(l,-m,m_,alpha,beta,gamma)
                  + power_mone(m) * D<ComplexType>(l,m,-m_,alpha,beta,gamma)
                  + power_mone(m+m_) * D<ComplexType>(l,m,m_,alpha,beta,gamma));
      }

      if (m > 0 && m_ < 0)
      {
        m = std::abs(m);
        m_ = std::abs(m_);
        result = std::complex<double>(0,1) / 2.0 *
                 (- D<ComplexType>(l,-m,-m_,alpha,beta,gamma)
                  + power_mone(m_) * D<ComplexType>(l,-m,m_,alpha,beta,gamma)
                  - power_mone(m) * D<ComplexType>(l,m,-m_,alpha,beta,gamma)
                  + power_mone(m+m_) * D<ComplexType>(l,m,m_,alpha,beta,gamma));
      }

      if (m < 0 && m_ > 0)
      {
        m = std::abs(m);
        m_ = std::abs(m_);
        result = std::complex<double>(0,1) / 2.0 *
                 (+ D<ComplexType>(l,-m,-m_,alpha,beta,gamma)
                  + power_mone(m_) * D<ComplexType>(l,-m,m_,alpha,beta,gamma)
                  - power_mone(m) * D<ComplexType>(l,m,-m_,alpha,beta,gamma)
                  - power_mone(m+m_) * D<ComplexType>(l,m,m_,alpha,beta,gamma));
      }

      if (m < 0 && m_ < 0)
      {
        m = std::abs(m);
        m_ = std::abs(m_);
        result = 1.0 / 2.0 *
                 (+ D<ComplexType>(l,-m,-m_,alpha,beta,gamma)
                  - power_mone(m_) * D<ComplexType>(l,-m,m_,alpha,beta,gamma)
                  - power_mone(m) * D<ComplexType>(l,m,-m_,alpha,beta,gamma)
                  + power_mone(m+m_) * D<ComplexType>(l,m,m_,alpha,beta,gamma));
      }
    }

    return result.real();
  }



  template<typename T>
  T D(int l, int m, int m_, point const & s)
  {
    double s_theta;
    double s_phi;
    double s_r;
    to_spherical(s, s_theta, s_phi, s_r);
    return D<T>(l, m, m_, 0, s_theta, s_phi);
  }


}




#endif
