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

#include "volumetric_resample.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/inclusion.hpp"
#include "viennagrid/algorithm/centroid.hpp"


namespace viennamesh
{
  volumetric_resample::volumetric_resample() {}
  std::string volumetric_resample::name() { return "volumetric_resample"; }

  bool volumetric_resample::run(viennamesh::algorithm_handle &)
  {
    data_handle<double> input_cube_size_x = get_required_input<double>("cube_size_x");
    data_handle<double> input_cube_size_y = get_required_input<double>("cube_size_y");
    data_handle<double> input_cube_size_z = get_required_input<double>("cube_size_z");

    data_handle<int> sample_count = get_input<int>("sample_count");


    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    mesh_handle output_mesh = make_data<mesh_handle>();

    std::pair<point_t, point_t> bounding_box = viennagrid::bounding_box( input_mesh() );
    int cube_count_x = (bounding_box.second[0]-bounding_box.first[0]) / input_cube_size_x() + 0.5;
    int cube_count_y = (bounding_box.second[1]-bounding_box.first[1]) / input_cube_size_y() + 0.5;
    int cube_count_z = (bounding_box.second[2]-bounding_box.first[2]) / input_cube_size_z() + 0.5;

    double cube_size_x = (bounding_box.second[0]-bounding_box.first[0]) / cube_count_x;
    double cube_size_y = (bounding_box.second[1]-bounding_box.first[1]) / cube_count_y;
    double cube_size_z = (bounding_box.second[2]-bounding_box.first[2]) / cube_count_z;

    double cube_volume = std::min(cube_size_x, std::min(cube_size_y, cube_size_z));
    cube_volume = cube_volume * cube_volume * cube_volume;

    int border = 0;

    cube_count_x += 2*border;
    cube_count_y += 2*border;
    cube_count_z += 2*border;

    point_t ll = bounding_box.first - viennagrid::make_point(cube_size_x, cube_size_y, cube_size_z)*border;



    info(1) << "Count x = " << cube_count_x << std::endl;
    info(1) << "Count y = " << cube_count_y << std::endl;
    info(1) << "Count z = " << cube_count_z << std::endl;

    int vertex_count_x = cube_count_x+1;
    int vertex_count_y = cube_count_y+1;
    int vertex_count_z = cube_count_z+1;

    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::element<MeshType>::type ElementType;

    int total_cube_count = cube_count_x*cube_count_y*cube_count_z;
    int total_vertex_count = vertex_count_x*vertex_count_y*vertex_count_z;
    std::vector<ElementType> vertices(total_vertex_count);

    MeshType tmp;

    int index = 0;
    for (int z = 0; z != vertex_count_z; ++z)
      for (int y = 0; y != vertex_count_y; ++y)
        for (int x = 0; x != vertex_count_x; ++x)
        {
          point_t pos = viennagrid::make_point(x*cube_size_x, y*cube_size_y, z*cube_size_z) + ll;
          vertices[index++] = viennagrid::make_vertex(tmp, pos);
        }


    for (int z = 0; z != cube_count_z; ++z)
      for (int y = 0; y != cube_count_y; ++y)
        for (int x = 0; x != cube_count_x; ++x)
        {
          viennagrid::make_hexahedron(tmp,
                                      vertices[ (z+0)*vertex_count_y*vertex_count_x + (y+0)*vertex_count_x + x+0 ],
                                      vertices[ (z+0)*vertex_count_y*vertex_count_x + (y+0)*vertex_count_x + x+1 ],
                                      vertices[ (z+0)*vertex_count_y*vertex_count_x + (y+1)*vertex_count_x + x+0 ],
                                      vertices[ (z+0)*vertex_count_y*vertex_count_x + (y+1)*vertex_count_x + x+1 ],
                                      vertices[ (z+1)*vertex_count_y*vertex_count_x + (y+0)*vertex_count_x + x+0 ],
                                      vertices[ (z+1)*vertex_count_y*vertex_count_x + (y+0)*vertex_count_x + x+1 ],
                                      vertices[ (z+1)*vertex_count_y*vertex_count_x + (y+1)*vertex_count_x + x+0 ],
                                      vertices[ (z+1)*vertex_count_y*vertex_count_x + (y+1)*vertex_count_x + x+1 ] );
        }

    typedef viennagrid::result_of::cell_range<MeshType>::type CellRangeType;
    typedef viennagrid::result_of::iterator<CellRangeType>::type CellRangeIterator;


    srand(time(NULL));

//     CellRangeType src_cells( input_mesh() );
//     for (CellRangeIterator scit = src_cells.begin(); scit != src_cells.end(); ++scit)
//     {
//       point_t pt_a = viennagrid::get_point( viennagrid::vertices(*scit)[0] );
//       point_t pt_b = viennagrid::get_point( viennagrid::vertices(*scit)[1] );
//       point_t pt_c = viennagrid::get_point( viennagrid::vertices(*scit)[2] );
//       point_t pt_d = viennagrid::get_point( viennagrid::vertices(*scit)[3] );
//
//       double volume = viennagrid::volume(*scit);
//       double count = volume / cube_volume;
//       int step = std::pow(count, 1.0/3.0) + 2.5;
//
//       for (int i = 0; i < sample_count(); ++i)
//       {
//         double a = static_cast<double>(rand())/RAND_MAX;
//         double b = static_cast<double>(rand())/RAND_MAX;
//         double c = static_cast<double>(rand())/RAND_MAX;
//         double d = static_cast<double>(rand())/RAND_MAX;
//
//         double sum = a+b+c+d;
//         a /= sum;
//         b /= sum;
//         c /= sum;
//         d /= sum;
//
//         point_t pt = pt_a*a + pt_b*b + pt_c*c + pt_d*d - ll;
//
//         int x = pt[0] / cube_size_x + 0.5;
//         int y = pt[1] / cube_size_y + 0.5;
//         int z = pt[2] / cube_size_z + 0.5;
//
//         int index = z*cube_count_y*cube_count_x + y*cube_count_x + x;
//
// //         std::cout << "pt = " << pt << "  x = " << x << "  y = " << y << "  z = " << z << std::endl;
//
//
//         typedef viennagrid::result_of::region_range<MeshType, ElementType>::type RegionRangeType;
//         typedef viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;
//
//         RegionRangeType regions(input_mesh(),*scit);
//         for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
//         {
//           hits[index][(*rit).id()]++;
//         }
//
//       }
//
//
//     }
//
//
//
//
//     typedef viennagrid::result_of::element_copy_map<>::type ElementCopyMap;
//     ElementCopyMap copy_map( output_mesh() );
//
//     CellRangeType cells(tmp);
//     for (CellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
//     {
//       point_t pt = viennagrid::centroid(*cit) - ll;
//       int x = pt[0] / cube_size_x;
//       int y = pt[1] / cube_size_y;
//       int z = pt[2] / cube_size_z;
//
// //       std::cout << "pt = " << pt << "  x = " << x << "  y = " << y << "  z = " << z << std::endl;
//
//       int index = z*cube_count_y*cube_count_x + y*cube_count_x + x;
//
//       std::vector<int>::iterator max = std::max_element( hits[index].begin(), hits[index].end() );
//       int region_id = max - hits[index].begin();
//
//       if (*max > 0)
//       {
//         ElementType new_element = copy_map(*cit);
//         viennagrid::add( output_mesh().get_make_region(region_id), new_element );
//       }
//     }



    typedef viennagrid::result_of::element_copy_map<>::type ElementCopyMap;
    ElementCopyMap copy_map( output_mesh() );

    CellRangeType cells(tmp);
    for (CellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      point_t ll = viennagrid::get_point( viennagrid::vertices(*cit)[0] );
      point_t ur = viennagrid::get_point( viennagrid::vertices(*cit)[7] );

      std::vector<int> hits( input_mesh().region_count(), 0 );
      int total_hits = 0;

      for (int i = 0; i < sample_count(); ++i)
      {
        double x = ll[0] + (ur[0]-ll[0]) * static_cast<double>(rand())/RAND_MAX;
        double y = ll[1] + (ur[1]-ll[1]) * static_cast<double>(rand())/RAND_MAX;
        double z = ll[2] + (ur[2]-ll[2]) * static_cast<double>(rand())/RAND_MAX;

        point_t sample_point = viennagrid::make_point(x,y,z);

        CellRangeType src_cells( input_mesh() );
        for (CellRangeIterator scit = src_cells.begin(); scit != src_cells.end(); ++scit)
        {
          if (viennagrid::is_inside(*scit, sample_point))
          {
            typedef viennagrid::result_of::region_range<MeshType, ElementType>::type RegionRangeType;
            typedef viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

            RegionRangeType regions(input_mesh(),*scit);
            for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
            {
              ++total_hits;
              hits[(*rit).id()]++;
            }
          }
        }
      }

      if (total_hits > 0)
      {
        std::vector<int>::iterator max = std::max_element( hits.begin(), hits.end() );
        int region_id = max - hits.begin();

        ElementType new_element = copy_map(*cit);
        viennagrid::add( output_mesh().get_make_region(region_id), new_element );
      }
    }

    set_output( "mesh", output_mesh );

    return true;
  }

}
