#ifndef VIENNAMESH_ALGORITHM_EXTRACT_PLC_HPP
#define VIENNAMESH_ALGORITHM_EXTRACT_PLC_HPP

#include "viennagrid/domain/segmentation.hpp"

#include "viennamesh/utils/utils.hpp"



namespace viennamesh
{

template<typename hull_domain_type, typename element_handle_type, typename point_type, typename hull_view_type, typename visited_container_type>
void recursively_add_neighbours( hull_domain_type & hull_domain, element_handle_type const & element_handle, point_type const & normal_vector, visited_container_type & visited_container, hull_view_type & view )
{
    typedef typename viennagrid::storage::handle::value_type<element_handle_type>::type element_type;
    element_type & element = viennagrid::dereference_handle( hull_domain, element_handle );
    
    if (viennagrid::look_up(visited_container, element))
        return;
    
    
    point_type current_normal_vector = viennagrid::normal_vector( hull_domain, element );
    current_normal_vector /= viennagrid::norm_2(current_normal_vector);
    
    if ( std::abs(viennagrid::inner_prod(normal_vector, current_normal_vector)) >= 1.0-1e-6)
    {
        viennagrid::look_up(visited_container, element) = true;
        viennagrid::add_handle( view, hull_domain, element_handle );
        viennagrid::add_handle( view, hull_domain, viennagrid::lines(element).handle_at(0) );
        viennagrid::add_handle( view, hull_domain, viennagrid::lines(element).handle_at(1) );
        viennagrid::add_handle( view, hull_domain, viennagrid::lines(element).handle_at(2) );
        
        typedef typename viennagrid::result_of::neighbour_range<hull_domain_type, element_type>::type neighbour_range_type;
        typedef typename viennagrid::result_of::iterator<neighbour_range_type>::type neighbour_range_iterator;
        
        neighbour_range_type neighgbours = viennagrid::neighbour_elements< viennagrid::line_tag >( hull_domain, element_handle );
        for (neighbour_range_iterator it = neighgbours.begin(); it != neighgbours.end(); ++it)
            recursively_add_neighbours( hull_domain, it.handle(), normal_vector, visited_container, view );
    }
}




template<typename line_domain_type, typename vertex_handle_type, typename line_handle_type, typename point_type, typename visited_container_type>
vertex_handle_type get_endpoint( line_domain_type & line_domain, vertex_handle_type const & vertex_handle, line_handle_type const & line_handle, point_type direction, visited_container_type & visited_container )
{
    typedef typename viennagrid::result_of::coboundary_range<line_domain_type, viennagrid::line_tag>::type coboundary_range_type;
    typedef typename viennagrid::result_of::iterator<coboundary_range_type>::type coboundary_range_iterator;
    typedef typename viennagrid::storage::handle::value_type<line_handle_type>::type line_type;
    
    coboundary_range_type lines = viennagrid::create_coboundary_elements<viennagrid::line_tag>(line_domain, vertex_handle);
    if (lines.size() != 2)
    {
        return vertex_handle;
    }

    
    line_type & line = viennagrid::dereference_handle( line_domain, line_handle );
    
    line_handle_type other_line_handle;
    if (lines.handle_at(0) == line_handle)
        other_line_handle = lines.handle_at(1);
    else
        other_line_handle = lines.handle_at(0);
    
    
    line_type & other_line = viennagrid::dereference_handle( line_domain, other_line_handle );
    
    point_type other_direction = viennagrid::point( line_domain, viennagrid::vertices(other_line)[0] ) - viennagrid::point( line_domain, viennagrid::vertices(other_line)[1] );
    other_direction /= viennagrid::norm_2(other_direction);
    
    if ( std::abs(viennagrid::inner_prod(direction, other_direction)) >= 1.0-1e-6)
    {
        viennagrid::look_up( visited_container, viennagrid::dereference_handle(line_domain, other_line_handle) ) = true;
        
        vertex_handle_type other_vertex_handle;
        if ( viennagrid::vertices(other_line).handle_at(0) == vertex_handle )
            other_vertex_handle = viennagrid::vertices(other_line).handle_at(1);
        else
            other_vertex_handle = viennagrid::vertices(other_line).handle_at(0);
        
        return get_endpoint( line_domain, other_vertex_handle, other_line_handle, direction, visited_container );
    }
    
    return vertex_handle;
}




template<typename hull_domain_type, typename hull_segment_container_type, typename plc_domain_type>
void extract_plcs( hull_domain_type & hull_domain, hull_segment_container_type & hull_segments, plc_domain_type & plc_domain )
{
    typedef typename hull_segment_container_type::value_type hull_segment_type;
    typedef typename viennagrid::result_of::point_type<hull_domain_type>::type hull_point_type;
    typedef typename viennagrid::result_of::cell_type<hull_domain_type>::type hull_cell_type;
    typedef typename viennagrid::result_of::handle<hull_domain_type, hull_cell_type>::type hull_cell_handle_type;
    typedef typename viennagrid::result_of::cell_range<hull_domain_type>::type cell_range_type;
    typedef typename viennagrid::result_of::iterator<cell_range_type>::type cell_range_iterator;
    
    cell_range_type cells = viennagrid::elements( hull_domain );
    
    std::deque<bool> visited( cells.size(), false );
    
    typedef typename viennagrid::result_of::domain_view<hull_domain_type>::type hull_view_type;
    typedef typename viennagrid::result_of::vertex_handle<hull_view_type>::type hull_view_vertex_handle_type;
    typedef typename viennagrid::result_of::element_range<hull_view_type, viennagrid::line_tag>::type view_line_range_type;
    typedef typename viennagrid::result_of::iterator<view_line_range_type>::type view_line_range_iterator;

    std::deque<hull_view_type> plane_lines;
    
    hull_view_type all_lines = viennagrid::create_view<hull_view_type>( hull_domain );
    
    for (cell_range_iterator it = cells.begin(); it != cells.end(); ++it)
    {
        hull_cell_type const & cell = *it;
        
        if (viennagrid::look_up(visited, cell))
            continue;
        
        hull_point_type normal_vector = viennagrid::normal_vector( hull_domain, cell );
        normal_vector /= viennagrid::norm_2(normal_vector);
        
               
        hull_view_type current_plane = viennagrid::create_view<hull_view_type>( hull_domain );
        std::deque<hull_view_type> current_plane_view( hull_segments.size() );
        for (int i = 0; i < current_plane_view.size(); ++i)
            current_plane_view[i] = viennagrid::create_view<hull_view_type>( hull_domain );
        
        plane_lines.resize( plane_lines.size()+1 );
        hull_view_type & current_plane_lines = plane_lines.back();
        current_plane_lines = viennagrid::create_view<hull_view_type>( hull_domain );        

        
        recursively_add_neighbours( hull_domain, it.handle(), normal_vector, visited, current_plane );
        
        typedef typename viennagrid::result_of::cell_range<hull_view_type>::type view_cell_range_type;
        typedef typename viennagrid::result_of::iterator<view_cell_range_type>::type view_cell_range_iterator;
        
        view_cell_range_type plane_elements = viennagrid::elements( current_plane );
        for (view_cell_range_iterator it = plane_elements.begin(); it != plane_elements.end(); ++it)
        {
            for (int i = 0; i < current_plane_view.size(); ++i)
            {
                hull_segment_type & current_segment = hull_segments[i];
                view_cell_range_iterator jt = viennagrid::find_by_handle(current_segment, it.handle());
                if ( jt != viennagrid::cells(current_segment).end() )
                    viennagrid::add_handle( current_plane_view[i], hull_domain, it.handle() );
            }
            
            
        }
        
        view_line_range_type lines = viennagrid::elements( current_plane );
        
        for (int i = 0; i < current_plane_view.size(); ++i)
        {
            viennadata::access<viennagrid::boundary_key<hull_view_type>, bool>(viennagrid::boundary_key<hull_view_type>(current_plane_view[i]))(current_plane_view[i]) = false;
            viennadata::erase<viennadata::all, viennadata::all>()(current_plane_view[i]);
            for (view_line_range_iterator it = lines.begin(); it != lines.end(); ++it)
                viennadata::erase<viennadata::all, viennadata::all>()(*it);
        }
        
        for (view_line_range_iterator it = lines.begin(); it != lines.end(); ++it)
        {

            for (int i = 0; i < current_plane_view.size(); ++i)
            {
                hull_view_type & current_view = current_plane_view[i];
                
                if (viennagrid::is_boundary<viennagrid::triangle_tag>( *it, current_view ))
                {
                    viennagrid::add_handle( all_lines, hull_domain, it.handle() );
                    viennagrid::add_handle( current_plane_lines, hull_domain, it.handle() );
                    break;
                }
            }
        }
    }
    

    
    typedef typename viennagrid::result_of::line_handle<plc_domain_type>::type plc_line_handle_type;
    typedef typename viennagrid::result_of::vertex_handle<plc_domain_type>::type plc_vertex_handle_type;
    typedef typename viennagrid::result_of::point_type<plc_domain_type>::type plc_point_type;
    
    
    for (std::size_t i = 0; i != plane_lines.size(); ++i)
    {
        hull_view_type & current_plane_lines_view = plane_lines[i];
        
        view_line_range_type current_plane_lines = viennagrid::elements(current_plane_lines_view);
        std::deque<bool> lines_visited( current_plane_lines.size(), false );
        
        std::deque<plc_line_handle_type> plc_lines;
        
        for (view_line_range_iterator it = current_plane_lines.begin(); it != current_plane_lines.end(); ++it)
        {            
            if (viennagrid::look_up(lines_visited, *it) == true)
                continue;
            
            
            
            hull_point_type direction = viennagrid::point( all_lines, viennagrid::vertices(*it)[0] ) - viennagrid::point( all_lines, viennagrid::vertices(*it)[1] );
            direction /= viennagrid::norm_2(direction);
            
            hull_view_vertex_handle_type first = get_endpoint( all_lines, viennagrid::vertices(*it).handle_at(0), it.handle(), direction, lines_visited );
            hull_view_vertex_handle_type second = get_endpoint( all_lines, viennagrid::vertices(*it).handle_at(1), it.handle(), direction, lines_visited );
            
            viennagrid::look_up( lines_visited, *it ) = true;
            
            plc_lines.push_back(viennagrid::create_line(plc_domain,
                                        viennagrid::create_unique_vertex(plc_domain, viennagrid::point( all_lines, first )),
                                        viennagrid::create_unique_vertex(plc_domain, viennagrid::point( all_lines, second ))));
        }
        
        plc_vertex_handle_type tmp_vtx_handle;
        plc_point_type tmp_pt;
        viennagrid::create_plc(plc_domain, plc_lines.begin(), plc_lines.end(), &tmp_vtx_handle, &tmp_vtx_handle, &tmp_pt, &tmp_pt);
    }
}
    
}

#endif