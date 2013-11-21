#ifndef VIENNAMESH_ALGORITHM_TETGEN_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_TETGEN_GENERATOR_HPP

#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/algorithm/tetgen/mesh.hpp"


namespace viennamesh
{
  namespace tetgen
  {
    class algorithm : public base_algorithm
    {
    public:

      string name() const { return "Tetgen 1.4 mesher"; }

      bool run_impl()
      {
        viennamesh::result_of::const_parameter_handle<tetgen::input_mesh>::type input_mesh = get_required_input<tetgen::input_mesh>("default");

        output_parameter_proxy<tetgen::output_mesh> output_mesh = output_proxy<tetgen::output_mesh>("default");



        std::ostringstream options;
        options << "zp";

        const_double_parameter_handle cell_size = get_input<double>("cell_size");
        if (cell_size)
          options << "a" << cell_size();

        const_double_parameter_handle max_radius_edge_ratio = get_input<double>("max_radius_edge_ratio");
        const_double_parameter_handle min_dihedral_angle = get_input<double>("min_dihedral_angle");

        if (max_radius_edge_ratio && min_dihedral_angle)
          options << "q" << max_radius_edge_ratio() << "q" << min_dihedral_angle() / M_PI * 180.0;
        else if (max_radius_edge_ratio)
          options << "q" << max_radius_edge_ratio();
        else if (min_dihedral_angle)
          options << "qq" << min_dihedral_angle() / M_PI * 180.0;



//         tetgenbehavior tetgen_settings;
//   //             tetgen_settings.quiet = 1;
//         tetgen_settings.plc = 1;
//
//         param = inputs.get<double>("cell_radius_edge_ratio");
//         if (param)
//         {
//           tetgen_settings.quality = 1;
//           tetgen_settings.minratio = param->get();
//         }
//
//         param = inputs.get<double>("cell_size");
//         if (param)
//         {
//           tetgen_settings.fixedvolume = 1;
//           tetgen_settings.maxvolume = param->get();
//         }
//
//         tetgen_settings.steiner = -1;     // Steiner Points?? -1 = unlimited, 0 = no steiner points
//   //      tetgen_settings.metric = 1;
//   //      const_cast<tetgenio::TetSizeFunc&>(native_input_mesh.tetunsuitable) = test_volume;
//
//
//         tetgen_settings.useshelles = tetgen_settings.plc || tetgen_settings.refine || tetgen_settings.coarse || tetgen_settings.quality; // tetgen.cxx:3008
//         tetgen_settings.goodratio = tetgen_settings.minratio; // tetgen.cxx:3009
//         tetgen_settings.goodratio *= tetgen_settings.goodratio; // tetgen.cxx:3010
//
//         // tetgen.cxx:3040
//         if (tetgen_settings.fixedvolume || tetgen_settings.varvolume) {
//           if (tetgen_settings.quality == 0) {
//             tetgen_settings.quality = 1;
//           }
//         }
//
//         tetgen_settings.goodangle = cos(tetgen_settings.minangle * tetgenmesh::PI / 180.0);   // tetgen.cxx:3046
//         tetgen_settings.goodangle *= tetgen_settings.goodangle;                               // tetgen.cxx:3047


        tetgenio & tmp = (tetgenio&)input_mesh();

        int old_numberofregions = tmp.numberofregions;
        REAL * old_regionlist = tmp.regionlist;

        tmp.numberofregions = 0;
        tmp.regionlist = NULL;

        int old_numberofholes = tmp.numberofholes;
        REAL * old_holelist = tmp.holelist;

        tmp.numberofholes = 0;
        tmp.holelist = NULL;

        typedef viennamesh::result_of::const_parameter_handle<seed_point_3d_container>::type ConstSeedPointContainerHandle;
        ConstSeedPointContainerHandle seed_points_handle = get_input<seed_point_3d_container>("seed_points");
        if (seed_points_handle && !seed_points_handle().empty())
        {
          info(5) << "Found seed points" << std::endl;

          seed_point_3d_container const & seed_points = seed_points_handle();

          REAL * tmp_regionlist = new REAL[5 * (seed_points.size() + tmp.numberofregions)];
          memcpy( tmp_regionlist, tmp.regionlist, sizeof(REAL)*5*tmp.numberofregions );

          for (std::size_t i = 0; i < seed_points.size(); ++i)
          {
            tmp_regionlist[5*(tmp.numberofregions+i)+0] = seed_points[i].first[0];
            tmp_regionlist[5*(tmp.numberofregions+i)+1] = seed_points[i].first[1];
            tmp_regionlist[5*(tmp.numberofregions+i)+2] = seed_points[i].first[2];
            tmp_regionlist[5*(tmp.numberofregions+i)+3] = REAL(seed_points[i].second);
            tmp_regionlist[5*(tmp.numberofregions+i)+4] = 0;
          }

          tmp.numberofregions += seed_points.size();
          tmp.regionlist = tmp_regionlist;

//           tetgen_settings.regionattrib = 1;
          options << "A";
        }


        typedef viennamesh::result_of::const_parameter_handle<point_3d_container>::type ConstPointContainerHandle;
        ConstPointContainerHandle hole_points_handle = get_input<point_3d_container>("hole_points");
        if (hole_points_handle && !hole_points_handle().empty())
        {
          info(5) << "Found hole points" << std::endl;

          point_3d_container const & hole_points = hole_points_handle();


          REAL * tmp_holelist = new REAL[3 * (hole_points.size() + tmp.numberofholes)];
          memcpy( tmp_holelist, tmp.holelist, sizeof(REAL)*3*tmp.numberofholes );

          for (std::size_t i = 0; i < hole_points.size(); ++i)
          {
            tmp_holelist[3*(tmp.numberofholes+i)+0] = hole_points[i][0];
            tmp_holelist[3*(tmp.numberofholes+i)+1] = hole_points[i][1];
            tmp_holelist[3*(tmp.numberofholes+i)+2] = hole_points[i][2];
          }

          tmp.numberofholes += hole_points.size();
          tmp.holelist = tmp_holelist;
        }


        char * buffer = new char[options.str().length()];
        std::strcpy(buffer, options.str().c_str());

        viennautils::StdCapture capture;
        capture.start();

        tetrahedralize(buffer, &tmp, &output_mesh());

        capture.finish();
        info(5) << capture.get() << std::endl;

        delete[] buffer;
        delete[] tmp.regionlist;
        delete[] tmp.holelist;

        tmp.numberofregions = old_numberofregions;
        tmp.regionlist = old_regionlist;

        tmp.numberofholes = old_numberofholes;
        tmp.holelist = old_holelist;

        return true;
      }

    private:
    };
  }

}



#endif
