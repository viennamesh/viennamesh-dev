#ifndef VIENNAMESH_ALGORITHM_SYMMETRY_COMMON_HPP
#define VIENNAMESH_ALGORITHM_SYMMETRY_COMMON_HPP


#include <boost/numeric/ublas/matrix.hpp>
namespace ublas = boost::numeric::ublas;


namespace boost
{
  namespace numeric
  {
    namespace ublas
    {
      viennagrid::point prod(ublas::matrix<double> const & mat, viennagrid::point const & vec)
      {
        assert(mat.size2() == vec.size());
        viennagrid::point result( mat.size1() );
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
  void from_spherical(double theta, double phi, double r, point & result)
  {
    result.resize(3);
    result[0] = r * std::sin(theta) * std::cos(phi);
    result[1] = r * std::sin(theta) * std::sin(phi);
    result[2] = r * std::cos(theta);
  }

  void to_spherical(point const & p, double & theta, double & phi, double & r)
  {
    r = std::sqrt( p[0]*p[0] + p[1]*p[1] + p[2]*p[2] );
    theta = std::acos( p[2] / r );
    phi = atan2(p[1],p[0]);
  }

  ublas::matrix<double> rotation_x(double alpha)
  {
    ublas::matrix<double> R = ublas::identity_matrix<double>(3);
    R(1,1) = std::cos(alpha);
    R(1,2) = -std::sin(alpha);
    R(2,1) = std::sin(alpha);
    R(2,2) = std::cos(alpha);
    return R;
  }

  ublas::matrix<double> rotation_y(double alpha)
  {
    ublas::matrix<double> R = ublas::identity_matrix<double>(3);
    R(0,0) = std::cos(alpha);
    R(0,2) = std::sin(alpha);
    R(2,0) = -std::sin(alpha);
    R(2,2) = std::cos(alpha);
    return R;
  }

  ublas::matrix<double> rotation_z(double alpha)
  {
    ublas::matrix<double> R = ublas::identity_matrix<double>(3);
    R(0,0) = std::cos(alpha);
    R(0,1) = -std::sin(alpha);
    R(1,0) = std::sin(alpha);
    R(1,1) = std::cos(alpha);
    return R;
  }

  ublas::matrix<double> euler_rotation_zyz(double alpha, double beta, double gamma)
  {
    ublas::matrix<double> tmp = ublas::prod( rotation_y(beta), rotation_z(gamma) );
    return ublas::prod( rotation_z(alpha), tmp );
  }


  // http://de.wikipedia.org/wiki/Drehmatrix#Drehmatrizen_des_Raumes_R.C2.B3
  ublas::matrix<double> rotation(point axis, double alpha)
  {
    axis /= viennagrid::norm_2(axis);

    double n1 = axis[0];
    double n2 = axis[1];
    double n3 = axis[2];

    double cos_a = std::cos(alpha);
    double sin_a = std::sin(alpha);

    ublas::matrix<double> R = ublas::zero_matrix<double>(3);

    R(0,0) = n1*n1*(1-cos_a) + cos_a;
    R(0,1) = n1*n2*(1-cos_a) - n3*sin_a;
    R(0,2) = n1*n3*(1-cos_a) + n2*sin_a;

    R(1,0) = n2*n1*(1-cos_a) + n3*sin_a;
    R(1,1) = n2*n2*(1-cos_a) + cos_a;
    R(1,2) = n2*n3*(1-cos_a) - n1*sin_a;

    R(2,0) = n3*n1*(1-cos_a) - n2*sin_a;
    R(2,1) = n3*n2*(1-cos_a) + n1*sin_a;
    R(2,2) = n3*n3*(1-cos_a) + cos_a;

    return R;
  }

  std::pair<double, double> prod(ublas::matrix<double> const & rot, double theta, double phi)
  {
    point pt;
    from_spherical(theta, phi, 1.0, pt);
    pt = ublas::prod(rot, pt);

    std::pair<double, double> result;
    double tmp;
    to_spherical( pt, result.first, result.second, tmp );
    return result;
  }





















  template<typename CoefficientT>
  class polynom : public std::vector<CoefficientT>
  {
  public:

    polynom() : std::vector<CoefficientT>(1,0) {}

//     CoefficientT & operator[](std::size_t i)
//     {
//       return coeffiecients_[i];
//     }
//
//     CoefficientT operator[](std::size_t i) const
//     {
//       return coeffiecients_[i];
//     }

    std::size_t grad() const
    {
      return this->size()-1;
    }

    void set_grad(std::size_t grad)
    {
      this->resize(grad+1, 0);
    }



    template<typename T>
    T operator()(T value) const
    {
      T sum = (*this)[0];
      T cur = value;

      for (std::size_t i = 1; i <= grad(); ++i, cur *= value)
        sum += cur*(*this)[i];
      return sum;
    }

    template<typename T>
    T eval_even(T value) const
    {
      T sum = (*this)[0];
      T cur = value*value;

      for (std::size_t i = 2; i <= grad(); i += 2, cur *= value*value)
        sum += cur*(*this)[i];
      return sum;
    }

    template<typename T>
    T eval_odd(T value) const
    {
      T sum = 0;
      T cur = value;

      for (std::size_t i = 1; i <= grad(); i += 2, cur *= value*value)
        sum += cur*(*this)[i];
      return sum;
    }




    polynom<CoefficientT> operator-() const
    {
      polynom<CoefficientT> tmp = *this;
      for (std::size_t i = 0; i <= tmp.grad(); ++i)
        tmp[i] = -tmp[i];
      return tmp;
    }


    polynom<CoefficientT> & operator+=(CoefficientT const & lhs)
    {
      (*this)[0] += lhs;
      return *this;
    }

    polynom<CoefficientT> & operator-=(CoefficientT const & lhs)
    {
      (*this)[0] -= lhs;
      return *this;
    }

    polynom<CoefficientT> & operator+=(polynom<CoefficientT> const & lhs)
    {
      if (grad() < lhs.grad())
        set_grad(lhs.grad());

      for (std::size_t i = 0; i <= std::min(grad(), lhs.grad()); ++i)
        (*this)[i] += lhs[i];

      return *this;
    }

    polynom<CoefficientT> & operator-=(polynom<CoefficientT> const & lhs)
    {
      if (grad() < lhs.grad())
        resize(lhs.grad());

      for (std::size_t i = 0; i <= std::min(grad(), lhs.grad()); ++i)
         (*this)[i] -= lhs[i];

      return *this;
    }



    polynom<CoefficientT> & operator*=(polynom<CoefficientT> const & lhs)
    {
      polynom<CoefficientT> tmp = *this;

      set_grad( lhs.grad()+grad() );
      std::fill(this->begin(), this->end(), 0);

      for (std::size_t i = 0; i <= tmp.grad(); ++i)
      {
        for (std::size_t j = 0; j <= lhs.grad(); ++j)
          (*this)[i+j] += tmp[i]*lhs[j];
      }

      return *this;
    }





    polynom<CoefficientT> & operator*=(CoefficientT const & scalar)
    {
      for (std::size_t i = 0; i <= grad(); ++i)
        (*this)[i] *= scalar;
      return *this;
    }

    polynom<CoefficientT> & operator/=(CoefficientT const & scalar)
    {
      for (std::size_t i = 0; i <= grad(); ++i)
        (*this)[i] /= scalar;
      return *this;
    }



    static polynom<CoefficientT> X;

//   private:
//     std::vector<CoefficientT> coeffiecients_;
  };



  template<typename CoefficientT>
  std::ostream & operator<<(std::ostream & stream, polynom<CoefficientT> const & poly)
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




  template<typename CoefficientT>
  polynom<CoefficientT> monom(std::size_t i)
  {
    polynom<CoefficientT> poly;
    poly.set_grad(i);
    poly[i] = 1;
    return poly;
  }


  template<typename CoefficientT>
  polynom<CoefficientT> polynom<CoefficientT>::X = monom<CoefficientT>(1);




  template<typename CoefficientT>
  polynom<CoefficientT> pow(polynom<CoefficientT> const & poly, std::size_t exponent)
  {
    if (exponent == 0)
      return monom<CoefficientT>(0);

    polynom<CoefficientT> result = poly;
    for (std::size_t i = 1; i != exponent; ++i)
      result *= poly;

    return result;
  }





  template<typename CoefficientT>
  polynom<CoefficientT> operator+(polynom<CoefficientT> const & lhs, polynom<CoefficientT> const & rhs)
  {
    polynom<CoefficientT> result = lhs;
    result += rhs;
    return result;
  }

  template<typename CoefficientT>
  polynom<CoefficientT> operator-(polynom<CoefficientT> const & lhs, polynom<CoefficientT> const & rhs)
  {
    polynom<CoefficientT> result = lhs;
    result -= rhs;
    return result;
  }

  template<typename CoefficientT>
  polynom<CoefficientT> operator*(polynom<CoefficientT> const & lhs, polynom<CoefficientT> const & rhs)
  {
    polynom<CoefficientT> result = lhs;
    result *= rhs;
    return result;
  }



  template<typename CoefficientT>
  polynom<CoefficientT> operator*(polynom<CoefficientT> const & lhs, CoefficientT const & rhs)
  {
    polynom<CoefficientT> result = lhs;
    result *= rhs;
    return result;
  }

  template<typename CoefficientT>
  polynom<CoefficientT> operator*(CoefficientT const & lhs, polynom<CoefficientT> const & rhs)
  {
    polynom<CoefficientT> result = rhs;
    result *= lhs;
    return result;
  }


  template<typename CoefficientT>
  polynom<CoefficientT> operator/(polynom<CoefficientT> const & lhs, CoefficientT const & rhs)
  {
    polynom<CoefficientT> result = lhs;
    result /= rhs;
    return result;
  }





  template<typename CoefficientT>
  polynom<CoefficientT> operator+(polynom<CoefficientT> const & lhs, CoefficientT const & rhs)
  {
    polynom<CoefficientT> result = lhs;
    result += rhs;
    return result;
  }

  template<typename CoefficientT>
  polynom<CoefficientT> operator+(CoefficientT const & lhs, polynom<CoefficientT> const & rhs)
  {
    polynom<CoefficientT> result = rhs;
    result += lhs;
    return result;
  }


  template<typename CoefficientT>
  polynom<CoefficientT> operator-(polynom<CoefficientT> const & lhs, CoefficientT const & rhs)
  {
    polynom<CoefficientT> result = lhs;
    result -= rhs;
    return result;
  }

  template<typename CoefficientT>
  polynom<CoefficientT> operator-(CoefficientT const & lhs, polynom<CoefficientT> const & rhs)
  {
    polynom<CoefficientT> result = rhs;
    result -= lhs;
    return result;
  }



}

#endif
