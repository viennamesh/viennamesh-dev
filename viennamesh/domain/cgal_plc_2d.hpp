#ifndef VIENNAMESH_DOMAIN_CGAL_PLC_2D_HPP
#define VIENNAMESH_DOMAIN_CGAL_PLC_2D_HPP

#include "viennagrid/domain/config.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennamesh/base/convert.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>


namespace viennamesh
{
    struct cgal_plc_2d_element
    {
        typedef CGAL::Exact_predicates_inexact_constructions_kernel     Kernel;
        
        typedef CGAL::Triangulation_vertex_base_2<Kernel>               VertexBase;
        typedef CGAL::Delaunay_mesh_face_base_2<Kernel>                 FaceBase;
        typedef CGAL::Triangulation_data_structure_2<VertexBase, FaceBase> Triangulation_structure;
        
        typedef CGAL::Constrained_Delaunay_triangulation_2<Kernel, Triangulation_structure> CDT;
        
        typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;
        
        typedef CDT::Vertex_handle Vertex_handle;
        typedef CDT::Point Point;
      
        
        CDT cdt;
        std::list<Point> cgal_list_of_holes;
    };
    
    struct cgal_plc_2d_domain
    {
        typedef std::vector<cgal_plc_2d_element> cell_container;
        cell_container cells;
    };
    
    
    
    
    template<>
    struct convert_impl<viennagrid::config::plc_2d_domain, cgal_plc_2d_domain>
    {
        typedef viennagrid::config::plc_2d_domain vgrid_domain_type;
        typedef viennagrid::config::plc_2d_cell vgrid_element_type;
        
        typedef cgal_plc_2d_domain cgal_domain_type;
        typedef cgal_plc_2d_element cgal_element_type;
        
        
        typedef vgrid_domain_type input_domain_type;
        typedef vgrid_element_type input_element_type;
        
        typedef cgal_domain_type output_domain_type;
        typedef cgal_element_type output_element_type;
        
        static bool convert( vgrid_domain_type const & vgrid_domain, cgal_domain_type & cgal_domain )
        {
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::plc_tag>::type plc_type;
            typedef viennagrid::result_of::element_hook<vgrid_domain_type, viennagrid::plc_tag>::type plc_hook_type;
            
            typedef viennagrid::result_of::const_element_range<vgrid_domain_type, viennagrid::plc_tag>::type plc_range_type;
            typedef viennagrid::result_of::iterator<plc_range_type>::type plc_range_iterator;
            
            typedef viennagrid::result_of::point_type<vgrid_domain_type>::type point_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::vertex_tag>::type vertex_type;
            typedef viennagrid::result_of::const_element_hook<vgrid_domain_type, viennagrid::vertex_tag>::type vertex_const_hook_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::line_tag>::type line_type;
            typedef viennagrid::result_of::element_hook<vgrid_domain_type, viennagrid::line_tag>::type line_hook_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::polygon_tag>::type polygon_type;
            typedef viennagrid::result_of::element_hook<vgrid_domain_type, viennagrid::polygon_tag>::type polygon_hook_type;
            
            
            typedef viennagrid::result_of::const_element_range<vgrid_element_type, viennagrid::vertex_tag>::type vertex_range_type;
            typedef viennagrid::result_of::hook_iterator<vertex_range_type>::type vertex_range_hook_iterator;
            
            typedef viennagrid::result_of::const_element_range<vgrid_element_type, viennagrid::line_tag>::type line_range_type;
            typedef viennagrid::result_of::iterator<line_range_type>::type line_range_iterator;
            
            
            
            
            plc_range_type plcs = viennagrid::elements<viennagrid::plc_tag>( vgrid_domain );
            cgal_domain.cells.resize(plcs.size());
            
            cgal_domain_type::cell_container::iterator jt = cgal_domain.cells.begin();
            
            for (plc_range_iterator it = plcs.begin(); it != plcs.end(); ++it)
            {
                output_element_type & cgal_element = *jt;
                vgrid_element_type const & vgrid_element = *it;
                
                std::map<vertex_const_hook_type, cgal_element_type::Vertex_handle> vertex_handle_map;
                
                vertex_range_type vertices = viennagrid::elements<viennagrid::vertex_tag>(vgrid_element);
                for (vertex_range_hook_iterator it = vertices.hook_begin(); it != vertices.hook_end(); ++it)
                {
                    vertex_const_hook_type const & vtx_hook = *it;
                    vertex_type const & vtx = viennagrid::dereference_hook(vgrid_domain, *it);
                    point_type const & vgrid_point = viennagrid::point(vgrid_domain, vtx );
                    
                    cgal_element_type::Vertex_handle handle = cgal_element.cdt.insert( cgal_element_type::Point(vgrid_point[0], vgrid_point[1]) );
                    
                    vertex_handle_map[vtx_hook] = handle;
                }

                
                line_range_type lines = viennagrid::elements<viennagrid::line_tag>(vgrid_element);
                for (line_range_iterator it = lines.begin(); it != lines.end(); ++it)
                {
                    line_type const & line = *it;
                    
                    vertex_const_hook_type vgrid_v0 = viennagrid::elements<viennagrid::vertex_tag>(line).hook_at(0);
                    vertex_const_hook_type vgrid_v1 = viennagrid::elements<viennagrid::vertex_tag>(line).hook_at(1);
                    
                    cgal_element_type::Vertex_handle cgal_v0 = vertex_handle_map[vgrid_v0];
                    cgal_element_type::Vertex_handle cgal_v1 = vertex_handle_map[vgrid_v1];
                    
                    cgal_element.cdt.insert_constraint(cgal_v0, cgal_v1);
                }
                
                const std::vector<point_type> & vgrid_list_of_holes = viennagrid::hole_points<vgrid_domain_type>(vgrid_element);
                
                for (std::vector<point_type>::const_iterator it = vgrid_list_of_holes.begin(); it != vgrid_list_of_holes.end(); ++it)
                    cgal_element.cgal_list_of_holes.push_back( cgal_element_type::Point( (*it)[0], (*it)[1] ) );
            }
            
            return true;
        }  
    };
    
    
    
    
    
    template<>
    struct convert_impl<cgal_plc_2d_domain, viennagrid::config::triangular_2d_domain>
    {              
        typedef cgal_plc_2d_domain cgal_domain_type;
        typedef cgal_plc_2d_element cgal_element_type;
        
        typedef viennagrid::config::triangular_2d_domain vgrid_domain_type;
        typedef viennagrid::config::triangular_2d_cell vgrid_element_type;
        
        
        typedef cgal_domain_type input_domain_type;
        typedef cgal_element_type input_element_type;
        
        typedef vgrid_domain_type output_domain_type;
        typedef vgrid_element_type output_element_type;

        
        static bool convert( cgal_domain_type const & cgal_domain, vgrid_domain_type & vgrid_domain )
        {
            typedef viennagrid::result_of::point_type<vgrid_domain_type>::type triangle_point_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::vertex_tag>::type triangle_vertex_type;
            typedef viennagrid::result_of::element_hook<vgrid_domain_type, viennagrid::vertex_tag>::type triangle_vertex_hook_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::line_tag>::type triangle_line_type;
            typedef viennagrid::result_of::element_hook<vgrid_domain_type, viennagrid::line_tag>::type triangle_line_hook_type;
            
            typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::triangle_tag>::type triangle_triangle_type;
            typedef viennagrid::result_of::element_hook<vgrid_domain_type, viennagrid::triangle_tag>::type triangle_triangle_hook_type;
            
            
            std::map<cgal_element_type::Point, triangle_vertex_hook_type> points;
            
            for (cgal_domain_type::cell_container::const_iterator it = cgal_domain.cells.begin(); it != cgal_domain.cells.end(); ++it)
            {
                cgal_element_type const & cgal_element = *it;
                
                for(cgal_element_type::CDT::Finite_faces_iterator fit = cgal_element.cdt.finite_faces_begin(); fit != cgal_element.cdt.finite_faces_end(); ++fit) 
                {
                    if(fit->is_in_domain())
                    {
                        typedef cgal_element_type::CDT::Triangle Triangle;
                        Triangle tri = cgal_element.cdt.triangle(fit);
                        
                        viennagrid::storage::static_array<triangle_vertex_hook_type, 3> vgrid_vtx;
                        
                        for (int i = 0; i < 3; ++i)
                        {
                            std::map<cgal_element_type::Point, triangle_vertex_hook_type>::iterator pit = points.find( tri[i] );
                            if (pit == points.end())
                            {
                                vgrid_vtx[i] = viennagrid::create_element<triangle_vertex_type>( vgrid_domain, triangle_point_type(tri[i].x(), tri[i].y()) );
                                points[ tri[i] ] = vgrid_vtx[i];
                            }
                            else
                                vgrid_vtx[i] = pit->second;
                        }
                        
                        viennagrid::create_element<triangle_triangle_type>( vgrid_domain, vgrid_vtx.begin(), vgrid_vtx.end() );
                    }
                }
                
            }
            
            return true;
        }
    };
    
    

    
    
    
}

#endif