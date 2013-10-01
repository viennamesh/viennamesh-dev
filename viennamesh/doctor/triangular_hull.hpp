#ifndef VIENNAMESH_DOCTOR_TRIANGULAR_HULL_HPP
#define VIENNAMESH_DOCTOR_TRIANGULAR_HULL_HPP

#include "viennagrid/mesh/segmentation.hpp"

#include "viennamesh/utils/utils.hpp"



namespace viennamesh
{

//     template<typename vertex_handle_iterator_type>
//     std::deque< typename std::iterator_traits<vertex_handle_iterator_type>::value_type > common_values(
//         vertex_handle_iterator_type first, vertex_handle_iterator_type const & first_end,
//         vertex_handle_iterator_type const & second_, vertex_handle_iterator_type const & second_end)
//     {
//         std::deque< typename std::iterator_traits<vertex_handle_iterator_type>::value_type > common;
//         
//         for (; first != first_end; ++first)
//             for (vertex_handle_iterator_type second = second_; second != second_end; ++second)
//             {
//     //             std::cout << "      " << *first << " " << *second;
//                 if (*first == *second)
//                 {
//                     common.push_back( *first );
//     //                 std::cout << "   MATCH!";
//                 }
//     //             std::cout << std::endl;
//             }
//             
//         return common;
//     }
// 
// 
// 
//     template<typename hull_mesh_type, typename segmentation_type, typename segment_id_type>
//     bool check_hull_topology( hull_mesh_type const & hull_mesh, segmentation_type const & segmentation, segment_id_type segment_id )
//     {
//         typedef typename viennagrid::result_of::cell_type<hull_mesh_type>::type cell_type;
//         typedef typename viennagrid::result_of::const_cell_range<hull_mesh_type>::type cell_range_type;
//         typedef typename viennagrid::result_of::iterator<cell_range_type>::type cell_range_iterator;
//         
//         typedef typename viennagrid::result_of::const_vertex_handle<hull_mesh_type>::type vertex_const_handle_type;
//         
//         cell_range_type cells = viennagrid::elements(hull_mesh);
//         
//         for (cell_range_iterator it = cells.begin(); it != cells.end(); ++it)
//         {
//             typedef typename viennagrid::result_of::const_neighbour_range<hull_mesh_type, cell_type>::type neighbour_range_type;
//             typedef typename viennagrid::result_of::iterator<neighbour_range_type>::type neighbour_range_iterator;
//             
//             viennagrid::storage::static_array<vertex_const_handle_type, 3> vh;
//             for (int i = 0; i < 3; ++i)
//                 vh[i] = viennagrid::vertices(*it).handle_at(i);        
//             
//             neighbour_range_type neighbours = viennagrid::create_neighbour_elements<viennagrid::line_tag>(hull_mesh, it.handle());
//             if (neighbours.size() != 3)
//             {
//                 std::cout << "TOPOLOGY ERROR!! hull triangle has " << neighbours.size() << " neighbours" << std::endl;
//                 return false;
//             }
//             
//             bool faces_outward = viennagrid::faces_outward_on_segment( segmentation, *it, segment_id );
//             
//             for (neighbour_range_iterator jt = neighbours.begin(); jt != neighbours.end(); ++jt)
//             {
//                 viennagrid::storage::static_array<vertex_const_handle_type, 3> wh;
//                 for (int i = 0; i < 3; ++i)
//                     wh[i] = viennagrid::vertices(*jt).handle_at(i);
//                 
//                 std::deque<vertex_const_handle_type> common_handles = common_values( vh.begin(), vh.end(), wh.begin(), wh.end() );
//                 
//                 
//                 typename viennagrid::storage::static_array<vertex_const_handle_type, 3>::iterator vh0 = std::find( vh.begin(), vh.end(), common_handles[0] );
//                 typename viennagrid::storage::static_array<vertex_const_handle_type, 3>::iterator vh1 = std::find( vh.begin(), vh.end(), common_handles[1] );
//                 
//                 bool orient = viennagrid::geometry::circular_next(vh0, vh.begin(), vh.end()) == vh1;
// 
//                 typename viennagrid::storage::static_array<vertex_const_handle_type, 3>::iterator wh0 = std::find( wh.begin(), wh.end(), common_handles[0] );
//                 typename viennagrid::storage::static_array<vertex_const_handle_type, 3>::iterator wh1 = std::find( wh.begin(), wh.end(), common_handles[1] );
//                 
//                 bool neighbour_orient = viennagrid::geometry::circular_next(wh0, wh.begin(), wh.end()) == wh1;
//                 
//                 bool neighbour_faces_outward = viennagrid::faces_outward_on_segment( segmentation, *jt, segment_id );
//                 
//                 bool face_orient = orient;
//                 if (!faces_outward) face_orient = !face_orient;
//                 
//                 bool face_neighbour_orient = neighbour_orient;
//                 if (!neighbour_faces_outward) face_neighbour_orient = !face_neighbour_orient;
//                 
//                 if (!(face_orient^face_neighbour_orient))
//                 {
//                     std::cout << common_handles.size() << " " << orient << " " << faces_outward << " " << neighbour_orient << " " << neighbour_faces_outward << std::endl;
//                     std::cout << common_handles[0] << " " << common_handles[1] << std::endl;
//                     std::cout << "  " << vh[0] << std::endl;
//                     std::cout << "  " << vh[1] << std::endl;
//                     std::cout << "  " << vh[2] << std::endl;
//                     std::cout << std::endl;
//                     std::cout << "  " << wh[0] << std::endl;
//                     std::cout << "  " << wh[1] << std::endl;
//                     std::cout << "  " << wh[2] << std::endl;
//                     
//                     std::cout << "   passts? " << (face_orient^face_neighbour_orient) << std::endl;
//                     std::cout << std::endl << std::endl;
//                     return false;
//                 }
// 
//             }
//             
//         }
//         
//         return true;
//     }
    
}

#endif