#ifndef VIENNAMESH_CORE_ALGORITHM_HPP
#define VIENNAMESH_CORE_ALGORITHM_HPP

#include "viennamesh/core/convert.hpp"
#include "viennamesh/utils/timer.hpp"


namespace viennamesh
{

  namespace result_of
  {
    template<typename algorithm_tag, typename mesh_type>
    struct best_matching_native_input_mesh;

    template<typename algorithm_tag, typename segmentation_type>
    struct best_matching_native_input_segmentation;

    template<typename algorithm_tag, typename mesh_type>
    struct best_matching_native_output_mesh;

    template<typename algorithm_tag, typename segmentation_type>
    struct best_matching_native_output_segmentation;


    template<typename algorithm_tag, typename mesh_type>
    struct is_native_input_mesh
    {
        static const bool value = viennagrid::detail::EQUAL<
                mesh_type,
                typename best_matching_native_input_mesh<algorithm_tag, mesh_type>::type
            >::value;
    };

    template<typename algorithm_tag, typename segmentation_type>
    struct is_native_input_segmentation
    {
        static const bool value = viennagrid::detail::EQUAL<
                segmentation_type,
                typename best_matching_native_input_segmentation<algorithm_tag, segmentation_type>::type
            >::value;
    };

    template<typename algorithm_tag, typename mesh_type, typename segmentation_type>
    struct is_native_input
    {
        static const bool value = is_native_input_mesh<algorithm_tag, mesh_type>::value &&
                                  is_native_input_segmentation<algorithm_tag, segmentation_type>::value;
    };



    template<typename algorithm_tag, typename mesh_type>
    struct is_native_output_mesh
    {
        static const bool value = viennagrid::detail::EQUAL<
                mesh_type,
                typename best_matching_native_output_mesh<algorithm_tag, mesh_type>::type
            >::value;
    };

    template<typename algorithm_tag, typename segmentation_type>
    struct is_native_output_segmentation
    {
        static const bool value = viennagrid::detail::EQUAL<
                segmentation_type,
                typename best_matching_native_output_segmentation<algorithm_tag, segmentation_type>::type
            >::value;
    };

    template<typename algorithm_tag, typename mesh_type, typename segmentation_type>
    struct is_native_output
    {
        static const bool value = is_native_output_mesh<algorithm_tag, mesh_type>::value &&
                                  is_native_output_segmentation<algorithm_tag, segmentation_type>::value;
    };




    template<typename algorithm_tag>
    struct works_in_place;

    template<typename algorithm_tag>
    struct algorithm_info;
  }





//     template<typename info_stream, typename warning_stream, typename error_stream>
  class algorithm_feedback_t
  {
  public:
      algorithm_feedback_t( std::string const & a_ ) : algorithm_(a_), success_(false), run_time_(0) {}

      std::string const & algorithm() const { return algorithm_; }

      bool success() const { return success_; }
      operator bool() const { return success(); }
      void set_success( bool s = true ) { success_ = s; }

      double run_time() const { return run_time_; }
      void set_run_time( double t ) { run_time_ = t; }

  private:
      const std::string algorithm_;

      bool success_;
      double run_time_;

      std::string info_;
      std::string warning_;
      std::string error_;
  };

  typedef algorithm_feedback_t algorithm_feedback;

  template<typename stream_type>
  inline stream_type & operator<<(stream_type & stream, algorithm_feedback_t const & feedback)
  {
      stream << "---------------------------------------------------------------\n\n";
      stream << "Algorithm:  " << feedback.algorithm() << "\n\n";
      stream << " sucess:    " << feedback.success() << "\n";
      stream << " run-time:  " << feedback.run_time() << " sec\n\n";
      stream << "---------------------------------------------------------------\n";

      return stream;
  }






  template<typename algorithm_tag>
  struct native_algorithm_impl;



  template<typename algorithm_tag, typename native_mesh_type, typename settings_type>
  algorithm_feedback run_algo_native( native_mesh_type & native_mesh, settings_type const & settings )
  {
      return native_algorithm_impl<algorithm_tag>::run(native_mesh, settings);
  }

  template<typename algorithm_tag, typename native_mesh_type, typename segmentation_type, typename settings_type>
  algorithm_feedback run_algo_native( native_mesh_type & native_mesh, segmentation_type & segmentation, settings_type const & settings )
  {
      return native_algorithm_impl<algorithm_tag>::run(native_mesh, segmentation, settings);
  }

  template<typename algorithm_tag, typename native_input_mesh_type, typename native_ouput_mesh_type, typename settings_type>
  algorithm_feedback run_algo_native( native_input_mesh_type const & native_input_mesh, native_ouput_mesh_type & native_output_mesh, settings_type const & settings )
  {
      return native_algorithm_impl<algorithm_tag>::run(native_input_mesh, native_output_mesh, settings);
  }

  template<typename algorithm_tag, typename native_input_mesh_type, typename input_segmentation_type, typename native_ouput_mesh_type, typename output_segmentation_type, typename settings_type>
  algorithm_feedback run_algo_native( native_input_mesh_type const & native_input_mesh, input_segmentation_type const & input_segmentation,
                        native_ouput_mesh_type & native_output_mesh, output_segmentation_type & output_segmentation,
                        settings_type const & settings )
  {
      return native_algorithm_impl<algorithm_tag>::run(native_input_mesh, input_segmentation, native_output_mesh, output_segmentation, settings);
  }





  template<typename algorithm_tag, bool algo_works_in_place, bool is_input_native, bool is_output_native>
  struct algorithm_impl;


  template<typename algorithm_tag>
  struct algorithm_impl<algorithm_tag, true, true, true>
  {
      template<typename input_mesh_type, typename output_mesh_type, typename settings_type>
      static algorithm_feedback run( input_mesh_type const & input_mesh,
                        output_mesh_type & output_mesh,
                        settings_type const & settings )
      {
          if (&input_mesh != &output_mesh)
              convert(input_mesh, output_mesh);

          return native_algorithm_impl<algorithm_tag>::run( output_mesh, settings );
      }

      template<typename input_mesh_type, typename input_segmentation_type, typename output_mesh_type, typename output_segmentation_type, typename settings_type>
      static algorithm_feedback run( input_mesh_type const & input_mesh, input_segmentation_type const & input_segmentation,
                        output_mesh_type & output_mesh, output_segmentation_type & output_segmentation,
                        settings_type const & settings )
      {
          if (&input_mesh != &output_mesh)
              convert(input_mesh, input_segmentation, output_mesh, output_segmentation);

          return native_algorithm_impl<algorithm_tag>::run( output_mesh, output_segmentation, settings );
      }
  };

  template<typename algorithm_tag>
  struct algorithm_impl<algorithm_tag, true, false, true>
  {
      template<typename input_mesh_type, typename output_mesh_type, typename settings_type>
      static algorithm_feedback run( input_mesh_type const & input_mesh,
                        output_mesh_type & output_mesh,
                        settings_type const & settings )
      {
          convert( input_mesh, output_mesh );

          return native_algorithm_impl<algorithm_tag>::run( output_mesh, settings );
      }

      template<typename input_mesh_type, typename input_segmentation_type, typename output_mesh_type, typename output_segmentation_type, typename settings_type>
      static algorithm_feedback run( input_mesh_type const & input_mesh, input_segmentation_type const & input_segmentation,
                        output_mesh_type & output_mesh, output_segmentation_type & output_segmentation,
                        settings_type const & settings )
      {
          convert( input_mesh, input_segmentation, output_mesh, output_segmentation );

          return native_algorithm_impl<algorithm_tag>::run( output_mesh, output_segmentation, settings );
      }
  };

  template<typename algorithm_tag>
  struct algorithm_impl<algorithm_tag, true, true, false>
  {
      template<typename input_mesh_type, typename output_mesh_type, typename settings_type>
      static algorithm_feedback run( input_mesh_type const & input_mesh,
                        output_mesh_type & output_mesh,
                        settings_type const & settings )
      {
          input_mesh_type tmp_mesh(input_mesh);

          algorithm_feedback result = native_algorithm_impl<algorithm_tag>::run( tmp_mesh, settings );

          convert( tmp_mesh, output_mesh );
          return result;
      }

      template<typename input_mesh_type, typename input_segmentation_type, typename output_mesh_type, typename output_segmentation_type, typename settings_type>
      static algorithm_feedback run( input_mesh_type const & input_mesh, input_segmentation_type const & input_segmentation,
                        output_mesh_type & output_mesh, output_segmentation_type & output_segmentation,
                        settings_type const & settings )
      {
//             typedef typename result_of::best_matching_native_output_mesh<algorithm_tag, output_mesh_type>::type native_output_mesh_type;
          input_mesh_type tmp_mesh(input_mesh);
          input_segmentation_type tmp_segmentation(input_segmentation);

          algorithm_feedback result = native_algorithm_impl<algorithm_tag>::run( tmp_mesh, tmp_segmentation, settings );

          convert( tmp_mesh, tmp_segmentation, output_mesh, output_segmentation );
          return result;
      }
  };

  template<typename algorithm_tag>
  struct algorithm_impl<algorithm_tag, true, false, false>
  {
      template<typename input_mesh_type, typename output_mesh_type, typename settings_type>
      static algorithm_feedback run( input_mesh_type const & input_mesh,
                        output_mesh_type & output_mesh,
                        settings_type const & settings )
      {
          typedef typename result_of::best_matching_native_output_mesh<algorithm_tag, output_mesh_type>::type native_output_mesh_type;
          native_output_mesh_type tmp_mesh;

          convert( input_mesh, tmp_mesh );

          algorithm_feedback result = native_algorithm_impl<algorithm_tag>::run( tmp_mesh, settings );

          convert( tmp_mesh, output_mesh );
          return result;
      }

      template<typename input_mesh_type, typename input_segmentation_type, typename output_mesh_type, typename output_segmentation_type, typename settings_type>
      static algorithm_feedback run( input_mesh_type const & input_mesh, input_segmentation_type const & input_segmentation,
                        output_mesh_type & output_mesh, output_segmentation_type & output_segmentation,
                        settings_type const & settings )
      {
          typedef typename result_of::best_matching_native_output_mesh<algorithm_tag, output_mesh_type>::type native_output_mesh_type;
          typedef typename result_of::best_matching_native_output_segmentation<algorithm_tag, output_segmentation_type>::type native_output_segmentation_type;
          native_output_mesh_type tmp_mesh;
          native_output_segmentation_type tmp_segmentation;

          convert( input_mesh, input_segmentation, tmp_mesh, tmp_segmentation );

          algorithm_feedback result = native_algorithm_impl<algorithm_tag>::run( tmp_mesh, tmp_segmentation, settings );

          convert( tmp_mesh, tmp_segmentation, output_mesh, output_segmentation );
          return result;
      }
  };

  template<typename algorithm_tag>
  struct algorithm_impl<algorithm_tag, false, true, true>
  {
      template<typename input_mesh_type, typename output_mesh_type, typename settings_type>
      static algorithm_feedback run( input_mesh_type const & input_mesh,
                        output_mesh_type & output_mesh,
                        settings_type const & settings )        {
          return native_algorithm_impl<algorithm_tag>::run( input_mesh, output_mesh, settings );
      }

      template<typename input_mesh_type, typename input_segmentation_type, typename output_mesh_type, typename output_segmentation_type, typename settings_type>
      static algorithm_feedback run( input_mesh_type const & input_mesh, input_segmentation_type const & input_segmentation,
                        output_mesh_type & output_mesh, output_segmentation_type & output_segmentation,
                        settings_type const & settings )
      {
          return native_algorithm_impl<algorithm_tag>::run( input_mesh, input_segmentation, output_mesh, output_segmentation, settings );
      }
  };

  template<typename algorithm_tag>
  struct algorithm_impl<algorithm_tag, false, false, true>
  {
      template<typename input_mesh_type, typename output_mesh_type, typename settings_type>
      static algorithm_feedback run( input_mesh_type const & input_mesh,
                        output_mesh_type & output_mesh,
                        settings_type const & settings )
      {
          typedef typename result_of::best_matching_native_input_mesh<algorithm_tag, input_mesh_type>::type native_input_mesh_type;
          native_input_mesh_type native_input_mesh;

          convert( input_mesh, native_input_mesh );
          return native_algorithm_impl<algorithm_tag>::run( native_input_mesh, output_mesh, settings );
      }

      template<typename input_mesh_type, typename input_segmentation_type, typename output_mesh_type, typename output_segmentation_type, typename settings_type>
      static algorithm_feedback run( input_mesh_type const & input_mesh, input_segmentation_type const & input_segmentation,
                        output_mesh_type & output_mesh, output_segmentation_type & output_segmentation,
                        settings_type const & settings )
      {
          typedef typename result_of::best_matching_native_input_mesh<algorithm_tag, input_mesh_type>::type native_input_mesh_type;
          typedef typename result_of::best_matching_native_input_segmentation<algorithm_tag, input_segmentation_type>::type native_input_segmentation_type;
          native_input_mesh_type native_input_mesh;
          native_input_segmentation_type native_input_segmentation;

          convert( input_mesh, input_segmentation, native_input_mesh, native_input_segmentation );
          return native_algorithm_impl<algorithm_tag>::run( native_input_mesh, input_segmentation, output_mesh, output_segmentation, settings );
      }
  };

  template<typename algorithm_tag>
  struct algorithm_impl<algorithm_tag, false, true, false>
  {
      template<typename input_mesh_type, typename output_mesh_type, typename settings_type>
      static algorithm_feedback run( input_mesh_type const & input_mesh,
                        output_mesh_type & output_mesh,
                        settings_type const & settings )
      {
          typedef typename result_of::best_matching_native_output_mesh<algorithm_tag, output_mesh_type>::type native_output_mesh_type;
          native_output_mesh_type native_output_mesh;

          algorithm_feedback result = native_algorithm_impl<algorithm_tag>::run( input_mesh, native_output_mesh, settings );
          convert( native_output_mesh, output_mesh );

          return result;
      }


      template<typename input_mesh_type, typename input_segmentation_type, typename output_mesh_type, typename output_segmentation_type, typename settings_type>
      static algorithm_feedback run( input_mesh_type const & input_mesh, input_segmentation_type const & input_segmentation,
                        output_mesh_type & output_mesh, output_segmentation_type & output_segmentation,
                        settings_type const & settings )
      {
          typedef typename result_of::best_matching_native_output_mesh<algorithm_tag, output_mesh_type>::type native_output_mesh_type;
          typedef typename result_of::best_matching_native_output_segmentation<algorithm_tag, output_segmentation_type>::type native_output_segmentation_type;
          native_output_mesh_type native_output_mesh;
          native_output_segmentation_type native_output_segmentation;

          algorithm_feedback result = native_algorithm_impl<algorithm_tag>::run( input_mesh, input_segmentation, native_output_mesh, native_output_segmentation, settings );
          convert( native_output_mesh, native_output_segmentation, output_mesh, output_segmentation );

          return result;
      }
  };

  template<typename algorithm_tag>
  struct algorithm_impl<algorithm_tag, false, false, false>
  {
      template<typename input_mesh_type, typename output_mesh_type, typename settings_type>
      static algorithm_feedback run( input_mesh_type const & input_mesh,
                        output_mesh_type & output_mesh,
                        settings_type const & settings )
      {
          typedef typename result_of::best_matching_native_input_mesh<algorithm_tag, input_mesh_type>::type native_input_mesh_type;
          typedef typename result_of::best_matching_native_output_mesh<algorithm_tag, output_mesh_type>::type native_output_mesh_type;
          native_input_mesh_type native_input_mesh;
          native_output_mesh_type native_output_mesh;

          convert( input_mesh, native_input_mesh );
          algorithm_feedback result = native_algorithm_impl<algorithm_tag>::run( native_input_mesh, native_output_mesh, settings );
          convert( native_output_mesh, output_mesh );

          return result;
      }

      template<typename input_mesh_type, typename input_segmentation_type, typename output_mesh_type, typename output_segmentation_type, typename settings_type>
      static algorithm_feedback run( input_mesh_type const & input_mesh, input_segmentation_type const & input_segmentation,
                        output_mesh_type & output_mesh, output_segmentation_type & output_segmentation,
                        settings_type const & settings )
      {
          typedef typename result_of::best_matching_native_input_mesh<algorithm_tag, input_mesh_type>::type native_input_mesh_type;
          typedef typename result_of::best_matching_native_input_segmentation<algorithm_tag, input_segmentation_type>::type native_input_segmentation_type;
          typedef typename result_of::best_matching_native_output_mesh<algorithm_tag, output_mesh_type>::type native_output_mesh_type;
          typedef typename result_of::best_matching_native_output_segmentation<algorithm_tag, output_segmentation_type>::type native_output_segmentation_type;
          native_input_mesh_type native_input_mesh;
          native_input_segmentation_type native_input_segmentation;
          native_output_mesh_type native_output_mesh;
          native_output_segmentation_type native_output_segmentation;

          convert( input_mesh, input_segmentation, native_input_mesh, native_input_segmentation );
          algorithm_feedback result = native_algorithm_impl<algorithm_tag>::run( native_input_mesh, native_input_segmentation, native_output_mesh, native_output_segmentation, settings );
          convert( native_output_mesh, native_output_segmentation, output_mesh, output_segmentation );

          return result;
      }
  };


  // id - input: mesh; od - output: mesh
  template<typename algorithm_tag, typename input_mesh_type, typename output_mesh_type, typename settings_type>
  algorithm_feedback run_algo_id_od( input_mesh_type const & input_mesh,
                  output_mesh_type & output_mesh,
                  settings_type const & settings )
  {
      return algorithm_impl<
                  algorithm_tag,
                  result_of::works_in_place<algorithm_tag>::value,
                  result_of::is_native_input_mesh<algorithm_tag, input_mesh_type>::value,
                  result_of::is_native_output_mesh<algorithm_tag, output_mesh_type>::value
          >::run(input_mesh, output_mesh, settings);
  }

  // id - input: mesh; ods - output: mesh and segmentation
  template<typename algorithm_tag, typename input_mesh_type, typename output_mesh_type, typename output_segmentation_type, typename settings_type>
  algorithm_feedback run_algo_id_ods( input_mesh_type const & input_mesh,
                  output_mesh_type & output_mesh, output_segmentation_type & output_segmentation,
                  settings_type const & settings )
  {
      return algorithm_impl<
                  algorithm_tag,
                  result_of::works_in_place<algorithm_tag>::value,
                  result_of::is_native_input_mesh<algorithm_tag, input_mesh_type>::value,
                  result_of::is_native_output<algorithm_tag, output_mesh_type, output_segmentation_type>::value
          >::run(input_mesh, output_mesh, output_segmentation, settings);
  }

  // ids - input: mesh and segmentation; od - output: mesh
  template<typename algorithm_tag, typename input_mesh_type, typename input_segmentation_type, typename output_mesh_type, typename settings_type>
  algorithm_feedback run_algo_ids_od( input_mesh_type const & input_mesh, input_segmentation_type const & input_segmentation,
                  output_mesh_type & output_mesh,
                  settings_type const & settings )
  {
      return algorithm_impl<
                  algorithm_tag,
                  result_of::works_in_place<algorithm_tag>::value,
                  result_of::is_native_input<algorithm_tag, input_mesh_type, input_segmentation_type>::value,
                  result_of::is_native_output_mesh<algorithm_tag, output_mesh_type>::value
          >::run(input_mesh, input_segmentation, output_mesh, settings);
  }

  // ids - input: mesh and segmentation; ods - output: mesh and segmentation
  template<typename algorithm_tag, typename input_mesh_type, typename input_segmentation_type, typename output_mesh_type, typename output_segmentation_type, typename settings_type>
  algorithm_feedback run_algo_ids_ods( input_mesh_type const & input_mesh, input_segmentation_type const & input_segmentation,
                  output_mesh_type & output_mesh, output_segmentation_type & output_segmentation,
                  settings_type const & settings )
  {
      return algorithm_impl<
                  algorithm_tag,
                  result_of::works_in_place<algorithm_tag>::value,
                  result_of::is_native_input<algorithm_tag, input_mesh_type, input_segmentation_type>::value,
                  result_of::is_native_output<algorithm_tag, output_mesh_type, output_segmentation_type>::value
          >::run(input_mesh, input_segmentation, output_mesh, output_segmentation, settings);
  }



  template<typename algorithm_tag, typename input_mesh_type, typename output_mesh_type, typename settings_type>
  algorithm_feedback run_algo( input_mesh_type const & input_mesh,
                  output_mesh_type & output_mesh,
                  settings_type const & settings )
  {
      viennautils::Timer t;
      t.start();
      algorithm_feedback feedback = run_algo_id_od<algorithm_tag>(input_mesh, output_mesh, settings);
      feedback.set_run_time( t.get() );

      return feedback;
  }

  template<typename algorithm_tag, typename input_mesh_type, typename input_segmentation_type, typename output_mesh_type, typename output_segmentation_type, typename settings_type>
  algorithm_feedback run_algo( input_mesh_type const & input_mesh, input_segmentation_type const & input_segmentation,
                  output_mesh_type & output_mesh, output_segmentation_type & output_segmentation,
                  settings_type const & settings )
  {
      viennautils::Timer t;
      t.start();
      algorithm_feedback feedback = run_algo_ids_ods<algorithm_tag>(input_mesh, input_segmentation, output_mesh, output_segmentation, settings);
      feedback.set_run_time( t.get() );

      return feedback;
  }

}

#endif
