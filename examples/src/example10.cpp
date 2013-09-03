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






template<typename SrcDomainT, typename SrcSegmentationT, typename DstDomainT, typename DstSegmentationT>
void copy_cells(SrcDomainT const & src_domain,  SrcSegmentationT const & src_segmentation,
                DstDomainT & dst_domain,        DstSegmentationT & dst_segmentation )
{
//     typedef typename src_segment_container_type::value_type src_segment_type;
//     typedef typename dst_segment_container_type::value_type dst_segment_type;
    typedef typename viennagrid::result_of::segment<SrcSegmentationT>::type SrcSegmentType;
    typedef typename viennagrid::result_of::segment<DstSegmentationT>::type DstSegmentType;
    
    typedef typename viennagrid::result_of::cell_tag<SrcDomainT>::type  SrcCellTag;
    typedef typename viennagrid::result_of::cell<SrcDomainT>::type      SrcCellType;
    
    typedef typename viennagrid::result_of::const_vertex_range<SrcDomainT>::type    SrcVertexRangeType;
    typedef typename viennagrid::result_of::iterator<SrcVertexRangeType>::type      SrcVertexRangeIterator;
    
    typedef typename viennagrid::result_of::const_cell_range<SrcSegmentType>::type  SrcCellRangeType;
    typedef typename viennagrid::result_of::iterator<SrcCellRangeType>::type        SrcCellRangeIterator;
    
    typedef typename viennagrid::result_of::const_vertex_handle<SrcDomainT>::type   SrcConstVertexHandle;
    typedef typename viennagrid::result_of::vertex_handle<DstDomainT>::type         DstVertexHandleType;
    
    if (&src_domain == &dst_domain)
        return;
    
    dst_domain.clear();
    dst_segmentation.clear();
    
    std::map<SrcConstVertexHandle, DstVertexHandleType> vertex_handle_map;
    
    SrcVertexRangeType vertices = viennagrid::elements( src_domain );
//     std::cout << "Copy: vertex count = " << vertices.size() << std::endl;
    for (SrcVertexRangeIterator it = vertices.begin(); it != vertices.end(); ++it)
        vertex_handle_map[it.handle()] = viennagrid::make_vertex( dst_domain, viennagrid::point(src_domain, *it) );
    

    
    for (typename SrcSegmentationT::const_iterator seg_it = src_segmentation.begin(); seg_it != src_segmentation.end(); ++seg_it)
    {
//         dst_segments.push_back( viennagrid::make_view<dst_segment_type>(dst_domain) );
        DstSegmentType & dst_segment = dst_segmentation.get_make_segment( seg_it->id() );
        
        SrcCellRangeType cells = viennagrid::elements( *seg_it );
//         std::cout << "Copy: cell count = " << cells.size() << std::endl;
        for (SrcCellRangeIterator it = cells.begin(); it != cells.end(); ++it)
        {
            SrcCellType const & cell = *it;
            
            std::deque<DstVertexHandleType> vertex_handles;
            
            typedef typename viennagrid::result_of::const_vertex_range<SrcCellType>::type SrcVertexOnSrcCellRangeType;
            typedef typename viennagrid::result_of::iterator<SrcVertexOnSrcCellRangeType>::type SrcVertexOnSrcCellRangeIterator;
            
            SrcVertexOnSrcCellRangeType cell_vertices = viennagrid::elements(cell);
            for (SrcVertexOnSrcCellRangeIterator jt = cell_vertices.begin(); jt != cell_vertices.end(); ++jt)
                vertex_handles.push_back( vertex_handle_map[jt.handle()] );
            
            typedef typename viennagrid::result_of::cell<DstDomainT>::type DstCellType;
            viennagrid::make_element<DstCellType>( dst_segment, vertex_handles.begin(), vertex_handles.end() );
        }
    }
}



template<typename DomainInT, typename SegmentationInT, typename DomainOutT, typename SegmentationOutT, typename FunctorT>
unsigned int refine( DomainInT const & domain_in,   SegmentationInT const & segments_in,
                     DomainOutT & domain_out,       SegmentationOutT & segments_out,
                     FunctorT functor, unsigned int max_iteration_count = 10 )
{
    typedef typename viennagrid::result_of::cell<DomainInT>::type CellType;
    typedef typename viennagrid::result_of::cell_tag<DomainInT>::type cell_tag;
    typedef typename viennagrid::result_of::const_cell_range<DomainInT>::type cell_range_type;
    typedef typename viennagrid::result_of::iterator<cell_range_type>::type cell_range_itertor;
    
    DomainOutT tmp_domain;
    SegmentationOutT tmp_segmentation(tmp_domain);
    
    
    copy_cells( domain_in, segments_in, tmp_domain, tmp_segmentation );
    
    
    DomainOutT * domain_from = &tmp_domain;
    SegmentationOutT * segmentation_from = &tmp_segmentation;
    
    DomainOutT * domain_to = &domain_out;
    SegmentationOutT * segmentation_to = &segments_out;




    
    
    
    std::deque<DomainOutT> refined_domains;
    
    bool all_cells_good = false;
    unsigned int iteration_count = 0;
    for (; iteration_count < max_iteration_count; ++iteration_count)
    {
        std::deque<bool> refinement_flag;
        typename viennagrid::result_of::accessor<std::deque<bool>, CellType>::type refinement_flag_accessor(refinement_flag);
        
        std::cout << "Iteration " << iteration_count << std::endl;
        
        DomainOutT const & domain_src = *domain_from; //refined_domains.empty() ? domain_in : refined_domains.back();
        SegmentationOutT const & segmentation_src = *segmentation_from;
        
        cell_range_type cells = viennagrid::elements( domain_src );
        
        std::cout << "Number of Cells: " << cells.size() << std::endl;
        
        unsigned int bad_cells_count = 0;
        all_cells_good = true;
        for (cell_range_itertor it = cells.begin(); it != cells.end(); ++it)
        {
            if ( !functor( domain_src, *it ) )
            {
              refinement_flag_accessor(*it) = true;
              all_cells_good = false;
              
              ++bad_cells_count;
            }
        }
        
//     {
//       char name[100];
//       sprintf( name, "src_iter%d", iteration_count );
//         viennagrid::io::vtk_writer<DomainOutT> vtk_writer;
// //         viennagrid::io::add_scalar_data_on_cells(vtk_writer, refinement_flag_double_accessor, "to_refine");
//         vtk_writer(domain_src, segmentation_src, name);
//     }
        
        std::cout << "Bad cells count = " << bad_cells_count << std::endl;
        
        if (all_cells_good)
            break;
        
//         refined_domains.resize( refined_domains.size()+1 );
            
        DomainOutT & domain_dst = *domain_to; //refined_domains.empty() ? domain_in : refined_domains.back();
        SegmentationOutT & segmentation_dst = *segmentation_to;
        
//         viennagrid::clear_domain(domain_dst);
        domain_dst.clear();
        segmentation_dst.clear();
//         segmentation_dst = SegmentationOutT();
        
        viennagrid::cell_refine( domain_src, domain_dst, refinement_flag_accessor );
//         viennagrid::refine_element( domain_src, segmentation_src, domain_dst, segmentation_dst, refinement_flag_accessor );

//         cells = viennagrid::elements( domain_dst );
        std::cout << "Number of refined Cells: " << viennagrid::lines( domain_dst ).size() << std::endl;
        
    {
      char name[100];
      sprintf( name, "refined_iter%d", iteration_count );
        viennagrid::io::vtk_writer<DomainOutT> vtk_writer;
//         viennagrid::io::add_scalar_data_on_cells<std::string, double>(vtk_writer, "segment_id", "segment_id");
//         vtk_writer(domain_dst, segmentation_dst, name);
        vtk_writer(domain_dst, name);
    }
        
        
        std::swap( domain_from, domain_to );
        std::swap( segmentation_from, segmentation_to );
    }
    
    
    copy_cells( *domain_from, *segmentation_from, domain_out, segments_out );
    
    return iteration_count;
}


struct volume_functor
{
    volume_functor(double volume_) : volume(volume_) {}
    
    template<typename domain_type, typename element_type>
    bool operator() ( domain_type const & domain, element_type const & element )
    {
        return viennagrid::volume( element ) < volume;
    }
    
    double volume;
};




struct smallest_boundary_volume_functor
{
    smallest_boundary_volume_functor(double smallest_boundary_volume_) : smallest_boundary_volume(smallest_boundary_volume_) {}
    
    template<typename domain_type, typename element_type>
    bool operator() ( domain_type const & domain, element_type const & element )
    {
        return viennagrid::volume( viennagrid::dereference_handle( domain, viennagrid::smallest_boundary_volume<viennagrid::line_tag>( domain, element ) ) ) < smallest_boundary_volume;
    }
    
    double smallest_boundary_volume;
};

struct largest_boundary_volume_functor
{
    largest_boundary_volume_functor(double largest_boundary_volume_) : largest_boundary_volume(largest_boundary_volume_) {}
    
    template<typename domain_type, typename element_type>
    bool operator() ( domain_type const & domain, element_type const & element )
    {
        return viennagrid::volume( viennagrid::dereference_handle( domain, viennagrid::largest_boundary_volume<viennagrid::line_tag>( domain, element ) ) ) < largest_boundary_volume;
    }
    
    double largest_boundary_volume;
};





int main()
{
    ///////////////////////////////////////////
    // Volume meshing
    ///////////////////////////////////////////
    
    
//     typedef viennagrid::config::result_of::full_domain_config< viennagrid::tetrahedron_tag, viennagrid::config::point_type_3d, viennagrid::storage::pointer_handle_tag >::type domain_config_type;
    
//     typedef viennagrid::config::tetrahedral_3d_domain domain_type;
    typedef viennagrid::tetrahedral_3d_domain DomainType;
    typedef viennagrid::tetrahedral_3d_segmentation SegmentationType;
    
//     typedef viennagrid::result_of::domain_view<domain_type>::type segment_type;
    typedef viennagrid::result_of::cell<DomainType>::type cell_type;
    typedef viennagrid::result_of::point<DomainType>::type point_type;
    typedef viennagrid::result_of::cell_tag<DomainType>::type cell_tag;
    
    DomainType domain;
    SegmentationType segmentation(domain);
    
    try
    {
        viennagrid::io::vtk_reader<DomainType> my_reader;
    //     my_reader(my_domain, my_segments, "../examples/data/netgen_volume.vtu_main.pvd");
        my_reader(domain, segmentation, "../../examples/data/half-trigate_main.pvd");
    }
    catch (...)
    {
        std::cerr << "File-Reader failed. Aborting program..." << std::endl;
        return EXIT_FAILURE;
    }
    
    

    typedef viennagrid::result_of::cell_range<DomainType>::type cell_range_type;
    typedef viennagrid::result_of::iterator<cell_range_type>::type cell_range_iterator;
    
    point_type to_refine( 0.0, 0.0, -80.0 );
    double refine_radius = 10.0;
    
    
    
    
    DomainType refined_domain;
    SegmentationType refined_segmentation(refined_domain);
    
    
    
    
    
    
    
    
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
    refine( domain, segmentation, refined_domain, refined_segmentation, volume_functor(100.0) );
    
//     copy(domain, adaptively_refined_domain);
    
    
    {        
        viennagrid::io::vtk_writer<DomainType> vtk_writer;
//         viennagrid::io::add_scalar_data_on_cells<std::string, double>(vtk_writer, "segment_id", "segment_id");
        vtk_writer(refined_domain, refined_segmentation, "refined_volume");
    }

    
    
}
