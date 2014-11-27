#include "viennamesh/algorithm/remove_overlapping_surfaces.hpp"

#include "viennagrid/algorithm/inner_prod.hpp"
#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/volume.hpp"

#include "viennagrid/mesh/neighbor_iteration.hpp"

#include "viennagrid/io/vtk_writer.hpp"

#include <stdlib.h>
#include <time.h>


namespace viennamesh
{


  template<typename ConnectorTagT, typename MeshT, typename ElementT, typename IDAccessorT, typename NumericConfigT>
  void recursively_mark_neighbours( MeshT const & mesh,
                                    ElementT const & element,
                                    int id,
                                    IDAccessorT & id_accessor,
                                    NumericConfigT nc )
  {
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::coord<MeshT>::type CoordType;

    if (id_accessor(element) != -1)
    {
//       std::cout << "This should no happen!" << std::endl;
      return;
    }

    PointType base_normal = viennagrid::normal_vector(element);
    base_normal /= viennagrid::norm_2(base_normal);

    id_accessor(element) = id;

    typedef typename viennagrid::result_of::const_neighbor_range<MeshT, ElementT, ConnectorTagT>::type NeighbourRangeType;
    typedef typename viennagrid::result_of::iterator<NeighbourRangeType>::type NeighbourRangeIterator;


    NeighbourRangeType neighbors(mesh, element);

    for (NeighbourRangeIterator neit = neighbors.begin(); neit != neighbors.end(); ++neit)
    {
      PointType neighbor_normal = viennagrid::normal_vector(*neit);
      neighbor_normal /= viennagrid::norm_2(neighbor_normal);

      double cos_angle = viennagrid::inner_prod(base_normal, neighbor_normal);
//       std::cout << cos_angle << std::endl;

//       if ( std::abs(cos_angle) > 1.0 - viennagrid::detail::absolute_tolerance<CoordType>(nc) )
      if ( cos_angle > 1.0 - viennagrid::detail::absolute_tolerance<CoordType>(nc) )
      {
        recursively_mark_neighbours<ConnectorTagT>(mesh, *neit, id, id_accessor, nc);
      }
    }
  }



  template<typename MeshT, typename IDAcessorT, typename NumericConfigT>
  int mark_surfaces(MeshT const & mesh, IDAcessorT & id_accessor, NumericConfigT normal_tolerance)
  {
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::coord<MeshT>::type CoordType;

    typedef typename viennagrid::result_of::cell<MeshT>::type CellType;
    typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;

    ConstCellRangeType cells(mesh);

    int num_surfaces = 0;
    for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      if (id_accessor(*cit) != -1)
        continue;

      typedef typename viennagrid::result_of::facet_tag<CellType>::type ConnectorTagT;
      recursively_mark_neighbours<ConnectorTagT>(mesh, *cit, num_surfaces++, id_accessor, normal_tolerance);
    }

    return num_surfaces;
  }





  template<typename MeshT, typename IDAccessorT>
  double distance(MeshT const & mesh, IDAccessorT const & id_accessor, int surface_0, int surface_1, int num_distance_points)
  {
    srand (time(NULL));

    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;

    ConstCellRangeType cells(mesh);

    double area_0 = 0.0;

    for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      if (id_accessor(*cit) == surface_0)
        area_0 += viennagrid::volume(*cit);
    }


    double total_distance = 0.0;
    int total_point_count = 0;

    for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      if (id_accessor(*cit) != surface_0)
        continue;

      double current_area = viennagrid::volume(*cit);

//       if (surface_0 == 11)
//         std::cout << "!!!!!!!!!!!!!!!!!!!!!!! " << current_area << " " << area_0 << std::endl;

      int num_points = std::max(static_cast<int>((current_area/area_0)*num_distance_points + 0.5), 1);
      for (int i = 0; i < num_points; ++i)
      {
        double a = static_cast<double>(rand() % 1000001) / 1000000.0;
        double b = static_cast<double>(rand() % 1000001) / 1000000.0;
        double c = static_cast<double>(rand() % 1000001) / 1000000.0;

        double sum = a+b+c;
        a /= sum;
        b /= sum;
        c /= sum;

        PointType p;
        p = viennagrid::point(viennagrid::vertices(*cit)[0]) * a +
            viennagrid::point(viennagrid::vertices(*cit)[1]) * b +
            viennagrid::point(viennagrid::vertices(*cit)[2]) * c;

        double smallest_distance = -1;
        for (ConstCellRangeIterator cit2 = cells.begin(); cit2 != cells.end(); ++cit2)
        {
          if (id_accessor(*cit2) != surface_1)
            continue;

          double tmp_distance = viennagrid::distance( *cit2, p );
          if (smallest_distance < 0 || tmp_distance < smallest_distance)
            smallest_distance = tmp_distance;
        }

        if (smallest_distance < 0)
          std::cout << "Distance < 0 ??" << std::endl;
        else
        {
          total_distance += smallest_distance;
          ++total_point_count;
        }
      }
    }

//     std::cout << "Distance " << surface_0 << " to " << surface_1 << std::endl;
//     std::cout << "  points used = " << num_distance_points << std::endl;
//     std::cout << "  points used (actually) = " << total_point_count << std::endl;
//     std::cout << "  distance = " << total_distance / total_point_count << std::endl;

    return total_distance / total_point_count;
  }







  template<typename MeshT,
           typename SegmentationT,
           typename NumericConfigT>
  void do_something(MeshT const & mesh,
                    MeshT & output_mesh,
                    SegmentationT & output_segmentation,
                    NumericConfigT normal_tolerance,
                    double max_distance_,
                    int distance_point_count_)
  {
    typedef typename viennagrid::result_of::cell_handle<MeshT>::type CellHandleType;
    typedef typename viennagrid::result_of::cell<MeshT>::type CellType;

    std::vector<int> surface_id_container( viennagrid::cells(mesh).size(), -1 );
    typename viennagrid::result_of::accessor<std::vector<int>, CellType>::type surface_id(surface_id_container);

    int num_surfaces = mark_surfaces(mesh, surface_id, normal_tolerance);
    std::cout << "Found " << num_surfaces << " surfaces" << std::endl;


//     distance(mesh, surface_id, 0, 0, 1000);

    std::vector<bool> surface_valid(num_surfaces, true);
    for (int i = 0; i < num_surfaces; ++i)
    {
      for (int j = 0; j < num_surfaces; ++j)
      {
        if (i == j)
          continue;

        if (!surface_valid[j])
          continue;

        double d = distance(mesh, surface_id, i, j, distance_point_count_);
//         std::cout << "Distance " << i << " to " << j << " = " << d << std::endl;

        if (d < max_distance_)
        {
          std::cout << "Distance " << i << " to " << j << " = " << d << std::endl;
          std::cout << "Deleting surface " << i << std::endl;
          surface_valid[i] = false;
          break;
        }
      }
    }



    typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;

    viennagrid::vertex_copy_map<MeshT, MeshT> copy_map(output_mesh);
    ConstCellRangeType cells(mesh);
    for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      if ( surface_valid[surface_id(*cit)] )
      {
        CellHandleType new_cell = copy_map.copy_element(*cit);
        viennagrid::add( output_segmentation(surface_id(*cit)), new_cell );
      }
    }



//     viennagrid::io::vtk_writer<MeshT> writer;
//     writer.add_scalar_data_on_cells( surface_id, "surface_id" );
//     writer(mesh, "surface_id_output");

  }





  template<typename MeshT, typename ElementT, typename IDAccessorT>
  void recursively_mark_neighbour_lines( MeshT const & mesh,
                                         ElementT const & element,
                                         int id,
                                         IDAccessorT & id_accessor )
  {
    typedef typename viennagrid::result_of::cell<MeshT>::type CellType;

    if ( id_accessor(element) != -1 )
      return;

    typedef typename viennagrid::result_of::const_coboundary_range<MeshT, ElementT, CellType>::type CoboundaryRangeType;
    CoboundaryRangeType coboundary_cells(mesh, element);
    if (coboundary_cells.size() > 1)
      return;

    id_accessor(element) = id;

    typedef typename viennagrid::result_of::const_neighbor_range<MeshT, ElementT, viennagrid::vertex_tag>::type NeighbourRangeType;
    typedef typename viennagrid::result_of::iterator<NeighbourRangeType>::type NeighbourRangeIterator;

    NeighbourRangeType neighbor_lines(mesh, element);
    for (NeighbourRangeIterator lit = neighbor_lines.begin(); lit != neighbor_lines.end(); ++lit)
      recursively_mark_neighbour_lines(mesh, *lit, id, id_accessor);
  }


  template<typename MeshT>
  void detect_open_strips(MeshT & mesh)
  {
    typedef typename viennagrid::result_of::line<MeshT>::type LineType;
    typedef typename viennagrid::result_of::line_range<MeshT>::type LineRangeType;
    typedef typename viennagrid::result_of::iterator<LineRangeType>::type LineRangeTypeIterator;

    typedef typename viennagrid::result_of::cell<MeshT>::type CellType;
    typedef typename viennagrid::result_of::cell_range<MeshT>::type CellRangeType;
    typedef typename viennagrid::result_of::iterator<CellRangeType>::type CellRangeTypeIterator;


    LineRangeType lines(mesh);

    std::vector<int> line_strip_id_container( lines.size(), -1 );
    typename viennagrid::result_of::accessor<std::vector<int>, LineType>::type line_strip_ids(line_strip_id_container);

    int id = 0;
    for (LineRangeTypeIterator lit = lines.begin(); lit != lines.end(); ++lit)
    {
      if ( line_strip_ids(*lit) != -1 )
        continue;

      typedef typename viennagrid::result_of::const_coboundary_range<MeshT, LineType, CellType>::type CoboundaryRangeType;
      CoboundaryRangeType coboundary_cells(mesh, *lit);
//       std::cout << coboundary_cells.size() << std::endl
      if (coboundary_cells.size() > 1)
        continue;

//       std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
      recursively_mark_neighbour_lines(mesh, *lit, id++, line_strip_ids);
    }

    std::cout << "Found " << id << " open strips" << std::endl;

  }




  remove_overlapping_surfaces::remove_overlapping_surfaces() :
    input_mesh(*this, parameter_information("mesh","mesh","The input mesh, segmented triangular 3d mesh supported")),
    normal_difference_tolerance(*this, parameter_information("normal_difference_tolerance","double","The tolerance for normal difference"), 1e-6),
    max_distance(*this, parameter_information("max_distance","double","The maximal distance of two surfaces"), 1.0),
    distance_point_count(*this, parameter_information("distance_point_count","int","Point count for distance measurement"), 100),
    output_mesh(*this, parameter_information("mesh", "mesh", "The output mesh, plc 3d mesh")),
    output_segmented_mesh(*this, parameter_information("segmented_mesh", "segmented_mesh", "The output mesh, plc 3d mesh")){}


  std::string remove_overlapping_surfaces::name() const { return "ViennaMesh Surface Whatever"; }
  std::string remove_overlapping_surfaces::id() const { return "remove_overlapping_surfaces"; }


  bool remove_overlapping_surfaces::run_impl()
  {
    typedef viennagrid::triangular_3d_mesh InputMeshType;

    typedef viennagrid::triangular_3d_mesh OutputMeshType;
    typedef viennagrid::result_of::segmentation<OutputMeshType>::type OutputSegmentationType;
    typedef viennagrid::segmented_mesh<OutputMeshType, OutputSegmentationType> OutputSegmentedMeshType;

    viennamesh::result_of::const_parameter_handle<InputMeshType>::type imp = input_mesh.get<InputMeshType>();

    if (imp)
    {
      output_parameter_proxy<OutputMeshType> omp(output_mesh);
      output_parameter_proxy<OutputSegmentedMeshType> segmented_omp(output_segmented_mesh);

      do_something( imp(), segmented_omp().mesh, segmented_omp().segmentation, normal_difference_tolerance(), max_distance(), distance_point_count() );

      viennagrid::copy( segmented_omp().mesh, omp() );

//       do_something( imp(), omp(), normal_difference_tolerance(), max_distance(), distance_point_count() );
      detect_open_strips(omp());

      return true;
    }

    error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
    return false;
  }


}



