#ifndef VIENNAMESH_STATISTICS_STATISTIC_HPP
#define VIENNAMESH_STATISTICS_STATISTIC_HPP

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

#include <limits>
#include <set>

#include "mesh_comparison.hpp"
#include "libigl_convert.hpp"

#define HIST 0


namespace viennamesh
{


    template<typename NumericT> NumericT infinity()
    {
        return std::numeric_limits<NumericT>::infinity();
    }



#if HIST

    /*
    The current statistics plugin implementation does not depend on histogram features. If they are not needed it is possible to
    completely omit the following.
    */

    template<typename NumericT, typename BinT>
    class histogram
    {
        typedef std::map<NumericT, BinT> BinContainerType;
        typedef typename BinContainerType::iterator iterator;

        iterator begin()
        {
            return bins.begin();
        }
        iterator end()
        {
            return bins.end();
        }
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

        const_iterator begin() const
        {
            return bins.begin();
        }
        const_iterator end() const
        {
            return bins.end();
        }

        std::pair<NumericT, NumericT> bin_interval(const_iterator bin_it) const
        {
            if (bin_it == bins.begin())
                return std::make_pair( -infinity<NumericT>(), bin_it->first );

            if (bin_it == bins.end())
                return std::make_pair( bins.rbegin()->first, infinity<NumericT>() );

            const_iterator prev_it = bin_it;
            --prev_it;
            return std::make_pair( prev_it->first, bin_it->first );
        }

        BinT overflow_bin() const
        {
            return overflow_bin_;
        }

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
        {
            return bins.upper_bound(value);
        }
        const_iterator bin(NumericT value) const
        {
            return bins.upper_bound(value);
        }

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

#endif

    /*Manages statistical parameters that are associated with cell shape quality and provides mesh comparison measures*/
    template<typename NumericT>
    class statistic
    {

        template<typename T>
        friend std::ostream & operator <<(std::ostream & stream, statistic<T> const & stats);

    public:


#if HIST
        typedef viennamesh::histogram<NumericT, viennagrid_numeric> histogram_type;
#endif


        statistic()
        {
            sum_ = 0;
            good_element_count_ = 0;
            min_dist_rms_ = -1;      // -1 = not yet calculated
            mean_curvature_ = -1;    // not yet calculated
            volume_deviation_ = -1;  // not yet calculated


            //default values for weighting coefficients
            alpha_ = 0.4;
            beta_ = 20;
            gamma_ = 1.0;
            delta_ = 1.3;

            good_elements_counted_ = false;
            comparison_measures_calculated_ = false;
            ordered_values_.clear();


#if HIST
            histogram_.reset();
#endif
        }


        /*
        Calculates min, max, mean, median of the cell shape quality metric given via the Functor functor.
        */
        template<typename MeshT, typename FunctorT>
        void cell_stats(MeshT const & mesh, FunctorT functor)
        {
            typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
            typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

            ConstCellRangeType cells(mesh);
            ConstCellIteratorType cit = cells.begin();

            count_ = cells.size();

            min_ = max_ = functor(*cit++);

            for (; cit != cells.end(); ++cit) //iterate through all cells
            {
                NumericT cell_metric = functor(*cit);
                min_ = std::min(min_, cell_metric);
                max_ = std::max(max_, cell_metric);
                sum_ += cell_metric;

#if HIST
                histogram_.increase( cell_metric );
#endif
                ordered_values_.insert( cell_metric ); //for median calculation
            }
        }

        /*
        Additionally checks if a cell is regarded as 'high quality cell'. The 'high quality' condition is provided via good_element_threshold.
        * E.g., radius_ratio < 1.5
        * Whether '<' or '>' is used for comparison is deduced from metric_ordering_tag
        */
        template<typename MetricTagT, typename MeshT, typename FunctorT >
        void cell_quality_count(MeshT const & mesh, FunctorT functor,  NumericT good_element_threshold)
        {
            typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
            typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;


            ConstCellRangeType cells(mesh);
            ConstCellIteratorType cit = cells.begin();

            count_ = cells.size();

            min_ = max_ = functor(*cit++);

            for (; cit != cells.end(); ++cit) //iterate through all cells
            {
                NumericT cell_metric = functor(*cit);
                min_ = std::min(min_, cell_metric);
                max_ = std::max(max_, cell_metric);
                sum_ += cell_metric;

                //check if cell is a good element according to the given cell metric and decision threshold
                if(is_good_element<MetricTagT, NumericT>(cell_metric, good_element_threshold) )
                {
                    ++good_element_count_;
                }

                ordered_values_.insert( cell_metric ); //for median
            }

            good_elements_counted_ = true;
        }


        /*
        * Calculates the mesh comparison metrics minimum distance RMS, mean curvature difference RMS and surface area deviation.
        * Parameter mesh is compared to parameter mesh_orig (original mesh)
        */
        template <typename MeshT>
        void mesh_comparison_quality(MeshT const & mesh, MeshT const & mesh_orig)
        {

            //Comparison metrics are implemented using libigl, which uses matrices provided by Eigen library.
            Eigen::Matrix<NumericT, Eigen::Dynamic, Eigen::Dynamic> Vertices, Vertices_orig;
            Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> Facets, Facets_orig;

            convert_to_igl_mesh(mesh, Vertices, Facets);
            convert_to_igl_mesh(mesh_orig, Vertices_orig, Facets_orig);


            //MeshQuality object manages efficient calculation of the above given metrics, original mesh comes first!
            MeshQuality<NumericT> meshq(Vertices_orig, Facets_orig, Vertices, Facets );


            min_dist_rms_ = meshq.min_distance_RMS();

            //additionally, Hausdorff distance and Gaussian Curvature difference RMS can be calculated
            //hausdorff = meshq.hausdorff_distance();
            //gaussian = meshq.gaussian_curvature();


            mean_curvature_= meshq.mean_curvature();

            //calculate surface area (surface mesh) or volume (3D mesh) deviation
            NumericT volume_input = viennagrid::volume(mesh);
            NumericT volume_orig = viennagrid::volume(mesh_orig);
            volume_deviation_ =  std::fabs(volume_orig - volume_input) / (volume_orig);

            comparison_measures_calculated_ = true;
        }


#if HIST
        void set_histogram( histogram_type const & histogram_x )
        {
            histogram_ = histogram_x;
        }
#endif
        // returns minimum value of given metric
        NumericT min() const
        {
            return min_;
        }
        // returns maximum value of given metric
        NumericT max() const
        {
            return max_;
        }

        // returns sum of cell values of given metric
        //TODO: is it really necessary to access the sum?
        NumericT sum() const
        {
            return sum_;
        }

        //returns number of cells in mesh
        size_t count() const
        {
            return count_;
        }

#if HIST
        void normalize()
        {
            histogram_.normalize();
        }
#endif

        // returns mean value of given metric
        NumericT mean() const
        {
            return sum() / count();
        }

        // returns meadian value of given metric
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

        //returns the number of 'good' cells according to given metric and decision threshold
        NumericT good_elements() const
        {
            return good_element_count_ ;
        }

        NumericT min_dist_rms() const
        {
            return min_dist_rms_;
        }

        NumericT mean_curvature() const
        {
            return mean_curvature_;

        }

        //surface area for surface meshes, volume for 3D meshes
        NumericT volume_deviation() const
        {
            return volume_deviation_;
        }

        NumericT mesh_quality_metric() const
        {
            return alpha_ * (1 - (NumericT) good_element_count_/count_) + beta_ * min_dist_rms_ + gamma_ * mean_curvature_ + delta_ * volume_deviation_;
        }


        bool good_elements_counted() const
        {
            return good_elements_counted_;
        }

        bool comparison_measures_calculated() const
        {
            return comparison_measures_calculated_;
        }

        void set_mesh_quality_weights(NumericT alpha, NumericT beta, NumericT gamma, NumericT delta)
        {
            alpha_ = alpha;
            beta_ = beta;
            gamma_ = gamma;
            delta_ = delta;
        }


#if HIST
        histogram_type const & histogram() const
        {
            return histogram_;
        }
#endif

    private:

        NumericT sum_;
        size_t count_;

        NumericT min_;
        NumericT max_;


        size_t good_element_count_;
        NumericT min_dist_rms_, mean_curvature_;
        NumericT volume_deviation_;

        NumericT alpha_, beta_, gamma_, delta_;

        bool good_elements_counted_;
        bool comparison_measures_calculated_;



        std::multiset<NumericT> ordered_values_;
#if HIST
        histogram_type histogram_;
#endif


    };


    template<typename NumericT>
    std::ostream & operator <<(std::ostream & stream, statistic<NumericT> const & stats)
    {
        stream << "min    = " << stats.min() << "\n";
        stream << "max    = " << stats.max() << "\n";
        stream << "mean   = " << stats.mean() << "\n";
        stream << "median = " << stats.median() << "\n";

        if (stats.good_elements_counted())
        {
            stream << "Number of high quality cells = " << stats.good_elements() << "\n";
            stream << "Total number of cells = " << stats.count() << "\n";
            stream << "High quality cell ratio =  " << stats.good_elements() / stats.count() << "\n";

        }

        if (stats.comparison_measures_calculated())
        {
            stream << "\nMesh Comparison Measures:\n";
            stream << "min distance RMS / bounding_box_diagonal= " <<  stats.min_dist_rms() << "\n";
            stream << "Mean curvature at vertex difference RMS = " <<  stats.mean_curvature() << "\n";
            stream << "Surface area deviation (related to original area) = " << stats.volume_deviation() << "\n";
            stream << "\nComprehensive mesh comparison measure = " << stats.mesh_quality_metric() <<  "\n";
        }

#if HIST
        stream << stats.histogram();
#endif
        return stream;
    }

}

#endif
