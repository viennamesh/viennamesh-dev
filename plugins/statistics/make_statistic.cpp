/* ============================================================================
   Copyright (c) 2011-2016, Institute for Microelectronics,
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


/*Histograms features seem not to be fully functionable. With HIST = 0 all histogram related functions and structs are cut out.
    --> Set in statistic.hpp HIST = 1 if you want to enable histogram features
*/
#define HIST 0


/* This algorithm provides various statistical and mesh quality parameters for simplicial mesh. Currently, the implementation is optimized for
triangular meshes, for tetrahedra meshes only few shape quality metrics are implemented (see comments in metrics/).

Two basic modes of operation are available.
  -> Input: 1 mesh,    cell shape quality statistics
  -> Input: 2 meshes,  cell shape quality statistics and mesh comparison metrics

The cell shape quality parameters are ratios of geometric quantities defined in the header files of metrics/. A particular metric can be chosen
by setting the (required) input "metric_type". For each parameter min, max, mean and median values for the considered mesh can be calculated.
Optionally (input "good_element_threshold"), the number of 'high quality' cells can be evaluated. This requires the definition of a threshold value
in order to define the 'high quality' cell condition as PARAMETER < threshold (or PARAMETER > threshold). The choice of the comparison operator is
automatically deduced (see element_metrics.hpp).

By setting a second mesh as input ("original_mesh") further metrics are available. These metrics are based on mesh comparison and provide minimum distance,
curvature and surface area deviation metrics (see mesh_comparison.hpp). Additionally, a DIFFERENT cell quality metric is established. For the given
shape quality parameter ("metric_type") the median value for the original mesh is automatically set as threshold value, ignoring a possibly given
"good_element_threshold".

 */


namespace viennamesh
{
    make_statistic::make_statistic() {}

    std::string make_statistic::name()
    {
        return "make_statistic";
    }

    bool make_statistic::run(viennamesh::algorithm_handle &)
    {
        info(1) << "make_statistic started:" << std::endl;

        mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");

        /*original mesh for comparison statistics (hausdorff distance, curvature difference...)*/
        mesh_handle original_mesh = get_input<mesh_handle>("original_mesh");

        /*All provided cell quality metric types are implemented as header files, which are located in statistics/metrics.
         * Note that most metrics are only implemented for triangles*/
        data_handle<viennamesh_string> metric_type = get_required_input<viennamesh_string>("metric_type");

        /*Decision threshold for triangle qualitity classification
         * E.g., radius_ratio < 1.5
         * Whether '<' or '>' is used for comparison is deduced from metric_ordering_tag*/
        data_handle<viennagrid_numeric> good_element_threshold = get_input<viennagrid_numeric>("good_element_threshold");

        /* comprehensive mesh quality metric is defined as
             alpha * (1 - good_elements_counted/count) + beta * min_dist_rms + gamma * mean_curvature + delta * volume_deviation;
        */
        data_handle<viennagrid_numeric> alpha = get_input<viennagrid_numeric>("alpha");
        data_handle<viennagrid_numeric> beta = get_input<viennagrid_numeric>("beta");
        data_handle<viennagrid_numeric> gamma = get_input<viennagrid_numeric>("gamma");
        data_handle<viennagrid_numeric> delta = get_input<viennagrid_numeric>("delta");

#if HIST
        data_handle<viennagrid_numeric> histogram_bins = get_input<viennagrid_numeric>("histogram_bin");
        data_handle<viennagrid_numeric> histogram_min = get_input<viennagrid_numeric>("histogram_min");
        data_handle<viennagrid_numeric> histogram_max = get_input<viennagrid_numeric>("histogram_max");
        data_handle<int> histogram_bin_count = get_input<int>("histogram_bin_count");
#endif


        typedef viennagrid::mesh                                  MeshType;
        typedef viennagrid::result_of::element<MeshType>::type    ElementType; //=Triangle or Tetrahedron

        typedef viennamesh::statistic<viennagrid_numeric>         StatisticType;
        StatisticType statistic;

#if HIST
        typedef StatisticType::histogram_type                     HistogramType;


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
            //If histograms are about to be added again, returing false here will prevent the basic statistics features from working!
            error(1) << "No histogram configuration provided" << std::endl;
            return false;
        }
#endif





        //Good element threshold input was set, call statistics with the appropriate metric
        if(good_element_threshold.valid())
        {
            viennamesh::LoggingStack stack( std::string("High quality cell counter with metric type \"") + metric_type() + "\"" );

            if (metric_type() == "aspect_ratio")
                statistic.cell_quality_count<viennamesh::aspect_ratio_tag>( input_mesh(), viennamesh::aspect_ratio<ElementType>, good_element_threshold());
            else if (metric_type() == "min_angle")
                statistic.cell_quality_count<viennamesh::min_angle_tag>( input_mesh(), viennamesh::min_angle<ElementType>, good_element_threshold());
            else if (metric_type() == "max_angle")
                statistic.cell_quality_count<viennamesh::max_angle_tag>( input_mesh(), viennamesh::max_angle<ElementType>, good_element_threshold());
            else if (metric_type() == "min_dihedral_angle")
                statistic.cell_quality_count<viennamesh::min_dihedral_angle_tag>( input_mesh(), viennamesh::min_dihedral_angle<ElementType>, good_element_threshold());
            else if (metric_type() == "radius_edge_ratio")
                statistic.cell_quality_count<viennamesh::radius_edge_ratio_tag>( input_mesh(), viennamesh::radius_edge_ratio<ElementType>, good_element_threshold());
            else if (metric_type() == "radius_ratio")
                statistic.cell_quality_count<viennamesh::radius_ratio_tag>( input_mesh(), viennamesh::radius_ratio<ElementType>, good_element_threshold());
            else if (metric_type() == "perimeter_inradius_ratio")
                statistic.cell_quality_count<viennamesh::perimeter_inradius_ratio_tag>( input_mesh(), viennamesh::perimeter_inradius_ratio<ElementType>, good_element_threshold());
            else if (metric_type() == "edge_ratio")
                statistic.cell_quality_count<viennamesh::edge_ratio_tag>( input_mesh(), viennamesh::edge_ratio<ElementType>, good_element_threshold());
            else if (metric_type() == "circum_perimeter_ratio")
                statistic.cell_quality_count<viennamesh::circum_perimeter_ratio_tag>( input_mesh(), viennamesh::circum_perimeter_ratio<ElementType>, good_element_threshold());
            else if (metric_type() == "stretch")
                statistic.cell_quality_count<viennamesh::stretch_tag>( input_mesh(), viennamesh::stretch<ElementType>, good_element_threshold());
            else if (metric_type() == "skewness")
                statistic.cell_quality_count<viennamesh::skewness_tag>( input_mesh(), viennamesh::skewness<ElementType>, good_element_threshold());
            else
            {
                error(1) << "Metric type \"" << metric_type() << "\" is not supported for cell quality classifaction" << std::endl;
                return false;
            }
        }
        else //no triangle classifiction takes place
        {
            viennamesh::LoggingStack stack( std::string("Cell statistics with metric type \"") + metric_type() + "\"" );

            if (metric_type() == "aspect_ratio")
                statistic.cell_stats( input_mesh(), viennamesh::aspect_ratio<ElementType> );
            else if (metric_type() == "min_angle")
                statistic.cell_stats( input_mesh(), viennamesh::min_angle<ElementType> );
            else if (metric_type() == "max_angle")
                statistic.cell_stats( input_mesh(), viennamesh::max_angle<ElementType> );
            else if (metric_type() == "min_dihedral_angle")
                statistic.cell_stats( input_mesh(), viennamesh::min_dihedral_angle<ElementType> );
            else if (metric_type() == "radius_edge_ratio")
                statistic.cell_stats( input_mesh(), viennamesh::radius_edge_ratio<ElementType> );
            else if (metric_type() == "radius_ratio")
                statistic.cell_stats( input_mesh(), viennamesh::radius_ratio<ElementType> );
            else if (metric_type() == "perimeter_inradius_ratio")
                statistic.cell_stats( input_mesh(), viennamesh::perimeter_inradius_ratio<ElementType> );
            else if (metric_type() == "edge_ratio")
                statistic.cell_stats( input_mesh(), viennamesh::edge_ratio<ElementType> );
            else if (metric_type() == "circum_perimeter_ratio")
                statistic.cell_stats( input_mesh(), viennamesh::circum_perimeter_ratio<ElementType> );
            else if (metric_type() == "stretch")
                statistic.cell_stats( input_mesh(), viennamesh::stretch<ElementType> );
            else if (metric_type() == "skewness")
                statistic.cell_stats( input_mesh(), viennamesh::skewness<ElementType> );
            else
            {
                error(1) << "Metric type \"" << metric_type() << "\" is not supported" << std::endl;
                return false;
            }
        }

        if(original_mesh.valid())//a second mesh is set, calculate mesh comparison measures
        {
            viennamesh::LoggingStack stack( std::string("Calculation of Mesh Comparison Measures") );

            statistic.mesh_comparison_quality(input_mesh(), original_mesh());



            ConstTriangleRange tr_orig(original_mesh());
            ConstTriangleRange tr(input_mesh());

            StatisticType statistic_orig;
            statistic_orig.cell_stats( original_mesh(), viennamesh::aspect_ratio<ElementType> );


            //use median of orig mesh for input mesh triangle shape characterization
            statistic.cell_quality_count<viennamesh::aspect_ratio_tag>( input_mesh(), viennamesh::aspect_ratio<ElementType>, statistic_orig.median());

            if(alpha.valid() && beta.valid() && gamma.valid() && delta.valid() )
            {
                statistic.set_mesh_quality_weights(alpha(), beta(), gamma(), delta());
                info(5) << "values for comprehensive mesh quality metric: alpha = " << alpha()
                        << ", beta = " << beta() << ", gamma = " << gamma() << ", delta = "<< delta() <<  std::endl;

            }
            else
            {
                info(5) << "default values for comprehensive mesh quality metric used: alpha = 0.25, beta = 20, gamma = 1.0, delta = 1.3" << std::endl;
            }

            set_output("minimum_distance_rms", statistic.min_dist_rms());
            set_output("mean_curvature_difference", statistic.mean_curvature());
            set_output("area_deviation", statistic.volume_deviation());
            set_output("triangle_shape", statistic.good_elements()/statistic.count());
            set_output("mesh_quality_metric", statistic.mesh_quality_metric());
        }


        info(5) << statistic << "\n";

#if HIST
        statistic.normalize();
        std::vector<viennagrid_numeric> bins;
        for (HistogramType::const_iterator bit = statistic.histogram().begin(); bit != statistic.histogram().end(); ++bit)
            bins.push_back( (*bit).second );
        bins.push_back( statistic.histogram().overflow_bin() );

        data_handle<viennagrid_numeric> output_bins = make_data<viennagrid_numeric>();
        output_bins.set( bins );
        set_output( "bins", output_bins );
#endif

        set_output( "min", statistic.min() );
        set_output( "max", statistic.max() );
        set_output( "mean", statistic.mean() );
        set_output( "median", statistic.median());

        return true;
    }

}
