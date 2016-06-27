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

#include "triangle_mesh.hpp"
#include "triangle_make_hull.hpp"

#include "viennagrid/algorithm/refine.hpp"

namespace viennamesh
{
  namespace triangle
  {

    double max_length = 0.0;
    int should_hull_triangle_be_refined_function(double * triorg, double * tridest, double * triapex, double)
    {
      REAL dxoa, dxda, dxod;
      REAL dyoa, dyda, dyod;
      REAL oalen, dalen, odlen;
      REAL maxlen;

      dxoa = triorg[0] - triapex[0];
      dyoa = triorg[1] - triapex[1];
      dxda = tridest[0] - triapex[0];
      dyda = tridest[1] - triapex[1];
      dxod = triorg[0] - tridest[0];
      dyod = triorg[1] - tridest[1];
      /* Find the squares of the lengths of the triangle's three edges. */
      oalen = dxoa * dxoa + dyoa * dyoa;
      dalen = dxda * dxda + dyda * dyda;
      odlen = dxod * dxod + dyod * dyod;
      /* Find the square of the length of the longest edge. */
      maxlen = (dalen > oalen) ? dalen : oalen;
      maxlen = (odlen > maxlen) ? odlen : maxlen;

//       std::cout << "           triangle (" << triorg[0] << "," << triorg[1] << ") (" << tridest[0] << "," << tridest[1] << ") (" << triapex[0] << "," << triapex[1] << ")" << std::endl;
//       std::cout << "     length = " << maxlen << "    max length = " << max_length << "       " << (maxlen > max_length*max_length) << std::endl;

      if (maxlen > max_length*max_length)
        return 1;
      else
        return 0;
    }





    struct cell_3d
    {
      triangulateio plc;

      std::vector<point> hole_points_2d;

      std::vector<int> global_vertex_ids;
//       std::vector<int> region_ids;

      viennagrid::plane_to_2d_projector<point> projection_functor;
    };

    class mesh_3d
    {
    public:
      mesh_3d() : region_count(1) {}

      std::vector<cell_3d> cells;
      std::vector<point> vertex_points_3d;

      int region_count;
    };


    viennamesh_error convert_to_triangle_3d_cell(viennagrid_plc plc, viennagrid_int facet_id, triangle::cell_3d & output)
    {
      viennagrid_dimension geometric_dimension;
      viennagrid_plc_geometric_dimension_get(plc, &geometric_dimension);

      std::map<viennagrid_int, int> vertex_map;

      viennagrid_int * vertices_begin;
      viennagrid_int * vertices_end;
      viennagrid_plc_boundary_elements(plc, facet_id, 0, &vertices_begin, &vertices_end);
      viennagrid_int vertex_count = vertices_end - vertices_begin;

      viennamesh::triangle::init_points( output.plc, vertex_count );

      std::vector<point> plc_points_3d( vertex_count );
      std::vector<point> plc_points_2d( vertex_count );

      int index = 0;
      for (viennagrid_int * vit = vertices_begin; vit != vertices_end; ++vit, ++index)
      {
        plc_points_3d[index] = viennagrid::get_point(plc, *vit);
      }

      output.projection_functor.init( plc_points_3d.begin(), plc_points_3d.end(), 1e-6 );
      output.projection_functor.project( plc_points_3d.begin(), plc_points_3d.end(), plc_points_2d.begin() );

      output.global_vertex_ids.resize( vertex_count );

      index = 0;
      for (viennagrid_int * vit = vertices_begin; vit != vertices_end; ++vit, ++index)
      {
        output.plc.pointlist[index*2+0] = plc_points_2d[index][0];
        output.plc.pointlist[index*2+1] = plc_points_2d[index][1];

        vertex_map[ *vit ] = index;
        output.global_vertex_ids[index] = *vit;
      }

      viennagrid_int hole_point_count;
      viennagrid_numeric * hole_points;
      viennagrid_plc_facet_hole_points_get(plc, facet_id, &hole_point_count, &hole_points);

      std::vector<point> hole_points_3d(hole_point_count);
      for (viennagrid_int i = 0; i != hole_point_count; ++i)
      {
        hole_points_3d[i].resize(geometric_dimension);
        std::copy( hole_points + i*geometric_dimension, hole_points + (i+1)*geometric_dimension, &hole_points_3d[i][0] );
      }
      output.hole_points_2d.resize(hole_points_3d.size());
      output.projection_functor.project( hole_points_3d.begin(), hole_points_3d.end(), output.hole_points_2d.begin() );

      viennagrid_int * lines_begin;
      viennagrid_int * lines_end;
      viennagrid_plc_boundary_elements(plc, facet_id, 1, &lines_begin, &lines_end);
      viennagrid_int line_count = lines_end - lines_begin;

      viennamesh::triangle::init_segments( output.plc, line_count );

      index = 0;
      for (viennagrid_int * lit = lines_begin; lit != lines_end; ++lit, ++index)
      {
        viennagrid_int * line_vertices_begin;
        viennagrid_int * line_vertices_end;
        viennagrid_plc_boundary_elements(plc, *lit, 0, &line_vertices_begin, &line_vertices_end);

        output.plc.segmentlist[2*index+0] = vertex_map[ *(line_vertices_begin+0) ];
        output.plc.segmentlist[2*index+1] = vertex_map[ *(line_vertices_begin+1) ];
      }

      return VIENNAMESH_SUCCESS;
    }






    viennamesh_error convert(viennagrid_plc plc, triangle::mesh_3d & output_)
    {
      triangle::mesh_3d & output = output_;

      viennagrid_dimension geometric_dimension;
      viennagrid_plc_geometric_dimension_get(plc, &geometric_dimension);

      viennagrid_element_id vertex_begin;
      viennagrid_element_id vertex_end;
      viennagrid_plc_elements_get(plc, 0, &vertex_begin, &vertex_end);

      viennagrid_element_id facet_begin;
      viennagrid_element_id facet_end;
      viennagrid_plc_elements_get(plc, 2, &facet_begin, &facet_end);


      output.vertex_points_3d.resize( vertex_end-vertex_begin );
      for (viennagrid_int vid = vertex_begin; vid != vertex_end; ++vid)
      {
        output.vertex_points_3d[vid] = viennagrid::get_point(plc, vid);

//         output.vertex_points_3d[vid].resize( geometric_dimension );

//         viennagrid_numeric * coords;
//         viennagrid_plc_vertex_get(plc, vid, &coords);
//         std::copy( coords, coords+geometric_dimension, &output.vertex_points_3d[vid][0] );
      }


      output.cells.resize( facet_end-facet_begin );
      for (viennagrid_int fid = facet_begin; fid != facet_end; ++fid)
      {
        convert_to_triangle_3d_cell(plc, fid, output.cells[viennagrid_index_from_element_id(fid)]);
      }


//       int index = 0;
//       for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit, ++index)
//       {
//         convert_to_triangle_3d_cell(*cit, output.cells[index]);
//
//         RegionRangeType regions(input, *cit);
//         for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
//           output.cells[index].region_ids.push_back( (*rit).id() );
//       }

      return VIENNAMESH_SUCCESS;
    }

    viennamesh_error convert(triangle::mesh_3d const & input_, viennagrid::mesh const & output)
    {
      triangle::mesh_3d const & input = input_;

      typedef viennagrid::mesh                                  MeshType;

      typedef viennagrid::result_of::point<MeshType>::type      PointType;
      typedef viennagrid::result_of::element<MeshType>::type    ElementType;

      std::vector<ElementType> vertices(input.vertex_points_3d.size());

      for (std::size_t i = 0; i != input.vertex_points_3d.size(); ++i)
      {
        vertices[i] = viennagrid::make_vertex( output, input.vertex_points_3d[i] );
      }

      for (unsigned int i = 0; i < input.cells.size(); ++i)
      {
        triangulateio const & plc = input.cells[i].plc;
        std::map<int, ElementType> vertex_map;

        for (int j = 0; j < plc.numberoftriangles; ++j)
        {
          ElementType v[3];
          for (int k = 0; k != 3; ++k)
          {
            int index = plc.trianglelist[3*j+k];

            if (index < static_cast<int>(input.cells[i].global_vertex_ids.size()))
            {
              v[k] = vertices[ input.cells[i].global_vertex_ids[index] ];
            }
            else
            {
              std::map<int, ElementType>::iterator it = vertex_map.find(index);
              if (it != vertex_map.end())
              {
                v[k] = it->second;
              }
              else
              {
                PointType p2d = viennagrid::make_point(plc.pointlist[2*index+0], plc.pointlist[2*index+1]);
                PointType p3d = input.cells[i].projection_functor.unproject(p2d);

                v[k] = viennagrid::make_vertex( output, p3d );
                vertex_map[index] = v[k];
              }
            }
          }

          viennagrid::make_triangle( output, v[0], v[1], v[2] );
        }
      }

      return VIENNAMESH_SUCCESS;
    }







    make_hull::make_hull() {}

    std::string make_hull::name() { return "triangle_make_hull"; }

    bool make_hull::run(viennamesh::algorithm_handle &)
    {
      data_handle<viennagrid_plc> input_plc = get_required_input<viennagrid_plc>("geometry");
      data_handle<viennagrid_mesh> output_mesh = make_data<viennagrid_mesh>();

      data_handle<double> min_angle = get_input<double>("min_angle");
      data_handle<double> cell_size = get_input<double>("cell_size");
      data_handle<bool> delaunay = get_input<bool>("delaunay");
      data_handle<viennamesh_string> algorithm_type = get_input<viennamesh_string>("algorithm_type");

      std::ostringstream options;
      options << "zpQYY";


      triangle::mesh_3d triangle_3d_input_mesh;

      if (cell_size.valid())
      {
        viennagrid_int line_count;
        viennagrid_plc_element_count(input_plc(), 1, &line_count);
        info(1) << "Line count of original mesh: " << line_count << std::endl;

        viennagrid_plc refined_plc;
        viennagrid_plc_create(&refined_plc);
        viennagrid_plc_line_refine( input_plc(), refined_plc, cell_size() );
        convert( refined_plc, triangle_3d_input_mesh );
        viennagrid_plc_release(refined_plc);

        max_length = cell_size();

        info(1) << "using cell size " << cell_size() << std::endl;

        options << "u";
        should_triangle_be_refined = should_hull_triangle_be_refined_function;
      }
      else
        convert( input_plc() , triangle_3d_input_mesh );

      if (min_angle.valid())
        options << "q" << min_angle() / M_PI * 180.0;

      if ( !delaunay.valid() || (delaunay.valid() && delaunay()) )
        options << "D";

      if (algorithm_type.valid())
      {
        if (algorithm_type() == "incremental_delaunay")
          options << "i";
        else if (algorithm_type() == "sweepline")
          options << "F";
        else if (algorithm_type() == "devide_and_conquer")
        {}
        else
        {
          warning(5) << "Algorithm not recognized: '" << algorithm_type() << "' supported algorithms:" << std::endl;
          warning(5) << "  'incremental_delaunay'" << std::endl;
          warning(5) << "  'sweepline'" << std::endl;
          warning(5) << "  'devide_and_conquer'" << std::endl;
        }
      }

      info(1) << "Making hull with option string " << options.str() << std::endl;






      triangle::mesh_3d triangle_3d_output_mesh;


      triangle_3d_output_mesh.cells.resize( triangle_3d_input_mesh.cells.size() );
      triangle_3d_output_mesh.vertex_points_3d = triangle_3d_input_mesh.vertex_points_3d;
      triangle_3d_output_mesh.region_count = triangle_3d_input_mesh.region_count;

      for (unsigned int i = 0; i < triangle_3d_input_mesh.cells.size(); ++i)
      {
        info(10) << "Create hull for mesh " << i << std::endl;
        triangulateio cur_tmp = triangle_3d_input_mesh.cells[i].plc;
        REAL * tmp_holelist = NULL;

        std::vector<point> const & hole_points_2d = triangle_3d_input_mesh.cells[i].hole_points_2d;

        if (!hole_points_2d.empty())
        {
          tmp_holelist = (REAL*)malloc( 2*sizeof(REAL)*(cur_tmp.numberofholes+hole_points_2d.size()) );
          memcpy( tmp_holelist, cur_tmp.holelist, 2*sizeof(REAL)*cur_tmp.numberofholes );

          for (std::size_t i = 0; i < hole_points_2d.size(); ++i)
          {
            tmp_holelist[2*(cur_tmp.numberofholes+i)+0] = hole_points_2d[i][0];
            tmp_holelist[2*(cur_tmp.numberofholes+i)+1] = hole_points_2d[i][1];
          }

          cur_tmp.numberofholes += hole_points_2d.size();
          cur_tmp.holelist = tmp_holelist;
        }

        char * buffer = new char[options.str().length()+1];
        std::strcpy(buffer, options.str().c_str());
//         info(1) << "   Using buffer " << buffer << std::endl;

        {
          StdCaptureHandle capture_handle;
          triangulate( buffer, &cur_tmp, &triangle_3d_output_mesh.cells[i].plc, NULL);
        }

        triangle_3d_output_mesh.cells[i].global_vertex_ids = triangle_3d_input_mesh.cells[i].global_vertex_ids;
        triangle_3d_output_mesh.cells[i].projection_functor = triangle_3d_input_mesh.cells[i].projection_functor;

        delete[] buffer;
        if (!hole_points_2d.empty())
          free(tmp_holelist);
      }


      convert(triangle_3d_output_mesh, output_mesh());

      set_output("mesh", output_mesh);

      return true;
    }



  }
}

