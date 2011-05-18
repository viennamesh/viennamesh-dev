// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric> // accumulate
#include <sstream>
#include <stdexcept>
#include <boost/random.hpp>
#include <boost/mp_math/mp_int.hpp>

#include "modes.hpp"

std::string create_random_hex_string(unsigned size_in_bits)
{
  std::string s;
  s.reserve(size_in_bits/4);

  boost::mt19937 r;
  boost::uniform_smallint<char> u(0,15);
  boost::variate_generator<boost::mt19937&, boost::uniform_smallint<char> > vg(r, u);
  // one hex digit occupies 4 bits
  for (unsigned i = 0; i < (size_in_bits+3)/4; ++i)
  {
    char tmp = vg();
    if (tmp < 10)
      tmp = '0' + tmp;
    else
      tmp = 'a' + (tmp-10);
    s.push_back(tmp);
  }
  return s;
}

std::string create_random_dec_string(unsigned size_in_digits)
{
  std::string s;
  s.reserve(size_in_digits);

  boost::mt19937 r;
  boost::uniform_smallint<char> u(0,9);
  boost::variate_generator<boost::mt19937&, boost::uniform_smallint<char> > vg(r, u);
  
  for (unsigned i = 0; i < size_in_digits; ++i)
    s.push_back('0' + vg());
  
  return s;
}


void data_file::write(const std::string& prefix) const
{
  const std::string filename(prefix + op + ".dat");
  std::ofstream file(filename.c_str());
  if (!file.is_open())
    throw std::runtime_error("couldn't open data file");

  for (unsigned int i = 0; i < cols.front().data.size(); ++i)
  {
    file << x_values.at(i) << "\t";

    std::list<column>::const_iterator it = cols.begin();
    
    while (it != cols.end())
    {
      file << it->data.at(i);
        
      if (++it != cols.end())
        file << "\t";
    }
    file << "\n";
  }
}


void write_gnuplot_scripts(unsigned int x, unsigned int y,
                           const std::string& modeprefix,
                           const std::string& xlabel,
                           const std::string& ylabel,
                           const std::list<data_file>& dfiles)
{
  for (std::list<data_file>::const_iterator df = dfiles.begin();
      df != dfiles.end(); ++df)
  {
    const std::string filename(modeprefix + df->op + ".plt");
    std::ofstream file(filename.c_str());
    if (!file.is_open())
      throw std::runtime_error("couldn't open plot file");

    file << "set title \"" << df->op << "\"\n"
            "set xlabel " << "\"" << xlabel << "\"\n"
            "set ylabel " << "\"" << ylabel << "\"\n"
            "set autoscale\n"
            "set term png size " << x << "," << y << "\n"
            "set output \"" << modeprefix << df->op << ".png\"\n";
    file << "plot \\\n";
    
    std::list<data_file::column>::const_iterator c = df->cols.begin();
    int count = 2;
    while (c != df->cols.end())
    {
      file << '"' << modeprefix << df->op << ".dat\" using 1:" << count++
           << " title \"" << c->libname << "\" with lines";
      if (++c != df->cols.end())
        file << ",\\\n";
    }
    file << std::endl;
  }
}

void write_data_files(const std::string& modeprefix,
                      const std::list<data_file>& dfiles)
{
  for (std::list<data_file>::const_iterator it = dfiles.begin();
      it != dfiles.end(); ++it)
    it->write(modeprefix);
}


mode1::mode1(unsigned int num_input_samples,
             unsigned long operand_size_beg,
             unsigned long operand_size_end)
:
  beg_(operand_size_beg ? operand_size_beg : 1), // zero length not allowed
  end_(operand_size_end),
  num_input_samples_(num_input_samples)
{
  std::ofstream iv_file("mode1_input_vecs.dat");
  if (!iv_file.is_open())
    throw std::runtime_error("couldn't open file mode1_input_vecs.dat");

  const unsigned long min_size = 
    std::min(get_operand1_size(0),
             get_operand2_size(0));
  
  const unsigned long max_size = 
    std::max(get_operand1_size(num_input_samples_),
             get_operand2_size(num_input_samples_));

  std::cout << "Creating " << num_input_samples_
            << " operand indices for numbers between "
            << min_size << " and " << max_size << " bits."
            << std::endl;
  
  const std::string hex_s = create_random_hex_string(max_size);

  std::cout << "creating input vectors";
  std::cout.flush();

  vec1.reserve(num_input_samples_);
  vec2.reserve(num_input_samples_);
  hex_str_vec.reserve(num_input_samples_);
  dec_str_vec.reserve(num_input_samples_);

  // create input vector where number size is determined by the sine function
  const unsigned int stepsize = num_input_samples_ / 10;
  for (unsigned int i = 0; i < num_input_samples_; ++i)
  {
    const unsigned long size1 = get_operand1_size(i);
    const unsigned long size2 = get_operand2_size(i);
    
    // divide by 4 because hex_s has max/4 hex digits
    vec1.push_back(hex_s.substr(0, size1/4));
    vec2.push_back(hex_s.substr(0, size2/4));
    
    // output the input sizes for a graph here
    iv_file << size1 << "\t" << size2 << "\n";
    
    if (((i+1) % stepsize) == 0)
    {
      std::cout << ".";
      std::cout.flush();
    }      
  }

  std::cout << std::endl;

  iv_file.close();

  // init ctor strings
  for (unsigned int i = 0; i < num_input_samples_; ++i)
  {
    hex_str_vec.push_back(hex_s.substr(0, (get_operand1_size(i)+3)/4));
    // use mp_int to ensure decimal string results in the same number of bits as
    // the hex string
    boost::mp_math::mp_int<> tmp(hex_str_vec.back(), std::ios::hex);
    dec_str_vec.push_back(tmp.to_string<std::string>());
  }
}

void mode1::create_data_files(std::list<benchmark_result>& results)
{
  std::list<benchmark_result>::const_iterator r = results.begin();
  
  // expects result list to be sorted by op name
  while (r != results.end())
  {
    data_file d;

    for (unsigned i = 0; i < num_input_samples_; ++i)
      d.x_values.push_back(i);

    std::list<benchmark_result>::const_iterator cur = r;
    while (r != results.end() && r->op == cur->op)
    {
      data_file::column c;

      c.libname = r->libname;
      c.data = r->ops;
      
      // scale from ops per sample_time to ops per millisecond
      for (std::vector<double>::iterator it = c.data.begin();
          it != c.data.end(); ++it)
        *it /= (r->sample_time * 1000.0);

      c.sum = std::accumulate(r->ops.begin(), r->ops.end(), 0.);
      d.cols.push_back(c);
      ++r;
    }

    d.op = cur->op;

    d.cols.sort();

    dfiles_.push_back(d);
  }
}

void mode1::write_input_vector_plotfile(unsigned int x, unsigned int y) const
{
  std::ofstream pf("mode1_input_vecs.plt");
  if (!pf.is_open())
    throw std::runtime_error("couldn't open plotfile");
  pf << "set term png size " << x << "," << y << "\n"
        "set output \"mode1_input_vecs.png\"\n"
        "set title \"input size of operands\"\n"
        "set key left\n"
        "set xlabel \"operand number\"\n"
        "set ylabel \"size in bits\"\n"
        "plot \"mode1_input_vecs.dat\" using 0:1 title \"operand 1 size\" with lines,\\\n"
        "     \"mode1_input_vecs.dat\" using 0:2 title \"operand 2 size\" with lines";
}

void mode1::write_summary_file() const
{
  std::ostringstream s;
  for (std::list<data_file>::const_iterator d = dfiles_.begin();
      d != dfiles_.end(); ++d)
  {
    s << d->op << "------------------------------------\n";
    for (std::list<data_file::column>::const_iterator c = d->cols.begin();
        c != d->cols.end(); ++ c)
    {
      s << c->libname << " total ops = " << c->sum << "\n";
    }
  }

  std::cout << s.str();

  std::ofstream file("mode1_summary.txt");
  if (!file.is_open())
    throw std::runtime_error("couldn't open mode1_summary.txt file");
  file << s.str();
}

void mode1::write_results(unsigned int x, unsigned int y) const
{
  write_input_vector_plotfile(x, y);
  
  std::cout << "writing data files..." << std::endl;
  write_data_files("mode1_", dfiles_);
  
  std::cout << "writing gnuplot scripts..." << std::endl;
  write_gnuplot_scripts(x, y, "mode1_", "operand index", "ops/msec", dfiles_);
  
  write_summary_file();
}


unsigned long mode1::get_operand1_size(unsigned int sample_number) const
{
  // linear growth
  const double m = static_cast<double>(end_ - beg_) /
                   static_cast<double>(num_input_samples_);
  return (m * sample_number) + beg_;
}

unsigned long mode1::get_operand2_size(unsigned int sample_number) const
{
  // modified sine curve
  const double pi = 3.141592654;
  const double sample_num_to_rad = num_input_samples_ / (4.5 * pi);
  
  // scale sample_number into the range [0...4.5*PI]
  const double x = sample_number / sample_num_to_rad;

  const double y_val = std::sin(x) + 1.0;

  const double y_top = get_operand1_size(sample_number);
  const double p0 = pi;
  double y_bottom;
  if (x <= p0)
    y_bottom = y_top / (x / p0 * 2.0 + 1.0) + 1.0;
  else
    y_bottom = y_top / 3.0;

  // we want to scale the curve at the point p1 to be above y_top at x = 2.5*pi
  const double p1 = 2.5 * pi;
  
  // here we use a function of the form y = -ax^2 + b to scale the curve at (and
  // around) p1
  const double a = 1.0/(p1 * p1);
  double adjust = -a * ((x-p1) * (x-p1)) + 1.0;
  // scale to be 10% above p1
  adjust *= 1.1;
  // stop scaling once we're past p1 and 'adjust' falls below 1.0
  if (x > p1 && adjust < 1.0)
    adjust = 1.0;
  
  return static_cast<unsigned long>(y_bottom * adjust * y_val + y_bottom);
}


mode2::mode2(unsigned int pow_from, unsigned int pow_to, unsigned int stepsize)
:
  pow_from_(pow_from),
  pow_to_(pow_to),
  pow_step_(stepsize)
{
  const unsigned long max_size = 1UL << pow_to;
  
  const std::string hex_s = create_random_hex_string((max_size+3)/4);

  for (; pow_from < pow_to; pow_from += stepsize)
  {
    const unsigned long size = 1UL << pow_from;
    
    vec1.push_back(hex_s.substr(0, size/4));
    
    hex_str_vec.push_back(hex_s.substr(0, (size+3)/4));
    
    boost::mp_math::mp_int<> tmp(hex_str_vec.back(), std::ios::hex);
    dec_str_vec.push_back(tmp.to_string<std::string>());
  }

  vec2 = vec1;
}


void mode2::create_data_files(std::list<benchmark_result>& results)
{
  std::list<benchmark_result>::const_iterator r = results.begin();
  
  // expects result list to be sorted by op name
  while (r != results.end())
  {
    data_file d;
    
    for (unsigned i = pow_from_; i < pow_to_; i += pow_step_)
      d.x_values.push_back(i);

    std::list<benchmark_result>::const_iterator cur = r;
    while (r != results.end() && r->op == cur->op)
    {
      data_file::column c;

      c.libname = r->libname;
      c.data = r->ops;
      
      // scale from ops per sample_time to seconds per op
      for (std::vector<double>::iterator it = c.data.begin();
          it != c.data.end(); ++it)
      {
        *it /= r->sample_time;
        *it = 1.0 / *it;
      }

      c.sum = std::accumulate(r->ops.begin(), r->ops.end(), 0.);
      d.cols.push_back(c);
      ++r;
    }

    d.op = cur->op;

    d.cols.sort();
    dfiles_.push_back(d);
  }
}

void mode2::write_summary_file() const
{
  std::ostringstream s;
  for (std::list<data_file>::const_iterator d = dfiles_.begin();
      d != dfiles_.end(); ++d)
  {
    s << d->op << "------------------------------------\n";
    for (std::list<data_file::column>::const_iterator c = d->cols.begin();
        c != d->cols.end(); ++ c)
    {
      s << c->libname << " total seconds = " << c->sum << "\n";
    }
  }

  std::cout << s.str();

  std::ofstream file("mode2_summary.txt");
  if (!file.is_open())
    throw std::runtime_error("couldn't open mode2_summary.txt file");
  file << s.str();
}

void mode2::write_results(unsigned int x, unsigned int y) const
{
  std::cout << "writing data files..." << std::endl;
  write_data_files("mode2_", dfiles_);
  
  std::cout << "writing gnuplot scripts..." << std::endl;
  write_gnuplot_scripts(x, y, "mode2_", "operand size (2^x)", "seconds/op",
                        dfiles_);

  write_summary_file();
}

