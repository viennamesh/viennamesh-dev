/* ============================================================================
   Copyright (c) 2011-2016, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */


#include "cgal_mesh.hpp"
#include <CGAL/Polyhedron_incremental_builder_3.h>


namespace viennamesh
{
    //ViennaGrid typedefs
    typedef viennagrid::mesh                                                        MeshType;

    typedef viennagrid::result_of::element<MeshType>::type                          VertexType;
    typedef viennagrid::result_of::element<MeshType>::type                          FacetType;
    typedef viennagrid::result_of::element<MeshType>::type                          TriangleType;
    typedef viennagrid::result_of::element<MeshType>::type ElementType;

    typedef viennagrid::result_of::const_cell_range<MeshType>::type                 ConstTriangleRange;
    typedef viennagrid::result_of::iterator<ConstTriangleRange>::type               ConstTriangleIterator;
    typedef viennagrid::result_of::const_neighbor_range<MeshType, 1, 2>::type       ConstNeighborRange;
    typedef viennagrid::result_of::iterator<ConstNeighborRange>::type               ConstNeighborIterator;

    typedef viennagrid::result_of::const_vertex_range<MeshType>::type               ConstVertexRangeType;
    typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type             ConstVertexIteratorType;

    typedef viennagrid::result_of::const_element_range<MeshType, 2>::type           ConstCellRangeType;

    typedef viennagrid::result_of::const_facet_range<TriangleType>::type            ConstEdgeOnTriangleRange;
    typedef viennagrid::result_of::iterator<ConstEdgeOnTriangleRange>::type         ConstEdgeOnTriangleIterator;

    typedef viennagrid::result_of::const_vertex_range<TriangleType>::type           ConstVertexOnTriangleRange;
    typedef viennagrid::result_of::iterator<ConstVertexOnTriangleRange>::type       ConstVertexOnTriangleIterator;

    /*
     * Class to provide a functor for building a CGAL triangular mesh. GAL uses the half edge data structure.
    */
    template <class HDS>
    class Build_triangle_mesh : public CGAL::Modifier_base<HDS>
    {
    public:

        /*
         * points ... Coordinates of vertices (as they are stored in ViennaGrid data structure) are given in the format:
         *      point1_x, point1_y, point1_z, point2_x, point2_y, point2_z, point3_x, ...
         *
         * facets ... Contains the vertex id of the vertices which form a triangle, format:
         *      triangle1_vertex1_id, triangle1_vertex2_id, triangle1_vertex3_id, triangle2_vertex1_id, ...
        */
        Build_triangle_mesh(const std::vector<double> points, const std::vector<int> facets) : points(points), facets(facets) {}
        void operator()( HDS& hds)
        {
            typedef cgal::Point_3  Point;

            // Postcondition: hds is a valid polyhedral surface.
            CGAL::Polyhedron_incremental_builder_3<HDS> Builder( hds, true);

            //The triangle mesh formes one surface/segment, which is initialized here
            Builder.begin_surface( points.size() / 3, facets.size() / 3);


            //adding vertex coordinates (from ViennaGrid) to CGAL vertex list
            for(std::size_t i = 0; i < points.size()/3; ++i)
            {
                Builder.add_vertex( Point(points[3*i], points[3*i+1], points[3*i+2]));

            }




            /*number of triangle, which failed to be added because none of the both possible orientation ensures a valid half
             * edge data structure (This is mostly due to topological or numeric problems.*/
            int failed_triangle_cnt = 0;

            /* Triangles are added to the surface. If a triangle's orientation does not fit to the already present half edge
             * data structre it is flipped by choosing an anticyclic permutation of the vertices.*/
            for(std::vector<int>::const_iterator it = facets.begin(); it < facets.end(); it = it+3)
            {

                if(Builder.test_facet(it, it+3) == true) //CGAL function that tests the current triangle orientation
                {
                    Builder.add_facet (it,it+3);
                }
                else //choose anticyclic vertex permutation
                {
                    int tmp_facet[3];

                    tmp_facet[0] = *it;
                    tmp_facet[1] = *(it+2);
                    tmp_facet[2] = *(it+1);

                    if(Builder.test_facet(tmp_facet, tmp_facet + 3) == false) //test again
                    {
                        error(1) <<  "Triangle insertion failed!" << std::endl;

                        ++failed_triangle_cnt;
                    }
                    else
                    {
                        Builder.add_facet (tmp_facet, tmp_facet + 3);
                    }
                }
            }

            Builder.end_surface();

            if(failed_triangle_cnt > 0)
            {
                info(5) << "CGAL half edge data structure built, but "<< failed_triangle_cnt << " triangles were not included (maybe due to topological reasons)."  << std::endl;
            }
            else
            {
                info(5) << "CGAL half edge data structure successfully built" << std::endl;
            }

        }
    private:
        const std::vector<double> points;
        const std::vector<int> facets;
    };


    /* Creates a vector of triangles with the property that a triangle has at least one edge in common with any of its predecessors.
     * Uses recursion, potential STACK OVERFLOW! Use only for small meshes.
     *
     * Currently not used for cgal mesh creation.
     * */
    void build_triangles_by_neighbor(const MeshType& mesh, TriangleType& tri, std::vector<TriangleType>& triangles, std::set<int>& triangle_visited)
    {

        triangles.push_back(tri);
        triangle_visited.insert(tri.id().index());


        ConstNeighborRange cr(mesh, tri);

        for(ConstNeighborIterator ci = cr.begin(); ci != cr.end(); ++ci)
        {
            if(triangle_visited.find((*ci).id().index()) == triangle_visited.end()) //not found
            {
                TriangleType triangle = *ci;
                build_triangles_by_neighbor(mesh, triangle, triangles, triangle_visited);
            }
        }
    }

    /* Creates a vector of triangles with the property that a triangle has at least one edge in common with any of its predecessors.
     * Carried out using iterations only.
    */
    void build_triangles_by_iteration(const MeshType& mesh,  std::vector<TriangleType>& triangles)
    {
        std::set<int> visited_edges; // edges that form parts of the boundary of already visited triangles
        std::vector<TriangleType> remaining_triangles; //triangles which do not fulfill the above given requirement are collected here

        remaining_triangles.reserve(triangles.size()/10);


        //first iteration through all triangles in mesh as provided by ViennaGrid data structure
        ConstTriangleRange tr(mesh);
        for(ConstTriangleIterator it = tr.begin(); it != tr.end(); ++it)
        {

            //check if triangle is degenerate
            typedef viennagrid::result_of::point<MeshType>::type PointType;

            //   for a specific triangle get all vertices
            PointType points[3];
            ConstVertexOnTriangleRange votr(*it);

            for(std::size_t i = 0; i < 3; ++i)
            {
                points[i] = viennagrid::get_point(mesh, votr[i]);
            }

            if((points[0] == points[1]) || (points[0] == points[2]) || (points[1] == points[2]))
            {
                //triangle is degenerate, ignore it by not adding it to triangles vector, print it to info(5)?
                //info(5) << "Degerate Triangle encountered: " << points[0] << ", " << points[1] << ", " << points[2] << "--> ignored for triangle list" << std::endl;
                continue;
            }
            //degeneracy check end


            bool is_bounding_edge = false; //is edge part of the boundary of any triangle already transversed?

            ConstEdgeOnTriangleRange er(*it);
            for(ConstEdgeOnTriangleIterator eit = er.begin(); eit != er.end(); ++eit)
            {
                if((visited_edges.find((*eit).id().index()) != visited_edges.end()) || visited_edges.empty() ) //suitable new triangle found
                {
                    is_bounding_edge = true;

                    triangles.push_back(*it);
                    break;
                }
            }

            if(is_bounding_edge == true)
            {
                //add all boundary edges of the suitable triangle to the visited edges
                for(ConstEdgeOnTriangleIterator eit = er.begin(); eit != er.end(); ++eit)
                {
                    visited_edges.insert((*eit).id().index());
                }
            }
            else
            {
                remaining_triangles.push_back(*it);
            }
        }

        //the remaining triangles are added by repeatedly iterating through remaining_triangles
        while(!remaining_triangles.empty()) //not empty
        {
            //iterator increment is provided at the end because vector elements are possibly erased
            for(std::vector<TriangleType>::const_iterator vit = remaining_triangles.begin(); vit != remaining_triangles.end();)
            {
                bool is_bounding_edge = false; //is edge part of the boundary of any triangle already transversed?

                ConstEdgeOnTriangleRange er(*vit);
                for(ConstEdgeOnTriangleIterator eit = er.begin(); eit != er.end(); ++eit)
                {
                    if((visited_edges.find((*eit).id().index()) != visited_edges.end()) || visited_edges.empty())
                    {
                        is_bounding_edge = true;

                        triangles.push_back(*vit);
                        break;
                    }
                }


                if(is_bounding_edge == true)
                {
                    for(ConstEdgeOnTriangleIterator eit = er.begin(); eit != er.end(); ++eit)
                    {
                        visited_edges.insert((*eit).id().index());
                    }

                    vit = remaining_triangles.erase(vit); //erase returns iterator to the element that follows the one to erase
                }
                else
                {
                    ++vit;
                }
            }
        }
    }

    /* Converts a triangular surface mesh given in ViennaGrid data structure to CGAL half edge data structure.
     * In order to achieve a valid half edge data structure degenerated triangles are ignored and for each triangle orientation
     * (which is defined by the order of vertices) is set appropriately.
     *
     * Conversion involves 3 steps:
     *  1. Collect all points in mesh --> vector points
     *  2. Transverse all triangles in mesh in such a way that the following property is fulfilled:
     *       the next triangle added has at least one edge in common with any of its predecessors. --> build_triangles_by_iteration()
     *  3. Build CGAL half edge data structure by iterating through the triangles as given in 2 and change orientation if necessary
     *          --> functor of type Build_triangle_mesh
     * */
    viennamesh_error convert(viennagrid::mesh const & input, cgal::polyhedron_surface_mesh & output)
    {


        typedef cgal::polyhedron_surface_mesh::HalfedgeDS HalfedgeDS;

        //points in mesh, sequence is given by ViennaGrid data structure; format: point1_x, point1_y, point1_z, point2_x, ...
        std::vector<double> points;

        ConstVertexRangeType vertices(input);
        points.reserve(vertices.size()*3);


        int index=0;
        for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
        {
            points.push_back(viennagrid::get_point(input, *vit)[0]);
            points.push_back(viennagrid::get_point(input, *vit)[1]);
            points.push_back(viennagrid::get_point(input, *vit)[2]);
        }


        // vector of triangles with the property that a triangle has at least one edge in common with any of its predecessors.
        std::vector<TriangleType> triangles;

        // vector of vertex IDs; format: triangle1_vertex1, triangle1_vertex2, triangle1_vertex3, triangle2_vertex1, ...
        std::vector<int> faces;


        ConstCellRangeType ccr(input);
        triangles.reserve(ccr.size());



        info(5) << "\nBuilding triangle list for CGAL half edge data structure creation" << std::endl;

        build_triangles_by_iteration(input, triangles);



        faces.reserve(triangles.size()*3);

        //In order to build CGAL half edge data structure for all triangles the IDs of its vertices are collected
        for(std::size_t i= 0; i < triangles.size(); ++i)
        {
            ConstVertexOnTriangleRange boundary_vertices( triangles[i]);

            for (ConstVertexOnTriangleIterator vit = boundary_vertices.begin(); vit != boundary_vertices.end(); ++vit)
            {
                faces.push_back((*vit).id().index());
            }
        }

        info(5) << "Generate CGAL half edge data structure" << std::endl;

        Build_triangle_mesh<HalfedgeDS> triangle_mesh(points, faces);
        output.delegate(triangle_mesh); //triangle mesh validity is checked (postcondition)

        return VIENNAMESH_SUCCESS;
    }


    int get_id_of_vertex(cgal::Point_3 p, cgal::polyhedron_surface_mesh const & input)
    {
        typedef cgal::polyhedron_surface_mesh::Vertex_const_iterator Vertex_iterator;
        int pos =0;
        {
            Vertex_iterator begin = input.vertices_begin();
            for ( ; begin != input.vertices_end(); ++begin, ++pos)
                if(begin->point()==p)
                    return pos;
        }
        return -1;
    }

    viennamesh_error convert(cgal::polyhedron_surface_mesh const & input, viennagrid::mesh & output)
    {
        typedef cgal::polyhedron_surface_mesh::Vertex_const_iterator	Vertex_iterator;
        typedef cgal::polyhedron_surface_mesh::Facet_const_iterator		Facet_iterator;
        int numberofpoints =0;

        numberofpoints = input.size_of_vertices();

        // create EMPTY vector containining Viennagrid Vertices with size numberofpoints
        //std::map<CGAL_POINT_ID_TYPE, VertexType> vertex_handles; // ToDo (Florian)
        std::vector<VertexType> vertex_handles(numberofpoints);


        // iterate through all CGAL vertices and store the coordinates in the viennagrid vertices for each
        {
            Vertex_iterator v = input.vertices_begin();
            for (int i = 0; i < numberofpoints; ++i, ++v)
            {
                //vertex_handles[ CGAL_ID(v->point()) ] = viennagrid::make_vertex( output,	// ToDo (Florian)

                vertex_handles[i] = viennagrid::make_vertex( output,
                                    viennagrid::make_point(v->point().x(),v->point().y(),v->point().z())
                                                           );


            }
        }

        // interate through all CGAL Facets and create a Viennagrid triangle each time
        Facet_iterator f = input.facets_begin();
        for ( ; f != input.facets_end(); ++f)
        {


            viennagrid::make_triangle(
                output,
                //vertex_handles[CGAL_ID(begin->facet_begin()->vertex()] // ToDo (Florian)
                vertex_handles[get_id_of_vertex(f->facet_begin()->vertex()->point(),input)],
                vertex_handles[get_id_of_vertex(f->facet_begin()->next()->vertex()->point(),input)],
                vertex_handles[get_id_of_vertex(f->facet_begin()->opposite()->vertex()->point(),input)]
            );
        }


        //------------------------------------------------------------
        //---------------------------- END ---------------------------
        //------------------------------------------------------------

        return VIENNAMESH_SUCCESS;
    }

    template<>
    viennamesh_error internal_convert<viennagrid_mesh, cgal::polyhedron_surface_mesh>(viennagrid_mesh const & input, cgal::polyhedron_surface_mesh & output)
    {
        return convert( input, output );
    }

    template<>
    viennamesh_error internal_convert<cgal::polyhedron_surface_mesh, viennagrid_mesh>(cgal::polyhedron_surface_mesh const & input, viennagrid_mesh & output)
    {
        viennagrid::mesh output_pp(output);
        return convert( input, output_pp );
    }
}
