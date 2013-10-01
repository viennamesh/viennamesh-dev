#ifndef VIENNAMESH_DOMAIN_NETGEN_TETRAHEDRON_HPP
#define VIENNAMESH_DOMAIN_NETGEN_TETRAHEDRON_HPP

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/mesh/segmentation.hpp"
#include "viennamesh/base/convert.hpp"


namespace nglib
{
  #include "netgen-5.0.0/nglib/nglib.h"
  
}

// #include "netgen-5.0.0/libsrc/include/nginterface.h"



namespace viennamesh
{
    
    struct netgen_lib
    {
        netgen_lib()
        {
//             std::cout << "Init Netgen" << std::endl;
            nglib::Ng_Init();
//             netgen::printmessage_importance = -1;
        }
        
        ~netgen_lib()
        {
//             std::cout << "Exit Netgen" << std::endl;
            nglib::Ng_Exit();
        }
    };
    
    
    
    struct netgen_tetrahedron_mesh
    {
        typedef int segment_id_type;
        typedef nglib::Ng_Mesh * netgen_mesh_type;
        typedef std::map<segment_id_type, netgen_mesh_type> netgen_mesh_container_type;
        
        netgen_tetrahedron_mesh()
        {
            static netgen_lib lib;
        }        
        
        template<typename SegmentationT>
        void init(SegmentationT const & segmentation)
        {
            deinit();
            for (typename SegmentationT::const_iterator it = segmentation.begin(); it != segmentation.end(); ++it)
                meshes[ it->id() ] = nglib::Ng_NewMesh();
        }
        
        void deinit()
        {
            for (netgen_mesh_container_type::iterator it = meshes.begin(); it != meshes.end(); ++it)
                nglib::Ng_DeleteMesh( it->second );
            meshes.clear();
        }
        
        ~netgen_tetrahedron_mesh()
        {
            deinit();
        }
        
        netgen_mesh_container_type meshes;
    };
    
}    
    
namespace viennagrid
{
    namespace result_of
    {
        template<>
        struct point<viennamesh::netgen_tetrahedron_mesh>
        {
            typedef viennagrid::config::point_type_3d type;
        };
        
        template<>
        struct point<const viennamesh::netgen_tetrahedron_mesh>
        {
            typedef viennagrid::config::point_type_3d type;
        };
    }
}

    
namespace viennamesh
{
    
    
    template<typename triangular_3d_mesh_type, typename triangular_3d_segmentation_type>
    struct convert_impl<triangular_3d_mesh_type, triangular_3d_segmentation_type, netgen_tetrahedron_mesh, NoSegmentation >
    {
        typedef triangular_3d_mesh_type vgrid_mesh_type;
        typedef netgen_tetrahedron_mesh netgen_mesh_type;
        
        typedef vgrid_mesh_type input_mesh_type;
        typedef triangular_3d_segmentation_type input_segmentation_type;
        typedef netgen_mesh_type output_mesh_type;
        typedef NoSegmentation output_segmentation_type;
        
        static bool convert( input_mesh_type const & vgrid_mesh, input_segmentation_type const & input_segmentation,
                             output_mesh_type & netgen_mesh, output_segmentation_type & output_segmentation )
        {
            typedef typename viennagrid::result_of::point<vgrid_mesh_type>::type point_type;
            
            typedef typename viennagrid::result_of::element<vgrid_mesh_type, viennagrid::vertex_tag>::type vertex_type;
            typedef typename viennagrid::result_of::const_handle<vgrid_mesh_type, viennagrid::vertex_tag>::type vertex_const_handle_type;
            typedef typename viennagrid::result_of::element<vgrid_mesh_type, viennagrid::triangle_tag>::type triangle_type;
            
            typedef typename viennagrid::result_of::const_element_range<vgrid_mesh_type, viennagrid::vertex_tag>::type vertex_range_type;
            typedef typename viennagrid::result_of::iterator<vertex_range_type>::type vertex_range_iterator;

            typedef typename viennagrid::result_of::const_element_range<vgrid_mesh_type, viennagrid::triangle_tag>::type triangle_range_type;
            typedef typename viennagrid::result_of::iterator<triangle_range_type>::type triangle_range_iterator;

            typedef typename triangular_3d_segmentation_type::segment_handle_type SegmentType;
            
//             typedef typename input_segmentation_type::segment_ids_container_type segment_ids_container_type;
//             segment_ids_container_type const & used_segments = input_segmentation.segments();
            
//             segment_id_container_type const & used_segments = segments(vgrid_mesh);
            
            netgen_mesh.init( input_segmentation );
            
            for (netgen_mesh_type::netgen_mesh_container_type::const_iterator seg_it = netgen_mesh.meshes.begin(); seg_it != netgen_mesh.meshes.end(); ++seg_it)
            {
                SegmentType const & vgrid_segment = input_segmentation( seg_it->first );
              
                std::map<vertex_const_handle_type, int> vertex_index_map;
                int index = 0;
                
                netgen_mesh_type::netgen_mesh_type current_mesh = seg_it->second;
                
                triangle_range_type triangles = viennagrid::elements<viennagrid::triangle_tag>( vgrid_mesh );
                
                unsigned int triangle_count = 0;
                for (triangle_range_iterator it = triangles.begin(); it != triangles.end(); ++it)
                {
                    triangle_type const & triangle = *it;
                    
//                     typename input_segmentation_type::element_segment_info_type const & element_segment_info = input_segmentation.segment_info(triangle);
                    
//                     if ( !element_segment_info.is_on_segment(seg_it->first) )
//                     if ( !is_face_on_segment(triangle, seg_it->first) )
                    if (!viennagrid::is_in_segment( vgrid_segment, triangle ))
                        continue;
                    
                    ++triangle_count;
                    
                    int indices[3];
                    for (int i = 0; i < 3; ++i)
                    {
                        typename std::map<vertex_const_handle_type, int>::iterator tmp = vertex_index_map.find(viennagrid::elements<viennagrid::vertex_tag>( triangle ).handle_at(i));
                        if (tmp == vertex_index_map.end())
                        {
                            vertex_const_handle_type vh = viennagrid::elements<viennagrid::vertex_tag>( triangle ).handle_at(i);
                            point_type const & vgrid_point = viennagrid::point( vgrid_mesh, vh );
                            
                            tmp = vertex_index_map.insert( std::make_pair(vh, index+1) ).first;     // increase by one because netgen start counting at 1
                            ++index;
                            
                            double netgen_point[3];
                            std::copy( vgrid_point.begin(), vgrid_point.end(), netgen_point );
                            
                            nglib::Ng_AddPoint( current_mesh, netgen_point );
                        }
                        
                        indices[i] = tmp->second;
                    }
                    
//                     if ( faces_outward_on_segment(triangle, seg_it->first) )
//                     if ( element_segment_info.faces_outward_on_segment(seg_it->first) )
                    bool faces_outward = true;
                    bool const * faces_outward_on_segment_pointer = viennagrid::segment_element_info( vgrid_segment, triangle );
                    if (faces_outward_on_segment_pointer)
                      faces_outward = *faces_outward_on_segment_pointer;

//                     if (!viennagrid::faces_outward_on_segment( element_segment_info, seg_it->first ))

                    if (!faces_outward)
                        std::swap( indices[1], indices[2] );
                    
                    nglib::Ng_AddSurfaceElement( current_mesh, nglib::NG_TRIG, indices );
                }
                
//                 std::cout << " Num Triangles in segment " << (*seg_it).first << " = " << triangle_count << std::endl;
            }
    
            return true;
        }
    };
    
    
    
    
    
    template<>
    struct convert_impl<netgen_tetrahedron_mesh, NoSegmentation, viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>
    {              
        
        typedef netgen_tetrahedron_mesh netgen_mesh_type;
        typedef viennagrid::tetrahedral_3d_mesh vgrid_mesh_type;
        
        
        typedef netgen_mesh_type input_mesh_type;
        typedef NoSegmentation input_segmentation_type;
        typedef vgrid_mesh_type output_mesh_type;
        typedef viennagrid::tetrahedral_3d_segmentation output_segmentation_type;

        
        static bool convert( input_mesh_type const & netgen_mesh, input_segmentation_type const & input_segmentation,
                             output_mesh_type & vgrid_mesh, output_segmentation_type & output_segmentation )
        {
            typedef viennagrid::result_of::point<vgrid_mesh_type>::type point_type;
            
            typedef viennagrid::result_of::element<vgrid_mesh_type, viennagrid::vertex_tag>::type vertex_type;
            typedef viennagrid::result_of::handle<vgrid_mesh_type, viennagrid::vertex_tag>::type vertex_handle_type;
            
            typedef viennagrid::result_of::element<vgrid_mesh_type, viennagrid::line_tag>::type line_type;
            typedef viennagrid::result_of::handle<vgrid_mesh_type, viennagrid::line_tag>::type line_handle_type;
            
            typedef viennagrid::result_of::element<vgrid_mesh_type, viennagrid::triangle_tag>::type triangle_type;
            typedef viennagrid::result_of::handle<vgrid_mesh_type, viennagrid::triangle_tag>::type triangle_handle_type;
            
            typedef viennagrid::result_of::element<vgrid_mesh_type, viennagrid::tetrahedron_tag>::type tetrahedron_type;
            typedef viennagrid::result_of::handle<vgrid_mesh_type, viennagrid::tetrahedron_tag>::type tetrahedron_handle_type;

            typedef typename output_segmentation_type::segment_handle_type SegmentType;
            
//             typedef std::vector< std::pair<point_type, vertex_handle_type> > point_vertex_map_type;
//             point_vertex_map_type point_vertex_map;
            
            
            for (typename netgen_mesh_type::netgen_mesh_container_type::const_iterator it = netgen_mesh.meshes.begin(); it != netgen_mesh.meshes.end(); ++it)
            {
//                 vgrid_mesh_type current_tmp;
//                 std::map<int, vertex_handle_type> current_tmp_index_vertex_map;
                
                int num_points = nglib::Ng_GetNP(it->second);
                int num_tets = nglib::Ng_GetNE(it->second);
                
                std::map<int, vertex_handle_type> index_vertex_map;

                SegmentType & segment = output_segmentation.get_make_segment( it->first );
                
                
                for (int i = 0; i < num_points; ++i)
                {
                    double netgen_point[3];
                    nglib::Ng_GetPoint(it->second, i+1, netgen_point);
                    
                    point_type vgrid_point(netgen_point[0], netgen_point[1], netgen_point[2]);
                    
//                     int pit = 0;
//                     for (; pit != point_vertex_map.size(); ++pit)
//                     {
//                         if ( viennagrid::norm_2( vgrid_point - point_vertex_map[i].first ) < 1e-6 )
//                             break;
//                     }
//                     
//                     if (pit == point_vertex_map.size())
//                     {
//                     vertex_handle_type vh = viennagrid::create_unique_vertex( vgrid_mesh, vgrid_point );
//                     point_vertex_map.push_back( std::make_pair(vgrid_point, vh) );
//                     index_vertex_map.insert( std::make_pair(i+1, vh) );
                    
                    index_vertex_map[i+1] = viennagrid::make_unique_vertex( vgrid_mesh, vgrid_point );
                    
                    
//                     current_tmp_index_vertex_map[i+1] = viennagrid::create_unique_vertex( current_tmp, vgrid_point );
//                     }
//                     else
//                         index_vertex_map.insert( std::make_pair(i+1, point_vertex_map[pit].second) );
                }
                
                
                for (int i = 0; i < num_tets; ++i)
                {
                    int netgen_tet[4];
                    nglib::Ng_GetVolumeElement(it->second, i+1, netgen_tet);
                    
                    vertex_handle_type vhs[4];
                    vhs[0] = index_vertex_map[netgen_tet[0]];
                    vhs[1] = index_vertex_map[netgen_tet[1]];
                    vhs[2] = index_vertex_map[netgen_tet[2]];
                    vhs[3] = index_vertex_map[netgen_tet[3]];
                    
                    tetrahedron_handle_type tetrahedron_handle = viennagrid::make_element<tetrahedron_type>( segment, vhs, vhs+4 );
                    
//                     output_segmentation.set_segment_info( viennagrid::dereference_handle(vgrid_mesh, tetrahedron_handle), output_segmentation_type::element_segment_info_type(it->first) );
                    
//                     segment( viennagrid::dereference_handle(vgrid_mesh, tetrahedron_handle) ) = it->first;
                    
                    
//                     vhs[0] = current_tmp_index_vertex_map[netgen_tet[0]];
//                     vhs[1] = current_tmp_index_vertex_map[netgen_tet[1]];
//                     vhs[2] = current_tmp_index_vertex_map[netgen_tet[2]];
//                     vhs[3] = current_tmp_index_vertex_map[netgen_tet[3]];
//                     
//                     viennagrid::create_element<tetrahedron_type>( current_tmp, vhs, vhs+4 );
                }
                
//                 {
//                     std::stringstream tmp;
//                     tmp << "netgen_output_segment_" << it->first << std::endl;
//                     viennagrid::io::vtk_writer<viennagrid::config::tetrahedral_3d_mesh, viennagrid::tetrahedron_tag> vtk_writer;
//                     vtk_writer(current_tmp, tmp.str());
//                 }
            }

            return true;
        }
    };
    
    

    
    
    
}

#endif