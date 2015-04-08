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
#include <functional>

#include "detection_3d.hpp"
// #include "viennagrid/algorithm/volume.hpp"
// #include "viennagrid/algorithm/surface.hpp"
#include "viennagridpp/algorithm/centroid.hpp"
#include "viennagridpp/algorithm/distance.hpp"
// #include "viennagrid/algorithm/refine.hpp"
#include "viennagridpp/io/vtk_writer.hpp"
#include "viennagridpp/mesh/object_creation.hpp"

// #include "spherical_harmonics.hpp"

#include <boost/numeric/ublas/matrix.hpp>
namespace ublas = boost::numeric::ublas;


namespace boost
{
  namespace numeric
  {
    namespace ublas
    {
      viennagrid::point_t prod(ublas::matrix<double> const & mat, viennagrid::point_t const & vec)
      {
        assert(mat.size2() == vec.size());
        viennagrid::point_t result( mat.size1() );
        for (std::size_t row = 0; row != mat.size2(); ++row)
        {
          result[row] = 0.0;
          for (std::size_t col = 0; col != mat.size1(); ++col)
            result[row] += mat(row, col) * vec[col];
        }
        return result;
      }
    }
  }
}


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
                         double /*alpha*/, double beta, double gamma)
  {
    assert(m == 0);

    // A signal-processing framework for reflection, doi>10.1145/1027411.1027416, (31).1
    // http://cseweb.ucsd.edu/~ravir/p1004-ramamoorthi.pdf
    std::complex<double> d_ = std::sqrt( 4*M_PI / (2.0*l+1.0) ) * SphericalHarmonic(l,m_).conj_complex(beta, M_PI);

    // A signal-processing framework for reflection, doi>10.1145/1027411.1027416, (27)
    // http://cseweb.ucsd.edu/~ravir/p1004-ramamoorthi.pdf
    d_ *= std::exp( std::complex<double>(0,1)*static_cast<double>(m_)*gamma );

    // Maple procedures for the coupling of angular momenta. IX. Wigner D-functions and rotation matrices,         doi:10.1016/j.cpc.2005.12.008, (4)
    // http://www.atomic-theory.uni-jena.de/pub/p186.b06.cpc-racahIX-original.pdf
    d_ *= power_mone(l-m_);

//     std::cout << d_ << std::endl;

    return d_;
  }


  std::complex<double> C_(int two_l, int m, int two_p, point_t const & s)
  {
    if (two_l%2 != 0)
      abort();

    if (two_p%2 != 0)
      abort();

    double s_theta;
    double s_phi;
    double s_r;
    to_spherical(s, s_theta, s_phi, s_r);

    std::complex<double> D_ = D(two_l, 0, m, 0, -s_theta, -s_phi);

    return std::pow(viennagrid::norm_2(s), two_p) * D_;//  D(0, m, l, s);
  }


  std::complex<double> integrate(viennagrid::element_t const & element,
                                 std::function< std::complex<double> (point_t const &) > const & f,
                                 int N)
  {
    point_t A = viennagrid::get_point(element, 0);
    point_t B = viennagrid::get_point(element, 1);
    point_t C = viennagrid::get_point(element, 2);

    int base = N*3;
    std::complex<double> result = 0.0;

    for (int row = 0; row != N; ++row)
    {
      for (int column = 0; column != 2*row+1; ++column)
      {
        int a = (base-2) - 3*row + (column % 2 == 0 ? 0 : 1);
        int b =(column % 2 == 0) ? (1+column/2*3) : ((column+1)/2*3-1);
        int c = base - a - b;

        point_t pt = (A*a + B*b + C*c) / base;
        result += f(pt);
      }
    }

    return result / static_cast<double>(N*N) * viennagrid::volume(element);
  }


  std::complex<double> integrate(viennagrid::element_t const & element,
                                 std::function< std::complex<double> (point_t const &) > const & f,
                                 double relative_error, int max_iterations)
  {
    std::complex<double> prev_result = integrate(element, f, 1);
//     std::cout << "relative integrate" << std::endl;

    for (int N = 2; N != max_iterations; ++N)
    {
      std::complex<double> result = integrate(element, f, N);

      double error;
      if ( std::abs(result) > std::abs(prev_result) )
        error = std::abs( (result - prev_result) / result );
      else
        error = std::abs( (prev_result - result) / prev_result );

//       std::cout << "  N = " << N << "  prev_result = " << prev_result << "  result = " << result << "   error = " << error << std::endl;

      if (error < relative_error)
        return result;

      prev_result = result;
    }

    return prev_result;
  }



  template<bool mesh_is_const>
  std::complex<double> C(int two_l, int m, int two_p,
                         viennagrid::base_mesh<mesh_is_const> const & mesh, double integrate_tolerance)
  {
    typedef viennagrid::base_mesh<mesh_is_const> MeshType;
    typedef typename viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRange;
    typedef typename viennagrid::result_of::iterator<ConstCellRange>::type ConstCellIterator;

    if (two_l%2 != 0)
      abort();

    if (two_p%2 != 0)
      abort();

    std::complex<double> result = 0.0;
//     std::complex<double> result_2 = 0.0;
    int l = two_l/2;
    int p = two_p/2;


    std::function< std::complex<double> (point_t const &) > cf =
        std::bind< std::complex<double> >(C_, two_l, m, two_p, std::placeholders::_1);


    ConstCellRange cells( mesh );
    for (ConstCellIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      result += integrate( *cit, cf, integrate_tolerance, 10 );
    }

    return result * S(p,l);
  }


  template<typename MeshT>
  class CachedC
  {
  public:

    CachedC(int two_p_,
            MeshT const & mesh,
            double integrate_tolerance_in) : mesh_(mesh), integrate_tolerance_(integrate_tolerance_in), p(two_p_/2), min_l(0), max_l(p), min_m(-max_l*2), max_m(max_l*2)
    {}

    double operator()(double theta, double phi) const
    {
      std::complex<double> sum = 0.0;
      for (int l = 0; l <= p; ++l)
      {
        std::cout << "  2*l = " << 2*l << std::endl;
        for (int m = -2*l; m <= 2*l; ++m)
        {
          std::cout << "   m = " << m << "  " << this->C(2*l, m) << "  " << SphericalHarmonic(2*l,m).complex(theta, phi) << "          " << this->C(2*l, m) * SphericalHarmonic(2*l,m).complex(theta, phi) << std::endl;

          sum += this->C(2*l, m) * SphericalHarmonic(2*l,m).complex(theta, phi);
        }
        std::cout << std::endl << std::endl;
      }

      std::cout << sum << std::endl;

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


    void print() const
    {
      for (std::map< std::pair<int, int>, std::complex<double> >::const_iterator it = values.begin();
                                                                                 it != values.end();
                                                                               ++it)
      {
        std::cout << "2*l = " << it->first.first << " m = " << it->first.second << " 2*p = " << 2*p << "    " << it->second << std::endl;
      }
    }

    std::complex<double> C(int two_l, int m) const
    {
      std::map< std::pair<int, int>, std::complex<double> >::iterator it = values.find( std::make_pair(two_l,m) );
      if (it != values.end())
        return it->second;

      std::complex<double> tmp = viennamesh::C(two_l, m, 2*p, mesh_, integrate_tolerance_);
      values.insert( std::make_pair( std::make_pair(two_l,m), tmp ) );
      return tmp;
    }

  private:

    MeshT const & mesh_;
    double integrate_tolerance_;
    mutable std::map< std::pair<int, int>, std::complex<double> > values;

    int p;
    int min_l;
    int max_l;
    int min_m;
    int max_m;
  };

}




viennagrid::point_t reflect(viennagrid::point_t const & pt, viennagrid::point_t const & axis)
{
  return pt - 2.0 * axis * viennagrid::inner_prod(pt, axis);
}


template<bool mesh_is_const>
double distance(viennagrid::base_mesh<mesh_is_const> const & mesh, viennagrid::point_t const & pt)
{
  typedef viennagrid::base_mesh<mesh_is_const> MeshType;
  typedef typename viennagrid::result_of::point<MeshType>::type PointType;

  typedef typename viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRangeType;
  typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;

  ConstCellRangeType cells(mesh);
  if (cells.empty())
    return -1;

  ConstCellRangeIterator cit = cells.begin();
  double d = viennagrid::distance(*cit, pt);
  ++cit;

  for (; cit != cells.end(); ++cit)
  {
    double tmp = viennagrid::distance(*cit, pt);
    d = std::min(d, tmp);
  }

  return d;
}


template<bool mesh_is_const>
bool check_mirror(viennagrid::base_mesh<mesh_is_const> const & mesh,
                  viennagrid::point_t axis,
                  double tolerance)
{
  axis /= viennagrid::norm_2(axis);

  typedef viennagrid::base_mesh<mesh_is_const> MeshType;
  typedef typename viennagrid::result_of::point<MeshType>::type PointType;

  typedef typename viennagrid::result_of::const_vertex_range<MeshType>::type ConstVertexRangeType;
  typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;

  double max = -1.0;
  ConstVertexRangeType vertices(mesh);
  for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
  {
    PointType reflected = reflect( viennagrid::get_point(*vit), axis );
    double d = distance(mesh, reflected);
    max = std::max(max, d);
  }

  std::cout << "Max distance = " << max  << std::endl;
  return max < tolerance;
}



template<bool mesh_is_const>
bool check_rotational(viennagrid::base_mesh<mesh_is_const> const & mesh,
                      viennagrid::point_t axis, int frequency,
                      double tolerance)
{
  axis /= viennagrid::norm_2(axis);

  double angle = 2*M_PI / frequency;
  double cos_angle = std::cos(angle);
  double sin_angle = std::sin(angle);

  ublas::matrix<double> rot(3,3);

  rot(0,0) = axis[0]*axis[0] * (1-cos_angle) + cos_angle;
  rot(0,1) = axis[0]*axis[1] * (1-cos_angle) - axis[2]*sin_angle;
  rot(0,2) = axis[0]*axis[2] * (1-cos_angle) + axis[1]*sin_angle;

  rot(1,0) = axis[1]*axis[0] * (1-cos_angle) + axis[2]*sin_angle;
  rot(1,1) = axis[1]*axis[1] * (1-cos_angle) + cos_angle;
  rot(1,2) = axis[1]*axis[2] * (1-cos_angle) - axis[0]*sin_angle;

  rot(2,0) = axis[2]*axis[0] * (1-cos_angle) - axis[1]*sin_angle;
  rot(2,1) = axis[2]*axis[1] * (1-cos_angle) + axis[0]*sin_angle;
  rot(2,2) = axis[2]*axis[2] * (1-cos_angle) + cos_angle;


  typedef viennagrid::base_mesh<mesh_is_const> MeshType;
  typedef typename viennagrid::result_of::point<MeshType>::type PointType;

  typedef typename viennagrid::result_of::const_vertex_range<MeshType>::type ConstVertexRangeType;
  typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;

  double max = -1.0;
  ConstVertexRangeType vertices(mesh);
  for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
  {
    PointType reflected = ublas::prod(rot, viennagrid::get_point(*vit));
    double d = distance(mesh, reflected);
    max = std::max(max, d);
  }

  std::cout << "Max distance = " << max  << std::endl;
  return max < tolerance;
}





namespace viennamesh
{
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
    typedef viennagrid::result_of::element<MeshType>::type ElementType;

    typedef viennagrid::result_of::const_vertex_range<MeshType>::type ConstVertexRangeType;
    typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;

    typedef viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRangeType;
    typedef viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;


    MeshType mesh = input_mesh();

    CachedC<MeshType> cached_c_2(2, mesh, 1e-6);
    CachedC<MeshType> cached_c_4(4, mesh, 1e-6);
    CachedC<MeshType> cached_c_6(6, mesh, 1e-6);


    double sphere_radius = 1.0;
    if (get_input<double>("sphere_radius").valid())
      sphere_radius = get_input<double>("sphere_radius")();

    MeshType sphere;
    viennagrid::make_sphere_hull( sphere, viennagrid::make_point(0,0,0), sphere_radius, 4 );

    viennagrid::quantity_field gradient_field(0, 1);
    gradient_field.set_name("gradient");

    ConstVertexRangeType vertices(sphere);
    for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      PointType const & pt = viennagrid::get_point(*vit);

      double theta;
      double phi;
      double r;
      to_spherical(pt, theta, phi, r);

      double grad = cached_c_6.grad(theta, phi, 1e-2);

      gradient_field.set(*vit, grad);
    }



    cached_c_6.print();







//     std::cout << "Check mirror (1,0,0) = " << std::boolalpha << check_mirror(mesh, viennagrid::make_point(1,0,0), 1e-6) << std::endl;
//     std::cout << "Check mirror (0,1,0) = " << std::boolalpha << check_mirror(mesh, viennagrid::make_point(0,1,0), 1e-6) << std::endl;
//     std::cout << "Check mirror (0,0,1) = " << std::boolalpha << check_mirror(mesh, viennagrid::make_point(0,0,1), 1e-6) << std::endl;
//
//     std::cout << "Check mirror (1,1,1) = " << std::boolalpha << check_mirror(mesh, viennagrid::make_point(1,1,0), 1e-6) << std::endl;



//     std::cout << "Check rotational (1,0,0), 2 = " << std::boolalpha << check_rotational(mesh, viennagrid::make_point(1,0,0), 2, 1e-6) << std::endl;
//     std::cout << "Check rotational (1,0,0), 3 = " << std::boolalpha << check_rotational(mesh, viennagrid::make_point(1,0,0), 3, 1e-6) << std::endl;
//     std::cout << "Check rotational (1,0,0), 4 = " << std::boolalpha << check_rotational(mesh, viennagrid::make_point(1,0,0), 4, 1e-6) << std::endl;
//     std::cout << "Check rotational (1,0,0), 5 = " << std::boolalpha << check_rotational(mesh, viennagrid::make_point(1,0,0), 5, 1e-6) << std::endl;




    set_output("mesh", sphere);
    set_output("quantities", gradient_field);

    return true;
  }

}
