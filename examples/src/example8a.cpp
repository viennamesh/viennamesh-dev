#include <iostream>


#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"

#include "viennagrid/io/poly_reader.hpp"

#include "viennamesh/algorithm/cgal_plc_mesher.hpp"
#include "viennagrid/domain/neighbour_iteration.hpp"
#include "viennagrid/algorithm/geometry.hpp"

#include "viennagrid/algorithm/seed_point_segmenting.hpp"

#include "viennamesh/algorithm/vgmodeler_hull_adaption.hpp"
#include "viennamesh/algorithm/netgen_tetrahedron_mesher.hpp"


#include "viennamesh/statistics/element_metrics.hpp"


#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/density.hpp>

#include "viennagrid/algorithm/angle.hpp"


int main()
{
    viennagrid::plc_3d_domain plc_domain;
    
    
    viennagrid::io::poly_reader reader;
    reader(plc_domain, "../../examples/data/big_and_small_cube.poly");
    
    
    viennagrid::triangular_3d_domain triangulated_plc_domain;
    viennamesh::result_of::settings<viennamesh::cgal_plc_3d_mesher_tag>::type plc_settings(0.0, 0.0);
    
    {
        viennamesh::algorithm_feedback fb = viennamesh::run_algo< viennamesh::cgal_plc_3d_mesher_tag >( plc_domain, triangulated_plc_domain, plc_settings );
        std::cout << fb << std::endl;
    }
    
    {
        viennagrid::io::vtk_writer<viennagrid::triangular_3d_domain> vtk_writer;
        vtk_writer(triangulated_plc_domain, "meshed_plc_hull");
    }

    
    
    typedef viennagrid::result_of::point<viennagrid::triangular_3d_domain>::type point_type;
    
    std::cout << "Num triangles after PLC meshing: " << viennagrid::elements<viennagrid::triangle_tag>( triangulated_plc_domain ).size() << std::endl;
    
    
//     typedef viennagrid::result_of::segmentation<viennagrid::triangular_3d_domain, viennagrid::triangle_tag>::type segmentation_type;
    viennagrid::triangular_hull_3d_segmentation triangulated_plc_segmentation(triangulated_plc_domain);
    
    
    std::vector< std::pair< int, point_type > > seed_points;
    seed_points.push_back( std::make_pair(0, point_type(0.0, 0.0, 0.0)) );
    seed_points.push_back( std::make_pair(1, point_type(0.0, 0.0, 20.0)) );
    
    viennagrid::mark_face_segments( triangulated_plc_domain, triangulated_plc_segmentation, seed_points.begin(), seed_points.end() );

    
    viennagrid::triangular_3d_domain oriented_adapted_hull_domain;
    viennagrid::triangular_hull_3d_segmentation oriented_adapted_hull_segmentation(oriented_adapted_hull_domain);
    viennamesh::result_of::settings<viennamesh::vgmodeler_hull_adaption_tag>::type vgm_settings;
    
    vgm_settings.cell_size = 3.0;
    
    {
        viennamesh::algorithm_feedback fb = viennamesh::run_algo< viennamesh::vgmodeler_hull_adaption_tag >( triangulated_plc_domain, triangulated_plc_segmentation,
                                                                        oriented_adapted_hull_domain, oriented_adapted_hull_segmentation,
                                                                        vgm_settings );
        std::cout << fb << std::endl;
    }
    
    
    {        
        viennagrid::io::vtk_writer<viennagrid::triangular_3d_domain> vtk_writer;
        vtk_writer(oriented_adapted_hull_domain, "netgen_adapt_hull");
    }

    viennagrid::tetrahedral_3d_domain tetrahedron_domain;
    viennagrid::tetrahedral_3d_segmentation tetrahedron_segmentation(tetrahedron_domain);
    viennamesh::result_of::settings<viennamesh::netgen_tetrahedron_tag>::type netgen_settings;
    
    netgen_settings.cell_size = 3.0;
    
    {
        viennamesh::algorithm_feedback fb = viennamesh::run_algo< viennamesh::netgen_tetrahedron_tag >( oriented_adapted_hull_domain, oriented_adapted_hull_segmentation,
                                                                    tetrahedron_domain, tetrahedron_segmentation,
                                                                    netgen_settings );
        std::cout << fb << std::endl;
    }
    


    std::deque<double> aspect_ratio;
    std::deque<double> min_angle;
    std::deque<double> min_dihedral_angle;
    
    viennagrid::result_of::accessor< std::deque<double>, viennagrid::tetrahedral_3d_cell>::type aspect_ratio_accessor(aspect_ratio);
    viennagrid::result_of::accessor< std::deque<double>, viennagrid::tetrahedral_3d_cell>::type min_angle_accessor(min_angle);
    viennagrid::result_of::accessor< std::deque<double>, viennagrid::tetrahedral_3d_cell>::type min_dihedral_angle_accessor(min_dihedral_angle);
    
    
    typedef viennagrid::result_of::element_range<viennagrid::tetrahedral_3d_domain, viennagrid::tetrahedron_tag>::type tetrahedron_range_type;
    typedef viennagrid::result_of::iterator<tetrahedron_range_type>::type tetrahedron_range_iterator;

    tetrahedron_range_type tetrahedrons = viennagrid::elements( tetrahedron_domain );
    for (tetrahedron_range_iterator tetit = tetrahedrons.begin(); tetit != tetrahedrons.end(); ++tetit)
    {
        aspect_ratio_accessor(*tetit) = viennamesh::aspect_ratio( *tetit );
        min_angle_accessor(*tetit) = viennamesh::min_angle( *tetit );
        min_dihedral_angle_accessor(*tetit) = viennamesh::min_dihedral_angle( *tetit );
    }
    
    
    
    
    
    {        
        viennagrid::io::vtk_writer<viennagrid::tetrahedral_3d_domain> vtk_writer;
        viennagrid::io::add_scalar_data_on_cells(vtk_writer, aspect_ratio_accessor, "aspect_ratio");
        viennagrid::io::add_scalar_data_on_cells(vtk_writer, min_angle_accessor, "min_angle");
        viennagrid::io::add_scalar_data_on_cells(vtk_writer, min_dihedral_angle_accessor, "min_dihedral_angle");
        vtk_writer(tetrahedron_domain, tetrahedron_segmentation, "netgen_volume");
    }

    
    
    
    typedef boost::accumulators::accumulator_set<double, boost::accumulators::features<boost::accumulators::tag::min, boost::accumulators::tag::max, boost::accumulators::tag::mean, boost::accumulators::tag::density> > acc;
    typedef boost::iterator_range<std::vector<std::pair<double, double> >::iterator > histogram_type;
    
    acc myAccumulator( boost::accumulators::tag::density::num_bins = 4, boost::accumulators::tag::density::cache_size = 10 );
    
//     std::for_each(
//         tetrahedrons.begin(),
//         tetrahedrons.end(),
//         [&tetrahedron_domain, &myAccumulator] ( tetrahedron_range_type::value_type & element )
//         { myAccumulator( viennamesh::min_dihedral_angle( element ) ); }
//     );
    
    
    for (tetrahedron_range_iterator tetit = tetrahedrons.begin(); tetit != tetrahedrons.end(); ++tetit)
        myAccumulator( viennamesh::min_dihedral_angle( *tetit ) );

    histogram_type hist = boost::accumulators::density(myAccumulator);
    
    double total = 0.0;
    
    for( int i = 0; i < hist.size(); i++ ) 
    {
        std::cout << "Bin lower bound: " << hist[i].first << ", Value: " << hist[i].second << std::endl; 
        total += hist[i].second;
    }
    
    std::cout << "Min:    " << boost::accumulators::min(myAccumulator) << std::endl;
    std::cout << "Max:    " << boost::accumulators::max(myAccumulator) << std::endl;
    std::cout << "Mean:   " << boost::accumulators::mean(myAccumulator) << std::endl;
    
}
