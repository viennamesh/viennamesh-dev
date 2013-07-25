#ifndef VIENNAMESH_BASE_ALGORITHM_HPP
#define VIENNAMESH_BASE_ALGORITHM_HPP

#include "viennamesh/base/convert.hpp"
#include "viennamesh/utils/timer.hpp"


namespace viennamesh
{

    namespace result_of
    {       
        template<typename algorithm_tag, typename domain_type>
        struct best_matching_native_input_domain;
        
        template<typename algorithm_tag, typename segmentation_type>
        struct best_matching_native_input_segmentation;

        template<typename algorithm_tag, typename domain_type>
        struct best_matching_native_output_domain;
        
        template<typename algorithm_tag, typename segmentation_type>
        struct best_matching_native_output_segmentation;


        template<typename algorithm_tag, typename domain_type>
        struct is_native_input_domain
        {
            static const bool value = viennagrid::meta::EQUAL<
                    domain_type,
                    typename best_matching_native_input_domain<algorithm_tag, domain_type>::type
                >::value;
        };
        
        template<typename algorithm_tag, typename segmentation_type>
        struct is_native_input_segmentation
        {
            static const bool value = viennagrid::meta::EQUAL<
                    segmentation_type,
                    typename best_matching_native_input_segmentation<algorithm_tag, segmentation_type>::type
                >::value;
        };
        
        template<typename algorithm_tag, typename domain_type, typename segmentation_type>
        struct is_native_input
        {
            static const bool value = is_native_input_domain<algorithm_tag, domain_type>::value &&
                                      is_native_input_segmentation<algorithm_tag, segmentation_type>::value;
        };
        
        
        
        template<typename algorithm_tag, typename domain_type>
        struct is_native_output_domain
        {
            static const bool value = viennagrid::meta::EQUAL<
                    domain_type,
                    typename best_matching_native_output_domain<algorithm_tag, domain_type>::type
                >::value;
        };
        
        template<typename algorithm_tag, typename segmentation_type>
        struct is_native_output_segmentation
        {
            static const bool value = viennagrid::meta::EQUAL<
                    segmentation_type,
                    typename best_matching_native_output_segmentation<algorithm_tag, segmentation_type>::type
                >::value;
        };
        
        template<typename algorithm_tag, typename domain_type, typename segmentation_type>
        struct is_native_output
        {
            static const bool value = is_native_output_domain<algorithm_tag, domain_type>::value &&
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
    
    
    
    template<typename algorithm_tag, typename native_domain_type, typename settings_type>
    algorithm_feedback run_algo_native( native_domain_type & native_domain, settings_type const & settings )
    {
        return native_algorithm_impl<algorithm_tag>::run(native_domain, settings);
    }
    
    template<typename algorithm_tag, typename native_domain_type, typename segmentation_type, typename settings_type>
    algorithm_feedback run_algo_native( native_domain_type & native_domain, segmentation_type & segmentation, settings_type const & settings )
    {
        return native_algorithm_impl<algorithm_tag>::run(native_domain, segmentation, settings);
    }
    
    template<typename algorithm_tag, typename native_input_domain_type, typename native_ouput_domain_type, typename settings_type>
    algorithm_feedback run_algo_native( native_input_domain_type const & native_input_domain, native_ouput_domain_type & native_output_domain, settings_type const & settings )
    {
        return native_algorithm_impl<algorithm_tag>::run(native_input_domain, native_output_domain, settings);
    }
    
    template<typename algorithm_tag, typename native_input_domain_type, typename input_segmentation_type, typename native_ouput_domain_type, typename output_segmentation_type, typename settings_type>
    algorithm_feedback run_algo_native( native_input_domain_type const & native_input_domain, input_segmentation_type const & input_segmentation,
                          native_ouput_domain_type & native_output_domain, output_segmentation_type & output_segmentation,
                          settings_type const & settings )
    {
        return native_algorithm_impl<algorithm_tag>::run(native_input_domain, input_segmentation, native_output_domain, output_segmentation, settings);
    }
    
    
    
    
    
    template<typename algorithm_tag, bool algo_works_in_place, bool is_input_native, bool is_output_native>
    struct algorithm_impl;
    
    
    template<typename algorithm_tag>
    struct algorithm_impl<algorithm_tag, true, true, true>
    {
        template<typename input_domain_type, typename output_domain_type, typename settings_type>
        static algorithm_feedback run( input_domain_type const & input_domain,
                         output_domain_type & output_domain,
                         settings_type const & settings )
        {
            if (&input_domain != &output_domain)
                convert(input_domain, output_domain);
                
            return native_algorithm_impl<algorithm_tag>::run( output_domain, settings );
        }
        
        template<typename input_domain_type, typename input_segmentation_type, typename output_domain_type, typename output_segmentation_type, typename settings_type>
        static algorithm_feedback run( input_domain_type const & input_domain, input_segmentation_type const & input_segmentation,
                         output_domain_type & output_domain, output_segmentation_type & output_segmentation,
                         settings_type const & settings )
        {
            if (&input_domain != &output_domain)
                convert(input_domain, input_segmentation, output_domain, output_segmentation);
                
            return native_algorithm_impl<algorithm_tag>::run( output_domain, output_segmentation, settings );
        }
    };
    
    template<typename algorithm_tag>
    struct algorithm_impl<algorithm_tag, true, false, true>
    {
        template<typename input_domain_type, typename output_domain_type, typename settings_type>
        static algorithm_feedback run( input_domain_type const & input_domain,
                         output_domain_type & output_domain,
                         settings_type const & settings )
        {
            convert( input_domain, output_domain );
            
            return native_algorithm_impl<algorithm_tag>::run( output_domain, settings );
        }
        
        template<typename input_domain_type, typename input_segmentation_type, typename output_domain_type, typename output_segmentation_type, typename settings_type>
        static algorithm_feedback run( input_domain_type const & input_domain, input_segmentation_type const & input_segmentation,
                         output_domain_type & output_domain, output_segmentation_type & output_segmentation,
                         settings_type const & settings )
        {
            convert( input_domain, input_segmentation, output_domain, output_segmentation );
            
            return native_algorithm_impl<algorithm_tag>::run( output_domain, output_segmentation, settings );
        }
    };
    
    template<typename algorithm_tag>
    struct algorithm_impl<algorithm_tag, true, true, false>
    {
        template<typename input_domain_type, typename output_domain_type, typename settings_type>
        static algorithm_feedback run( input_domain_type const & input_domain,
                         output_domain_type & output_domain,
                         settings_type const & settings )
        {
            input_domain_type tmp_domain(input_domain);
                
            algorithm_feedback result = native_algorithm_impl<algorithm_tag>::run( tmp_domain, settings );
            
            convert( tmp_domain, output_domain );
            return result;
        }
        
        template<typename input_domain_type, typename input_segmentation_type, typename output_domain_type, typename output_segmentation_type, typename settings_type>
        static algorithm_feedback run( input_domain_type const & input_domain, input_segmentation_type const & input_segmentation,
                         output_domain_type & output_domain, output_segmentation_type & output_segmentation,
                         settings_type const & settings )        
        {
//             typedef typename result_of::best_matching_native_output_domain<algorithm_tag, output_domain_type>::type native_output_domain_type;
            input_domain_type tmp_domain(input_domain);
            input_segmentation_type tmp_segmentation(input_segmentation);
                
            algorithm_feedback result = native_algorithm_impl<algorithm_tag>::run( tmp_domain, tmp_segmentation, settings );
            
            convert( tmp_domain, tmp_segmentation, output_domain, output_segmentation );
            return result;
        }
    };
    
    template<typename algorithm_tag>
    struct algorithm_impl<algorithm_tag, true, false, false>
    {
        template<typename input_domain_type, typename output_domain_type, typename settings_type>
        static algorithm_feedback run( input_domain_type const & input_domain,
                         output_domain_type & output_domain,
                         settings_type const & settings )
        {
            typedef typename result_of::best_matching_native_output_domain<algorithm_tag, output_domain_type>::type native_output_domain_type;
            native_output_domain_type tmp_domain;
            
            convert( input_domain, tmp_domain );
            
            algorithm_feedback result = native_algorithm_impl<algorithm_tag>::run( tmp_domain, settings );
            
            convert( tmp_domain, output_domain );
            return result;
        }
        
        template<typename input_domain_type, typename input_segmentation_type, typename output_domain_type, typename output_segmentation_type, typename settings_type>
        static algorithm_feedback run( input_domain_type const & input_domain, input_segmentation_type const & input_segmentation,
                         output_domain_type & output_domain, output_segmentation_type & output_segmentation,
                         settings_type const & settings )     
        {
            typedef typename result_of::best_matching_native_output_domain<algorithm_tag, output_domain_type>::type native_output_domain_type;
            typedef typename result_of::best_matching_native_output_segmentation<algorithm_tag, output_segmentation_type>::type native_output_segmentation_type;
            native_output_domain_type tmp_domain;
            native_output_segmentation_type tmp_segmentation;
            
            convert( input_domain, input_segmentation, tmp_domain, tmp_segmentation );
            
            algorithm_feedback result = native_algorithm_impl<algorithm_tag>::run( tmp_domain, tmp_segmentation, settings );
            
            convert( tmp_domain, tmp_segmentation, output_domain, output_segmentation );
            return result;
        }
    };
        
    template<typename algorithm_tag>
    struct algorithm_impl<algorithm_tag, false, true, true>
    {
        template<typename input_domain_type, typename output_domain_type, typename settings_type>
        static algorithm_feedback run( input_domain_type const & input_domain,
                         output_domain_type & output_domain,
                         settings_type const & settings )        {
            return native_algorithm_impl<algorithm_tag>::run( input_domain, output_domain, settings );
        }
        
        template<typename input_domain_type, typename input_segmentation_type, typename output_domain_type, typename output_segmentation_type, typename settings_type>
        static algorithm_feedback run( input_domain_type const & input_domain, input_segmentation_type const & input_segmentation,
                         output_domain_type & output_domain, output_segmentation_type & output_segmentation,
                         settings_type const & settings )  
        {
            return native_algorithm_impl<algorithm_tag>::run( input_domain, input_segmentation, output_domain, output_segmentation, settings );
        }
    };
    
    template<typename algorithm_tag>
    struct algorithm_impl<algorithm_tag, false, false, true>
    {
        template<typename input_domain_type, typename output_domain_type, typename settings_type>
        static algorithm_feedback run( input_domain_type const & input_domain,
                         output_domain_type & output_domain,
                         settings_type const & settings )
        {
            typedef typename result_of::best_matching_native_input_domain<algorithm_tag, input_domain_type>::type native_input_domain_type;
            native_input_domain_type native_input_domain;
            
            convert( input_domain, native_input_domain );
            return native_algorithm_impl<algorithm_tag>::run( native_input_domain, output_domain, settings );
        }
        
        template<typename input_domain_type, typename input_segmentation_type, typename output_domain_type, typename output_segmentation_type, typename settings_type>
        static algorithm_feedback run( input_domain_type const & input_domain, input_segmentation_type const & input_segmentation,
                         output_domain_type & output_domain, output_segmentation_type & output_segmentation,
                         settings_type const & settings )
        {
            typedef typename result_of::best_matching_native_input_domain<algorithm_tag, input_domain_type>::type native_input_domain_type;
            typedef typename result_of::best_matching_native_input_segmentation<algorithm_tag, input_segmentation_type>::type native_input_segmentation_type;
            native_input_domain_type native_input_domain;
            native_input_segmentation_type native_input_segmentation;
            
            convert( input_domain, input_segmentation, native_input_domain, native_input_segmentation );
            return native_algorithm_impl<algorithm_tag>::run( native_input_domain, input_segmentation, output_domain, output_segmentation, settings );
        }
    };
    
    template<typename algorithm_tag>
    struct algorithm_impl<algorithm_tag, false, true, false>
    {
        template<typename input_domain_type, typename output_domain_type, typename settings_type>
        static algorithm_feedback run( input_domain_type const & input_domain,
                         output_domain_type & output_domain,
                         settings_type const & settings )
        {
            typedef typename result_of::best_matching_native_output_domain<algorithm_tag, output_domain_type>::type native_output_domain_type;
            native_output_domain_type native_output_domain;
                
            algorithm_feedback result = native_algorithm_impl<algorithm_tag>::run( input_domain, native_output_domain, settings );
            convert( native_output_domain, output_domain );
            
            return result;
        }
        
        
        template<typename input_domain_type, typename input_segmentation_type, typename output_domain_type, typename output_segmentation_type, typename settings_type>
        static algorithm_feedback run( input_domain_type const & input_domain, input_segmentation_type const & input_segmentation,
                         output_domain_type & output_domain, output_segmentation_type & output_segmentation,
                         settings_type const & settings )
        {
            typedef typename result_of::best_matching_native_output_domain<algorithm_tag, output_domain_type>::type native_output_domain_type;
            typedef typename result_of::best_matching_native_output_segmentation<algorithm_tag, output_segmentation_type>::type native_output_segmentation_type;
            native_output_domain_type native_output_domain;
            native_output_segmentation_type native_output_segmentation;
                
            algorithm_feedback result = native_algorithm_impl<algorithm_tag>::run( input_domain, input_segmentation, native_output_domain, native_output_segmentation, settings );
            convert( native_output_domain, native_output_segmentation, output_domain, output_segmentation );
            
            return result;
        }
    };
    
    template<typename algorithm_tag>
    struct algorithm_impl<algorithm_tag, false, false, false>
    {
        template<typename input_domain_type, typename output_domain_type, typename settings_type>
        static algorithm_feedback run( input_domain_type const & input_domain,
                         output_domain_type & output_domain,
                         settings_type const & settings )
        {
            typedef typename result_of::best_matching_native_input_domain<algorithm_tag, input_domain_type>::type native_input_domain_type;
            typedef typename result_of::best_matching_native_output_domain<algorithm_tag, output_domain_type>::type native_output_domain_type;
            native_input_domain_type native_input_domain;
            native_output_domain_type native_output_domain;
            
            convert( input_domain, native_input_domain );
            algorithm_feedback result = native_algorithm_impl<algorithm_tag>::run( native_input_domain, native_output_domain, settings );
            convert( native_output_domain, output_domain );
            
            return result;
        }
        
        template<typename input_domain_type, typename input_segmentation_type, typename output_domain_type, typename output_segmentation_type, typename settings_type>
        static algorithm_feedback run( input_domain_type const & input_domain, input_segmentation_type const & input_segmentation,
                         output_domain_type & output_domain, output_segmentation_type & output_segmentation,
                         settings_type const & settings )
        {
            typedef typename result_of::best_matching_native_input_domain<algorithm_tag, input_domain_type>::type native_input_domain_type;
            typedef typename result_of::best_matching_native_input_segmentation<algorithm_tag, input_segmentation_type>::type native_input_segmentation_type;
            typedef typename result_of::best_matching_native_output_domain<algorithm_tag, output_domain_type>::type native_output_domain_type;
            typedef typename result_of::best_matching_native_output_segmentation<algorithm_tag, output_segmentation_type>::type native_output_segmentation_type;
            native_input_domain_type native_input_domain;
            native_input_segmentation_type native_input_segmentation;
            native_output_domain_type native_output_domain;
            native_output_segmentation_type native_output_segmentation;
            
            convert( input_domain, input_segmentation, native_input_domain, native_input_segmentation );
            algorithm_feedback result = native_algorithm_impl<algorithm_tag>::run( native_input_domain, native_input_segmentation, native_output_domain, native_output_segmentation, settings );
            convert( native_output_domain, native_output_segmentation, output_domain, output_segmentation );
            
            return result;
        }
    };
    
    
    // id - input: domain; od - output: domain
    template<typename algorithm_tag, typename input_domain_type, typename output_domain_type, typename settings_type>
    algorithm_feedback run_algo_id_od( input_domain_type const & input_domain,
                   output_domain_type & output_domain,
                   settings_type const & settings )
    {
        return algorithm_impl<
                    algorithm_tag,
                    result_of::works_in_place<algorithm_tag>::value,
                    result_of::is_native_input_domain<algorithm_tag, input_domain_type>::value,
                    result_of::is_native_output_domain<algorithm_tag, output_domain_type>::value        
            >::run(input_domain, output_domain, settings);
    }
    
    // id - input: domain; ods - output: domain and segmentation
    template<typename algorithm_tag, typename input_domain_type, typename output_domain_type, typename output_segmentation_type, typename settings_type>
    algorithm_feedback run_algo_id_ods( input_domain_type const & input_domain, 
                   output_domain_type & output_domain, output_segmentation_type & output_segmentation,
                   settings_type const & settings )
    {
        return algorithm_impl<
                    algorithm_tag,
                    result_of::works_in_place<algorithm_tag>::value,
                    result_of::is_native_input_domain<algorithm_tag, input_domain_type>::value,
                    result_of::is_native_output<algorithm_tag, output_domain_type, output_segmentation_type>::value        
            >::run(input_domain, output_domain, output_segmentation, settings);
    }
    
    // ids - input: domain and segmentation; od - output: domain
    template<typename algorithm_tag, typename input_domain_type, typename input_segmentation_type, typename output_domain_type, typename settings_type>
    algorithm_feedback run_algo_ids_od( input_domain_type const & input_domain, input_segmentation_type const & input_segmentation,
                   output_domain_type & output_domain,
                   settings_type const & settings )
    {
        return algorithm_impl<
                    algorithm_tag,
                    result_of::works_in_place<algorithm_tag>::value,
                    result_of::is_native_input<algorithm_tag, input_domain_type, input_segmentation_type>::value,
                    result_of::is_native_output_domain<algorithm_tag, output_domain_type>::value        
            >::run(input_domain, input_segmentation, output_domain, settings);
    }
    
    // ids - input: domain and segmentation; ods - output: domain and segmentation
    template<typename algorithm_tag, typename input_domain_type, typename input_segmentation_type, typename output_domain_type, typename output_segmentation_type, typename settings_type>
    algorithm_feedback run_algo_ids_ods( input_domain_type const & input_domain, input_segmentation_type const & input_segmentation,
                   output_domain_type & output_domain, output_segmentation_type & output_segmentation,
                   settings_type const & settings )
    {
        return algorithm_impl<
                    algorithm_tag,
                    result_of::works_in_place<algorithm_tag>::value,
                    result_of::is_native_input<algorithm_tag, input_domain_type, input_segmentation_type>::value,
                    result_of::is_native_output<algorithm_tag, output_domain_type, output_segmentation_type>::value        
            >::run(input_domain, input_segmentation, output_domain, output_segmentation, settings);
    }
    
    
    
    template<typename algorithm_tag, typename input_domain_type, typename output_domain_type, typename settings_type>
    algorithm_feedback run_algo( input_domain_type const & input_domain,
                   output_domain_type & output_domain,
                   settings_type const & settings )
    {
        viennautils::Timer t;
        t.start();
        algorithm_feedback feedback = run_algo_id_od<algorithm_tag>(input_domain, output_domain, settings);        
        feedback.set_run_time( t.get() );
        
        return feedback;
    }
    
    template<typename algorithm_tag, typename input_domain_type, typename input_segmentation_type, typename output_domain_type, typename output_segmentation_type, typename settings_type>
    algorithm_feedback run_algo( input_domain_type const & input_domain, input_segmentation_type const & input_segmentation,
                   output_domain_type & output_domain, output_segmentation_type & output_segmentation,
                   settings_type const & settings )
    {
        viennautils::Timer t;
        t.start();
        algorithm_feedback feedback = run_algo_ids_ods<algorithm_tag>(input_domain, input_segmentation, output_domain, output_segmentation, settings);
        feedback.set_run_time( t.get() );
        
        return feedback;
    }

}

#endif