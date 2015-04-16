#ifndef VIENNAMESH_ALGORITHM_SYMMETRY_INTEGRATE_HPP
#define VIENNAMESH_ALGORITHM_SYMMETRY_INTEGRATE_HPP

namespace viennamesh
{
  template<typename F>
  typename F::result_type integrate(viennagrid::element_t const & element,
                                    viennagrid::triangle_tag,
                                    F f,
                                    int N)
  {
    assert(element.tag() == viennagrid::triangle_tag());

    typedef typename F::result_type ResultType;

    point_t A = viennagrid::get_point(element, 0);
    point_t B = viennagrid::get_point(element, 1);
    point_t C = viennagrid::get_point(element, 2);

    int base = N*3;
    ResultType result = 0.0;

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

  template<typename F>
  typename F::result_type integrate(viennagrid::element_t const & element,
                                    viennagrid::quadrilateral_tag,
                                    F f,
                                    int N)
  {
    assert(element.tag() == viennagrid::quadrilateral_tag());

    typedef typename F::result_type ResultType;

    point_t A = viennagrid::get_point(element, 0);
    point_t B = viennagrid::get_point(element, 1);
    point_t C = viennagrid::get_point(element, 2);
    point_t D = viennagrid::get_point(element, 3);

    ResultType result = 0.0;

    for (int row = 0; row != N; ++row)
    {
      for (int column = 0; column != N; ++column)
      {
        double alpha = (row+0.5)/N;
        double beta = (column+0.5)/N;

        point_t pt = beta*(A+alpha*(C-A)) + (1.0-beta)*(B+alpha*(D-B));
        result += f(pt);
      }
    }

    return result / static_cast<double>(N*N) * viennagrid::volume(element);
  }



  template<typename F>
  typename F::result_type integrate(viennagrid::element_t const & element,
                                 F f,
                                 int N)
  {
    switch (element.tag().internal())
    {
      case VIENNAGRID_ELEMENT_TAG_TRIANGLE:
        return integrate(element, viennagrid::triangle_tag(), f, N);
      case VIENNAGRID_ELEMENT_TAG_QUADRILATERAL:
        return integrate(element, viennagrid::quadrilateral_tag(), f, N);

    }

    assert(false);
    return 0;
  }


  template<typename F>
  typename F::result_type integrate(viennagrid::element_t const & element,
                                 F f,
                                 double relative_error, double absolute_error, int max_iterations)
  {
    typedef typename F::result_type ResultType;
    ResultType prev_result = integrate(element, f, 1);
    double error;

    int N = 2;

    for (int it = 0; it != max_iterations; ++it, N *= 2)
    {
      ResultType result = integrate(element, f, N);

      if ( (std::abs(result) < absolute_error) && (std::abs(prev_result) < absolute_error) )
      {
//         std::cout << "Iteration finished (close to 0) with (it = " << it << ") N = " << N << ", error = " << error << std::endl;
        return result;
      }

      if ( std::abs(result) > std::abs(prev_result) )
        error = std::abs( (result - prev_result) / result );
      else
        error = std::abs( (prev_result - result) / prev_result );

      if (error < relative_error)
      {
//         std::cout << "Iteration finished with (it = " << it << ") N = " << N << ", error = " << error << std::endl;
        return result;
      }
//       std::cout << "   Iteration " << it << " result = " << result << " error = " << error << " N = " << N << std::endl;

      prev_result = result;
    }

//     std::cout << "Iteration finished (max iteration count reached " << max_iterations << ") error = " << error << std::endl;

    return prev_result;
  }

}

#endif
