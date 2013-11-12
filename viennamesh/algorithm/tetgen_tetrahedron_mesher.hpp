#ifndef VIENNAMESH_ALGORITHM_TETGEN_TETRAHEDRON_MESHER_HPP
#define VIENNAMESH_ALGORITHM_TETGEN_TETRAHEDRON_MESHER_HPP

#include "viennamesh/core/dynamic_algorithm.hpp"
#include "viennamesh/mesh/tetgen_tetrahedron.hpp"



namespace viennamesh
{
  struct tetgen_tetrahedron_tag {};

  namespace result_of
  {
    template<>
    struct native_input_mesh_wrapper<tetgen_tetrahedron_tag>
    {
      typedef MeshWrapper<tetgen_tetrahedron_mesh, NoSegmentation> type;
    };

    template<>
    struct native_output_mesh_wrapper<tetgen_tetrahedron_tag>
    {
      typedef MeshWrapper<tetgen_tetrahedron_mesh, NoSegmentation> type;
    };

    template<>
    struct works_in_place<tetgen_tetrahedron_tag>
    {
      static const bool value = false;
    };

    template<>
    struct algorithm_info<tetgen_tetrahedron_tag>
    {
      static const std::string name() { return "Tetgen 1.4.3 Triangle Hull to Tetrahedron Mesher"; }
    };

    template<typename mesh_type>
    struct best_matching_native_input_mesh<tetgen_tetrahedron_tag, mesh_type>
    {
      typedef tetgen_tetrahedron_mesh type;
    };

    template<typename mesh_type>
    struct best_matching_native_output_mesh<tetgen_tetrahedron_tag, mesh_type>
    {
      typedef tetgen_tetrahedron_mesh type;
    };


    template<typename mesh_type>
    struct best_matching_native_input_segmentation<tetgen_tetrahedron_tag, mesh_type>
    {
      typedef NoSegmentation type;
    };

    template<typename mesh_type>
    struct best_matching_native_output_segmentation<tetgen_tetrahedron_tag, mesh_type>
    {
      typedef NoSegmentation type;
    };
  }




  bool test_volume(REAL * p0, REAL * p1, REAL * p2, REAL * p3, REAL * elen, REAL volume)
  {
    return volume < 5.0;

    REAL center[3];
    center[0] = (p0[0]+p1[0]+p2[0]+p3[0])/4;
    center[1] = (p0[1]+p1[1]+p2[1]+p3[1])/4;
    center[2] = (p0[2]+p1[2]+p2[2]+p3[2])/4;

    if (center[2] >= -10.0)
      return volume < (center[2]+11);
    else
      return true;
  }





  typedef std::vector< std::pair<viennagrid::config::point_type_3d, int> > seed_point_3d_container;

  template<>
  struct static_init<seed_point_3d_container>
  {
    typedef seed_point_3d_container SelfT;

    static void init()
    {
      static bool to_init = true;
      if (to_init)
      {
        to_init = false;
        info(10) << "static_init<seed_point_3d_container>::init" << std::endl;
      }
    }
  };




  typedef std::vector<viennagrid::config::point_type_3d> point_3d_container;


  template<>
  struct static_init<point_3d_container>
  {
    typedef point_3d_container SelfT;

    static void init()
    {
      static bool to_init = true;
      if (to_init)
      {
        to_init = false;
        info(10) << "static_init<point_3d_container>::init" << std::endl;
      }
    }
  };






  template<>
  struct native_algorithm_impl<tetgen_tetrahedron_tag>
  {
    typedef tetgen_tetrahedron_tag algorithm_tag;

    template<typename native_input_mesh_type, typename native_output_mesh_type>
    static algorithm_feedback run( native_input_mesh_type const & native_input_mesh, native_output_mesh_type & native_output_mesh, ConstParameterSet const & parameters )
    {
      algorithm_feedback feedback( result_of::algorithm_info<algorithm_tag>::name() );
      ConstDoubleParameterHandle param;

      tetgenbehavior tetgen_settings;
//             tetgen_settings.quiet = 1;
      tetgen_settings.plc = 1;

      param = parameters.get<double>("cell_radius_edge_ratio");
//       param = parameters.get_scalar("cell_radius_edge_ratio");
      if (param)
      {
        tetgen_settings.quality = 1;
        tetgen_settings.minratio = param->value;
      }

//       param = parameters.get_scalar("cell_size");
      param = parameters.get<double>("cell_size");
      if (param)
      {
        tetgen_settings.fixedvolume = 1;
        tetgen_settings.maxvolume = param->value;
      }

      tetgen_settings.steiner = -1;     // Steiner Points?? -1 = unlimited, 0 = no steiner points
//      tetgen_settings.metric = 1;
//      const_cast<tetgenio::TetSizeFunc&>(native_input_mesh.tetunsuitable) = test_volume;


      tetgen_settings.useshelles = tetgen_settings.plc || tetgen_settings.refine || tetgen_settings.coarse || tetgen_settings.quality; // tetgen.cxx:3008
      tetgen_settings.goodratio = tetgen_settings.minratio; // tetgen.cxx:3009
      tetgen_settings.goodratio *= tetgen_settings.goodratio; // tetgen.cxx:3010

      // tetgen.cxx:3040
      if (tetgen_settings.fixedvolume || tetgen_settings.varvolume) {
        if (tetgen_settings.quality == 0) {
          tetgen_settings.quality = 1;
        }
      }

      tetgen_settings.goodangle = cos(tetgen_settings.minangle * tetgenmesh::PI / 180.0);   // tetgen.cxx:3046
      tetgen_settings.goodangle *= tetgen_settings.goodangle;                               // tetgen.cxx:3047


      tetgenio & tmp = (tetgenio&)native_input_mesh;

      int old_numberofregions = tmp.numberofregions;
      REAL * old_regionlist = tmp.regionlist;

      tmp.numberofregions = 0;
      tmp.regionlist = NULL;

      int old_numberofholes = tmp.numberofholes;
      REAL * old_holelist = tmp.holelist;

      tmp.numberofholes = 0;
      tmp.holelist = NULL;

      typedef viennamesh::result_of::const_parameter_handle<seed_point_3d_container>::type ConstSeedPointContainerHandle;
      ConstSeedPointContainerHandle seed_points_handle = parameters.get<seed_point_3d_container>("seed_points");
      if (seed_points_handle && !seed_points_handle->value.empty())
      {
        info(5) << "Found seed points" << std::endl;

        seed_point_3d_container const & seed_points = seed_points_handle->value;

        tmp.numberofregions = seed_points.size();
        tmp.regionlist = new REAL[5 * seed_points.size()];

        for (int i = 0; i < seed_points.size(); ++i)
        {
          info(10) << "  Seed Point " << seed_points[i].first << " for segment " << seed_points[i].second << std::endl;
          tmp.regionlist[5*i+0] = seed_points[i].first[0];
          tmp.regionlist[5*i+1] = seed_points[i].first[1];
          tmp.regionlist[5*i+2] = seed_points[i].first[2];
          tmp.regionlist[5*i+3] = REAL(seed_points[i].second);
          tmp.regionlist[5*i+4] = REAL(seed_points[i].second);
        }

        tetgen_settings.regionattrib = 1;
      }


      typedef viennamesh::result_of::const_parameter_handle<point_3d_container>::type ConstPointContainerHandle;
      ConstPointContainerHandle hole_points_handle = parameters.get<point_3d_container>("hole_points");
      if (hole_points_handle && !hole_points_handle->value.empty())
      {
        info(5) << "Found hole points" << std::endl;

        point_3d_container const & hole_points = hole_points_handle->value;

        tmp.numberofholes = hole_points.size();
        tmp.holelist = new REAL[3 * hole_points.size()];

        for (int i = 0; i < hole_points.size(); ++i)
        {
          info(10) << "  Hole Point " << hole_points[i] << std::endl;
          tmp.holelist[3*i+0] = hole_points[i][0];
          tmp.holelist[3*i+1] = hole_points[i][1];
          tmp.holelist[3*i+2] = hole_points[i][2];
        }
      }

      tetrahedralize(&tetgen_settings, &tmp, &native_output_mesh);

      delete[] tmp.regionlist;
      delete[] tmp.holelist;

      tmp.numberofregions = old_numberofregions;
      tmp.regionlist = old_regionlist;

      tmp.numberofholes = old_numberofholes;
      tmp.holelist = old_holelist;


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
