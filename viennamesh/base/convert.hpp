#ifndef VIENNAMESH_BASE_CONVERT_HPP
#define VIENNAMESH_BASE_CONVERT_HPP

#include "viennamesh/base/segments.hpp"

namespace viennamesh
{
    template<typename input_domain_type, typename output_domain_type>
    struct convert_impl
    {
        static bool convert( input_domain_type const & in, output_domain_type & out );
    };
    
    template<typename input_domain_type, typename output_domain_type>
    bool convert( input_domain_type const & in, output_domain_type & out )
    {
        bool status = convert_impl<input_domain_type, output_domain_type>::convert(in, out);;
        
        copy_segment_information(in, out);
        
        return status;
    }
    
    template<typename domain_type>
    bool convert( domain_type const & in, domain_type & out )
    {
        if (&in != &out)
            out = in;
        
        return true;
    }
}

#endif