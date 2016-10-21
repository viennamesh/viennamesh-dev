#ifndef VIENNAMESH_STATISTICS_LIBIGL_CONVERT_HPP
#define VIENNAMESH_STATISTICS_LIBIGL_CONVERT_HPP

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

#include <vector>
#include <cmath>

/*
* The mesh comparison metrics are implemented using libigl. libigl uses Eigen matrices to represent vertices and facets.
* In this file the conversion between viennagrid datastructure and libigl matrices is provided.
* All mesh comparison metrics require a mesh without degerate triangles. Therefore a mesh repair function is provided here.
*
* TODO Standardize the use of mesh repair functions.
*/
namespace viennamesh
{

    typedef viennagrid::mesh                                                     MeshT;
    typedef viennagrid::result_of::element<MeshT>::type                          TriangleType;
    typedef  viennagrid::result_of::element<MeshT>::type                          VertexType;

    typedef  viennagrid::result_of::const_cell_range<MeshT>::type                 ConstTriangleRange;
    typedef  viennagrid::result_of::iterator<ConstTriangleRange>::type            ConstTriangleIterator;


    typedef  viennagrid::result_of::const_vertex_range<MeshT>::type               ConstVertexRange;
    typedef  viennagrid::result_of::iterator<ConstVertexRange>::type              ConstVertexIterator;

    typedef  viennagrid::result_of::const_vertex_range<TriangleType>::type        ConstVertexOnTriangleRange;
    typedef  viennagrid::result_of::iterator<ConstVertexOnTriangleRange>::type    ConstVertexOnTriangleIterator;

    typedef  viennagrid::result_of::point<MeshT>::type PointType;

    const double EPS = 1e-12; //for zero_test


    bool zero_test(PointType p) //check if a point is zero with respect to the tolarance EPS
    {
        if( std::fabs(p[0]) < EPS && std::fabs(p[1]) < EPS && std::fabs(p[2]) < EPS   )
            return true;
        else
            return false;
    }

    /*Generates a mesh without degenerate cells from an input mesh
    * This is only a dirty workaround!
    * TODO Efficient implementation immediately after viennagrid vtk reader
    */
    void repair_viennagrid_mesh(MeshT const& input, MeshT const& output)
    {
        std::vector<VertexType> vertices;
        int degenerate_num = 0;


        ConstVertexRange vr(input);
        ConstTriangleRange tr(input);


        vertices.reserve((std::size_t) (vr.size()*0.9));


        int row = 0;
        for(ConstVertexIterator vit = vr.begin(); vit != vr.end(); ++vit, ++row)
        {
            vertices.push_back(viennagrid::make_vertex(output, viennagrid::make_point(viennagrid::get_point(input, *vit)[0], viennagrid::get_point(input, *vit)[1], viennagrid::get_point(input, *vit)[2])));

        }


        row = 0;
        for(ConstTriangleIterator tit = tr.begin(); tit != tr.end(); ++tit, ++row)
        {
            ConstVertexOnTriangleRange votr(*tit);

            //TODO at the moment PointType and VertexType arrays are built, only use one
            PointType points[3];
            for(std::size_t i = 0; i < 3; ++i)
            {
                points[i] = viennagrid::get_point(input, votr[i]);
            }

            if( zero_test(points[0] - points[1]) || zero_test(points[0] - points[2]) || zero_test(points[1] - points[2]) )
            {
                //triangle is degenerate, ignore it by not adding it to triangles vector, print it to info(5)?
                ++degenerate_num;

            }
            else
            {
                VertexType tripoint[3];
                int i = 0;
                for(ConstVertexOnTriangleIterator voti = votr.begin(); voti != votr.end(); ++voti, ++i)
                {
                    tripoint[i] = vertices.at((*voti).id().index());
                }

                viennagrid::make_triangle(output, tripoint[0], tripoint[1], tripoint[2] );
            }
        }

    }

    /*
    * In libigl a mesh is stored by using two matrices: Vertices and Facets. The format of the matrices is the same as the vtu file format uses.
    * Vertices: Every row defines a vertex
    * Facets: Every row defines a cell by using the vertex index (= row in Vertices matrix)
    */
    template<typename NumericT>
    void convert_to_igl_mesh(MeshT const & input1, Eigen::Matrix<NumericT, Eigen::Dynamic, Eigen::Dynamic>& Vertices,
                                         Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>& Facets)
    {
        MeshT input; //repaired input

        Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> Facets_tmp;
        Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> C;

        repair_viennagrid_mesh(input1,input);

        ConstVertexRange vr(input);
        ConstTriangleRange tr(input);

        Vertices.resize(vr.size(), 3);
        Facets_tmp.resize(tr.size(), 3);

        int row = 0;
        for(ConstVertexIterator vit = vr.begin(); vit != vr.end(); ++vit, ++row)
        {
            Vertices(row, 0) = viennagrid::get_point(input, *vit)[0];
            Vertices(row, 1) = viennagrid::get_point(input, *vit)[1];
            Vertices(row, 2) = viennagrid::get_point(input, *vit)[2];

        }

        row = 0;
        for(ConstTriangleIterator tit = tr.begin(); tit != tr.end(); ++tit, ++row)
        {
            ConstVertexOnTriangleRange votr(*tit);

            int col = 0;
            for(ConstVertexOnTriangleIterator voti = votr.begin(); voti != votr.end(); ++voti, ++col)
            {
                Facets_tmp(row, col) = (*voti).id().index();
            }
        }

        // orients all facets cosistently; for the currently metrics this is not absolutely necessary
        igl::bfs_orient(Facets_tmp, Facets, C);

    }
}
#endif
