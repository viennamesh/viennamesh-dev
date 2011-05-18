// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_TOOLS_BENCHMARK_MP_MATH_HPP
#define BOOST_MP_MATH_TOOLS_BENCHMARK_MP_MATH_HPP

#include <boost/mp_math/mp_int.hpp>
#include "benchmark.hpp"


struct benchmark_mp_math : benchmark<boost::mp_math::mp_int<> >
{
  typedef benchmark<boost::mp_math::mp_int<> > base;

  benchmark_mp_math();
  virtual ~benchmark_mp_math();

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
    void operator()(unsigned int i) const { b.dst[i].assign(b.dec_str[i], std::ios::dec); }
  };

  struct ctor_hex_op
  {
    base& b;
    explicit ctor_hex_op(base& ba) : b(ba) {}
    void operator()(unsigned int i) const { b.dst[i].assign(b.hex_str[i], std::ios::hex); }
  };

  struct to_dec_op
  {
    base& b;
    explicit to_dec_op(base& ba) : b(ba) {}
    void operator()(unsigned int i) const { b.str = b.src1[i].to_string<std::string>(std::ios::dec); }
  };

  struct to_hex_op
  {
    base& b;
    explicit to_hex_op(base& ba) : b(ba) {}
    void operator()(unsigned int i) const { b.str = b.src1[i].to_string<std::string>(std::ios::hex); }
  };

  #define bench_functor(name,op)              \
  struct name##_op {                          \
    base& b;                                  \
    explicit name##_op(base& ba) : b(ba) {}   \
    void operator()(unsigned int i) const     \
    { b.dst[i] = b.src1[i]; b.dst[i] op##= b.src2[i]; } \
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
      b.dst[i] = b.src1[i];
      b.dst[i].sqr();
    }
  };

  struct modpow_op
  {
    base& b;
    explicit modpow_op(base& ba) : b(ba) {}
    void operator()(unsigned int i) const
    {
      b.dst[i] = boost::mp_math::modpow(b.src1[i], b.src1[i], b.src2[i]);
    }
  };
};



#endif

