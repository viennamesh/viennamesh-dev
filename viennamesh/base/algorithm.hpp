#ifndef VIENNAMESH_BASE_ALGORITHM_HPP
#define VIENNAMESH_BASE_ALGORITHM_HPP

#include "viennamesh/base/convert.hpp"

namespace viennamesh
{

    namespace result_of
    {       
        template<typename algorithm_tag, typename domain_type>
        struct best_matching_native_input;

        template<typename algorithm_tag, typename domain_type>
        struct best_matching_native_output;


        
        template<typename algorithm_tag, typename domain_type>
        struct is_native_input
        {
            static const bool value = viennameta::_equal<
                    domain_type,
                    typename best_matching_native_input<algorithm_tag, domain_type>::type
                >::value;
        };
        
        template<typename algorithm_tag, typename domain_type>
        struct is_native_output
        {
            static const bool value = viennameta::_equal<
                    domain_type,
                    typename best_matching_native_output<algorithm_tag, domain_type>::type
                >::value;
        };

        
        
        
        template<typename algorithm_tag>
        struct works_in_place;
        
        template<typename algorithm_tag>
        struct settings;
    }
    

    template<typename settings_type, typename parameter_tag>
    struct settings_parameter {};
    
    template<typename parameter_tag, typename settings_type>
    typename settings_parameter<settings_type, parameter_tag>::type get_settings_parameter( settings_type const & settings )
    {
        return settings_parameter<settings_type, parameter_tag>::get( settings );
    }
    
    
    template<typename algorithm_tag>
    struct native_algorithm_impl;
    
    
    
    template<typename algorithm_tag, typename native_domain_type, typename settings_type>
    bool run_algo_native( native_domain_type & native_domain, settings_type const & settings )
    {
        return native_algorithm_impl<algorithm_tag>::run(native_domain, settings);
    }
    
    template<typename algorithm_tag, typename native_input_domain_type, typename native_ouput_domain_type, typename settings_type>
    bool run_algo_native( native_input_domain_type const & native_input_domain, native_ouput_domain_type & native_output_domain, settings_type const & settings )
    {
        return native_algorithm_impl<algorithm_tag>::run(native_input_domain, native_output_domain, settings);
    }
    
    
    
    
    
    
    template<typename algorithm_tag, bool algo_works_in_place, bool is_input_native, bool is_output_native>
    struct algorithm_impl;
    
    
    template<typename algorithm_tag>
    struct algorithm_impl<algorithm_tag, true, true, true>
    {
        template<typename input_domain_type, typename output_domain_type, typename settings_type>
        static bool run( input_domain_type const & input_domain, output_domain_type & output_domain, settings_type const & settings )
        {
            if (&input_domain != &output_domain)
                convert(input_domain, output_domain);
                
            return native_algorithm_impl<algorithm_tag>::run( output_domain, settings );
        }
    };
    
    template<typename algorithm_tag>
    struct algorithm_impl<algorithm_tag, true, false, true>
    {
        template<typename input_domain_type, typename output_domain_type, typename settings_type>
        static bool run( input_domain_type const & input_domain, output_domain_type & output_domain, settings_type const & settings )
        {
            convert( input_domain, output_domain );
            
            return native_algorithm_impl<algorithm_tag>::run( output_domain, settings );
        }
    };
    
    template<typename algorithm_tag>
    struct algorithm_impl<algorithm_tag, true, true, false>
    {
        template<typename input_domain_type, typename output_domain_type, typename settings_type>
        static bool run( input_domain_type const & input_domain, output_domain_type & output_domain, settings_type const & settings )
        {
            typedef typename result_of::best_matching_native_output<algorithm_tag, output_domain_type>::type native_output_domain_type;
            input_domain_type tmp_domain(input_domain);
                
            bool result = native_algorithm_impl<algorithm_tag>::run( tmp_domain, settings );
            
            convert( tmp_domain, output_domain );
            return result;
        }
    };
    
    template<typename algorithm_tag>
    struct algorithm_impl<algorithm_tag, true, false, false>
    {
        template<typename input_domain_type, typename output_domain_type, typename settings_type>
        static bool run( input_domain_type const & input_domain, output_domain_type & output_domain, settings_type const & settings )
        {
            typedef typename result_of::best_matching_native_output<algorithm_tag, output_domain_type>::type native_output_domain_type;
            native_output_domain_type tmp_domain;
            
            convert( input_domain, tmp_domain );
            
            bool result = native_algorithm_impl<algorithm_tag>::run( tmp_domain, settings );
            
            convert( tmp_domain, output_domain );
            return result;
        }
    };
        
    template<typename algorithm_tag>
    struct algorithm_impl<algorithm_tag, false, true, true>
    {
        template<typename input_domain_type, typename output_domain_type, typename settings_type>
        static bool run( input_domain_type const & input_domain, output_domain_type & output_domain, settings_type const & settings )
        {
            return native_algorithm_impl<algorithm_tag>::run( input_domain, output_domain, settings );
        }
    };
    
    template<typename algorithm_tag>
    struct algorithm_impl<algorithm_tag, false, false, true>
    {
        template<typename input_domain_type, typename output_domain_type, typename settings_type>
        static bool run( input_domain_type const & input_domain, output_domain_type & output_domain, settings_type const & settings )
        {
            typedef typename result_of::best_matching_native_input<algorithm_tag, input_domain_type>::type native_input_domain_type;
            native_input_domain_type native_input_domain;
            
            convert( input_domain, native_input_domain );
            return native_algorithm_impl<algorithm_tag>::run( native_input_domain, output_domain, settings );
        }
    };
    
    template<typename algorithm_tag>
    struct algorithm_impl<algorithm_tag, false, true, false>
    {
        template<typename input_domain_type, typename output_domain_type, typename settings_type>
        static bool run( input_domain_type const & input_domain, output_domain_type & output_domain, settings_type const & settings )
        {
            typedef typename result_of::best_matching_native_output<algorithm_tag, output_domain_type>::type native_output_domain_type;
            native_output_domain_type native_output_domain;
                
            bool result = native_algorithm_impl<algorithm_tag>::run( input_domain, native_output_domain, settings );
            convert( native_output_domain, output_domain );
            
            return result;
        }
    };
    
    template<typename algorithm_tag>
    struct algorithm_impl<algorithm_tag, false, false, false>
    {
        template<typename input_domain_type, typename output_domain_type, typename settings_type>
        static bool run( input_domain_type const & input_domain, output_domain_type & output_domain, settings_type const & settings )
        {
            typedef typename result_of::best_matching_native_input<algorithm_tag, input_domain_type>::type native_input_domain_type;
            typedef typename result_of::best_matching_native_output<algorithm_tag, output_domain_type>::type native_output_domain_type;
            native_input_domain_type native_input_domain;
            native_output_domain_type native_output_domain;
            
            convert( input_domain, native_input_domain );
            bool result = native_algorithm_impl<algorithm_tag>::run( native_input_domain, native_output_domain, settings );
            convert( native_output_domain, output_domain );
            
            return result;
        }
    };
    
    
    template<typename algorithm_tag, typename input_domain_type, typename output_domain_type, typename settings_type>
    bool run_algo( input_domain_type const & input_domain, output_domain_type & output_domain, settings_type const & settings )
    {
        return algorithm_impl<
                    algorithm_tag,
                    result_of::works_in_place<algorithm_tag>::value,
                    result_of::is_native_input<algorithm_tag,input_domain_type>::value,
                    result_of::is_native_output<algorithm_tag,output_domain_type>::value        
            >::run(input_domain, output_domain, settings);
    }
}

#endif