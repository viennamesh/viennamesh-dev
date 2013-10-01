#ifndef VIENNAMESH_BASE_CGAL_DATASTRUCTURE_HPP
#define VIENNAMESH_BASE_CGAL_DATASTRUCTURE_HPP

#include "viennamesh/base/datastructure.hpp"
#include "viennagrid/mesh/config.hpp"

namespace viennamesh
{

    template<typename config_type_>
    struct viennagrid_tag
    {
        typedef config_type_ config_type;
    };
    
    typedef viennagrid_tag<viennagrid::config::plc_2d> viennagrid_plc_2d_tag;
    typedef viennagrid_tag<viennagrid::config::plc_3d> viennagrid_plc_3d_tag;
    
    
    namespace result_of
    {
        template<>
        struct cell< viennagrid_tag<viennagrid::config::plc_2d> >
        {
            typedef viennagrid::config::plc_2d_cell type;
        };
        
        template<>
        struct mesh< viennagrid_tag<viennagrid::config::plc_2d> >
        {
            typedef viennagrid::config::plc_2d_mesh type;
        };
        
        
        
        template<>
        struct cell< viennagrid_tag<viennagrid::config::plc_3d> >
        {
            typedef viennagrid::config::plc_3d_cell type;
        };
        
        template<>
        struct mesh< viennagrid_tag<viennagrid::config::plc_3d> >
        {
            typedef viennagrid::config::plc_3d_mesh type;
        };
        
        
    }
}

#endif