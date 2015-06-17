#ifndef VIENNAMESH_STATISTICS_STATISTIC_HPP
#define VIENNAMESH_STATISTICS_STATISTIC_HPP

/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include <limits>
#include <set>
#include "element_metrics.hpp"

namespace viennamesh
{
  template<typename NumericT>
  NumericT infinity()
  { return std::numeric_limits<NumericT>::infinity(); }

  template<typename NumericT, typename BinT>
  class histogram
  {
    typedef std::map<NumericT, BinT> BinContainerType;
    typedef typename BinContainerType::iterator iterator;

    iterator begin() { return bins.begin(); }
    iterator end() { return bins.end(); }
  public:

    typedef histogram<NumericT, BinT> self_type;

    static self_type make_uniform( NumericT min, NumericT max, std::size_t bin_count )
    {
      self_type tmp;
      tmp.bins.clear();
      for (std::size_t i = 0; i < bin_count+1; ++i)
        tmp.bins[ min + i/static_cast<NumericT>(bin_count)*(max-min) ] = 0;
      return tmp;
    }

    template<typename BinBorderIteratorT>
    static self_type make( BinBorderIteratorT begin_it, BinBorderIteratorT const & end_it )
    {
      self_type tmp;
      tmp.bins.clear();
      for (; begin_it != end_it; ++begin_it)
        tmp.bins[ *begin_it ] = 0;
      return tmp;
    }

    void reset()
    {
      for (iterator bin_it = begin(); bin_it != end(); ++bin_it)
        bin_it->second = 0;
      overflow_bin_ = 0;
    }

    void increase(NumericT value, BinT to_increase = 1)
    {
      iterator bin_it = bin(value);
      if (bin_it != bins.end())
        bin_it->second += to_increase;
      else
        overflow_bin_ += to_increase;
    }

    BinT get(NumericT value) const
    {
      const_iterator bin_it = bin(value);
      if (bin_it != bins.end())
        return bin_it->second;
      else
        return overflow_bin_;
    }

    typedef typename BinContainerType::const_iterator const_iterator;

    const_iterator begin() const { return bins.begin(); }
    const_iterator end() const { return bins.end(); }

    std::pair<NumericT, NumericT> bin_interval(const_iterator bin_it) const
    {
      if (bin_it == bins.begin())
        return std::make_pair( -infinity<NumericT>(), bin_it->first );

      if (bin_it == bins.end())
        return std::make_pair( bins.rbegin()->first, infinity<NumericT>() );

      const_iterator prev_it = bin_it; --prev_it;
      return std::make_pair( prev_it->first, bin_it->first );
    }

    BinT overflow_bin() const { return overflow_bin_; }

    void normalize()
    {
      BinT sum = overflow_bin_;
      for (iterator it = begin(); it != end(); ++it)
        sum += (*it).second;

      for (iterator it = begin(); it != end(); ++it)
        (*it).second /= sum;
      overflow_bin_ /= sum;
    }

  private:

    iterator bin(NumericT value)
    { return bins.upper_bound(value); }
    const_iterator bin(NumericT value) const
    { return bins.upper_bound(value); }

    BinContainerType bins;
    BinT overflow_bin_;
  };



  template<typename NumericT, typename BinT>
  std::ostream & operator <<(std::ostream & stream, histogram<NumericT, BinT> const & hist)
  {
    std::pair<NumericT, NumericT> bin_interval;
    for (typename histogram<NumericT, BinT>::const_iterator bin_it = hist.begin(); bin_it != hist.end(); ++bin_it)
    {
      bin_interval = hist.bin_interval(bin_it);
      stream << "  [" << bin_interval.first << "," << bin_interval.second << "] = " << bin_it->second << "\n";
    }
    stream << "  [" << bin_interval.second << "," << infinity<NumericT>() << "] = " << hist.overflow_bin();

    return stream;
  }





  template<typename NumericT>
  class statistic
  {

    template<typename T>
    friend std::ostream & operator <<(std::ostream & stream, statistic<T> const & stats);

  public:

    typedef viennamesh::histogram<NumericT, viennagrid_numeric> histogram_type;

    statistic() {}

    void clear()
    {
      sum_ = 0;
      histogram_.reset();
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
        histogram_.increase( cell_metric );
        ordered_values_.insert( cell_metric );
      }
    }


    void set_histogram( histogram_type const & histogram_x )
    { histogram_ = histogram_x; }


    NumericT min() const { return min_; }
    NumericT max() const { return max_; }
    NumericT sum() const { return sum_; }
    size_t count() const { return count_; }

    void normalize()
    {
      histogram_.normalize();
    }

    NumericT mean() const { return sum() / count(); }
    NumericT median() const
    {
      typename std::multiset<NumericT>::iterator it = ordered_values_.begin();

      NumericT tmp;

      if (count() % 2 == 0)
      {
        std::advance(it,  count()/2-1);
        tmp = *it;
        ++it;
        tmp += *it;
        return tmp/2;
      }
      else
      {
        std::advance(it,  count()/2);
        return *it;
      }
    }
    histogram_type const & histogram() const { return histogram_; }


  private:

    NumericT sum_;
    size_t count_;

    NumericT min_;
    NumericT max_;

    std::multiset<NumericT> ordered_values_;

    histogram_type histogram_;
  };


  template<typename NumericT>
  std::ostream & operator <<(std::ostream & stream, statistic<NumericT> const & stats)
  {
    stream << "min    = " << stats.min() << "\n";
    stream << "max    = " << stats.max() << "\n";
    stream << "mean   = " << stats.mean() << "\n";
    stream << "median = " << stats.median() << "\n";
    stream << stats.histogram();

    return stream;
  }

}

#endif
