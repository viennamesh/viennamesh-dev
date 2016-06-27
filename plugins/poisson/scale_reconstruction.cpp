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
#include "poisson_mesh.hpp"
#include "scale_reconstruction.hpp"

namespace viennamesh
{
  namespace poisson
  {

    struct scale_options
    {
      int sample_size;        //number of samples to be take       (used to estimate the average neighborhood_radius )
      int neighborhood_size;  //number of points in a neighborhood (used to estimate the average neighborhood_radius )
      int scale;              //scale of the reconstruction (the higher the scale, the smoother the reconstruction
    };

    void scale_reconstruction_impl(Point_collection & input,
                                   viennagrid::mesh output,
                                   scale_options options)
    {
      typedef Reconstruction::Triple_const_iterator                   Triple_iterator;
      // Construct the reconstruction with parameters for
      // the neighborhood squared radius estimation.
      Reconstruction reconstruct( options.neighborhood_size, options.sample_size );
      // Add the points.
      reconstruct.insert( input.begin(), input.end() );
      // Advance the scale-space several steps.
      // This automatically estimates the scale-space.
      reconstruct.increase_scale( options.scale );

      typedef viennagrid::mesh                                   MeshType;
      typedef viennagrid::result_of::element<MeshType>::type    VertexType;
      std::vector<VertexType> vertex_handles(input.size());
      int i=0;

      for(Point_collection::iterator begin = input.begin();begin!=input.end();++begin,++i)
        vertex_handles[i] = viennagrid::make_vertex( output,
          viennagrid::make_point(begin->x(),begin->y(),begin->z()));

      for( std::size_t shell = 0; shell < reconstruct.number_of_shells(); ++shell )
        for( Triple_iterator it = reconstruct.shell_begin( shell ); it != reconstruct.shell_end( shell ); ++it )
        {
          viennagrid::make_triangle(
            output,vertex_handles[(*it)[0]],
                   vertex_handles[(*it)[1]],
                   vertex_handles[(*it)[2]]);
        }
    }

    scale_reconstruction::scale_reconstruction() {}

    std::string scale_reconstruction::name() { return "scale_reconstruction"; }

    bool scale_reconstruction::run(viennamesh::algorithm_handle &)
    {
      point_handle input_points = get_required_input<point_handle>("points");
      data_handle<int> sample_option = get_input<int>("neighborhood_sample_size");
      data_handle<int> neighborhood_option = get_input<int>("neighborhood_size");
      data_handle<int> scale_option = get_input<int>("scale");
      Point_collection points;
      for (int i = 0; i != input_points.size(); ++i)
        points.push_back(Point(input_points(i)[0],
                               input_points(i)[1],
                               input_points(i)[2]));
      scale_options options;
      if(sample_option.valid() && sample_option() > 0)
        options.sample_size=sample_option();
      else
        options.sample_size=100;

      if(neighborhood_option.valid() && neighborhood_option() > 0)
        options.neighborhood_size=neighborhood_option();
      else
        options.neighborhood_size=10;

      if(scale_option.valid() && scale_option() > 0)
        options.scale=scale_option();
      else
        options.scale=1;

      mesh_handle output = make_data<mesh_handle>();
      Point_collection & im = points;

      scale_reconstruction_impl(im,output(),options);
      set_output("mesh", output);
      return true;
    }
  }
}

// #endif
