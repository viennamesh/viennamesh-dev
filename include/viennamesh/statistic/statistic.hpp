#ifndef VIENNAMESH_STATISTICS_STATISTIC_HPP
#define VIENNAMESH_STATISTICS_STATISTIC_HPP

#include "viennamesh/statistics/element_metrics.hpp"

namespace viennamesh
{

  template<typename NumericT>
  class statistic
  {

    template<typename T>
    friend std::ostream & operator <<(std::ostream & stream, statistic<T> const & stats);

  public:

    statistic()
    {
      set_bin_count(10);
    }

    statistic(size_t num_bins)
    {
      set_bin_count(num_bins);
      clear();
    }

    void clear()
    {
      sum_ = 0;
      std::fill(bins.begin(), bins.end(), 0);
    }

    template<typename MeshT, typename FunctorT>
    void operator()(MeshT const & mesh, FunctorT functor)
    {
      clear();

      typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

      ConstCellRangeType cells(mesh);
      ConstCellIteratorType cit = cells.begin();

      count_ = cells.size();

      min_ = max_ = functor(*cit++);

      for (; cit != cells.end(); ++cit)
      {
        NumericT cell_metric = functor(*cit);
        min_ = std::min(min_, cell_metric);
        max_ = std::max(max_, cell_metric);
        sum_ += cell_metric;
      }

      for (cit = cells.begin(); cit != cells.end(); ++cit)
      {
        NumericT cell_metric = functor(*cit);
        ++bins[get_bin(cell_metric)];
      }
    }

    size_t bin_count() const { return bins.size(); }
    void set_bin_count(size_t num_bins) { bins.resize(num_bins); }

    NumericT min() const { return min_; }
    NumericT max() const { return max_; }
    NumericT sum() const { return sum_; }
    size_t count() const { return count_; }

    NumericT mean() const { return sum() / count(); }


  private:

    size_t get_bin(NumericT value) const
    {
      if (max_ == min_)
        return 0;

      if (value < min_)
        return 0;

      if (value >= max_)
        return bins.size()-1;

      return (value-min_) / (max_-min_) * bin_count();
    }


    NumericT sum_;
    size_t count_;

    NumericT min_;
    NumericT max_;
    std::vector<size_t> bins;
  };


  template<typename NumericT>
  std::ostream & operator <<(std::ostream & stream, statistic<NumericT> const & stats)
  {
    stream << "min  = " << stats.min() << "\n";
    stream << "max  = " << stats.max() << "\n";
    stream << "mean = " << stats.mean() << "\n";

    NumericT tmp = (stats.max()-stats.min())/static_cast<NumericT>(stats.bin_count());

    for (size_t i = 0; i < stats.bin_count(); ++i)
    {
      stream << "  [" << stats.min() + static_cast<NumericT>(i)*tmp << "," <<
                         stats.min() + static_cast<NumericT>(i+1)*tmp << "] = " <<
                         stats.bins[i];

      if (i != stats.bin_count()-1)
        stream << "\n";
    }

    return stream;
  }

}

#endif
