#ifndef VIENNAMESH_DOMAIN_VIENNASAP_LAYER_HPP
#define VIENNAMESH_DOMAIN_VIENNASAP_LAYER_HPP

#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/intersect.hpp"
#include "viennagrid/domain/element_creation.hpp"

namespace viennamesh
{
    namespace viennasap
        {
        
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
        struct layer_t
        {
            typedef layer_t<point_type_> self_type;
            
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
                
                std::cout << "OOOOOOPS!!" << std::endl;
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
                typedef viennagrid::result_of::element_handle<viennagrid::config::line_2d_domain, viennagrid::vertex_tag>::type vertex_handle_type;
                
                std::map<index_type, vertex_handle_type> vertex_handle_map;
                
                for (unsigned int it = 0; it != points.size(); ++it)
                    vertex_handle_map[it] = viennagrid::create_vertex( vgrid_domain, get_point(it) );
                
                for (typename line_container_type::const_iterator it = lines.begin(); it != lines.end(); ++it)
                    viennagrid::create_line( vgrid_domain, vertex_handle_map[it->first], vertex_handle_map[it->second] );
            }
            
            
            
            point_container_type points;
            
            line_container_type lines;
            polygon_container_type polygons;
        };
            

        template<typename numeric_type>
        struct layer_interface_t
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
            

            numeric_type operator() ( numeric_type x, numeric_type y ) const
            {
                std::pair<numeric_type, numeric_type> parallel_x = cut_parallel_x(y);
                std::pair<numeric_type, numeric_type> parallel_y = cut_parallel_x(x);
                
                numeric_type length_x = max_x() - min_x();
                numeric_type parallel_x_interpolated = parallel_x.first * ( max_x() - x ) / length_x + parallel_x.second * ( x - min_x() ) / length_x;

                numeric_type length_y = max_y() - min_y();
                numeric_type parallel_y_interpolated = parallel_y.first * ( max_y() - y ) / length_y + parallel_y.second * ( y - min_y() ) / length_y;

                return (parallel_x_interpolated + parallel_y_interpolated) / 2;
            }
            
            
            template<typename point_type>
            numeric_type operator() ( point_type const & point ) const
            {
                return (*this)(point[0], point[1]);
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
    }



    
    struct viennasap_layer_domain
    {
        typedef viennagrid::config::point_type_2d point_type;
        
        typedef viennasap::layer_t<point_type> layer_type;
        typedef viennasap::layer_interface_t<double> interface_type;
        
        void calc_interface_layers()
        {
            interface_layers.push_back( layers.front() );
            for (unsigned int i = 0; i < layers.size()-1; ++i)
            {
                interface_layers.push_back( layers[i] );
                interface_layers.back().add_layer( layers[i+1] );
            }
            interface_layers.push_back( layers.back() );
        }
        
        std::deque< layer_type > layers;
        std::deque< interface_type > interfaces;
        std::deque< layer_type > interface_layers;
    };
    

    
}



#endif
