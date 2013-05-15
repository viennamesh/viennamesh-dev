#include <iostream>

// using std::string;
// #include "external/netgen-5.0.0/libsrc/general/ngexception.hpp"

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"

#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/domain/domain.hpp"
#include "viennagrid/domain/neighbour_iteration.hpp"

#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/refine.hpp"
#include "viennagrid/algorithm/surface.hpp"






template<typename src_domain_type, typename src_segment_container_type, typename dst_domain_type, typename dst_segment_container_type>
void copy( src_domain_type const & src_domain, src_segment_container_type const & src_segments,
           dst_domain_type & dst_domain, dst_segment_container_type & dst_segments )
{
    typedef typename src_segment_container_type::value_type src_segment_type;
    typedef typename dst_segment_container_type::value_type dst_segment_type;
    
    typedef typename viennagrid::result_of::cell_tag<src_domain_type>::type src_cell_tag;
    typedef typename viennagrid::result_of::cell_type<src_domain_type>::type src_cell_type;
    
    typedef typename viennagrid::result_of::const_vertex_range<src_domain_type>::type src_vertex_range_type;
    typedef typename viennagrid::result_of::iterator<src_vertex_range_type>::type src_vertex_range_iterator;
    
    typedef typename viennagrid::result_of::const_cell_range<src_segment_type>::type src_cell_range_type;
    typedef typename viennagrid::result_of::iterator<src_cell_range_type>::type src_cell_range_itertor;
    
    typedef typename viennagrid::result_of::const_vertex_handle<src_domain_type>::type src_const_vertex_handle;
    typedef typename viennagrid::result_of::vertex_handle<dst_domain_type>::type dst_vertex_handle;
    
    if (&src_domain == &dst_domain)
        return;
    
    viennagrid::clear_domain(dst_domain);
    dst_segments.clear();
    
    std::map<src_const_vertex_handle, dst_vertex_handle> vertex_handle_map;
    
    src_vertex_range_type vertices = viennagrid::elements( src_domain );
//     std::cout << "Copy: vertex count = " << vertices.size() << std::endl;
    for (src_vertex_range_iterator it = vertices.begin(); it != vertices.end(); ++it)
        vertex_handle_map[it.handle()] = viennagrid::create_vertex( dst_domain, viennagrid::point(src_domain, *it) );
    

    
    for (typename src_segment_container_type::const_iterator seg_it = src_segments.begin(); seg_it != src_segments.end(); ++seg_it)
    {
        dst_segments.push_back( viennagrid::create_view<dst_segment_type>(dst_domain) );
        dst_segment_type & dst_segment = dst_segments.back();
        
        src_cell_range_type cells = viennagrid::elements( *seg_it );
//         std::cout << "Copy: cell count = " << cells.size() << std::endl;
        for (src_cell_range_itertor it = cells.begin(); it != cells.end(); ++it)
        {
            src_cell_type const & cell = *it;
            
            std::deque<dst_vertex_handle> vertex_handles;
            
            typedef typename viennagrid::result_of::const_vertex_range<src_cell_type>::type src_vertex_on_src_cell_range_type;
            typedef typename viennagrid::result_of::iterator<src_vertex_on_src_cell_range_type>::type src_vertex_on_src_cell_range_iterator;
            
            src_vertex_on_src_cell_range_type cell_vertices = viennagrid::elements(cell);
            for (src_vertex_on_src_cell_range_iterator jt = cell_vertices.begin(); jt != cell_vertices.end(); ++jt)
                vertex_handles.push_back( vertex_handle_map[jt.handle()] );
            
            typedef typename viennagrid::result_of::cell_type<dst_domain_type>::type dst_cell_type;
            viennagrid::create_element<dst_cell_type>( dst_segment, vertex_handles.begin(), vertex_handles.end() );
        }
    }
}



template<typename domain_in_type, typename segment_container_type_in, typename domain_out_type, typename segment_container_type_out, typename functor_type>
unsigned int refine( domain_in_type const & domain_in, segment_container_type_in const & segments_in,
                     domain_out_type & domain_out, segment_container_type_out & segments_out,
                     functor_type functor, unsigned int max_iteration_count = 10 )
{
    typedef typename viennagrid::result_of::cell_tag<domain_in_type>::type cell_tag;
    typedef typename viennagrid::result_of::const_cell_range<domain_in_type>::type cell_range_type;
    typedef typename viennagrid::result_of::iterator<cell_range_type>::type cell_range_itertor;
    
    domain_out_type tmp_domain;
    segment_container_type_out tmp_segments;
    
    
    copy( domain_in, segments_in, tmp_domain, tmp_segments );
    
    
    domain_out_type * domain_from = &tmp_domain;
    segment_container_type_out * segments_from = &tmp_segments;
    
    domain_out_type * domain_to = &domain_out;
    segment_container_type_out * segments_to = &segments_out;
    
    
    std::deque<domain_out_type> refined_domains;
    
    bool all_cells_good = false;
    unsigned int iteration_count = 0;
    for (; iteration_count < max_iteration_count; ++iteration_count)
    {
        std::cout << "Iteration " << iteration_count << std::endl;
        
        domain_out_type const & domain_src = *domain_from; //refined_domains.empty() ? domain_in : refined_domains.back();
        segment_container_type_out const & segments_src = *segments_from;
        
        cell_range_type cells = viennagrid::elements( domain_src );
        
        unsigned int bad_cells_count = 0;
        all_cells_good = true;
        for (cell_range_itertor it = cells.begin(); it != cells.end(); ++it)
        {
            if ( !functor( domain_src, *it ) )
            {
                viennadata::access<viennagrid::refinement_key, bool>()(*it) = true;
                all_cells_good = false;
                ++bad_cells_count;
            }
        }
        
        std::cout << "Bad cells count = " << bad_cells_count << std::endl;
        
        if (all_cells_good)
            break;
        
//         refined_domains.resize( refined_domains.size()+1 );
            
        domain_out_type & domain_dst = *domain_to; //refined_domains.empty() ? domain_in : refined_domains.back();
        segment_container_type_out & segments_dst = *segments_to;
        
        viennagrid::clear_domain(domain_dst);
        segments_dst.clear();
        
        viennagrid::refine<cell_tag>( domain_src, segments_src, domain_dst, segments_dst, viennagrid::local_refinement_tag() );
        
        std::swap( domain_from, domain_to );
        std::swap( segments_from, segments_to );
    }
    
    
    copy( *domain_from, *segments_from, domain_out, segments_out );
    
    return iteration_count;
}


struct volume_functor
{
    volume_functor(double volume_) : volume(volume_) {}
    
    template<typename domain_type, typename element_type>
    bool operator() ( domain_type const & domain, element_type const & element )
    {
        return viennagrid::volume( domain, element ) < volume;
    }
    
    double volume;
};




struct smallest_boundary_volume_functor
{
    smallest_boundary_volume_functor(double smallest_boundary_volume_) : smallest_boundary_volume(smallest_boundary_volume_) {}
    
    template<typename domain_type, typename element_type>
    bool operator() ( domain_type const & domain, element_type const & element )
    {
        return viennagrid::volume( domain,
                viennagrid::dereference_handle( domain, viennagrid::smallest_boundary_volume<viennagrid::line_tag>( domain, element ) ) ) < smallest_boundary_volume;
    }
    
    double smallest_boundary_volume;
};

struct largest_boundary_volume_functor
{
    largest_boundary_volume_functor(double largest_boundary_volume_) : largest_boundary_volume(largest_boundary_volume_) {}
    
    template<typename domain_type, typename element_type>
    bool operator() ( domain_type const & domain, element_type const & element )
    {
        return viennagrid::volume( domain,
                viennagrid::dereference_handle( domain, viennagrid::largest_boundary_volume<viennagrid::line_tag>( domain, element ) ) ) < largest_boundary_volume;
    }
    
    double largest_boundary_volume;
};





int main()
{
    ///////////////////////////////////////////
    // Volume meshing
    ///////////////////////////////////////////
    
    
    typedef viennagrid::config::result_of::full_domain_config< viennagrid::tetrahedron_tag, viennagrid::config::point_type_3d, viennagrid::storage::pointer_handle_tag >::type domain_config_type;
    
//     typedef viennagrid::config::tetrahedral_3d_domain domain_type;
    typedef viennagrid::result_of::domain< domain_config_type >::type domain_type;
    
    typedef viennagrid::result_of::domain_view<domain_type>::type segment_type;
    typedef viennagrid::result_of::cell_type<domain_type>::type cell_type;
    typedef viennagrid::result_of::point_type<domain_type>::type point_type;
    typedef viennagrid::result_of::cell_tag<domain_type>::type cell_tag;
    
    domain_type domain;
    std::deque< segment_type > segments;
    
    try
    {
        viennagrid::io::vtk_reader<cell_type, domain_type> my_reader;
    //     my_reader(my_domain, my_segments, "../examples/data/netgen_volume.vtu_main.pvd");
        my_reader(domain, segments, "../../examples/data/half-trigate_main.pvd");
    }
    catch (...)
    {
        std::cerr << "File-Reader failed. Aborting program..." << std::endl;
        return EXIT_FAILURE;
    }
    
    

    typedef viennagrid::result_of::cell_range<viennagrid::config::tetrahedral_3d_domain>::type cell_range_type;
    typedef viennagrid::result_of::iterator<cell_range_type>::type cell_range_iterator;
    
    point_type to_refine( 0.0, 0.0, -80.0 );
    double refine_radius = 10.0;
    
    
    
    
    domain_type refined_domain;
    std::deque<segment_type> refined_segments;
    
    
    
    
    
    
    
    
//     cell_range_type cells = viennagrid::elements( domain );
//     unsigned int refine_count = 0;
//     for (cell_range_iterator it = cells.begin(); it != cells.end(); ++it)
//     {
// //             std::cout << viennagrid::volume( domain,
// //                 viennagrid::dereference_handle( domain, viennagrid::smallest_boundary_volume<viennagrid::line_tag>( domain, *it ) ) ) << std::endl;
//         
//         point_type centroid = viennagrid::centroid( domain, *it );
//         if (viennagrid::norm_2(centroid - to_refine) < refine_radius)
// //         if (viennagrid::volume(domain, *it) < 10.0)
//         {
// 
//             
//             viennadata::access<viennagrid::refinement_key, bool>()(*it) = true;
//             
// //             std::cout << "REFINE!!" << std::endl;
//             ++refine_count;
//         }
//     }
    
//     viennagrid::refine<cell_tag>(domain, segments, refined_domain, refined_segments, viennagrid::local_refinement_tag());
    
//     std::cout << "Cell count = " << cells.size() << std::endl;
//     std::cout << "Refine count = " << refine_count << std::endl;
    
    
//     domain_type adaptively_refined_domain = viennagrid::refine<cell_tag>(domain, viennagrid::local_refinement_tag());
    
//     {
//         cell_range_type cells = viennagrid::elements( adaptively_refined_domain );
//         for (cell_range_iterator it = cells.begin(); it != cells.end(); ++it)
//             std::cout << viennagrid::volume( adaptively_refined_domain, *it ) << std::endl;
//     }
//     domain_type adaptively_refined_domain;
//     refine(domain, adaptively_refined_domain, volume_functor(20.0));
    refine( domain, segments, refined_domain, refined_segments, volume_functor(30.0) );
    
//     copy(domain, adaptively_refined_domain);
    
    
    {        
        viennagrid::io::vtk_writer<domain_type, cell_type> vtk_writer;
//         viennagrid::io::add_scalar_data_on_cells<std::string, double>(vtk_writer, "segment_id", "segment_id");
        vtk_writer(refined_domain, refined_segments, "refined_volume");
    }

    
    
}
