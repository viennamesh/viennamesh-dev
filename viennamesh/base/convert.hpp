#ifndef VIENNAMESH_BASE_CONVERT_HPP
#define VIENNAMESH_BASE_CONVERT_HPP

#include "viennagrid/domain/segmentation.hpp"

namespace viennamesh
{
    
//     struct dummy_segmentation {};
    
    template<typename input_domain_type, typename input_segmentation_type, typename output_domain_type, typename output_segmentation_type>
    struct convert_impl
    {
        // Prototype for convert
        // static bool convert( input_domain_type const & in, input_segmentation_type const & input_segmentation,
        //                      output_domain_type & out, output_segmentation_type & output_segmentation );
    };
    
    
    template<typename input_domain_type, typename input_segmentation_type, typename output_domain_type, typename output_segmentation_type>
    bool convert( input_domain_type const & in, input_segmentation_type const & input_segmentation, output_domain_type & out, output_segmentation_type & output_segmentation )
    {
        bool status = convert_impl<input_domain_type, input_segmentation_type, output_domain_type, output_segmentation_type>::
            convert(in, input_segmentation, out, output_segmentation);
        
        return status;
    }
    
    
    template<typename input_domain_type, typename output_domain_type>
    bool convert( input_domain_type const & in, output_domain_type & out )
    {
        viennagrid::dummy_segmentation<> dummy;
        return convert( in, dummy, out, dummy );
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