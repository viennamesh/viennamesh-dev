#ifndef VIENNAMESH_TRIANGLE_BASE_TRIANGLE_DATASTRUCTURE_HPP
#define VIENNAMESH_TRIANGLE_BASE_TRIANGLE_DATASTRUCTURE_HPP

#define ANSI_DECLARATORS
#define VOID void
typedef double REAL;
#include "triangle/triangle.h"

#include "viennagrid/domain/config.hpp"

namespace viennamesh
{
    
    namespace triangle
    {
        typedef triangulateio domain_type;
        
        inline void init( domain_type & domain )
        {
            domain.pointlist = NULL;
            domain.pointattributelist = NULL;
            domain.pointmarkerlist = NULL;
            domain.numberofpoints = 0;
            domain.numberofpointattributes = 0;
            
            domain.trianglelist = NULL;
            domain.triangleattributelist = NULL;
            domain.trianglearealist = NULL;
            domain.neighborlist = NULL;
            domain.numberoftriangles = 0;
            domain.numberofcorners = 0;
            domain.numberoftriangleattributes = 0;
            
            domain.segmentlist = NULL;
            domain.segmentmarkerlist = NULL;
            domain.numberofsegments = 0;
            
            domain.holelist = NULL;
            domain.numberofholes = 0;
            
            domain.regionlist = NULL;
            domain.numberofregions = 0;
            
            domain.edgelist = NULL;
            domain.edgemarkerlist = NULL;
            domain.normlist = NULL;
            domain.numberofedges = 0;
        }
        
        
        inline void deinit( domain_type & domain )
        {
            free( domain.pointlist );
            free( domain.pointattributelist );
            free( domain.pointmarkerlist );
            
            free( domain.trianglelist );
            free( domain.triangleattributelist );
            free( domain.trianglearealist );
            free( domain.neighborlist );
            
            free( domain.segmentlist );
            free( domain.segmentmarkerlist );
            
            free( domain.holelist );
            
            free( domain.regionlist );
            
            free( domain.edgelist );
            free( domain.edgemarkerlist );
            free( domain.normlist );
        }
    }
    
    
    struct triangle_plc_tag;
    
    
    namespace result_of
    {        
        template<>
        struct domain< triangle_plc_tag >
        {
            typedef viennamesh::triangle::domain_type type;
        };
    }
}

#endif