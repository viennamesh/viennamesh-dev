#ifndef VIENNAMESH_ALGORITHM_SYMMETRY_INTEGRATE_HPP
#define VIENNAMESH_ALGORITHM_SYMMETRY_INTEGRATE_HPP

#include "viennagrid/algorithm/volume.hpp"

#include "spherical_harmonics.hpp"
#include "common.hpp"

namespace viennamesh
{
  template<typename F>
  typename F::result_type integrate(viennagrid::element const & element,
                                    viennagrid::triangle_tag,
                                    F const & f,
                                    int N)
  {
    assert(element.tag() == viennagrid::triangle_tag());

    typedef typename F::result_type ResultType;

    point A = viennagrid::get_point(element, 0);
    point B = viennagrid::get_point(element, 1);
    point C = viennagrid::get_point(element, 2);

    double base = N*3;
    ResultType result = 0.0;

    for (int row = 0; row != N; ++row)
    {
      for (int column = 0; column != 2*row+1; ++column)
      {
        int a = (base-2) - 3*row + (column % 2 == 0 ? 0 : 1);
        int b =(column % 2 == 0) ? (1+column/2*3) : ((column+1)/2*3-1);
        int c = base - a - b;

//         point_t pt = (A*a + B*b + C*c) / base;
//         result += f(pt);

        double x = (A[0]*a + B[0]*b + C[0]*c) / base;
        double y = (A[1]*a + B[1]*b + C[1]*c) / base;
        double z = (A[2]*a + B[2]*b + C[2]*c) / base;
        result += f(x,y,z);
      }
    }

    return result / static_cast<double>(N*N) * viennagrid::volume(element);
  }

  template<typename F>
  typename F::result_type integrate(viennagrid::element const & element,
                                    viennagrid::quadrilateral_tag,
                                    F const & f,
                                    int N)
  {
    assert(element.tag() == viennagrid::quadrilateral_tag());

    typedef typename F::result_type ResultType;

    point A = viennagrid::get_point(element, 0);
    point B = viennagrid::get_point(element, 1);
    point C = viennagrid::get_point(element, 2);
    point D = viennagrid::get_point(element, 3);

    ResultType result = 0.0;

    for (int row = 0; row != N; ++row)
    {
      for (int column = 0; column != N; ++column)
      {
        double alpha = (row+0.5)/N;
        double beta = (column+0.5)/N;

//         point_t pt = beta*(A+alpha*(C-A)) + (1.0-beta)*(B+alpha*(D-B));
//         result += f(pt);

        double x = beta*(A[0]+alpha*(C[0]-A[0])) + (1.0-beta)*(B[0]+alpha*(D[0]-B[0]));
        double y = beta*(A[1]+alpha*(C[1]-A[1])) + (1.0-beta)*(B[1]+alpha*(D[1]-B[1]));
        double z = beta*(A[2]+alpha*(C[2]-A[2])) + (1.0-beta)*(B[2]+alpha*(D[2]-B[2]));
        result += f(x,y,z);
      }
    }

    return result / static_cast<double>(N*N) * viennagrid::volume(element);
  }



  template<typename F>
  typename F::result_type integrate(viennagrid::element const & element,
                                    F const & f,
                                    int N)
  {
//     return integrate(element, viennagrid::triangle_tag(), f, N);


    switch (element.tag().internal())
    {
      case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
        return integrate(element, viennagrid::triangle_tag(), f, N);
      case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
        return integrate(element, viennagrid::quadrilateral_tag(), f, N);

    }

    assert(false);
    return 0;
  }


  template<typename F>
  typename F::result_type integrate(viennagrid::element const & element,
                                    F const & f,
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





  template<typename T, int DIM>
  struct weight_t
  {
    weight_t() {}

    weight_t(T x_in, T w_in) : w(w_in)
    {
      p[0] = x_in;
    }

    weight_t(T x_in, T y_in, T w_in) : w(w_in)
    {
      p[0] = x_in;
      p[1] = y_in;
    }

    weight_t(T x_in, T y_in, T z_in, T w_in) : w(w_in)
    {
      p[0] = x_in;
      p[1] = y_in;
      p[2] = z_in;
    }

    T p[DIM];
    T w;
  };




  template<typename T, int N>
  struct triangle_gauss_weights_generator;

  template<typename T>
  struct triangle_gauss_weights_generator<T,1>
  {
    static const int count = 1;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.3333333333333335, 0.3333333333333335, 1 );
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,2>
  {
    static const int count = 3;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type(0.1666666666666665, 0.1666666666666665, 0.3333333333333335);
      weights[i++] = weight_type(0.1666666666666665, 0.6666666666666665, 0.3333333333333335);
      weights[i++] = weight_type(0.6666666666666665, 0.1666666666666665, 0.3333333333333335);
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,3>
  {
    static const int count = 4;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.3333333333333335, 0.3333333333333335, -0.5625);
      weights[i++] = weight_type( 0.2, 0.2, 0.5208333333333335);
      weights[i++] = weight_type( 0.2, 0.6, 0.5208333333333335);
      weights[i++] = weight_type( 0.6, 0.2, 0.5208333333333335);
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,4>
  {
    static const int count = 5;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.445948490915965, 0.445948490915965, 0.223381589678011 );
      weights[i++] = weight_type( 0.445948490915965, 0.10810301816807, 0.223381589678011 );
      weights[i++] = weight_type( 0.10810301816807, 0.445948490915965, 0.223381589678011 );
      weights[i++] = weight_type( 0.09157621350977102, 0.09157621350977102, 0.109951743655322 );
      weights[i++] = weight_type( 0.09157621350977102, 0.816847572980459, 0.109951743655322 );
      weights[i++] = weight_type( 0.816847572980459, 0.09157621350977102, 0.109951743655322 );
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,5>
  {
    static const int count = 7;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.3333333333333335, 0.3333333333333335, 0.225);
      weights[i++] = weight_type( 0.470142064105115, 0.470142064105115, 0.132394152788506);
      weights[i++] = weight_type( 0.470142064105115, 0.05971587178977, 0.132394152788506);
      weights[i++] = weight_type( 0.05971587178977, 0.470142064105115, 0.132394152788506);
      weights[i++] = weight_type( 0.101286507323456, 0.101286507323456, 0.125939180544827);
      weights[i++] = weight_type( 0.101286507323456, 0.797426985353087, 0.125939180544827);
      weights[i++] = weight_type( 0.797426985353087, 0.101286507323456, 0.125939180544827);
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,6>
  {
    static const int count = 12;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.24928674517091, 0.24928674517091, 0.116786275726379);
      weights[i++] = weight_type( 0.24928674517091, 0.501426509658179, 0.116786275726379);
      weights[i++] = weight_type( 0.501426509658179, 0.24928674517091, 0.116786275726379);
      weights[i++] = weight_type( 0.063089014491502, 0.063089014491502, 0.050844906370207);
      weights[i++] = weight_type( 0.063089014491502, 0.873821971016996, 0.050844906370207);
      weights[i++] = weight_type( 0.873821971016996, 0.063089014491502, 0.050844906370207);
      weights[i++] = weight_type( 0.310352451033784, 0.636502499121399, 0.082851075618374);
      weights[i++] = weight_type( 0.636502499121399, 0.05314504984481699, 0.082851075618374);
      weights[i++] = weight_type( 0.05314504984481699, 0.310352451033784, 0.082851075618374);
      weights[i++] = weight_type( 0.310352451033784, 0.05314504984481699, 0.082851075618374);
      weights[i++] = weight_type( 0.636502499121399, 0.310352451033784, 0.082851075618374);
      weights[i++] = weight_type( 0.05314504984481699, 0.636502499121399, 0.082851075618374);
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,7>
  {
    static const int count = 13;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.3333333333333335, 0.3333333333333335, -0.149570044467682);
      weights[i++] = weight_type( 0.26034596607904, 0.26034596607904, 0.175615257433208);
      weights[i++] = weight_type( 0.26034596607904, 0.47930806784192, 0.175615257433208);
      weights[i++] = weight_type( 0.47930806784192, 0.26034596607904, 0.175615257433208);
      weights[i++] = weight_type( 0.06513010290221599, 0.06513010290221599, 0.053347235608838);
      weights[i++] = weight_type( 0.06513010290221599, 0.869739794195568, 0.053347235608838);
      weights[i++] = weight_type( 0.869739794195568, 0.06513010290221599, 0.053347235608838);
      weights[i++] = weight_type( 0.312865496004874, 0.63844418856981, 0.077113760890257);
      weights[i++] = weight_type( 0.63844418856981, 0.04869031542531599, 0.077113760890257);
      weights[i++] = weight_type( 0.04869031542531599, 0.312865496004874, 0.077113760890257);
      weights[i++] = weight_type( 0.312865496004874, 0.04869031542531599, 0.077113760890257);
      weights[i++] = weight_type( 0.63844418856981, 0.312865496004874, 0.077113760890257);
      weights[i++] = weight_type( 0.04869031542531599, 0.63844418856981, 0.077113760890257);
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,8>
  {
    static const int count = 16;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.3333333333333335, 0.3333333333333335, 0.144315607677787);
      weights[i++] = weight_type( 0.459292588292723, 0.459292588292723, 0.09509163426728499);
      weights[i++] = weight_type( 0.459292588292723, 0.08141482341455403, 0.09509163426728499);
      weights[i++] = weight_type( 0.08141482341455403, 0.459292588292723, 0.09509163426728499);
      weights[i++] = weight_type( 0.17056930775176, 0.17056930775176, 0.103217370534718);
      weights[i++] = weight_type( 0.17056930775176, 0.65886138449648, 0.103217370534718);
      weights[i++] = weight_type( 0.65886138449648, 0.17056930775176, 0.103217370534718);
      weights[i++] = weight_type( 0.05054722831703101, 0.05054722831703101, 0.032458497623198);
      weights[i++] = weight_type( 0.05054722831703101, 0.898905543365938, 0.0324584976231975);
      weights[i++] = weight_type( 0.898905543365938, 0.05054722831703101, 0.032458497623198);
      weights[i++] = weight_type( 0.263112829634638, 0.728492392955404, 0.027230314174435);
      weights[i++] = weight_type( 0.728492392955404, 0.008394777409957976, 0.027230314174435);
      weights[i++] = weight_type( 0.008394777409957976, 0.263112829634638, 0.027230314174435);
      weights[i++] = weight_type( 0.263112829634638, 0.008394777409957976, 0.027230314174435);
      weights[i++] = weight_type( 0.728492392955404, 0.263112829634638, 0.027230314174435);
      weights[i++] = weight_type( 0.008394777409957976, 0.728492392955404, 0.027230314174435);
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,9>
  {
    static const int count = 19;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.3333333333333335, 0.3333333333333335, 0.097135796282799);
      weights[i++] = weight_type( 0.489682519198738, 0.489682519198738, 0.031334700227139);
      weights[i++] = weight_type( 0.489682519198738, 0.02063496160252498, 0.031334700227139);
      weights[i++] = weight_type( 0.02063496160252498, 0.489682519198738, 0.031334700227139);
      weights[i++] = weight_type( 0.437089591492937, 0.437089591492937, 0.077827541004774);
      weights[i++] = weight_type( 0.437089591492937, 0.125820817014127, 0.077827541004774);
      weights[i++] = weight_type( 0.125820817014127, 0.437089591492937, 0.077827541004774);
      weights[i++] = weight_type( 0.188203535619033, 0.188203535619033, 0.07964773892721);
      weights[i++] = weight_type( 0.188203535619033, 0.6235929287619351, 0.07964773892721);
      weights[i++] = weight_type( 0.6235929287619351, 0.188203535619033, 0.07964773892721);
      weights[i++] = weight_type( 0.04472951339445302, 0.04472951339445302, 0.025577675658698);
      weights[i++] = weight_type( 0.04472951339445302, 0.910540973211095, 0.025577675658698);
      weights[i++] = weight_type( 0.910540973211095, 0.04472951339445302, 0.025577675658698);
      weights[i++] = weight_type( 0.221962989160766, 0.741198598784498, 0.043283539377289);
      weights[i++] = weight_type( 0.741198598784498, 0.03683841205473598, 0.043283539377289);
      weights[i++] = weight_type( 0.03683841205473598, 0.221962989160766, 0.043283539377289);
      weights[i++] = weight_type( 0.221962989160766, 0.03683841205473598, 0.043283539377289);
      weights[i++] = weight_type( 0.741198598784498, 0.221962989160766, 0.043283539377289);
      weights[i++] = weight_type( 0.03683841205473598, 0.741198598784498, 0.043283539377289);
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,10>
  {
    static const int count = 25;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.3333333333333335, 0.3333333333333335, 0.090817990382754);
      weights[i++] = weight_type( 0.485577633383657, 0.485577633383657, 0.036725957756467);
      weights[i++] = weight_type( 0.485577633383657, 0.02884473323268499, 0.036725957756467);
      weights[i++] = weight_type( 0.02884473323268499, 0.485577633383657, 0.036725957756467);
      weights[i++] = weight_type( 0.109481575485037, 0.109481575485037, 0.045321059435528);
      weights[i++] = weight_type( 0.109481575485037, 0.781036849029926, 0.045321059435528);
      weights[i++] = weight_type( 0.781036849029926, 0.109481575485037, 0.045321059435528);
      weights[i++] = weight_type( 0.307939838764121, 0.550352941820999, 0.07275791684542);
      weights[i++] = weight_type( 0.550352941820999, 0.14170721941488, 0.07275791684542);
      weights[i++] = weight_type( 0.14170721941488, 0.307939838764121, 0.07275791684542);
      weights[i++] = weight_type( 0.307939838764121, 0.14170721941488, 0.07275791684542);
      weights[i++] = weight_type( 0.550352941820999, 0.307939838764121, 0.07275791684542);
      weights[i++] = weight_type( 0.14170721941488, 0.550352941820999, 0.07275791684542);
      weights[i++] = weight_type( 0.246672560639903, 0.728323904597411, 0.028327242531057);
      weights[i++] = weight_type( 0.728323904597411, 0.02500353476268602, 0.028327242531057);
      weights[i++] = weight_type( 0.02500353476268602, 0.246672560639903, 0.028327242531057);
      weights[i++] = weight_type( 0.246672560639903, 0.02500353476268602, 0.028327242531057);
      weights[i++] = weight_type( 0.728323904597411, 0.246672560639903, 0.028327242531057);
      weights[i++] = weight_type( 0.02500353476268602, 0.728323904597411, 0.028327242531057);
      weights[i++] = weight_type( 0.06680325101220003, 0.9236559335875, 0.009421666963733);
      weights[i++] = weight_type( 0.9236559335875, 0.009540815400298996, 0.009421666963733);
      weights[i++] = weight_type( 0.009540815400298996, 0.06680325101220003, 0.009421666963733);
      weights[i++] = weight_type( 0.06680325101220003, 0.009540815400298996, 0.009421666963733);
      weights[i++] = weight_type( 0.9236559335875, 0.06680325101220003, 0.009421666963733);
      weights[i++] = weight_type( 0.009540815400298996, 0.9236559335875, 0.009421666963733);
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,11>
  {
    static const int count = 27;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.534611048270758, 0.534611048270758, 0.000927006328961);
      weights[i++] = weight_type( 0.534611048270758, -0.06922209654151701, 0.000927006328961);
      weights[i++] = weight_type( -0.06922209654151701, 0.534611048270758, 0.000927006328961);
      weights[i++] = weight_type( 0.398969302965855, 0.398969302965855, 0.07714953491481299);
      weights[i++] = weight_type( 0.398969302965855, 0.20206139406829, 0.07714953491481299);
      weights[i++] = weight_type( 0.20206139406829, 0.398969302965855, 0.07714953491481299);
      weights[i++] = weight_type( 0.203309900431282, 0.203309900431282, 0.059322977380774);
      weights[i++] = weight_type( 0.203309900431282, 0.593380199137435, 0.059322977380774);
      weights[i++] = weight_type( 0.593380199137435, 0.203309900431282, 0.059322977380774);
      weights[i++] = weight_type( 0.119350912282581, 0.119350912282581, 0.036184540503418);
      weights[i++] = weight_type( 0.119350912282581, 0.761298175434837, 0.036184540503418);
      weights[i++] = weight_type( 0.761298175434837, 0.119350912282581, 0.036184540503418);
      weights[i++] = weight_type( 0.03236494811127599, 0.03236494811127599, 0.013659731002678);
      weights[i++] = weight_type( 0.03236494811127599, 0.935270103777448, 0.013659731002678);
      weights[i++] = weight_type( 0.935270103777448, 0.03236494811127599, 0.013659731002678);
      weights[i++] = weight_type( 0.356620648261293, 0.593201213428213, 0.052337111962204);
      weights[i++] = weight_type( 0.593201213428213, 0.050178138310495, 0.052337111962204);
      weights[i++] = weight_type( 0.050178138310495, 0.356620648261293, 0.052337111962204);
      weights[i++] = weight_type( 0.356620648261293, 0.050178138310495, 0.052337111962204);
      weights[i++] = weight_type( 0.593201213428213, 0.356620648261293, 0.052337111962204);
      weights[i++] = weight_type( 0.050178138310495, 0.593201213428213, 0.052337111962204);
      weights[i++] = weight_type( 0.171488980304042, 0.807489003159792, 0.020707659639141);
      weights[i++] = weight_type( 0.807489003159792, 0.02102201653616598, 0.020707659639141);
      weights[i++] = weight_type( 0.02102201653616598, 0.171488980304042, 0.020707659639141);
      weights[i++] = weight_type( 0.171488980304042, 0.02102201653616598, 0.020707659639141);
      weights[i++] = weight_type( 0.807489003159792, 0.171488980304042, 0.020707659639141);
      weights[i++] = weight_type( 0.02102201653616598, 0.807489003159792, 0.020707659639141);
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,12>
  {
    static const int count = 33;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.488217389773805, 0.488217389773805, 0.025731066440455);
      weights[i++] = weight_type( 0.488217389773805, 0.02356522045239001, 0.025731066440455);
      weights[i++] = weight_type( 0.02356522045239001, 0.488217389773805, 0.025731066440455);
      weights[i++] = weight_type( 0.43972439229446, 0.43972439229446, 0.043692544538038);
      weights[i++] = weight_type( 0.43972439229446, 0.120551215411079, 0.043692544538038);
      weights[i++] = weight_type( 0.120551215411079, 0.43972439229446, 0.043692544538038);
      weights[i++] = weight_type( 0.271210385012116, 0.271210385012116, 0.06285822421788501);
      weights[i++] = weight_type( 0.271210385012116, 0.457579229975768, 0.06285822421788501);
      weights[i++] = weight_type( 0.457579229975768, 0.271210385012116, 0.06285822421788501);
      weights[i++] = weight_type( 0.127576145541586, 0.127576145541586, 0.034796112930709);
      weights[i++] = weight_type( 0.127576145541586, 0.7448477089168279, 0.034796112930709);
      weights[i++] = weight_type( 0.7448477089168279, 0.127576145541586, 0.034796112930709);
      weights[i++] = weight_type( 0.02131735045320998, 0.02131735045320998, 0.006166261051559);
      weights[i++] = weight_type( 0.02131735045320998, 0.957365299093579, 0.006166261051559);
      weights[i++] = weight_type( 0.957365299093579, 0.02131735045320998, 0.006166261051559);
      weights[i++] = weight_type( 0.275713269685514, 0.608943235779788, 0.040371557766381);
      weights[i++] = weight_type( 0.608943235779788, 0.115343494534698, 0.040371557766381);
      weights[i++] = weight_type( 0.115343494534698, 0.275713269685514, 0.040371557766381);
      weights[i++] = weight_type( 0.275713269685514, 0.115343494534698, 0.040371557766381);
      weights[i++] = weight_type( 0.608943235779788, 0.275713269685514, 0.040371557766381);
      weights[i++] = weight_type( 0.115343494534698, 0.608943235779788, 0.040371557766381);
      weights[i++] = weight_type( 0.28132558098994, 0.6958360867878031, 0.022356773202303);
      weights[i++] = weight_type( 0.6958360867878031, 0.02283833222225701, 0.022356773202303);
      weights[i++] = weight_type( 0.02283833222225701, 0.28132558098994, 0.022356773202303);
      weights[i++] = weight_type( 0.28132558098994, 0.02283833222225701, 0.022356773202303);
      weights[i++] = weight_type( 0.6958360867878031, 0.28132558098994, 0.022356773202303);
      weights[i++] = weight_type( 0.02283833222225701, 0.6958360867878031, 0.022356773202303);
      weights[i++] = weight_type( 0.116251915907597, 0.858014033544073, 0.017316231108659);
      weights[i++] = weight_type( 0.858014033544073, 0.02573405054833, 0.017316231108659);
      weights[i++] = weight_type( 0.02573405054833, 0.116251915907597, 0.017316231108659);
      weights[i++] = weight_type( 0.116251915907597, 0.02573405054833, 0.017316231108659);
      weights[i++] = weight_type( 0.858014033544073, 0.116251915907597, 0.017316231108659);
      weights[i++] = weight_type( 0.02573405054833, 0.858014033544073, 0.017316231108659);
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,13>
  {
    static const int count = 37;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.3333333333333335, 0.3333333333333335, 0.052520923400802);
      weights[i++] = weight_type( 0.495048184939705, 0.495048184939705, 0.01128014520933);
      weights[i++] = weight_type( 0.495048184939705, 0.009903630120590978, 0.01128014520933);
      weights[i++] = weight_type( 0.009903630120590978, 0.495048184939705, 0.01128014520933);
      weights[i++] = weight_type( 0.468716635109574, 0.468716635109574, 0.031423518362454);
      weights[i++] = weight_type( 0.468716635109574, 0.06256672978085198, 0.031423518362454);
      weights[i++] = weight_type( 0.06256672978085198, 0.468716635109574, 0.031423518362454);
      weights[i++] = weight_type( 0.414521336801277, 0.414521336801277, 0.047072502504194);
      weights[i++] = weight_type( 0.414521336801277, 0.170957326397447, 0.047072502504194);
      weights[i++] = weight_type( 0.170957326397447, 0.414521336801277, 0.047072502504194);
      weights[i++] = weight_type( 0.229399572042831, 0.229399572042831, 0.047363586536355);
      weights[i++] = weight_type( 0.229399572042831, 0.541200855914337, 0.047363586536355);
      weights[i++] = weight_type( 0.541200855914337, 0.229399572042831, 0.047363586536355);
      weights[i++] = weight_type( 0.11442449519633, 0.11442449519633, 0.031167529045794);
      weights[i++] = weight_type( 0.11442449519633, 0.77115100960734, 0.031167529045794);
      weights[i++] = weight_type( 0.77115100960734, 0.11442449519633, 0.031167529045794);
      weights[i++] = weight_type( 0.024811391363459, 0.024811391363459, 0.007975771465074);
      weights[i++] = weight_type( 0.024811391363459, 0.950377217273082, 0.007975771465074);
      weights[i++] = weight_type( 0.950377217273082, 0.024811391363459, 0.007975771465074);
      weights[i++] = weight_type( 0.268794997058761, 0.63635117456166, 0.036848402728732);
      weights[i++] = weight_type( 0.63635117456166, 0.09485382837957901, 0.036848402728732);
      weights[i++] = weight_type( 0.09485382837957901, 0.268794997058761, 0.036848402728732);
      weights[i++] = weight_type( 0.268794997058761, 0.09485382837957901, 0.036848402728732);
      weights[i++] = weight_type( 0.63635117456166, 0.268794997058761, 0.036848402728732);
      weights[i++] = weight_type( 0.09485382837957901, 0.63635117456166, 0.036848402728732);
      weights[i++] = weight_type( 0.291730066734288, 0.690169159986905, 0.017401463303822);
      weights[i++] = weight_type( 0.690169159986905, 0.01810077327880699, 0.017401463303822);
      weights[i++] = weight_type( 0.01810077327880699, 0.291730066734288, 0.017401463303822);
      weights[i++] = weight_type( 0.291730066734288, 0.01810077327880699, 0.017401463303822);
      weights[i++] = weight_type( 0.690169159986905, 0.291730066734288, 0.017401463303822);
      weights[i++] = weight_type( 0.01810077327880699, 0.690169159986905, 0.017401463303822);
      weights[i++] = weight_type( 0.126357385491669, 0.851409537834241, 0.015521786839045);
      weights[i++] = weight_type( 0.851409537834241, 0.02223307667409002, 0.015521786839045);
      weights[i++] = weight_type( 0.02223307667409002, 0.126357385491669, 0.015521786839045);
      weights[i++] = weight_type( 0.126357385491669, 0.02223307667409002, 0.015521786839045);
      weights[i++] = weight_type( 0.851409537834241, 0.126357385491669, 0.015521786839045);
      weights[i++] = weight_type( 0.02223307667409002, 0.851409537834241, 0.015521786839045);
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,14>
  {
    static const int count = 42;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.488963910362179 , 0.488963910362179 , 0.021883581369429 );
      weights[i++] = weight_type( 0.488963910362179 , 0.02207217927564298 , 0.021883581369429 );
      weights[i++] = weight_type( 0.02207217927564298 , 0.488963910362179 , 0.021883581369429 );
      weights[i++] = weight_type( 0.417644719340454 , 0.417644719340454 , 0.032788353544125 );
      weights[i++] = weight_type( 0.417644719340454 , 0.164710561319092 , 0.032788353544125 );
      weights[i++] = weight_type( 0.164710561319092 , 0.417644719340454 , 0.032788353544125 );
      weights[i++] = weight_type( 0.273477528308839 , 0.273477528308839 , 0.051774104507292 );
      weights[i++] = weight_type( 0.273477528308839 , 0.453044943382323 , 0.051774104507292 );
      weights[i++] = weight_type( 0.453044943382323 , 0.273477528308839 , 0.051774104507292 );
      weights[i++] = weight_type( 0.177205532412543 , 0.177205532412543 , 0.042162588736993 );
      weights[i++] = weight_type( 0.177205532412543 , 0.645588935174913 , 0.042162588736993 );
      weights[i++] = weight_type( 0.645588935174913 , 0.177205532412543 , 0.042162588736993 );
      weights[i++] = weight_type( 0.06179988309087298 , 0.06179988309087298 , 0.014433699669777 );
      weights[i++] = weight_type( 0.06179988309087298 , 0.876400233818255 , 0.014433699669777 );
      weights[i++] = weight_type( 0.876400233818255 , 0.06179988309087298 , 0.014433699669777 );
      weights[i++] = weight_type( 0.01939096124870099 , 0.01939096124870099 , 0.0049234036024 );
      weights[i++] = weight_type( 0.01939096124870099 , 0.961218077502598 , 0.0049234036024 );
      weights[i++] = weight_type( 0.961218077502598 , 0.01939096124870099 , 0.0049234036024 );
      weights[i++] = weight_type( 0.172266687821356 , 0.770608554774996 , 0.024665753212564 );
      weights[i++] = weight_type( 0.770608554774996 , 0.05712475740364797 , 0.024665753212564 );
      weights[i++] = weight_type( 0.05712475740364797 , 0.172266687821356 , 0.024665753212564 );
      weights[i++] = weight_type( 0.172266687821356 , 0.05712475740364797 , 0.024665753212564 );
      weights[i++] = weight_type( 0.770608554774996 , 0.172266687821356 , 0.024665753212564 );
      weights[i++] = weight_type( 0.05712475740364797 , 0.770608554774996 , 0.024665753212564 );
      weights[i++] = weight_type( 0.336861459796345 , 0.570222290846683 , 0.038571510787061 );
      weights[i++] = weight_type( 0.570222290846683 , 0.09291624935697201 , 0.038571510787061 );
      weights[i++] = weight_type( 0.09291624935697201 , 0.336861459796345 , 0.038571510787061 );
      weights[i++] = weight_type( 0.336861459796345 , 0.09291624935697201 , 0.038571510787061 );
      weights[i++] = weight_type( 0.570222290846683 , 0.336861459796345 , 0.038571510787061 );
      weights[i++] = weight_type( 0.09291624935697201 , 0.570222290846683 , 0.038571510787061 );
      weights[i++] = weight_type( 0.298372882136258 , 0.686980167808088 , 0.014436308113534 );
      weights[i++] = weight_type( 0.686980167808088 , 0.01464695005565397 , 0.014436308113534 );
      weights[i++] = weight_type( 0.01464695005565397 , 0.298372882136258 , 0.014436308113534 );
      weights[i++] = weight_type( 0.298372882136258 , 0.01464695005565397 , 0.014436308113534 );
      weights[i++] = weight_type( 0.686980167808088 , 0.298372882136258 , 0.014436308113534 );
      weights[i++] = weight_type( 0.01464695005565397 , 0.686980167808088 , 0.014436308113534 );
      weights[i++] = weight_type( 0.118974497696957 , 0.8797571713701711  , 0.005010228838501 );
      weights[i++] = weight_type( 0.8797571713701711  , 0.001268330932871986  , 0.005010228838501 );
      weights[i++] = weight_type( 0.001268330932871986  , 0.118974497696957 , 0.005010228838501 );
      weights[i++] = weight_type( 0.118974497696957 , 0.001268330932871986  , 0.005010228838501 );
      weights[i++] = weight_type( 0.8797571713701711  , 0.118974497696957 , 0.005010228838501 );
      weights[i++] = weight_type( 0.001268330932871986  , 0.8797571713701711  , 0.005010228838501 );
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,15>
  {
    static const int count = 48;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.506972916858243 , 0.506972916858243 , 0.001916875642849 );
      weights[i++] = weight_type( 0.506972916858243 , -0.01394583371648594  , 0.001916875642849 );
      weights[i++] = weight_type( -0.01394583371648594  , 0.506972916858243 , 0.001916875642849 );
      weights[i++] = weight_type( 0.431406354283023 , 0.431406354283023 , 0.044249027271145 );
      weights[i++] = weight_type( 0.431406354283023 , 0.137187291433955 , 0.044249027271145 );
      weights[i++] = weight_type( 0.137187291433955 , 0.431406354283023 , 0.044249027271145 );
      weights[i++] = weight_type( 0.277693644847144 , 0.277693644847144 , 0.051186548718852 );
      weights[i++] = weight_type( 0.277693644847144 , 0.444612710305711 , 0.051186548718852 );
      weights[i++] = weight_type( 0.444612710305711 , 0.277693644847144 , 0.051186548718852 );
      weights[i++] = weight_type( 0.126464891041254 , 0.126464891041254 , 0.023687735870688 );
      weights[i++] = weight_type( 0.126464891041254 , 0.747070217917492 , 0.023687735870688 );
      weights[i++] = weight_type( 0.747070217917492 , 0.126464891041254 , 0.023687735870688 );
      weights[i++] = weight_type( 0.07080838597468597 , 0.07080838597468597 , 0.013289775690021 );
      weights[i++] = weight_type( 0.07080838597468597 , 0.8583832280506281  , 0.013289775690021 );
      weights[i++] = weight_type( 0.8583832280506281  , 0.07080838597468597 , 0.013289775690021 );
      weights[i++] = weight_type( 0.01896517024107303 , 0.01896517024107303 , 0.004748916608192 );
      weights[i++] = weight_type( 0.01896517024107303 , 0.9620696595178531  , 0.004748916608192 );
      weights[i++] = weight_type( 0.9620696595178531  , 0.01896517024107303 , 0.004748916608192 );
      weights[i++] = weight_type( 0.261311371140087 , 0.604954466893291 , 0.038550072599593 );
      weights[i++] = weight_type( 0.604954466893291 , 0.133734161966621 , 0.038550072599593 );
      weights[i++] = weight_type( 0.133734161966621 , 0.261311371140087 , 0.038550072599593 );
      weights[i++] = weight_type( 0.261311371140087 , 0.133734161966621 , 0.038550072599593 );
      weights[i++] = weight_type( 0.604954466893291 , 0.261311371140087 , 0.038550072599593 );
      weights[i++] = weight_type( 0.133734161966621 , 0.604954466893291 , 0.038550072599593 );
      weights[i++] = weight_type( 0.388046767090269 , 0.575586555512814 , 0.027215814320624 );
      weights[i++] = weight_type( 0.575586555512814 , 0.03636667739691701 , 0.027215814320624 );
      weights[i++] = weight_type( 0.03636667739691701 , 0.388046767090269 , 0.027215814320624 );
      weights[i++] = weight_type( 0.388046767090269 , 0.03636667739691701 , 0.027215814320624 );
      weights[i++] = weight_type( 0.575586555512814 , 0.388046767090269 , 0.027215814320624 );
      weights[i++] = weight_type( 0.03636667739691701 , 0.575586555512814 , 0.027215814320624 );
      weights[i++] = weight_type( 0.285712220049916 , 0.724462663076655 , 0.002182077366797 );
      weights[i++] = weight_type( 0.724462663076655 , -0.010174883126571  , 0.002182077366797 );
      weights[i++] = weight_type( -0.010174883126571  , 0.285712220049916 , 0.002182077366797 );
      weights[i++] = weight_type( 0.285712220049916 , -0.010174883126571  , 0.002182077366797 );
      weights[i++] = weight_type( 0.724462663076655 , 0.285712220049916 , 0.002182077366797 );
      weights[i++] = weight_type( -0.010174883126571  , 0.724462663076655 , 0.002182077366797 );
      weights[i++] = weight_type( 0.215599664072284 , 0.747556466051838 , 0.021505319847731 );
      weights[i++] = weight_type( 0.747556466051838 , 0.03684386987587801 , 0.021505319847731 );
      weights[i++] = weight_type( 0.03684386987587801 , 0.215599664072284 , 0.021505319847731 );
      weights[i++] = weight_type( 0.215599664072284 , 0.03684386987587801 , 0.021505319847731 );
      weights[i++] = weight_type( 0.747556466051838 , 0.215599664072284 , 0.021505319847731 );
      weights[i++] = weight_type( 0.03684386987587801 , 0.747556466051838 , 0.021505319847731 );
      weights[i++] = weight_type( 0.103575616576386 , 0.883964574092416 , 0.007673942631049 );
      weights[i++] = weight_type( 0.883964574092416 , 0.012459809331199 , 0.007673942631049 );
      weights[i++] = weight_type( 0.012459809331199 , 0.103575616576386 , 0.007673942631049 );
      weights[i++] = weight_type( 0.103575616576386 , 0.012459809331199 , 0.007673942631049 );
      weights[i++] = weight_type( 0.883964574092416 , 0.103575616576386 , 0.007673942631049 );
      weights[i++] = weight_type( 0.012459809331199 , 0.883964574092416 , 0.007673942631049 );
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,16>
  {
    static const int count = 52;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.3333333333333335  , 0.3333333333333335  , 0.046875697427642 );
      weights[i++] = weight_type( 0.497380541948438 , 0.497380541948438 , 0.006405878578585 );
      weights[i++] = weight_type( 0.497380541948438 , 0.005238916103122992  , 0.006405878578585 );
      weights[i++] = weight_type( 0.005238916103122992  , 0.497380541948438 , 0.006405878578585 );
      weights[i++] = weight_type( 0.413469438549352 , 0.413469438549352 , 0.041710296739387 );
      weights[i++] = weight_type( 0.413469438549352 , 0.173061122901295 , 0.041710296739387 );
      weights[i++] = weight_type( 0.173061122901295 , 0.413469438549352 , 0.041710296739387 );
      weights[i++] = weight_type( 0.470458599066991 , 0.470458599066991 , 0.026891484250064 );
      weights[i++] = weight_type( 0.470458599066991 , 0.059082801866017 , 0.026891484250064 );
      weights[i++] = weight_type( 0.059082801866017 , 0.470458599066991 , 0.026891484250064 );
      weights[i++] = weight_type( 0.240553749969521 , 0.240553749969521 , 0.04213252276165  );
      weights[i++] = weight_type( 0.240553749969521 , 0.518892500060958 , 0.04213252276165  );
      weights[i++] = weight_type( 0.518892500060958 , 0.240553749969521 , 0.04213252276165  );
      weights[i++] = weight_type( 0.147965794222573 , 0.147965794222573 , 0.030000266842773 );
      weights[i++] = weight_type( 0.147965794222573 , 0.704068411554854 , 0.030000266842773 );
      weights[i++] = weight_type( 0.704068411554854 , 0.147965794222573 , 0.030000266842773 );
      weights[i++] = weight_type( 0.07546518765747401 , 0.07546518765747401 , 0.014200098925024 );
      weights[i++] = weight_type( 0.07546518765747401 , 0.849069624685052 , 0.014200098925024 );
      weights[i++] = weight_type( 0.849069624685052 , 0.07546518765747401 , 0.014200098925024 );
      weights[i++] = weight_type( 0.01659640262302498 , 0.01659640262302498 , 0.003582462351273 );
      weights[i++] = weight_type( 0.01659640262302498 , 0.96680719475395  , 0.003582462351273 );
      weights[i++] = weight_type( 0.96680719475395  , 0.01659640262302498 , 0.003582462351273 );
      weights[i++] = weight_type( 0.296555596579887 , 0.599868711174861 , 0.032773147460627 );
      weights[i++] = weight_type( 0.599868711174861 , 0.103575692245252 , 0.032773147460627 );
      weights[i++] = weight_type( 0.103575692245252 , 0.296555596579887 , 0.032773147460627 );
      weights[i++] = weight_type( 0.296555596579887 , 0.103575692245252 , 0.032773147460627 );
      weights[i++] = weight_type( 0.599868711174861 , 0.296555596579887 , 0.032773147460627 );
      weights[i++] = weight_type( 0.103575692245252 , 0.599868711174861 , 0.032773147460627 );
      weights[i++] = weight_type( 0.337723063403079 , 0.6421935249415049  , 0.015298306248441 );
      weights[i++] = weight_type( 0.6421935249415049  , 0.02008341165541599 , 0.015298306248441 );
      weights[i++] = weight_type( 0.02008341165541599 , 0.337723063403079 , 0.015298306248441 );
      weights[i++] = weight_type( 0.337723063403079 , 0.02008341165541599 , 0.015298306248441 );
      weights[i++] = weight_type( 0.6421935249415049  , 0.337723063403079 , 0.015298306248441 );
      weights[i++] = weight_type( 0.02008341165541599 , 0.6421935249415049  , 0.015298306248441 );
      weights[i++] = weight_type( 0.204748281642812 , 0.7995927209713269  , 0.002386244192839 );
      weights[i++] = weight_type( 0.7995927209713269  , -0.004341002614138945 , 0.002386244192839 );
      weights[i++] = weight_type( -0.004341002614138945 , 0.204748281642812 , 0.002386244192839 );
      weights[i++] = weight_type( 0.204748281642812 , -0.004341002614138945 , 0.002386244192839 );
      weights[i++] = weight_type( 0.7995927209713269  , 0.204748281642812 , 0.002386244192839 );
      weights[i++] = weight_type( -0.004341002614138945 , 0.7995927209713269  , 0.002386244192839 );
      weights[i++] = weight_type( 0.189358492130623 , 0.768699721401368 , 0.019084792755899 );
      weights[i++] = weight_type( 0.768699721401368 , 0.04194178646801  , 0.019084792755899 );
      weights[i++] = weight_type( 0.04194178646801  , 0.189358492130623 , 0.019084792755899 );
      weights[i++] = weight_type( 0.189358492130623 , 0.04194178646801  , 0.019084792755899 );
      weights[i++] = weight_type( 0.768699721401368 , 0.189358492130623 , 0.019084792755899 );
      weights[i++] = weight_type( 0.04194178646801  , 0.768699721401368 , 0.019084792755899 );
      weights[i++] = weight_type( 0.08528361568265702 , 0.900399064086661 , 0.006850054546542 );
      weights[i++] = weight_type( 0.900399064086661 , 0.014317320230681 , 0.006850054546542 );
      weights[i++] = weight_type( 0.014317320230681 , 0.08528361568265702 , 0.006850054546542 );
      weights[i++] = weight_type( 0.08528361568265702 , 0.014317320230681 , 0.006850054546542 );
      weights[i++] = weight_type( 0.900399064086661 , 0.08528361568265702 , 0.006850054546542 );
      weights[i++] = weight_type( 0.014317320230681 , 0.900399064086661 , 0.006850054546542 );
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,17>
  {
    static const int count = 61;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.3333333333333335  , 0.3333333333333335  , 0.033437199290803 );
      weights[i++] = weight_type( 0.497170540556774 , 0.497170540556774 , 0.005093415440507 );
      weights[i++] = weight_type( 0.497170540556774 , 0.00565891888645198 , 0.005093415440507 );
      weights[i++] = weight_type( 0.00565891888645198 , 0.497170540556774 , 0.005093415440507 );
      weights[i++] = weight_type( 0.482176322624625 , 0.482176322624625 , 0.014670864527638 );
      weights[i++] = weight_type( 0.482176322624625 , 0.03564735475075098 , 0.014670864527638 );
      weights[i++] = weight_type( 0.03564735475075098 , 0.482176322624625 , 0.014670864527638 );
      weights[i++] = weight_type( 0.450239969020782 , 0.450239969020782 , 0.024350878353672 );
      weights[i++] = weight_type( 0.450239969020782 , 0.09952006195843699 , 0.024350878353672 );
      weights[i++] = weight_type( 0.09952006195843699 , 0.450239969020782 , 0.024350878353672 );
      weights[i++] = weight_type( 0.400266239377397 , 0.400266239377397 , 0.031107550868969 );
      weights[i++] = weight_type( 0.400266239377397 , 0.199467521245206 , 0.031107550868969 );
      weights[i++] = weight_type( 0.199467521245206 , 0.400266239377397 , 0.031107550868969 );
      weights[i++] = weight_type( 0.252141267970953 , 0.252141267970953 , 0.03125711121862  );
      weights[i++] = weight_type( 0.252141267970953 , 0.495717464058095 , 0.03125711121862  );
      weights[i++] = weight_type( 0.495717464058095 , 0.252141267970953 , 0.03125711121862  );
      weights[i++] = weight_type( 0.162047004658461 , 0.162047004658461 , 0.024815654339665 );
      weights[i++] = weight_type( 0.162047004658461 , 0.675905990683077 , 0.024815654339665 );
      weights[i++] = weight_type( 0.675905990683077 , 0.162047004658461 , 0.024815654339665 );
      weights[i++] = weight_type( 0.07587588226074599 , 0.07587588226074599 , 0.014056073070557 );
      weights[i++] = weight_type( 0.07587588226074599 , 0.8482482354785079  , 0.014056073070557 );
      weights[i++] = weight_type( 0.8482482354785079  , 0.07587588226074599 , 0.014056073070557 );
      weights[i++] = weight_type( 0.015654726967822 , 0.015654726967822 , 0.003194676173779 );
      weights[i++] = weight_type( 0.015654726967822 , 0.968690546064356 , 0.003194676173779 );
      weights[i++] = weight_type( 0.968690546064356 , 0.015654726967822 , 0.003194676173779 );
      weights[i++] = weight_type( 0.334319867363658 , 0.655493203809423 , 0.008119655318993 );
      weights[i++] = weight_type( 0.655493203809423 , 0.01018692882691902 , 0.008119655318993 );
      weights[i++] = weight_type( 0.01018692882691902 , 0.334319867363658 , 0.008119655318993 );
      weights[i++] = weight_type( 0.334319867363658 , 0.01018692882691902 , 0.008119655318993 );
      weights[i++] = weight_type( 0.655493203809423 , 0.334319867363658 , 0.008119655318993 );
      weights[i++] = weight_type( 0.01018692882691902 , 0.655493203809423 , 0.008119655318993 );
      weights[i++] = weight_type( 0.292221537796944 , 0.57233759053202  , 0.026805742283163 );
      weights[i++] = weight_type( 0.57233759053202  , 0.135440871671036 , 0.026805742283163 );
      weights[i++] = weight_type( 0.135440871671036 , 0.292221537796944 , 0.026805742283163 );
      weights[i++] = weight_type( 0.292221537796944 , 0.135440871671036 , 0.026805742283163 );
      weights[i++] = weight_type( 0.57233759053202  , 0.292221537796944 , 0.026805742283163 );
      weights[i++] = weight_type( 0.135440871671036 , 0.57233759053202  , 0.026805742283163 );
      weights[i++] = weight_type( 0.31957488542319  , 0.626001190286228 , 0.018459993210822 );
      weights[i++] = weight_type( 0.626001190286228 , 0.05442392429058301 , 0.018459993210822 );
      weights[i++] = weight_type( 0.05442392429058301 , 0.31957488542319  , 0.018459993210822 );
      weights[i++] = weight_type( 0.31957488542319  , 0.05442392429058301 , 0.018459993210822 );
      weights[i++] = weight_type( 0.626001190286228 , 0.31957488542319  , 0.018459993210822 );
      weights[i++] = weight_type( 0.05442392429058301 , 0.626001190286228 , 0.018459993210822 );
      weights[i++] = weight_type( 0.190704224192292 , 0.796427214974071 , 0.008476868534328 );
      weights[i++] = weight_type( 0.796427214974071 , 0.01286856083363702 , 0.008476868534328 );
      weights[i++] = weight_type( 0.01286856083363702 , 0.190704224192292 , 0.008476868534328 );
      weights[i++] = weight_type( 0.190704224192292 , 0.01286856083363702 , 0.008476868534328 );
      weights[i++] = weight_type( 0.796427214974071 , 0.190704224192292 , 0.008476868534328 );
      weights[i++] = weight_type( 0.01286856083363702 , 0.796427214974071 , 0.008476868534328 );
      weights[i++] = weight_type( 0.180483211648746 , 0.752351005937729 , 0.018292796770025 );
      weights[i++] = weight_type( 0.752351005937729 , 0.067165782413524 , 0.018292796770025 );
      weights[i++] = weight_type( 0.067165782413524 , 0.180483211648746 , 0.018292796770025 );
      weights[i++] = weight_type( 0.180483211648746 , 0.067165782413524 , 0.018292796770025 );
      weights[i++] = weight_type( 0.752351005937729 , 0.180483211648746 , 0.018292796770025 );
      weights[i++] = weight_type( 0.067165782413524 , 0.752351005937729 , 0.018292796770025 );
      weights[i++] = weight_type( 0.08071131367956402 , 0.9046255040956079  , 0.006665632004165 );
      weights[i++] = weight_type( 0.9046255040956079  , 0.01466318222482799 , 0.006665632004165 );
      weights[i++] = weight_type( 0.01466318222482799 , 0.08071131367956402 , 0.006665632004165 );
      weights[i++] = weight_type( 0.08071131367956402 , 0.01466318222482799 , 0.006665632004165 );
      weights[i++] = weight_type( 0.9046255040956079  , 0.08071131367956402 , 0.006665632004165 );
      weights[i++] = weight_type( 0.01466318222482799 , 0.9046255040956079  , 0.006665632004165 );
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,18>
  {
    static const int count = 70;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.3333333333333335  , 0.3333333333333335  , 0.030809939937647 );
      weights[i++] = weight_type( 0.493344808630921 , 0.493344808630921 , 0.009072436679404 );
      weights[i++] = weight_type( 0.493344808630921 , 0.01331038273815699 , 0.009072436679404 );
      weights[i++] = weight_type( 0.01331038273815699 , 0.493344808630921 , 0.009072436679404 );
      weights[i++] = weight_type( 0.469210594241957 , 0.469210594241957 , 0.018761316939594 );
      weights[i++] = weight_type( 0.469210594241957 , 0.06157881151608602 , 0.018761316939594 );
      weights[i++] = weight_type( 0.06157881151608602 , 0.469210594241957 , 0.018761316939594 );
      weights[i++] = weight_type( 0.436281395887006 , 0.436281395887006 , 0.019441097985477 );
      weights[i++] = weight_type( 0.436281395887006 , 0.127437208225989 , 0.019441097985477 );
      weights[i++] = weight_type( 0.127437208225989 , 0.436281395887006 , 0.019441097985477 );
      weights[i++] = weight_type( 0.394846170673416 , 0.394846170673416 , 0.02775394861081  );
      weights[i++] = weight_type( 0.394846170673416 , 0.210307658653168 , 0.02775394861081  );
      weights[i++] = weight_type( 0.210307658653168 , 0.394846170673416 , 0.02775394861081  );
      weights[i++] = weight_type( 0.249794568803157 , 0.249794568803157 , 0.032256225351457 );
      weights[i++] = weight_type( 0.249794568803157 , 0.500410862393686 , 0.032256225351457 );
      weights[i++] = weight_type( 0.500410862393686 , 0.249794568803157 , 0.032256225351457 );
      weights[i++] = weight_type( 0.161432193743843 , 0.161432193743843 , 0.025074032616922 );
      weights[i++] = weight_type( 0.161432193743843 , 0.677135612512315 , 0.025074032616922 );
      weights[i++] = weight_type( 0.677135612512315 , 0.161432193743843 , 0.025074032616922 );
      weights[i++] = weight_type( 0.07659822748537098 , 0.07659822748537098 , 0.015271927971832 );
      weights[i++] = weight_type( 0.07659822748537098 , 0.846803545029257 , 0.015271927971832 );
      weights[i++] = weight_type( 0.846803545029257 , 0.07659822748537098 , 0.015271927971832 );
      weights[i++] = weight_type( 0.02425243935345001 , 0.02425243935345001 , 0.006793922022963 );
      weights[i++] = weight_type( 0.02425243935345001 , 0.9514951212931 , 0.006793922022963 );
      weights[i++] = weight_type( 0.9514951212931 , 0.02425243935345001 , 0.006793922022963 );
      weights[i++] = weight_type( 0.04314636721696502 , 0.04314636721696502 , -0.00222309872992 );
      weights[i++] = weight_type( 0.04314636721696502 , 0.913707265566071 , -0.00222309872992 );
      weights[i++] = weight_type( 0.913707265566071 , 0.04314636721696502 , -0.00222309872992 );
      weights[i++] = weight_type( 0.358911494940944 , 0.6326579688566361  , 0.006331914076406 );
      weights[i++] = weight_type( 0.6326579688566361  , 0.008430536202420014  , 0.006331914076406 );
      weights[i++] = weight_type( 0.008430536202420014  , 0.358911494940944 , 0.006331914076406 );
      weights[i++] = weight_type( 0.358911494940944 , 0.008430536202420014  , 0.006331914076406 );
      weights[i++] = weight_type( 0.6326579688566361  , 0.358911494940944 , 0.006331914076406 );
      weights[i++] = weight_type( 0.008430536202420014  , 0.6326579688566361  , 0.006331914076406 );
      weights[i++] = weight_type( 0.294402476751957 , 0.574410971510855 , 0.027257538049138 );
      weights[i++] = weight_type( 0.574410971510855 , 0.131186551737188 , 0.027257538049138 );
      weights[i++] = weight_type( 0.131186551737188 , 0.294402476751957 , 0.027257538049138 );
      weights[i++] = weight_type( 0.294402476751957 , 0.131186551737188 , 0.027257538049138 );
      weights[i++] = weight_type( 0.574410971510855 , 0.294402476751957 , 0.027257538049138 );
      weights[i++] = weight_type( 0.131186551737188 , 0.574410971510855 , 0.027257538049138 );
      weights[i++] = weight_type( 0.325017801641814 , 0.6247790467925121  , 0.017676785649465 );
      weights[i++] = weight_type( 0.6247790467925121  , 0.05020315156567501 , 0.017676785649465 );
      weights[i++] = weight_type( 0.05020315156567501 , 0.325017801641814 , 0.017676785649465 );
      weights[i++] = weight_type( 0.325017801641814 , 0.05020315156567501 , 0.017676785649465 );
      weights[i++] = weight_type( 0.6247790467925121  , 0.325017801641814 , 0.017676785649465 );
      weights[i++] = weight_type( 0.05020315156567501 , 0.6247790467925121  , 0.017676785649465 );
      weights[i++] = weight_type( 0.184737559666046 , 0.748933176523037 , 0.01837948463807  );
      weights[i++] = weight_type( 0.748933176523037 , 0.06632926381091597 , 0.01837948463807  );
      weights[i++] = weight_type( 0.06632926381091597 , 0.184737559666046 , 0.01837948463807  );
      weights[i++] = weight_type( 0.184737559666046 , 0.06632926381091597 , 0.01837948463807  );
      weights[i++] = weight_type( 0.748933176523037 , 0.184737559666046 , 0.01837948463807  );
      weights[i++] = weight_type( 0.06632926381091597 , 0.748933176523037 , 0.01837948463807  );
      weights[i++] = weight_type( 0.218796800013321 , 0.769207005420443 , 0.008104732808191999  );
      weights[i++] = weight_type( 0.769207005420443 , 0.01199619456623602 , 0.008104732808191999  );
      weights[i++] = weight_type( 0.01199619456623602 , 0.218796800013321 , 0.008104732808191999  );
      weights[i++] = weight_type( 0.218796800013321 , 0.01199619456623602 , 0.008104732808191999  );
      weights[i++] = weight_type( 0.769207005420443 , 0.218796800013321 , 0.008104732808191999  );
      weights[i++] = weight_type( 0.01199619456623602 , 0.769207005420443 , 0.008104732808191999  );
      weights[i++] = weight_type( 0.101179597136408 , 0.8839623022734671  , 0.007634129070725 );
      weights[i++] = weight_type( 0.8839623022734671  , 0.01485810059012499 , 0.007634129070725 );
      weights[i++] = weight_type( 0.01485810059012499 , 0.101179597136408 , 0.007634129070725 );
      weights[i++] = weight_type( 0.101179597136408 , 0.01485810059012499 , 0.007634129070725 );
      weights[i++] = weight_type( 0.8839623022734671  , 0.101179597136408 , 0.007634129070725 );
      weights[i++] = weight_type( 0.01485810059012499 , 0.8839623022734671  , 0.007634129070725 );
      weights[i++] = weight_type( 0.02087475528258598 , 1.014347260005363 , 4.6187660794e-05  );
      weights[i++] = weight_type( 1.014347260005363 , -0.03522201528794899  , 4.6187660794e-05  );
      weights[i++] = weight_type( -0.03522201528794899  , 0.02087475528258598 , 4.6187660794e-05  );
      weights[i++] = weight_type( 0.02087475528258598 , -0.03522201528794899  , 4.6187660794e-05  );
      weights[i++] = weight_type( 1.014347260005363 , 0.02087475528258598 , 4.6187660794e-05  );
      weights[i++] = weight_type( -0.03522201528794899  , 1.014347260005363 , 4.6187660794e-05  );
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,19>
  {
    static const int count = 73;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.3333333333333335  , 0.3333333333333335  , 0.032906331388919 );
      weights[i++] = weight_type( 0.489609987073006 , 0.489609987073006 , 0.010330731891272 );
      weights[i++] = weight_type( 0.489609987073006 , 0.02078002585398703 , 0.010330731891272 );
      weights[i++] = weight_type( 0.02078002585398703 , 0.489609987073006 , 0.010330731891272 );
      weights[i++] = weight_type( 0.454536892697893 , 0.454536892697893 , 0.022387247263016 );
      weights[i++] = weight_type( 0.454536892697893 , 0.09092621460421502 , 0.022387247263016 );
      weights[i++] = weight_type( 0.09092621460421502 , 0.454536892697893 , 0.022387247263016 );
      weights[i++] = weight_type( 0.401416680649431 , 0.401416680649431 , 0.030266125869468 );
      weights[i++] = weight_type( 0.401416680649431 , 0.197166638701138 , 0.030266125869468 );
      weights[i++] = weight_type( 0.197166638701138 , 0.401416680649431 , 0.030266125869468 );
      weights[i++] = weight_type( 0.255551654403098 , 0.255551654403098 , 0.030490967802198 );
      weights[i++] = weight_type( 0.255551654403098 , 0.488896691193805 , 0.030490967802198 );
      weights[i++] = weight_type( 0.488896691193805 , 0.255551654403098 , 0.030490967802198 );
      weights[i++] = weight_type( 0.17707794215213  , 0.17707794215213  , 0.024159212741641 );
      weights[i++] = weight_type( 0.17707794215213  , 0.645844115695741 , 0.024159212741641 );
      weights[i++] = weight_type( 0.645844115695741 , 0.17707794215213  , 0.024159212741641 );
      weights[i++] = weight_type( 0.110061053227952 , 0.110061053227952 , 0.016050803586801 );
      weights[i++] = weight_type( 0.110061053227952 , 0.779877893544096 , 0.016050803586801 );
      weights[i++] = weight_type( 0.779877893544096 , 0.110061053227952 , 0.016050803586801 );
      weights[i++] = weight_type( 0.05552862425184002 , 0.05552862425184002 , 0.008084580261784 );
      weights[i++] = weight_type( 0.05552862425184002 , 0.8889427514963211  , 0.008084580261784 );
      weights[i++] = weight_type( 0.8889427514963211  , 0.05552862425184002 , 0.008084580261784 );
      weights[i++] = weight_type( 0.01262186377722901 , 0.01262186377722901 , 0.002079362027485 );
      weights[i++] = weight_type( 0.01262186377722901 , 0.974756272445543 , 0.002079362027485 );
      weights[i++] = weight_type( 0.974756272445543 , 0.01262186377722901 , 0.002079362027485 );
      weights[i++] = weight_type( 0.395754787356943 , 0.600633794794645 , 0.003884876904981 );
      weights[i++] = weight_type( 0.600633794794645 , 0.003611417848412013  , 0.003884876904981 );
      weights[i++] = weight_type( 0.003611417848412013  , 0.395754787356943 , 0.003884876904981 );
      weights[i++] = weight_type( 0.395754787356943 , 0.003611417848412013  , 0.003884876904981 );
      weights[i++] = weight_type( 0.600633794794645 , 0.395754787356943 , 0.003884876904981 );
      weights[i++] = weight_type( 0.003611417848412013  , 0.600633794794643 , 0.003884876904981 );
      weights[i++] = weight_type( 0.307929983880436 , 0.557603261588784 , 0.025574160612022 );
      weights[i++] = weight_type( 0.557603261588784 , 0.13446675453078  , 0.025574160612022 );
      weights[i++] = weight_type( 0.13446675453078  , 0.307929983880436 , 0.025574160612022 );
      weights[i++] = weight_type( 0.307929983880436 , 0.13446675453078  , 0.025574160612022 );
      weights[i++] = weight_type( 0.557603261588784 , 0.307929983880436 , 0.025574160612022 );
      weights[i++] = weight_type( 0.13446675453078  , 0.557603261588784 , 0.025574160612022 );
      weights[i++] = weight_type( 0.26456694840652  , 0.720987025817365 , 0.008880903573337999  );
      weights[i++] = weight_type( 0.720987025817365 , 0.01444602577611498 , 0.008880903573337999  );
      weights[i++] = weight_type( 0.01444602577611498 , 0.26456694840652  , 0.008880903573337999  );
      weights[i++] = weight_type( 0.26456694840652  , 0.01444602577611498 , 0.008880903573337999  );
      weights[i++] = weight_type( 0.720987025817365 , 0.26456694840652  , 0.008880903573337999  );
      weights[i++] = weight_type( 0.01444602577611498 , 0.720987025817365 , 0.008880903573337999  );
      weights[i++] = weight_type( 0.358539352205951 , 0.594527068955871 , 0.016124546761731 );
      weights[i++] = weight_type( 0.594527068955871 , 0.046933578838178 , 0.016124546761731 );
      weights[i++] = weight_type( 0.046933578838178 , 0.358539352205951 , 0.016124546761731 );
      weights[i++] = weight_type( 0.358539352205951 , 0.046933578838178 , 0.016124546761731 );
      weights[i++] = weight_type( 0.594527068955871 , 0.358539352205951 , 0.016124546761731 );
      weights[i++] = weight_type( 0.046933578838178 , 0.594527068955871 , 0.016124546761731 );
      weights[i++] = weight_type( 0.157807405968595 , 0.839331473680839 , 0.002491941817491 );
      weights[i++] = weight_type( 0.839331473680839 , 0.002861120350567015  , 0.002491941817491 );
      weights[i++] = weight_type( 0.002861120350567015  , 0.157807405968595 , 0.002491941817491 );
      weights[i++] = weight_type( 0.157807405968595 , 0.002861120350567015  , 0.002491941817491 );
      weights[i++] = weight_type( 0.839331473680839 , 0.157807405968595 , 0.002491941817491 );
      weights[i++] = weight_type( 0.002861120350567015  , 0.839331473680839 , 0.002491941817491 );
      weights[i++] = weight_type( 0.07505059697591099 , 0.701087978926173 , 0.018242840118951 );
      weights[i++] = weight_type( 0.701087978926173 , 0.223861424097916 , 0.018242840118951 );
      weights[i++] = weight_type( 0.223861424097916 , 0.07505059697591099 , 0.018242840118951 );
      weights[i++] = weight_type( 0.07505059697591099 , 0.223861424097916 , 0.018242840118951 );
      weights[i++] = weight_type( 0.701087978926173 , 0.07505059697591099 , 0.018242840118951 );
      weights[i++] = weight_type( 0.223861424097916 , 0.701087978926173 , 0.018242840118951 );
      weights[i++] = weight_type( 0.142421601113383 , 0.822931324069857 , 0.010258563736199 );
      weights[i++] = weight_type( 0.822931324069857 , 0.03464707481675999 , 0.010258563736199 );
      weights[i++] = weight_type( 0.03464707481675999 , 0.142421601113383 , 0.010258563736199 );
      weights[i++] = weight_type( 0.142421601113383 , 0.03464707481675999 , 0.010258563736199 );
      weights[i++] = weight_type( 0.822931324069857 , 0.142421601113383 , 0.010258563736199 );
      weights[i++] = weight_type( 0.03464707481675999 , 0.822931324069857 , 0.010258563736199 );
      weights[i++] = weight_type( 0.06549462808293799 , 0.924344252620784 , 0.003799928855302 );
      weights[i++] = weight_type( 0.924344252620784 , 0.010161119296278 , 0.003799928855302 );
      weights[i++] = weight_type( 0.010161119296278 , 0.06549462808293799 , 0.003799928855302 );
      weights[i++] = weight_type( 0.06549462808293799 , 0.010161119296278 , 0.003799928855302 );
      weights[i++] = weight_type( 0.924344252620784 , 0.06549462808293799 , 0.003799928855302 );
      weights[i++] = weight_type( 0.010161119296278 , 0.924344252620784 , 0.003799928855302 );
      return weights;
    }
  };

  template<typename T>
  struct triangle_gauss_weights_generator<T,20>
  {
    static const int count = 79;
    typedef weight_t<T,2> weight_type;
    typedef boost::array<weight_type, count> weight_container_type;

    static weight_container_type init()
    {
      int i = 0;
      weight_container_type weights;
      weights[i++] = weight_type( 0.3333333333333335  , 0.3333333333333335  , 0.033057055541624 );
      weights[i++] = weight_type( 0.5009504643522 , 0.5009504643522 , 0.000867019185663 );
      weights[i++] = weight_type( 0.5009504643522 , -0.001900928704399951 , 0.000867019185663 );
      weights[i++] = weight_type( -0.001900928704399951 , 0.5009504643522 , 0.000867019185663 );
      weights[i++] = weight_type( 0.4882129579347285  , 0.4882129579347285  , 0.011660052716448 );
      weights[i++] = weight_type( 0.4882129579347285  , 0.02357408413054302 , 0.011660052716448 );
      weights[i++] = weight_type( 0.02357408413054302 , 0.4882129579347285  , 0.011660052716448 );
      weights[i++] = weight_type( 0.4551366869502825  , 0.4551366869502825  , 0.022876936356421 );
      weights[i++] = weight_type( 0.4551366869502825  , 0.08972663609943499 , 0.022876936356421 );
      weights[i++] = weight_type( 0.08972663609943499 , 0.4551366869502825  , 0.022876936356421 );
      weights[i++] = weight_type( 0.4019962593182895  , 0.4019962593182895  , 0.030448982673938 );
      weights[i++] = weight_type( 0.4019962593182895  , 0.196007481363421 , 0.030448982673938 );
      weights[i++] = weight_type( 0.196007481363421 , 0.4019962593182895  , 0.030448982673938 );
      weights[i++] = weight_type( 0.2558929097594215  , 0.2558929097594215  , 0.030624891725355 );
      weights[i++] = weight_type( 0.2558929097594215  , 0.488214180481157 , 0.030624891725355 );
      weights[i++] = weight_type( 0.488214180481157 , 0.2558929097594215  , 0.030624891725355 );
      weights[i++] = weight_type( 0.176488255995106 , 0.176488255995106 , 0.0243680576768 );
      weights[i++] = weight_type( 0.176488255995106 , 0.647023488009788 , 0.0243680576768 );
      weights[i++] = weight_type( 0.647023488009788 , 0.176488255995106 , 0.0243680576768 );
      weights[i++] = weight_type( 0.1041708553367585  , 0.1041708553367585  , 0.015997432032024 );
      weights[i++] = weight_type( 0.1041708553367585  , 0.791658289326483 , 0.015997432032024 );
      weights[i++] = weight_type( 0.791658289326483 , 0.1041708553367585  , 0.015997432032024 );
      weights[i++] = weight_type( 0.05306896384093002 , 0.05306896384093002 , 0.007698301815602 );
      weights[i++] = weight_type( 0.05306896384093002 , 0.8938620723181401  , 0.007698301815602 );
      weights[i++] = weight_type( 0.8938620723181401  , 0.05306896384093002 , 0.007698301815602 );
      weights[i++] = weight_type( 0.041618715196029 , 0.041618715196029 , -0.0006320604974879999  );
      weights[i++] = weight_type( 0.041618715196029 , 0.916762569607942 , -0.0006320604974879999  );
      weights[i++] = weight_type( 0.916762569607942 , 0.041618715196029 , -0.0006320604974879999  );
      weights[i++] = weight_type( 0.01158192140682202 , 0.01158192140682202 , 0.001751134301193 );
      weights[i++] = weight_type( 0.01158192140682202 , 0.976836157186356 , 0.001751134301193 );
      weights[i++] = weight_type( 0.976836157186356 , 0.01158192140682202 , 0.001751134301193 );
      weights[i++] = weight_type( 0.344855770229001 , 0.60640264610616  , 0.016465839189576 );
      weights[i++] = weight_type( 0.60640264610616  , 0.04874158366483899 , 0.016465839189576 );
      weights[i++] = weight_type( 0.04874158366483899 , 0.344855770229001 , 0.016465839189576 );
      weights[i++] = weight_type( 0.344855770229001 , 0.04874158366483899 , 0.016465839189576 );
      weights[i++] = weight_type( 0.60640264610616  , 0.344855770229001 , 0.016465839189576 );
      weights[i++] = weight_type( 0.04874158366483899 , 0.60640264610616  , 0.016465839189576 );
      weights[i++] = weight_type( 0.377843269594854 , 0.615842614456541 , 0.004839033540485 );
      weights[i++] = weight_type( 0.615842614456541 , 0.006314115948604981  , 0.004839033540485 );
      weights[i++] = weight_type( 0.006314115948604981  , 0.377843269594854 , 0.004839033540485 );
      weights[i++] = weight_type( 0.377843269594854 , 0.006314115948604981  , 0.004839033540485 );
      weights[i++] = weight_type( 0.615842614456541 , 0.377843269594854 , 0.004839033540485 );
      weights[i++] = weight_type( 0.006314115948604981  , 0.615842614456541 , 0.004839033540485 );
      weights[i++] = weight_type( 0.306635479062357 , 0.559048000390295 , 0.02580490653465  );
      weights[i++] = weight_type( 0.559048000390295 , 0.134316520547348 , 0.02580490653465  );
      weights[i++] = weight_type( 0.134316520547348 , 0.306635479062357 , 0.02580490653465  );
      weights[i++] = weight_type( 0.306635479062357 , 0.134316520547348 , 0.02580490653465  );
      weights[i++] = weight_type( 0.559048000390295 , 0.306635479062357 , 0.02580490653465  );
      weights[i++] = weight_type( 0.134316520547348 , 0.559048000390295 , 0.02580490653465  );
      weights[i++] = weight_type( 0.249419362774742 , 0.736606743262866 , 0.008471091054441 );
      weights[i++] = weight_type( 0.736606743262866 , 0.01397389396239201 , 0.008471091054441 );
      weights[i++] = weight_type( 0.01397389396239201 , 0.249419362774742 , 0.008471091054441 );
      weights[i++] = weight_type( 0.249419362774742 , 0.01397389396239201 , 0.008471091054441 );
      weights[i++] = weight_type( 0.736606743262866 , 0.249419362774742 , 0.008471091054441 );
      weights[i++] = weight_type( 0.01397389396239201 , 0.736606743262866 , 0.008471091054441 );
      weights[i++] = weight_type( 0.212775724802802 , 0.711675142287434 , 0.01835491410628  );
      weights[i++] = weight_type( 0.711675142287434 , 0.07554913290976401 , 0.01835491410628  );
      weights[i++] = weight_type( 0.07554913290976401 , 0.212775724802802 , 0.01835491410628  );
      weights[i++] = weight_type( 0.212775724802802 , 0.07554913290976401 , 0.01835491410628  );
      weights[i++] = weight_type( 0.711675142287434 , 0.212775724802802 , 0.01835491410628  );
      weights[i++] = weight_type( 0.07554913290976401 , 0.711675142287434 , 0.01835491410628  );
      weights[i++] = weight_type( 0.146965436053239 , 0.861402717154987 , 0.000704404677908 );
      weights[i++] = weight_type( 0.861402717154987 , -0.008368153208227036 , 0.000704404677908 );
      weights[i++] = weight_type( -0.008368153208227036 , 0.146965436053239 , 0.000704404677908 );
      weights[i++] = weight_type( 0.146965436053239 , -0.008368153208227036 , 0.000704404677908 );
      weights[i++] = weight_type( 0.861402717154987 , 0.146965436053239 , 0.000704404677908 );
      weights[i++] = weight_type( -0.008368153208227036 , 0.861402717154987 , 0.000704404677908 );
      weights[i++] = weight_type( 0.137726978828923 , 0.8355869579123629  , 0.010112684927462 );
      weights[i++] = weight_type( 0.8355869579123629  , 0.02668606325871398 , 0.010112684927462 );
      weights[i++] = weight_type( 0.02668606325871398 , 0.137726978828923 , 0.010112684927462 );
      weights[i++] = weight_type( 0.137726978828923 , 0.02668606325871398 , 0.010112684927462 );
      weights[i++] = weight_type( 0.8355869579123629  , 0.137726978828923 , 0.010112684927462 );
      weights[i++] = weight_type( 0.02668606325871398 , 0.8355869579123629  , 0.010112684927462 );
      weights[i++] = weight_type( 0.05969610914900703 , 0.929756171556853 , 0.00357390938595  );
      weights[i++] = weight_type( 0.929756171556853 , 0.010547719294141 , 0.00357390938595  );
      weights[i++] = weight_type( 0.010547719294141 , 0.05969610914900703 , 0.00357390938595  );
      weights[i++] = weight_type( 0.05969610914900703 , 0.010547719294141 , 0.00357390938595  );
      weights[i++] = weight_type( 0.929756171556853 , 0.05969610914900703 , 0.00357390938595  );
      weights[i++] = weight_type( 0.010547719294141 , 0.929756171556853 , 0.00357390938595  );
      return weights;
    }
  };


  template<typename WeightsGeneratorT>
  class triangle_quadrature
  {
  public:
    static const int count = WeightsGeneratorT::count;
    typedef typename WeightsGeneratorT::weight_type weight_type;
    typedef typename WeightsGeneratorT::weight_container_type weight_container_type;

    static weight_container_type const & weights() { return weights_; }

    template<typename PointT, typename F>
    typename F::result_type operator()(PointT const & p0, PointT const & p1, PointT const & p2, F const & f)
    {
      typedef typename F::result_type ResultType;
      ResultType result = 0;

      PointT d0 = p1-p0;
      PointT d1 = p2-p0;

      for (typename weight_container_type::size_type i = 0; i != weights().size(); ++i)
      {
        weight_type const & weight = weights()[i];

        PointT tmp = p0;
        for (typename PointT::size_type i = 0; i != tmp.size(); ++i)
          tmp[i] += weight.p[0]*d0[i] + weight.p[1]*d1[i];
        result += f(tmp) * weight.w;
      }

      return result * viennagrid::spanned_volume(p0, p1, p2);
    }


    template<typename ElementT, typename F>
    typename F::result_type operator()(ElementT const & element, F const & f)
    {
      typedef typename viennagrid::result_of::point<ElementT>::type PointType;

      PointType p0 = viennagrid::get_point(element, 0);
      PointType p1 = viennagrid::get_point(element, 1);
      PointType p2 = viennagrid::get_point(element, 2);

      return (*this)(p0, p1, p2, f);
    }

  private:
    static const weight_container_type weights_;
  };

  template<typename WeightsGeneratorT>
  const typename triangle_quadrature<WeightsGeneratorT>::weight_container_type triangle_quadrature<WeightsGeneratorT>::weights_ = WeightsGeneratorT::init();


}

#endif
