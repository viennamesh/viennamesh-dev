#ifndef VIENNAGRID_IO_FLD_READER_GUARD
#define VIENNAGRID_IO_FLD_READER_GUARD

/* =======================================================================
   Copyright (c) 2011-2012, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   Authors:      Karl Rupp                           rupp@iue.tuwien.ac.at
                 Josef Weinbub                    weinbub@iue.tuwien.ac.at
               
   (A list of additional contributors can be found in the PDF manual)

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */


#include <fstream>
#include "viennamesh/io/viennasap_common.hpp"
#include "viennamesh/utils/string_tools.hpp"
#include "viennamesh/base/segments.hpp"

#include "viennagrid/domain/config.hpp"
#include "viennagrid/domain/element_creation.hpp"


/** @file netgen_reader.hpp
    @brief Provides a reader for Netgen files
*/


namespace viennagrid
{
  namespace io
  {

    /** @brief Reader for Netgen files obtained from the 'Export mesh...' menu item. Tested with Netgen version 4.9.12. */
    struct fld_reader
    {
      /** @brief The functor interface triggering the read operation.
       * 
       * @param domain    A ViennaGrid domain
       * @param filename  Name of the file
       */
      template <typename GeometricDomainType>
      int operator()(GeometricDomainType & domain, std::string const & filename)
      {        
        typedef typename viennagrid::result_of::point_type<GeometricDomainType>::type    PointType;
        typedef typename viennagrid::result_of::coord_type< PointType >::type         CoordType;

        enum { point_dim = viennagrid::traits::static_size<PointType>::value };
        
        typedef tetrahedron_tag CellTag;
        typedef typename result_of::element<GeometricDomainType, CellTag>::type CellType;
        typedef typename result_of::element_handle<GeometricDomainType, CellTag>::type                           CellHookType;

        typedef typename result_of::element_handle<GeometricDomainType, vertex_tag>::type                           VertexHookType;
        
        
        std::ifstream reader(filename.c_str());
        
        
        #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
        std::cout << "* netgen_reader::operator(): Reading file " << filename << std::endl;
        #endif

        if (!reader)
        {
          throw cannot_open_file_exception(filename);
          return EXIT_FAILURE;
        }
        
        std::string file_content = stringtools::read_stream(reader);
        
        boost::shared_ptr<viennasap_common::token_t> root = viennasap_common::split_into_tokens( file_content, 0, file_content.size() );

        std::map<int, VertexHookType> vertex_map;
        
        double scaling = 1.0;
        {
            boost::shared_ptr<viennasap_common::token_t> scaling_factor = root->get_child("node/scalingfactor");
            std::stringstream current_node_string(scaling_factor->content);
            current_node_string >> scaling;
        }
        
        boost::shared_ptr<viennasap_common::token_t> nodes = root->get_child("node");
        for (viennasap_common::token_t::unnamed_children_container::iterator it = nodes->unnamed_children.begin(); it != nodes->unnamed_children.end(); ++it)
        {
            std::stringstream current_node_string((*it)->content);
            
            int index;
            PointType point;
            
            current_node_string >> index;
            for (int i = 0; i < point_dim; ++i)
                current_node_string >> point[i];
            
            point *= scaling;

            vertex_map[index] = viennagrid::create_vertex(domain, point);
        }
        
        
        boost::shared_ptr<viennasap_common::token_t> elements = root->get_child("element");
        for (viennasap_common::token_t::unnamed_children_container::iterator it = elements->unnamed_children.begin(); it != elements->unnamed_children.end(); ++it)
        {
            std::stringstream current_element_string((*it)->content);
            
            int index;
            std::string type;
            int segment;
            viennagrid::storage::static_array<int, 10> node_ids;
            
            current_element_string >> index >> type >> segment;
            for (viennagrid::storage::static_array<int, 10>::iterator jt = node_ids.begin(); jt != node_ids.end(); ++jt)
                current_element_string >> *jt;
            
//             PointType const & p0 = viennagrid::point( domain, vertex_map[node_ids[0]] );
//             PointType const & p1 = viennagrid::point( domain, vertex_map[node_ids[1]] );
//             PointType const & p2 = viennagrid::point( domain, vertex_map[node_ids[2]] );
//             PointType const & p3 = viennagrid::point( domain, vertex_map[node_ids[3]] );
//             
//             double det = viennamesh::utils::determinant(p1-p0, p2-p0, p3-p0);
//             std::cout << "det=" << det << std::endl;
            
            CellHookType tet_handle = viennagrid::create_tetrahedron( domain, vertex_map[node_ids[0]], vertex_map[node_ids[1]], vertex_map[node_ids[2]], vertex_map[node_ids[3]] );
            viennamesh::segment( viennagrid::dereference_handle( domain, tet_handle ) ) = segment;
        }
        
        
        nodeattribute_name = root->get_child("nodeattribute/name")->content;
        
        boost::shared_ptr<viennasap_common::token_t> nodeattributes = root->get_child("nodeattribute");
        for (viennasap_common::token_t::unnamed_children_container::iterator it = nodeattributes->unnamed_children.begin(); it != nodeattributes->unnamed_children.end(); ++it)
        {
            std::stringstream current_element_string((*it)->content);
            
            int node_index;
            double value;
            
            current_element_string >> node_index >> value;
            
            viennadata::access<std::string, double>(nodeattribute_name)(viennagrid::dereference_handle(domain, vertex_map[node_index])) = value;
        }
        

        
        return EXIT_SUCCESS;
      } //operator()
      
      
      std::string nodeattribute_name;
      
    }; //class netgen_reader
  
  } //namespace io
} //namespace viennagrid

#endif
