#include <iostream>

// using std::string;
// #include "external/netgen-5.0.0/libsrc/general/ngexception.hpp"

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"

#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/neighbour_iteration.hpp"

#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/refine.hpp"
#include "viennagrid/algorithm/surface.hpp"






template<typename SrcMeshT, typename SrcSegmentationT, typename DstMeshT, typename DstSegmentationT>
void copy_cells(SrcMeshT const & src_mesh,  SrcSegmentationT const & src_segmentation,
                DstMeshT & dst_mesh,        DstSegmentationT & dst_segmentation )
{
//     typedef typename src_segment_container_type::value_type src_segment_type;
//     typedef typename dst_segment_container_type::value_type dst_segment_type;
    typedef typename viennagrid::result_of::segment<SrcSegmentationT>::type SrcSegmentType;
    typedef typename viennagrid::result_of::segment<DstSegmentationT>::type DstSegmentType;
    
    typedef typename viennagrid::result_of::cell_tag<SrcMeshT>::type  SrcCellTag;
    typedef typename viennagrid::result_of::cell<SrcMeshT>::type      SrcCellType;
    
    typedef typename viennagrid::result_of::const_vertex_range<SrcMeshT>::type    SrcVertexRangeType;
    typedef typename viennagrid::result_of::iterator<SrcVertexRangeType>::type      SrcVertexRangeIterator;
    
    typedef typename viennagrid::result_of::const_cell_range<SrcSegmentType>::type  SrcCellRangeType;
    typedef typename viennagrid::result_of::iterator<SrcCellRangeType>::type        SrcCellRangeIterator;
    
    typedef typename viennagrid::result_of::const_vertex_handle<SrcMeshT>::type   SrcConstVertexHandle;
    typedef typename viennagrid::result_of::vertex_handle<DstMeshT>::type         DstVertexHandleType;
    
    if (&src_mesh == &dst_mesh)
        return;
    
    dst_mesh.clear();
    dst_segmentation.clear();
    
    std::map<SrcConstVertexHandle, DstVertexHandleType> vertex_handle_map;
    
    SrcVertexRangeType vertices = viennagrid::elements( src_mesh );
//     std::cout << "Copy: vertex count = " << vertices.size() << std::endl;
    for (SrcVertexRangeIterator it = vertices.begin(); it != vertices.end(); ++it)
        vertex_handle_map[it.handle()] = viennagrid::make_vertex( dst_mesh, viennagrid::point(src_mesh, *it) );
    

    
    for (typename SrcSegmentationT::const_iterator seg_it = src_segmentation.begin(); seg_it != src_segmentation.end(); ++seg_it)
    {
//         dst_segments.push_back( viennagrid::make_view<dst_segment_type>(dst_mesh) );
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
            
            typedef typename viennagrid::result_of::cell<DstMeshT>::type DstCellType;
            viennagrid::make_element<DstCellType>( dst_segment, vertex_handles.begin(), vertex_handles.end() );
        }
    }
}



template<typename MeshInT, typename SegmentationInT, typename MeshOutT, typename SegmentationOutT, typename FunctorT>
unsigned int refine( MeshInT const & mesh_in,   SegmentationInT const & segments_in,
                     MeshOutT & mesh_out,       SegmentationOutT & segments_out,
                     FunctorT functor, unsigned int max_iteration_count = 10 )
{
    typedef typename viennagrid::result_of::cell<MeshInT>::type CellType;
    typedef typename viennagrid::result_of::cell_tag<MeshInT>::type cell_tag;
    typedef typename viennagrid::result_of::const_cell_range<MeshInT>::type cell_range_type;
    typedef typename viennagrid::result_of::iterator<cell_range_type>::type cell_range_itertor;
    
    MeshOutT tmp_mesh;
    SegmentationOutT tmp_segmentation(tmp_mesh);
    
    
    copy_cells( mesh_in, segments_in, tmp_mesh, tmp_segmentation );
    
    
    MeshOutT * mesh_from = &tmp_mesh;
    SegmentationOutT * segmentation_from = &tmp_segmentation;
    
    MeshOutT * mesh_to = &mesh_out;
    SegmentationOutT * segmentation_to = &segments_out;




    
    
    
    std::deque<MeshOutT> refined_meshs;
    
    bool all_cells_good = false;
    unsigned int iteration_count = 0;
    for (; iteration_count < max_iteration_count; ++iteration_count)
    {
        std::deque<bool> refinement_flag;
        typename viennagrid::result_of::accessor<std::deque<bool>, CellType>::type refinement_flag_accessor(refinement_flag);
        
        std::cout << "Iteration " << iteration_count << std::endl;
        
        MeshOutT const & mesh_src = *mesh_from; //refined_meshs.empty() ? mesh_in : refined_meshs.back();
        SegmentationOutT const & segmentation_src = *segmentation_from;
        
        cell_range_type cells = viennagrid::elements( mesh_src );
        
        std::cout << "Number of Cells: " << cells.size() << std::endl;
        
        unsigned int bad_cells_count = 0;
        all_cells_good = true;
        for (cell_range_itertor it = cells.begin(); it != cells.end(); ++it)
        {
            if ( !functor( mesh_src, *it ) )
            {
              refinement_flag_accessor(*it) = true;
              all_cells_good = false;
              
              ++bad_cells_count;
            }
        }
        
//     {
//       char name[100];
//       sprintf( name, "src_iter%d", iteration_count );
//         viennagrid::io::vtk_writer<MeshOutT> vtk_writer;
// //         viennagrid::io::add_scalar_data_on_cells(vtk_writer, refinement_flag_double_accessor, "to_refine");
//         vtk_writer(mesh_src, segmentation_src, name);
//     }
        
        std::cout << "Bad cells count = " << bad_cells_count << std::endl;
        
        if (all_cells_good)
            break;
        
//         refined_meshs.resize( refined_meshs.size()+1 );
            
        MeshOutT & mesh_dst = *mesh_to; //refined_meshs.empty() ? mesh_in : refined_meshs.back();
        SegmentationOutT & segmentation_dst = *segmentation_to;
        
//         viennagrid::clear_mesh(mesh_dst);
        mesh_dst.clear();
        segmentation_dst.clear();
//         segmentation_dst = SegmentationOutT();
        
        viennagrid::cell_refine( mesh_src, mesh_dst, refinement_flag_accessor );
//         viennagrid::refine_element( mesh_src, segmentation_src, mesh_dst, segmentation_dst, refinement_flag_accessor );

//         cells = viennagrid::elements( mesh_dst );
        std::cout << "Number of refined Cells: " << viennagrid::lines( mesh_dst ).size() << std::endl;
        
    {
      char name[100];
      sprintf( name, "refined_iter%d", iteration_count );
        viennagrid::io::vtk_writer<MeshOutT> vtk_writer;
//         viennagrid::io::add_scalar_data_on_cells<std::string, double>(vtk_writer, "segment_id", "segment_id");
//         vtk_writer(mesh_dst, segmentation_dst, name);
        vtk_writer(mesh_dst, name);
    }
        
        
        std::swap( mesh_from, mesh_to );
        std::swap( segmentation_from, segmentation_to );
    }
    
    
    copy_cells( *mesh_from, *segmentation_from, mesh_out, segments_out );
    
    return iteration_count;
}


struct volume_functor
{
    volume_functor(double volume_) : volume(volume_) {}
    
    template<typename mesh_type, typename element_type>
    bool operator() ( mesh_type const & mesh, element_type const & element )
    {
        return viennagrid::volume( element ) < volume;
    }
    
    double volume;
};




struct smallest_boundary_volume_functor
{
    smallest_boundary_volume_functor(double smallest_boundary_volume_) : smallest_boundary_volume(smallest_boundary_volume_) {}
    
    template<typename mesh_type, typename element_type>
    bool operator() ( mesh_type const & mesh, element_type const & element )
    {
        return viennagrid::volume( viennagrid::dereference_handle( mesh, viennagrid::smallest_boundary_volume<viennagrid::line_tag>( mesh, element ) ) ) < smallest_boundary_volume;
    }
    
    double smallest_boundary_volume;
};

struct largest_boundary_volume_functor
{
    largest_boundary_volume_functor(double largest_boundary_volume_) : largest_boundary_volume(largest_boundary_volume_) {}
    
    template<typename mesh_type, typename element_type>
    bool operator() ( mesh_type const & mesh, element_type const & element )
    {
        return viennagrid::volume( viennagrid::dereference_handle( mesh, viennagrid::largest_boundary_volume<viennagrid::line_tag>( mesh, element ) ) ) < largest_boundary_volume;
    }
    
    double largest_boundary_volume;
};





int main()
{
    ///////////////////////////////////////////
    // Volume meshing
    ///////////////////////////////////////////
    
    
//     typedef viennagrid::config::result_of::full_mesh_config< viennagrid::tetrahedron_tag, viennagrid::config::point_type_3d, viennagrid::storage::pointer_handle_tag >::type mesh_config_type;
    
//     typedef viennagrid::config::tetrahedral_3d_mesh mesh_type;
    typedef viennagrid::tetrahedral_3d_mesh MeshType;
    typedef viennagrid::tetrahedral_3d_segmentation SegmentationType;
    
//     typedef viennagrid::result_of::mesh_view<mesh_type>::type segment_type;
    typedef viennagrid::result_of::cell<MeshType>::type cell_type;
    typedef viennagrid::result_of::point<MeshType>::type point_type;
    typedef viennagrid::result_of::cell_tag<MeshType>::type cell_tag;
    
    MeshType mesh;
    SegmentationType segmentation(mesh);
    
    try
    {
        viennagrid::io::vtk_reader<MeshType> my_reader;
    //     my_reader(my_mesh, my_segments, "../examples/data/netgen_volume.vtu_main.pvd");
        my_reader(mesh, segmentation, "../../examples/data/half-trigate_main.pvd");
    }
    catch (...)
    {
        std::cerr << "File-Reader failed. Aborting program..." << std::endl;
        return EXIT_FAILURE;
    }
    
    

    typedef viennagrid::result_of::cell_range<MeshType>::type cell_range_type;
    typedef viennagrid::result_of::iterator<cell_range_type>::type cell_range_iterator;
    
    point_type to_refine( 0.0, 0.0, -80.0 );
    double refine_radius = 10.0;
    
    
    
    
    MeshType refined_mesh;
    SegmentationType refined_segmentation(refined_mesh);
    
    
    
    
    
    
    
    
//     cell_range_type cells = viennagrid::elements( mesh );
//     unsigned int refine_count = 0;
//     for (cell_range_iterator it = cells.begin(); it != cells.end(); ++it)
//     {
// //             std::cout << viennagrid::volume( mesh,
// //                 viennagrid::dereference_handle( mesh, viennagrid::smallest_boundary_volume<viennagrid::line_tag>( mesh, *it ) ) ) << std::endl;
//         
//         point_type centroid = viennagrid::centroid( mesh, *it );
//         if (viennagrid::norm_2(centroid - to_refine) < refine_radius)
// //         if (viennagrid::volume(mesh, *it) < 10.0)
//         {
// 
//             
//             viennadata::access<viennagrid::refinement_key, bool>()(*it) = true;
//             
// //             std::cout << "REFINE!!" << std::endl;
//             ++refine_count;
//         }
//     }
    
//     viennagrid::refine<cell_tag>(mesh, segments, refined_mesh, refined_segments, viennagrid::local_refinement_tag());
    
//     std::cout << "Cell count = " << cells.size() << std::endl;
//     std::cout << "Refine count = " << refine_count << std::endl;
    
    
//     mesh_type adaptively_refined_mesh = viennagrid::refine<cell_tag>(mesh, viennagrid::local_refinement_tag());
    
//     {
//         cell_range_type cells = viennagrid::elements( adaptively_refined_mesh );
//         for (cell_range_iterator it = cells.begin(); it != cells.end(); ++it)
//             std::cout << viennagrid::volume( adaptively_refined_mesh, *it ) << std::endl;
//     }
//     mesh_type adaptively_refined_mesh;
//     refine(mesh, adaptively_refined_mesh, volume_functor(20.0));
    refine( mesh, segmentation, refined_mesh, refined_segmentation, volume_functor(100.0) );
    
//     copy(mesh, adaptively_refined_mesh);
    
    
    {        
        viennagrid::io::vtk_writer<MeshType> vtk_writer;
//         viennagrid::io::add_scalar_data_on_cells<std::string, double>(vtk_writer, "segment_id", "segment_id");
        vtk_writer(refined_mesh, refined_segmentation, "refined_volume");
    }

    
    
}
