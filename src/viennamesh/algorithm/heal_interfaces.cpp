#include "viennamesh/algorithm/heal_interfaces.hpp"

#include "viennagrid/algorithm/detail/numeric.hpp"
#include "viennagrid/algorithm/refine.hpp"

namespace viennamesh
{


  template<typename PointT, typename NumericConfigT>
  bool point_line_intersect(PointT const & point,
                            PointT const & line_start, PointT const & line_end,
                            NumericConfigT nc)
  {
    typedef typename viennagrid::result_of::coord<PointT>::type CoordType;

    PointT dir = line_end - line_start;

    if (viennagrid::norm_2(dir) < viennagrid::detail::absolute_tolerance<CoordType>(nc))
      return false;

    PointT rhs = point - line_start;

    CoordType alpha;
    std::size_t i = 0;
    for (; i < dir.size(); ++i)
    {
      if ( std::abs(dir[i]) > viennagrid::detail::absolute_tolerance<CoordType>(nc) )
      {
        alpha = rhs[i]/dir[i];
        break;
      }
    }

    if (i == dir.size())
      return false;

    for (; i < dir.size(); ++i)
    {
      if ( std::abs(dir[i]) < viennagrid::detail::absolute_tolerance<CoordType>(nc) )
      {
        if ( std::abs(rhs[i]) > viennagrid::detail::absolute_tolerance<CoordType>(nc) )
          return false;
        else
          continue;
      }

      CoordType cur_alpha = rhs[i]/dir[i];
      if (!viennagrid::detail::is_equal(nc, alpha, cur_alpha))
        return false;
    }

    return 0 < alpha && alpha < 1;
  }




  template<typename InputMeshT, typename OutputMeshT, typename NumericConfigT>
  void heal_point_line_intersections(InputMeshT const & input_mesh, OutputMeshT & output_mesh, NumericConfigT nc)
  {
    typedef typename viennagrid::result_of::cell_tag<InputMeshT>::type CellTag;
    typedef typename viennagrid::result_of::point<InputMeshT>::type PointType;

    typedef typename viennagrid::result_of::line<InputMeshT>::type LineType;
    typedef typename viennagrid::result_of::const_line_range<InputMeshT>::type ConstLineRangeType;
    typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineRangeIterator;

    typedef typename viennagrid::result_of::const_vertex_range<InputMeshT>::type ConstVertexRangeType;
    typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;

    typedef typename viennagrid::result_of::vertex_handle<OutputMeshT>::type OutputVertexHandleType;


    std::deque<bool> line_refinement_tag_container;
    typename viennagrid::result_of::accessor<std::deque<bool>, LineType>::type line_refinement_tag_accessor(line_refinement_tag_container);

    std::deque<OutputVertexHandleType> line_refinement_vertex_handle_container;
    typename viennagrid::result_of::accessor<std::deque<OutputVertexHandleType>, LineType>::type line_refinement_vertex_handle_accessor(line_refinement_vertex_handle_container);


    viennagrid::vertex_copy_map<InputMeshT, OutputMeshT> vertex_map(output_mesh);

    ConstLineRangeType lines(input_mesh);
    ConstVertexRangeType vertices(input_mesh);

    std::size_t line_refine_count = 0;

    for (ConstLineRangeIterator lit = lines.begin(); lit != lines.end(); ++lit)
    {
      PointType const & line_start = viennagrid::point( viennagrid::vertices(*lit)[0] );
      PointType const & line_end = viennagrid::point( viennagrid::vertices(*lit)[1] );

      line_refinement_tag_accessor(*lit) = false;
      for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
      {
        if (vit.handle() == viennagrid::vertices(*lit).handle_at(0) ||
            vit.handle() == viennagrid::vertices(*lit).handle_at(1))
          continue;

        PointType const & point = viennagrid::point(*vit);

        if (point_line_intersect(point, line_start, line_end, nc))
        {
          ++line_refine_count;
          line_refinement_tag_accessor(*lit) = true;
          line_refinement_vertex_handle_accessor(*lit) = vertex_map(*vit);
          break;
        }
      }
    }

    if (line_refine_count > 0)
    {
      viennagrid::simple_refine<CellTag>(input_mesh, output_mesh,
                                         vertex_map,
                                         line_refinement_tag_accessor,
                                         line_refinement_vertex_handle_accessor);
    }

  }












  heal_interfaces::heal_interfaces() :
    input_mesh(*this, parameter_information("mesh","mesh","The input mesh, segmented triangular 3d mesh supported")),
    tolerance(*this, parameter_information("tolerance","double","The tolerance"), 1e-6),
    output_mesh(*this, parameter_information("mesh", "mesh", "The output mesh, plc 3d mesh")) {}


  std::string heal_interfaces::name() const { return "ViennaMesh Interface Healer"; }
  std::string heal_interfaces::id() const { return "heal_interfaces"; }


  bool heal_interfaces::run_impl()
  {
    typedef viennagrid::triangular_3d_mesh InputMeshType;
    typedef viennagrid::triangular_3d_mesh OutputMeshType;

    viennamesh::result_of::const_parameter_handle<InputMeshType>::type imp = input_mesh.get<InputMeshType>();

    if (imp)
    {
      output_parameter_proxy<OutputMeshType> omp(output_mesh);

      heal_point_line_intersections( imp(), omp(), tolerance() );

      return true;
    }

    error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
    return false;
  }


}
