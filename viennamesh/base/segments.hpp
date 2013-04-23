#ifndef VIENNAMESH_BASE_SEGMENTS_HPP
#define VIENNAMESH_BASE_SEGMENTS_HPP


#include <boost/shared_ptr.hpp>
#include "viennadata/api.hpp"

#include "viennagrid/domain/config.hpp"
#include "viennagrid/domain/coboundary_iteration.hpp"

#include "viennagrid/algorithm/cross_prod.hpp"
#include "viennagrid/algorithm/inner_prod.hpp"
#include "viennagrid/algorithm/geometry.hpp"

namespace viennamesh
{
    struct face_visited_tag {};
}
VIENNADATA_ENABLE_TYPE_BASED_KEY_DISPATCH(viennamesh::face_visited_tag)

namespace viennamesh
{
    template<typename face_element_type>
    bool was_visited( face_element_type const & face )
    {
        if (viennadata::find<face_visited_tag, bool>()(face))
            return viennadata::access<face_visited_tag, bool>()(face);
        else
            return false;
    }

    template<typename face_element_type>
    bool set_visited( face_element_type & face )
    {
        viennadata::access<face_visited_tag, bool>()(face) = true;
    }

    template<typename face_element_type>
    void clear_visited( face_element_type & face )
    {
        viennadata::erase<face_visited_tag, bool>()(face);
    }
}


namespace viennamesh
{
    typedef unsigned int segment_id_type;
    typedef std::set<segment_id_type> segment_id_container_type;
    
    struct segments_tag {};
    struct segment_seed_points_tag {};
}

VIENNADATA_ENABLE_TYPE_BASED_KEY_DISPATCH(viennamesh::segment_seed_points_tag)
VIENNADATA_ENABLE_TYPE_BASED_KEY_DISPATCH(viennamesh::segments_tag)

namespace viennamesh
{
    namespace result_of
    {
        template<typename domain_type>
        struct seed_point_container
        {
            typedef typename std::deque< std::pair<segment_id_type, typename viennagrid::result_of::point_type<domain_type>::type> > type;
        };
    }
    
    template<typename domain_type>
    segment_id_container_type const & segments( domain_type const & domain )
    {
        return viennadata::access<segments_tag, std::set<segment_id_type> >()(domain);
    }
    
    template<typename domain_type>
    void add_segment( domain_type & domain, segment_id_type segment_id )
    {
        viennadata::access<segments_tag, segment_id_container_type>()(domain).insert(segment_id);
    }
    
    template<typename domain_type>
    void clear_segments( domain_type & domain )
    {
        viennadata::access<segments_tag, segment_id_container_type>()(domain).clear();
    }
    
    template<typename domain_type>
    void remove_segment( domain_type & domain, segment_id_type segment_id )
    {
        viennadata::access<segments_tag, segment_id_container_type>()(domain).erase(segment_id);
    }
    
    
    template<typename domain_type>
    typename result_of::seed_point_container<domain_type>::type & segment_seed_points( domain_type & domain )
    {
        typedef typename result_of::seed_point_container<domain_type>::type seed_point_container_type;
        return viennadata::access<segment_seed_points_tag, seed_point_container_type>()(domain);
    }
    
    template<typename domain_type>
    typename result_of::seed_point_container<domain_type>::type const & segment_seed_points( domain_type const & domain )
    {
        typedef typename result_of::seed_point_container<domain_type>::type seed_point_container_type;
        return viennadata::access<segment_seed_points_tag, seed_point_container_type>()(domain);
    }
    
    template<typename domain_type>
    void add_segment_seed_point( domain_type & domain, segment_id_type segment_id, typename viennagrid::result_of::point_type<domain_type>::type const & seed_point)
    {
        segment_seed_points(domain).push_back( std::make_pair(segment_id, seed_point) );
        add_segment(domain, segment_id);
    }
    
    template<typename in_domain_type, typename out_domain_type>
    void copy_segment_information( in_domain_type const & domain_in, out_domain_type & domain_out )
    {
        viennadata::access<segments_tag, segment_id_container_type>()(domain_out) = segments(domain_in);
        segment_seed_points(domain_out) = segment_seed_points(domain_in);
    }
}



namespace viennamesh
{
    // each element has map of segment indices and a corresponding faces outward flag
    //   if normal = cross( p1-p0, p2-p0 ) faces outward for this segment, this flag is true; false otherwise
    typedef std::map< segment_id_type, bool > face_segment_definition_type;
    struct face_segment_definition_tag {};
}

VIENNADATA_ENABLE_TYPE_BASED_KEY_DISPATCH(viennamesh::face_segment_definition_tag)


namespace viennamesh
{
    template<typename face_element_type>
    face_segment_definition_type const & face_segments( face_element_type const & face )
    {
        return viennadata::access<face_segment_definition_tag, face_segment_definition_type>()(face);
    }

    template<typename face_element_type>
    face_segment_definition_type & face_segments( face_element_type & face )
    {
        return viennadata::access<face_segment_definition_tag, face_segment_definition_type>()(face);
    }
    
    // return if the element is a face of the segment
    template<typename face_element_type>
    bool is_face_on_segment( face_element_type const & face, segment_id_type segment_id )
    {
        face_segment_definition_type const & seg_def = face_segments(face);
        face_segment_definition_type::const_iterator it = seg_def.find(segment_id);

        return (it != seg_def.end());
    }
    
    // return if the element faces outward for this segment; if the element is not a face of the segment, false is returned
    template<typename face_element_type>
    bool faces_outward_on_segment( face_element_type const & face, segment_id_type segment_id )
    {
        face_segment_definition_type const & seg_def = face_segments(face);
        face_segment_definition_type::const_iterator it = seg_def.find(segment_id);

        if (it != seg_def.end())
            return false;
        else
            return it->second;
    }

    // adds a face element to a segment
    template<typename domain_type, typename face_element_type>
    bool add_face_to_segment( domain_type & domain, face_element_type & face, segment_id_type segment_id, bool faces_outward )
    {
        add_segment(domain, segment_id);
        face_segment_definition_type & seg_def = face_segments(face);
        face_segment_definition_type::iterator it = seg_def.find(segment_id);
        if (it == seg_def.end())
            seg_def.insert( std::make_pair(segment_id, faces_outward) );
        else
            return faces_outward == it->second;
        
        return true;
    }

    
    
    
    template<typename face_element_tag_or_type, typename domain_type>
    bool check_face_segments( domain_type const & domain )
    {
        typedef typename viennagrid::result_of::const_element_range<domain_type, face_element_tag_or_type>::type face_range_type;
        typedef typename viennagrid::result_of::iterator<face_range_type>::type face_range_iterator;
        
        face_range_type faces = viennagrid::elements<face_element_tag_or_type>( domain );
        for (face_range_iterator it = faces.begin(); it != faces.end(); ++it)
        {
            face_segment_definition_type const & fs_def = face_segments(*it);
            bool error = false;
            
            if (fs_def.size() == 0)
            {
                std::cout << "  ERROR: A face has no segment!" << std::endl;
                error = true;
            }
            
            if (fs_def.size() > 2)
            {
                std::cout << "  ERROR: A face is in 2 or more segments!" << std::endl;
                error = true;
            }
            
            if ( (fs_def.size() == 2) &&  ((fs_def.begin())->second == (++fs_def.begin())->second) )
            {
                std::cout << "  ERROR: A face is in 2 segments with same orientation" << std::endl;
                error = true;
            }
            
            if ( (fs_def.size() == 2) && ((fs_def.begin())->first == (++fs_def.begin())->first) )
            {
                std::cout << "  ERROR: A face is in 2 identical segments" << std::endl;
                error = true;
            }
            
            if (error)
            {
                std::cout << "TRIANGLE: " << viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(*it)[0] ) << " - " <<
                            viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(*it)[1] ) << " - " <<
                            viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(*it)[2] ) << std::endl;
                
            }
            
//             viennamesh::segment_id_type first = (segment_def.begin())->first;
//             viennamesh::segment_id_type second = (++segment_def.begin())->first;
            
            
//             for (typename face_segment_definition_type::const_iterator jt = fs_def.begin(); jt != fs_def.end(); ++jt)
                
        }
    }
    
    

    
    template<typename face_element_tag_or_type, typename domain_type>
    void generate_segment_ids( domain_type & domain )
    {
        clear_segments(domain);
        
        typedef typename viennagrid::result_of::element_range<domain_type, face_element_tag_or_type>::type face_range_type;
        typedef typename viennagrid::result_of::iterator<face_range_type>::type face_range_iterator;
        
        face_range_type faces = viennagrid::elements<face_element_tag_or_type>( domain );
        for (face_range_iterator it = faces.begin(); it != faces.end(); ++it)
        {
            face_segment_definition_type const & fs_def = face_segments(*it);
            for (typename face_segment_definition_type::const_iterator jt = fs_def.begin(); jt != fs_def.end(); ++jt)
                add_segment(domain, jt->first);
        }
    }
    
    
    
    
    template<typename face_element_tag_or_type, typename domain_type>
    void defragment_segment_ids( domain_type & domain )
    {
        segment_id_type segment_id = 0;
        std::map<segment_id_type, segment_id_type> segment_id_map;
        
        typedef typename viennagrid::result_of::element_range<domain_type, face_element_tag_or_type>::type face_range_type;
        typedef typename viennagrid::result_of::iterator<face_range_type>::type face_range_iterator;
        
        face_range_type faces = viennagrid::elements<face_element_tag_or_type>( domain );
        for (face_range_iterator it = faces.begin(); it != faces.end(); ++it)
        {
            face_segment_definition_type fs_def = face_segments(*it);
            face_segment_definition_type new_fs_def;
            
            for (typename face_segment_definition_type::const_iterator jt = fs_def.begin(); jt != fs_def.end(); ++jt)
            {
                segment_id_type current_segment_id;
                std::map<segment_id_type, segment_id_type>::iterator kt = segment_id_map.find( jt->first );
                if (kt != segment_id_map.end())
                    current_segment_id = kt->second;
                else
                {
                    current_segment_id = segment_id++;
                    segment_id_map[jt->first] = current_segment_id;
                }
                
                new_fs_def[current_segment_id] = jt->second;
            }
            
            face_segments(*it) = new_fs_def;                
        }
        
        clear_segments(domain);
        for (std::map<segment_id_type, segment_id_type>::iterator it = segment_id_map.begin(); it != segment_id_map.end(); ++it)
            add_segment(domain, it->second);
    }
    
    
    template<typename face_element_tag_or_type, typename domain_type>
    void cleanup_face_segment_definition( domain_type & domain )
    {
        typedef typename viennagrid::result_of::element_range<domain_type, face_element_tag_or_type>::type face_range_type;
        typedef typename viennagrid::result_of::iterator<face_range_type>::type face_range_iterator;
        
        face_range_type faces = viennagrid::elements<face_element_tag_or_type>( domain );
        for (face_range_iterator it = faces.begin(); it != faces.end(); ++it)
        {
            face_segment_definition_type & fs_def = face_segments( *it );
            
            for (typename face_segment_definition_type::iterator jt = fs_def.begin(); jt != fs_def.end(); ++jt)
            {
                typename face_segment_definition_type::iterator kt = jt; ++kt;
                for ( ; kt != fs_def.end(); )
                {
                    std::cout << "Found double! " << jt->first << " " << kt->first << std::endl;
                    if ( jt->second == kt->second )
                    {
                        typename face_segment_definition_type::iterator tmp = kt; ++kt;
                        fs_def.erase(tmp);
                    }
                    else
                        ++kt;
                }
                
            }
            
        }
        
//         generate_segments<face_element_tag_or_type>(domain);
        defragment_segment_ids<face_element_tag_or_type>(domain);
    }
    
}

namespace viennamesh
{
    struct unique_segment_tag {};
}

VIENNADATA_ENABLE_TYPE_BASED_KEY_DISPATCH(viennamesh::unique_segment_tag)




namespace viennamesh
{
    template<typename volume_element_type>
    segment_id_type & segment( volume_element_type & volume )
    {
        return viennadata::access<unique_segment_tag, segment_id_type>()(volume);
    }
    
    template<typename volume_element_type>
    segment_id_type const & segment( volume_element_type const & volume )
    {
        return viennadata::access<unique_segment_tag, segment_id_type>()(volume);
    }
}



namespace viennamesh
{
    namespace utils
    {
        template<typename point_type>
        typename viennagrid::result_of::coord_type<point_type>::type determinant( point_type const & p0, point_type const & p1, point_type const & p2 )
        {
            return p0[0]*p1[1]*p2[2] + p1[0]*p2[1]*p0[2] + p2[0]*p0[1]*p1[2] - p0[2]*p1[1]*p2[0] - p1[2]*p2[1]*p0[0] - p2[2]*p0[1]*p1[0];
        }


        template<typename point_type>
        bool triangle_ray_intersect(point_type const & r, point_type d, point_type const & A, point_type const & B, point_type const & C)
        {
            point_type b = B-A; // vector from A to B
            point_type c = C-A; // vector from A to C
            
            point_type rhs = r-A;
            
            // solve the equation:
            // r + lambda*d = A + beta*b + gamma*c
            // beta*b + gamma*c - lambda*d = r-A
            // (b c -d) (beta gamma lambda)^T = r-A
            // (beta gamma lambda)^T = (b c -d)^-1 (r-A)

            double det = determinant( b, c, -d );
            
//             std::cout << "      det = " << det << std::endl;
            
            if ( std::abs(det) < 1e-6)
            {
                
                point_type n = viennagrid::cross_prod( B-A, C-A );
                
                point_type center = (A+B+C)/3.0;
                if( std::abs( viennagrid::inner_prod( n, r-center ) ) < 1e-6 )
                    return true;    // r lies on triangle plane, TODO behandeln: kreuzt strahl dreieck?
                else
                    return false;
            }
            else
            {
                point_type im[3]; // inverse matrix (b c -d)^-1
                
                d = -d; // invert for simplicity
                
                im[0][0] =   c[1]*d[2] - c[2]*d[1];
                im[1][0] = -(b[1]*d[2] - b[2]*d[1]);
                im[2][0] =   b[1]*c[2] - b[2]*c[1];
                
                im[0][1] = -(c[0]*d[2] - c[2]*d[0]);
                im[1][1] =   b[0]*d[2] - b[2]*d[0];
                im[2][1] = -(b[0]*c[2] - b[2]*c[0]);
                
                im[0][2] =   c[0]*d[1] - c[1]*d[0];
                im[1][2] = -(b[0]*d[1] - b[1]*d[0]);
                im[2][2] =   b[0]*c[1] - b[1]*c[0];
                
                double beta = viennagrid::inner_prod( rhs, im[0] );
                double gamma = viennagrid::inner_prod( rhs, im[1] );
                double lambda = viennagrid::inner_prod( rhs, im[2] );
                
                if (det < 0)
                {
                    det = -det;
                    beta = -beta;
                    gamma = -gamma;
                    lambda = -lambda;
                }

                
                
                double alpha = det - beta - gamma;
                
//                 std::cout << "      alpha = " << alpha << std::endl;
//                 std::cout << "      beta = " << beta << std::endl;
//                 std::cout << "      gamma = " << gamma << std::endl;
//                 std::cout << "      lambda = " << lambda << std::endl;
                
                
//                                 std::cout << "    Found intersection: " << alpha/det << " " << beta/det << " " << gamma/det << " " << lambda/det << std::endl;
                double offset = det * 1e-6;
                double lower = 0 - offset;
                double upper = det + offset;
                                
                if ( (alpha >= lower ) && (beta >= lower) && (gamma >= lower) && (alpha <= upper) && (beta <= upper) && (gamma <= upper) && (lambda >= lower) && (lambda <= upper))
                {       
//                     std::cout << "  Triangle: " << A << " / " << B << " / " << C << std::endl;
//                     std::cout << "    Found intersection: " << alpha/det << " " << beta/det << " " << gamma/det << " " << lambda/det << std::endl;
                    return true;
                }
                else
                    return false;
            }
            
        }
        
        
        
        // adds an element and its neighbour elements to a segment
        //   this method is called recursively on all valid neighbour elements
        //   if a line is shared by more than 2 triangles, the triangle with the smallest inward angle is selected; this ensures a valid segment complex
        //   the triangle with the smallest angle is calculated in this way:
        //     for each boundary line set up a coordinate system in 2D where
        //       the y-vector is the inverse normal of the current triangle
        //       the x-vector is the cross product of the shared line vector and the normal of the current triangle
        //     project the vector from line center to the neighbour triangle center onto this cooridanite system
        //     calculate the oriented angle between the positive x-axis (=vector to the current triangle) and the projected vector to the neighbour triangle
        //   the neighbour triangle with the smallest angle is selected

        template<typename triangle_handle_type, typename domain_type>
        void mark_facing_shortes_angle( domain_type & domain, unsigned int segment_id, triangle_handle_type triangle_handle, bool triangle_faces_outward )
        {
            typedef typename viennagrid::result_of::point_type<domain_type>::type point_type;
            typedef typename viennagrid::result_of::coord_type<point_type>::type coord_type;
            
            typedef typename viennagrid::result_of::element<domain_type, viennagrid::line_tag>::type line_type;
            typedef typename viennagrid::result_of::element<domain_type, viennagrid::triangle_tag>::type triangle_type;
            typedef typename viennagrid::result_of::element_handle<domain_type, viennagrid::vertex_tag>::type vertex_handle_type;
            
            typedef viennagrid::storage::static_array<vertex_handle_type, 3> handle_array;
            
            triangle_type & triangle = viennagrid::dereference_handle(domain, triangle_handle);
            
            // if this triangle has already been visited -> nothing to do
            if (viennamesh::was_visited(triangle))
                return;
            
            // add this triangle to the segment
            viennamesh::add_face_to_segment( domain, triangle, segment_id, triangle_faces_outward );
            // ... and set the visited flag
            viennamesh::set_visited(triangle);
            
            handle_array vtx;
            vtx[0] = viennagrid::elements<viennagrid::vertex_tag>(triangle).handle_at(0);
            vtx[1] = viennagrid::elements<viennagrid::vertex_tag>(triangle).handle_at(1);
            vtx[2] = viennagrid::elements<viennagrid::vertex_tag>(triangle).handle_at(2);
            
            
            viennagrid::storage::static_array<point_type,3> p;
            p[0] = viennagrid::point( domain, vtx[0] );
            p[1] = viennagrid::point( domain, vtx[1] );
            p[2] = viennagrid::point( domain, vtx[2] );
            
            // calculate the center of the triangle
            point_type center = (p[0]+p[1]+p[2])/3.0;
            
            // ... and its normal
            point_type normal = viennagrid::cross_prod( p[1]-p[0], p[2]-p[0] );
            // correcting the normal direction
            if (!triangle_faces_outward) normal = -normal;
            // normalizing the normal vector
            normal /= viennagrid::norm_2(normal);
            
            
        //     std::cout << " --- Triangle " << std::endl;
        //     std::cout << "    adding to segment " << segment_id  << std::endl;
        //     std::cout << "   " << p[0] << std::endl;
        //     std::cout << "   " << p[1] << std::endl;
        //     std::cout << "   " << p[2] << std::endl;
        //     std::cout << "   normal = " << normal << std::endl;
            
            
            
            typedef typename viennagrid::result_of::element_range<triangle_type, viennagrid::line_tag>::type lines_on_triangle_range_type;
            typedef typename viennagrid::result_of::handle_iterator<lines_on_triangle_range_type>::type lines_on_triangle_range_iterator;
            
            // iterating over all boundary lines
            lines_on_triangle_range_type lines = viennagrid::elements<viennagrid::line_tag>( triangle );
            for ( lines_on_triangle_range_iterator lit = lines.begin(); lit != lines.end(); ++lit )
            {
                line_type & line = viennagrid::dereference_handle( domain, *lit );
                
                viennagrid::storage::static_array<vertex_handle_type, 2> lvtx;
                lvtx[0] = viennagrid::elements<viennagrid::vertex_tag>(line).handle_at(0);
                lvtx[1] = viennagrid::elements<viennagrid::vertex_tag>(line).handle_at(1);
                
                viennagrid::storage::static_array<point_type, 2> lp;
                lp[0] = viennagrid::point( domain, lvtx[0] );
                lp[1] = viennagrid::point( domain, lvtx[1] );
                
                
                // calculating the line vector
                point_type line_vector = lp[1]-lp[0];
                // ... and normalizing it
                line_vector /= viennagrid::norm_2(line_vector);
                
                // calculating the center of the boundary line
                point_type line_center = (lp[0]+lp[1]) / 2.0;
                // calculate the vector facing towards the current triangle
                point_type line_to_triangle_vector = viennagrid::cross_prod( -normal, line_vector );
                // ... and normalizing it
                line_to_triangle_vector /= viennagrid::norm_2(line_to_triangle_vector);
                
                // check and correnct the orentietion of the vector facing towards the current triangle
                if (viennagrid::inner_prod(line_to_triangle_vector, center - line_center) < 0)
                    line_to_triangle_vector = -line_to_triangle_vector;
   
                
//                 std::cout << "  line: " << lp[0] << " - " << lp[1] << std::endl;
                
                
                typedef typename viennagrid::result_of::coboundary_range<domain_type, viennagrid::triangle_tag>::type neighbour_range_type;
                typedef typename viennagrid::result_of::handle_iterator<neighbour_range_type>::type neighbour_handle_iterator;
                
                
                neighbour_range_type neighbour_triangles = viennagrid::coboundary_elements<viennagrid::triangle_tag>(domain, *lit);
                
//                 if ( neighbour_triangles.size() != 2 )
//                 {
        //             std::cout << "    INFO - more than two triangle share the line" << std::endl;
        //             std::cout << "ERROR there is something wrong... A line has not 2 coboundary triangles: " << neighbour_triangles.size() << std::endl;
        //             std::cout << viennagrid::point(domain, viennagrid::elements<viennagrid::vertex_tag>(triangle)[0]) << " / " <<
        //                         viennagrid::point(domain, viennagrid::elements<viennagrid::vertex_tag>(triangle)[1])  << " / " <<
        //                         viennagrid::point(domain, viennagrid::elements<viennagrid::vertex_tag>(triangle)[2]) << std::endl;
        //             return ;
//                 }
                
                
                // smallest angle, triangle with smallest angle and facing outward flag of this triangle
                coord_type smallest_angle = std::numeric_limits<coord_type>::max();
                triangle_handle_type smallest_angle_triangle;
                bool smallest_angle_triangle_faces_outward;
                
                // iterating over all coboundary triangles of the current line
                for (neighbour_handle_iterator it = neighbour_triangles.handle_begin(); it != neighbour_triangles.handle_end(); ++it)
                {
                    triangle_handle_type handle = *it;
                    triangle_type & neighbour_triangle = viennagrid::dereference_handle(domain, handle);
                    
        //             std::cout << "  neighbour " << neighbour_triangle << std::endl;
                    
                    // is the coboundary triangle the current triangle -> skipping
                    if (handle == triangle_handle)
                        continue;
                    
        //             if (viennamesh::was_visited(neighbour_triangle))
        //                 continue;
                    
                    handle_array nvtx;
                    nvtx[0] = viennagrid::elements<viennagrid::vertex_tag>(neighbour_triangle).handle_at(0);
                    nvtx[1] = viennagrid::elements<viennagrid::vertex_tag>(neighbour_triangle).handle_at(1);
                    nvtx[2] = viennagrid::elements<viennagrid::vertex_tag>(neighbour_triangle).handle_at(2);
                    
                    viennagrid::storage::static_array<point_type,3> np;
                    np[0] = viennagrid::point( domain, nvtx[0] );
                    np[1] = viennagrid::point( domain, nvtx[1] );
                    np[2] = viennagrid::point( domain, nvtx[2] );

                    // calculating the center of the neighbour triangle
                    point_type neighbour_center = (np[0]+np[1]+np[2])/3.0;
                    // calculating the vector from the line center towards the neighbour triangle
                    point_type line_to_neighbour_triangle_vector = neighbour_center - line_center;
                    // ... and normalizing it
                    line_to_neighbour_triangle_vector /= viennagrid::norm_2(line_to_neighbour_triangle_vector);
                    
                    // projecting the vector facing to the neighbour triangle onto the 2D coordinate system
                    coord_type x = viennagrid::inner_prod( line_to_triangle_vector, line_to_neighbour_triangle_vector );
                    coord_type y = viennagrid::inner_prod( -normal, line_to_neighbour_triangle_vector );
                    
                    // normalizing the 2D vector
                    coord_type tmp = std::sqrt( x*x + y*y );
                    x /= tmp;
                    y /= tmp;
                    
                    
                    
                    // calculate the angle
                    coord_type angle_center_vectors = std::acos( x );
                    // calculate the corrected oriented angle; if y < 0 the angle has to be corrected
                    coord_type oriented_angle = (y > 0) ? angle_center_vectors : 2.0*M_PI - angle_center_vectors;
                    
        //             std::cout << "   --- Neighbour Triangle " << std::endl;
        //             std::cout << "     " << np[0] << std::endl;
        //             std::cout << "     " << np[1] << std::endl;
        //             std::cout << "     " << np[2] << std::endl;
        //             std::cout << "     normal = " << neighbour_normal << std::endl;
        // //             std::cout << "     line_vector = " << line_vector << std::endl;
        //             std::cout << "     vector triangle = " << line_to_triangle_vector << std::endl;
        //             std::cout << "     vector neighbour triangle = " << line_to_neighbour_triangle_vector << std::endl;
        //             std::cout << "     angle orientation = " << angle_orientation << std::endl;
        //             std::cout << "     angle center vectors = " << angle_center_vectors << std::endl;
        //             std::cout << "     orientated angle = " << oriented_angle << std::endl;
                    
                    
                    // if the corrent angle is smaller than the best -> yay! better choice found
                    if (oriented_angle < smallest_angle)
                    {
                        smallest_angle = oriented_angle;
                        smallest_angle_triangle = handle;
                        
                        typename handle_array::iterator vtx_it;
                        typename handle_array::iterator nvtx_it;
                        
                        for (vtx_it = vtx.begin(); vtx_it != vtx.end(); ++vtx_it)
                        {
                            nvtx_it = std::find(nvtx.begin(), nvtx.end(), *vtx_it);
                            if (nvtx_it != nvtx.end())
                                break;
                        }
                        

                        // topology check
                        if ( ( *viennagrid::geometry::circular_next(vtx_it, vtx.begin(), vtx.end()) == *viennagrid::geometry::circular_next(nvtx_it, nvtx.begin(), nvtx.end()) ) ||
                            ( *viennagrid::geometry::circular_prev(vtx_it, vtx.begin(), vtx.end()) == *viennagrid::geometry::circular_prev(nvtx_it, nvtx.begin(), nvtx.end()) ) )
                        {
                            smallest_angle_triangle_faces_outward = !triangle_faces_outward;
                            
                        }
                        else if ( ( *viennagrid::geometry::circular_next(vtx_it, vtx.begin(), vtx.end()) == *viennagrid::geometry::circular_prev(nvtx_it, nvtx.begin(), nvtx.end()) ) ||
                            ( *viennagrid::geometry::circular_prev(vtx_it, vtx.begin(), vtx.end()) == *viennagrid::geometry::circular_next(nvtx_it, nvtx.begin(), nvtx.end()) ) )
                        {
                            smallest_angle_triangle_faces_outward = triangle_faces_outward;
                        }
                        else
                        {
                            std::cout << "Something went wrong..." << std::endl;
                        }
                    }
                }
                
                // is a triangle found -> call mark_facing_shortes_angle recursively
                if (smallest_angle != std::numeric_limits<coord_type>::max())
                {
//                     triangle_type & best = viennagrid::dereference_handle(domain, smallest_angle_triangle);
//                     
//                     handle_array nvtx;
//                     nvtx[0] = viennagrid::elements<viennagrid::vertex_tag>(best).handle_at(0);
//                     nvtx[1] = viennagrid::elements<viennagrid::vertex_tag>(best).handle_at(1);
//                     nvtx[2] = viennagrid::elements<viennagrid::vertex_tag>(best).handle_at(2);
//                     
//                     viennagrid::storage::static_array<point_type,3> np;
//                     np[0] = viennagrid::point( domain, nvtx[0] );
//                     np[1] = viennagrid::point( domain, nvtx[1] );
//                     np[2] = viennagrid::point( domain, nvtx[2] );
                    
        //             std::cout << "   Best neighbour triangle" << std::endl;
        //             std::cout << "       " << np[0] << std::endl;
        //             std::cout << "       " << np[1] << std::endl;
        //             std::cout << "       " << np[2] << std::endl;
        //             std::cout << "       faces outward = " << smallest_angle_triangle_faces_outward << std::endl;
                    
                    mark_facing_shortes_angle( domain, segment_id, smallest_angle_triangle, smallest_angle_triangle_faces_outward );
                }
                
            }
        }
        
        
        
        // this function detects and marks a face segment based on a seed point within it
        //   is searches for all triangle which vector from its center towards the seed points does not intersect any other triangle
        //   these triangles is for sure in the this segment
        //   mark_facing_shortes_angle is called on those triangles to complete the segments
        
        template< typename domain_type, typename vector_type >
        void detect_and_mark_face_segment( domain_type & domain, unsigned int segment_index, vector_type seed_point )
        {
            typedef typename viennagrid::result_of::point_type<domain_type>::type point_type;
            typedef typename viennagrid::result_of::coord_type<point_type>::type coord_type;
            
            typedef typename viennagrid::result_of::element<domain_type, viennagrid::triangle_tag>::type triangle_type;
            typedef typename viennagrid::result_of::element_handle<domain_type, viennagrid::triangle_tag>::type triangle_handle_type;
            typedef typename viennagrid::result_of::element_range<domain_type, viennagrid::triangle_tag>::type triangle_range_type;
            typedef typename viennagrid::result_of::handle_iterator<triangle_range_type>::type triangle_range_handle_iterator;
            typedef typename viennagrid::result_of::iterator<triangle_range_type>::type triangle_range_iterator;
            
            // iteratin over all triangles
            triangle_range_type triangles = viennagrid::elements<viennagrid::triangle_tag>( domain );
            for (triangle_range_handle_iterator it = triangles.handle_begin(); it != triangles.handle_end(); ++it)
            {
                triangle_type & triangle = viennagrid::dereference_handle( domain, *it );
                
                // has this triangle already been visited? -> skipping
                if (viennamesh::was_visited(triangle))
                    continue;
                
                point_type const & p0 = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(triangle)[0] );
                point_type const & p1 = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(triangle)[1] );
                point_type const & p2 = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(triangle)[2] );
                
//                 std::cout << std::endl << std::endl << std::endl;
//                 std::cout << "Triangle: " << p0 << " / " << p1 << " / " << p2 << std::endl;
                
                // calculating the center of the triangle
                point_type r = (p0+p1+p2)/3.0;
                
                // calculating the normale vector of the triangle
                point_type n = viennagrid::cross_prod( p1-p0, p2-p0 );
                // ... and normalizing it
                n /= viennagrid::norm_2(n);
                
                // calculating the ray vector from the center of the triangle the the seed point
                point_type d = seed_point - r;
                
//                 std::cout << " Seed point: " << seed_point << std::endl;
//                 std::cout << " Triangle center: " << r << std::endl;
//                 std::cout << " vector to seed point: " << d << std::endl;
                
                // projecting the normalized ray vector onto the normal vector
                coord_type p = viennagrid::inner_prod( d, n ) / viennagrid::norm_2(d);
                
                // if the projection if near zero (happens when the ray vector and the triangle are co-linear) -> skip this triangle
                if ( std::abs(p) < 1e-6 )
                {
    //                 std::cout << " Line from triangle center to seed point is orthogonal to triangle normal -> skipping" << std::endl;
                    continue;
                }
                
                // if the projection if negative, the triangle faces outward
                bool faces_outward = p < 0;
                
                bool is_inside = false;
                // iterating over all triangles and check for intersection
                triangle_range_iterator jt = triangles.begin();
                for (; jt != triangles.end(); ++jt)
                {
                    if (it == jt) continue; // no self intersect test
                    
                    triangle_type const & to_test = *jt;
                    
                    point_type const & A = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(to_test)[0] );
                    point_type const & B = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(to_test)[1] );
                    point_type const & C = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(to_test)[2] );
                    
//                     std::cout << "  Triangle intersection test: " << A << " / " << B << " / " << C << std::endl;
                    
                    if (utils::triangle_ray_intersect(r, d, A, B, C)) // TODO: scale by bounding box to ensure a ray outside the mesh
                    {
//                         std::cout << "    INTERSECTION!" << std::endl;
                        break;
                    }
                    else
                    {
//                         std::cout << "    no intersection" << std::endl;
                    }
                }
                
                // if there was no intersection -> mark this triangle and all neighbour triangles recursively
                if (jt == triangles.end())
                {
                    if (!faces_outward) n = -n;
//                     std::cout << " YAY! triangle has visible line to seed point " << seed_point << " " << n << std::endl;
                    
    //                 viennamesh::add_face_to_segment(triangle, segment_index, faces_outward);
                    utils::mark_facing_shortes_angle( domain, segment_index, *it, faces_outward );
    //                 break;
                }
            }        
            
            
            for (triangle_range_handle_iterator it = triangles.handle_begin(); it != triangles.handle_end(); ++it)
            {
                viennamesh::clear_visited( viennagrid::dereference_handle( domain, *it ) );
            }
        }
        
        
    }
    
    
    
    template< typename domain_type >
    void mark_face_segments( domain_type & domain )
    {
        typedef typename result_of::seed_point_container<domain_type>::type seed_point_container_type;
        seed_point_container_type seed_points = segment_seed_points(domain);
        
        for (typename seed_point_container_type::iterator it = seed_points.begin(); it != seed_points.end(); ++it)
            utils::detect_and_mark_face_segment(domain, it->first, it->second);
    }

    
    
}

#endif