// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_TOOLS_BENCHMARK_HPP
#define BOOST_MP_MATH_TOOLS_BENCHMARK_HPP

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/timer.hpp>

enum op_type
{
  op_ctor_dec,
  op_ctor_hex,
  op_to_dec,
  op_to_hex,
  op_add,
  op_subtract,
  op_multiply,
  op_divide,
  op_modulo,
  op_square,
  op_modpow
};

const char* to_string(op_type op);


struct benchmark_result
{
  std::string libname;
  std::string op;

  double        sample_time;
  double        total_time;
  // time for a single op execution in milliseconds
  std::vector<double> ops;

  bool operator < (const benchmark_result& r) const { return op < r.op; }

  void clear()
  {
    total_time = 0;
    ops.clear();
  }
};


struct benchmark_base
{
  typedef std::vector<std::string> string_vec;

  benchmark_base(const std::string& name, const std::string& version);
  
  virtual ~benchmark_base();

  virtual void set_num_input_samples(unsigned int num)
  {
    num_input_samples_ = num;
  }
  
  void run(benchmark_result& r, const std::string& op, double sample_time);

  // these need to be invoked because we need these for all other benchmarks
  virtual void construct_operand_1(const std::string& src, unsigned int i) = 0;
  virtual void construct_operand_2(const std::string& src, unsigned int i) = 0;

  //virtual bool has_op(op_type) = 0;

  // now come the individual benchmarks
  virtual void ctor_dec(benchmark_result&, double sample_time) = 0;
  virtual void ctor_hex(benchmark_result&, double sample_time) = 0;
  virtual void to_dec  (benchmark_result&, double sample_time) = 0;
  virtual void to_hex  (benchmark_result&, double sample_time) = 0;
  virtual void add     (benchmark_result&, double sample_time) = 0;
  virtual void subtract(benchmark_result&, double sample_time) = 0;
  virtual void multiply(benchmark_result&, double sample_time) = 0;
  virtual void divide  (benchmark_result&, double sample_time) = 0;
  virtual void modulo  (benchmark_result&, double sample_time) = 0;
  virtual void square  (benchmark_result&, double sample_time) = 0;
  virtual void modpow  (benchmark_result&, double sample_time) = 0;

  const std::string& name   () const { return name_;    }
  const std::string& version() const { return version_; }

  string_vec dec_str; // inputs for ctor_dec
  string_vec hex_str; // inputs for ctor_hex

protected:

  const std::string name_;
  const std::string version_;
  unsigned int      num_input_samples_;
};


template<class MPInt>
struct benchmark
:
  benchmark_base
{
  typedef MPInt mp_int_type;
  typedef std::vector<mp_int_type> mp_int_vec;

  benchmark(const std::string& name, const std::string& version)
  :
    benchmark_base(name, version)
  {}

  virtual ~benchmark();

  virtual void clear_dst_vector() = 0;

  virtual void set_num_input_samples(unsigned int num);

  template<class F>
  void do_bench(F f, benchmark_result& r, double sample_time);
  template<class F>
  void do_bench2(F f, benchmark_result& r, double sample_time);

  mp_int_vec dst;   // destination vector
  mp_int_vec src1;  // operand 1 vector
  mp_int_vec src2;  // operand 2 vector
  std::string str; // holds result of to_dec, to_hex string conversions
};


template<class MPInt>
benchmark<MPInt>::~benchmark()
{}

template<class MPInt>
void benchmark<MPInt>::set_num_input_samples(unsigned int num)
{
  dst.resize(num);
  src1.resize(num);
  src2.resize(num);
  benchmark_base::set_num_input_samples(num);
}

template<class MPInt>
template<class BenchFunctor>
void benchmark<MPInt>::do_bench(BenchFunctor f, benchmark_result& r, double sample_time)
{
  boost::timer total;
  boost::timer op_time;
  for (unsigned int i = 0; i < num_input_samples_; ++i)
  {
    unsigned long num_ops_executed = 0;
    op_time.restart();
    for (; op_time.elapsed() < sample_time; ++num_ops_executed)
      f(i);
    // push the number of ops per sample_time
    r.ops.push_back(num_ops_executed/op_time.elapsed());
  }
  r.total_time = total.elapsed();
  // clear dst vector for next test
  clear_dst_vector();
}


typedef boost::shared_ptr<benchmark_base> benchmark_ptr;


// This class simply holds all available benchmark libraries
struct benchmark_manager
{
  typedef std::vector<benchmark_ptr> b_set;
  
  void add(const benchmark_ptr& p);

  const benchmark_ptr& get(const std::string name) const;

  void print_available_libs(std::ostream& out) const;

  b_set b_;
};



#endif

