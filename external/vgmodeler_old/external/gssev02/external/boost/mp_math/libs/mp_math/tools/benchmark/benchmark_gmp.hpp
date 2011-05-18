// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_TOOLS_BENCHMARK_GMP_HPP
#define BOOST_MP_MATH_TOOLS_BENCHMARK_GMP_HPP

#include <gmpxx.h>
#include "benchmark.hpp"


struct benchmark_gmp : benchmark<mpz_class>
{
  typedef benchmark<mpz_class> base;

  benchmark_gmp();
  ~benchmark_gmp();

  void clear_dst_vector();

  void construct_operand_1(const std::string& src, unsigned int i);
  void construct_operand_2(const std::string& src, unsigned int i);

  void ctor_dec(benchmark_result&, double sample_time);
  void ctor_hex(benchmark_result&, double sample_time);
  void to_dec  (benchmark_result&, double sample_time);
  void to_hex  (benchmark_result&, double sample_time);
  void add     (benchmark_result&, double sample_time);
  void subtract(benchmark_result&, double sample_time);
  void multiply(benchmark_result&, double sample_time);
  void divide  (benchmark_result&, double sample_time);
  void modulo  (benchmark_result&, double sample_time);
  void square  (benchmark_result&, double sample_time);
  void modpow  (benchmark_result&, double sample_time);

  struct ctor_dec_op
  {
    base& b;
    explicit ctor_dec_op(base& ba) : b(ba) {}
    void operator()(unsigned int i) const { b.dst[i] = mpz_class(b.dec_str[i], 10); }
  };

  struct ctor_hex_op
  {
    base& b;
    explicit ctor_hex_op(base& ba) : b(ba) {}
    void operator()(unsigned int i) const { b.dst[i] = mpz_class(b.hex_str[i], 16); }
  };

  struct to_dec_op
  {
    base& b;
    explicit to_dec_op(base& ba) : b(ba) {}
    void operator()(unsigned int i) const { b.str = b.src1[i].get_str(10); }
  };

  struct to_hex_op
  {
    base& b;
    explicit to_hex_op(base& ba) : b(ba) {}
    void operator()(unsigned int i) const { b.str = b.src1[i].get_str(16); }
  };

  #define bench_functor(name,op)              \
  struct name##_op {                          \
    base& b;                                  \
    explicit name##_op(base& ba) : b(ba) {}   \
    void operator()(unsigned int i) const     \
    { b.dst[i] = b.src1[i] op b.src2[i]; }    \
  }

  bench_functor(add,+);
  bench_functor(subtract,-);
  bench_functor(multiply,*);
  bench_functor(divide,/);
  bench_functor(modulo,%);

  #undef bench_functor

  struct square_op
  {
    base& b;
    explicit square_op(base& ba) : b(ba) {}
    void operator()(unsigned int i) const
    {
      b.dst[i] = b.src1[i] * b.src1[i];
    }
  };

  struct modpow_op
  {
    base& b;
    explicit modpow_op(base& ba) : b (ba) {}
    void operator()(unsigned int i) const
    {
      mpz_powm(
          b.dst[i].get_mpz_t(),
          b.src1[i].get_mpz_t(), b.src1[i].get_mpz_t(), b.src2[i].get_mpz_t());
    }
  };
};



#endif

