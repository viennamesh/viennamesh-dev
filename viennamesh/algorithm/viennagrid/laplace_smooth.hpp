#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_LAPLACE_SMOOTH_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_LAPLACE_SMOOTH_HPP

#include "viennagrid/algorithm/angle.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/mesh/element_deletion.hpp"

#include "viennamesh/core/algorithm.hpp"



namespace viennamesh
{

  namespace laplace_smooth
  {

    template<typename MeshT>
    void smooth( MeshT & mesh, double lambda, double max_distance )
    {
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;

      typedef typename viennagrid::result_of::vertex_id<MeshT>::type VertexIDType;
      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;

      typedef typename viennagrid::result_of::line_id<MeshT>::type LineIDType;


      typedef typename viennagrid::result_of::vertex_range<MeshT>::type VertexRangeType;
      typedef typename viennagrid::result_of::iterator<VertexRangeType>::type VertexIteratorType;

      VertexRangeType vertices(mesh);
      for (VertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
      {
        typedef typename viennagrid::result_of::coboundary_range<MeshT, viennagrid::vertex_tag, viennagrid::line_tag>::type CoboundaryVertexRangeType;
        typedef typename viennagrid::result_of::iterator<CoboundaryVertexRangeType>::type CoboundaryLineIteratorType;

        CoboundaryVertexRangeType coboundary_lines(mesh, vit.handle());

        if (coboundary_lines.size() != 2)
          continue;

        PointType point = viennagrid::point(*vit);

        PointType neighbor_point0 = viennagrid::point(viennagrid::vertices(coboundary_lines[0]).handle_at(0) == vit.handle() ?
                                  viennagrid::vertices(coboundary_lines[0])[1] :
                                  viennagrid::vertices(coboundary_lines[0])[0]);
        PointType neighbor_point1 = viennagrid::point(viennagrid::vertices(coboundary_lines[1]).handle_at(0) == vit.handle() ?
                                  viennagrid::vertices(coboundary_lines[1])[1] :
                                  viennagrid::vertices(coboundary_lines[1])[0]);

        PointType line_vector0 = neighbor_point0 - point;
        PointType line_vector1 = neighbor_point1 - point;

        if (max_distance > 0)
          if ( (viennagrid::norm_2(line_vector0) > max_distance) && (viennagrid::norm_2(line_vector0) > max_distance) )
            continue;

        viennagrid::point(*vit) += (line_vector0+line_vector1) / 2.0 * lambda;
      }
    }

    class algorithm : public base_algorithm
    {
    public:

      string name() const { return "ViennaGrid Laplace Smoothing"; }

      template<typename MeshT>
      bool generic_run()
      {
//         typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;

        typedef typename viennamesh::result_of::point< viennagrid::result_of::geometric_dimension<MeshT>::value >::type PointType;
        typedef typename viennamesh::result_of::seed_point_container<PointType>::type SeedPointContainerType;

        double lambda = 0.5;
        copy_input( "lambda", lambda );

        double max_distance = -1;
        copy_input( "max_distance", max_distance );

        typename viennamesh::result_of::const_parameter_handle<MeshT>::type input_mesh = get_input<MeshT>("default");
        if (input_mesh)
        {
          output_parameter_proxy<MeshT> output_mesh = output_proxy<MeshT>( "default" );
          output_mesh() = input_mesh();
          smooth(output_mesh(), lambda, max_distance);

          return true;
        }

        return false;
      }

      bool run_impl()
      {
        if (generic_run<viennagrid::line_2d_mesh>())
          return true;

        if (generic_run<viennagrid::line_3d_mesh>())
          return true;


        error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
        return false;
      }

    private:
    };

  }
}

#endif
