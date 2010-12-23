// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_TOOLS_BENCHMARK_LIBTOM_HPP
#define BOOST_MP_MATH_TOOLS_BENCHMARK_LIBTOM_HPP

#include "benchmark.hpp"
#include "tommath.h"

extern const char* ltm_version;

struct benchmark_libtom : benchmark<mp_int>
{
  typedef benchmark<mp_int> base;

  benchmark_libtom();
  ~benchmark_libtom();

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
    void operator()(unsigned int i) const { mp_read_radix(&b.dst[i], b.dec_str[i].c_str(), 10); }
  };

  struct ctor_hex_op
  {
    base& b;
    explicit ctor_hex_op(base& ba) : b(ba) {}
    void operator()(unsigned int i) const { mp_read_radix(&b.dst[i], b.hex_str[i].c_str(), 16); }
  };

  struct to_dec_op
  {
    base& b;
    char* tmp;
    unsigned size;
    
    explicit to_dec_op(base& ba) : b(ba), tmp(0), size(0) {}
    ~to_dec_op() { delete[] tmp; }
    
    void operator()(unsigned int i)
    {
      const unsigned needed = b.src1[i].used * DIGIT_BIT / 3;
      if (size < needed)
        tmp = new char[needed];
      mp_toradix(&b.src1[i], tmp, 10);
      b.str = tmp;
    }
  };

  struct to_hex_op
  {
    base& b;
    char* tmp;
    unsigned size;
    
    explicit to_hex_op(base& ba) : b(ba), tmp(0), size(0) {}
    ~to_hex_op() { delete[] tmp; }
    
    void operator()(unsigned int i)
    {
      const unsigned needed = b.src1[i].used * DIGIT_BIT / 4 + 1;
      if (size < needed)
        tmp = new char[needed];
      mp_toradix(&b.src1[i], tmp, 16);
      b.str = tmp;
    }
  };

  static void mp_div_wrapper(mp_int* x, mp_int* y, mp_int* dst)
  {
    mp_div(x, y, dst, NULL);
  }

  #define bench_functor(name,op,func)           \
  struct name##_op {                            \
    base& b;                                    \
    explicit name##_op(base& ba) : b(ba) {}     \
    void operator()(unsigned int i) const       \
    { func(&b.src1[i],&b.src2[i],&b.dst[i]); }  \
  }
  
  bench_functor(add,+,mp_add);
  bench_functor(subtract,-,mp_sub);
  bench_functor(multiply,*,mp_mul);
  bench_functor(divide,/,mp_div_wrapper);
  bench_functor(modulo,%,mp_mod);

  #undef bench_functor

  struct square_op
  {
    base& b;
    explicit square_op(base& ba) : b(ba) {}
    void operator()(unsigned int i) const { mp_sqr(&b.src1[i], &b.dst[i]); }
  };

  struct modpow_op
  {
    base& b;
    explicit modpow_op(base& ba) : b(ba) {}
    void operator()(unsigned int i) const
    {
      mp_exptmod(&b.src1[i], &b.src1[i], &b.src2[i], &b.dst[i]);
    }
  };
};



#endif

