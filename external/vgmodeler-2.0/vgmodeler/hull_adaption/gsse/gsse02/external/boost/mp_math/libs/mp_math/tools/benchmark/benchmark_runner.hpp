// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_TOOLS_BENCHMARK_RUNNER_HPP
#define BOOST_MP_MATH_TOOLS_BENCHMARK_RUNNER_HPP

#include <list>
#include "benchmark.hpp"


struct benchmark_runner
{
  // add a library
  void add_library(const boost::shared_ptr<benchmark_base>& b);
  
  // benches all operations given in ops
  void run(const std::vector<std::string>& ops,
           double sample_time,
           const std::vector<std::string>& vec1,
           const std::vector<std::string>& vec2,
           const std::vector<std::string>& hex_str_vec,
           const std::vector<std::string>& dec_str_vec);

  void execute_bench_op(const std::string& op, double sample_time);

  std::list<benchmark_result>& get_results() { return results_; }

private:

  typedef std::vector<boost::shared_ptr<benchmark_base> > library_vec;
  typedef std::list<benchmark_result>                     result_list;

  library_vec libs_;
  result_list results_;
};


#endif

