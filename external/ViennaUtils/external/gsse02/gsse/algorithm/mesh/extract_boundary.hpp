/* ============================================================================
   Copyright (c) 2007-2010 Franz Stimpfl                          franz@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_TOPOLOGY_EXTRACT_BOUNDARY_HH)
#define GSSE_TOPOLOGY_EXTRACT_BOUNDARY_HH

// *** system includes
// *** BOOST includes
// *** GSSE includes
#include "gsse/datastructure/utils.hpp"
#include "gsse/topology/coboundary.hpp"
#include "gsse/topology/boundary.hpp"
#include "gsse/util/at_complex.hpp"
#include "gsse/algorithm/mesh/keys.hpp"
#include "gsse/util/logger.hpp"

// ############################################################
//
using namespace gsse::access_specifier; 

namespace gsse {

   template <typename FullSpace>
   void initialize_topology(FullSpace& full_space, std::ostream& stream = std::cout)
   {
      typedef typename gsse::result_of::at_dim<FullSpace, AC>::type                 SpaceTopologySegmentsT;
      typedef typename gsse::result_of::at_dim_index<SpaceTopologySegmentsT>::type  CellComplex;
      typedef typename gsse::result_of::at_dim_isd<CellComplex, AT_cl>::type        CellT;      
      typedef typename gsse::result_of::at_dim_isd<CellComplex, AT_vx>::type        VertexT;     

      SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(full_space);

      //dump_type<VertexT>();

      size_t   segment_size = segments_topology.size();
      // --------------------------------------------
      for (size_t si = 0; si < segment_size; ++si)
      {   
         //
         // vertex_con_t --> to impose uniqueness on the segment vertices
         //
         typedef std::map<size_t, bool>      vertex_con_t;
         //
         // vertex_map_t --> to bridge between the dense vertex container elements
         //                  and the sparse real vertex indices
         //
         typedef std::map<size_t, size_t>    vertex_map_t;
         vertex_con_t      vertex_con;   
         vertex_map_t      vertex_map;      

         size_t cellsize = gsse::at_dim<AT_cl>(gsse::at_fiber(segments_topology[ si ])).size();

         //
         // SETUP UNIQUE VERTEX CONTAINER
         //
         for (size_t ci = 0 ; ci < cellsize; ++ci)
         {
            CellT cell = gsse::at(ci) (gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology)))	);
      
            for (size_t voci = 0; voci < cell.size(); ++voci)    
            {
               if (!vertex_con[ cell[voci] ])
                  vertex_con[ cell[voci] ] = true;
            }           
         }
         //
         // RESIZE VERTEX CONTAINER OF THE ACTUAL SEGMENT ACCORDING TO THE UNIQUE VERTEX CONTAINER
         //      
         gsse::resize(vertex_con.size())(gsse::at_dim<AT_vx>( gsse::at_fiber(gsse::at(si)(segments_topology))));
         //
         // SETUP TRANSFER UNIQUE VERTEX DATASET TO THE SEGMENT VERTEX CONTAINER
         //        
         size_t   vindex = 0;
         for(vertex_con_t::iterator viter = vertex_con.begin(); viter != vertex_con.end(); ++viter)
         {
            gsse::at_index(gsse::at(vindex)(gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(si)(segments_topology))))) = (*viter).first; 
            //
            // setup index space link: 
            //
            //       key:     real vertex index
            //       value:   base space index
            //        
            vertex_map[(*viter).first] = vindex;         
            vindex++;
         }
         //
         // SETUP CELL ON VERTEX RELATION
         //                  

         // [TODO] find a more settle solution - not just resize the whole fiberbundle !!!!!!
         for(size_t vi = 0 ; vi < gsse::size(gsse::at_dim<AT_vx>( gsse::at_fiber(gsse::at(si)(segments_topology)))); ++vi)
         {      
//          gsse::dump_type(gsse::at_fiber(gsse::at(vi)(gsse::at_dim<AT_vx>( gsse::at_fiber(gsse::at(si)(segments_topology)))))); 
//          std::cout << "..before resize: ";
//          gsse::print_1d(gsse::at_fiber(gsse::at(vi)(gsse::at_dim<AT_vx>( gsse::at_fiber(gsse::at(si)(segments_topology)))))); 
//          std::cout << std::endl;

            gsse::resize(0)(gsse::at_fiber(gsse::at(vi)(gsse::at_dim<AT_vx>( gsse::at_fiber(gsse::at(si)(segments_topology)) )   )));

//          std::cout << "..after resize: ";
//          gsse::print_1d(gsse::at_fiber(gsse::at(vi)(gsse::at_dim<AT_vx>( gsse::at_fiber(gsse::at(si)(segments_topology)))))); 
//          std::cout << std::endl;
         }

         // [TODO] find a more settle solution - not just resize the whole fiberbundle !!!!!!
//      gsse::resize(0)(gsse::at_dim<AT_vx>( gsse::at_fiber(gsse::at(si)(segments_topology))));

#ifdef DEBUG_FULL
         std::cout << "\t ..after resize: " << gsse::size(gsse::at_dim<AT_vx>( gsse::at_fiber(gsse::at(si)(segments_topology))));
         gsse::print_1d(gsse::at_fiber(gsse::at(0)(gsse::at_dim<AT_vx>( gsse::at_fiber(gsse::at(si)(segments_topology)))))); 
#endif
    
         for (size_t ci = 0 ; ci < cellsize; ++ci)
         {
            CellT cell = gsse::at(ci) (gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology)))	);      
            for (size_t voci = 0; voci < cell.size(); ++voci)    
            {
               gsse::pushback // store on the fiber (vector<long>)
                  ( 
                     gsse::at_fiber // access the fiber of this vertex
                     (
                        //
                        // use index space link to derive the vertex container position(aka base space index) 
                        // for a given real vertex index out of the sparse vertex index space
                        //                |
                        //                v
                        gsse::at(vertex_map[gsse::at(voci)(cell)]) //  access the vertex container element, a certain vertex
                        (gsse::at_dim<AT_vx>( gsse::at_fiber(gsse::at(si)(segments_topology)) ) ) // <- vertex container
                     ) 
                  ) = ci;  

#ifdef DEBUG_FULL
               std::cout << "..cell on vertex(" << vertex_map[gsse::at(voci)(cell)] << ") = " << ci << std::endl;
#endif

            } 


            // [TODO] calculate the normal vector of this cell and store it in FB
            //
	 
         }
      }
      //
      // CREATE EDGES
      //
      gsse::resize(0)(gsse::at_dim<AT_ee>( gsse::at_fiber((gsse::at(0) (segments_topology)))));
      gsse::create_edges(full_space);
   
#ifdef DEBUG_FULL
      for (size_t si = 0; si < segment_size; ++si)
      {  
         stream << "segment-number " << si << std::endl;
         stream << "  vertexsize " << gsse::size(gsse::at_dim<AT_vx>( gsse::at_fiber((gsse::at(si) (segments_topology))) )) << std::endl;
         stream << "  edgesize " << gsse::size(gsse::at_dim<AT_ee>( gsse::at_fiber((gsse::at(si) (segments_topology))) )) << std::endl;
         stream << "  cellsize " << gsse::size(gsse::at_dim<AT_cl>( gsse::at_fiber((gsse::at(si) (segments_topology))) )) << std::endl;
         
         std::cout << "\tsegment #: " << si << std::endl;
         std::cout << "\t\tvertexsize: \033[1;32m"
                   << gsse::size(gsse::at_dim<AT_vx>( gsse::at_fiber((gsse::at(si) (segments_topology))) )) 
                   << "\033[0m" << std::endl;
         std::cout << "\t\tcellsize: \033[1;32m"  
                   << gsse::size(gsse::at_dim<AT_cl>( gsse::at_fiber((gsse::at(si) (segments_topology))) )) 
                   << "\033[0m" << std::endl;
         std::cout << "\t\tedgesize: \033[1;32m"  
                   << gsse::size(gsse::at_dim<AT_ee>( gsse::at_fiber((gsse::at(si) (segments_topology))) )) 
                   << "\033[0m" << std::endl;
      }
#endif
   }


   template <typename Space, typename CellT, typename SubSpaceCCI, typename GlobLocMapT, typename BoundContT>
   void count_neighbour_cell (CellT&         cell, 
                              SubSpaceCCI&   ss_cci,
                              GlobLocMapT&   global_local,
                              BoundContT&    boundary)
   {
      typedef gsse::property_space<Space>  space_property_type;

//    typedef typename gsse::result_of::cell_on_edge<space_property_type>::type   cl_on_ee_type;
//    typedef typename gsse::result_of::cell_on_edge<space_property_type>::result cl_on_ee_result;

//    typedef typename gsse::result_of::edge_on_cell<space_property_type>::type   ee_on_cl_type;
//    typedef typename gsse::result_of::edge_on_cell<space_property_type>::result ee_on_cl_result;

//       typedef typename gsse::result_of::cell_on_facet<space_property_type>::type   cl_on_f0_type;
//       typedef typename gsse::result_of::cell_on_facet<space_property_type>::result cl_on_f0_result;

//       typedef typename gsse::result_of::facet_on_cell<space_property_type>::type   f0_on_cl_type;
//       typedef typename gsse::result_of::facet_on_cell<space_property_type>::result f0_on_cl_result;
      
//       typedef typename gsse::boundary<space_property_type::DIMT,space_property_type::DIMT-2, typename space_property_type::CellTopology>::type   f0_on_cl_type;
//       typedef typename gsse::result_of::boundary<space_property_type::DIMT,space_property_type::DIMT-2, typename space_property_type::CellTopology>::result f0_on_cl_result;

      static const long DIM     = space_property_type::DIMT;
      static const long DIM_bnd = 1; // [FS] originally it was DIM-2 taken from facet_on_cell

      typedef typename gsse::boundary<DIM, DIM_bnd, typename gsse::cell_simplex>   f0_on_cl_type;
      typedef typename gsse::result_of::boundary<DIM, DIM_bnd, typename gsse::cell_simplex>::type f0_on_cl_result;

      typedef gsse::segment_coboundary<
      typename gsse::result_of::val<typename space_property_type::SubSpace_CellComplex>::type
         , DIM-1 ,1, typename space_property_type::CellTopology>  cl_on_f0_type;

      typedef typename gsse::result_of::coboundary<2, DIM-2, typename space_property_type::CellTopology>::type   cl_on_f0_result;



#ifdef DEBUG_FULL
      std::cout << "cell: " << cell << std::endl;
#endif  

      f0_on_cl_type    facet_on_cell;    
      f0_on_cl_result  f0_on_cl = facet_on_cell ( cell );

      cl_on_f0_type    cell_on_facet ( ss_cci  );
      for (size_t cnt_f0 = 0; cnt_f0 < gsse::size( f0_on_cl ); ++cnt_f0)
      {
         cl_on_f0_result cl_on_f0 = cell_on_facet( gsse::at(cnt_f0) (f0_on_cl)  ,   global_local );

#ifdef DEBUG_FULL
         std::cout << "facet:" << gsse::at(cnt_f0) (f0_on_cl); 

//          std::cout << "\tcell_on_facet_size .. facet: " 
//                    <<  global_local  [gsse::at(cnt_f0) (f0_on_cl)[0]] << "/" 
//                    <<  global_local  [gsse::at(cnt_f0) (f0_on_cl)[1]] << "/" 
//                    <<  global_local  [gsse::at(cnt_f0) (f0_on_cl)[2]];

         std::cout << "\tsize:  " << gsse::size(cl_on_f0) << std::endl;
#endif   
                  
         if (gsse::size (cl_on_f0) == 1)
         {
            // store the boundary simplex on the passed container 
            // (for later processing)
            //
            std::cout << "..boundary element: ";
            typename BoundContT::value_type bound_element;
            for(size_t i=0; i<gsse::size(bound_element); ++i)
            {
               gsse::at(i)(bound_element) = gsse::at(gsse::at(i)(gsse::at(cnt_f0)(f0_on_cl)))(global_local);
               std::cout << gsse::at(gsse::at(i)(gsse::at(cnt_f0)(f0_on_cl)))(global_local) << " ";
            }
            std::cout << std::endl;

            gsse::pushback(boundary) = bound_element;
         }
      }
   }


   
   template <typename InputSpaceT, typename OutputSpaceT>
   void extract_boundary(InputSpaceT& input_space, OutputSpaceT& output_space, std::ostream& stream = std::cout)
   {
      typedef gsse::property_space<InputSpaceT>                                    gssespace_type;      
      typedef typename gsse::result_of::property_Geometry<gssespace_type>::type    geometry_type;
      typedef typename gsse::result_of::property_SubSpaceCC<gssespace_type>::type  subspace_cc_type;      
      typedef typename gsse::result_of::property_CellComplex<gssespace_type>::type cellcomplex_type;      
      typedef typename gsse::result_of::property_CellT<gssespace_type>::type       cell_type;
      typedef typename gsse::result_of::property_CoordT<gssespace_type>::type      coord_type;


      typedef gsse::property_space<OutputSpaceT>                                       gsse_outspace_type;     
      typedef typename gsse::result_of::property_Geometry<gsse_outspace_type>::type    out_geometry_type;
      typedef typename gsse::result_of::property_SubSpaceCC<gsse_outspace_type>::type  out_subspace_cc_type;
      typedef typename gsse::result_of::property_CoordT<gsse_outspace_type>::type      out_coord_type;
      typedef typename gsse::result_of::property_CellT<gsse_outspace_type>::type       out_cell_type;
          
      // extra declarations
      //
//       typedef typename gsse::result_of::at_dim<cellcomplex_type, gsse::AT_f0>::type  container_f0_type;
//       typedef typename gsse::result_of::val<container_f0_type>::type                 trig_type;
      

      // [INFO] fill necessary container
      initialize_topology(input_space);


      subspace_cc_type& ss_cc = gsse::at_dim<gsse::AC>(input_space);
      geometry_type&    geom  = gsse::at_dim<gsse::AP>(input_space);
           
      out_subspace_cc_type& ss_cc_out = gsse::at_dim<gsse::AC>(output_space);
      out_geometry_type&    geom_out  = gsse::at_dim<gsse::AP>(output_space);

      std::map<coord_type, long> point_map;
         
      for (size_t ssi = 0; ssi < gsse::size(ss_cc); ++ssi)   
      {
         cellcomplex_type& cellcomplex = gsse::at_fiber(gsse::at(ssi)( ss_cc ));
         
         // [INFO] setup global-local index space link
         //
         size_t vertexsize = gsse::size( gsse::at_dim<gsse::AT_vx>(cellcomplex) );
         std::map<long, long>   global_local;
         for (size_t vi = 0; vi < vertexsize; ++vi )
         {
            // [RH][TODO] ..use sparse index space 
            // [FS][TODO] ..this assignment does not work using a 2D space => check this!!!
//             long real_index = gsse::at_index(gsse::at(vi) ( gsse::at_dim<gsse::AT_vx>(cellcomplex)));   
//             global_local[real_index] = vi;
            global_local[vi] = vi;
         }    
         
         // [INFO] iterate over the cells and check the coboundary size => has to be 2
         //
         size_t cellsize = gsse::size( gsse::at_dim<gsse::AT_cl>(cellcomplex) );
         for (size_t ci = 0; ci < cellsize; ++ci)
         {
            cell_type real_cell = gsse::at(ci) ( gsse::at_dim<AT_cl>( cellcomplex ) );   
         
            std::vector<out_cell_type> boundary;
         

            count_neighbour_cell<InputSpaceT>(real_cell, gsse::at(ssi)(ss_cc), global_local, boundary);  

         
            for(size_t i=0; i<gsse::size(boundary); ++i)
            {
               out_cell_type temp_cell;
            
               for(size_t j=0; j<gsse::size(gsse::at(i)(boundary)); ++j)
               {
                  coord_type new_point = gsse::at(gsse::at(j)(gsse::at(i)(boundary)))(geom);
               
                  typename std::map<coord_type, long>::iterator result = point_map.find(new_point);
                  if(result == point_map.end())
                  {
                     gsse::at(j)(temp_cell) = gsse::size(geom_out);
                     point_map[new_point] = gsse::size(geom_out);
                     gsse::at(gsse::size(geom_out))(geom_out) = new_point;                  
                  }
                  else
                  {
                     gsse::at(j)(temp_cell) = (*result).second;
                  }
               }

               gsse::pushback(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(ssi)(ss_cc_out)))) = temp_cell;
            }
         }     
         
      }                                

      // [TODO] apply orient algorithm
      GSSELOGGER(gsse::logger::red) << "\n\tINFO: output is not oriented .. apply orientation functor!!!\n\n";
   }
   
} // namespace gsse


#endif
