// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "benchmark_libtom.hpp"

const char* ltm_version = "0.41";

benchmark_libtom::benchmark_libtom()
:
  base("libtommath", ltm_version)
{
  // init vectors
  for (mp_int_vec::iterator it = dst.begin(); it != dst.end(); ++it)
    mp_init(&*it);
  for (mp_int_vec::iterator it = src1.begin(); it != src1.end(); ++it)
    mp_init(&*it);
  for (mp_int_vec::iterator it = src2.begin(); it != src2.end(); ++it)
    mp_init(&*it);
}

benchmark_libtom::~benchmark_libtom()
{
  for (mp_int_vec::iterator it = dst.begin(); it != dst.end(); ++it)
    mp_clear(&*it);
}

void benchmark_libtom::clear_dst_vector()
{
  for (mp_int_vec::iterator it = dst.begin(); it != dst.end(); ++it)  
  {                                                                     
    mp_clear(&*it);                                                     
    mp_init(&*it);                                                      
  }
}

void benchmark_libtom::construct_operand_1(const std::string& src, unsigned int i)
{
  mp_read_radix(&src1[i], src.c_str(), 16);
}

void benchmark_libtom::construct_operand_2(const std::string& src, unsigned int i)
{
  mp_read_radix(&src2[i], src.c_str(), 16);
}

#define bench_function_def(f)                                      \
void benchmark_libtom::f(benchmark_result& r, double sample_time)  \
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

