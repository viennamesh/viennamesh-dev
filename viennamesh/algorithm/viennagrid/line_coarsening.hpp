#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_LINE_COARSENING_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_LINE_COARSENING_HPP

#include "viennagrid/algorithm/angle.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/mesh/element_deletion.hpp"

#include "viennamesh/core/algorithm.hpp"



namespace viennamesh
{

  namespace line_coarsening
  {

    template<typename MeshT>
    void coarsen( MeshT & mesh, double max_angle )
    {
      typedef typename viennagrid::result_of::vertex_id<MeshT>::type VertexIDType;
      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;

      typedef typename viennagrid::result_of::line_id<MeshT>::type LineIDType;


      typedef typename viennagrid::result_of::vertex_range<MeshT>::type VertexRangeType;
      typedef typename viennagrid::result_of::iterator<VertexRangeType>::type VertexIteratorType;

      VertexRangeType vertices(mesh);
      for (VertexIteratorType vit = vertices.begin(); vit != vertices.end();)
      {
        typedef typename viennagrid::result_of::coboundary_range<MeshT, viennagrid::vertex_tag, viennagrid::line_tag>::type CoboundaryVertexRangeType;
        typedef typename viennagrid::result_of::iterator<CoboundaryVertexRangeType>::type CoboundaryLineIteratorType;

        CoboundaryVertexRangeType coboundary_lines(mesh, vit.handle());
        if (coboundary_lines.size() != 2)
        {
          ++vit;
          continue;
        }


        VertexHandleType middle = vit.handle();
        VertexHandleType first = viennagrid::vertices(coboundary_lines[0]).handle_at(0) == middle ?
                                  viennagrid::vertices(coboundary_lines[0]).handle_at(1) :
                                  viennagrid::vertices(coboundary_lines[0]).handle_at(0);
        VertexHandleType last = viennagrid::vertices(coboundary_lines[1]).handle_at(0) == middle ?
                                  viennagrid::vertices(coboundary_lines[1]).handle_at(1) :
                                  viennagrid::vertices(coboundary_lines[1]).handle_at(0);

        double angle = viennagrid::angle( viennagrid::point(mesh, first), viennagrid::point(mesh, last), viennagrid::point(mesh, middle) );
        if (angle > max_angle)
        {
          VertexIDType first_id = viennagrid::dereference_handle(mesh, first).id();
          VertexIDType last_id = viennagrid::dereference_handle(mesh, last).id();

          viennagrid::erase_element(mesh, middle);
          viennagrid::make_line( mesh, viennagrid::find(mesh, first_id).handle(), viennagrid::find(mesh, last_id).handle() );
          vit = vertices.begin();
        }
        else
        {
          ++vit;
        }
      }
    }

    class algorithm : public base_algorithm
    {
    public:

      string name() const { return "ViennaGrid Line Coarsing"; }

      template<typename MeshT, typename SegmentationT>
      bool generic_run()
      {
        typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;

        typedef typename viennamesh::result_of::point< viennagrid::result_of::geometric_dimension<MeshT>::value >::type PointType;
        typedef typename viennamesh::result_of::seed_point_container<PointType>::type SeedPointContainerType;

        double max_angle = 2.5;
        copy_input( "max_angle", max_angle );

        {
          typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type input_mesh = get_input<SegmentedMeshType>("default");
          if (input_mesh)
          {
            output_parameter_proxy<MeshT> output_mesh = output_proxy<MeshT>( "default" );
            MeshT & mesh = output_mesh();
            output_mesh() = input_mesh().mesh;
            coarsen(output_mesh(), max_angle);

            return true;
          }
        }

        {
          typename viennamesh::result_of::const_parameter_handle<MeshT>::type input_mesh = get_input<MeshT>("default");
          if (input_mesh)
          {
            output_parameter_proxy<MeshT> output_mesh = output_proxy<MeshT>( "default" );
            MeshT & mesh = output_mesh();
            output_mesh() = input_mesh();
            coarsen(output_mesh(), max_angle);

            return true;
          }
        }

        return false;
      }

      bool run_impl()
      {
        if (generic_run<viennagrid::line_2d_mesh, viennagrid::line_2d_segmentation>())
          return true;

        if (generic_run<viennagrid::line_3d_mesh, viennagrid::line_3d_segmentation>())
          return true;


        error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
        return false;
      }

    private:
    };

  }
}

#endif
