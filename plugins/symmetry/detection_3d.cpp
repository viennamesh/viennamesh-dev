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

#include <set>
#include <map>
#include <iterator>
#include <iomanip>
#include <fstream>
#include <complex>

#include "detection_3d.hpp"
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/refine.hpp"

// #include "spherical_harmonics.hpp"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
namespace ublas = boost::numeric::ublas;



namespace viennamesh
{


//   template<typename T>
//   T power(T value, int exponent)
//   {
//     if (exponent == 0)
//       return 1;
//     if (exponent < 0)
//       return T(1)/power(value, -exponent);
//
//     T result = T(1);
//     for (int i = 0; i != exponent; ++i)
//       result *= value;
//
//     return result;
//   }


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

      if (l_ < 0)
        return AssocLegendre(-l_-1,m_)(x);
      else if (m_ < 0)
      {
        return power_mone(m_) * factorial(l_+m_) / factorial(l_-m_) * AssocLegendre(l_,-m_)(x);
      }
      else if (l_ == m_)
        return power_mone(l_) * doublefactorial(2*l_ - 1) * pow(sqrt(1.0 - x*x), l_);
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
      const double pi = 3.1415926535897932384626433832795;
      normalisation = sqrt( (2.0*l_ + 1.0) * factorial(l_ - abs(m_)) / (2.0 * pi * factorial(l_ + abs(m_))) );
    }

    double operator()(double theta, double phi) const
    {
      if (m_ > 0)
        return normalisation * AssocLegendre(l_, m_)(cos(theta)) * cos(m_ * phi);
      else if (m_ == 0)
        return normalisation * AssocLegendre(l_, m_)(cos(theta)) / sqrt(2.0);
      else
        return normalisation * AssocLegendre(l_,-m_)(cos(theta)) * sin(-m_ * phi);
    }

    std::complex<double> complex(double theta, double phi) const
    {
      double normalisation_complex = sqrt( (2.0*l_ + 1.0) * factorial(l_-m_) / (4.0 * M_PI * factorial(l_+m_)) );
      return power_mone(m_) * normalisation_complex * AssocLegendre(l_, m_)(cos(theta)) *
             std::exp( std::complex<double>(0,1)*static_cast<double>(m_)*phi );
    }

    std::complex<double> real(double theta, double phi) const
    {
      if (m_ > 0)
        return 1.0/std::sqrt(2.0) *
                ( SphericalHarmonic(l_,-m_).complex(theta,phi) +
                  power_mone(m_)*SphericalHarmonic(l_, m_).complex(theta,phi) );
      else if (m_ == 0)
        return complex(theta, phi);
      else
        return std::complex<double>(0,1)/std::sqrt(2.0) *
                ( SphericalHarmonic(l_, m_).complex(theta,phi) -
                  power_mone(m_)*SphericalHarmonic(l_,-m_).complex(theta,phi) );
    }

    double conj(double theta, double phi) const
    {
      return SphericalHarmonic(l_,-m_)(theta,phi) * power_mone(m_);
    }

    std::complex<double> conj_complex(double theta, double phi) const
    {
      return power_mone(m_) * SphericalHarmonic(l_,-m_).complex(theta,phi);
    }

  private:
    double normalisation;
    int l_;
    int m_;
  };


  double complex_spherical_harmonics_pi(int l, int m, double theta)
  {
    double normalisation = sqrt( (2.0*l + 1.0)/(4.0 * M_PI) * factorial(l - abs(m))/factorial(l + abs(m)) );
    double P_l_m = AssocLegendre(l, m)( std::cos(theta) );
    return power_mone(m) * normalisation * P_l_m;
  }

  double conj_complex_spherical_harmonics_pi(int l, int m, double theta)
  {
    return power_mone(m) * complex_spherical_harmonics_pi(l, -m, theta);
  }



  point_t prod(ublas::matrix<double> const & mat, point_t const & vec)
  {
    ublas::vector<double> tmp(vec.size());
    std::copy(vec.begin(), vec.end(), tmp.begin());
    ublas::vector<double> result_tmp = ublas::prod(mat, tmp);
    point_t result(result_tmp.size());
    std::copy(result_tmp.begin(), result_tmp.end(), result.begin());
    return result;
  }




  void from_spherical(double theta, double phi, double r, point_t & result)
  {
    result.resize(3);
    result[0] = r * std::sin(theta) * std::cos(phi);
    result[1] = r * std::sin(theta) * std::sin(phi);
    result[2] = r * std::cos(theta);
  }

  void to_spherical(point_t const & p, double & theta, double & phi, double & r)
  {
    r = std::sqrt( p[0]*p[0] + p[1]*p[1] + p[2]*p[2] );
    theta = std::acos( p[2] / r );
    phi = atan2(p[1],p[0]);
  }


  void transform(ublas::matrix<double> const & rot,
                 double theta_in, double phi_in, double r_in,
                 double & theta_out, double & phi_out, double & r_out)
  {
    point_t p_in;
    from_spherical(theta_in, phi_in, r_in, p_in);
    point_t p_out = prod(rot, p_in);
    to_spherical(p_out, theta_out, phi_out, r_out);
  }




  // http://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
  ublas::matrix<double> euler_angle_matrix_zyz(double alpha, double beta, double gamma)
  {
    double s1 = std::sin(alpha);
    double c1 = std::cos(alpha);

    double s2 = std::sin(beta);
    double c2 = std::cos(beta);

    double s3 = std::sin(gamma);
    double c3 = std::cos(gamma);

    ublas::matrix<double> tmp(3,3);

    tmp(0,0) = c1*c2*c3 - s1*s3;
    tmp(0,1) = -c3*s1 - c1*c2*s3;
    tmp(0,2) = c1*s2;

    tmp(1,0) = c1*s3 + c2*c3*s1;
    tmp(1,1) = c1*c3 - c2*s1*s3;
    tmp(1,2) = s1*s2;

    tmp(2,0) = -c3*s2;
    tmp(2,1) = s2*s3;
    tmp(2,2) = c2;

    return tmp;
  }



  double S(int p, int l)
  {
    double sum = 0.0;
    for (int k = l; k <= 2*l; ++k)
    {
      sum += power_mone(k) * static_cast<double>(std::pow(2.0, 2*p+1) * factorial(p) * factorial(2*k) * factorial(p+k-l)) /
              static_cast<double>( factorial(2*(p+k-l)+1) * factorial(k-l) * factorial(k) * factorial(2*l-k) );
    }

    return sum * std::sqrt( (4*l+1)*M_PI ) / static_cast<double>(std::pow(2.0, 2*l));
  }


  std::complex<double> D(double l, double m, double m_,
                             double alpha, double beta, double gamma)
  {
    assert(m == 0);

    // A signal-processing framework for reflection, doi>10.1145/1027411.1027416, (32).1
    // http://cseweb.ucsd.edu/~ravir/p1004-ramamoorthi.pdf
    std::complex<double> d_ = std::sqrt( 4*M_PI / (2.0*l+1.0) ) * SphericalHarmonic(l,m_).conj_complex(beta, M_PI);

    // A signal-processing framework for reflection, doi>10.1145/1027411.1027416, (27)
    // http://cseweb.ucsd.edu/~ravir/p1004-ramamoorthi.pdf
    d_ *= std::exp( std::complex<double>(0,1)*static_cast<double>(m_)*gamma );

    // Maple procedures for the coupling of angular momenta. IX. Wigner D-functions and rotation matrices,         doi:10.1016/j.cpc.2005.12.008, (4)
    // http://www.atomic-theory.uni-jena.de/pub/p186.b06.cpc-racahIX-original.pdf
    d_ *= power_mone(l-m_);
    return d_;
  }


  std::complex<double> C(int two_l, int m, int two_p, point_t const & s)
  {
    if (two_l%2 != 0)
      abort();

    if (two_p%2 != 0)
      abort();

    int p = two_p/2;
    int l = two_l/2;

    double s_theta;
    double s_phi;
    double s_r;
    to_spherical(s, s_theta, s_phi, s_r);

    std::complex<double> D_ = D(two_l, 0, m, 0, -s_theta, -s_phi);

    return std::pow(viennagrid::norm_2(s), two_p) * D_;//  D(0, m, l, s);
  }


  template<bool mesh_is_const>
  std::complex<double> C(int two_l, int m, int two_p, viennagrid::base_mesh<mesh_is_const> const & mesh)
  {
    typedef viennagrid::base_mesh<mesh_is_const> MeshType;
    typedef typename viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRange;
    typedef typename viennagrid::result_of::iterator<ConstCellRange>::type ConstCellIterator;

    if (two_l%2 != 0)
      abort();

    if (two_p%2 != 0)
      abort();

    std::complex<double> result = 0.0;
    int l = two_l/2;
    int p = two_p/2;

    ConstCellRange cells( mesh );
    for (ConstCellIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      point_t point = viennagrid::centroid(*cit);
      result += C(two_l,m,two_p,point) * viennagrid::volume(*cit);
    }

    return result * S(p,l);
  }




  template<bool mesh_is_const>
  double M(int two_p, viennagrid::base_mesh<mesh_is_const> const & mesh, double theta, double phi)
  {
    assert(two_p % 2 == 0);
    int p = two_p/2;

    std::complex<double> sum = 0.0;
    for (int l = 0; l <= p; ++l)
      for (int m = -2*l; m <= 2*l; ++m)
        sum += viennamesh::C(2*l, m, two_p, mesh) * SphericalHarmonic(2*l,m).complex(theta, phi);

    return sum.real();
  }

  template<bool mesh_is_const>
  double grad_M(int p, viennagrid::base_mesh<mesh_is_const> const & mesh, double theta, double phi, double eps)
  {
    double theta_m_eps = M(p, mesh, theta-eps, phi);
    double theta_p_eps = M(p, mesh, theta+eps, phi);

    double d_theta = (theta_p_eps-theta_m_eps) / (2.0*eps);

    double phi_m_eps = M(p, mesh, theta, phi-eps);
    double phi_p_eps = M(p, mesh, theta, phi+eps);

    double d_phi = (phi_p_eps-phi_m_eps) / (2.0*eps);

    return std::sqrt(d_theta*d_theta + d_phi*d_phi);
  }


  template<bool mesh_is_const>
  double M_direct(int p, viennagrid::base_mesh<mesh_is_const> const & mesh, point_t const & w)
  {
    typedef viennagrid::base_mesh<mesh_is_const> MeshType;
    typedef typename viennagrid::result_of::const_element_range<MeshType,2>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;

    double result = 0.0;

    ConstCellRangeType cells(mesh);
    for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      point_t point = viennagrid::centroid(*cit);
      result += pow( viennagrid::norm_2(viennagrid::cross_prod(point, w)), p ) * viennagrid::volume(*cit);
    }

    return result;
  }

  template<bool mesh_is_const>
  double M_direct(int p, viennagrid::base_mesh<mesh_is_const> const & mesh, double theta, double phi)
  {
    point_t w;
    from_spherical(theta, phi, 1.0, w);
    return M_direct(p, mesh, w);
  }

  template<bool mesh_is_const>
  double M_grad_direct(int p, viennagrid::base_mesh<mesh_is_const> const & mesh, double theta, double phi, double eps)
  {
    double theta_m_eps = M_direct(p, mesh, theta-eps, phi);
    double theta_p_eps = M_direct(p, mesh, theta+eps, phi);

    double d_theta = (theta_p_eps-theta_m_eps) / (2.0*eps);

    double phi_m_eps = M_direct(p, mesh, theta, phi-eps);
    double phi_p_eps = M_direct(p, mesh, theta, phi+eps);

    double d_phi = (phi_p_eps-phi_m_eps) / (2.0*eps);

    return std::sqrt(d_theta*d_theta + d_phi*d_phi);
  }





  template<typename MeshT>
  class CachedM
  {
  public:

    CachedM(int two_p_, MeshT const & mesh) : mesh_(mesh), p(two_p_/2), min_l(0), max_l(p), min_m(-max_l*2), max_m(max_l*2)
    {}

    double operator()(double theta, double phi) const
    {
      std::complex<double> sum = 0.0;
      for (int l = 0; l <= p; ++l)
        for (int m = -2*l; m <= 2*l; ++m)
        {
          sum += this->C(2*l, m) * SphericalHarmonic(2*l,m).complex(theta, phi);
        }

      return sum.real();
    }

    double grad(double theta, double phi, double eps) const
    {
      double theta_m_eps = (*this)(theta-eps, phi);
      double theta_p_eps = (*this)(theta+eps, phi);

      double d_theta = (theta_p_eps-theta_m_eps) / (2.0*eps);

      double phi_m_eps = (*this)(theta, phi-eps);
      double phi_p_eps = (*this)(theta, phi+eps);

      double d_phi = (phi_p_eps-phi_m_eps) / (2.0*eps);

      return std::sqrt(d_theta*d_theta + d_phi*d_phi);
    }


  private:

    std::complex<double> C(int l, int m) const
    {
      std::map< std::pair<int, int>, std::complex<double> >::iterator it = values.find( std::make_pair(l,m) );
      if (it != values.end())
        return it->second;

      std::complex<double> tmp = viennamesh::C(l, m, 2*p, mesh_);
      values.insert( std::make_pair( std::make_pair(l,m), tmp ) );
      return tmp;
    }

    MeshT const & mesh_;
    mutable std::map< std::pair<int, int>, std::complex<double> > values;

    int p;
    int min_l;
    int max_l;
    int min_m;
    int max_m;
  };










//   double sin_2p(int two_p, double beta)
//   {
//     return std::pow(std::sin(beta), two_p);
//   }
//
//   double sin_2p_SH(int two_p, double beta)
//   {
//     double sum = 0.0;
//
//     if (two_p%2 != 0)
//       abort();
//
//     int p = two_p/2;
//
//     for (int l = 0; l <= p; ++l)
//       sum += S(p,l) * SphericalHarmonic(2*l,0)(beta, 0);
//
// //       complex_spherical_harmonics_pi(2*l, 0, beta);
//
//     return sum;
//   }


//   std::complex<double> d_complex(double l, double m, double m_,
//                                  double beta,
//                                  int theta_step_count = 10, int phi_step_count = 20)
//   {
//     double theta_step = M_PI/theta_step_count;
//     double phi_step = 2.0*M_PI/phi_step_count;
//
// //     ublas::matrix<double> rotate = euler_angle_matrix_zyz(beta, alpha, gamma);
//   }



//   std::complex<double> D_complex(double l, double m, double m_,
//                                  double alpha, double beta, double gamma,
//                                  int theta_step_count = 10, int phi_step_count = 20)
//   {
//     double theta_step = M_PI/theta_step_count;
//     double phi_step = 2.0*M_PI/phi_step_count;
//
//     ublas::matrix<double> rotate = euler_angle_matrix_zyz(alpha, beta, gamma);
//
//     std::complex<double> result = 0;
//
//     for (double theta = theta_step/2.0; theta < M_PI; theta += theta_step)
//     {
//       for (double phi = phi_step/2.0; phi < 2.0*M_PI; phi += phi_step)
//       {
//         double theta_rotated;
//         double phi_rotated;
//         double r_rotated;
//
//         transform(rotate, theta, phi, 1.0, theta_rotated, phi_rotated, r_rotated);
//
//         result += (SphericalHarmonic(l,m).complex(theta_rotated, phi_rotated) *
//                   SphericalHarmonic(l,m_).conj_complex(theta, phi) *
//                   std::sin(theta)
//                   * theta_step*phi_step);
//       }
//     }
//
//     return result;
//   }
//
//
//   std::complex<double> SH_rotated(int l, int m, double alpha, double beta, double gamma, double theta, double phi)
//   {
// //     double s_theta;
// //     double s_phi;
// //     double s_r;
// //     to_spherical(s, s_theta, s_phi, s_r);
//
//     ublas::matrix<double> rot = euler_angle_matrix_zyz(alpha, beta, gamma);
//
//     double rotated_theta;
//     double rotated_phi;
//     double rotated_r;
//
//     transform(rot, theta, phi, 1.0, rotated_theta, rotated_phi, rotated_r);
//
//     return SphericalHarmonic(l,m).complex(rotated_theta, rotated_phi);
//   }
//
//   std::complex<double> SH_rotated2(int l, int m, double alpha, double beta, double gamma, double theta, double phi)
//   {
//     assert(m == 0);
//
//     std::complex<double> sum = 0.0;
//     for (int m_ = -l; m_ <= l; ++m_)
//     {
// //       sum += std::sqrt( 4.0 * M_PI / static_cast<double>(2.0*l+1.0) ) *
// //       conj_complex_spherical_harmonics_pi(l,m_,s_theta) * SphericalHarmonic(l,m_)(theta, phi);
//
//
// //       double d = std::sqrt( 4*M_PI / (2.0*l+1.0) ) * SphericalHarmonic(l,m_).conj_complex(s_theta, M_PI).real();
// //       double d = D_alpha(l,m,m_,s_theta);
// //       std::complex<double> d = D_complex(l,m,m_,alpha, beta, gamma, 10, 20);
//       std::complex<double> d = D_new(l,m,m_,alpha, beta, gamma);
//
//
//
// //       std::cout << D_complex(l,m,m_,s_theta,0,0) << std::endl;
// //       std::cout << SphericalHarmonic(l,m_).conj_complex(s_theta, M_PI) * std::sqrt( 4*M_PI / (2.0*l+1.0) ) << "  " << D_alpha(l,m,m_,s_theta) << "  " << D_complex(l,m,m_,s_theta,0,0) << std::endl;
//
//       sum += d * SphericalHarmonic(l,m_).complex(theta, phi);
//     }
//
//     assert(std::abs(sum.imag()) < 1e-4);
//
//     return sum;
//   }
//
//
//   double cross_sin_2p(int two_p, point_t s, point_t w)
//   {
//     s = s / viennagrid::norm_2(s);
//     w = w / viennagrid::norm_2(w);
//
//     return std::pow( std::sin(std::acos(viennagrid::inner_prod(s,w))), two_p );
//   }
//
//
//   double cross_sin_2p_SH(int two_p, point_t s, point_t w)
//   {
//     s = s / viennagrid::norm_2(s);
//     w = w / viennagrid::norm_2(w);
//
//     double s_theta;
//     double s_phi;
//     double s_r;
//     to_spherical(s, s_theta, s_phi, s_r);
//
// //     if (s_theta < 0)
// //     {
// //       s_theta = -s_theta;
// //       s_phi -= M_PI;
// //     }
// //
// //     while (s_phi < 0)
// //       s_phi += 2*M_PI;
//
//     double w_theta;
//     double w_phi;
//     double w_r;
//     to_spherical(w, w_theta, w_phi, w_r);
//
// //     std::cout << "s theta=" << s_theta << " phi=" << s_phi << std::endl;
// //     std::cout << "w theta=" << w_theta << " phi=" << w_phi << std::endl;
//
//     double sum = 0.0;
//
//     assert(two_p % 2 == 0);
//     int p = two_p/2;
//
//     for (int l = 0; l <= p; ++l)
//     {
//       sum += S(p,l) * SH_rotated2(2*l, 0, 0, -s_theta, -s_phi, w_theta, w_phi).real();
//
//
// //       for (int m = -2*l; m <= 2*l; ++m)
// //       {
// //         double D_ = std::sqrt( 4*M_PI / (2.0*2*l+1.0) ) * SphericalHarmonic(2*l,m).conj_complex(s_theta, M_PI).real();
// //         sum += S(p,l) * D_ * SphericalHarmonic(2*l,m).complex(w_theta, w_phi).real();
// //         std::cout << "   " << S(p,l) * D_ * SphericalHarmonic(2*l,m).complex(w_theta, w_phi).real() << std::endl;
// //       }
//     }
//
//     return sum;
//   }
//
//
//   void rotated_angle(point_t s, point_t w)
//   {
//     s = s / viennagrid::norm_2(s);
//     w = w / viennagrid::norm_2(w);
//
//     std::cout << std::acos(viennagrid::inner_prod(s,w)) << std::endl;
//
//     double s_theta;
//     double s_phi;
//     double s_r;
//     to_spherical(s, s_theta, s_phi, s_r);
//
//     double rot_theta = -s_theta;
//     double rot_phi = -s_phi;
//
// //     while (rot_theta < 0)
// //     {
// //       rot_theta = -rot_theta;
// //       rot_phi -= M_PI;
// //     }
// //
// //     while (rot_phi < 0)
// //       rot_phi += 2*M_PI;
//
//     std::cout << rot_theta << " " << rot_phi << std::endl;
//
//     ublas::matrix<double> rot = euler_angle_matrix_zyz(0, rot_theta, rot_phi);
//     point_t rotated_s = prod(rot, s);
//     point_t rotated_w = prod(rot, w);
//
//     std::cout << rotated_s << " " << rotated_w << std::endl;
// //     std::cout << prod(rot, s) << std::endl;
//
//     point_t z = viennagrid::make_point(0,0,1);
//     std::cout << std::acos(viennagrid::inner_prod(z,rotated_w)) << std::endl;
// //     std::cout << std::acos(viennagrid::inner_prod(rotated_s,rotated_w)) << std::endl;
//   }






  symmetry_detection_3d::symmetry_detection_3d() {}
  std::string symmetry_detection_3d::name() { return "symmetry_detection_3d"; }

  bool symmetry_detection_3d::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    int geometric_dimension = viennagrid::geometric_dimension( input_mesh() );
    int cell_dimension = viennagrid::cell_dimension( input_mesh() );

    if (geometric_dimension != 3)
      return false;

    if (cell_dimension != 2)
      return false;

    typedef viennagrid::mesh_t MeshType;
    typedef point_t PointType;




    MeshType input = input_mesh();

    MeshType refined1 = input.make_child();
    viennagrid::cell_refine_uniformly(input, refined1);

    MeshType refined2 = refined1.make_child();
    viennagrid::cell_refine_uniformly(refined1, refined2);

    MeshType refined3 = refined2.make_child();
    viennagrid::cell_refine_uniformly(refined2, refined3);

    MeshType refined4 = refined3.make_child();
    viennagrid::cell_refine_uniformly(refined3, refined4);





    MeshType quad_mesh;

    PointType points[8];
    points[0] = viennagrid::make_point( -1, -1, -1 );
    points[1] = viennagrid::make_point( -1,  1, -1 );
    points[2] = viennagrid::make_point(  1, -1, -1 );
    points[3] = viennagrid::make_point(  1,  1, -1 );

    points[4] = viennagrid::make_point( -1, -1,  1 );
    points[5] = viennagrid::make_point( -1,  1,  1 );
    points[6] = viennagrid::make_point(  1, -1,  1 );
    points[7] = viennagrid::make_point(  1,  1,  1 );

    typedef viennagrid::result_of::element<MeshType>::type VertexType;

    VertexType vertices[8];

    for (int i = 0; i != 8; ++i)
      vertices[i] = viennagrid::make_vertex(quad_mesh, points[i]);

    viennagrid::make_quadrilateral(quad_mesh, vertices[0], vertices[2], vertices[3], vertices[1]);
    viennagrid::make_quadrilateral(quad_mesh, vertices[4], vertices[6], vertices[7], vertices[5]);
    viennagrid::make_quadrilateral(quad_mesh, vertices[0], vertices[2], vertices[6], vertices[4]);
    viennagrid::make_quadrilateral(quad_mesh, vertices[1], vertices[5], vertices[7], vertices[3]);
    viennagrid::make_quadrilateral(quad_mesh, vertices[0], vertices[4], vertices[5], vertices[1]);
    viennagrid::make_quadrilateral(quad_mesh, vertices[2], vertices[3], vertices[7], vertices[6]);






    MeshType result_mesh = quad_mesh;


    std::ofstream file("matrix.mat");


    int theta_count = 128;
    int phi_count = 128;

    int two_p = 4;

    double theta_step = M_PI/theta_count;
    double phi_step = M_PI/phi_count;

    CachedM<MeshType> cached_m(two_p, result_mesh);

    file << "# Created by ViennaMesh" << std::endl;
    file << "# name: M" << std::endl;
    file << "# type: matrix" << std::endl;
    file << "# rows: " << phi_count+1 << std::endl;
    file << "# columns: " << theta_count+1 << std::endl;

    for (int i = 0; i <= theta_count; ++i)
    {
      for (int j = 0; j <= phi_count; ++j)
      {
// //         file << std::setw(12) << M(p, result_mesh, theta_step*i, phi_step*j) << (i == theta_count ? "\n" : " ");
// //         file << std::setw(12) << grad_M(p, result_mesh, theta_step*i, phi_step*j, 1e-2) << (i == theta_count ? "\n" : " ");
//
// //         file << std::setw(12) << M_grad_direct(p, result_mesh, theta_step*i, phi_step*j, 1e-2) << (i == theta_count ? "\n" : " ");
// //         file << std::setw(12) << M_direct(p, result_mesh, theta_step*i, phi_step*j) << (i == theta_count ? "\n" : " ");


//         double value_direct = M_direct(two_p, result_mesh, theta_step*i, phi_step*j);
//         double value = M(two_p, result_mesh, theta_step*i, phi_step*j);
//         double value_cached = cached_m(theta_step*i, phi_step*j);
//         std::cout << value_direct << " " << value << " " << value_cached << std::endl;

//         double grad_direct = M_grad_direct(two_p, result_mesh, theta_step*i, phi_step*j, 1e-2);
//         double grad = grad_M(two_p, result_mesh, theta_step*i, phi_step*j, 1e-2);
        double grad_cached = cached_m.grad(theta_step*i, phi_step*j, 1e-2);
//         std::cout << grad_direct << " " << grad << " " << grad_cached << std::endl;


        file << std::setw(12) << grad_cached << (j == phi_count ? "\n" : " ");
      }
    }

    file << "];";



//     int l = 4;
//     int m = 0;
//
//     for (int j = 0; j <= phi_count; ++j)
//     {
//       for (int i = 0; i <= theta_count; ++i)
//       {
//         double rotate_theta = 1.0;
//         double rotate_phi = 3.0;
//
//         double theta = theta_step*i;
//         double phi = phi_step*j;
//
//         double expl = SH_rotated(l,m, rotate_theta, rotate_phi, theta, phi);
//         double sh = SH_rotated2(l,m, rotate_theta, rotate_phi, theta, phi);
//
//         if ( std::abs(expl-sh) > 1e-6 )
//           std::cout << expl << "  " << sh << std::endl;
//       }
//     }



//     std::cout << SH_rotated(l,m, 1, 0, 1, 1) << std::endl;
//     std::cout << SH_rotated2(l,m, 1, 0, 1, 1) << std::endl;
//     std::cout << SphericalHarmonic(l,m)(2,1) << std::endl;




//     point_t s = viennagrid::make_point(0.5,0.5,-0.707107);
//     point_t w = viennagrid::make_point(0.5,0.5,0.707107);
//
//     std::cout << "classical = " << cross_sin_2p(p, s, w) << std::endl;
//     std::cout << "SH = " << cross_sin_2p_SH(p, s, w) << std::endl;
//
//     rotated_angle(s,w);
//
//
//     int mismatch = 0;
//
//     double step = M_PI/16.0;
//
//     for (double rot_theta = -M_PI; rot_theta <= M_PI; rot_theta += step)
//       for (double rot_phi = -2*M_PI; rot_phi <= 2*M_PI; rot_phi += step)
//         for (double theta = 0; theta <= M_PI; theta += step)
//           for (double phi = 0; phi <= 2*M_PI; phi += step)
//
// //     for (double rot_theta = 0; rot_theta <= M_PI; rot_theta += step)
// //       for (double rot_phi = 0; rot_phi <= 2*M_PI; rot_phi += step)
// //         for (double theta = 0; theta <= M_PI; theta += step)
// //           for (double phi = 0; phi <= 2*M_PI; phi += step)
//           {
//             point_t s;
//             point_t w;
//             from_spherical(rot_theta, rot_phi, 1, s);
//             from_spherical(theta, phi, 1, w);
//
//             double tmp0 = cross_sin_2p(p, s, w);
//             double tmp1 = cross_sin_2p_SH(p, s, w);
//
// //             double rot_theta_tmp = rot_theta;
// //             double rot_phi_tmp = rot_phi;
//
// //             while (rot_theta_tmp < 0)
// //             {
// //               rot_theta_tmp = -rot_theta_tmp;
// //               rot_phi_tmp -= M_PI;
// //             }
// //
// //             while (rot_phi_tmp  < 0)
// //               rot_phi_tmp += 2*M_PI;
//
// //             std::complex<double> tmp0 = SH_rotated(1, 0, 0, rot_theta, rot_phi, theta, phi);
// //             std::complex<double> tmp1 = SH_rotated2(1, 0, 0, rot_theta, rot_phi, theta, phi);
//
//
//             if (std::abs(tmp0-tmp1) > 1e-2)
//             {
//               std::cout << rot_theta << "," << rot_phi << "  " << theta << "," << phi << "    " << tmp0 << " " << tmp1 << " " << (std::abs(tmp0-tmp1) > 1e-2 ? "!!!" : "" ) << std::endl;
//             }
//
//             if (std::abs(tmp0-tmp1) > 1e-2)
//               ++mismatch;
//           }




//     for (int l = 0; l != 2; ++l)
//       for (double beta = -2*M_PI; beta <= 2*M_PI; beta += step)
//         for (double gamma = -2*M_PI; gamma <= 2*M_PI; gamma += step)
//         {
//           int two_l = 2*l;
//
//           std::complex<double> d = D_complex(two_l, 0, 0, 0, beta, gamma, 100, 200);
//
//           int m_ = 0;
//           std::complex<double> d_ = std::sqrt( 4*M_PI / (2.0*two_l+1.0) ) * SphericalHarmonic(two_l,m_).conj_complex(beta, M_PI);
//           d_ *= std::exp( std::complex<double>(0,1)*static_cast<double>(m_)*gamma );
//           d_ *= power_mone(two_l-m_);
//
//           if (std::abs(d-d_) > 1e-2)
//             std::cout << d << " " << d_ << std::endl;
//         }


//     std::cout << "MISMATCH " << mismatch << std::endl;



//     for (double beta = 0.0; beta <= 2*M_PI; beta += 2*M_PI/20.0)
//     {
//       double tmp0 = sin_2p(2, beta);
//       double tmp1 = sin_2p_SH(2, beta);
// //       double tmp2 = sin_2p_SH2(2, beta)
//
// //       double tmp0 = SH_rotated(1, 0, 1, 0, 2, beta);
// //       double tmp1 = SH_rotated2(1, 0, 1, 0, 2, beta);
//
//
//       std::cout << tmp0 << " " << tmp1 << std::endl;
//     }





    return true;
  }

}
