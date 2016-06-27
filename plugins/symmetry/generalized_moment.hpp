#ifndef VIENNAMESH_ALGORITHM_SYMMETRY_GENERALIZED_MOMENT_HPP
#define VIENNAMESH_ALGORITHM_SYMMETRY_GENERALIZED_MOMENT_HPP

#include "common.hpp"
#include "integrate.hpp"

namespace viennamesh
{



  template<typename T, int N>
  class static_polynom : public boost::array<T, ((N+1<0)?0:(N+1))>
  {
  public:

    typedef static_polynom<T,N> self_type;

    std::size_t grad() const
    {
      return this->size()-1;
    }

    template<typename U>
    U operator()(U value) const
    {
      U sum = (*this)[0];
      U cur = value;

      for (std::size_t i = 1; i <= grad(); ++i, cur *= value)
        sum += cur*(*this)[i];
      return sum;
    }

    float eval_float(float value) const
    {
      return (*this)(value);
    }

    double eval_float(double value) const
    {
      return (*this)(value);
    }


    self_type operator-() const
    {
      self_type tmp = *this;
      for (std::size_t i = 0; i <= tmp.grad(); ++i)
        tmp[i] = -tmp[i];
      return tmp;
    }


    self_type & operator+=(T const & lhs)
    {
      (*this)[0] += lhs;
      return *this;
    }

    self_type & operator-=(T const & lhs)
    {
      (*this)[0] -= lhs;
      return *this;
    }

    self_type & operator+=(self_type const & lhs)
    {
      for (std::size_t i = 0; i <= grad(); ++i)
        (*this)[i] += lhs[i];

      return *this;
    }

    self_type & operator-=(self_type const & lhs)
    {
      if (grad() < lhs.grad())
        resize(lhs.grad());

      for (std::size_t i = 0; i <= grad(); ++i)
         (*this)[i] -= lhs[i];

      return *this;
    }



    self_type & operator*=(T const & scalar)
    {
      for (std::size_t i = 0; i <= grad(); ++i)
        (*this)[i] *= scalar;
      return *this;
    }

    self_type & operator/=(T const & scalar)
    {
      for (std::size_t i = 0; i <= grad(); ++i)
        (*this)[i] /= scalar;
      return *this;
    }
  };


  template<typename T, int N>
  std::ostream & operator<<(std::ostream & stream, static_polynom<T,N> const & poly)
  {
    stream << "[ ";
    for (std::size_t i = 0; i <= poly.grad(); ++i)
    {
      int n = poly.grad()-i;

      stream << " ";
      if (i != 0 && poly[n] >= 0)
        stream << "+";
      stream << poly[n];

      if (n != 0)
      {
        stream << "*x";
        if (n != 1)
          stream << "^" << n;
      }
    }
    stream << " ]";
    return stream;
  }





  template<typename T, int N>
  static_polynom<T,N> operator+(static_polynom<T,N> const & lhs, static_polynom<T,N> const & rhs)
  {
    static_polynom<T,N> result = lhs;
    result += rhs;
    return result;
  }

  template<typename T, int N>
  static_polynom<T,N> operator-(static_polynom<T,N> const & lhs, static_polynom<T,N> const & rhs)
  {
    static_polynom<T,N> result = lhs;
    result -= rhs;
    return result;
  }




  template<typename T, int N>
  static_polynom<T,N> operator*(static_polynom<T,N> const & lhs, T const & rhs)
  {
    static_polynom<T,N> result = lhs;
    result *= rhs;
    return result;
  }

  template<typename T, int N>
  static_polynom<T,N> operator*(T const & lhs, static_polynom<T,N> const & rhs)
  {
    static_polynom<T,N> result = rhs;
    result *= lhs;
    return result;
  }


  template<typename T, int N>
  static_polynom<T,N> operator/(static_polynom<T,N> const & lhs, T const & rhs)
  {
    static_polynom<T,N> result = lhs;
    result /= rhs;
    return result;
  }





  template<typename T, int N>
  static_polynom<T,N> operator+(static_polynom<T,N> const & lhs, T const & rhs)
  {
    static_polynom<T,N> result = lhs;
    result += rhs;
    return result;
  }

  template<typename T, int N>
  static_polynom<T,N> operator+(T const & lhs, static_polynom<T,N> const & rhs)
  {
    static_polynom<T,N> result = rhs;
    result += lhs;
    return result;
  }


  template<typename T, int N>
  static_polynom<T,N> operator-(static_polynom<T,N> const & lhs, T const & rhs)
  {
    static_polynom<T,N> result = lhs;
    result -= rhs;
    return result;
  }

  template<typename T, int N>
  static_polynom<T,N> operator-(T const & lhs, static_polynom<T,N> const & rhs)
  {
    static_polynom<T,N> result = rhs;
    result -= lhs;
    return result;
  }



  template<typename T, int N>
  static_polynom<T,N> static_jacobi_polynom(int a, int b)
  {
    polynom<T> poly = jacobi_polynom<T>(N, a, b);
    static_polynom<T,N> result;
    std::copy(poly.begin(), poly.end(), result.begin());
    return result;
  }




  template<typename T>
  class base_static_C
  {
  public:
    typedef T result_type;

    base_static_C() : call_count_(0) {}
    virtual ~base_static_C<T>() {}
    virtual T operator()(double x, double y, double z) const = 0;
    T operator()(point const & s) const
    {
      return (*this)(s[0], s[1], s[2]);
    }

    virtual T integrate(viennagrid::element const & element, double relative_error, double absolute_error, int max_iterations) const = 0;

    int call_count() const { return call_count_; }

  protected:
    mutable int call_count_;
  };




  template<typename T, int l, int m_>
  class static_C : public base_static_C<T>
  {
  public:

    static const int m = (m_>0?m_:-m_);
    static const bool sign_m = m_ > 0;

    static_C(int p_in) : p(p_in)
    {
      J = static_jacobi_polynom<T,l-m>(m,m);

      for (std::size_t i = 0; i <= J.grad(); ++i)
        J[i] = (power_mone(m) + power_mone(i)) * J[i];

      factor = std::sqrt( (factorial(l+m)*factorial(l-m)) / (factorial(l)*factorial(l)) ) *
               std::pow(1.0/2.0, m) * (1.0 / std::sqrt(2));
    }

    T integrate(viennagrid::element const & element, double relative_error, double absolute_error, int max_iterations) const
    {
      return viennamesh::integrate(element, *this, relative_error, absolute_error, max_iterations);
    }





    T operator()(double x, double y, double z) const
    {
      ++this->call_count_;
      T r = std::sqrt(x*x+y*y+z*z);
      T cos_theta = z/r;

      T result;

      if (m == 0)
        result = J(cos_theta)/2;
      else
      {
        T sin_theta = std::sqrt(1-cos_theta*cos_theta);
        T phi = atan2(y,x);
        result = J(cos_theta) * (sign_m ? std::cos(m*phi) : std::sin(m*phi)) * factor * std::pow(sin_theta, m);
      }

      return std::pow(r, p) * result;
    }



  private:

    T factor;
    int p;
    static_polynom<T, l-m> J;
  };


  template<typename T, int l>
  base_static_C<T> * make_static_C(int m, int p)
  {
    switch (m)
    {
      case -8:
        return new static_C<T, l,-4>(p);
      case -7:
        return new static_C<T, l,-4>(p);
      case -6:
        return new static_C<T, l,-4>(p);
      case -5:
        return new static_C<T, l,-4>(p);
      case -4:
        return new static_C<T, l,-4>(p);
      case -3:
        return new static_C<T, l,-3>(p);
      case -2:
        return new static_C<T, l,-2>(p);
      case -1:
        return new static_C<T, l,-1>(p);
      case  0:
        return new static_C<T, l, 0>(p);
      case  1:
        return new static_C<T, l, 1>(p);
      case  2:
        return new static_C<T, l, 2>(p);
      case  3:
        return new static_C<T, l, 3>(p);
      case  4:
        return new static_C<T, l, 4>(p);
      case  5:
        return new static_C<T, l, 4>(p);
      case  6:
        return new static_C<T, l, 4>(p);
      case  7:
        return new static_C<T, l, 4>(p);
      case  8:
        return new static_C<T, l, 4>(p);
    }

    return 0;
  }


  template<typename T>
  base_static_C<T> * make_static_C(int l, int m, int p)
  {
    switch(l)
    {
      case 0:
        return make_static_C<T, 0>(m,p);
      case 1:
        return make_static_C<T, 1>(m,p);
      case 2:
        return make_static_C<T, 2>(m,p);
      case 3:
        return make_static_C<T, 3>(m,p);
      case 4:
        return make_static_C<T, 4>(m,p);
      case 5:
        return make_static_C<T, 4>(m,p);
      case 6:
        return make_static_C<T, 4>(m,p);
      case 7:
        return make_static_C<T, 4>(m,p);
      case 8:
        return make_static_C<T, 4>(m,p);
    }

    return 0;
  }









  class C_cached
  {
  public:

    C_cached(int l_in, int m_in, int p_in) : l(l_in), sign_m(m_in>0), m(std::abs(m_in)), p(p_in)
    {
      J = jacobi_polynom<double>(l-m,m,m);

      for (std::size_t i = 0; i <= J.grad(); ++i)
        J[i] = (power_mone(m) + power_mone(i)) * J[i];

      factor = std::sqrt( (factorial(l+m)*factorial(l-m)) / (factorial(l)*factorial(l)) ) *
               std::pow(1.0/2.0, m) * (1.0 / std::sqrt(2));
    }

    typedef double NumericType;
    typedef polynom<double> PolynomType;
    typedef viennagrid::point PointType;

    typedef NumericType result_type;


    NumericType operator()(PointType const & s) const
    {
      return (*this)(s[0], s[1], s[2]);
    }

    NumericType operator()(NumericType x, NumericType y, NumericType z) const
    {
      NumericType r = std::sqrt(x*x+y*y+z*z);
      NumericType cos_theta = z/r;

      NumericType result;

      if (m == 0)
        result = J(cos_theta)/2;
      else
      {
        NumericType sin_theta = std::sqrt(1-cos_theta*cos_theta);
        NumericType phi = atan2(y,x);
        result = J(cos_theta) * (sign_m ? std::cos(m*phi) : std::sin(m*phi)) * factor * std::pow(sin_theta, m);
      }
//       else if (m % 2 == 0)
//       {
//         NumericType sin_theta = std::sqrt(1-cos_theta*cos_theta);
//         NumericType phi = atan2(y,x);
//         result = J.eval_even(cos_theta) * (sign_m ? std::cos(m*phi) : std::sin(m*phi)) * factor * std::pow(sin_theta, m);
//       }
//       else
//       {
//         NumericType sin_theta = std::sqrt(1-cos_theta*cos_theta);
//         NumericType phi = atan2(y,x);
//         result = J.eval_odd(cos_theta) * (sign_m ? std::cos(m*phi) : std::sin(m*phi)) * factor * std::pow(sin_theta, m);
//       }

//       else if (!sign_m)
//       {
//         result = (power_mone(m) * J(cos_theta) + J(-cos_theta) ) * sin_mphi * factor * std::pow(sin_theta, m);
//       }
//       else
//       {
//         result = (power_mone(m) * J(cos_theta) + J(-cos_theta) ) * cos_mphi * factor * std::pow(sin_theta, m);
//       }
//       else
//       {
//         std::complex<double> tmp =
//               ( power_mone(m) * J(cos_theta) * std::complex<double>(cos_mphi, -sin_mphi) +
//            (sign_m ? 1 : -1) * J(-cos_theta) * std::complex<double>(cos_mphi, sin_mphi) ) *
//            (sign_m ? 1 : std::complex<double>(0,1)) * factor * std::pow(sin_theta, m);
//
//         result = tmp.real();
//       }

      return std::pow(r, p) * result;
    }

  private:

    int l;
    bool sign_m;
    int m;
    int p;

    NumericType factor;

    PolynomType J;
  };



  template<typename T>
  class C_to_integrate
  {
  public:

    C_to_integrate(int two_l, int m, int two_p) : two_l(two_l), m(m), two_p(two_p) {}

    typedef double result_type;

    double operator()(point const & s) const
    {
      assert(two_l%2 == 0);
      assert(two_p%2 == 0);

//       T tmp0 = std::pow(viennagrid::norm_2(s), two_p) * D<T>(two_l, 0, m, s);
//       T tmp1 = C_cached(two_l,m,two_p)(s[0],s[1],s[2]);

//       base_static_C<T> * cf_ptr = make_static_C<T>(two_l, m, two_p);
//       T tmp1 = (*cf_ptr)(s[0],s[1],s[2]);
//       delete cf_ptr;
//
//       if (std::abs(tmp0-tmp1) > 1e-6)
//         std::cout << "ERROR!!!!  " << two_l << "," << m << "      "  << tmp0 << "   " << tmp1 << std::endl;

      return std::pow(viennagrid::norm_2(s), two_p) * D<T>(two_l, 0, m, s);
    }

    double operator()(double x, double y, double z) const
    {
      return (*this)(viennagrid::make_point(x,y,z));
    }

  private:
    int two_l;
    int m;
    int two_p;
  };


//   template<typename T>
//   T C_to_integrate(int two_l, int m, int two_p, viennagrid::point_t const & s)
//   {
//
//   }


//   template<typename T>
//   T C_to_integrate(int two_l, int m, int two_p, point_t const & s)
//   {
//     assert(two_l%2 == 0);
//     assert(two_p%2 == 0);
//
//     T tmp0 = std::pow(viennagrid::norm_2(s), two_p) * D<T>(two_l, 0, m, s);
//     T tmp1 = C_cached(two_l,m,two_p)(s[0],s[1],s[2] );
//
//     if (std::abs(tmp0-tmp1) > 1e-6)
//       std::cout << "ERROR!!!!  " << two_l << "," << m << "      "  << tmp0 << "   " << tmp1 << std::endl;
//
//     return std::pow(viennagrid::norm_2(s), two_p) * D<T>(two_l, 0, m, s);
//   }


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


  template<typename T, bool mesh_is_const>
  T C(int two_l, int m, int two_p,
      viennagrid::base_mesh<mesh_is_const> const & mesh)
//   , double relative_integrate_tolerance, double absolute_integrate_tolerance, int max_integrate_iterations)
  {
    typedef viennagrid::base_mesh<mesh_is_const> MeshType;
    typedef typename viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRange;
    typedef typename viennagrid::result_of::iterator<ConstCellRange>::type ConstCellIterator;

    if (two_l%2 != 0)
      abort();

    if (two_p%2 != 0)
      abort();

    T result = 0.0;
    int l = two_l/2;
    int p = two_p/2;



    base_static_C<double> * cf_ptr = make_static_C<double>(two_l, m, two_p);
    base_static_C<double> & cf = *cf_ptr;


    triangle_quadrature< triangle_gauss_weights_generator<double, 20> > quadrature;

//     function< T (point_t const &) > cf = bind< T >(C_to_integrate<T>, two_l, m, two_p, _1);

//     C_to_integrate<float> cf(two_l, m, two_p);
//     C_cached cf(two_l, m, two_p);

    ConstCellRange cells( mesh );
    for (ConstCellIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
//       result += cf_ptr->integrate(*cit, relative_integrate_tolerance, absolute_integrate_tolerance, max_integrate_iterations);


//       T tmp0 = quadrature(*cit, cf);
//       T tmp1 = integrate( *cit, cf, relative_integrate_tolerance, absolute_integrate_tolerance, max_integrate_iterations );

//       if (std::abs(tmp0-tmp1) > 1e-6)
//         std::cout << "Integrate: " << tmp0 << " " << tmp1 << "      " << std::abs(tmp0-tmp1) << std::endl;

//       result += integrate( *cit, cf, relative_integrate_tolerance, absolute_integrate_tolerance, max_integrate_iterations );
//       result += weight_integrate< triangle_gauss_weights_generator<double,20> >(*cit, cf);

      result += quadrature(*cit, cf);
    }

//     std::cout << "C(" << two_l << "," << m << "," << two_p << ") has been called " << cf.call_count() << " times" << std::endl;

    delete cf_ptr;

    return result * S(p,l);
  }


  template<typename T>
  T real(T val) { return val; }
  template<typename T>
  T real(std::complex<T> val) { return val.real(); }


  template<typename CT>
  class GeneralizedMoment
  {
  public:

    typedef CT CType;

    template<typename MeshT>
    GeneralizedMoment(int two_p_,
            MeshT const & mesh)
//             double relative_integrate_tolerance, double absolute_integrate_tolerance, int max_integrate_iterations)
    {
      assert(two_p_ % 2 == 0);
      set_p(two_p_/2);

      for (int l = 0; l <= p(); ++l)
        for (int m = -2*l; m <= 2*l; ++m)
        {
          values[l][m+2*l] = viennamesh::C<CT>(2*l, m, 2*p(), mesh);
//                                                 ,
//                                                relative_integrate_tolerance,
//                                                absolute_integrate_tolerance,
//                                                max_integrate_iterations);

//           std::cout << "C(" << 2*l << "," << m << ") = " << values[l][m+2*l] << std::endl;
        }

    }

    double operator()(double theta, double phi) const
    {
      CType sum = 0.0;
      for (int l = 0; l <= p(); ++l)
      {
        for (int m = -2*l; m <= 2*l; ++m)
        {
          sum += this->C(2*l, m) * SphericalHarmonic<CT>(2*l,m)(theta, phi);
        }
      }

      return real(sum);
    }

    double operator()(point const & pt) const
    {
      double theta;
      double phi;
      double r;
      to_spherical(pt, theta, phi, r);
      return (*this)(theta, phi);
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

    double grad(point const & pt, double eps) const
    {
      double theta;
      double phi;
      double r;
      to_spherical(pt, theta, phi, r);
      return grad(theta, phi, eps);
    }


    void print() const
    {
      for (int l = 0; l <= p(); ++l)
      {
        std::cout << "2l = " << 2*l << std::endl;
        for (int m = -2*l; m <= 2*l; ++m)
        {
          std::cout << "     C(" << 2*l << "," << m << ") = " << C(2*l,m) << std::endl;
        }
        std::cout << std::endl << std::endl;
      }
    }

    CType C(int two_l, int m) const
    {
      assert(two_l % 2 == 0);
      return values[two_l/2][m+two_l];
    }

    int p() const { return p_; }

    GeneralizedMoment get_rotated(double alpha, double beta, double gamma) const
    {
      GeneralizedMoment tmp;
      tmp.set_p( p() );

      for (int l = 0; l <= p() ; ++l)
      {
        for (int m_ = -2*l; m_ <= 2*l; ++m_)
        {
          double sum = 0.0;

          for (int m = -2*l; m <= 2*l; ++m)
            sum += C(2*l,m) * D<CT>(2*l, m_, m, alpha, beta, gamma);

          tmp.values[l][m_+2*l] = sum;
        }
      }

      return tmp;
    }

    GeneralizedMoment get_rotated(point new_z) const
    {
      double theta;
      double phi;
      double r;
      to_spherical(new_z, theta, phi, r);

      return get_rotated(0, theta, phi);
    }



    bool z_mirror_symmetry(double tolerance) const
    {
      for (int l = 0; l <= p(); ++l)
      {
        for (int m = -2*l+1; m <= 2*l-1; m+=2)
        {
          if ( std::abs(C(2*l,m)) > tolerance )
            return false;
        }
      }

      return true;
    }


    double check_rotation_symmetry(double alpha) const
    {
      double max_error = -1.0;
      for (int l = p(); l >= 0 ; --l)
      {
        for (int m = 2*l; m >= 0; --m)
        {
          double error = std::abs(std::cos(m*alpha)*C(2*l,m) - std::sin(m*alpha)*C(2*l,-m) - C(2*l,m));
          max_error = std::max(error, max_error);
        }
      }

      return max_error;
    }

    void rotation_symmetry_angles(/*double tolerance*/) const
    {
      for (int l = p(); l >= 0 ; --l)
      {
        for (int m = 2*l; m >= 0; --m)
        {
          if (m == 0)
            continue;

//           if (std::abs( C(2*l,m) ) > tolerance)
          {
            double angle = std::abs(2.0 / m * atan( C(2*l,-m) / C(2*l,m) ));
//             int frequency = 2*M_PI/angle + 0.5;

//             if ( (frequency > 1) && (std::abs(2*M_PI/angle-frequency) < tolerance) )
              std::cout << "(" << 2*l << "," << m << ") Found angle: " << angle << "      rot_frequ = " << 2*M_PI/angle << "      " << C(2*l,-m) <<
            "/" << C(2*l,m) << std::endl;
          }
        }
      }
    }


  private:

    void set_p(int p_in)
    {
      values.clear();

      p_ = p_in;
      values.resize(p()+1);
      for (int l = 0; l <= p(); ++l)
        values[l].resize( 4*l+1 );
    }

    GeneralizedMoment() {}

    std::vector< std::vector<CType> > values;

    int p_;
  };

  typedef GeneralizedMoment<double> RealGeneralizedMoment;
  typedef GeneralizedMoment< std::complex<double> > ComplexGeneralizedMoment;

}













#endif
