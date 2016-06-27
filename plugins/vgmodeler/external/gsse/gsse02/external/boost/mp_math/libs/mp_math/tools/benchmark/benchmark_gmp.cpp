// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "benchmark_gmp.hpp"
#include <boost/timer.hpp>

benchmark_gmp::benchmark_gmp()
:
  base(std::string("gmp"), gmp_version)
{
  // init dst vector
  std::fill(dst.begin(), dst.end(), mpz_class());
}

benchmark_gmp::~benchmark_gmp()
{}

void benchmark_gmp::clear_dst_vector()
{
  std::fill(dst.begin(), dst.end(), mpz_class());
}

void benchmark_gmp::construct_operand_1(const std::string& src, unsigned int i)
{
  src1[i] = mpz_class(src, 16);
}

void benchmark_gmp::construct_operand_2(const std::string& src, unsigned int i)
{
  src2[i] = mpz_class(src, 16);
}

#define bench_function_def(f)                                   \
void benchmark_gmp::f(benchmark_result& r, double sample_time)  \
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


