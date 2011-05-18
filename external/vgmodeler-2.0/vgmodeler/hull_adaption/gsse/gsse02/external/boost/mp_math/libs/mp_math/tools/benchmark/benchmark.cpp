// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "benchmark.hpp"
#include <iostream>
#include <stdexcept>


const char* to_string(op_type op)
{
  switch (op)
  {
    case op_ctor_dec: return "ctor_dec";
    case op_ctor_hex: return "ctor_hex";
    case op_to_dec:   return "to_dec";
    case op_to_hex:   return "to_hex";
    case op_add:      return "add";
    case op_subtract: return "subtract";
    case op_multiply: return "multiply";
    case op_divide:   return "divide";
    case op_modulo:   return "modulo";
    case op_square:   return "square";
    case op_modpow:   return "modpow";
    default:
      return "unknown op";
  }
}


benchmark_base::benchmark_base(const std::string& name, const std::string& version)
:
  name_(name),
  version_(version),
  num_input_samples_(0)
{}

benchmark_base::~benchmark_base()
{}

void benchmark_base::run(benchmark_result& r, const std::string& op, double sample_time)
{
  std::cout << "executing "
            << name_ << "->" << op << " ops";
  std::cout.flush();

  if      (op == "ctor_dec") ctor_dec(r, sample_time);
  else if (op == "ctor_hex") ctor_hex(r, sample_time);
  else if (op == "to_dec"  ) to_dec  (r, sample_time);
  else if (op == "to_hex"  ) to_hex  (r, sample_time);
  else if (op == "add"     ) add     (r, sample_time);
  else if (op == "subtract") subtract(r, sample_time);
  else if (op == "multiply") multiply(r, sample_time);
  else if (op == "divide"  ) divide  (r, sample_time);
  else if (op == "modulo"  ) modulo  (r, sample_time);
  else if (op == "square"  ) square  (r, sample_time);
  else if (op == "modpow"  ) modpow  (r, sample_time);
  else
  {
    std::cout << "unknown op: " << op << std::endl;
    throw std::runtime_error("benchmark_base::run: unknown op");
  }

  std::cout << '\t' << r.total_time << " sec" << std::endl;
}


void benchmark_manager::add(const benchmark_ptr& p)
{
  b_.push_back(p);
};

const benchmark_ptr& benchmark_manager::get(const std::string name) const
{
  for (b_set::const_iterator i = b_.begin(); i != b_.end(); ++i)
    if ((*i)->name() == name)
      return *i;
  throw std::runtime_error("unknown library");
}

void benchmark_manager::print_available_libs(std::ostream& out) const
{
  for (b_set::const_iterator it = b_.begin(); it != b_.end(); ++it)
    out << (*it)->name() << "\n";
}




