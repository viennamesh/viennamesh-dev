#ifndef VIENNAMESH_DOMAIN_CGAL_DELAUNAY_TETRAHEDRON_HPP
#define VIENNAMESH_DOMAIN_CGAL_DELAUNAY_TETRAHEDRON_HPP

#include "viennagrid/domain/config.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennamesh/base/convert.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Projection_traits_xy_3.h>

#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <CGAL/Mesh_polyhedron_3.h>

#include <CGAL/Polyhedral_mesh_domain_3.h>
#include <CGAL/Polyhedron_copy_3.h>
#include <CGAL/Polyhedral_mesh_domain_with_features_3.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/refine_mesh_3.h>


namespace viennamesh
{
   
    struct cgal_mesh_polyhedron_domain
    {
        typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

        typedef CGAL::Mesh_polyhedron_3<K>::type MeshPolyhedron;
        typedef CGAL::Polyhedral_mesh_domain_with_features_3<K, MeshPolyhedron> Mesh_domain;
        
        MeshPolyhedron polyhedron;
        
        typedef MeshPolyhedron::Point_3 Point;
        
        typedef std::vector< viennagrid::storage::static_array<Point, 2> > feature_lines_type;
        feature_lines_type feature_lines;
    };
    
    struct cgal_delauney_tetdrahedron_domain
    {
        typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
        
        typedef CGAL::Mesh_polyhedron_3<K>::type MeshPolyhedron;
        typedef CGAL::Polyhedral_mesh_domain_with_features_3<K, MeshPolyhedron> Mesh_domain;

        // Triangulation
        typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;
        typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr, Mesh_domain::Corner_index,Mesh_domain::Curve_segment_index> C3t3;
        
        C3t3 tetdrahedron_triangulation;;
    };
    
    
    
    
    template<>
    struct convert_impl<viennagrid::config::triangular_3d_domain, cgal_mesh_polyhedron_domain>
    {
        typedef viennagrid::config::triangular_3d_domain vgrid_domain_type;
        typedef cgal_mesh_polyhedron_domain cgal_domain_type;
        
        
        typedef vgrid_domain_type input_domain_type;
        typedef cgal_domain_type output_domain_type;
        
        static bool convert( vgrid_domain_type const & vgrid_domain, cgal_domain_type & cgal_domain )
        {
            typedef viennagrid::result_of::point_type<vgrid_domain_type>::type point_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::vertex_tag>::type vertex_type;
            typedef viennagrid::result_of::const_element_handle<vgrid_domain_type, viennagrid::vertex_tag>::type vertex_const_handle_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::line_tag>::type line_type;
            typedef viennagrid::result_of::element_handle<vgrid_domain_type, viennagrid::line_tag>::type line_handle_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::triangle_tag>::type triangle_type;
            typedef viennagrid::result_of::element_handle<vgrid_domain_type, viennagrid::triangle_tag>::type triangle_handle_type;
            
            
            typedef viennagrid::result_of::const_element_range<vgrid_domain_type, viennagrid::triangle_tag>::type triangle_range_type;
            typedef viennagrid::result_of::iterator<triangle_range_type>::type triangle_range_iterator;
            
            typedef viennagrid::result_of::const_element_range<vgrid_domain_type, viennagrid::line_tag>::type line_range_type;
            typedef viennagrid::result_of::iterator<line_range_type>::type line_range_iterator;
            

            typedef cgal_domain_type::MeshPolyhedron::Point_3 Point;
            
            
            triangle_range_type triangles = viennagrid::elements<viennagrid::triangle_tag>( vgrid_domain );
            for (triangle_range_iterator it = triangles.begin(); it != triangles.end(); ++it)
            {
                triangle_type const & tri = *it;
                
                point_type const & p0 = viennagrid::point( vgrid_domain, viennagrid::elements<viennagrid::vertex_tag>(tri)[0] );
                point_type const & p1 = viennagrid::point( vgrid_domain, viennagrid::elements<viennagrid::vertex_tag>(tri)[1] );
                point_type const & p2 = viennagrid::point( vgrid_domain, viennagrid::elements<viennagrid::vertex_tag>(tri)[2] );
                
                cgal_domain.polyhedron.make_triangle( Point(p0[0], p0[1], p0[2]), Point(p1[0], p1[1], p1[2]), Point(p2[0], p2[1], p2[2]) );
            }
            
            
            
            
            
            line_range_type lines = viennagrid::elements<viennagrid::line_tag>( vgrid_domain );
            
            cgal_domain.feature_lines.resize( lines.size() );
            
            unsigned int index = 0;
            for (line_range_iterator it = lines.begin(); it != lines.end(); ++it, ++index)
            {
                line_type const & line = *it;
                
                point_type const & p0 = viennagrid::point( vgrid_domain, viennagrid::elements<viennagrid::vertex_tag>(line)[0] );
                point_type const & p1 = viennagrid::point( vgrid_domain, viennagrid::elements<viennagrid::vertex_tag>(line)[1] );
                
                cgal_domain.feature_lines[index][0] = Point(p0[0], p0[1], p0[2]);
                cgal_domain.feature_lines[index][1] = Point(p1[0], p1[1], p1[2]);
            }
            
            return true;
        }
    };
    
    
    
    
    
    template<>
    struct convert_impl<cgal_delauney_tetdrahedron_domain, viennagrid::config::tetrahedral_3d_domain>
    {              
        typedef cgal_delauney_tetdrahedron_domain cgal_domain_type;
        typedef viennagrid::config::tetrahedral_3d_domain vgrid_domain_type;
        
        
        typedef cgal_domain_type input_domain_type;
        typedef vgrid_domain_type output_domain_type;

        
        static bool convert( cgal_domain_type const & cgal_domain, vgrid_domain_type & vgrid_domain )
        {
            typedef viennagrid::result_of::point_type<vgrid_domain_type>::type point_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::vertex_tag>::type vertex_type;
            typedef viennagrid::result_of::element_handle<vgrid_domain_type, viennagrid::vertex_tag>::type vertex_handle_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::line_tag>::type line_type;
            typedef viennagrid::result_of::element_handle<vgrid_domain_type, viennagrid::line_tag>::type line_handle_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::triangle_tag>::type triangle_type;
            typedef viennagrid::result_of::element_handle<vgrid_domain_type, viennagrid::triangle_tag>::type triangle_handle_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::tetrahedron_tag>::type tetrahedron_type;
            typedef viennagrid::result_of::element_handle<vgrid_domain_type, viennagrid::tetrahedron_tag>::type tetrahedron_handle_type;
            
            
            
            typedef cgal_domain_type::Tr::Vertex_handle Vertex_handle;
            typedef cgal_domain_type::C3t3::Cell_iterator  Cell_iterator;
            
            std::map<Vertex_handle, vertex_handle_type> points;
            
            int mesh_faces_counter = 0;
            for (Cell_iterator it = cgal_domain.tetdrahedron_triangulation.cells_in_complex_begin(); it != cgal_domain.tetdrahedron_triangulation.cells_in_complex_end(); ++it)
            {
                cgal_domain_type::Tr::Cell t = *it;
                
                vertex_handle_type vgrid_vtx[4];
                
                for (int i = 0; i < 4; ++i)
                {
                    std::map<Vertex_handle, vertex_handle_type>::iterator pit = points.find( t.vertex(i) );
                    if (pit == points.end())
                    {
                        point_type tmp;
                        tmp[0] = t.vertex(i)->point().x();
                        tmp[2] = t.vertex(i)->point().y();
                        tmp[1] = t.vertex(i)->point().z();
                        
                        vgrid_vtx[i] = viennagrid::create_element<vertex_type>( vgrid_domain, tmp );
                        points[ t.vertex(i) ] = vgrid_vtx[i];
                    }
                    else
                        vgrid_vtx[i] = pit->second;
                }
                
                viennagrid::create_element<tetrahedron_type>( vgrid_domain, vgrid_vtx, vgrid_vtx+4 );
            }
            
            return true;
        }
    };
    
    

    
    
    
}

#endif