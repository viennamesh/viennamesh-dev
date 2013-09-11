#ifndef VIENNAMESH_DOMAIN_TETGEN_TETRAHEDRON_HPP
#define VIENNAMESH_DOMAIN_TETGEN_TETRAHEDRON_HPP

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/domain/segmentation.hpp"
#include "viennamesh/base/convert.hpp"


// #define TETLIBRARY
#include "tetgen/tetgen.h"



namespace viennamesh
{
  typedef tetgenio tetgen_tetrahedron_domain;
}    
    
namespace viennagrid
{
    namespace result_of
    {
        template<>
        struct point<viennamesh::tetgen_tetrahedron_domain>
        {
            typedef viennagrid::config::point_type_3d type;
        };
        
        template<>
        struct point<const viennamesh::tetgen_tetrahedron_domain>
        {
            typedef viennagrid::config::point_type_3d type;
        };
    }
}

    
namespace viennamesh
{
    
    
    template<typename triangular_3d_domain_type, typename triangular_3d_segmentation_type>
    struct convert_impl<triangular_3d_domain_type, triangular_3d_segmentation_type, tetgen_tetrahedron_domain, NoSegmentation >
    {
      typedef triangular_3d_domain_type vgrid_domain_type;
      typedef tetgen_tetrahedron_domain netgen_domain_type;
      
      typedef vgrid_domain_type input_domain_type;
      typedef triangular_3d_segmentation_type input_segmentation_type;
      typedef netgen_domain_type output_domain_type;
      typedef NoSegmentation output_segmentation_type;
      
      static bool convert( input_domain_type const & vgrid_domain, input_segmentation_type const & input_segmentation,
                            output_domain_type & tetgen_domain, output_segmentation_type & output_segmentation )
      {
        typedef typename viennagrid::result_of::point<vgrid_domain_type>::type point_type;
        
        typedef typename viennagrid::result_of::element<vgrid_domain_type, viennagrid::vertex_tag>::type vertex_type;
        typedef typename viennagrid::result_of::const_handle<vgrid_domain_type, viennagrid::vertex_tag>::type vertex_const_handle_type;
        typedef typename viennagrid::result_of::element<vgrid_domain_type, viennagrid::triangle_tag>::type triangle_type;
        
        typedef typename viennagrid::result_of::const_element_range<vgrid_domain_type, viennagrid::vertex_tag>::type vertex_range_type;
        typedef typename viennagrid::result_of::iterator<vertex_range_type>::type vertex_range_iterator;

        typedef typename viennagrid::result_of::const_element_range<vgrid_domain_type, viennagrid::triangle_tag>::type triangle_range_type;
        typedef typename viennagrid::result_of::iterator<triangle_range_type>::type triangle_range_iterator;

//         typedef typename triangular_3d_segmentation_type::segment_type SegmentType;
        
        
        std::map<vertex_const_handle_type, int> vertex_handle_to_tetgen_index_map;
        
        vertex_range_type vertices = viennagrid::elements(vgrid_domain);
        
        tetgen_domain.firstnumber = 0;
        tetgen_domain.numberofpoints = vertices.size();
        tetgen_domain.pointlist = new REAL[ tetgen_domain.numberofpoints * 3 ];
        
        int index = 0;
        for (vertex_range_iterator vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
        {
          
          tetgen_domain.pointlist[index*3+0] = viennagrid::point(vgrid_domain, *vit)[0];
          tetgen_domain.pointlist[index*3+1] = viennagrid::point(vgrid_domain, *vit)[1];
          tetgen_domain.pointlist[index*3+2] = viennagrid::point(vgrid_domain, *vit)[2];
          
          vertex_handle_to_tetgen_index_map[ vit.handle() ] = index;
        }
        
        
        triangle_range_type triangles = viennagrid::elements(vgrid_domain);
        
        tetgen_domain.numberoffacets = triangles.size();
        tetgen_domain.facetlist = new tetgenio::facet[tetgen_domain.numberoffacets];
        tetgen_domain.facetmarkerlist = new int[tetgen_domain.numberoffacets];

        index = 0;
        for (triangle_range_iterator tit = triangles.begin(); tit != triangles.end(); ++tit, ++index)
        {
          tetgenio::facet & facet = tetgen_domain.facetlist[index];
          
          facet.numberofpolygons = 1;
          facet.polygonlist = new tetgenio::polygon[1];
          facet.numberofholes = 0;
          facet.holelist = 0;
          
          tetgenio::polygon & polygon = facet.polygonlist[0];
          polygon.numberofvertices = 3;
          polygon.vertexlist = new int[3];
          polygon.vertexlist[0] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*tit).handle_at(0) ];
          polygon.vertexlist[1] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*tit).handle_at(1) ];
          polygon.vertexlist[2] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*tit).handle_at(2) ];
          
          tetgen_domain.facetmarkerlist[index] = 0; // TODO Was tut das?????
        }

        return true;
      }
    };
    
    
    
    
    
    template<typename OutputSegmentationT>
    struct convert_impl<tetgen_tetrahedron_domain, NoSegmentation, viennagrid::tetrahedral_3d_domain, OutputSegmentationT>//viennagrid::tetrahedral_3d_segmentation>
    {              
      typedef tetgen_tetrahedron_domain netgen_domain_type;
      typedef viennagrid::tetrahedral_3d_domain vgrid_domain_type;
      
      
      typedef netgen_domain_type input_domain_type;
      typedef NoSegmentation input_segmentation_type;
      typedef vgrid_domain_type output_domain_type;
      typedef OutputSegmentationT output_segmentation_type;
//         typedef viennagrid::tetrahedral_3d_segmentation output_segmentation_type;

      
      static bool convert( input_domain_type const & tetgen_domain, input_segmentation_type const & input_segmentation,
                            output_domain_type & vgrid_domain, output_segmentation_type & output_segmentation )
      {
        typedef viennagrid::result_of::point<vgrid_domain_type>::type point_type;
        
        typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::vertex_tag>::type vertex_type;
        typedef viennagrid::result_of::handle<vgrid_domain_type, viennagrid::vertex_tag>::type vertex_handle_type;
        
        typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::line_tag>::type line_type;
        typedef viennagrid::result_of::handle<vgrid_domain_type, viennagrid::line_tag>::type line_handle_type;
        
        typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::triangle_tag>::type triangle_type;
        typedef viennagrid::result_of::handle<vgrid_domain_type, viennagrid::triangle_tag>::type triangle_handle_type;
        
        typedef viennagrid::result_of::element<vgrid_domain_type, viennagrid::tetrahedron_tag>::type tetrahedron_type;
        typedef viennagrid::result_of::handle<vgrid_domain_type, viennagrid::tetrahedron_tag>::type tetrahedron_handle_type;

//         typedef typename output_segmentation_type::segment_type SegmentType;
        
        
        std::vector<vertex_handle_type> vertex_handles(tetgen_domain.numberofpoints);
        
        for (int i = 0; i < tetgen_domain.numberofpoints; ++i)
        {
          point_type point;
          
          point[0] = tetgen_domain.pointlist[3*i+0];
          point[1] = tetgen_domain.pointlist[3*i+1];
          point[2] = tetgen_domain.pointlist[3*i+2];
          
          vertex_handles[i] = viennagrid::make_vertex( vgrid_domain, point );
        }
        
        
        for (int i = 0; i < tetgen_domain.numberoftetrahedra; ++i)
        {
          viennagrid::make_tetrahedron(
            vgrid_domain,
            vertex_handles[ tetgen_domain.tetrahedronlist[4*i+0] ],
            vertex_handles[ tetgen_domain.tetrahedronlist[4*i+1] ],
            vertex_handles[ tetgen_domain.tetrahedronlist[4*i+2] ],
            vertex_handles[ tetgen_domain.tetrahedronlist[4*i+3] ]
          );
        }

        return true;
      }
    };
    
    

    
    
    
}

#endif