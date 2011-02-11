// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>

#include "benchmark_runner.hpp"

#include "benchmark_mp_math.hpp"
#include "benchmark_libtom.hpp"
#include "benchmark_gmp.hpp"

#include "modes.hpp"


const char* version = "0.4";


struct config
{
  // mode can be 0, 1 or 2 which is:
  // 0) run all modes
  // 1) run with continuously sized operands, this benchmark takes long because
  //    it uses num_input_samples (default = 1000) operands, so it should be
  //    used for smaller numbers.
  // 2) run benchmark with operands that are a power of two, this should be used
  //    to get a 'quick' look into how the functions perform for very large
  //    numbers.
  unsigned int mode;

  // mode 1 options
  unsigned int num_input_samples;
  double       max_error;
  double       sample_time;
  std::pair<unsigned int, unsigned int> range;

  // mode 2 options
  unsigned int pow_from;
  unsigned int pow_to;
  unsigned int pow_step;

  // general options
  unsigned int graph_x, graph_y;

  typedef std::vector<std::string> string_list;
  // we receive these two strings from the cmdline and need to tokenize them
  std::string ops_string;
  std::string libs_string;
  // the tokenized result
  string_list ops;  // ops that we shall execute
  string_list libs; // libs that we shall benchmark

  config();

  void show_available_ops (std::ostream&) const;

  const string_list& available_ops () const { return available_ops_; }

  double estimated_run_time() const;

private:

  string_list available_ops_;
};


config::config()
{
  available_ops_.push_back("ctor_dec");
  available_ops_.push_back("ctor_hex");
  available_ops_.push_back("to_dec");
  available_ops_.push_back("to_hex");
  available_ops_.push_back("add");
  available_ops_.push_back("subtract");
  available_ops_.push_back("multiply");
  available_ops_.push_back("divide");
  available_ops_.push_back("modulo");
  available_ops_.push_back("square");
  available_ops_.push_back("modpow");
}



void config::show_available_ops(std::ostream& os) const
{
  string_list::const_iterator op;
  for (op = available_ops_.begin(); op != available_ops_.end(); ++op)
    os << *op << "\n";
}

double config::estimated_run_time() const
{
  const double mode1_time =
    sample_time * num_input_samples * ops.size() * libs.size();
  
  const double mode2_time = 0.05 * ops.size() * libs.size();
  
  double sum = 0.0;
  if (mode == 0 || mode == 1)
    sum += mode1_time;
  if (mode == 0 || mode == 2)
    sum += mode2_time;

  return sum;
}


double calibrate_sample_time(double max_error)
{
  const int vec_length = 10;
  long vec[vec_length];
  double st = 0.01;
  
  int verified = 0;
  for (;;)
  {
    for (int k = 0; k < vec_length; ++k)
    {
      boost::timer t;
      boost::timer inner;
      long count = 0;
      while (inner.elapsed() < st)
        ++count;
      vec[k] = count;
    }
    // calculate error
    long average_value = 0;
    for (int i = 0; i < vec_length; ++i)
      average_value += vec[i];
    average_value /= vec_length;

    long average_deviation = 0;
    for (int i = 0; i < vec_length; ++i)
      average_deviation += std::abs(average_value - vec[i]);
    average_deviation /= vec_length;

    const double error = (double)average_deviation / (double)average_value;
    
    std::cout << "sample-time = " << st
              << ", current error = " << error;
    if (verified)
      std::cout << " (verifying)";
    std::cout << std::endl;

    if (error < max_error)
    {
      if (verified == 2)
        return st;
      ++verified;
      continue;
    }
    else
      verified = 0;
    if (error / max_error > 2.0)
      st += 0.01;
    else
      st += 0.002; 
    if (st >= 0.05)
      return 0.05;
  }
}




int main(int argc, char** argv)
{
  typedef config::string_list string_list;
  
  std::ios::sync_with_stdio(false);

  try
  {

  benchmark_manager bmgr;
  bmgr.add(benchmark_ptr(new benchmark_mp_math()));
  bmgr.add(benchmark_ptr(new benchmark_libtom()));
  bmgr.add(benchmark_ptr(new benchmark_gmp()));

  config c;

  using namespace boost::program_options;

  options_description opts
    ("This program benchmarks different multiprecision integer "
     "libraries and outputs data files for use with gnuplot "
     "(http://www.gnuplot.com).\n"
     "options"
    );

  opts.add_options()
    ("help,h",    "show this message")

    ("version,v", "print version")

    ("mode,m",
        value(&c.mode)->default_value(1),
          "0 = run all benchmarks\n"
          "1 = run benchmark meant to test small numbers\n"
          "2 = run benchmark meant to test large numbers")

    ("ops", 
        value(&c.ops_string),
          "the operations to benchmark")
    
    ("libs",
        value(&c.libs_string),
          "the libraries to benchmark")
    
    ("list-ops",  "lists available operations")
    
    ("list-libs", "lists available libraries")

    ("x",
        value(&c.graph_x)->default_value(1024),
          "width of graphs created by gnuplot")
    ("y",
        value(&c.graph_y)->default_value(768),
          "height of graphs created by gnuplot")
    ;

  options_description mode1_opts("Mode 1 options");

  mode1_opts.add_options()
    ("num-input-samples,n",
        value(&c.num_input_samples)->default_value(1000),
          "number of input samples to create")

    ("max-error,e",
        value(&c.max_error)->default_value(0.1, "0.1"),
          "this value is used to calculate the sample time, you should not "
          "need to modify it")
    
    ("sample-time,s",
        value(&c.sample_time)->default_value(0.035, "0.035"),
          "directly specify the sample time in seconds, it is used to reduce "
          "the error of the measurements")
    
    ("range-beg,a",
        value(&c.range.first)->default_value(32),
          "range of numbers, measured in bits")
    
    ("range-end,b",
        value(&c.range.second)->default_value(3072),
          "range of numbers, measured in bits")
    ;

  options_description mode2_opts("Mode 2 options");

  mode2_opts.add_options()

    ("pow-from,f",
        value(&c.pow_from)->default_value(7),
          "the first power of two to use")

    ("pow-to,t",
        value(&c.pow_to)->default_value(21),
          "the last power of two to use")

    ("pow-step,q",
        value(&c.pow_step)->default_value(1),
          "the stepsize used to iterate through the powers of two")
    ;

  opts.add(mode1_opts);
  opts.add(mode2_opts);

  variables_map vm;

  store(parse_command_line(argc, argv, opts), vm);

  notify(vm);

  if (vm.count("help"))
  {
    std::cout << opts << std::endl;
    return 0;
  }
  if (vm.count("version"))
  {
    std::cout << version << std::endl;
    return 0;
  }
  if (vm.count("list-ops"))
  {
    c.show_available_ops(std::cout);
    return 0;
  }
  if (vm.count("list-libs"))
  {
    bmgr.print_available_libs(std::cout);
    return 0;
  }
  if (!vm.count("ops"))
    c.ops = c.available_ops();
  else
  {
    boost::char_separator<char> sep(" ,");
    boost::tokenizer<boost::char_separator<char> > tokens(c.ops_string, sep);
    std::copy(tokens.begin(), tokens.end(), std::back_inserter(c.ops));
  }

  std::cout << "Ops that we are about to benchmark:\n";
  for (string_list::const_iterator it = c.ops.begin(); it != c.ops.end(); ++it)
    std::cout << "- " << *it << std::endl;

  if (!vm.count("libs"))
  {
    for (benchmark_manager::b_set::const_iterator it = bmgr.b_.begin();
        it != bmgr.b_.end(); ++it)
    c.libs.push_back((*it)->name());
  }
  else
  {
    boost::char_separator<char> sep(" ,");
    boost::tokenizer<boost::char_separator<char> > tokens(c.libs_string, sep);
    std::copy(tokens.begin(), tokens.end(), std::back_inserter(c.libs));
  }


  // switch to a new subdir to store the benchmark results
  const std::time_t time = std::time(0);
  char timestring[32];
  std::strftime(timestring, 32, "%Y %m %d %H:%M", std::localtime(&time));
  
  boost::filesystem::create_directory(timestring);
  boost::filesystem::current_path(timestring);


  benchmark_runner b_runner;

  std::cout << "Libraries that we are about to benchmark:\n";
  for (string_list::const_iterator lib = c.libs.begin();
       lib != c.libs.end(); ++lib)
  {
    const benchmark_ptr p(bmgr.get(*lib));
    std::cout << "- " << p->name() << "\n";
    b_runner.add_library(p);
  }

  if (vm.count("sample-time") && c.sample_time == 0.0)
  {
    std::cout << "calibrating sample time..." << std::endl;
    c.sample_time = calibrate_sample_time(c.max_error);
  }
    
  std::cout << "Sample time is: " << c.sample_time << " sec" <<  std::endl;

  const unsigned long runtime =
    static_cast<unsigned long>(c.estimated_run_time()/60.0);
  std::cout << "Estimated run time of benchmark is at least "
            << runtime << " minute(s), but will be much longer for "
            << "very large numbers." << std::endl;

  if (c.mode == 0 || c.mode == 1)
  {
    std::cout << "Entering mode 1..." << std::endl;
    mode1 m1(c.num_input_samples, c.range.first, c.range.second);
    b_runner.run(c.ops, c.sample_time,
                 m1.vec1, m1.vec2,
                 m1.hex_str_vec, m1.dec_str_vec);
    m1.create_data_files(b_runner.get_results());
    m1.write_results(c.graph_x, c.graph_y);
  }

  if (c.mode == 0 || c.mode == 2)
  {
    std::cout << "Entering mode 2..." << std::endl;
    mode2 m2(c.pow_from, c.pow_to, c.pow_step);
    b_runner.run(c.ops, 0.05,
                 m2.vec1, m2.vec2,
                 m2.hex_str_vec, m2.dec_str_vec);
    m2.create_data_files(b_runner.get_results());
    m2.write_results(c.graph_x, c.graph_y);
  }

  std::cout << "done with benchmarking!" << std::endl;

  std::cout << "creating graphs via gnuplot...\n";
  std::cout << "calling \'gnuplot *.plt\'" << std::endl;
  const int ret = std::system("gnuplot *.plt");
  if (ret)
    std::cout << "something went wrong, gnuplot call exited with code " << ret
              << std::endl;
  else
    std::cout << "done" << std::endl;

  if (boost::filesystem::is_empty(boost::filesystem::current_path()))
    boost::filesystem::remove(boost::filesystem::current_path());

  }
  catch (const std::exception& e)
  {
    std::cerr << "caught exception: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}


