#ifndef VIENNAMESH_TRIANGLE_BASE_TRIANGLE_DATASTRUCTURE_HPP
#define VIENNAMESH_TRIANGLE_BASE_TRIANGLE_DATASTRUCTURE_HPP

#define ANSI_DECLARATORS
#define VOID void
typedef double REAL;
#include "triangle/triangle.h"

#include "viennagrid/mesh/config.hpp"

namespace viennamesh
{
    
    namespace triangle
    {
        typedef triangulateio mesh_type;
        
        inline void init( mesh_type & mesh )
        {
            mesh.pointlist = NULL;
            mesh.pointattributelist = NULL;
            mesh.pointmarkerlist = NULL;
            mesh.numberofpoints = 0;
            mesh.numberofpointattributes = 0;
            
            mesh.trianglelist = NULL;
            mesh.triangleattributelist = NULL;
            mesh.trianglearealist = NULL;
            mesh.neighborlist = NULL;
            mesh.numberoftriangles = 0;
            mesh.numberofcorners = 0;
            mesh.numberoftriangleattributes = 0;
            
            mesh.segmentlist = NULL;
            mesh.segmentmarkerlist = NULL;
            mesh.numberofsegments = 0;
            
            mesh.holelist = NULL;
            mesh.numberofholes = 0;
            
            mesh.regionlist = NULL;
            mesh.numberofregions = 0;
            
            mesh.edgelist = NULL;
            mesh.edgemarkerlist = NULL;
            mesh.normlist = NULL;
            mesh.numberofedges = 0;
        }
        
        
        inline void deinit( mesh_type & mesh )
        {
            free( mesh.pointlist );
            free( mesh.pointattributelist );
            free( mesh.pointmarkerlist );
            
            free( mesh.trianglelist );
            free( mesh.triangleattributelist );
            free( mesh.trianglearealist );
            free( mesh.neighborlist );
            
            free( mesh.segmentlist );
            free( mesh.segmentmarkerlist );
            
            free( mesh.holelist );
            
            free( mesh.regionlist );
            
            free( mesh.edgelist );
            free( mesh.edgemarkerlist );
            free( mesh.normlist );
        }
    }
    
    
    struct triangle_plc_tag;
    
    
    namespace result_of
    {        
        template<>
        struct mesh< triangle_plc_tag >
        {
            typedef viennamesh::triangle::mesh_type type;
        };
    }
}

#endif