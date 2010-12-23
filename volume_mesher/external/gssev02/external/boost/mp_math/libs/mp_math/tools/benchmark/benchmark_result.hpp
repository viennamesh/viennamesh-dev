// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)




struct benchmark_result
{
  std::string libname;
  std::string op;

  double        total_time;
  unsigned long total_ops;
  std::vector<unsigned long> ops;

  bool operator < (const benchmark_result& r) const { return op < r.op; }

  void clear()
  {}
};



struct benchmark_summary
{
  typedef std::vector<std::string> string_list;
  // TODO need a list and a set view of op_result
  typedef std::set<benchmark_result>         result_list;
  typedef result_list::const_iterator const_result_iterator;


  result_list results;

  void write_data_files(const string_list&);
  void write_gnuplot_scripts();
  void write_summary_file();

  bool results_for_op_exist(const std::string& opname) const;
};



void benchmark_summary::write_data_files(const string_list& ops) const
{
  typedef std::vector<std::string> string_list;
  for (string_list::const_iterator op = ops.begin(); op != ops.end(); ++op)
  {
    if (!results_for_op_exist(op))
      continue;

    const std::string filename(std::string(to_string(op)) + ".dat");
    std::ofstream file(filename.c_str());
    if (!file.is_open())
      throw std::runtime_error("couldn't open data file");

    const unsigned int num_ops = results.count(op);
    const_result_iterator first = results.lower_bound(op);
    const_result_iterator last = results.upper_bound(op);

    for (unsigned int i = 0; i < num_input_samples_; ++i)
    {
      while (first != last)
      {
        file << first->ops.at(i);
        
        if (++first != last)
          file << "\t";
      }
      file << "\n";
    }
  }
}

bool benchmark_summary::results_for_op_exist(const std::string& opname) const
{
  return results.find(benchmark_result("", opname)) != results.end();
}




benchmark_runner::get_benchmark_result();

