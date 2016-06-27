// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_TOOLS_MODES_HPP
#define BOOST_MP_MATH_TOOLS_MODES_HPP

#include <list>
#include <string>
#include <vector>

#include "benchmark.hpp"


std::string create_random_hex_string(unsigned size_in_bits);
std::string create_random_dec_string(unsigned size_in_digits);


struct data_file
{
  struct column
  {
    std::string libname;
    std::vector<double> data;
    double sum;
    bool operator < (const column& rhs) const { return libname < rhs.libname; }
  };

  std::string op;

  std::vector<unsigned> x_values;
  std::list<column> cols; // the y-values

  void write(const std::string& prefix) const;
};


void write_gnuplot_scripts(unsigned int x, unsigned int y,
                           const std::string& modeprefix,
                           const std::string& xlabel,
                           const std::string& ylabel,
                           const std::list<data_file>& dfiles);

void write_data_files(const std::string& modeprefix,
                      const std::list<data_file>& dfiles);


struct mode1
{
  mode1(unsigned int num_input_samples,
        unsigned long operand_size_beg,
        unsigned long operand_size_end);

  void create_data_files(std::list<benchmark_result>&);
  void write_input_vector_plotfile(unsigned int x, unsigned int y) const;
  void write_summary_file() const;

  void write_results(unsigned int x, unsigned int y) const;

  std::vector<std::string> hex_str_vec; // hex ctor strings
  std::vector<std::string> dec_str_vec; // dec ctor strings
  std::vector<std::string> vec1;
  std::vector<std::string> vec2;

private:

  unsigned long get_operand1_size(unsigned int sample_numer) const;
  unsigned long get_operand2_size(unsigned int sample_numer) const;

  unsigned long beg_, end_;
  unsigned int num_input_samples_;
  std::list<data_file> dfiles_;
};


struct mode2
{
  mode2(unsigned int pow_from, unsigned int pow_to, unsigned int stepsize);

  void create_data_files(std::list<benchmark_result>&);
  void write_summary_file() const;

  void write_results(unsigned int x, unsigned int y) const;

  std::vector<std::string> hex_str_vec; // hex ctor strings
  std::vector<std::string> dec_str_vec; // dec ctor strings
  std::vector<std::string> vec1;
  std::vector<std::string> vec2;

private:

  unsigned int pow_from_, pow_to_, pow_step_;
  std::list<data_file> dfiles_;
};


#endif

