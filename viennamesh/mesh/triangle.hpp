#ifndef VIENNAMESH_MESH_TRIANGLE_HPP
#define VIENNAMESH_MESH_TRIANGLE_HPP

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/mesh/segmentation.hpp"
#include "viennamesh/core/convert.hpp"
#include "viennamesh/core/parameter.hpp"


// defines for triangle library
#define REAL double
#define VOID int
#define ANSI_DECLARATORS
#define TRILIBRARY
extern "C"
{
  #include "triangle/triangle.h"
}



namespace viennamesh
{
  struct triangle_mesh
  {
    triangle_mesh()
    {
      mesh.pointlist = NULL;
      mesh.pointattributelist = NULL;
      mesh.pointmarkerlist = NULL;
      mesh.numberofpoints = 0;
      mesh.numberofpointattributes = 0;

      mesh.trianglelist = NULL;
      mesh.triangleattributelist = NULL;
      mesh.trianglearealist = NULL;
      mesh.neighborlist = NULL;
      mesh.numberoftriangles = 0;
      mesh.numberofcorners = 0;
      mesh.numberoftriangleattributes = 0;

      mesh.segmentlist = NULL;
      mesh.segmentmarkerlist = NULL;
      mesh.numberofsegments = 0;

      mesh.holelist = NULL;
      mesh.numberofholes = 0;

      mesh.regionlist = NULL;
      mesh.numberofregions = 0;

      mesh.edgelist = NULL;
      mesh.edgemarkerlist = NULL;
      mesh.normlist = NULL;
      mesh.numberofedges = 0;
    }

    ~triangle_mesh()
    {
      if (mesh.pointlist) free(mesh.pointlist);
      if (mesh.pointattributelist) free(mesh.pointattributelist);
      if (mesh.pointmarkerlist) free(mesh.pointmarkerlist);

      if (mesh.trianglelist) free(mesh.trianglelist);
      if (mesh.triangleattributelist) free(mesh.triangleattributelist);
      if (mesh.trianglearealist) free(mesh.trianglearealist);
      if (mesh.neighborlist) free(mesh.neighborlist);

      if (mesh.segmentlist) free(mesh.segmentlist);
      if (mesh.segmentmarkerlist) free(mesh.segmentmarkerlist);

      if (mesh.holelist) free(mesh.holelist);

      if (mesh.regionlist) free(mesh.regionlist);

      if (mesh.edgelist) free(mesh.edgelist);
      if (mesh.edgemarkerlist) free(mesh.edgemarkerlist);
      if (mesh.normlist) free(mesh.normlist);
    }

    void init_points(int num_points)
    {
      if (mesh.pointlist) free(mesh.pointlist);
      mesh.pointlist = (REAL*)malloc( sizeof(REAL) * 2 * num_points);
      mesh.numberofpoints = num_points;
    }

    void init_segments(int num_segments)
    {
      if (mesh.segmentlist) free(mesh.segmentlist);
      mesh.segmentlist = (int*)malloc( sizeof(int) * 2 * num_segments);
      mesh.numberofsegments = num_segments;
    }

    void init_regions(int num_regions)
    {
      if (mesh.regionlist) free(mesh.regionlist);
      mesh.regionlist = (REAL*)malloc( sizeof(REAL) * 4 * num_regions);
      mesh.numberofregions = num_regions;
    }

    void init_holes(int num_holes)
    {
      if (mesh.holelist) free(mesh.holelist);
      mesh.holelist = (REAL*)malloc( sizeof(REAL) * 2 * num_holes);
      mesh.numberofholes = num_holes;
    }

    triangulateio mesh;
  };
}




namespace viennamesh
{



  template<typename WrappedContigT>
  void triangle_convert_generic( viennagrid::mesh<WrappedContigT> const & input, triangle_mesh & output )
  {
    typedef viennagrid::mesh<WrappedContigT> InputMeshType;
    typedef typename viennagrid::result_of::const_vertex_handle<InputMeshType>::type ConstVertexHandle;

    typedef typename viennagrid::result_of::const_vertex_range<InputMeshType>::type ConstVertexRange;
    typedef typename viennagrid::result_of::iterator<ConstVertexRange>::type ConstVertexIterator;

    typedef typename viennagrid::result_of::const_line_range<InputMeshType>::type ConstLineRange;
    typedef typename viennagrid::result_of::iterator<ConstLineRange>::type ConstCellIterator;


    std::map<ConstVertexHandle, int> vertex_handle_to_tetgen_index_map;

    ConstVertexRange vertices(input);
    output.init_points( vertices.size() );

    int index = 0;
    for (ConstVertexIterator vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
    {
      output.mesh.pointlist[index*2+0] = viennagrid::point(input, *vit)[0];
      output.mesh.pointlist[index*2+1] = viennagrid::point(input, *vit)[1];

      vertex_handle_to_tetgen_index_map[ vit.handle() ] = index;
    }


    ConstLineRange lines(input);
    output.init_segments( lines.size() );

    index = 0;
    for (ConstCellIterator lit = lines.begin(); lit != lines.end(); ++lit, ++index)
    {
      output.mesh.segmentlist[2*index+0] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lit).handle_at(0) ];
      output.mesh.segmentlist[2*index+1] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lit).handle_at(1) ];
    }
  }


  template<>
  struct convert_impl<viennagrid::plc_2d_mesh, NoSegmentation, triangle_mesh, NoSegmentation>
  {
    typedef viennagrid::plc_2d_mesh vgrid_mesh_type;
    typedef triangle_mesh triangle_mesh_type;

    typedef vgrid_mesh_type input_mesh_type;
    typedef NoSegmentation input_segmentation_type;
    typedef triangle_mesh_type output_mesh_type;
    typedef NoSegmentation output_segmentation_type;

    static bool convert( input_mesh_type const & vgrid_mesh, input_segmentation_type const & input_segmentation,
                          output_mesh_type & tri_mesh, output_segmentation_type & output_segmentation )
    {
      triangle_convert_generic(vgrid_mesh, tri_mesh);
      return true;
    }
  };

  template<>
  struct convert_impl<viennagrid::line_2d_mesh, NoSegmentation, triangle_mesh, NoSegmentation>
  {
    typedef viennagrid::line_2d_mesh vgrid_mesh_type;
    typedef triangle_mesh triangle_mesh_type;

    typedef vgrid_mesh_type input_mesh_type;
    typedef NoSegmentation input_segmentation_type;
    typedef triangle_mesh_type output_mesh_type;
    typedef NoSegmentation output_segmentation_type;

    static bool convert( input_mesh_type const & vgrid_mesh, input_segmentation_type const & input_segmentation,
                          output_mesh_type & tri_mesh, output_segmentation_type & output_segmentation )
    {
      triangle_convert_generic(vgrid_mesh, tri_mesh);
      return true;
    }
  };




  template<>
  struct convert_impl<triangle_mesh, NoSegmentation, viennagrid::triangular_2d_mesh, NoSegmentation>
  {
    typedef viennagrid::triangular_2d_mesh vgrid_mesh_type;

    typedef triangle_mesh input_mesh_type;
    typedef NoSegmentation input_segmentation_type;
    typedef vgrid_mesh_type output_mesh_type;
    typedef NoSegmentation output_segmentation_type;

    static bool convert( input_mesh_type const & tri_mesh, input_segmentation_type const & input_segmentation,
                          output_mesh_type & vgrid_mesh, output_segmentation_type & output_segmentation )
    {
      typedef viennagrid::result_of::point<vgrid_mesh_type>::type PointType;

      typedef viennagrid::result_of::vertex_handle<vgrid_mesh_type>::type VertexHandle;

      typedef viennagrid::result_of::element<vgrid_mesh_type, viennagrid::triangle_tag>::type triangle_type;
      typedef viennagrid::result_of::handle<vgrid_mesh_type, viennagrid::triangle_tag>::type triangle_handle_type;

      std::vector<VertexHandle> vertex_handles(tri_mesh.mesh.numberofpoints);

      std::cout << "Num attribs: " << tri_mesh.mesh.numberoftriangleattributes << std::endl;

      for (int i = 0; i < tri_mesh.mesh.numberofpoints; ++i)
      {
        PointType point;

        point[0] = tri_mesh.mesh.pointlist[2*i+0];
        point[1] = tri_mesh.mesh.pointlist[2*i+1];

        vertex_handles[i] = viennagrid::make_vertex( vgrid_mesh, point );
      }


      for (int i = 0; i < tri_mesh.mesh.numberoftriangles; ++i)
      {
        if (tri_mesh.mesh.numberoftriangleattributes > 0)
          std::cout << tri_mesh.mesh.triangleattributelist[i] << std::endl;

        viennagrid::make_triangle(
          vgrid_mesh,
          vertex_handles[ tri_mesh.mesh.trianglelist[3*i+0] ],
          vertex_handles[ tri_mesh.mesh.trianglelist[3*i+1] ],
          vertex_handles[ tri_mesh.mesh.trianglelist[3*i+2] ]
        );
      }

      return true;
    }
  };


  template<>
  struct convert_impl<triangle_mesh, NoSegmentation, viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation>
  {
    typedef viennagrid::triangular_2d_mesh vgrid_mesh_type;

    typedef triangle_mesh input_mesh_type;
    typedef NoSegmentation input_segmentation_type;
    typedef vgrid_mesh_type output_mesh_type;
    typedef viennagrid::triangular_2d_segmentation output_segmentation_type;

    static bool convert( input_mesh_type const & tri_mesh, input_segmentation_type const & input_segmentation,
                          output_mesh_type & vgrid_mesh, output_segmentation_type & output_segmentation )
    {
      typedef viennagrid::result_of::point<vgrid_mesh_type>::type PointType;

      typedef viennagrid::result_of::vertex_handle<vgrid_mesh_type>::type VertexHandle;

      typedef viennagrid::result_of::element<vgrid_mesh_type, viennagrid::triangle_tag>::type triangle_type;
      typedef viennagrid::result_of::handle<vgrid_mesh_type, viennagrid::triangle_tag>::type triangle_handle_type;

      std::vector<VertexHandle> vertex_handles(tri_mesh.mesh.numberofpoints);

//       std::cout << "Num attribs: " << tri_mesh.mesh.numberoftriangleattributes << std::endl;

      for (int i = 0; i < tri_mesh.mesh.numberofpoints; ++i)
      {
        PointType point;

        point[0] = tri_mesh.mesh.pointlist[2*i+0];
        point[1] = tri_mesh.mesh.pointlist[2*i+1];

        vertex_handles[i] = viennagrid::make_vertex( vgrid_mesh, point );
      }


      for (int i = 0; i < tri_mesh.mesh.numberoftriangles; ++i)
      {
        if (tri_mesh.mesh.numberoftriangleattributes == 0)
        {
          viennagrid::make_triangle(
            vgrid_mesh,
            vertex_handles[ tri_mesh.mesh.trianglelist[3*i+0] ],
            vertex_handles[ tri_mesh.mesh.trianglelist[3*i+1] ],
            vertex_handles[ tri_mesh.mesh.trianglelist[3*i+2] ]
          );
        }
        else
        {
          output_segmentation_type::segment_id_type segment_id = tri_mesh.mesh.triangleattributelist[i];

          viennagrid::make_triangle(
            output_segmentation.get_make_segment(segment_id),
            vertex_handles[ tri_mesh.mesh.trianglelist[3*i+0] ],
            vertex_handles[ tri_mesh.mesh.trianglelist[3*i+1] ],
            vertex_handles[ tri_mesh.mesh.trianglelist[3*i+2] ]
          );
        }
      }

      return true;
    }
  };

}

#endif
