#ifndef VIENNAMESH_BASE_CONVERT_HPP
#define VIENNAMESH_BASE_CONVERT_HPP

#include "viennagrid/domain/segmentation.hpp"

namespace viennamesh
{
    
    struct NoSegmentation {};
    
    template<typename InputDomainT, typename InputSegmentationT, typename OutputDomainT, typename OutputSegmentationT>
    struct convert_impl
    {
        // Prototype for convert
        // static bool convert( input_domain_type const & in, input_segmentation_type const & input_segmentation,
        //                      output_domain_type & out, output_segmentation_type & output_segmentation );
    };
    
    
    template<typename InputDomainT, typename InputSegmentationT, typename OutputDomainT, typename OutputSegmentationT>
    bool convert( InputDomainT const & in, InputSegmentationT const & input_segmentation, OutputDomainT & out, OutputSegmentationT & output_segmentation )
    {
        bool status = convert_impl<InputDomainT, InputSegmentationT, OutputDomainT, OutputSegmentationT>::
            convert(in, input_segmentation, out, output_segmentation);
        
        return status;
    }
    
    
    template<typename InputDomainT, typename OutputDomainT>
    bool convert( InputDomainT const & in, OutputDomainT & out )
    {
      NoSegmentation source_segmentation;
      NoSegmentation destination_segmentation;
      
      bool status = convert_impl<InputDomainT, NoSegmentation, OutputDomainT, NoSegmentation>::convert(in, source_segmentation, out, destination_segmentation);

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