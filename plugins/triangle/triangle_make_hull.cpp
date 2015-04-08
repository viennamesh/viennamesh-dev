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

#include "viennagridpp/algorithm/refine.hpp"

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

      if (maxlen > max_length*max_length)
        return 1;
      else
        return 0;
    }





    struct cell_3d
    {
      triangulateio plc;

      std::vector<point_t> hole_points_2d;

      std::vector<int> global_vertex_ids;
      std::vector<int> region_ids;

      viennagrid::plane_to_2d_projector<point_t> projection_functor;
    };

    class mesh_3d
    {
    public:
      mesh_3d() : region_count(1) {}

      std::vector<cell_3d> cells;
      std::vector<point_t> vertex_points_3d;

      int region_count;
    };


    viennamesh_error convert_to_triangle_3d_cell(viennagrid::element_t const & input, triangle::cell_3d & output)
    {
      typedef viennagrid::element_t CellType;

//       typedef viennagrid::result_of::point<CellType>::type PointType;
      typedef viennagrid::result_of::const_element<CellType>::type ConstVertexType;

      typedef viennagrid::result_of::const_vertex_range<CellType>::type ConstVertexRangeType;
      typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

      typedef viennagrid::result_of::const_element_range<CellType, 1>::type ConstLineRangeType;
      typedef viennagrid::result_of::iterator<ConstLineRangeType>::type ConstCellIteratorType;


      std::map<ConstVertexType, int> vertex_handle_to_tetgen_index_map;

      ConstVertexRangeType vertices(input);
      viennamesh::triangle::init_points( output.plc, vertices.size() );

      std::vector<point_t> plc_points_3d( vertices.size() );
      std::vector<point_t> plc_points_2d( vertices.size() );

      int index = 0;
      for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
        plc_points_3d[index] = viennagrid::get_point(*vit);

      output.projection_functor.init( plc_points_3d.begin(), plc_points_3d.end(), 1e-6 );
      output.projection_functor.project( plc_points_3d.begin(), plc_points_3d.end(), plc_points_2d.begin() );


      output.global_vertex_ids.resize(vertices.size());

      index = 0;
      for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
      {
        output.plc.pointlist[index*2+0] = plc_points_2d[index][0];
        output.plc.pointlist[index*2+1] = plc_points_2d[index][1];

        vertex_handle_to_tetgen_index_map[ *vit ] = index;

        output.global_vertex_ids[index] = (*vit).id();
      }

      std::vector<point_t> const & hole_points_3d = viennagrid::hole_points(input);
      output.hole_points_2d.resize(hole_points_3d.size());
      output.projection_functor.project( hole_points_3d.begin(), hole_points_3d.end(), output.hole_points_2d.begin() );

      ConstLineRangeType lines(input);
      viennamesh::triangle::init_segments( output.plc, lines.size() );

      index = 0;
      for (ConstCellIteratorType lit = lines.begin(); lit != lines.end(); ++lit, ++index)
      {
        output.plc.segmentlist[2*index+0] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lit)[0] ];
        output.plc.segmentlist[2*index+1] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lit)[1] ];
      }

      return VIENNAMESH_SUCCESS;
    }






    viennamesh_error convert(viennagrid::mesh_t const & input, triangle::mesh_3d & output_)
    {
      triangle::mesh_3d & output = output_;

      typedef viennagrid::mesh_t MeshType;

      output.region_count = input.region_count();

      typedef viennagrid::result_of::element<MeshType>::type CellType;

      typedef viennagrid::result_of::const_vertex_range<MeshType>::type ConstVertexRangeType;
      typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

      typedef viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRangeType;
      typedef viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

      typedef viennagrid::result_of::region_range<MeshType, CellType>::type RegionRangeType;
      typedef viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;


      ConstVertexRangeType vertices(input);
      output.vertex_points_3d.resize( vertices.size() );

      for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
        output.vertex_points_3d[ (*vit).id() ] = viennagrid::get_point(*vit);

      ConstCellRangeType cells(input);
      output.cells.resize(cells.size());

      int index = 0;
      for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit, ++index)
      {
        convert_to_triangle_3d_cell(*cit, output.cells[index]);

        RegionRangeType regions(input, *cit);
        for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
          output.cells[index].region_ids.push_back( (*rit).id() );
      }

      return VIENNAMESH_SUCCESS;
    }

    viennamesh_error convert(triangle::mesh_3d const & input_, viennagrid::mesh_t const & output)
    {
      triangle::mesh_3d const & input = input_;

      typedef viennagrid::mesh_t MeshType;

      typedef viennagrid::result_of::point<MeshType>::type PointType;
      typedef viennagrid::result_of::element<MeshType>::type VertexType;
      typedef viennagrid::result_of::element<MeshType>::type CellType;

      std::vector<VertexType> vertices(input.vertex_points_3d.size());

      for (std::size_t i = 0; i != input.vertex_points_3d.size(); ++i)
      {
        vertices[i] = viennagrid::make_vertex( output, input.vertex_points_3d[i] );
      }

      for (unsigned int i = 0; i < input.cells.size(); ++i)
      {
        triangulateio const & plc = input.cells[i].plc;

        for (int j = 0; j < plc.numberoftriangles; ++j)
        {
          VertexType v[3];
          for (int k = 0; k != 3; ++k)
          {
            int index = plc.trianglelist[3*j+k];
            if (index < static_cast<int>(input.cells[i].global_vertex_ids.size()))
            {
              v[k] = vertices[ input.cells[i].global_vertex_ids[index] ];
            }
            else
            {
              PointType p2d = viennagrid::make_point(plc.pointlist[2*index+0], plc.pointlist[2*index+1]);
              PointType p3d = input.cells[i].projection_functor.unproject(p2d);

              v[k] = viennagrid::make_vertex( output, p3d );
            }
          }

          CellType cell = viennagrid::make_triangle( output, v[0], v[1], v[2] );
          for (unsigned int k = 0; k < input.cells[i].region_ids.size(); ++k)
          {
            if (input.region_count > 1)
              viennagrid::add(output.get_make_region(input.cells[j].region_ids[k]), cell);
          }
        }
      }

      return VIENNAMESH_SUCCESS;
    }







    make_hull::make_hull() {}

    std::string make_hull::name() { return "triangle_make_hull"; }

    bool make_hull::run(viennamesh::algorithm_handle &)
    {
      data_handle<viennagrid_mesh> input_mesh = get_input<viennagrid_mesh>("mesh");
      data_handle<viennagrid_mesh> output_mesh = make_data<viennagrid_mesh>();

      data_handle<double> min_angle = get_input<double>("min_angle");
      data_handle<double> cell_size = get_input<double>("cell_size");
      data_handle<bool> delaunay = get_input<bool>("delaunay");
      data_handle<viennamesh_string> algorithm_type = get_input<viennamesh_string>("algorithm_type");

      std::ostringstream options;
      options << "zpQY";

      viennagrid::mesh_t tmp;

      if (cell_size.valid())
      {
        viennagrid::refine_plc_lines( input_mesh(), tmp, cell_size() );

        max_length = cell_size();
        options << "u";
        should_triangle_be_refined = should_hull_triangle_be_refined_function;
      }
      else
        tmp = input_mesh();

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



      triangle::mesh_3d triangle_3d_input_mesh;
      convert(tmp, triangle_3d_input_mesh);

      triangle::mesh_3d triangle_3d_output_mesh;


      triangle_3d_output_mesh.cells.resize( triangle_3d_input_mesh.cells.size() );
      triangle_3d_output_mesh.vertex_points_3d = triangle_3d_input_mesh.vertex_points_3d;
      triangle_3d_output_mesh.region_count = triangle_3d_input_mesh.region_count;

      for (unsigned int i = 0; i < triangle_3d_input_mesh.cells.size(); ++i)
      {
        info(1) << "Create hull for mesh " << i << std::endl;
        triangulateio cur_tmp = triangle_3d_input_mesh.cells[i].plc;
        REAL * tmp_holelist = NULL;

        std::vector<point_t> const & hole_points_2d = triangle_3d_input_mesh.cells[i].hole_points_2d;

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

        {
          StdCaptureHandle capture_handle;
          triangulate( buffer, &cur_tmp, &triangle_3d_output_mesh.cells[i].plc, NULL);
        }

        triangle_3d_output_mesh.cells[i].global_vertex_ids = triangle_3d_input_mesh.cells[i].global_vertex_ids;
        triangle_3d_output_mesh.cells[i].region_ids = triangle_3d_input_mesh.cells[i].region_ids;
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

