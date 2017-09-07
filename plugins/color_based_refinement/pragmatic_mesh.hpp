#ifndef VIENNAMESH_ALGORITHM_PRAGMATIC_MESH_HPP
#define VIENNAMESH_ALGORITHM_PRAGMATIC_MESH_HPP

#include "viennameshpp/plugin.hpp"
#include "Mesh.h"
#include "external/PragmaticWrapper.h"

namespace viennamesh
{
    namespace pragmatic_wrapper
    {
        //typedef Mesh<double> pragmatic_mesh;
        typedef PragmaticWrapper mesh;
    }

    viennamesh_error convert(viennagrid::mesh const & input, pragmatic_wrapper::mesh & output);
    viennamesh_error convert(pragmatic_wrapper::mesh const & input, viennagrid::mesh & output);

    template<>
    viennamesh_error internal_convert<viennagrid_mesh, pragmatic_wrapper::mesh>(viennagrid_mesh const & input, pragmatic_wrapper::mesh & output);
    template<>
    viennamesh_error internal_convert<pragmatic_wrapper::mesh>(pragmatic_wrapper::mesh const & input, viennagrid_mesh & output);

    namespace result_of
    {
        template<>
        struct data_information<pragmatic_wrapper::mesh>
        {
            static std::string type_name() { return "pragmatic_mesh"; }
            static viennamesh_data_make_function make_function() { return viennamesh::generic_make<pragmatic_wrapper::mesh>; }
            static viennamesh_data_delete_function delete_function() { return viennamesh::generic_delete<pragmatic_wrapper::mesh>; }
        };
    }
}
#endif