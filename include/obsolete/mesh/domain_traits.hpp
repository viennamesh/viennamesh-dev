#ifndef VIENNAMESH_DOMAIN_TRAITS_HPP
#define VIENNAMESH_DOMAIN_TRAITS_HPP

#include "viennagrid/mesh/config.hpp"


namespace viennamesh
{

    namespace mesh_traits
    {

        template<typename mesh_type>
        struct is_vienna_grid_topologic
        {
            static const bool value = false;
        };

        template<typename container_collection_type_, typename inserter_type_>
        struct is_vienna_grid_topologic< viennagrid::topologic_mesh_t<container_collection_type_,inserter_type_> >
        {
            static const bool value = true;
        };



        template<typename mesh_type>
        struct is_vienna_grid_geometric
        {
            static const bool value = false;
        };

        template<typename vector_type_, typename topologic_mesh_type_, typename metainfo_collection_type_>
        struct is_vienna_grid_geometric< viennagrid::mesh_t<vector_type_,topologic_mesh_type_, metainfo_collection_type_> >
        {
            static const bool value = true;
        };



        template<typename mesh_type>
        struct is_vienna_grid_geometric
        {
            static const bool value = is_vienna_grid_topologic<mesh_type>::value || is_vienna_grid_geometric<mesh_type>::value;
        };



        template<typename point_type>
        struct geometric_dimension
        {
            static const unsigned int value = point_type::dim;
        };

        template<typename vector_type_, typename topologic_mesh_type_, typename metainfo_collection_type_>
        struct geometric_dimension< viennagrid::mesh_t<vector_type_,topologic_mesh_type_, metainfo_collection_type_> >
        {
            static const unsigned int value = geometric_dimension<vector_type_>::value;
        };



        template<typename mesh_type, typename tag>
        struct has_element
        {
            static const bool value = false;
        };

        template<typename container_collection_type_, typename inserter_type_, typename tag>
        struct has_element< viennagrid::topologic_mesh_t<container_collection_type_,inserter_type_> >
        {
            static const bool value = !viennameta::_equal<
                    viennameta::typemap::result_of::find<typename container_collection_type_::typemap, tag>,
                    viennameta::not_found
                >::value;
        };

        template<typename vector_type_, typename topologic_mesh_type_, typename metainfo_collection_type_, typename tag>
        struct has_element< viennagrid::mesh_t<vector_type_,topologic_mesh_type_, metainfo_collection_type_> >
        {
            static const bool value = !viennameta::_equal<
                    viennameta::typemap::result_of::find<typename topologic_mesh_type_::container_collection_type::typemap, tag>,
                    viennameta::not_found
                >::value;
        };

    }

}

#endif
