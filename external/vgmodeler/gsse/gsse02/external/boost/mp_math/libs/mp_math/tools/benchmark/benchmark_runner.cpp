// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <stdexcept>
#include "benchmark_runner.hpp"


void benchmark_runner::run(const std::vector<std::string>& ops,
                           double sample_time,
                           const std::vector<std::string>& vec1,
                           const std::vector<std::string>& vec2,
                           const std::vector<std::string>& hex_str_vec,
                           const std::vector<std::string>& dec_str_vec)
{
  if (vec1.size() != vec2.size())
    throw std::runtime_error("input vectors must have same length");

  unsigned int stepsize = vec1.size() / 10;

  if (stepsize == 0)
    stepsize = 1;

  for (library_vec::iterator lib = libs_.begin(); lib != libs_.end(); ++lib)
  {
    (*lib)->set_num_input_samples(vec1.size());
    
    // init ctor strings
    (*lib)->hex_str = hex_str_vec;
    (*lib)->dec_str = dec_str_vec;

    std::cout << (*lib)->name() << "->constructing operand 1 input vector";
    std::cout.flush();

    for (unsigned int i = 0; i < vec1.size(); ++i)
    {
      (*lib)->construct_operand_1(vec1[i], i);
      if (((i+1) % stepsize) == 0)
      {
        std::cout << ".";
        std::cout.flush();
      }
    }
    std::cout << std::endl;

    std::cout << (*lib)->name() << "->constructing operand 2 input vector";
    std::cout.flush();

    for (unsigned int i = 0; i < vec2.size(); ++i)
    {
      (*lib)->construct_operand_2(vec2[i], i);
      if (((i+1) % stepsize) == 0)
      {
        std::cout << ".";
        std::cout.flush();
      }
    }
    std::cout << std::endl;
  }

  results_.clear();

  for (std::vector<std::string>::const_iterator op = ops.begin(); op != ops.end(); ++op)
    execute_bench_op(*op, sample_time);
}

void benchmark_runner::add_library(const boost::shared_ptr<benchmark_base>& b)
{
  libs_.push_back(b);
}

void benchmark_runner::execute_bench_op(const std::string& op, double sample_time)
{
  for (library_vec::iterator lib = libs_.begin(); lib != libs_.end(); ++lib)
  {
    benchmark_result r;
    
    r.libname = (*lib)->name() + "-" + (*lib)->version();
    r.op = op;
    r.sample_time = sample_time;

    (*lib)->run(r, op, sample_time);

    results_.push_back(r);
  }
}

