#include <iostream>
#include <string.h>


#include "viennagrid/domain/config.hpp"
#include "viennagrid/algorithm/intersect.hpp"
#include "viennagrid/io/vtk_writer.hpp"

#include "viennamesh/algorithm/cgal_plc_mesher.hpp"
#include "viennamesh/base/segments.hpp"
#include "viennamesh/algorithm/vgmodeler_hull_adaption.hpp"
#include "viennamesh/algorithm/netgen_tetrahedron_mesher.hpp"


// template<typename point_type_>
// struct layer;




template<typename numeric_type>
struct cut_ray_feedback
{
    cut_ray_feedback( numeric_type & min_s_, numeric_type & max_s_) : min_s(min_s_), max_s(max_s_) {}
    
    template<typename point_type, typename line1_tag, typename line2_tag, typename coord_type>
    void operator() (point_type const & v0, point_type const & v1 , line1_tag tag1,
                        point_type const & w0, point_type const & w1, line2_tag tag2,
                        coord_type s, coord_type t, coord_type denominator)
    {
//         std::cout << "|| " << min_s << " " << max_s << " " << s/denominator << std::endl;
        
        min_s = std::min(min_s, s/denominator);
        max_s = std::max(max_s, s/denominator);
    }
    
    numeric_type & min_s;
    numeric_type & max_s;
};





template<typename layer_type>
struct point_intersection_feedback
{
    point_intersection_feedback(layer_type & l_, typename layer_type::line_container_type::iterator current_line_iterator_) :
        layer(l_), current_line_iterator(current_line_iterator_) {}
    
    template<typename point_type, typename line1_tag, typename line2_tag, typename coord_type>
    void operator() (point_type const & v0, point_type const & v1 , line1_tag tag1,
                        point_type const & w0, point_type const & w1, line2_tag tag2,
                        coord_type s, coord_type t, coord_type denominator)
    {
//         std::cout << "Point intersection: " << std::endl;
        layer.erase_line(current_line_iterator);
        
        typename layer_type::point_type intersection_point = v0 + (v1-v0)*s/denominator;
        
//         std::cout << "    " << intersection_point << std::endl;
        
        typename layer_type::index_type i = layer.simple_create_point(intersection_point);
        
//         std::cout << "+  adding " << v0 << " - " << intersection_point << std::endl;
        layer.add_line( v0, intersection_point );
//         std::cout << "+  adding " << intersection_point << " - " << v1 << std::endl;
        layer.add_line( intersection_point, v1 );
        
//         std::cout << "+  adding " << w0 << " - " << intersection_point << std::endl;
        layer.simple_add_line( layer.get_point(w0), i );
//         std::cout << "+  adding " << intersection_point << " - " << w1 << std::endl;
        layer.simple_add_line( i, layer.get_point(w1) );
    }
    
    layer_type & layer;
    typename layer_type::line_container_type::iterator current_line_iterator;
};

template<typename layer_type>
struct overlapping_lines_feedback
{
    overlapping_lines_feedback(layer_type & l_, typename layer_type::line_container_type::iterator current_line_iterator_) :
        layer(l_), current_line_iterator(current_line_iterator_) {}
    
    template<typename point_type, typename line1_tag, typename line2_tag, typename coord_type>
    void operator() (point_type const & v0, point_type const & v1, line1_tag tag1,
                     point_type const & w0, point_type const & w1, line2_tag tag2,
                     coord_type first1, coord_type second1,
                     coord_type first2, coord_type second2)
    {
//         std::cout << "overlapping intersection: " << std::endl;
        layer.erase_line(current_line_iterator);
        
        std::map<coord_type, point_type> points;
        points[first1] = v0;
        points[second1] = v1;
        points[first2] = w0;
        points[second2] = w1;

        {
            typename std::map<coord_type, point_type>::iterator it1 = points.begin();
            typename std::map<coord_type, point_type>::iterator it2 = it1; ++it2;
            
            for (; it2 != points.end(); ++it1, ++it2)
                layer.add_line( it1->second, it2->second );
        }   
    }
    
    layer_type & layer;
    typename layer_type::line_container_type::iterator current_line_iterator;
};




template<typename point_type_, typename element_id_type_ = std::size_t>
struct layer
{
    typedef layer<point_type_> self_type;
    
    typedef std::size_t index_type;
    typedef element_id_type_ element_id_type;
    typedef point_type_ point_type;
    
    typedef typename viennagrid::result_of::coord_type<point_type>::type coord_type;
    
    typedef std::deque<point_type> point_container_type;
    
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
        
//         std::cout << "CREATING POINT " << point << std::endl;
        
        points.push_back( point );
        index = points.size()-1;
        
        for (typename line_container_type::iterator it = lines.begin(); it != lines.end();)
        {
            point_type const & l0 = get_point( it->first );
            point_type const & l1 = get_point( it->second );
            
//             std::cout << "  checking line [" << l0 << " - " << l1 << "]" << std::endl;
                
            if (viennagrid::geometry::point_line_intersect( point, l0, l1, viennagrid::geometry::interval::open_open_tag(), 1e-6 ))
            {
//                 std::cout << "  !! Splitting line [" << l0 << " - " << l1 << "]" << std::endl;
                simple_add_line( it->first, index );
                simple_add_line( index, it->second );

//                 std::cout << "-  erasing line at point creation [" << get_point(it->first) << " - " << get_point(it->second) << "]" << std::endl;
                it = erase_line(it);
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
                it = erase_line(it);
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
        
        if (line.first == line.second)
        {
//             std::cout << "  simple add line - trying to insert a point line: " << get_point(line.first) << std::endl;
            return;
        }
        
//         std::cout << " + simple add line: [" << get_point(line.first) << " - " << get_point(line.second) << "]" << std::endl;
        lines.push_front( line );
    }
    
    void simple_add_line( index_type p0, index_type p1 )
    {
        simple_add_line( line_type(p0, p1) );
    }
    
    typename line_container_type::iterator erase_line(typename line_container_type::iterator to_erase)
    {
//         std::cout << " - delete line " << get_point(to_erase->first) << " - " << get_point(to_erase->second) << std::endl;
        return lines.erase(to_erase);
    }
    
    void add_cut_line( point_type const & first, point_type const & second )
    {
        coord_type min_s = 1;
        coord_type max_s = 0;
        cut_ray_feedback<coord_type> crfb(min_s, max_s);
        
        for (line_container_type::iterator it = lines.begin(); it != lines.end(); ++it)
        {
            point_type const & p0 = get_point(it->first);
            point_type const & p1 = get_point(it->second);
            
            viennagrid::geometry::line_line_intersect(first, second, viennagrid::geometry::interval::closed_closed_tag(),
                                                      p0, p1, viennagrid::geometry::interval::closed_closed_tag(),
                                                      1e-6,
                                                      crfb,
                                                      viennagrid::geometry::trivial_line_line_intersect_overlapping_lines_functor() );
        }
        
//         std::cout << " cut line : " << first << " - " << second << std::endl;
//         std::cout << "   actual cut line : " << first + (second-first) * min_s << " - " << first + (second-first) * max_s  << std::endl;
        
        if ((min_s == max_s) || ((min_s == 1) && (max_s == 0)))
            return;
        
        add_line( first + (second-first) * min_s, first + (second-first) * max_s );
    }
    
    void add_cut_parallel_x( coord_type y )
    {
        add_cut_line( point_type(min_x(), y), point_type(max_x(), y) );
    }
    
    void add_cut_parallel_y( coord_type x )
    {
        add_cut_line( point_type(x, min_y()), point_type(x, max_y()) );
    }
    
    
    coord_type min_x() const
    {
        if (points.empty()) return 0.0;
        coord_type x = (*points.begin())[0];
        typename point_container_type::const_iterator it = points.begin(); ++it;
        for ( ; it != points.end(); ++it)
            x = std::min(x, (*it)[0]);
        return x;
    }
    
    coord_type max_x() const
    {
        if (points.empty()) return 0.0;
        coord_type x = (*points.begin())[0];
        typename point_container_type::const_iterator it = points.begin(); ++it;
        for ( ; it != points.end(); ++it)
            x = std::max(x, (*it)[0]);
        return x;
    }
    
    coord_type min_y() const
    {
        if (points.empty()) return 0.0;
        coord_type y = (*points.begin())[1];
        typename point_container_type::const_iterator it = points.begin(); ++it;
        for ( ; it != points.end(); ++it)
            y = std::min(y, (*it)[1]);
        return y;
    }
    
    coord_type max_y() const
    {
        if (points.empty()) return 0.0;
        coord_type y = (*points.begin())[1];
        typename point_container_type::const_iterator it = points.begin(); ++it;
        for ( ; it != points.end(); ++it)
            y = std::max(y, (*it)[1]);
        return y;
    }
    
    
    bool intersect( point_type const & l0, point_type const & l1 ) const
    {
        line_container_type::iterator it = lines.begin();
        for (; it != lines.end(); ++it)
        {
            point_type const & p0 = get_point(it->first);
            point_type const & p1 = get_point(it->second);
            
            point_intersection_feedback<self_type> pifb(*this, it);
            overlapping_lines_feedback<self_type> olfb(*this, it);
            
            if( viennagrid::geometry::line_line_intersect(l0, l1, viennagrid::geometry::interval::open_open_tag(),
                                                          p0, p1, viennagrid::geometry::interval::open_open_tag(),
                                                          1e-6) )
            {
                return true;
            }
        }
        
        return false;
    }
    
    void add_line( point_type const & to_insert0, point_type const & to_insert1  )
    {                    
//             std::cout << "  Add line " << to_insert0 << " - " << to_insert1 << std::endl;
        if (viennagrid::norm_2( to_insert1 - to_insert0 ) < 1e-6)
        {
//                 std::cout << "  Trying to insert same line: " << to_insert0 << std::endl;
            return ;
        }
    
        index_type to_insert_index0 = create_point(to_insert0);
        index_type to_insert_index1 = create_point(to_insert1);
        
        bool intersect = false;
        
        line_container_type::iterator it = lines.begin();
        for (; it != lines.end(); ++it)
        {
//                 if ((line_type(to_insert_index0, to_insert_index1) == *it) || (line_type(to_insert_index1, to_insert_index0) == *it))
//                     break;
            
            
            point_type const & p0 = get_point(it->first);
            point_type const & p1 = get_point(it->second);
            
            point_intersection_feedback<self_type> pifb(*this, it);
            overlapping_lines_feedback<self_type> olfb(*this, it);
            
            viennagrid::geometry::intersection_result current_intersect = viennagrid::geometry::line_line_intersect(
                                                            to_insert0, to_insert1, viennagrid::geometry::interval::open_open_tag(),
                                                            p0, p1, viennagrid::geometry::interval::open_open_tag(),
                                                            1e-6,
                                                            pifb, olfb);
            
            if (current_intersect)
                return;
        }
            
        simple_add_line( to_insert_index0, to_insert_index1  );
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
    
    
    void check() const
    {
        for (typename line_container_type::const_iterator it = lines.begin(); it != lines.end(); ++it)
        {
            typename line_container_type::const_iterator jt = it; ++jt;
            for (; jt != lines.end(); ++jt)
            {
                if (viennagrid::geometry::line_line_intersect(get_point(it->first), get_point(it->second), viennagrid::geometry::interval::open_open_tag(),
                                                              get_point(jt->first), get_point(jt->second), viennagrid::geometry::interval::open_open_tag(),
                                                              1e-6))
                {
                    std::cout << "ERRRRRRORRRR!!!" << std::endl;
                    std::cout << "  line [ " << get_point(it->first) << " - " << get_point(it->second) << " ]" << std::endl;
                    std::cout << "  line [ " << get_point(jt->first) << " - " << get_point(jt->second) << " ]" << std::endl;
                }
            }
            
        }
        
        std::cout << std::endl;
        
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




template<typename numeric_type>
struct layer_interface
{
    
    static numeric_type interpolate( std::map< numeric_type, numeric_type > const & values, numeric_type position )
    {
        typename std::map< numeric_type, numeric_type >::const_iterator it = values.lower_bound( position );
        
        // exact match
        if ( it->first == position )
            return it->second;
        
        // value to interpolate is smaller than first element -> taking the first value
        if ( it == values.begin() )
            return it->second;
        
        // value to interpolate is greater then the last element -> taking the last value
        if ( it == values.end() )
        {
            --it;
            return it->second;
        }
            
        // linear interpolation
        typename std::map< numeric_type, numeric_type >::const_iterator prev = it; --prev;
        
        numeric_type length = it->first - prev->first;
        return it->second * (position - prev->first) / length + prev->second * (it->first - position) / length;
    }
    
    
    template<typename layer_type>
    void planar_from_layer( layer_type const & layer, double z_pos )
    {
        if (layer.points.empty()) return;
        
        double min_x = (*layer.points.begin())[0];
        double max_x = (*layer.points.begin())[0];
        
        double min_y = (*layer.points.begin())[1];
        double max_y = (*layer.points.begin())[1];
        
        typename layer_type::point_container_type::const_iterator it = layer.points.begin(); ++it;
        for (; it != layer.points.end(); ++it)
        {
            min_x = std::min( min_x, (*it)[0] );
            max_x = std::max( max_x, (*it)[0] );
            
            min_y = std::min( min_y, (*it)[0] );
            max_y = std::max( max_y, (*it)[0] );
        }
        
        lower_parallel_x.insert( std::make_pair(min_x, z_pos) );
        lower_parallel_x.insert( std::make_pair(max_x, z_pos) );

        upper_parallel_x.insert( std::make_pair(min_x, z_pos) );
        upper_parallel_x.insert( std::make_pair(max_x, z_pos) );

        lower_parallel_y.insert( std::make_pair(min_y, z_pos) );
        lower_parallel_y.insert( std::make_pair(max_y, z_pos) );

        upper_parallel_y.insert( std::make_pair(min_y, z_pos) );
        upper_parallel_y.insert( std::make_pair(max_y, z_pos) );        
    }
    
    
    
    
    template<typename layer_type>
    void sin_from_layer( layer_type & layer, numeric_type z_pos, double sin_height, unsigned int num_points = 10, unsigned int num_intervals = 1 )
    {
        if (layer.points.empty()) return;
        
        numeric_type min_x = (*layer.points.begin())[0];
        numeric_type max_x = (*layer.points.begin())[0];
        
        numeric_type min_y = (*layer.points.begin())[1];
        numeric_type max_y = (*layer.points.begin())[1];
        
        typename layer_type::point_container_type::const_iterator it = layer.points.begin(); ++it;
        for (; it != layer.points.end(); ++it)
        {
            min_x = std::min( min_x, (*it)[0] );
            max_x = std::max( max_x, (*it)[0] );
            
            min_y = std::min( min_y, (*it)[0] );
            max_y = std::max( max_y, (*it)[0] );
        }
        
        double step_x = (max_x - min_x) / (num_points-1);
        double step_y = (max_y - min_y) / (num_points-1);
        
        double x = min_x;
        double y = min_y;
        
        numeric_type step = M_PI * static_cast<numeric_type>(num_intervals) / (num_points-1);
        numeric_type pos = 0;
        
        for (unsigned int i = 0; i != num_points; ++i, pos += step, x += step_x, y += step_y)
        {
            numeric_type cur_z = std::sin(pos) * sin_height + z_pos;
            lower_parallel_x.insert( std::make_pair(x, cur_z) );
            upper_parallel_x.insert( std::make_pair(x, cur_z) );

            lower_parallel_y.insert( std::make_pair(y, cur_z) );
            upper_parallel_y.insert( std::make_pair(y, cur_z) );
            
            layer.add_cut_parallel_x( y );
            layer.add_cut_parallel_y( x );
        }
        
//         for (x = min_x; x < max_x; x += step_x)
//             for (y = min_x; y < max_x; y += step_y)
//                 layer.create_point( typename layer_type::point_type(x, y) );
    }
    
    
    
    std::pair<numeric_type, numeric_type> cut_parallel_x(numeric_type y) const
    {
        return std::make_pair(
            interpolate( lower_parallel_y, y ),
            interpolate( upper_parallel_y, y )
        );
    }
    
    std::pair<numeric_type, numeric_type> cut_parallel_y(numeric_type x) const
    {
        return std::make_pair(
            interpolate( lower_parallel_x, x ),
            interpolate( upper_parallel_x, x )
        );
    }
    
    
    template<typename point_type>
    numeric_type operator() ( point_type const & point ) const
    {
        numeric_type x = point[0];
        numeric_type y = point[1];
        
        std::pair<numeric_type, numeric_type> parallel_x = cut_parallel_x(y);
        std::pair<numeric_type, numeric_type> parallel_y = cut_parallel_x(x);
        
        numeric_type length_x = max_x() - min_x();
        numeric_type parallel_x_interpolated = parallel_x.first * ( max_x() - x ) / length_x + parallel_x.second * ( x - min_x() ) / length_x;

        numeric_type length_y = max_y() - min_y();
        numeric_type parallel_y_interpolated = parallel_y.first * ( max_y() - y ) / length_y + parallel_y.second * ( y - min_y() ) / length_y;

        return (parallel_x_interpolated + parallel_y_interpolated) / 2;
    }
    
    
    
    numeric_type min_x() const { return std::min( lower_parallel_x.begin()->first, upper_parallel_x.begin()->first ); }
    numeric_type max_x() const { return std::max( lower_parallel_x.rbegin()->first, upper_parallel_x.rbegin()->first ); }
    
    numeric_type min_y() const { return std::min( lower_parallel_y.begin()->first, upper_parallel_y.begin()->first ); }
    numeric_type max_y() const { return std::max( lower_parallel_y.rbegin()->first, upper_parallel_y.rbegin()->first ); }
    
    
    
    std::map< numeric_type, numeric_type > lower_parallel_x;
    std::map< numeric_type, numeric_type > upper_parallel_x;
    
    std::map< numeric_type, numeric_type > lower_parallel_y;
    std::map< numeric_type, numeric_type > upper_parallel_y;
};




int main()
{
    typedef viennagrid::config::point_type_2d point_type_2d;
    
    typedef layer<point_type_2d> layer_type;    
    typedef layer_interface<double> layer_interface_type;
    
    
    
//     layer_type tmp;
//     
//     tmp.add_line( point_type_2d(0,0), point_type_2d(10,0) );
//     tmp.add_line( point_type_2d(0,0), point_type_2d(0,10) );
// 
//     tmp.add_line( point_type_2d(10,0), point_type_2d(10,10) );
//     tmp.add_line( point_type_2d(0,10), point_type_2d(10,10) );
//     
//     tmp.add_circle( point_type_2d(10, 10), 4.0, 16, 0 );
//     
// //     tmp.check();
// //     tmp.print_lines();
//     
// //     tmp.add_cut_parallel_x( 0 );
// //     tmp.add_line( point_type_2d(0,10), point_type_2d(10,10) );
// //     tmp.check();
// //     
// //     tmp.add_cut_parallel_y( 0 );
// //     tmp.check();
// 
//     tmp.add_rectangle( point_type_2d(0,0), point_type_2d(10, 10), 3 );
//     tmp.add_circle( point_type_2d(9,9), 3.0, 16, 1 );
//     
//     
//     tmp.add_cut_parallel_x( 2 );
//     tmp.add_cut_parallel_y( 2 );
// //     tmp.check();
// 
//     tmp.add_cut_parallel_x( 4 );
//     tmp.add_cut_parallel_y( 4 );
// //     tmp.check();
//     
//     tmp.add_cut_parallel_x( 6 );
//     tmp.add_cut_parallel_y( 6 );
// //     tmp.check();
//     
//     tmp.add_cut_parallel_x( 8 );
//     tmp.add_cut_parallel_y( 8 );
// //     tmp.check();
//     
//     tmp.add_cut_parallel_x( 10 );
//     tmp.add_cut_parallel_y( 10 );
// //     tmp.check();
// 
//     tmp.add_cut_parallel_x( 12 );
//     
//     std::cout << std::endl << std::endl << std::endl << std::endl << std::endl;
//     
//     tmp.add_cut_parallel_y( 12 );
// //     tmp.check();
// 
//     tmp.add_cut_parallel_x( 14 );
//     tmp.add_cut_parallel_y( 14 );
//     tmp.check();
//     
//     viennagrid::config::line_2d_domain line_domain;
//     tmp.to_viennagrid_domain(line_domain);
//     
//     {
//         viennagrid::io::vtk_writer<viennagrid::config::line_2d_domain, viennagrid::config::line_2d_cell> vtk_writer;
//         vtk_writer(line_domain, "tmp_layer");
//     }
    
    
//     std::vector< std::pair<layer_type, double> > layers(3);
//     
//     layers[2].first.add_rectangle( point_type_2d(1,1), point_type_2d(7, 8), 0 );
//     layers[2].first.add_rectangle( point_type_2d(3,3), point_type_2d(4, 4), 1 );
//     layers[2].first.add_rectangle( point_type_2d(5,3), point_type_2d(6, 4), 2 );
//     layers[2].second = 7.0;
//     
//     layers[1].first.add_rectangle( point_type_2d(0,0), point_type_2d(10, 10), 3 );
//     layers[1].first.add_circle( point_type_2d(9,9), 3.0, 16, 4 );
//     layers[1].second = 5.0;
//     
//     layers[0].first.add_rectangle( point_type_2d(-5,-5), point_type_2d(15, 15), 5 );
//     layers[0].second = 10.0;

    
    
    unsigned int object_index = 0;
    std::vector< std::pair<layer_type, double> > layers;
    
    {
        std::pair<layer_type, double> tmp;
        tmp.first.add_rectangle( point_type_2d(0,0), point_type_2d(4.8, 4.8), object_index++ );
        tmp.second = 4.95;
        layers.push_back(tmp);
    }
    
    {
        std::pair<layer_type, double> tmp;
        tmp.first.add_rectangle( point_type_2d(0,0), point_type_2d(4.8, 4.8), object_index++ );
        tmp.second = 5.0;
        layers.push_back(tmp);
    }

    {
        std::pair<layer_type, double> tmp;
        tmp.first.add_rectangle( point_type_2d(0,0), point_type_2d(4.8, 4.8), object_index++);
        tmp.second = 0.5;
        layers.push_back(tmp);
    }

    {
        std::pair<layer_type, double> tmp;
        tmp.first.add_rectangle( point_type_2d(0,0), point_type_2d(4.8, 4.8), object_index++ );
        tmp.first.add_rectangle( point_type_2d(0.6,0), point_type_2d(1.2, 4.8), object_index++ );
        tmp.first.add_rectangle( point_type_2d(1.8,0), point_type_2d(2.4, 4.8), object_index++ );
        tmp.first.add_rectangle( point_type_2d(3.0,0), point_type_2d(3.6, 4.8), object_index++ );
        tmp.first.add_rectangle( point_type_2d(4.2,0), point_type_2d(4.8, 4.8), object_index++ );
        tmp.second = 0.5;
        layers.push_back(tmp);
    }

    {
        std::pair<layer_type, double> tmp;
        tmp.first.add_rectangle( point_type_2d(0,0), point_type_2d(4.8, 4.8), object_index++ );
        tmp.first.add_rectangle( point_type_2d(3.1,1.8), point_type_2d(3.5, 2.2), object_index++ );
        tmp.first.add_rectangle( point_type_2d(4.3,0.1), point_type_2d(4.7, 0.5), object_index++ );
        tmp.second = 0.5;
        layers.push_back(tmp);
    }

    {
        std::pair<layer_type, double> tmp;
        tmp.first.add_rectangle( point_type_2d(0,0), point_type_2d(4.8, 4.8), object_index++ );
        tmp.first.add_rectangle( point_type_2d(0,0), point_type_2d(4.8, 0.8), object_index++ );
        tmp.first.add_rectangle( point_type_2d(0,1.6), point_type_2d(4.8, 2.4), object_index++ );
        tmp.first.add_rectangle( point_type_2d(0,3.2), point_type_2d(4.8, 4.0), object_index++ );
        tmp.second = 0.5;
        layers.push_back(tmp);
    }

    {
        std::pair<layer_type, double> tmp;
        tmp.first.add_rectangle( point_type_2d(0,0), point_type_2d(4.8, 4.8), object_index++ );
        tmp.second = 3.0;
        layers.push_back(tmp);
    }

    

    

    

    

    


    
    

    

    
    viennagrid::config::plc_3d_domain plc_domain;
    typedef viennagrid::config::point_type_3d point_type_3d;
    
    typedef viennagrid::result_of::element<viennagrid::config::plc_3d_domain, viennagrid::plc_tag>::type plc_type;
    typedef viennagrid::result_of::element_hook<viennagrid::config::plc_3d_domain, viennagrid::vertex_tag>::type vertex_hook_type;
    typedef viennagrid::result_of::element_hook<viennagrid::config::plc_3d_domain, viennagrid::line_tag>::type line_hook_type;
//     typedef viennagrid::result_of::element_hook<viennagrid::config::plc_3d_domain, viennagrid::polygon_tag>::type polygon_hook_type;
    
    
    std::vector<viennagrid::config::triangular_2d_domain> triangulated_interface_domains;
    triangulated_interface_domains.resize( layers.size()+1 );
    
    double sin_height = 1.0;
    unsigned int num_sin_points = 4;
    unsigned int num_sin_intervals = 5;
    
    double cur_z = 0.0;
    std::vector< std::pair<layer_type, layer_interface_type> > interfaces;
    {
        layer_interface_type interface;
        layer_type tmp = layers.begin()->first;
        interface.planar_from_layer( layers.begin()->first, cur_z );
//         interface.sin_from_layer( tmp, cur_z, sin_height, num_sin_points, num_sin_intervals );
        interfaces.push_back( std::make_pair(tmp, interface) );
    }
    
    std::vector< std::pair<layer_type, double> >::iterator lit1 = layers.begin();
    std::vector< std::pair<layer_type, double> >::iterator lit2 = lit1; ++lit2;
    for (; lit2 != layers.end(); ++lit1, ++lit2)
    {
        cur_z += lit1->second;
        layer_type tmp = lit1->first;
        tmp.add_layer(lit2->first);
        
        layer_interface_type interface;
        interface.planar_from_layer(tmp, cur_z);
//         interface.sin_from_layer( tmp, cur_z, sin_height, num_sin_points, num_sin_intervals );
        
        interfaces.push_back( std::make_pair(tmp, interface) );
    }
    
    cur_z += lit1->second;
    {
        layer_interface_type interface;
        layer_type tmp = lit1->first;
        interface.planar_from_layer( lit1->first, cur_z );
//         interface.sin_from_layer( tmp, cur_z, sin_height, num_sin_points, num_sin_intervals );
        interfaces.push_back( std::make_pair(tmp, interface) );
    }
    
    
    std::vector< std::map<layer_type::index_type, vertex_hook_type> > interface_vertex_hook_map( interfaces.size() );
    std::vector< std::map<layer_type::line_type, line_hook_type> > interface_line_hook_map( interfaces.size() );
    
    
    // adding lines
    {
        unsigned int interface_index = 0;
        for (std::vector< std::pair<layer_type, layer_interface_type> >::iterator iit = interfaces.begin(); iit != interfaces.end(); ++iit, ++interface_index)
        {
//             double height = iit->second;
            layer_interface_type const & interface = iit->second;
            layer_type const & current_layer = iit->first;
            
            viennagrid::config::line_2d_domain line_domain;
            current_layer.to_viennagrid_domain( line_domain );            
            viennamesh::result_of::settings<viennamesh::cgal_plc_2d_mesher_tag>::type settings;
            viennamesh::run_algo< viennamesh::cgal_plc_2d_mesher_tag >( line_domain, triangulated_interface_domains[interface_index], settings );
            
            
            {
                std::stringstream tmp;
                tmp << "interface_lines_" << interface_index << std::endl;
                viennagrid::io::vtk_writer<viennagrid::config::triangular_2d_domain, viennagrid::triangle_tag> vtk_writer;
                vtk_writer(triangulated_interface_domains[interface_index], tmp.str());
            }
            
//             current_layer.check();
//             current_layer.print_lines();
            
            std::map<layer_type::index_type, vertex_hook_type> & vertex_hook_map = interface_vertex_hook_map[interface_index];
            std::map<layer_type::line_type, line_hook_type> & line_hook_map = interface_line_hook_map[interface_index];
            
            for (layer_type::index_type pit = 0; pit != current_layer.points.size(); ++pit)
                vertex_hook_map[pit] = viennagrid::create_vertex( plc_domain, point_type_3d( current_layer.points[pit][0], current_layer.points[pit][1], interface(current_layer.points[pit]) ) );
            
            std::vector<line_hook_type> plc_lines;
            
            for (layer_type::line_container_type::const_iterator lit = current_layer.lines.begin(); lit != current_layer.lines.end(); ++lit)
            {
                line_hook_type line_hook = viennagrid::create_line( plc_domain, vertex_hook_map[lit->first], vertex_hook_map[lit->second] );
                line_hook_map[*lit] = line_hook;
                plc_lines.push_back( line_hook );
            }
            
        }
    }
    
    
    
    
    {
        unsigned int interface_index = 0;
        std::vector< std::pair<layer_type, layer_interface_type> >::iterator iit1 = interfaces.begin();
        std::vector< std::pair<layer_type, layer_interface_type> >::iterator iit2 = iit1; ++iit2;
        
        for (std::vector< std::pair<layer_type, double> >::iterator lit = layers.begin(); lit != layers.end(); ++lit, ++iit1, ++iit2, ++interface_index)
        {
            layer_type const & current_layer = lit->first;
            layer_type const & lower_interface = iit1->first;
            layer_type const & upper_interface = iit2->first;
            
//             lower_interface.check();
//             upper_interface.check();
            
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
                
                
//                 polygon_hook_type poly_hook;
                vertex_hook_type vtx_hook;
                point_type_3d pt;
                
                viennagrid::create_element<plc_type>( plc_domain, plc_lines.begin(), plc_lines.end(), &vtx_hook, &vtx_hook, &pt, &pt );
                
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


    {
        viennagrid::io::vtk_writer<viennagrid::config::plc_3d_domain, viennagrid::line_tag> vtk_writer;
        vtk_writer(plc_domain, "plc_lines");
    }


    viennagrid::config::triangular_3d_domain triangle_domain_3d;
    
    viennamesh::result_of::settings<viennamesh::cgal_plc_3d_mesher_tag>::type settings;
    
    viennamesh::run_algo< viennamesh::cgal_plc_3d_mesher_tag >( plc_domain, triangle_domain_3d, settings );
    
    {
        unsigned int interface_index;
        std::vector< std::pair<layer_type, layer_interface_type> >::iterator iit = interfaces.begin();
        for (std::vector<viennagrid::config::triangular_2d_domain>::iterator it = triangulated_interface_domains.begin(); it != triangulated_interface_domains.end(); ++it, ++interface_index, ++iit)
        {
            viennagrid::config::triangular_2d_domain const & triangle_domain = *it;
            layer_interface_type const & interface = iit->second;
                      
            typedef viennagrid::result_of::const_element_range<viennagrid::config::triangular_2d_domain, viennagrid::triangle_tag>::type triangle_range_type;
            typedef viennagrid::result_of::iterator<triangle_range_type>::type triangle_range_iterator;
            
            triangle_range_type triangles = viennagrid::elements<viennagrid::triangle_tag>(triangle_domain);
            for (triangle_range_iterator triit = triangles.begin(); triit != triangles.end(); ++triit)
            {
                typedef viennagrid::result_of::element_hook<viennagrid::config::triangular_3d_domain, viennagrid::vertex_tag>::type vertex_hook_type;
                vertex_hook_type triangle_hooks[3];
                
                for (unsigned int i = 0; i != 3; ++i)
                {
                    point_type_2d point_2d = viennagrid::point( triangle_domain, viennagrid::elements<viennagrid::vertex_tag>(*triit)[i] );
                    point_type_3d point_3d( point_2d[0], point_2d[1], interface(point_2d) );
                    
                    triangle_hooks[i] = viennagrid::create_unique_vertex( triangle_domain_3d, point_3d );
                }
                
                viennagrid::create_triangle( triangle_domain_3d, triangle_hooks[0], triangle_hooks[1], triangle_hooks[2] );
            }
        }
    }
    
    
 
    {
        viennagrid::io::vtk_writer<viennagrid::config::triangular_3d_domain, viennagrid::config::triangular_3d_cell> vtk_writer;
        vtk_writer(triangle_domain_3d, "layer_triangulated_3d");
    }
    
    
    
//     std::map<layer_type::element_id_type, point_type_3d> segment_seed_points;
//     typedef std::vector< std::pair<layer_type::element_id_type, point_type_3d> > segment_seed_point_container_type;
//     segment_seed_point_container_type segment_seed_points;

    
    
    {
        unsigned int tmp_index = 0;
        std::map<unsigned int, layer_type::element_id_type> segment_index_map;
        
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
                
//                 segment_seed_points[segment_id] = center_3d;
                
                
                segment_index_map[tmp_index] = segment_id;
                
                viennamesh::add_segment_seed_point( triangle_domain_3d, segment_id, center_3d );
//                 viennamesh::add_segment_seed_point( triangle_domain_3d, tmp_index, center_3d );
                
                tmp_index++;
            }
            
            
            cur_z += height;
        }
        
        
//         for (std::map<layer_type::element_id_type, point_type_3d>::iterator it = segment_seed_points.begin(); it != segment_seed_points.end(); ++it)
//         {
//             std::cout << "Adding seed point: " << it->second << " to segment " << it->first << std::endl;
//             viennamesh::add_segment_seed_point( triangle_domain_3d, it->first, it->second );
//         }
        
        viennamesh::mark_face_segments( triangle_domain_3d );
        
        
//         viennamesh::cleanup_face_segment_definition<viennagrid::triangle_tag>( triangle_domain_3d );
        

        std::map<std::size_t, viennagrid::config::triangular_3d_domain> hull_segments;
        
        std::map<std::size_t, std::size_t> triangle_count;
        viennamesh::segment_id_container_type segments = viennamesh::segments( triangle_domain_3d );
        std::cout << "Number segments " << segments.size() << std::endl;
        for (viennamesh::segment_id_container_type::iterator it = segments.begin(); it != segments.end(); ++it)
        {
//             hull_segments.insert( std::make_pair(*it, viennagrid::config::triangular_3d_domain()) );
            triangle_count[ *it ] = 0;
        }
        
        typedef viennagrid::result_of::element_range<viennagrid::config::triangular_3d_domain, viennagrid::triangle_tag>::type triangles_range_type;
        typedef viennagrid::result_of::iterator<triangles_range_type>::type triangles_range_iterator;
        
        triangles_range_type triangles = viennagrid::elements<viennagrid::triangle_tag>( triangle_domain_3d );
        for (triangles_range_iterator it = triangles.begin(); it != triangles.end(); ++it)
        {
            viennamesh::face_segment_definition_type & face_segments = viennamesh::face_segments( *it );
            
            if ( face_segments.empty() )
            {
//                 std::cout << " !! ERROR !! a triangle is not marked for any segment" << std::endl;
                std::cout << "   " << viennagrid::point(triangle_domain_3d, viennagrid::elements<viennagrid::vertex_tag>(*it)[0]) << std::endl;
                std::cout << "   " << viennagrid::point(triangle_domain_3d, viennagrid::elements<viennagrid::vertex_tag>(*it)[1]) << std::endl;
                std::cout << "   " << viennagrid::point(triangle_domain_3d, viennagrid::elements<viennagrid::vertex_tag>(*it)[2]) << std::endl;
                
                continue;
            }
            

//                 std::cout << "   " << viennagrid::point(triangle_domain_3d, viennagrid::elements<viennagrid::vertex_tag>(*it)[0]) << std::endl;
//                 std::cout << "   " << viennagrid::point(triangle_domain_3d, viennagrid::elements<viennagrid::vertex_tag>(*it)[1]) << std::endl;
//                 std::cout << "   " << viennagrid::point(triangle_domain_3d, viennagrid::elements<viennagrid::vertex_tag>(*it)[2]) << std::endl;

            
//             for (viennamesh::face_segment_definition_type::iterator jt = face_segments.begin(); jt != face_segments.end(); ++jt)
//             {
//                 viennamesh::face_segment_definition_type::iterator kt = jt; ++kt;
//                 
//                 for ( ; kt != face_segments.end();)
//                 {
//                     if ( segment_index_map[jt->first] == segment_index_map[kt->first] )
//                     {
// //                         std::cout << "Found double segment element: " << jt->first << " " << kt->first << " seg_id=" << segment_index_map[jt->first] << std::endl;
//                         viennamesh::face_segment_definition_type::iterator tmp = kt; ++kt;
//                         face_segments.erase(tmp);
//                         viennamesh::remove_segment(triangle_domain_3d, kt->first);
//                         hull_segments.erase( kt->first );
//                         triangle_count.erase( kt->first );
//                     }
//                     else
//                         ++kt;
//                 }
//             }
            
            
            for (viennamesh::face_segment_definition_type::iterator jt = face_segments.begin(); jt != face_segments.end(); ++jt)
            {
//                 if ( jt->first != 2 ) continue;
                viennagrid::config::triangular_3d_domain & current_segment = hull_segments[jt->first];
                
                
                point_type_3d p0 = viennagrid::point(triangle_domain_3d, viennagrid::elements<viennagrid::vertex_tag>(*it)[0]);
                point_type_3d p1 = viennagrid::point(triangle_domain_3d, viennagrid::elements<viennagrid::vertex_tag>(*it)[1]);
                point_type_3d p2 = viennagrid::point(triangle_domain_3d, viennagrid::elements<viennagrid::vertex_tag>(*it)[2]);
                
                point_type_3d center = (p0+p1+p2)/3.0;
//                 point_type_3d vec_to_seedpoint = segment_seed_points[jt->first] - center;
                
                point_type_3d normal = viennagrid::cross_prod( p1-p0, p2-p0 );
                if ( jt->second ) normal = -normal;
                
                viennagrid::create_triangle( current_segment,
                    viennagrid::create_unique_vertex(current_segment, p0),
                    viennagrid::create_unique_vertex(current_segment, p1),
                    viennagrid::create_unique_vertex(current_segment, p2) );
                
//                 std::cout << "Segment ID = " << jt->first << std::endl;
//                 std::cout << "TRIANGLE (orientation " << jt->second << ") (faces to seed point " << (viennagrid::inner_prod(normal, vec_to_seedpoint) >= 0) << ")" << std::endl;
//                 std::cout << "   " << p0 << std::endl;
//                 std::cout << "   " << p1 << std::endl;
//                 std::cout << "   " << p2 << std::endl;
//                 std::cout << std::endl;
                
                ++triangle_count[ jt->first ];
            }
        }
        
        for (std::map<std::size_t, std::size_t>::iterator it = triangle_count.begin(); it != triangle_count.end(); ++it)
            std::cout << "Segment " << it->first << " has " << it-> second << " triangles" << std::endl;
        
        for (std::map<std::size_t, viennagrid::config::triangular_3d_domain>::iterator it = hull_segments.begin(); it != hull_segments.end(); ++it)
        {
            std::stringstream tmp;
            tmp << "hull_segment_" << it->first << std::endl;
            viennagrid::io::vtk_writer<viennagrid::config::triangular_3d_domain, viennagrid::triangle_tag> vtk_writer;
            vtk_writer(it->second, tmp.str());
        }
    }
    

    viennagrid::config::triangular_3d_domain hull_domain;
    viennamesh::result_of::settings<viennamesh::vgmodeler_hull_adaption_tag>::type vgm_settings;
    
    vgm_settings.cell_size = 0.3;
    
    viennamesh::run_algo< viennamesh::vgmodeler_hull_adaption_tag >( triangle_domain_3d, hull_domain, vgm_settings );
    
    
    {        
        viennagrid::io::vtk_writer<viennagrid::config::triangular_3d_domain, viennagrid::config::triangular_3d_cell> vtk_writer;
        vtk_writer(hull_domain, "layer_adapted_hull_3d");
    }
    
    
    {
        std::map<std::size_t, viennagrid::config::triangular_3d_domain> adapted_hull_segments;
        
        typedef viennagrid::result_of::element_range<viennagrid::config::triangular_3d_domain, viennagrid::triangle_tag>::type triangles_range_type;
        typedef viennagrid::result_of::iterator<triangles_range_type>::type triangles_range_iterator;
        
        triangles_range_type triangles = viennagrid::elements<viennagrid::triangle_tag>( hull_domain );
        for (triangles_range_iterator it = triangles.begin(); it != triangles.end(); ++it)
        {
            viennamesh::face_segment_definition_type face_segments = viennamesh::face_segments( *it );
            
            if ( face_segments.empty() )
                std::cout << " !! ERROR !! a triangle is not marked for any segment" << std::endl;
            
            
            for (viennamesh::face_segment_definition_type::iterator jt = face_segments.begin(); jt != face_segments.end(); ++jt)
            {
//                 if ( jt->first != 2 ) continue;
                viennagrid::config::triangular_3d_domain & current_segment = adapted_hull_segments[jt->first];
                
                
                point_type_3d p0 = viennagrid::point(hull_domain, viennagrid::elements<viennagrid::vertex_tag>(*it)[0]);
                point_type_3d p1 = viennagrid::point(hull_domain, viennagrid::elements<viennagrid::vertex_tag>(*it)[1]);
                point_type_3d p2 = viennagrid::point(hull_domain, viennagrid::elements<viennagrid::vertex_tag>(*it)[2]);
                
                point_type_3d center = (p0+p1+p2)/3.0;
//                 point_type_3d vec_to_seedpoint = segment_seed_points[jt->first] - center;
                
                point_type_3d normal = viennagrid::cross_prod( p1-p0, p2-p0 );
                if ( jt->second ) normal = -normal;
                
                viennagrid::create_triangle( current_segment,
                    viennagrid::create_unique_vertex(current_segment, p0),
                    viennagrid::create_unique_vertex(current_segment, p1),
                    viennagrid::create_unique_vertex(current_segment, p2) );
                
//                 std::cout << "Segment ID = " << jt->first << std::endl;
//                 std::cout << "TRIANGLE (orientation " << jt->second << ") (faces to seed point " << (viennagrid::inner_prod(normal, vec_to_seedpoint) >= 0) << ")" << std::endl;
//                 std::cout << "   " << p0 << std::endl;
//                 std::cout << "   " << p1 << std::endl;
//                 std::cout << "   " << p2 << std::endl;
//                 std::cout << std::endl;
                
//                 ++triangle_count[ jt->first ];
            }
        }
        
//         for (std::map<std::size_t, std::size_t>::iterator it = triangle_count.begin(); it != triangle_count.end(); ++it)
//             std::cout << "Segment " << it->first << " has " << it-> second << " triangles" << std::endl;
        
        for (std::map<std::size_t, viennagrid::config::triangular_3d_domain>::iterator it = adapted_hull_segments.begin(); it != adapted_hull_segments.end(); ++it)
        {
            std::stringstream tmp;
            tmp << "adapted_hull_segment_" << it->first << std::endl;
            viennagrid::io::vtk_writer<viennagrid::config::triangular_3d_domain, viennagrid::triangle_tag> vtk_writer;
            vtk_writer(it->second, tmp.str());
        }
    }
    
    
    
    viennagrid::config::tetrahedral_3d_domain tetrahedron_domain;
    viennamesh::result_of::settings<viennamesh::netgen_tetrahedron_tag>::type netgen_settings;
    
    netgen_settings.cell_size = 0.3;
    
    viennamesh::run_algo< viennamesh::netgen_tetrahedron_tag >( hull_domain, tetrahedron_domain, netgen_settings );
    
    
    {        
        viennagrid::io::vtk_writer<viennagrid::config::tetrahedral_3d_domain, viennagrid::config::tetrahedral_3d_cell> vtk_writer;
        viennagrid::io::add_scalar_data_on_cells<viennamesh::unique_segment_tag, viennamesh::segment_id_type>(vtk_writer, "segment_id");
        vtk_writer(tetrahedron_domain, "netgen_volume");
    }

    
    return 0;
}
