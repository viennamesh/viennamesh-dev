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

#include "make_statistic.hpp"
#include "statistic.hpp"

namespace viennamesh
{
  make_statistic::make_statistic() {}
  std::string make_statistic::name() { return "make_statistic"; }

  bool make_statistic::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    data_handle<viennamesh_string> metric_type = get_required_input<viennamesh_string>("metric_type");
    data_handle<viennagrid_numeric> histogram_bins = get_input<viennagrid_numeric>("histogram_bin");

    data_handle<viennagrid_numeric> histogram_min = get_input<viennagrid_numeric>("histogram_min");
    data_handle<viennagrid_numeric> histogram_max = get_input<viennagrid_numeric>("histogram_max");
    data_handle<int> histogram_bin_count = get_input<int>("histogram_bin_count");


    typedef viennagrid::mesh                                  MeshType;
    typedef viennagrid::result_of::element<MeshType>::type    ElementType;

    typedef viennamesh::statistic<viennagrid_numeric>         StatisticType;
    typedef StatisticType::histogram_type                     HistogramType;
    StatisticType statistic;


    if (histogram_bins.valid())
    {
      std::vector<viennagrid_numeric> bins;
      for (int i = 0; i != histogram_bins.size(); ++i)
        bins.push_back( histogram_bins(i) );

      statistic.set_histogram( StatisticType::histogram_type::make(bins.begin(), bins.end()) );
    }
    else if (histogram_min.valid() && histogram_max.valid() && histogram_bin_count.valid())
    {
      statistic.set_histogram( StatisticType::histogram_type::make_uniform(histogram_min(), histogram_max(), histogram_bin_count()) );
    }
    else
    {
      error(1) << "No histogram configuration provided" << std::endl;
      return false;
    }

    {
      viennamesh::LoggingStack stack( std::string("metric type \"") + metric_type() + "\"" );

      statistic.clear();

      if (metric_type() == "aspect_ratio")
        statistic( input_mesh(), viennamesh::aspect_ratio<ElementType> );
      else if (metric_type() == "condition_number")
        statistic( input_mesh(), viennamesh::condition_number<ElementType> );
//       else if (metric_type() == "min_angle")
//         statistic( input_mesh(), viennamesh::min_angle<ElementType> );
//       else if (metric_type() == "max_angle")
//         statistic( input_mesh(), viennamesh::max_angle<ElementType> );
      else if (metric_type() == "min_dihedral_angle")
        statistic( input_mesh(), viennamesh::min_dihedral_angle<ElementType> );
      else if (metric_type() == "radius_edge_ratio")
        statistic( input_mesh(), viennamesh::radius_edge_ratio<ElementType> );
      else
      {
        error(1) << "Metric type \"" << metric_type() << "\" is not supported" << std::endl;
        return false;
      }

      statistic.normalize();
      info(5) << statistic << std::endl;


      std::vector<viennagrid_numeric> bins;
      for (HistogramType::const_iterator bit = statistic.histogram().begin(); bit != statistic.histogram().end(); ++bit)
        bins.push_back( (*bit).second );
      bins.push_back( statistic.histogram().overflow_bin() );

      data_handle<viennagrid_numeric> output_bins = make_data<viennagrid_numeric>();
      output_bins.set( bins );
      set_output( "bins", output_bins );

      set_output( "min", statistic.min() );
      set_output( "max", statistic.max() );

    }


    return true;
  }

}
