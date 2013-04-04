#include <iostream>
#include <string.h>
#include <boost/concept_check.hpp>


#include "viennagrid/domain/config.hpp"
#include "viennagrid/algorithm/intersect.hpp"
#include "viennagrid/io/vtk_writer.hpp"

#include "viennamesh/algorithm/cgal_plc_mesher.hpp"
#include "viennamesh/base/segments.hpp"
#include "viennamesh/algorithm/vgmodeler_hull_adaption.hpp"
#include "viennamesh/algorithm/netgen_tetrahedron_mesher.hpp"


// template<typename point_type_>
// struct layer;


template<typename layer_type>
struct point_intersection_feedback
{
    point_intersection_feedback(layer_type & l_) : l(l_) {}
    
    template<typename point_type, typename line1_tag, typename line2_tag, typename coord_type>
    void operator() (point_type const & v0, point_type const & v1 , line1_tag tag1,
                        point_type const & w0, point_type const & w1, line2_tag tag2,
                        coord_type s, coord_type t, coord_type denominator)
    {
//         std::cout << s << " " << t << " " << denominator << std::endl;
        intersection_point = v0 + (v1-v0)*s/denominator;
//         std::cout << "single point intersection: " << intersection_point << std::endl;
        
        typename layer_type::index_type i = l.simple_create_point(intersection_point);
        
//         std::cout << "+  adding " << v0 << " - " << intersection_point << std::endl;
        l.simple_add_line( l.get_point(v0), i );
//         std::cout << "+  adding " << intersection_point << " - " << v1 << std::endl;
        l.simple_add_line( i, l.get_point(v1) );
        
//         std::cout << "+  adding " << w0 << " - " << intersection_point << std::endl;
        l.simple_add_line( l.get_point(w0), i );
//         std::cout << "+  adding " << intersection_point << " - " << w1 << std::endl;
        l.simple_add_line( i, l.get_point(w1) );
    }
    
    typename layer_type::point_type intersection_point;
    layer_type & l;
};

template<typename layer_type>
struct overlapping_lines_feedback
{
    overlapping_lines_feedback(layer_type & l_) : l(l_) {}
    
    template<typename point_type, typename line1_tag, typename line2_tag, typename coord_type>
    void operator() (point_type const & v0, point_type const & v1 , line1_tag tag1,
                     point_type const & w0, point_type const & w1, line2_tag tag2,
                     coord_type first1, coord_type second1,
                     coord_type first2, coord_type second2)
    {
//         std::cout << "This should not happen!!" << std::endl;
        
        l.simple_add_line( l.get_point(v0), l.get_point(v1) );
//         std::cout << "+ line overlapping, adding " << v0 << " - " << v1 << std::endl;
        
//         std::map<coord_type, point_type> points;
//         points[first1] = v0;
//         points[second1] = v1;
//         points[first2] = w0;
//         points[second2] = w1;
//        
//         
//         typename std::map<coord_type, point_type>::iterator it1 = points.begin();
//         typename std::map<coord_type, point_type>::iterator it2 = it1; ++it2;
//         
//         std::cout << "overlapping intersection: " << it1->second;
//         for (; it2 != points.end(); ++it1, ++it2)
//         {
//             std::cout << " - " << it2->second;
//             l.simple_add_line( it1->second, it2->second );
//         }
//         
//         std::cout << std::endl;
//             std::cout << it->second << std::endl;
        
        
    }
    
    layer_type & l;
};




template<typename point_type_, typename element_id_type_ = std::size_t>
struct layer
{
    typedef layer<point_type_> self_type;
    
    typedef std::size_t index_type;
    typedef element_id_type_ element_id_type;
    typedef point_type_ point_type;
    
    typedef typename viennagrid::result_of::coord_type<point_type>::type coord_type;
    
    typedef std::vector<point_type> point_container_type;
    
    typedef std::pair<index_type, index_type> line_type;
    typedef std::list<line_type> line_container_type;
    
    typedef std::vector<point_type> polygon_type;
    typedef std::list< std::pair<polygon_type, element_id_type> > polygon_container_type;
    
    
    point_type & get_point( index_type index ) { return points[index]; }
    point_type const & get_point( index_type index ) const { return points[index]; }
    
    index_type simple_create_point( point_type const & point )
    {
        points.push_back( point );
        return points.size()-1;
    }
    
    index_type create_point( point_type const & point )
    {
        index_type index = get_point(point);
        
        if (index != -1)
            return index;
        
        points.push_back( point );
        index = points.size()-1;
        
        for (typename line_container_type::iterator it = lines.begin(); it != lines.end();)
        {
            point_type const & l0 = get_point( it->first );
            point_type const & l1 = get_point( it->second );

                
            if (viennagrid::geometry::point_line_intersect( point, l0, l1, viennagrid::geometry::interval::open_open_tag(), 1e-6 ))
            {
//                 std::cout << "Splitting line [" << l0 << " - " << l1 << "] because of new point " << point << std::endl;
                simple_add_line( it->first, index );
                simple_add_line( index, it->second );

//                 std::cout << "-  erasing line at point creation [" << get_point(it->first) << " - " << get_point(it->second) << "]" << std::endl;
                it = lines.erase(it);
            }
            else
                ++it;
        }
        
        
        return index;
    }
    
    index_type get_point( point_type const & point ) const
    {
        for (index_type index = 0; index != points.size(); ++index)
        {
            if ( viennagrid::norm_2( point - get_point(index) ) < 1e-6 )
                return index;
        }
        
        return -1;
    }
    
    
    void delete_point( index_type index )
    {
        points.erase( points.begin() + index );
        for (typename line_container_type::iterator it = lines.begin(); it != lines.end();)
        {
            if ((it->first == index) || (it->second == index))
            {
                it = lines.erase(it);
                continue;
            }
            
            if (it->first > index) --(it->first);
            if (it->second > index) --(it->second);
            
            ++it;
        }
    }
    
    void delete_point( point_type const & point )
    {
        delete_point( get_point( point) ) ;
    }
    
    
    
    void simple_add_line( line_type const & line )
    {
        for (line_container_type::iterator it = lines.begin(); it != lines.end(); ++it)
        {
            if ((line == *it) || (line_type(line.second, line.first) == *it))
                return;
        }
        
//         std::cout << " simple add line: [" << get_point(line.first) << " - " << get_point(line.second) << "]" << std::endl;
        lines.push_front( line );
    }
    
    void simple_add_line( index_type p0, index_type p1 )
    {
//         std::cout << " simple add line: [" << p0 << " - " << p1 << "]" << std::endl;
        simple_add_line( line_type(p0, p1) );
    }
    
//     void simple_add_line( point_type const & p0, point_type const & p1 )
//     {
// //         std::cout << " simple add line: [" << p0 << " - " << p1 << "]" << std::endl;
//         simple_add_line( line_type( get_point(p0), get_point(p1) ) );
//     }
    
    
    std::map<coord_type, index_type> split_line( point_type const & p0, point_type const & p1 )
    {
        create_point(p0);
        create_point(p1);
        
        std::map<coord_type, index_type> splitted_points;
        point_type const & start = p0;
        point_type vector = p1 - p0;
        
        splitted_points.insert( std::make_pair( viennagrid::inner_prod(p0-start, vector), get_point(p0) ) );
        splitted_points.insert( std::make_pair( viennagrid::inner_prod(p1-start, vector), get_point(p1) ) );
        
        for (index_type it = 0; it != points.size(); ++it)
        {
            point_type const & cur = get_point(it);
//             std::cout << "  checking point " << cur << std::endl;
            if (viennagrid::geometry::point_line_intersect( cur, p0, p1, viennagrid::geometry::interval::open_open_tag(), 1e-6 ))
            {
                splitted_points.insert( std::make_pair( viennagrid::inner_prod(cur-start, vector), it ) );
//                 std::cout << "Inserting point " << cur << std::endl;
            }
        }
        
        for (typename line_container_type::iterator it = lines.begin(); it != lines.end();)
        {
            point_type const & l0 = get_point( it->first );
            point_type const & l1 = get_point( it->second );
            
//             bool intersection = false;
            typename std::map<coord_type, index_type>::iterator jt = splitted_points.begin();
            for (; jt != splitted_points.end(); ++jt)
            {
                point_type const & cur = get_point(jt->second);
                
                if (viennagrid::geometry::point_line_intersect( cur, l0, l1, viennagrid::geometry::interval::open_open_tag(), 1e-6 ))
                {
//                     std::cout << "Line [" << p0 << " - " << p1 << "]  has intersection with line [" << l0 << " - " << l1 << "]" << std::endl;
                    
                    
                    splitted_points.insert( std::make_pair( viennagrid::inner_prod(l0-start, vector), it->first ) );
                    splitted_points.insert( std::make_pair( viennagrid::inner_prod(l1-start, vector), it->second ) );
    //                 std::cout << "Inserting point " << cur << std::endl;
//                     intersection = true;
                    break;
                }
            }
            
            if (jt != splitted_points.end())
            {
//                 std::cout << "-  erasing line at split line [" << get_point(it->first) << " - " << get_point(it->second) << "]" << std::endl;
                it = lines.erase(it);
            }
            else
                ++it;
        }
        
//         std::cout << "# Splitting line: " << p0 << " - " << p1 << std::endl;
        
//         typename std::map<coord_type, index_type>::iterator lit1 =  splitted_points.begin();
//         typename std::map<coord_type, index_type>::iterator lit2 = lit1; ++lit2;
//         for (; lit2 != splitted_points.end(); ++lit1, ++lit2)
//             std::cout << "#   line " << get_point(lit1->second) << " - " << get_point(lit2->second) << std::endl;
        
        return splitted_points;
    }
    
    
    void add_line( point_type const & line_point0, point_type const & line_point1  )
    {        
        std::map<coord_type, index_type> points = split_line(line_point0, line_point1);
        
        typename std::map<coord_type, index_type>::iterator lit1 =  points.begin();
        typename std::map<coord_type, index_type>::iterator lit2 = lit1; ++lit2;
        
        
        for (; lit2 != points.end(); ++lit1, ++lit2)
        {
            point_type const & lp0 = get_point(lit1->second);
            point_type const & lp1 = get_point(lit2->second);
            
//             std::cout << "  Checking line " << lp0 << " - " << lp1 << std::endl;
            
            bool intersect = false;
            
            line_container_type::iterator it = lines.begin();
            for (; it != lines.end();)
            {
                if ((line_type(lit1->second, lit2->second) == *it) || (line_type(lit2->second, lit1->second) == *it))
                    break;
                
                
                point_type const & p0 = get_point(it->first);
                point_type const & p1 = get_point(it->second);
                
                point_intersection_feedback<self_type> pifb(*this);
                overlapping_lines_feedback<self_type> olfb(*this);
                
                viennagrid::geometry::intersection_result current_intersect = viennagrid::geometry::line_line_intersect(
                                                                lp0, lp1, viennagrid::geometry::interval::open_open_tag(),
                                                                p0, p1, viennagrid::geometry::interval::open_open_tag(),
                                                                1e-6,
                                                                pifb, olfb);
                
                if (current_intersect)
                {
                    intersect = true;
//                     std::cout << "-  erasing line at add line [" << get_point(it->first) << " - " << get_point(it->second) << "]" << std::endl;
                    it = lines.erase(it);
                }
                else
                    ++it;
            }
            
            if (!intersect && (it == lines.end()))
            {
//                 std::cout << "Adding because no intersection and new unique line" << std::endl;
                simple_add_line( line_type(lit1->second, lit2->second)  );
            }
        }
    }
    
    
    
    
    template<typename point_iterator_type>
    void add_polygon( point_iterator_type const & start, point_iterator_type const & end, element_id_type id )
    {
        point_iterator_type it1 = start;
        point_iterator_type it2 = it1; ++it2;
        
        for ( ; it2 != end; ++it1, ++it2 )
            add_line(*it1, *it2);
        add_line(*it1, *start);
        
        std::vector<point_type> polygon;
        for (it1 = start; it1 != end; ++it1)
            polygon.push_back( *it1 );
        polygons.push_front( std::make_pair(polygon, id) );
        
        for (unsigned int i = 0; i < points.size();)
        {
            point_type const & cur = points[i];
            bool inside = viennagrid::geometry::is_inside( polygon.begin(), polygon.end(), viennagrid::geometry::interval::open_tag(), cur, 1e-6 );
            
            if (inside)
                delete_point( cur );
            else
                ++i;
        }
    }
    
    
    void add_rectangle( point_type const & line_point0, point_type const & line_point1, element_id_type id )
    {
        std::vector<point_type> points;
        
        points.push_back( point_type( std::min(line_point0[0], line_point1[0]), std::min(line_point0[1], line_point1[1]) ) );
        points.push_back( point_type( std::max(line_point0[0], line_point1[0]), std::min(line_point0[1], line_point1[1]) ) );
        points.push_back( point_type( std::max(line_point0[0], line_point1[0]), std::max(line_point0[1], line_point1[1]) ) );
        points.push_back( point_type( std::min(line_point0[0], line_point1[0]), std::max(line_point0[1], line_point1[1]) ) );
        
        add_polygon(points.begin(), points.end(), id);
    }
    

    
    void add_circle( point_type const & center, coord_type radius, unsigned int num_points, element_id_type id )
    {
        std::vector<point_type> points;
        
        coord_type step = 2*M_PI / static_cast<coord_type>(num_points);
        for (unsigned int i = 0; i != num_points; ++i)
        {
            point_type point = center;
            point[0] += std::cos(step*i) * radius;
            point[1] += std::sin(step*i) * radius;
            
            points.push_back( point );
        }
        
        add_polygon(points.begin(), points.end(), id);
    }
    
    template<typename layer_type>
    void add_layer( layer_type const & other )
    {
        for (typename layer_type::line_container_type::const_iterator it = other.lines.begin(); it != other.lines.end(); ++it)
        {
            add_line( other.get_point(it->first), other.get_point(it->second) );
        }
    }
    
    
    
    element_id_type get_element_id( point_type const & point ) const
    {
        for (typename polygon_container_type::const_iterator it = polygons.begin(); it != polygons.end(); ++it)
        {
            if (viennagrid::geometry::is_inside( it->first.begin(), it->first.end(), viennagrid::geometry::interval::open_tag(), point, 1e-6 ))
                return it->second;
        }
    }
    
//     void add_line( point_type const & p0, point_type const & p1 )
//     {
//         add_line( line_type( create_point(p0), create_point(p1) ) );
//     }
    
    void print_lines() const
    {
        std::cout << "Lines of Layer" << std::endl;
        for (line_container_type::const_iterator it = lines.begin(); it != lines.end(); ++it)
        {
            std::cout << "  " << get_point(it->first) << " - " << get_point(it->second) << std::endl;
        }
    }
    
    void to_viennagrid_domain( viennagrid::config::line_2d_domain & vgrid_domain ) const
    {
        typedef viennagrid::result_of::element_hook<viennagrid::config::line_2d_domain, viennagrid::vertex_tag>::type vertex_hook_type;
        
        std::map<index_type, vertex_hook_type> vertex_hook_map;
        
        for (unsigned int it = 0; it != points.size(); ++it)
            vertex_hook_map[it] = viennagrid::create_vertex( vgrid_domain, get_point(it) );
        
        for (typename line_container_type::const_iterator it = lines.begin(); it != lines.end(); ++it)
            viennagrid::create_line( vgrid_domain, vertex_hook_map[it->first], vertex_hook_map[it->second] );
    }
    
    
    
    point_container_type points;
    
    line_container_type lines;
    polygon_container_type polygons;
};







template<typename domain_type, typename point_type, typename coord_type>
typename viennagrid::result_of::element_hook<domain_type, viennagrid::vertex_tag>::type get_create_vertex( domain_type & domain, point_type const & point, coord_type eps = 1e-6 )
{
    typedef typename viennagrid::result_of::element_range<domain_type, viennagrid::vertex_tag>::type vertex_range_type;
    typedef typename viennagrid::result_of::hook_iterator<vertex_range_type>::type vertex_range_hook_iterator;
    
    vertex_range_type vertices = viennagrid::elements<viennagrid::vertex_tag>(domain);
    for (vertex_range_hook_iterator it = vertices.hook_begin(); it != vertices.hook_end(); ++it)
    {
        if (viennagrid::norm_2( point - viennagrid::point(domain, *it) ) < eps)
            return *it;
    }
    
    return viennagrid::create_vertex( domain, point );
}





int main()
{
    typedef viennagrid::config::point_type_2d point_type_2d;
    
    
    std::vector< std::pair<layer<point_type_2d>, double> > layers(3);
    
    layers[2].first.add_rectangle( point_type_2d(1,1), point_type_2d(7, 8), 0 );
    layers[2].first.add_rectangle( point_type_2d(3,3), point_type_2d(4, 4), 1 );
    layers[2].first.add_rectangle( point_type_2d(5,3), point_type_2d(6, 4), 2 );
    layers[2].second = 7.0;
    
    layers[1].first.add_rectangle( point_type_2d(0,0), point_type_2d(10, 10), 3 );
    layers[1].first.add_circle( point_type_2d(9,9), 3.0, 16, 1 );
    layers[1].second = 5.0;
    
    layers[0].first.add_rectangle( point_type_2d(-5,-5), point_type_2d(15, 15), 4 );
    layers[0].second = 10.0;

    
//     l.print_lines();
    
    
//     viennagrid::config::line_2d_domain line_domain;
//     
//     l.to_viennagrid_domain( line_domain );
//     
//     {
//         viennagrid::io::vtk_writer<viennagrid::config::line_2d_domain, viennagrid::config::line_2d_cell> vtk_writer;
//         vtk_writer(line_domain, "layer_lines");
//     }
//     
//     
//     
//     viennagrid::config::triangular_2d_domain triangle_domain_2d;
//     
//     viennamesh::result_of::settings<viennamesh::cgal_plc_2d_mesher_tag>::type settings;
//     
//     viennamesh::run_algo< viennamesh::cgal_plc_2d_mesher_tag >( line_domain, triangle_domain_2d, settings );
//     
//     {
//         viennagrid::io::vtk_writer<viennagrid::config::triangular_2d_domain, viennagrid::config::triangular_2d_cell> vtk_writer;
//         vtk_writer(triangle_domain_2d, "layer_triangulated");
//     }
    
    
    
    typedef layer<point_type_2d> layer_type;
    
    viennagrid::config::plc_3d_domain plc_domain;
    typedef viennagrid::config::point_type_3d point_type_3d;
    
    typedef viennagrid::result_of::element<viennagrid::config::plc_3d_domain, viennagrid::plc_tag>::type plc_type;
    typedef viennagrid::result_of::element_hook<viennagrid::config::plc_3d_domain, viennagrid::vertex_tag>::type vertex_hook_type;
    typedef viennagrid::result_of::element_hook<viennagrid::config::plc_3d_domain, viennagrid::line_tag>::type line_hook_type;
    typedef viennagrid::result_of::element_hook<viennagrid::config::plc_3d_domain, viennagrid::polygon_tag>::type polygon_hook_type;
    
    
    double cur_z = 0.0;
    std::vector< std::pair<layer_type, double> > interfaces;
    interfaces.push_back( std::make_pair(layers.begin()->first, cur_z) );
    
    std::vector< std::pair<layer_type, double> >::iterator lit1 = layers.begin();
    std::vector< std::pair<layer_type, double> >::iterator lit2 = lit1; ++lit2;
    for (; lit2 != layers.end(); ++lit1, ++lit2)
    {
        cur_z += lit1->second;
        layer_type tmp = lit1->first;
        tmp.add_layer(lit2->first);
        interfaces.push_back( std::make_pair(tmp, cur_z) );
    }
    
    cur_z += lit1->second;
    interfaces.push_back( std::make_pair(lit1->first, cur_z) );
    
    
    std::vector< std::map<layer_type::index_type, vertex_hook_type> > interface_vertex_hook_map( interfaces.size() );
    std::vector< std::map<layer_type::line_type, line_hook_type> > interface_line_hook_map( interfaces.size() );
    
    
    {
        unsigned int interface_index = 0;
        for (std::vector< std::pair<layer_type, double> >::iterator iit = interfaces.begin(); iit != interfaces.end(); ++iit, ++interface_index)
        {
            double height = iit->second;
            layer_type const & current_layer = iit->first;
            
            std::map<layer_type::index_type, vertex_hook_type> & vertex_hook_map = interface_vertex_hook_map[interface_index];
            std::map<layer_type::line_type, line_hook_type> & line_hook_map = interface_line_hook_map[interface_index];
            
            for (layer_type::index_type pit = 0; pit != current_layer.points.size(); ++pit)
                vertex_hook_map[pit] = viennagrid::create_vertex( plc_domain, point_type_3d( current_layer.points[pit][0], current_layer.points[pit][1], height ) );
            
            std::vector<line_hook_type> plc_lines;
            
            for (layer_type::line_container_type::const_iterator lit = current_layer.lines.begin(); lit != current_layer.lines.end(); ++lit)
            {
                line_hook_type line_hook = viennagrid::create_line( plc_domain, vertex_hook_map[lit->first], vertex_hook_map[lit->second] );
                line_hook_map[*lit] = line_hook;
                plc_lines.push_back( line_hook );
            }
            
            polygon_hook_type poly_hook;
            vertex_hook_type vtx_hook;
            point_type_3d pt;
            
            viennagrid::create_element<plc_type>( plc_domain, &poly_hook, &poly_hook, plc_lines.begin(), plc_lines.end(), &vtx_hook, &vtx_hook, &pt, &pt );
        }
    }
    
    
    
    
    {
        unsigned int interface_index = 0;
        std::vector< std::pair<layer_type, double> >::iterator iit1 = interfaces.begin();
        std::vector< std::pair<layer_type, double> >::iterator iit2 = iit1; ++iit2;
        
        for (std::vector< std::pair<layer_type, double> >::iterator lit = layers.begin(); lit != layers.end(); ++lit, ++iit1, ++iit2, ++interface_index)
        {
            layer_type const & current_layer = lit->first;
            layer_type const & lower_interface = iit1->first;
            layer_type const & upper_interface = iit2->first;
            
            std::map<layer_type::index_type, line_hook_type> z_line_hook_map;
            
            std::map<layer_type::index_type, vertex_hook_type> & lower_vertex_hook_map = interface_vertex_hook_map[interface_index];
            std::map<layer_type::line_type, line_hook_type> & lower_line_hook_map = interface_line_hook_map[interface_index];

            std::map<layer_type::index_type, vertex_hook_type> & upper_vertex_hook_map = interface_vertex_hook_map[interface_index+1];
            std::map<layer_type::line_type, line_hook_type> & upper_line_hook_map = interface_line_hook_map[interface_index+1];

            
            for (layer_type::index_type pit = 0; pit != current_layer.points.size(); ++pit)
                z_line_hook_map[pit] = viennagrid::create_line( plc_domain,
                                                                lower_vertex_hook_map[lower_interface.get_point(current_layer.get_point(pit))],
                                                                upper_vertex_hook_map[upper_interface.get_point(current_layer.get_point(pit))] );
            
            
            
            
            layer_type::line_container_type tmp_lines = current_layer.lines;
            for (layer_type::line_container_type::iterator it = tmp_lines.begin(); it != tmp_lines.end();)
            {
                std::vector<line_hook_type> plc_lines;
                
                layer_type::line_type const & line = *it;
                
                layer_type::point_type const & p0 = current_layer.get_point(line.first);
                layer_type::point_type const & p1 = current_layer.get_point(line.second);
                
                for (layer_type::line_container_type::const_iterator jt = lower_interface.lines.begin() ; jt != lower_interface.lines.end(); ++jt)
                {
                    if (viennagrid::geometry::point_ray_intersect( lower_interface.get_point(jt->first), p0, p1, 1e-6 ) &&
                        viennagrid::geometry::point_ray_intersect( lower_interface.get_point(jt->second), p0, p1, 1e-6 ))
                        plc_lines.push_back( lower_line_hook_map[*jt] );
                }
                
                for (layer_type::line_container_type::const_iterator jt = upper_interface.lines.begin() ; jt != upper_interface.lines.end(); ++jt)
                {
                    if (viennagrid::geometry::point_ray_intersect( upper_interface.get_point(jt->first), p0, p1, 1e-6 ) &&
                        viennagrid::geometry::point_ray_intersect( upper_interface.get_point(jt->second), p0, p1, 1e-6 ))
                        plc_lines.push_back( upper_line_hook_map[*jt] );
                }
                
                
                for (layer_type::index_type jt = 0; jt != current_layer.points.size(); ++jt)
                {
                    if (viennagrid::geometry::point_ray_intersect( current_layer.get_point(jt), current_layer.get_point(line.first), current_layer.get_point(line.second), 1e-6 ))
                        plc_lines.push_back( z_line_hook_map[jt] );
                }
                
                
                polygon_hook_type poly_hook;
                vertex_hook_type vtx_hook;
                point_type_3d pt;
                
                viennagrid::create_element<plc_type>( plc_domain, &poly_hook, &poly_hook, plc_lines.begin(), plc_lines.end(), &vtx_hook, &vtx_hook, &pt, &pt );
                
                layer_type::line_container_type::iterator jt = it; ++jt;
                for (; jt != tmp_lines.end();)
                {
                    if (viennagrid::geometry::point_ray_intersect( current_layer.get_point(jt->first), p0, p1, 1e-6 ) &&
                        viennagrid::geometry::point_ray_intersect( current_layer.get_point(jt->second), p0, p1, 1e-6 ))
                        jt = tmp_lines.erase(jt);
                    else
                        ++jt;
                }
                
                it = tmp_lines.erase(it);
            }
        }
    }
    

    viennagrid::config::triangular_3d_domain triangle_domain_3d;
    
    viennamesh::result_of::settings<viennamesh::cgal_plc_3d_mesher_tag>::type settings;
    
    viennamesh::run_algo< viennamesh::cgal_plc_3d_mesher_tag >( plc_domain, triangle_domain_3d, settings );
 
    {
        viennagrid::io::vtk_writer<viennagrid::config::triangular_3d_domain, viennagrid::config::triangular_3d_cell> vtk_writer;
        vtk_writer(triangle_domain_3d, "layer_triangulated_3d");
    }
    
    
    
    {
        double cur_z = 0.0;
        unsigned int interface_index;
        for (std::vector< std::pair<layer_type, double> >::iterator lit = layers.begin(); lit != layers.end(); ++lit, ++interface_index)
        {
            layer_type const & current_laver = lit->first;
            double height = lit->second;
            double middle = cur_z + height / 2.0;
            
            viennagrid::config::line_2d_domain line_domain_2d;
            typedef typename viennagrid::result_of::element_hook<viennagrid::config::line_2d_domain, viennagrid::vertex_tag>::type vertex_hook_type;
            
            std::map<layer_type::index_type, vertex_hook_type> vertex_hook_map;
            
            for (layer_type::index_type it = 0; it != current_laver.points.size(); ++it)
                vertex_hook_map[it] = viennagrid::create_vertex( line_domain_2d, current_laver.points[it] );
            
            for (layer_type::line_container_type::const_iterator it = current_laver.lines.begin(); it != current_laver.lines.end(); ++it)
                viennagrid::create_line( line_domain_2d, vertex_hook_map[it->first], vertex_hook_map[it->second] );
            
            
            viennagrid::config::triangular_2d_domain triangular_domain_2d;
            viennamesh::result_of::settings<viennamesh::cgal_plc_2d_mesher_tag>::type settings;
    
            viennamesh::run_algo< viennamesh::cgal_plc_2d_mesher_tag >( line_domain_2d, triangular_domain_2d, settings );
            
            typedef typename viennagrid::result_of::point_type<viennagrid::config::triangular_2d_domain>::type point_type_2d;
            typedef typename viennagrid::result_of::element_range<viennagrid::config::triangular_2d_domain, viennagrid::triangle_tag>::type triangle_range_type;
            typedef typename viennagrid::result_of::iterator<triangle_range_type>::type triangle_range_iterator;
            
            triangle_range_type triangles = viennagrid::elements<viennagrid::triangle_tag>( triangular_domain_2d );
            for (triangle_range_iterator it = triangles.begin(); it != triangles.end(); ++it)
            {
                point_type_2d const & p0 = viennagrid::point( triangular_domain_2d, viennagrid::elements<viennagrid::vertex_tag>(*it)[0] );
                point_type_2d const & p1 = viennagrid::point( triangular_domain_2d, viennagrid::elements<viennagrid::vertex_tag>(*it)[1] );
                point_type_2d const & p2 = viennagrid::point( triangular_domain_2d, viennagrid::elements<viennagrid::vertex_tag>(*it)[2] );
                
                point_type_2d center = (p0+p1+p2) / 3.0;
                point_type_3d center_3d( center[0], center[1], middle );
                
                layer_type::element_id_type segment_id = current_laver.get_element_id( center );
                
                viennamesh::add_segment_seed_point( triangle_domain_3d, segment_id, center_3d );
                
//                 std::cout << "Adding seed point " << center_3d << " for Segment " << segment_id << std::endl;
                
//                 viennamesh::detect_and_mark_face_segment( triangle_domain_3d, center_3d, segment_id );
            }
            
            
            cur_z += height;
        }
        
        
        viennamesh::mark_face_segments( triangle_domain_3d );
        
    }
    
    
    
    

    viennagrid::config::triangular_3d_domain hull_domain;
    viennamesh::result_of::settings<viennamesh::vgmodeler_hull_adaption_tag>::type vgm_settings;
    
    vgm_settings.cell_size = 1.0;
    
    viennamesh::run_algo< viennamesh::vgmodeler_hull_adaption_tag >( triangle_domain_3d, hull_domain, vgm_settings );
    
    
    {        
        viennagrid::io::vtk_writer<viennagrid::config::triangular_3d_domain, viennagrid::config::triangular_3d_cell> vtk_writer;
        vtk_writer(hull_domain, "layer_adapted_hull_3d");
    }
    
    
    viennagrid::config::tetrahedral_3d_domain tetrahedron_domain;
    viennamesh::result_of::settings<viennamesh::netgen_tetrahedron_tag>::type netgen_settings;
    
    netgen_settings.cell_size = 1.0;
    
    viennamesh::run_algo< viennamesh::netgen_tetrahedron_tag >( hull_domain, tetrahedron_domain, netgen_settings );
    
    
    {        
        viennagrid::io::vtk_writer<viennagrid::config::tetrahedral_3d_domain, viennagrid::config::tetrahedral_3d_cell> vtk_writer;
        viennagrid::io::add_scalar_data_on_cells<viennamesh::unique_segment_tag, viennamesh::segment_id_type>(vtk_writer, "segment_id");
        vtk_writer(tetrahedron_domain, "netgen_volume");
    }

    
    return 0;
}
