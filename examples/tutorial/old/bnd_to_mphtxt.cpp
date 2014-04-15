#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/bnd_reader.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/mphtxt_writer.hpp"
#include "viennagrid/mesh/element_creation.hpp"

#include "viennamesh/viennamesh.hpp"

#include "viennamesh/algorithm/viennagrid/extract_plc.hpp"
#include "viennamesh/algorithm/viennagrid/extract_seed_points.hpp"








template<typename SegmentHandleT, typename AccessorType, typename ElementHandleT>
void mark_planar_neighbors(
        SegmentHandleT const & segment, AccessorType & accessor,
        ElementHandleT const & element_handle,
        typename AccessorType::value_type invalid,
        typename viennagrid::result_of::coord<SegmentHandleT>::type tolerance)
{
  typedef typename viennagrid::result_of::segmentation<SegmentHandleT>::type SegmentationType;
  typedef typename viennagrid::result_of::point<SegmentHandleT>::type PointType;
  typedef typename viennagrid::result_of::coord<SegmentHandleT>::type NumericType;

  typedef typename viennagrid::detail::result_of::value_type<ElementHandleT>::type ElementType;

  typedef typename viennagrid::result_of::segment_id_range<SegmentationType, ElementType>::type ElementSegmentIDRangeType;
  ElementSegmentIDRangeType const & segment_id_range = viennagrid::segment_ids(segment.parent(), viennagrid::dereference_handle(segment, element_handle));

  typedef typename viennagrid::result_of::const_neighbor_range<SegmentHandleT, ElementType, viennagrid::line_tag>::type NeigborElementsRangeType;
  typedef typename viennagrid::result_of::iterator<NeigborElementsRangeType>::type NeigborElementsIteratorType;

  PointType normal = viennagrid::normal_vector( viennagrid::dereference_handle(segment, element_handle) );
  typename AccessorType::value_type to_mark = accessor( viennagrid::dereference_handle(segment, element_handle) );

  NeigborElementsRangeType neighbor_elements( segment, element_handle );
  for (NeigborElementsIteratorType neit = neighbor_elements.begin(); neit != neighbor_elements.end(); ++neit )
  {
    if (accessor(*neit) != invalid)
      continue;

    if (!segment_id_range.is_equal(viennagrid::segment_ids(segment.parent(), *neit)))
      continue;

    PointType neighbor_normal = viennagrid::normal_vector(*neit);
    NumericType dot = viennagrid::inner_prod( normal, neighbor_normal );

    if (std::abs(dot) > (1.0-tolerance) * viennagrid::norm_2(normal) * viennagrid::norm_2(neighbor_normal))
    {
      accessor(*neit) = to_mark;
      mark_planar_neighbors( segment, accessor, neit.handle(), invalid, tolerance);
    }
  }
}

template<typename HullElementTypeOrTagT, typename HullMeshT, typename HullSegmentationT, typename PLCMeshT, typename PLCSegmentationT>
void extract_plcs( HullMeshT const & hull_mesh, HullSegmentationT const & hull_segmentation,
                   PLCMeshT & plc_mesh, PLCSegmentationT & plc_segmentation,
                   typename viennagrid::result_of::coord<PLCMeshT>::type tolerance)
{
  typedef typename viennagrid::result_of::point<PLCMeshT>::type PLCPointType;
  typedef typename viennagrid::result_of::coord<PLCMeshT>::type PLCNumericType;

  typedef typename viennagrid::result_of::segment_handle<HullSegmentationT>::type SegmentHandleType;

  typedef typename viennagrid::result_of::element_tag<HullElementTypeOrTagT>::type HullElementTag;
  typedef typename viennagrid::result_of::element<HullMeshT, HullElementTag>::type HullElementType;

  typedef typename viennagrid::result_of::const_element_range<HullMeshT, HullElementTag>::type ConstHullElementRangeType;
  typedef typename viennagrid::result_of::iterator<ConstHullElementRangeType>::type ConstHullElementIteratorType;


  std::vector<int> plc_index_array;
  typename viennagrid::result_of::accessor<std::vector<int>, HullElementType>::type plc_index_accessor(plc_index_array);

  {
    ConstHullElementRangeType hull_elements(hull_mesh);
    for (ConstHullElementIteratorType heit = hull_elements.begin(); heit != hull_elements.end(); ++heit)
      plc_index_accessor( *heit ) = -1;
  }


  std::map<int, std::vector<int> > plc_segment_map;
  int number_of_plcs = 0;
  for (typename HullSegmentationT::const_iterator sit = hull_segmentation.begin(); sit != hull_segmentation.end(); ++sit)
  {
    typedef typename viennagrid::result_of::const_element_range<SegmentHandleType, HullElementTag>::type ConstHullElementOnSegmentRangeType;
    typedef typename viennagrid::result_of::iterator<ConstHullElementOnSegmentRangeType>::type ConstHullElementOnSegmentIteratorType;

    ConstHullElementOnSegmentRangeType hull_elements_on_segment(*sit);
    for (ConstHullElementOnSegmentIteratorType hesit = hull_elements_on_segment.begin(); hesit != hull_elements_on_segment.end(); ++hesit)
    {
      if (plc_index_accessor(*hesit) == -1)
      {
        plc_index_accessor(*hesit) = number_of_plcs;
        mark_planar_neighbors(*sit, plc_index_accessor, hesit.handle(), -1, tolerance);

        typedef typename viennagrid::result_of::segment_id_range<HullSegmentationT, HullElementType>::type SegmentIDRange;
        SegmentIDRange const & segment_ids = viennagrid::segment_ids(hull_segmentation, *hesit);
        for (typename SegmentIDRange::const_iterator siit = segment_ids.begin(); siit != segment_ids.end(); ++siit)
          plc_segment_map[number_of_plcs].push_back( (*siit) );

//         plc_segment_map[number_of_plcs] = (*sit).id();

        ++number_of_plcs;
      }
    }
  }

  std::cout << "Number of PLCS: " << number_of_plcs << std::endl;


  typedef typename viennagrid::result_of::line<HullMeshT>::type HullLineType;

  std::vector<bool> delete_line_array;
  typename viennagrid::result_of::accessor<std::vector<bool>, HullLineType>::type delete_line_accessor(delete_line_array);

  typedef typename viennagrid::result_of::const_line_range<HullMeshT>::type ConstHullLineRangeType;
  typedef typename viennagrid::result_of::iterator<ConstHullLineRangeType>::type ConstHullLineIteratorType;

  {
    ConstHullLineRangeType hull_lines(hull_mesh);
    for (ConstHullLineIteratorType lit = hull_lines.begin(); lit != hull_lines.end(); ++lit)
      delete_line_accessor(*lit) = false;

    for (ConstHullLineIteratorType lit = hull_lines.begin(); lit != hull_lines.end(); ++lit)
    {
      typedef typename viennagrid::result_of::const_coboundary_range<HullMeshT, viennagrid::line_tag, HullElementTag>::type CoboundaryElementsRangeType;
      typedef typename viennagrid::result_of::iterator<CoboundaryElementsRangeType>::type CoboundaryElementsIteratorType;

      CoboundaryElementsRangeType coboundary_triangles(hull_mesh, lit.handle());
      if (coboundary_triangles.size() != 2)
        continue;

      if ( plc_index_accessor(coboundary_triangles[0]) == plc_index_accessor(coboundary_triangles[1]) )
        delete_line_accessor(*lit) = true;
    }
  }



  typedef typename viennagrid::result_of::vertex_id<HullMeshT>::type HullVertexIDType;
  typedef typename viennagrid::result_of::vertex_handle<PLCMeshT>::type PLCVertexHandleType;

  typedef typename viennagrid::result_of::line_id<HullMeshT>::type HullLineIDType;
  typedef typename viennagrid::result_of::line_handle<PLCMeshT>::type PLCLineHandleType;
  typedef typename viennagrid::result_of::line<PLCMeshT>::type PLCLineType;

  typedef typename viennagrid::result_of::const_line_range<HullMeshT>::type HullLineRangeType;
  typedef typename viennagrid::result_of::iterator<HullLineRangeType>::type HullLineIteratorType;

  std::map<HullLineIDType, PLCLineHandleType> lines;
  std::map<HullVertexIDType, PLCVertexHandleType> vertices;

  HullLineRangeType hull_lines(hull_mesh);
  for (HullLineIteratorType hlit = hull_lines.begin(); hlit != hull_lines.end(); ++hlit)
  {
    if (!delete_line_accessor(*hlit))
    {
//       std::vector<PLCLineHandleType> lines_for_current_plc;

      typename std::map<HullLineIDType, PLCLineHandleType>::iterator lit = lines.find( (*hlit).id() );
      if (lit != lines.end())
      {
//         lines_for_current_plc.push_back( lit->second );
      }
      else
      {
        viennagrid::static_array<PLCVertexHandleType, 2> line_vertices;

        for (int i = 0; i < 2; ++i)
        {
          typename std::map<HullVertexIDType, PLCVertexHandleType>::iterator vit = vertices.find( viennagrid::vertices(*hlit)[i].id() );
          if (vit != vertices.end())
            line_vertices[i] = vit->second;
          else
          {
            line_vertices[i] = viennagrid::make_vertex( plc_mesh, viennagrid::point(viennagrid::vertices(*hlit)[i]) );
            vertices[ viennagrid::vertices(*hlit)[i].id() ] = line_vertices[i];
          }
        }

        lines[(*hlit).id()] = viennagrid::make_line( plc_mesh, line_vertices[0], line_vertices[1] );
      }
    }
  }


  std::vector< std::vector<int> > line_plc_vector;
  typename viennagrid::result_of::accessor<std::vector< std::vector<int> >, PLCLineType>::type line_plc_accessor(line_plc_vector);



//   std::vector< std::vector<PLCLineHandleType> > plc_line_handles(number_of_plcs);

  {
    ConstHullElementRangeType hull_elements(hull_mesh);
    for (ConstHullElementIteratorType heit = hull_elements.begin(); heit != hull_elements.end(); ++heit)
    {
      int plc_index = plc_index_accessor( *heit );

      typedef typename viennagrid::result_of::const_line_range<HullElementType>::type ConstLineRangeType;
      typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineIteratorType;

      ConstLineRangeType lines_on_hull_element( *heit );
      for (ConstLineIteratorType lheit = lines_on_hull_element.begin(); lheit != lines_on_hull_element.end(); ++lheit)
      {
        if (!delete_line_accessor(*lheit))
        {
          line_plc_accessor( viennagrid::dereference_handle(plc_mesh, lines[(*lheit).id()]) ).push_back(plc_index);
//           std::cout << "Added line to PLC " << plc_index << std::endl;
//           plc_line_handles[plc_index].push_back( lines[(*lheit).id()] );
        }
      }
    }
  }

  for (std::size_t i = 0; i < line_plc_vector.size(); ++i)
    std::sort( line_plc_vector[i].begin(), line_plc_vector[i].end() );




  {
    typedef typename viennagrid::result_of::vertex_range<PLCMeshT>::type PLCVertexRangeType;
    typedef typename viennagrid::result_of::iterator<PLCVertexRangeType>::type PLCVertexIteratorType;

    PLCVertexRangeType vertices(plc_mesh);
    for (PLCVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      typedef typename viennagrid::result_of::coboundary_range<PLCMeshT, viennagrid::vertex_tag, viennagrid::line_tag>::type PLCCoboundaryLineRangeType;
      typedef typename viennagrid::result_of::iterator<PLCCoboundaryLineRangeType>::type PLCCoboundaryLineIteratorType;

      viennagrid::detail::create_coboundary_information<viennagrid::vertex_tag, viennagrid::line_tag>( plc_mesh );

      PLCCoboundaryLineRangeType coboundary_lines(plc_mesh, vit.handle());
      if (coboundary_lines.size() != 2)
        continue;

      if ( line_plc_accessor(coboundary_lines[0]) != line_plc_accessor(coboundary_lines[1]) )
        continue;

      PLCPointType v0 = viennagrid::point( viennagrid::vertices(coboundary_lines[0])[0]) - viennagrid::point( viennagrid::vertices(coboundary_lines[0])[1]);
      PLCPointType v1 = viennagrid::point( viennagrid::vertices(coboundary_lines[1])[0]) - viennagrid::point( viennagrid::vertices(coboundary_lines[1])[1]);

      PLCNumericType dot = viennagrid::inner_prod(v0, v1);

      if (std::abs(dot) < (1.0-tolerance) * viennagrid::norm_2(v0) * viennagrid::norm_2(v1))
        continue;

//       std::cout << "Deleting Vertex " << *vit << std::endl;
      std::vector<int> line_plcs = line_plc_accessor(coboundary_lines[0]);

      viennagrid::static_array<PLCVertexHandleType, 2> new_line_vertices;

      for (int i = 0; i < 2; ++i)
      {
        if (viennagrid::vertices(coboundary_lines[i]).handle_at(0) == vit.handle())
          new_line_vertices[i] = viennagrid::vertices(coboundary_lines[i]).handle_at(1);
        else
          new_line_vertices[i] = viennagrid::vertices(coboundary_lines[i]).handle_at(0);
      }


      typedef typename viennagrid::result_of::mesh_view<PLCMeshT>::type mesh_view_type;
      mesh_view_type elements_to_erase = viennagrid::make_view(plc_mesh);
      viennagrid::mark_erase_elements( plc_mesh, elements_to_erase, coboundary_lines.handle_at(0) );
      viennagrid::mark_erase_elements( plc_mesh, elements_to_erase, coboundary_lines.handle_at(1) );

      viennagrid::erase_elements(plc_mesh, elements_to_erase);

//       PLCLineHandleType h0 =

//       viennagrid::erase_element(plc_mesh, coboundary_lines.handle_at(0));
//       viennagrid::erase_element(plc_mesh, coboundary_lines.handle_at(1));

      PLCLineHandleType new_line_handle = viennagrid::make_line( plc_mesh, new_line_vertices[0], new_line_vertices[1] );

      line_plc_accessor( viennagrid::dereference_handle(plc_mesh, new_line_handle) ) = line_plcs;

//       vit = vertices.begin();
//       break;
    }
  }



  {
    std::vector< std::vector<PLCLineHandleType> > plc_line_handles(number_of_plcs);

    typedef typename viennagrid::result_of::line_range<PLCMeshT>::type PLCLineRangeType;
    typedef typename viennagrid::result_of::iterator<PLCLineRangeType>::type PLCLineIteratorType;

    PLCLineRangeType lines(plc_mesh);
    for (PLCLineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
    {
      std::vector<int> const & plcs = line_plc_accessor(*lit);
      for (std::size_t i = 0; i < plcs.size(); ++i)
      {
        plc_line_handles[plcs[i]].push_back(lit.handle());
      }
    }


    for (int plc = 0; plc != number_of_plcs; ++plc)
    {
//       std::cout << "Number of lines in PLC " << plc << ":" << plc_line_handles[plc].size() << std::endl;
//
//       for (int i = 0; i < plc_line_handles[plc].size(); ++i)
//       {
//         std::cout << "   " << viennagrid::dereference_handle(plc_mesh, plc_line_handles[plc][i]) << std::endl;
//       }

      typedef typename viennagrid::result_of::cell_handle<PLCMeshT>::type CellHandleType;
      CellHandleType cell_handle = viennagrid::make_plc( plc_mesh, plc_line_handles[plc].begin(), plc_line_handles[plc].end() );

      std::map<int, std::vector<int> >::const_iterator siit = plc_segment_map.find(plc);
      for (std::size_t j = 0; j < siit->second.size(); ++j)
      {
//         std::cout << "  added to segment " << siit->second[j] << std::endl;
        viennagrid::add( plc_segmentation.get_make_segment(siit->second[j]), cell_handle );
      }

//       viennagrid::make_plc( plc_segmentation.get_make_segment(plc_segment_map[plc]), plc_line_handles[plc].begin(), plc_line_handles[plc].end() );
    }
  }




//   {
//     typedef typename viennagrid::result_of::vertex<PLCMeshT>::type PLCVertexType;
//     std::vector<bool> vertex_usage_array( viennagrid::vertices(plc_mesh).size(), false );
//     typename viennagrid::result_of::accessor< std::vector<bool>, PLCVertexType>::type vertex_usage_accessor( vertex_usage_array );
//
//     typedef typename viennagrid::result_of::cell<PLCMeshT>::type CellType;
//     typedef typename viennagrid::result_of::cell_range<PLCMeshT>::type PLCCellRangeType;
//     typedef typename viennagrid::result_of::iterator<PLCCellRangeType>::type PLCCellIteratorType;
//
//     PLCCellRangeType cells(plc_mesh);
//     for (PLCCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
//     {
//       typedef typename viennagrid::result_of::vertex_range<CellType>::type VertexOnPLCCellRangeType;
//       typedef typename viennagrid::result_of::iterator<VertexOnPLCCellRangeType>::type VertexOnPLCCellIteratorType;
//
//       VertexOnPLCCellRangeType vertices_on_cells(*cit);
//       for (VertexOnPLCCellIteratorType vcit = vertices_on_cells.begin(); vcit != vertices_on_cells.end(); ++vcit)
//         vertex_usage_accessor(*vcit) = true;
//     }
//
//     for (int i = 0; i != vertex_usage_array.size(); ++i)
//       std::cout << vertex_usage_array[i] << " ";
//     std::cout << std::endl;
//   }

}




template<typename HullMeshT, typename HullSegmentationT, typename PLCMeshT, typename PLCSegmentationT>
void extract_plcs( HullMeshT const & hull_mesh, HullSegmentationT const & hull_segmentation,
                   PLCMeshT & plc_mesh, PLCSegmentationT & plc_segmentation,
                   typename viennagrid::result_of::coord<PLCMeshT>::type tolerance)
{
  typedef typename viennagrid::result_of::cell_tag<HullMeshT>::type CellTag;
  extract_plcs<CellTag>(hull_mesh, hull_segmentation, plc_mesh, plc_segmentation, tolerance);
}









int main()
{
  typedef viennagrid::segmented_mesh<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> SegmentedTriangleMeshType;

  typedef viennagrid::segmented_mesh<viennagrid::brep_3d_mesh, viennagrid::brep_3d_segmentation> SegmentedBrepMeshType;

  typedef viennagrid::segmented_mesh<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> SegmentedTetrahedronMeshType;

  viennamesh::result_of::parameter_handle<SegmentedTriangleMeshType>::type triangle_mesh = viennamesh::make_parameter<SegmentedTriangleMeshType>();
  viennamesh::result_of::parameter_handle<SegmentedBrepMeshType>::type brep_mesh = viennamesh::make_parameter<SegmentedBrepMeshType>();
//   viennamesh::result_of::parameter_handle<SegmentedTetrahedronMeshType>::type tetrahedron_mesh = viennamesh::make_parameter<SegmentedTetrahedronMeshType>();

  SegmentedTetrahedronMeshType tetrahedron_mesh;




//   {
//     viennagrid::io::bnd_reader reader;
//   reader( triangle_mesh().mesh, triangle_mesh().segmentation, "../data/simple.bnd" );
//   reader( triangle_mesh().mesh, triangle_mesh().segmentation, "../data/test.bnd" );
//   reader( triangle_mesh().mesh, triangle_mesh().segmentation, "../data/tsv_3d_chopped.bnd" );
//   }

//   {
//     viennagrid::io::vtk_writer<viennagrid::triangular_3d_mesh> writer;
//     writer( triangle_mesh().mesh, triangle_mesh().segmentation, "bnd_test" );
//   }

  {
    viennagrid::io::vtk_reader<viennagrid::triangular_3d_mesh> writer;
    writer( triangle_mesh().mesh, triangle_mesh().segmentation, "bnd_test_main.pvd" );
  }



//   viennamesh::seed_point_3d_container seed_points;
//   viennamesh::extract_seed_points::extract_seed_points( triangle_mesh().mesh, triangle_mesh().segmentation, seed_points );
//   std::cout << "Number of seed points: " << seed_points.size() << std::endl;


//   std::cout << "Number of cells in Mesh: " << viennagrid::cells( triangle_mesh().mesh ).size() << std::endl;
//   std::cout << "Number of cells in Segment 0: " << viennagrid::cells( triangle_mesh().segmentation.get_segment(0) ).size() << std::endl;
//   std::cout << "Number of cells in Segment 1: " << viennagrid::cells( triangle_mesh().segmentation.get_segment(1) ).size() << std::endl;




//   viennamesh::extract_plc::extract_plcs( triangle_mesh().mesh, triangle_mesh().segmentation, brep_mesh().mesh );
//   extract_plcs( triangle_mesh().mesh, triangle_mesh().segmentation, brep_mesh().mesh, brep_mesh().segmentation, 1e-8 );

  std::cout << "Number of vertices: " << viennagrid::vertices(brep_mesh().mesh).size() << std::endl;
  std::cout << "Number of lines: " << viennagrid::lines(brep_mesh().mesh).size() << std::endl;
  std::cout << "Number of cells: " << viennagrid::cells(brep_mesh().mesh).size() << std::endl;


  {
    viennagrid::line_3d_mesh line_mesh;
    viennagrid::line_3d_segmentation line_segmentation(line_mesh);

    for (viennagrid::brep_3d_segmentation::iterator sit = brep_mesh().segmentation.begin(); sit != brep_mesh().segmentation.end(); ++sit)
    {

      typedef viennagrid::result_of::const_line_range<viennagrid::brep_3d_segment_handle>::type ConstLineRangeType;
//     typedef viennagrid::result_of::iterator<ConstLineRangeType::brep_3d_mesh>::type ConstLineIteratorType;
      ConstLineRangeType lines(*sit);
//     for (ConstLineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
//     {
//       viennagrid::copy_elements(  );
//     }

      viennagrid::copy_elements( lines.begin(), lines.end(), line_segmentation.make_segment(), 0 );
    }

    viennagrid::io::vtk_writer<viennagrid::line_3d_mesh> writer;
    writer( line_mesh, line_segmentation, "brep_mesh" );
  }




//   viennamesh::algorithm_handle vmesh_reader( new viennamesh::io::mesh_reader() );
//   vmesh_reader->set_input( "filename", "../data/lado_test.poly" );
//
//   vmesh_reader->run();


  viennamesh::algorithm_handle mesher( new viennamesh::tetgen::algorithm() );

//   mesher->link_input( "default", vmesh_reader, "default" );
  mesher->set_input( "default", triangle_mesh );
//   mesher->set_input( "default", brep_mesh );
//   mesher->reference_output( "default", tetrahedron_mesh );

  mesher->set_input("use_logger", false);
  mesher->set_input("option_string", "zpVq1.8/20O7/7x10000000");

//   mesher->set_input( "cell_size", 1000000.0 );              // maximum cell size
//   mesher->set_input( "max_radius_edge_ratio", 1e3 );  // maximum radius edge ratio
//   mesher->set_input( "min_dihedral_angle", 0.17 );     // minimum dihedral angle in radiant, 0.17 are about 10 degrees

//   mesher->link_input( "seed_points", vmesh_reader, "seed_points" );
//   mesher->set_input( "seed_points", seed_points );
//   mesher->set_input( "extract_segment_seed_points", false );

  mesher->run();

  {
    viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );
    writer->set_input( "filename", "../data/lado_test.poly" );
    writer->link_input( "default", mesher, "default" );

    writer->run();
  }


//   {
//     viennagrid::io::vtk_writer<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> writer;
//     writer( tetrahedron_mesh.mesh, tetrahedron_mesh.segmentation, "meshed_bnd" );
//   }

//   {
//     viennagrid::io::mphtxt_writer writer;
//     writer( tetrahedron_mesh.mesh, tetrahedron_mesh.segmentation, "meshed_bnd" );
//   }

}
