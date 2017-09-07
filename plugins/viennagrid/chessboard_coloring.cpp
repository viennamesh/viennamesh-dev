/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include "chessboard_coloring.hpp"

namespace viennamesh
{

    chessboard_coloring::chessboard_coloring() {}
    std::string chessboard_coloring::name() { return "chessboard_coloring"; }

    bool chessboard_coloring::run(viennamesh::algorithm_handle &)
    {
        viennamesh::info(1) << name() << std::endl;

        //get input mesh and create output mesh
        mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
        mesh_handle output_mesh = make_data<mesh_handle>();

        //Typedefs
        typedef viennagrid::mesh                                                                  MeshType;
        // typedef viennagrid::result_of::element<MeshType>::type                                    VertexType;
        typedef viennagrid::result_of::element<MeshType>::type                                    EdgeType;
        typedef viennagrid::result_of::element<MeshType>::type                                    TriangleType;

        //typedef viennagrid::result_of::element_range<MeshType, 0>::type                           VertexRange;

        typedef viennagrid::result_of::element_range<MeshType, 2>::type                           TriangleRange; 
        typedef viennagrid::result_of::iterator<TriangleRange>::type                              TriangleIterator; 

        typedef viennagrid::result_of::neighbor_range<MeshType, 1, 2>::type                       TriangleNeighborRangeType;
        typedef viennagrid::result_of::iterator<TriangleNeighborRangeType>::type                  TriangleNeighborIterator;
                        
        //get number of vertices and triangles
        int num_vertices = viennagrid::vertex_count(input_mesh());
        int num_triangles = viennagrid::element_count(input_mesh(), 2);
/*
        viennamesh::info(2) << "Number of vertices in mesh : " << num_vertices << std::endl;
        viennamesh::info(2) << "Number of triangles in mesh: " << num_triangles << std::endl;
*/
        //set up accessor
        std::vector<bool> color_triangles(num_triangles, false);
        std::vector<bool> touched (num_triangles, false);

        viennagrid::result_of::accessor<std::vector<bool>, TriangleType>::type color_accessor(color_triangles);

        //set first element to color "black" (BOOL = TRUE)
        color_accessor.set(viennagrid::cells(input_mesh())[0], true);

        //iterate triangles in mesh
        TriangleRange triangles(input_mesh());

        //Iterate over all triangles in the mesh
        viennagrid_element_id * triangle_ids_begin;
        viennagrid_element_id * triangle_ids_end;
        viennagrid_dimension topological_dimension = viennagrid::cell_dimension( input_mesh() );	

        viennagrid_element_id * neighbor_begin;
        viennagrid_element_id * neighbor_end;

        viennagrid_dimension connector = 1;

        viennagrid_mesh_elements_get(input_mesh().internal(), topological_dimension, &triangle_ids_begin, &triangle_ids_end);	

        viennagrid::quantity_field color_field(2,1);
       // viennagrid_quantity_field_create(&color_field);
        color_field.set_name("color");

        //viennagrid_quantity_field_init(color_field, 2, VIENNAGRID_QUANTITY_FIELD_TYPE_NUMERIC, 1 , VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE);

        //get triangles from mesh

        for (viennagrid_element_id *tri = triangle_ids_begin; tri != triangle_ids_end; ++tri)
        {
            int tri_index = viennagrid_index_from_element_id( *tri );

            //std::cout << tri_index << std::endl;

            if ( !touched[tri_index])
            { 
                //color_accessor.set(viennagrid::cells(input_mesh())[tri_index], true);
                color_triangles[tri_index] = true;
                touched[tri_index] = true;

                double clr = 1;

                color_field.set(*tri, clr);

                viennagrid_element_neighbor_elements(input_mesh().internal(), *tri, 0, 2, &neighbor_begin, &neighbor_end);

                for (viennagrid_element_id *n_tri = neighbor_begin; n_tri != neighbor_end; ++n_tri)
                {
                    int n_tri_index = viennagrid_index_from_element_id( *n_tri );

                    //std::cout << "  " << n_tri_index << std::endl;

                    viennagrid_element_id * n_neighbor_begin;
                    viennagrid_element_id * n_neighbor_end;

                    viennagrid_element_neighbor_elements(input_mesh().internal(), *n_tri, 0, 2, &n_neighbor_begin, &n_neighbor_end);

                    for (viennagrid_element_id *n_n_tri = n_neighbor_begin; n_n_tri != n_neighbor_end; ++n_n_tri)
                    {
                        int n_n_tri_index = viennagrid_index_from_element_id( *n_n_tri );

                        if ( !touched[n_n_tri_index])
                        { 
                            //color_accessor.set(viennagrid::cells(input_mesh())[n_tri_index], false);
                            touched[n_n_tri_index] = true;

                            clr = 0;
                            color_field.set(*n_n_tri, clr);
                        }
                    }
                }
            }
        }

        output_mesh = input_mesh;
        quantity_field_handle quantities = make_data<viennagrid::quantity_field>();

        quantities.set(color_field);
   
        set_output("color_field", quantities);
        set_output("mesh", output_mesh());

        return true;
    } //end of bool chessboard_coloring::run(viennamesh::algorithm_handle &)

}