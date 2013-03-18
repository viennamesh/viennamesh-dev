#ifndef VIENNAMESH_DOMAIN_CGAL_PLC_3D_HPP
#define VIENNAMESH_DOMAIN_CGAL_PLC_3D_HPP

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
    struct cgal_plc_3d_element
    {
        typedef CGAL::Exact_predicates_inexact_constructions_kernel     Kernel;
        
        typedef CGAL::Triangulation_vertex_base_2<Kernel>               VertexBase;
        typedef CGAL::Delaunay_mesh_face_base_2<Kernel>                 FaceBase;
        typedef CGAL::Triangulation_data_structure_2<VertexBase, FaceBase> Triangulation_structure;
        
        typedef CGAL::Constrained_Delaunay_triangulation_2<Kernel, Triangulation_structure> CDT;
        
        typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;
        
        typedef CDT::Vertex_handle Vertex_handle;
        typedef CDT::Point Point;
      
        viennagrid::config::point_type_3d center;
        viennagrid::storage::static_array<viennagrid::config::point_type_3d, 2> projection_matrix;
        
        CDT cdt;
        std::list<Point> cgal_list_of_holes;
    };
    
    struct cgal_plc_3d_domain
    {
        typedef std::vector<cgal_plc_3d_element> cell_container;
        cell_container cells;
    };
    
    
    
    

    
    

    
    
    
}

#endif