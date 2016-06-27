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

#include <set>
#include <map>
#include <iterator>

#include "recombine_slice.hpp"
#include "geometry.hpp"

#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/core/ntree.hpp"

#include "viennameshpp/progress_tracker.hpp"


namespace viennamesh
{

  void reflect(viennagrid::mesh const & input,
               viennagrid::mesh const & output,
               viennagrid::point const & centroid,
               viennagrid::point const & normal,
               double tol)
  {
    typedef viennagrid::mesh                                                MeshType;

    typedef viennagrid::result_of::point<MeshType>::type                    PointType;
    typedef viennagrid::result_of::element<MeshType>::type                  ElementType;

    typedef viennagrid::result_of::const_element_range<MeshType>::type      ConstElementRangeType;
    typedef viennagrid::result_of::iterator<ConstElementRangeType>::type    ConstElementIteratorType;

    std::map< ElementType, std::pair<ElementType,ElementType> > vertex_map;

    ConstElementRangeType vertices(input, 0);
    for (ConstElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      PointType point = viennagrid::get_point( *vit );

      double d = viennagrid::inner_prod( point-centroid, normal );
      if (std::abs(d) < tol)
      {
        ElementType nv = viennagrid::make_vertex( output, point );
        vertex_map[*vit] = std::make_pair(nv, nv);
      }
      else
      {
        assert( d > 0 );
        PointType reflected = point - 2 * normal * d;

        ElementType nv0 = viennagrid::make_vertex( output, point );
        ElementType nv1 = viennagrid::make_vertex( output, reflected );

        vertex_map[*vit] = std::make_pair(nv0, nv1);
      }
    }


    ConstElementRangeType cells(input, viennagrid::cell_dimension(input));
    for (ConstElementIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
    {
      typedef viennagrid::result_of::const_element_range<ElementType>::type ConstBoundaryRangeType;
      typedef viennagrid::result_of::iterator<ConstBoundaryRangeType>::type ConstBoundaryIteratorType;

      ConstBoundaryRangeType boundary_vertices(*cit, 0);

      std::vector<ElementType> vertices[2];

      int i = 0;
      for (ConstBoundaryIteratorType bvit = boundary_vertices.begin(); bvit != boundary_vertices.end(); ++bvit, ++i)
      {
        vertices[0].push_back( vertex_map[*bvit].first );
        vertices[1].push_back( vertex_map[*bvit].second );
      }

      for (int i = 0; i != 2; ++i)
      {
        ElementType cell = viennagrid::make_element( output, (*cit).tag(), vertices[i].begin(), vertices[i].end() );
        viennagrid::copy_region_information(*cit, cell);
      }
    }
  }






  struct status_logging
  {
    void operator()(double percentage)
    {
      info(5) << "Status: " << percentage*100 << "%" << std::endl;
    }
  };





  recombine_symmetric_slice::recombine_symmetric_slice() {}
  std::string recombine_symmetric_slice::name() { return "recombine_symmetric_slice"; }

  bool recombine_symmetric_slice::run(viennamesh::algorithm_handle &)
  {
    typedef viennagrid::mesh                                                          MeshType;
    typedef viennagrid::result_of::region<MeshType>::type                             RegionType;

    typedef viennagrid::result_of::point<MeshType>::type                              PointType;
    typedef viennagrid::result_of::element<MeshType>::type                            ElementType;

    typedef viennagrid::result_of::const_element_range<MeshType>::type                ConstElementRangeType;
    typedef viennagrid::result_of::iterator<ConstElementRangeType>::type              ConstElementIteratorType;

    typedef viennagrid::result_of::const_element_range<ElementType>::type             ConstBoundaryElementRangeType;
    typedef viennagrid::result_of::iterator<ConstBoundaryElementRangeType>::type      ConstBoundaryElementIteratorType;

    double tol = 1e-6;
    if (get_input<double>("tolerance").valid())
      tol = get_input<double>("tolerance")();

    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");


    PointType axis = get_required_input<point>("axis")();
    axis.normalize();


    int rotational_frequency = get_required_input<int>("rotational_frequency")();
    double angle = 2*M_PI/rotational_frequency;

    PointType N[2];
    N[0] = viennagrid::make_point(0,1,0);
    if ( std::abs(viennagrid::inner_prod(axis,N[0])) > 1.0-tol )
      N[0] = viennagrid::make_point(-1,0,0);
    N[0] -= axis * viennagrid::inner_prod(axis,N[0]);
    N[0].normalize();

    N[1] = -rotate(N[0], axis, angle);

    info(1) << "Using rotational frequency " << rotational_frequency << std::endl;
    info(1) << "Angle = " << angle << std::endl;
    info(1) << "Axis = " << axis << std::endl;
    info(1) << "Normal[0] = " << N[0] << std::endl;
    info(1) << "Normal[1] = " << N[1] << std::endl;



    ConstElementRangeType vertices( input_mesh(), 0 );
    ConstElementRangeType cells( input_mesh(), viennagrid::cell_dimension(input_mesh()) );

    std::vector<viennagrid::element_id> vertices_on_both_planes;
    std::vector<viennagrid::element_id> vertices_on_plane0;
    std::vector<viennagrid::element_id> vertices_on_plane1;
    std::vector<viennagrid::element_id> vertices_on_no_plane;

    viennagrid::vector<PointType> points( vertices.size() );

    int i = 0;
    for (ConstElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++i)
    {
      points[i] = viennagrid::get_point(*vit);

      double dp0 = std::abs(viennagrid::inner_prod(points[i], N[0]));
      double dp1 = std::abs(viennagrid::inner_prod(points[i], N[1]));

      if ( (dp0 < tol) && (dp1 < tol) )
        vertices_on_both_planes.push_back( (*vit).id() );
      else if ( dp0 < tol )
        vertices_on_plane0.push_back( (*vit).id() );
      else if ( dp1 < tol )
        vertices_on_plane1.push_back( (*vit).id() );
      else
        vertices_on_no_plane.push_back( (*vit).id() );
    }

    std::size_t shared_vertex_count = vertices_on_both_planes.size();
    std::size_t on_plane_0_count = shared_vertex_count + vertices_on_plane0.size();
    std::size_t on_no_plane_count = on_plane_0_count + vertices_on_no_plane.size();
    std::size_t non_shared_vertex_count = vertices_on_plane0.size() + 2*vertices_on_no_plane.size() + vertices_on_plane1.size();


    viennagrid::vector<viennagrid::element_id> vertex_mapping( vertices.size() );
    mesh_handle output_mesh = make_data<mesh_handle>();


    int offset = 0;

    for (std::size_t i = 0; i != vertices_on_both_planes.size(); ++i)
      vertex_mapping[vertices_on_both_planes[i]] = i+offset;
    offset += vertices_on_both_planes.size();

    for (std::size_t i = 0; i != vertices_on_plane0.size(); ++i)
      vertex_mapping[vertices_on_plane0[i]] = i+offset;
    offset += vertices_on_plane0.size();

    for (std::size_t i = 0; i != vertices_on_no_plane.size(); ++i)
      vertex_mapping[vertices_on_no_plane[i]] = i+offset;
    offset += vertices_on_no_plane.size();

    for (std::size_t i = 0; i != vertices_on_plane1.size(); ++i)
      vertex_mapping[vertices_on_plane1[i]] = i+offset;



    if (rotational_frequency % 2 == 0)
    {
      viennagrid::vector<viennagrid::element_id> new_vertices;
      new_vertices.reserve( vertices_on_both_planes.size() +
                          (vertices_on_plane0.size()+vertices_on_no_plane.size()+vertices_on_plane1.size()) * rotational_frequency );



      for (std::size_t i = 0; i != vertices_on_both_planes.size(); ++i)
        new_vertices.push_back( viennagrid::make_vertex(output_mesh(), points[vertices_on_both_planes[i]]).id() );

      for (int hrf = 0; hrf != rotational_frequency/2; ++hrf)
      {
        double current_angle = angle * hrf * 2;

        for (std::size_t i = 0; i != vertices_on_plane0.size(); ++i)
        {
          PointType rotated = rotate( points[ vertices_on_plane0[i] ], axis, current_angle );
          new_vertices.push_back( viennagrid::make_vertex(output_mesh(), rotated).id() );
        }

        for (std::size_t i = 0; i != vertices_on_no_plane.size(); ++i)
        {
          PointType rotated = rotate( points[ vertices_on_no_plane[i] ], axis, current_angle );
          new_vertices.push_back( viennagrid::make_vertex(output_mesh(), rotated).id() );
        }

        for (std::size_t i = 0; i != vertices_on_plane1.size(); ++i)
        {
          PointType rotated = rotate( points[ vertices_on_plane1[i] ], axis, current_angle );
          new_vertices.push_back( viennagrid::make_vertex(output_mesh(), rotated).id() );
        }

        current_angle = angle * (hrf+1) * 2;
        for (std::size_t i = 0; i != vertices_on_no_plane.size(); ++i)
        {
          PointType rotated = rotate( reflect(points[vertices_on_no_plane[i]], N[0]),
                                      axis, current_angle );
          new_vertices.push_back( viennagrid::make_vertex(output_mesh(), rotated).id() );
        }
      }

      info(1) << "New mesh has " << new_vertices.size() << " vertices (old had " << vertices.size() << ")" << std::endl;
      info(1) << "    shared vertex count = " << vertices_on_both_planes.size() << std::endl;
      info(1) << "    on plane count = " << vertices_on_plane0.size() << std::endl;
      info(1) << "    on no plane count = " << vertices_on_no_plane.size() << std::endl;


      typedef viennagrid::result_of::region<MeshType>::type RegionType;
      typedef viennagrid::result_of::region_range<MeshType>::type RegionRangeType;
      typedef viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIteratorType;
      RegionRangeType regions( input_mesh() );
      for (RegionRangeIteratorType rit = regions.begin(); rit != regions.end(); ++rit)
      {
        RegionType region = output_mesh().get_or_create_region( (*rit).id() );
        region.set_name( (*rit).get_name() );
      }



      std::vector<viennagrid_element_type> element_types( cells.size()*rotational_frequency );
      std::vector<viennagrid_int> cell_vertex_offsets( cells.size()*rotational_frequency + 1 );
      std::vector<viennagrid_element_id> cell_vertex_indices;
      std::vector<viennagrid_region_id> region_ids;

      if (!regions.empty())
        region_ids.resize( cells.size()*rotational_frequency );

      cell_vertex_offsets[0] = 0;
      cell_vertex_indices.reserve( cells.size()*rotational_frequency * (viennagrid::cell_dimension(input_mesh())+1) );

      viennagrid_int cid = 0;
      for (ConstElementIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
      {
        std::vector<viennagrid::element_id> vertex_ids;
        ConstBoundaryElementRangeType vertices_on_cell(*cit, 0);

        for (ConstBoundaryElementIteratorType vcit = vertices_on_cell.begin(); vcit != vertices_on_cell.end(); ++vcit)
          vertex_ids.push_back( vertex_mapping[(*vcit).id()] );

        viennagrid_region_id cell_region;
        if (!regions.empty())
        {
          typedef viennagrid::result_of::region_range<ElementType>::type RegionRangeType;
          RegionRangeType cell_regions( *cit );
          cell_region = (*(cell_regions.begin())).id();
        }

        for (int hrf = 0; hrf != rotational_frequency/2; ++hrf)
        {
          viennagrid_int offset0 = cell_vertex_indices.size();
          viennagrid_int offset1 = offset0 + vertex_ids.size();

          cell_vertex_indices.resize( offset0 + 2*vertex_ids.size() );
          cell_vertex_offsets[cid+1] = offset0 + vertex_ids.size();
          cell_vertex_offsets[cid+2] = offset1 + vertex_ids.size();

          element_types[cid+0] = (*cit).tag().internal();
          element_types[cid+1] = (*cit).tag().internal();

          if (!regions.empty())
          {
            region_ids[cid+0] = cell_region;
            region_ids[cid+1] = cell_region;
          }

          cid += 2;

          for (std::size_t i = 0; i != vertex_ids.size(); ++i)
          {
            int vertex_region;
            if (vertex_ids[i] < static_cast<int>(shared_vertex_count))
              vertex_region = 0;
            else if (vertex_ids[i] < static_cast<int>(on_plane_0_count))
              vertex_region = 1;
            else if (vertex_ids[i] < static_cast<int>(on_no_plane_count))
              vertex_region = 3;
            else
              vertex_region = 2;

            viennagrid::element_id index0;
            viennagrid::element_id index1;

            switch (vertex_region)
            {
              case 0:
                index0 = vertex_ids[i];
                index1 = vertex_ids[i];
                break;

              case 1:
                index0 = vertex_ids[i] + hrf*non_shared_vertex_count;
                if (index0 >= static_cast<int>(new_vertices.size()))
                  index0 -= rotational_frequency*non_shared_vertex_count/2;

                index1 = vertex_ids[i] + (hrf+1)*non_shared_vertex_count;
                if (index1 >= static_cast<int>(new_vertices.size()))
                  index1 -= rotational_frequency*non_shared_vertex_count/2;
                break;

              case 2:
                index0 = vertex_ids[i] + hrf*non_shared_vertex_count;
                if (index0 >= static_cast<int>(new_vertices.size()))
                  index0 -= rotational_frequency*non_shared_vertex_count/2;

                index1 = vertex_ids[i] + hrf*non_shared_vertex_count;
                if (index1 >= static_cast<int>(new_vertices.size()))
                  index1 -= rotational_frequency*non_shared_vertex_count/2;
                break;

              case 3:
                index0 = vertex_ids[i] + hrf*non_shared_vertex_count;
                index1 = vertex_ids[i] + vertices_on_no_plane.size() + vertices_on_plane1.size() + hrf*non_shared_vertex_count;
                break;

              default:
                assert(false);
            }

            cell_vertex_indices[offset0 + i] = new_vertices[index0].internal();
            cell_vertex_indices[offset1 + i] = new_vertices[index1].internal();
          }
        }
      }


      if (!regions.empty())
      {
        viennagrid_mesh_element_batch_create( output_mesh().internal(),
                                              element_types.size(), &element_types[0],
                                              &cell_vertex_offsets[0], &cell_vertex_indices[0],
                                              &region_ids[0], NULL );
      }
      else
      {
        viennagrid_mesh_element_batch_create( output_mesh().internal(),
                                              element_types.size(), &element_types[0],
                                              &cell_vertex_offsets[0], &cell_vertex_indices[0],
                                              NULL, NULL );
      }
    }
    else
    {
      PointType cs[2];

      cs[0] = viennagrid::make_point(1,0,0);
      if ( std::abs(viennagrid::inner_prod(cs[0], N[1])) > 1.0-tol )
        cs[0] = viennagrid::make_point(0,1,0);
      cs[1] = viennagrid::cross_prod(N[1], cs[0]);
      cs[1].normalize();
      cs[0] = viennagrid::cross_prod(cs[1], N[1]);

      std::vector<PointType> pp1( vertices_on_plane1.size() );
      for (std::size_t i = 0; i != vertices_on_plane1.size(); ++i)
      {
        pp1[i].resize(2);
        pp1[i][0] = viennagrid::inner_prod( cs[0], points[vertices_on_plane1[i]] );
        pp1[i][1] = viennagrid::inner_prod( cs[1], points[vertices_on_plane1[i]] );
      }

      PointType min = pp1[0];
      PointType max = pp1[1];

      for (std::size_t i1 = 1; i1 != vertices_on_plane1.size(); ++i1)
      {
        min = viennagrid::min(min, pp1[i1]);
        max = viennagrid::max(max, pp1[i1]);
      }

      PointType center = (min+max)/2;
      PointType size = max-min;

      typedef viennagrid::point_id_wrapper WrapperType;
      typedef viennagrid::ntree_node<WrapperType> NodeType;
      boost::shared_ptr<NodeType> root( new NodeType( center-size/2*1.3 , center+size/2*1.3 ) );

      {
        viennamesh::LoggingStack stack("build tree");
        for (std::size_t i1 = 0; i1 != vertices_on_plane1.size(); ++i1)
          root->add( WrapperType(pp1[i1], i1), 10, vertices_on_plane1.size()/10 );
      }

      std::vector<viennagrid::element_id> reordered_vertices_on_plane1( vertices_on_plane0.size() );
      for (std::size_t i0 = 0; i0 != vertices_on_plane0.size(); ++i0)
      {
        PointType const & p0 = points[vertices_on_plane0[i0]];
        PointType rotated_p0 = rotate(p0, axis, angle);

        PointType tmp0(2);
        tmp0[0] = viennagrid::inner_prod( cs[0], rotated_p0 );
        tmp0[1] = viennagrid::inner_prod( cs[1], rotated_p0 );

        NodeType * node = root->get(tmp0);
        for (std::size_t j = 0; j != node->elements().size(); ++j)
        {
          PointType tmp1 = node->elements()[j].point();
          if ( viennagrid::detail::is_equal(tol, tmp0, tmp1) )
          {
            reordered_vertices_on_plane1[i0] = vertices_on_plane1[ node->elements()[j].id() ];
            break;
          }
        }
      }

      vertices_on_plane1 = reordered_vertices_on_plane1;

      std::size_t shared_vertex_count = vertices_on_both_planes.size();
      std::size_t non_shared_vertex_count = vertices_on_plane0.size() + vertices_on_no_plane.size();


      viennagrid::vector<ElementType> new_vertices;
      viennagrid::vector<viennagrid::element_id> vertex_mapping( vertices.size() );
      mesh_handle output_mesh = make_data<mesh_handle>();


      viennamesh::LoggingStack stack("misc");

      int offset = 0;

      for (std::size_t i = 0; i != vertices_on_both_planes.size(); ++i)
        vertex_mapping[vertices_on_both_planes[i]] = i+offset;
      offset += vertices_on_both_planes.size();

      for (std::size_t i = 0; i != vertices_on_plane0.size(); ++i)
        vertex_mapping[vertices_on_plane0[i]] = i+offset;
      offset += vertices_on_plane0.size();

      for (std::size_t i = 0; i != vertices_on_no_plane.size(); ++i)
        vertex_mapping[vertices_on_no_plane[i]] = i+offset;
      offset += vertices_on_no_plane.size();

      for (std::size_t i = 0; i != vertices_on_plane1.size(); ++i)
        vertex_mapping[vertices_on_plane1[i]] = i+offset;



      typedef viennagrid::result_of::const_region_range<MeshType>::type RegionRangeType;
      typedef viennagrid::result_of::iterator<RegionRangeType>::type RegionIteratorType;

      RegionRangeType regions( input_mesh() );
      std::vector<RegionType> new_regions;
      for (RegionIteratorType rit = regions.begin(); rit != regions.end(); ++rit)
      {
        RegionType region = output_mesh().get_or_create_region( (*rit).id() );
        region.set_name( (*rit).get_name() );
        new_regions.push_back(region);

        info(1) << "  Copy region " << region.id() << " (name = \"" << region.get_name() << "\"" << std::endl;
      }


      for (std::size_t i = 0; i != vertices_on_both_planes.size(); ++i)
        new_vertices.push_back( viennagrid::make_vertex(output_mesh(), points[vertices_on_both_planes[i]]) );

      for (int rf = 0; rf != rotational_frequency; ++rf)
      {
        double current_angle = angle * rf;

        for (std::size_t i = 0; i != vertices_on_plane0.size(); ++i)
          new_vertices.push_back( viennagrid::make_vertex(output_mesh(), rotate(points[vertices_on_plane0[i]], axis, current_angle)) );

        for (std::size_t i = 0; i != vertices_on_no_plane.size(); ++i)
          new_vertices.push_back( viennagrid::make_vertex(output_mesh(), rotate(points[vertices_on_no_plane[i]], axis, current_angle)) );
      }

      info(1) << "New mesh has " << new_vertices.size() << " vertices (old had " << vertices.size() << ")" << std::endl;
      info(1) << "    shared vertex count = " << shared_vertex_count << std::endl;
      info(1) << "    on plane count = " << vertices_on_plane0.size() << std::endl;
      info(1) << "    on no plane count = " << vertices_on_no_plane.size() << std::endl;


      for (ConstElementIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
      {
        std::vector<viennagrid::element_id> vertex_ids;
        ConstBoundaryElementRangeType vertices_on_cell(*cit, 0);

        for (ConstBoundaryElementIteratorType vcit = vertices_on_cell.begin(); vcit != vertices_on_cell.end(); ++vcit)
          vertex_ids.push_back( vertex_mapping[(*vcit).id()] );


        for (int rf = 0; rf != rotational_frequency; ++rf)
        {
          std::vector<ElementType> local_vertices(vertex_ids.size());
          for (std::size_t i = 0; i != vertex_ids.size(); ++i)
          {
            if (vertex_ids[i] < static_cast<int>(shared_vertex_count))
              local_vertices[i] = new_vertices[vertex_ids[i]];
            else
            {
              viennagrid::element_id index = vertex_ids[i] + rf*non_shared_vertex_count;
              if (index >= static_cast<int>(new_vertices.size()))
                index -= rotational_frequency*non_shared_vertex_count;

              local_vertices[i] = new_vertices[index];
            }
          }

          ElementType new_cell = viennagrid::make_element( output_mesh(), (*cit).tag(), local_vertices.begin(), local_vertices.end() );
          viennagrid::copy_region_information(*cit, new_cell);
        }
      }
    }

    set_output( "mesh", output_mesh );
    return true;
  }

}
