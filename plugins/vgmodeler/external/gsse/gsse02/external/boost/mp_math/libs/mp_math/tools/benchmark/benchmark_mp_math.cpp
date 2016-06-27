// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/version.hpp>
#include "benchmark_mp_math.hpp"


benchmark_mp_math::benchmark_mp_math()
:
  base("boost.mp_math", BOOST_LIB_VERSION)
{
  // init dst vector
  std::fill(dst.begin(), dst.end(), mp_int_type());
}

benchmark_mp_math::~benchmark_mp_math()
{}

void benchmark_mp_math::clear_dst_vector()
{
  std::fill(dst.begin(), dst.end(), mp_int_type());
}

void benchmark_mp_math::construct_operand_1(const std::string& src, unsigned int i)
{
  src1[i].assign(src, std::ios::hex);
}

void benchmark_mp_math::construct_operand_2(const std::string& src, unsigned int i)
{
  src2[i].assign(src, std::ios::hex);
}

#define bench_function_def(f)                                       \
void benchmark_mp_math::f(benchmark_result& r, double sample_time)  \
{ do_bench(f##_op(*this), r, sample_time); }

bench_function_def(ctor_dec)
bench_function_def(ctor_hex)
bench_function_def(to_dec)
bench_function_def(to_hex)
bench_function_def(add)
bench_function_def(subtract)
bench_function_def(multiply)
bench_function_def(divide)
bench_function_def(modulo)
bench_function_def(square)
bench_function_def(modpow)

#undef bench_function_def

