#ifndef VIENNAMESH_STATISTICS_MESH_COMPARISON_HPP
#define VIENNAMESH_STATISTICS_MESH_COMPARISON_HPP

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




#include "element_metrics.hpp"


//libigl includes
#include <igl/bfs_orient.h>

#include <igl/hausdorff.h>
#include <igl/bounding_box_diagonal.h>
#include <igl/point_mesh_squared_distance.h>

#include <igl/gaussian_curvature.h>
#include <igl/barycentric_coordinates.h>

#include <igl/cotmatrix.h>
#include <igl/massmatrix.h>
#include <igl/invert_diag.h>
#include <igl/principal_curvature.h>


/*
* The class MeshQuality provides implementations of the mesh comparison metrics summarized in
* Zhou, Pang, "Metrics and visualization tools for surface mesh comparison". The provided metrics make only sense for TRIANGULATED surface meshes!
*
* The given implementations rely on libigl and Eigen library. The functionality is provided inside a class
* because point to mesh calculations are expensive and should be only done once (in the constructor).

*/

namespace viennamesh
{

    template<typename NumericT>
    class MeshQuality
    {
    public:

        /*
        * Mesh 1 (Vertices1, Facets1) is the original mesh, Mesh 2 (Vertices2, Facets2) is the mesh whose quality is eventually evaluated.
        */
        MeshQuality(Eigen::Matrix<NumericT, Eigen::Dynamic, Eigen::Dynamic>& Vertices1,
                    Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>& Facets1,
                    Eigen::Matrix<NumericT, Eigen::Dynamic, Eigen::Dynamic>& Vertices2,
                    Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>& Facets2)
        {
            this->Vertices1 = Vertices1;
            this->Facets1 = Facets1;
            this->Vertices2 = Vertices2;
            this->Facets2 = Facets2;


            //contents of I and C are never used later
            Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> I;
            Eigen::Matrix<NumericT, Eigen::Dynamic, Eigen::Dynamic> C;


            /*
            * Very expensive calculations, done here to only do it once!
            * Calculates for every Vertex of mesh 1 (2) the minimum squared distance to mesh 2 (1). The closest triangle (where the nearest
            * point in mesh 2 (1) is located is stored in clostestTriangles2 (I). The closest point itself is stored in closestPoints2 (C).
            */
            igl::point_mesh_squared_distance(Vertices1,Vertices2,Facets2,sqr_D12,closestTriangles2,closestPoints2);
            igl::point_mesh_squared_distance(Vertices2,Vertices1,Facets1,sqr_D21,I,C);

        }

        /*
        * Calculates the Hausdorff distance defined (e.g.) in Guthe, Bordin, Klein "Fast and accurate Hausdorff distance calculation between meshes"
        */
        NumericT hausdorff_distance()
        {
            NumericT bounding_box_diagonal = igl::bounding_box_diagonal(Vertices1);

            NumericT d21 = sqr_D21.maxCoeff();
            NumericT d12 = sqr_D12.maxCoeff();
            return std::sqrt(std::max(d21,d12)) / bounding_box_diagonal;
        }

        /*
        * Similar to Hausdorff distance, but taking the RMS value is not so prone to statistical outliers.
        */
        NumericT min_distance_RMS()
        {
            NumericT bounding_box_diagonal = igl::bounding_box_diagonal(Vertices1);

            NumericT d21 = sqr_D21.mean();
            NumericT d12 = sqr_D12.mean();

            return std::sqrt(std::max(d21, d12)) / bounding_box_diagonal;
        }

        /*
        * Gaussian curvature = product of principal curvatures. N.B. this implementation is extremely sensitive to low area triangles
        */
        NumericT gaussian_curvature()
        {

            Eigen::Matrix<NumericT, Eigen::Dynamic, 1> curvatures1, curvatures2;

            // Compute integral of Gaussian curvature (=angle deficit)
            igl::gaussian_curvature(Vertices1,Facets1,curvatures1);
            igl::gaussian_curvature(Vertices2,Facets2,curvatures2);


            // Compute mass (area) matrix
            Eigen::SparseMatrix<NumericT> M1, M2, Minv1, Minv2;

            igl::massmatrix(Vertices1,Facets1,igl::MASSMATRIX_TYPE_DEFAULT,M1);
            igl::massmatrix(Vertices2,Facets2,igl::MASSMATRIX_TYPE_DEFAULT,M2);

            igl::invert_diag(M1,Minv1);
            igl::invert_diag(M2,Minv2);

            // Divide by area to get integral average = gaussian curvature
            curvatures1 = (Minv1*curvatures1).eval();
            curvatures2 = (Minv2*curvatures2).eval();

            return point_to_point_curvature_diff(curvatures1, curvatures2) / curvatures1.maxCoeff();

        }

        /*
        * Mean curvatures = mean value of principal curvatures. This implementation uses a quadratic surface fitting approach described in
        Panozzo, Puppo Rocca, "Efficient Multi-scale Curvature and Crease Estimation". Shows much less sensitivity to low quality triangles.
        */
        NumericT mean_curvature()
        {
            Eigen::Matrix<NumericT, Eigen::Dynamic, 1> curvatures1, curvatures2;

            // Compute curvature directions via quadric fitting
            Eigen::MatrixXd PD1_1,PD2_1, PD1_2, PD2_2;
            Eigen::VectorXd PV1_1,PV2_1, PV1_2, PV2_2;



            //very expensive calculation
            igl::principal_curvature(Vertices1,Facets1,PD1_1,PD2_1,PV1_1,PV2_1, 2); //curvature eps radius = 2 times mean edgelength


            igl::principal_curvature(Vertices2,Facets2,PD1_2,PD2_2,PV1_2,PV2_2, 2);
            // mean curvature
            curvatures1 = 0.5* (PV1_1 + PV2_1);
            curvatures2 = 0.5* (PV1_2 + PV2_2);

            return point_to_point_curvature_diff(curvatures1, curvatures2) / curvatures1.maxCoeff();


        }


    private:
        Eigen::Matrix<NumericT, Eigen::Dynamic, Eigen::Dynamic> Vertices1, Vertices2;
        Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> Facets1, Facets2;

        Eigen::Matrix<NumericT, Eigen::Dynamic, Eigen::Dynamic> sqr_D12, sqr_D21;

        Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> closestTriangles2; //on mesh2
        Eigen::Matrix<NumericT, Eigen::Dynamic, Eigen::Dynamic> closestPoints2; //on mesh2 with respect to the points in mesh1


        /*
        *Implementation of the 'Point Pair and Differences' method described in Zhou, Pang, "Metrics and visualization tools for surface mesh comparison"
        */

        NumericT point_to_point_curvature_diff(Eigen::Matrix<NumericT, Eigen::Dynamic, 1> curvatures1, Eigen::Matrix<NumericT, Eigen::Dynamic, 1> curvatures2)
        {
            Eigen::Matrix<NumericT, Eigen::Dynamic, 1> bary_curvartures2; //weighted with barycentric coordinates
            Eigen::Matrix<NumericT, Eigen::Dynamic, 3> A,B,C; //closest Triangle vertices
            Eigen::Matrix<NumericT, Eigen::Dynamic, 3> barycentric_coords;
            Eigen::Matrix<NumericT, Eigen::Dynamic, 1> curvature_difference;


            //determine barycentric coordinates
            A.resize(closestTriangles2.rows(), Facets2.cols());
            B.resize(closestTriangles2.rows(), Facets2.cols());
            C.resize(closestTriangles2.rows(), Facets2.cols());

            for(int i = 0; i < closestTriangles2.rows(); ++i)
            {
                int triangle_index = closestTriangles2(i);

                A.row(i) = Vertices2.row(Facets2(triangle_index, 0));
                B.row(i) = Vertices2.row(Facets2(triangle_index, 1));
                C.row(i) = Vertices2.row(Facets2(triangle_index, 2));
            }

            igl::barycentric_coordinates(closestPoints2, A, B, C, barycentric_coords);

            bary_curvartures2.resize(curvatures1.rows(),1);
            curvature_difference.resize(curvatures1.rows(),1);


            for(int i = 0; i < barycentric_coords.rows(); ++i)
            {
                int triangle_index = closestTriangles2(i);

                int vertex_a_index = Facets2(triangle_index, 0);
                int vertex_b_index = Facets2(triangle_index, 1);
                int vertex_c_index = Facets2(triangle_index, 2);


                bary_curvartures2(i,0) = barycentric_coords(i, 0) * curvatures2(vertex_a_index) +
                                         barycentric_coords(i, 1) * curvatures2(vertex_b_index) + barycentric_coords(i, 2) * curvatures2(vertex_c_index) ;

                curvature_difference(i,0) = (bary_curvartures2(i, 0) - curvatures1(i,0))*(bary_curvartures2(i, 0) - curvatures1(i,0));
            }
            return std::sqrt(curvature_difference.mean());

        }
    };
}

#endif
