#ifndef VIENNAMESH_DOMAIN_NETGEN_TETRAHEDRON_HPP
#define VIENNAMESH_DOMAIN_NETGEN_TETRAHEDRON_HPP

#include "viennagrid/domain/config.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennamesh/base/convert.hpp"



namespace nglib
{
    #include "netgen-5.0.0/nglib/nglib.h"
}





namespace viennamesh
{
    
    struct netgen_lib
    {
        netgen_lib()
        {
            std::cout << "Init Netgen" << std::endl;
            nglib::Ng_Init();
        }
        
        ~netgen_lib()
        {
            std::cout << "Exit Netgen" << std::endl;
            nglib::Ng_Exit();
        }
    };
    
    
    
   
    struct netgen_tetrahedron_domain
    {
        netgen_tetrahedron_domain()
        {
            static netgen_lib lib;
            mesh = nglib::Ng_NewMesh();
        }
        
        ~netgen_tetrahedron_domain()
        {
            nglib::Ng_DeleteMesh(mesh);
        }
        
        nglib::Ng_Mesh * mesh;
    };
    
    
    
    
    template<>
    struct convert_impl<viennagrid::config::triangular_3d_domain, netgen_tetrahedron_domain>
    {
        typedef viennagrid::config::triangular_3d_domain vgrid_domain_type;
        typedef netgen_tetrahedron_domain netgen_domain_type;
        
        typedef vgrid_domain_type input_domain_type;
        typedef netgen_domain_type output_domain_type;
        
        static bool convert( vgrid_domain_type const & vgrid_domain, netgen_domain_type & netgen_domain )
        {
            typedef viennagrid::result_of::point_type<vgrid_domain_type>::type point_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::vertex_tag>::type vertex_type;
            typedef viennagrid::result_of::const_element_hook<vgrid_domain_type, viennagrid::vertex_tag>::type vertex_const_hook_type;
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::triangle_tag>::type triangle_type;
            
            typedef viennagrid::result_of::const_element_range<vgrid_domain_type, viennagrid::vertex_tag>::type vertex_range_type;
            typedef viennagrid::result_of::hook_iterator<vertex_range_type>::type vertex_range_hook_iterator;

            typedef viennagrid::result_of::const_element_range<vgrid_domain_type, viennagrid::triangle_tag>::type triangle_range_type;
            typedef viennagrid::result_of::iterator<triangle_range_type>::type triangle_range_iterator;

            
            
            std::map<vertex_const_hook_type, int> vertex_index_map;
            
            
            int index = 0;
            vertex_range_type vertices = viennagrid::elements<viennagrid::vertex_tag>( vgrid_domain );
            std::cout << " Num Points = " << vertices.size() << std::endl;
            for (vertex_range_hook_iterator it = vertices.hook_begin(); it != vertices.hook_end(); ++it, ++index)
            {
                vertex_const_hook_type vh = *it;
                point_type const & vgrid_point = viennagrid::point( vgrid_domain, vh );
                
                vertex_index_map.insert( std::make_pair(vh, index) );
                
                double netgen_point[3];
                std::copy( vgrid_point.begin(), vgrid_point.end(), netgen_point );
                
                nglib::Ng_AddPoint( netgen_domain.mesh, netgen_point );
            }
            
            triangle_range_type triangles = viennagrid::elements<viennagrid::triangle_tag>( vgrid_domain );
            std::cout << " Num Triangles = " << triangles.size() << std::endl;
            for (triangle_range_iterator it = triangles.begin(); it != triangles.end(); ++it)
            {
                triangle_type const & triangle = *it;
                
                int indices[3];
                indices[0] = vertex_index_map[ viennagrid::elements<viennagrid::vertex_tag>( triangle ).hook_at(0) ] +1;
                indices[1] = vertex_index_map[ viennagrid::elements<viennagrid::vertex_tag>( triangle ).hook_at(1) ] +1;
                indices[2] = vertex_index_map[ viennagrid::elements<viennagrid::vertex_tag>( triangle ).hook_at(2) ] +1;
                
                point_type p[3];
                p[0] = viennagrid::point( vgrid_domain, viennagrid::elements<viennagrid::vertex_tag>( triangle ).hook_at(0) );
                p[1] = viennagrid::point( vgrid_domain, viennagrid::elements<viennagrid::vertex_tag>( triangle ).hook_at(1) );
                p[2] = viennagrid::point( vgrid_domain, viennagrid::elements<viennagrid::vertex_tag>( triangle ).hook_at(2) );
                
                nglib::Ng_AddSurfaceElement( netgen_domain.mesh, nglib::NG_TRIG, indices );
            }
    
            return true;
        }
    };
    
    
    
    
    
    template<>
    struct convert_impl<netgen_tetrahedron_domain, viennagrid::config::tetrahedral_3d_domain>
    {              
        typedef netgen_tetrahedron_domain netgen_domain_type;
        typedef viennagrid::config::tetrahedral_3d_domain vgrid_domain_type;
        
        
        typedef netgen_domain_type input_domain_type;
        typedef vgrid_domain_type output_domain_type;

        
        static bool convert( netgen_domain_type const & netgen_domain, vgrid_domain_type & vgrid_domain )
        {
            typedef viennagrid::result_of::point_type<vgrid_domain_type>::type point_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::vertex_tag>::type vertex_type;
            typedef viennagrid::result_of::element_hook<vgrid_domain_type, viennagrid::vertex_tag>::type vertex_hook_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::line_tag>::type line_type;
            typedef viennagrid::result_of::element_hook<vgrid_domain_type, viennagrid::line_tag>::type line_hook_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::triangle_tag>::type triangle_type;
            typedef viennagrid::result_of::element_hook<vgrid_domain_type, viennagrid::triangle_tag>::type triangle_hook_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::tetrahedron_tag>::type tetrahedron_type;
            typedef viennagrid::result_of::element_hook<vgrid_domain_type, viennagrid::tetrahedron_tag>::type tetrahedron_hook_type;
            
            
            
            int num_points = nglib::Ng_GetNP(netgen_domain.mesh);
            int num_tets = nglib::Ng_GetNE(netgen_domain.mesh);
            
            std::cout << "Num Points: " << num_points << std::endl;
            std::cout << "Num Tetdrahedrons: " << num_tets << std::endl;
            
            std::map<int, vertex_hook_type> index_vertex_map;
            
            for (int i = 0; i < num_points; ++i)
            {
                double netgen_point[3];
                nglib::Ng_GetPoint(netgen_domain.mesh, i+1, netgen_point);
                
                vertex_hook_type vh = viennagrid::create_element<vertex_type>( vgrid_domain, point_type(netgen_point[0], netgen_point[1], netgen_point[2]) );
                
                index_vertex_map.insert( std::make_pair(i+1, vh) );
            }
            
            
            for (int i = 0; i < num_tets; ++i)
            {
                int netgen_tet[4];
                nglib::Ng_GetVolumeElement(netgen_domain.mesh, i+1, netgen_tet);
                
                vertex_hook_type vhs[4];
                vhs[0] = index_vertex_map[netgen_tet[0]];
                vhs[1] = index_vertex_map[netgen_tet[1]];
                vhs[2] = index_vertex_map[netgen_tet[2]];
                vhs[3] = index_vertex_map[netgen_tet[3]];
                
                viennagrid::create_element<tetrahedron_type>( vgrid_domain, vhs, vhs+4 );
            }
            
            return true;
        }
    };
    
    

    
    
    
}

#endif