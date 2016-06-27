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

#include <numeric>
#include <boost/concept_check.hpp>
#include "multi_material_marching_cubes.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/inclusion.hpp"
#include "viennagrid/algorithm/centroid.hpp"



namespace viennamesh
{


  template<typename ValueT>
  ValueT & access(std::vector<ValueT> & array,
                std::vector<int> const & size,
                std::vector<int> const & pos)
  {
    return array[ pos[2]*size[0]*size[1] + pos[1]*size[0] + pos[0] ];
  }

  template<typename ValueT>
  ValueT & access_symmetric(std::vector<ValueT> & array,
                          std::vector<int> const & size,
                          std::vector<int> pos)
  {
    for (int i = 0; i != 3; ++i)
      pos[i] += size[i]/2;

    return access(array, size, pos);
  }

  template<typename ValueT>
  ValueT & access_symmetric(std::vector<ValueT> & array,
                          std::vector<int> const & size,
                          int x, int y, int z)
  {
    std::vector<int> pos(3);
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
    return access_symmetric(array, size, pos);
  }





  struct line
  {
    line() {}

    line(int face_, int from_, int to_) :
      face(face_), from(std::min(from_, to_)), to(std::max(from_, to_)) {}

    int face;

    int from; // -1 for center
    int to;

    bool has_center_point() const
    {
      return from == -1 || to == -1;
    }

    bool operator==(line const & l) const
    {
      return (face == l.face) && (from == l.from) && (to == l.to);
    }

    bool operator!=(line const & l) const
    {
      return !(*this == l);
    }
  };

  struct marching_square
  {
    marching_square() {}
    marching_square(int v0, int v1, int v2, int v3,
                    int e0, int e1, int e2, int e3)
    {
      vertex_indices[0] = v0;
      vertex_indices[1] = v1;
      vertex_indices[2] = v2;
      vertex_indices[3] = v3;

      edge_indices[0] = e0;
      edge_indices[1] = e1;
      edge_indices[2] = e2;
      edge_indices[3] = e3;
    }

    int vertex_indices[4];
    int edge_indices[4];

    std::vector<line> lines;
    std::vector<line> center_lines;

    bool has_center_point() const
    {
      for (std::size_t i = 0; i != lines.size(); ++i)
        if (lines[i].has_center_point())
          return true;

      return false;
    }
  };




  struct poly_line
  {
    std::vector<int> vertex_indices;
    std::pair<int,int> regions;
  };


  struct marching_cube
  {
    marching_cube(int r0, int r1, int r2, int r3,
                  int r4, int r5, int r6, int r7) : face_centers(0)
    {
      region_ids[0] = r0;
      region_ids[1] = r1;
      region_ids[2] = r2;
      region_ids[3] = r3;
      region_ids[4] = r4;
      region_ids[5] = r5;
      region_ids[6] = r6;
      region_ids[7] = r7;

      faces[0] = marching_square(0, 1, 2, 3,   0, 1, 2, 3);
      faces[1] = marching_square(4, 5, 0, 1,   8, 4, 0, 5);
      faces[2] = marching_square(4, 0, 6, 2,   4, 9, 6, 1);
      faces[3] = marching_square(1, 5, 3, 7,   5, 3, 7, 11);
      faces[4] = marching_square(2, 3, 6, 7,   2, 6, 10, 7);
      faces[5] = marching_square(6, 7, 4, 5,   10, 9, 8, 11);
    }



//     void make_lines(std::map<int,int> & region_priority)
    void make_lines(std::vector<int> const & region_priority)
    {
      for (int f = 0; f != 6; ++f)
      {
        marching_square & face = faces[f];

        int local_region[4];
        std::map<int, int> regions;

        for (int i = 0; i != 4; ++i)
        {
          local_region[i] = region_ids[face.vertex_indices[i]];
          regions[ region_ids[face.vertex_indices[i]] ]++;
        }

        // all vertices are in the same region
        if (regions.size() == 1)
          continue;

        if (regions.size() == 2)
        {
          std::map<int, int>::iterator it = regions.begin();
          int region_id0 = it->first;
          int region_count0 = it->second;

          ++it;
          int region_id1 = it->first;
          int region_count1 = it->second;

          if ( region_count0 == 2 )
          {
            // 2 are in one region, 2 in the other
            if ( local_region[0] == local_region[3] )
            {
              // the two diagonal vertices have the same region, region with higher priority is connected

              // ensure region_id0 has higher priority

              if ( (region_id1 < 0) ||
                   ((region_id0 >= 0) && region_priority[region_id0] < region_priority[region_id1]) )
              {
                std::swap(region_id0, region_id1);
                std::swap(region_count0, region_count1);
              }


              if ( region_ids[face.vertex_indices[0]] == region_id0 )
              {
                // higher priority lies on local index 0 and 3
                add_line(f, 1, 2);
                add_line(f, 0, 3);
              }
              else
              {
                // higher priority lies on local index 1 and 2
                add_line(f, 0, 1);
                add_line(f, 2, 3);
              }
            }
            else
            {
              // the two neighboring vertices have the same region

              if ( local_region[0] == local_region[1] )
              {
                add_line(f, 1, 3);
              }
              else if ( local_region[0] == local_region[2] )
              {
                add_line(f, 0, 2);
              }
              else
              {
                std::cout << "ERROR " << __LINE__ << std::endl;
              }
            }
          }
          else
          {
            // ensure region_id0 is only present once
            if (region_count0 == 3)
            {
              std::swap(region_id0, region_id1);
              std::swap(region_count0, region_count1);
            }

            if ( local_region[0] == region_id0 )
            {
              add_line(f, 0, 1);
            }
            else if ( local_region[1] == region_id0 )
            {
              add_line(f, 0, 3);
            }
            else if ( local_region[2] == region_id0 )
            {
              add_line(f, 1, 2);
            }
            else
            {
              add_line(f, 2, 3);
            }
          }
        } // if (regions.size() == 2)
        else if (regions.size() == 3)
        {
          if (local_region[0] == local_region[3])
          {
            add_line(f, 1, 2);
            add_line(f, 0, 3);
          }
          else if (local_region[1] == local_region[2])
          {
            add_line(f, 0, 1);
            add_line(f, 2, 3);
          }
          else if (local_region[0] == local_region[1])
          {
            add_line(f, 1, -1);
            add_line(f, 3, -1);
            add_line(f, 2, -1);
          }
          else if (local_region[0] == local_region[2])
          {
            add_line(f, 0, -1);
            add_line(f, 2, -1);
            add_line(f, 3, -1);
          }
          else if (local_region[1] == local_region[3])
          {
            add_line(f, 0, -1);
            add_line(f, 2, -1);
            add_line(f, 1, -1);
          }
          else if (local_region[2] == local_region[3])
          {
            add_line(f, 1, -1);
            add_line(f, 3, -1);
            add_line(f, 0, -1);
          }
          else
          {
            std::cout << "ERROR!!!!" << std::endl;
          }
        }
        else if (regions.size() == 4)
        {
          add_line(f, 0, -1);
          add_line(f, 1, -1);
          add_line(f, 2, -1);
          add_line(f, 3, -1);
        }
        else
        {
          std::cout << "Regions size invalid! " << __LINE__ << std::endl;
        }


        if (face.has_center_point())
          ++face_centers;

//         for (std::size_t i = 0; i != face.lines.size(); ++i)
//         {
//           print(face.lines[i]);
//         }
      }
    }

    void add_line(int f, int edge0, int edge1)
    {
      marching_square & face = faces[f];

      edge0 = (edge0 != -1) ? face.edge_indices[edge0] : -1;
      edge1 = (edge1 != -1) ? face.edge_indices[edge1] : -1;

      line l(f, edge0, edge1);

//       std::cout << "Adding line ";
//       print(l);

      face.lines.push_back(l);

      if (l.from == -1 || l.to == -1)
      {
        face.center_lines.push_back(l);
      }

      if (l.from != -1)
        edge_lines[l.from].push_back(l);
      if (l.to != -1)
        edge_lines[l.to].push_back(l);
    }


    static std::pair<int, int> edge_vertices(int edge_id)
    {
      switch (edge_id)
      {
        case 0: return std::make_pair(0,1);
        case 1: return std::make_pair(0,2);
        case 2: return std::make_pair(2,3);
        case 3: return std::make_pair(1,3);
        case 4: return std::make_pair(0,4);
        case 5: return std::make_pair(1,5);
        case 6: return std::make_pair(2,6);
        case 7: return std::make_pair(3,7);
        case 8: return std::make_pair(4,5);
        case 9: return std::make_pair(4,6);
        case 10: return std::make_pair(6,7);
        case 11: return std::make_pair(5,7);
        default:
          assert(false);
          return std::make_pair(-1,-1);
      }
    }

    std::pair<int, int> line_regions(line const & l) const
    {
      std::pair<int, int> tmp;
      if (l.from != -1)
        tmp = edge_vertices(l.from);
      else
        tmp = edge_vertices(l.to);

      tmp.first = region_ids[tmp.first];
      tmp.second = region_ids[tmp.second];
      if (tmp.first > tmp.second)
        std::swap(tmp.first, tmp.second);

      return tmp;
    }



    void print(line const & l) const
    {
      std::pair<int, int> regions = line_regions(l);
      std::cout << "Line " << l.from << " " << l.to <<
          "     between regions " << regions.first << "-" << regions.second << std::endl;
    }


    std::vector<poly_line> make_poly_lines()
    {
      std::vector<poly_line> poly_lines;

      if (face_centers < 0 || face_centers == 1)
      {
        std::cout << "ERROR" << __FILE__ << ":" << __LINE__ << " face centers count invalid: " << face_centers << std::endl;
        assert(false);
      }

      bool edge_visited[12];
      for (int e = 0; e != 12; ++e)
        edge_visited[e] = false;


      if (face_centers > 0)
      {
        std::vector<bool> center_lines_visited[6];
        for (int f = 0; f != 6; ++f)
          center_lines_visited[f].resize( faces[f].center_lines.size(), false );


        for (int f = 0; f != 6; ++f)
        {
          marching_square & face = faces[f];

          if ( !face.has_center_point() )
            continue;

          for (std::size_t cl = 0; cl != face.center_lines.size(); ++cl)
          {
            if (center_lines_visited[f][cl])
              continue;

            poly_line pl;
            if (face_centers > 2)
              pl.vertex_indices.push_back( 18 );
            pl.vertex_indices.push_back( 12+f );

            line last_line = face.center_lines[cl];
            pl.regions = line_regions( last_line );
            int cur_edge = face.center_lines[cl].to;
            int end_f;



            while (true)
            {
              edge_visited[cur_edge] = true;
              pl.vertex_indices.push_back(cur_edge);

              line l = edge_lines[cur_edge][0];
              if (l == last_line)
                l = edge_lines[cur_edge][1];

              if (l.from == -1)
              {
                end_f = l.face;
                break;
              }

              if ( !edge_visited[l.from] )
                cur_edge = l.from;
              else if ( !edge_visited[l.to] )
                cur_edge = l.to;
              else
                std::cout << "ERROR" << std::endl;

              last_line = l;
            }

            pl.vertex_indices.push_back( 12+end_f );

            center_lines_visited[f][cl] = true;
            for (std::size_t cl_end = 0; cl_end != faces[end_f].center_lines.size(); ++cl_end)
            {
              if ( line_regions(faces[end_f].center_lines[cl_end]) == pl.regions )
              {
                center_lines_visited[end_f][cl_end] = true;
                break;
              }
            }

            poly_lines.push_back(pl);
          }
        }
      }



      for (int e = 0; e != 12; ++e)
      {
        if (edge_visited[e])
          continue;

        if (edge_lines[e].empty())
          continue;

        poly_line pl;
        pl.regions = line_regions( edge_lines[e][0] );

        int cur_edge = e;
        while (true)
        {
          edge_visited[cur_edge] = true;
          pl.vertex_indices.push_back(cur_edge);

          line l0 = edge_lines[cur_edge][0];
          line l1 = edge_lines[cur_edge][1];

          if ( edge_visited[l0.from] && edge_visited[l0.to] &&
                edge_visited[l1.from] && edge_visited[l1.to] )
          {
            break;
          }

          if ( !edge_visited[l0.from] )
            cur_edge = l0.from;
          else if ( !edge_visited[l0.to] )
            cur_edge = l0.to;
          else if ( !edge_visited[l1.from] )
            cur_edge = l1.from;
          else if ( !edge_visited[l1.to] )
            cur_edge = l1.to;
        }

        poly_lines.push_back(pl);
      }

      return poly_lines;
    }



    template<typename PointT>
    PointT point(int index, PointT * pts)
    {
      assert( index >= 0 );
      assert( index <= 18 );

      PointT p(pts[0].size());

      if (index == 18)
      {
        for (int i = 0; i < 8; ++i)
          p += pts[i];
        return p/8;
      }

      if (index >= 12)
      {
        int f = index-12;

        for (int i = 0; i != 4; ++i)
          p += pts[ faces[f].vertex_indices[i] ];
        return p/4;
      }

      std::pair<int,int> indices = edge_vertices(index);
      p = pts[indices.first] + pts[indices.second];
      return p/2;
    }



    int face_centers;

    marching_square faces[6];
    std::vector<line> edge_lines[12];

    bool face_center_point[6];
    bool edge_center_point[12];

    int region_ids[8];
  };




  multi_material_marching_cubes::multi_material_marching_cubes() {}
  std::string multi_material_marching_cubes::name() { return "multi_material_marching_cubes"; }

  bool multi_material_marching_cubes::run(viennamesh::algorithm_handle &)
  {
    typedef viennagrid::mesh                                    MeshType;
    typedef viennagrid::result_of::point<MeshType>::type        PointType;
    typedef viennagrid::result_of::element<MeshType>::type      ElementType;
    typedef viennagrid::result_of::region<MeshType>::type       RegionType;

    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");


    data_handle<int> size_add = get_required_input<int>("size_add");
    data_handle<double> region_scale = get_required_input<double>("region_scale");
    data_handle<double> is_inside_tolerance = get_required_input<double>("is_inside_tolerance");

    typedef viennagrid::result_of::element_range<MeshType>::type ElementRangeType;
    typedef viennagrid::result_of::iterator<ElementRangeType>::type ElementIteratorType;

    typedef viennagrid::result_of::element_range<RegionType>::type RegionElementRangeType;
    typedef viennagrid::result_of::iterator<RegionElementRangeType>::type RegionElementIteratorType;

    typedef viennagrid::result_of::region_range<MeshType>::type RegionRangeType;
    typedef viennagrid::result_of::iterator<RegionRangeType>::type RegionIteratorType;

    typedef viennagrid::result_of::region_range<ElementType>::type ElementRegionRangeType;


    MeshType mesh;
    viennagrid::copy( input_mesh(), mesh );


    RegionRangeType regions(mesh);
    for (RegionIteratorType rit = regions.begin(); rit != regions.end(); ++rit)
    {
      int vertex_count = 0;
      PointType center = viennagrid::make_point(0,0,0);

      RegionElementRangeType vertices(*rit, 1);
      for (RegionElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
      {
        center += viennagrid::get_point(*vit);
        ++vertex_count;
      }
      center /= vertex_count;

      for (RegionElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
        viennagrid::set_point( *vit, (viennagrid::get_point(*vit)-center)*region_scale()+center );
    }


    info(1) << "Finished regional re-scaling" << std::endl;


//     MeshType mesh = tmp;

    mesh_handle output_mesh = make_data<mesh_handle>();

    int region_count = input_mesh().region_count();


    viennagrid_region_id max_region_id = 0;
    for (RegionIteratorType rit = regions.begin(); rit != regions.end(); ++rit)
      max_region_id = std::max((*rit).id(), max_region_id);

    std::vector<int> region_priority(max_region_id);

    int counter = region_count;
    for (RegionIteratorType rit = regions.begin(); rit != regions.end(); ++rit)
      region_priority[(*rit).id()] = counter--;



    std::pair<PointType, PointType> bounding_box = viennagrid::bounding_box( mesh );
    PointType center = (bounding_box.first + bounding_box.second) / 2.0;
    PointType size = bounding_box.second - bounding_box.first;

    point_handle input_sample_size = get_required_input<point_handle>("sample_size");
    PointType sample_size = input_sample_size();



    size += sample_size*2;

    std::vector<int> sample_count(3);
    for (int i = 0; i != 3; ++i)
    {
      sample_count[i] = size[i] / sample_size[i] + 1.0 + size_add();
      if (sample_count[i] % 2 == 0)
        ++sample_count[i];
    }

    info(1) << "Sample counts: x=" << sample_count[0] << " y=" << sample_count[1] << " z=" << sample_count[2] << std::endl;

    int total_sample_size = sample_count[0] * sample_count[1] * sample_count[2];
    std::vector<char> sample_regions(total_sample_size, -1);

    ElementRangeType cells(mesh, viennagrid::cell_dimension(mesh));
    for (ElementIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
    {
      std::pair<PointType, PointType> cell_bb = viennagrid::bounding_box(*cit);

      std::vector<int> min_index(3);
      std::vector<int> max_index(3);

      for (int i = 0; i != 3; ++i)
      {
        min_index[i] = (cell_bb.first[i]-center[i]) / sample_size[i];
        if (cell_bb.first[i] > 0)
          ++min_index[i];

        max_index[i] = (cell_bb.second[i]-center[i]) / sample_size[i];
        if (cell_bb.second[i] < 0)
          --max_index[i];

        min_index[i] -= 3;
        max_index[i] += 3;

        min_index[i] = std::max(min_index[i], -sample_count[i]/2);
        max_index[i] = std::min(max_index[i],  sample_count[i]/2);
      }



      ElementRegionRangeType regions(*cit);
      if (regions.size() != 1)
        error(1) << "ERROR, one cell is on more than one region" << std::endl;

      int region_id = (*regions.begin()).id();

      for (int z = min_index[2]; z <= max_index[2]; ++z)
        for (int y = min_index[1]; y <= max_index[1]; ++y)
          for (int x = min_index[0]; x <= max_index[0]; ++x)
          {
            std::vector<int> pos(3);
            pos[0] = x;
            pos[1] = y;
            pos[2] = z;

            PointType sample_point = center;
            for (int j = 0; j != 3; ++j)
              sample_point[j] += pos[j]*sample_size[j];

            if (viennagrid::is_inside(*cit, sample_point, is_inside_tolerance()))
            {
              char & sample = access_symmetric(sample_regions, sample_count, pos);

              if ((sample == -1) || (region_priority[region_id] > region_priority[sample]))
                sample = region_id;
            }
          }
    }

    info(1) << "Finished regional sampling" << std::endl;




    typedef viennagrid::result_of::region<MeshType>::type RegionType;
//     std::vector<RegionType> regions;

    std::vector<char> & used_samples = sample_regions;

    for (int z = -sample_count[2]/2; z < sample_count[2]/2; ++z)
      for (int y = -sample_count[1]/2; y < sample_count[1]/2; ++y)
        for (int x = -sample_count[0]/2; x < sample_count[0]/2; ++x)
        {
          int r0 = access_symmetric(used_samples, sample_count, x  , y  , z  );
          int r1 = access_symmetric(used_samples, sample_count, x+1, y  , z  );
          int r2 = access_symmetric(used_samples, sample_count, x  , y+1, z  );
          int r3 = access_symmetric(used_samples, sample_count, x+1, y+1, z  );
          int r4 = access_symmetric(used_samples, sample_count, x  , y  , z+1);
          int r5 = access_symmetric(used_samples, sample_count, x+1, y  , z+1);
          int r6 = access_symmetric(used_samples, sample_count, x  , y+1, z+1);
          int r7 = access_symmetric(used_samples, sample_count, x+1, y+1, z+1);

          if (r0 == r1 && r0 == r2 && r0 == r3 && r0 == r4 && r0 == r5 && r0 == r6 && r0 == r7)
            continue;


          marching_cube mc(r0, r1, r2, r3, r4, r5, r6, r7);
          mc.make_lines(region_priority);
          std::vector<poly_line> poly_lines = mc.make_poly_lines();

          PointType p[8];
          p[0] = viennagrid::make_point( sample_size[0] *  x,    sample_size[1] *  y,    sample_size[2] * z     );
          p[1] = viennagrid::make_point( sample_size[0] * (x+1), sample_size[1] *  y,    sample_size[2] * z     );
          p[2] = viennagrid::make_point( sample_size[0] *  x   , sample_size[1] * (y+1), sample_size[2] * z     );
          p[3] = viennagrid::make_point( sample_size[0] * (x+1), sample_size[1] * (y+1), sample_size[2] * z     );
          p[4] = viennagrid::make_point( sample_size[0] *  x   , sample_size[1] *  y   , sample_size[2] * (z+1) );
          p[5] = viennagrid::make_point( sample_size[0] * (x+1), sample_size[1] *  y   , sample_size[2] * (z+1) );
          p[6] = viennagrid::make_point( sample_size[0] *  x   , sample_size[1] * (y+1), sample_size[2] * (z+1) );
          p[7] = viennagrid::make_point( sample_size[0] * (x+1), sample_size[1] * (y+1), sample_size[2] * (z+1) );

          for (int i = 0; i != 8; ++i)
            p[i] += center;

          for (std::size_t i = 0; i != poly_lines.size(); ++i)
          {
            poly_line const & pl = poly_lines[i];

//             ElementType v0 = viennagrid::make_unique_vertex( output_mesh(), mc.point(pl.vertex_indices[0], p), 1e-6 );
//             ElementType v_prev = viennagrid::make_unique_vertex( output_mesh(), mc.point(pl.vertex_indices[1], p), 1e-6 );

            ElementType v0 = viennagrid::make_vertex( output_mesh(), mc.point(pl.vertex_indices[0], p) );
            ElementType v_prev = viennagrid::make_vertex( output_mesh(), mc.point(pl.vertex_indices[1], p) );

            for (std::size_t j = 2; j != pl.vertex_indices.size(); ++j)
            {
              ElementType v_cur = viennagrid::make_vertex( output_mesh(), mc.point(pl.vertex_indices[j], p) );
//               ElementType v_cur = viennagrid::make_unique_vertex( output_mesh(), mc.point(pl.vertex_indices[j], p), 1e-6 );
              ElementType triangle = viennagrid::make_triangle( output_mesh(), v0, v_prev, v_cur );
              v_prev = v_cur;

//               if (pl.regions.first != -1)
                viennagrid::add( output_mesh().get_or_create_region(pl.regions.first+1), triangle );
//               if (pl.regions.second != -1)
                viennagrid::add( output_mesh().get_or_create_region(pl.regions.second+1), triangle );
            }
          }
        }



//     point_container_handle input_mc_regions = get_required_input<point_container_handle>("mc_regions");
//     PointType mc_regions;
//     convert( input_mc_regions(), mc_regions );
//
//     std::cout << "MC regions: " << mc_regions << std::endl;



//     marching_cube mc(mc_regions[0]+0.5, mc_regions[1]+0.5, mc_regions[2]+0.5, mc_regions[3]+0.5,
//                      mc_regions[4]+0.5, mc_regions[5]+0.5, mc_regions[6]+0.5, mc_regions[7]+0.5);
// //     marching_cube mc(2, 1, 1, 2, 3, 1, 3, 1);
//     mc.make_lines(region_priority);
//     std::vector< std::vector<int> > poly_lines = mc.make_poly_lines();
//
//     PointType p[8];
//     p[0] = viennagrid::make_point(-1, -1, -1);
//     p[1] = viennagrid::make_point( 1, -1, -1);
//     p[2] = viennagrid::make_point(-1,  1, -1);
//     p[3] = viennagrid::make_point( 1,  1, -1);
//     p[4] = viennagrid::make_point(-1, -1,  1);
//     p[5] = viennagrid::make_point( 1, -1,  1);
//     p[6] = viennagrid::make_point(-1,  1,  1);
//     p[7] = viennagrid::make_point( 1,  1,  1);
//
//     for (std::size_t i = 0; i != poly_lines.size(); ++i)
//     {
//       ElementType v0 = viennagrid::make_unique_vertex( output_mesh(), mc.point(poly_lines[i][0], p), 1e-6 );
//       ElementType v_prev = viennagrid::make_unique_vertex( output_mesh(), mc.point(poly_lines[i][1], p), 1e-6 );
//
//       for (std::size_t j = 2; j != poly_lines[i].size(); ++j)
//       {
//         ElementType v_cur = viennagrid::make_unique_vertex( output_mesh(), mc.point(poly_lines[i][j], p), 1e-6 );
//         viennagrid::make_triangle( output_mesh(), v0, v_prev, v_cur );
//         v_prev = v_cur;
//       }
//     }





//     for (int z = -sample_count[2]/2; z <= sample_count[2]/2; ++z)
//       for (int y = -sample_count[1]/2; y <= sample_count[1]/2; ++y)
//         for (int x = -sample_count[0]/2; x <= sample_count[0]/2; ++x)
//         {
//           std::vector<int> pos(3);
//           pos[0] = x;
//           pos[1] = y;
//           pos[2] = z;
//
//
//           char region_id = access_symmetric(sample_regions, sample_count, pos);
//           if (region_id == -1)
//             continue;
//
//           PointType sample_point = center;
//           for (int i = 0; i != 3; ++i)
//             sample_point[i] += pos[i]*sample_size[i];
//
//           PointType p[8];
//           p[0] = sample_point + viennagrid::make_point( -sample_size[0]/2, -sample_size[1]/2, -sample_size[2]/2 );
//           p[1] = sample_point + viennagrid::make_point(  sample_size[0]/2, -sample_size[1]/2, -sample_size[2]/2 );
//           p[2] = sample_point + viennagrid::make_point( -sample_size[0]/2,  sample_size[1]/2, -sample_size[2]/2 );
//           p[3] = sample_point + viennagrid::make_point(  sample_size[0]/2,  sample_size[1]/2, -sample_size[2]/2 );
//           p[4] = sample_point + viennagrid::make_point( -sample_size[0]/2, -sample_size[1]/2,  sample_size[2]/2 );
//           p[5] = sample_point + viennagrid::make_point(  sample_size[0]/2, -sample_size[1]/2,  sample_size[2]/2 );
//           p[6] = sample_point + viennagrid::make_point( -sample_size[0]/2,  sample_size[1]/2,  sample_size[2]/2 );
//           p[7] = sample_point + viennagrid::make_point(  sample_size[0]/2,  sample_size[1]/2,  sample_size[2]/2 );
//
//           ElementType vertices[8];
//           for (int i = 0; i != 8; ++i)
//             vertices[i] = viennagrid::make_vertex( output_mesh(), p[i] );
//
//           viennagrid::make_element<viennagrid::hexahedron_tag>( output_mesh().get_make_region(region_id), vertices, vertices+8 );
//         }

    set_output( "mesh", output_mesh );

    return true;
  }

}
