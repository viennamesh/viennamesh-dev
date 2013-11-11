#ifndef VIENNAMESH_ALGORITHM_TRIANGLE_MESHER_HPP
#define VIENNAMESH_ALGORITHM_TRIANGLE_MESHER_HPP

#include <sstream>
#include "viennamesh/core/dynamic_algorithm.hpp"
#include "viennamesh/mesh/triangle.hpp"



namespace viennamesh
{
  struct triangle_tag {};

  namespace result_of
  {
    template<>
    struct native_input_mesh_wrapper<triangle_tag>
    {
      typedef MeshWrapper<triangle_mesh, NoSegmentation> type;
    };

    template<>
    struct native_output_mesh_wrapper<triangle_tag>
    {
      typedef MeshWrapper<triangle_mesh, NoSegmentation> type;
    };

    template<>
    struct works_in_place<triangle_tag>
    {
      static const bool value = false;
    };

    template<>
    struct algorithm_info<triangle_tag>
    {
      static const std::string name() { return "Triangle 1.6 delaunay triangle mesher"; }
    };

    template<typename mesh_type>
    struct best_matching_native_input_mesh<triangle_tag, mesh_type>
    {
      typedef triangle_mesh type;
    };

    template<typename mesh_type>
    struct best_matching_native_output_mesh<triangle_tag, mesh_type>
    {
      typedef triangle_mesh type;
    };


    template<typename mesh_type>
    struct best_matching_native_input_segmentation<triangle_tag, mesh_type>
    {
      typedef NoSegmentation type;
    };

    template<typename mesh_type>
    struct best_matching_native_output_segmentation<triangle_tag, mesh_type>
    {
      typedef NoSegmentation type;
    };
  }



  struct seed_point_2d
  {
    seed_point_2d(double x, double y, int segment_id_) : segment_id(segment_id_)
    {
      point[0] = x;
      point[1] = y;
    }

    double point[2];
    int segment_id;
  };

  typedef std::vector<seed_point_2d> seed_point_2d_container;


  template<>
  struct static_init<seed_point_2d_container>
  {
    typedef seed_point_2d_container SelfT;

    static void init()
    {
      static bool to_init = true;
      if (to_init)
      {
        to_init = false;
        info(10) << "static_init<seed_point_2d_container>::init" << std::endl;
      }
    }
  };


  template<>
  struct native_algorithm_impl<triangle_tag>
  {
    typedef triangle_tag algorithm_tag;

    template<typename native_input_mesh_type, typename native_output_mesh_type>
    static algorithm_feedback run( native_input_mesh_type const & native_input_mesh, native_output_mesh_type & native_output_mesh, ConstParameterSet const & parameters )
    {
      algorithm_feedback feedback( result_of::algorithm_info<algorithm_tag>::name() );
      ConstDoubleParameterHandle param;

      std::ostringstream options;
      options << "zpDq";
      param = parameters.get<double>("min_angle");
      if (param)
        options << param->value;

      triangulateio tmp = native_input_mesh.mesh;


      typedef viennamesh::result_of::const_parameter_handle<seed_point_2d_container>::type ConstSeedPointContainerHandle;

      ConstSeedPointContainerHandle seed_points_handle = parameters.get<seed_point_2d_container>("seed_points");
      if (seed_points_handle)
      {
        info(5) << "Using seed points" << std::endl;

        seed_point_2d_container const & seed_points = seed_points_handle->value;

        tmp.numberofregions = seed_points.size();
        if (tmp.regionlist) free(tmp.regionlist);
        tmp.regionlist = (REAL*)malloc( 4*sizeof(REAL)*tmp.numberofregions );

        for (int i = 0; i < seed_points.size(); ++i)
        {
          tmp.regionlist[4*i+0] = seed_points[i].point[0];
          tmp.regionlist[4*i+1] = seed_points[i].point[1];
          tmp.regionlist[4*i+2] = REAL(seed_points[i].segment_id);
          tmp.regionlist[4*i+3] = REAL(seed_points[i].segment_id);
        }

        options << "A";
      }

      param = parameters.get<double>("cell_size");
      if (param)
        options << "a" << param->value;

      char * buffer = (char *)malloc( options.str().length() * sizeof(char) );
      std::strcpy(buffer, options.str().c_str());

      triangulate( buffer, &tmp, &native_output_mesh.mesh, NULL);

      free(buffer);

      feedback.set_success();
      return feedback;
    }

    template<typename native_input_mesh_type, typename native_input_segmentation_type, typename native_output_mesh_type, typename native_output_segmentation_type>
    static algorithm_feedback run( native_input_mesh_type const & native_input_mesh, native_input_segmentation_type const & input_segmentation, native_output_mesh_type & native_output_mesh, native_output_segmentation_type & native_output_segmentation, ConstParameterSet const & parameters )
    {
      return run(native_input_mesh, native_output_mesh, parameters);
    }

  };



}

#endif
