/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
           ViennaFVM - The Vienna Finite Volume Method Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               (add your name here)

   license:    To be discussed, see file LICENSE in the ViennaFVM base directory
======================================================================= */


#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"

#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/neighbour_iteration.hpp"

#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/centroid.hpp"





template<typename from_interpolate_tag, typename to_interpolate_tag, typename mesh_type, typename FromAccessorT, typename ToAccessorT>
void interpolate( mesh_type const & mesh, FromAccessorT const from_accessor, ToAccessorT to_accessor )
{
    typedef typename viennagrid::result_of::coord<mesh_type>::type numeric_type;
    
    typedef typename viennagrid::result_of::const_element_range<mesh_type, to_interpolate_tag>::type to_interpolate_range_type;
    typedef typename viennagrid::result_of::iterator<to_interpolate_range_type>::type to_interpolate_range_iterator;
    
    to_interpolate_range_type to_interpolate_range = viennagrid::elements( mesh );
    for (to_interpolate_range_iterator it = to_interpolate_range.begin(); it != to_interpolate_range.end(); ++it)
    {
        typedef typename viennagrid::result_of::const_coboundary_range<mesh_type, to_interpolate_tag, from_interpolate_tag>::type coboundary_range_type;
        typedef typename viennagrid::result_of::iterator<coboundary_range_type>::type coboundary_range_iterator;
        
        numeric_type value = 0.0;
        numeric_type volume = 0.0;
        
        coboundary_range_type coboundary_elements = viennagrid::coboundary_elements<to_interpolate_tag, from_interpolate_tag>( mesh, it.handle() );
        for (coboundary_range_iterator jt = coboundary_elements.begin(); jt != coboundary_elements.end(); ++jt)
        {
            numeric_type current_volume = viennagrid::volume( *jt );
            volume += current_volume;
            value += current_volume * from_accessor(*jt);
        }
        
        to_accessor(*it) = value / volume;
    }
}




int main(int argc, char **argv)
{
    typedef viennagrid::tetrahedral_3d_mesh             MeshType;
    typedef viennagrid::tetrahedral_3d_segmentation       SegmentationType;

    typedef viennagrid::result_of::vertex<MeshType>::type         VertexType;
    typedef viennagrid::result_of::cell<MeshType>::type           CellType;


    if (argc < 4)
    {
        std::cout << "Usage: mosfet_3d_viennamesh <input_file> <output_file> <interpolate_names ...>" << std::endl;
        return -1;
    }
    
    std::string input_file = argv[1];
    std::string output_file = argv[2];
    
    
    
    MeshType mesh;
    SegmentationType segmentation(mesh);
    
    std::map< std::string, std::deque<double> > cell_values;
    std::map< std::string, std::deque<double> > vertex_values;

    try
    {
        viennagrid::io::vtk_reader<MeshType> reader;
        
        for (int i = 3; i < argc; ++i)
          viennagrid::io::add_scalar_data_on_cells(reader, viennagrid::make_accessor<CellType>(cell_values[argv[i]]), argv[i] );

        reader(mesh, segmentation, input_file);
    }
    catch (...)
    {
        std::cerr << "File-Reader failed. Aborting program..." << std::endl;
        return EXIT_FAILURE;
    }
    
    viennagrid::io::vtk_writer<MeshType> vtk_writer;
    
    for ( std::map< std::string, std::deque<double> >::iterator it = cell_values.begin(); it != cell_values.end(); ++it )
    {
        std::string interpolate_value_name = it->first;
        std::cout << "Interpolating " << interpolate_value_name << std::endl;
        
        viennagrid::result_of::accessor<std::deque<double>, CellType>::type cell_accessor( it->second );
        viennagrid::result_of::accessor<std::deque<double>, VertexType>::type vertex_accessor( vertex_values[it->first] );
        
        interpolate<viennagrid::tetrahedron_tag, viennagrid::vertex_tag>( mesh, cell_accessor, vertex_accessor );
        
        viennagrid::io::add_scalar_data_on_cells(vtk_writer, cell_accessor, interpolate_value_name);
        viennagrid::io::add_scalar_data_on_vertices(vtk_writer, vertex_accessor, interpolate_value_name);
    }
    
    vtk_writer( mesh, segmentation, output_file);
    return 0;
}

