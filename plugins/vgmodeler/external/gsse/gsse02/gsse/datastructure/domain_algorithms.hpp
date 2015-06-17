/* ============================================================================
   Copyright (c) 2008-2010 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_CONTAINER_SPACE_ALGORITHMS_HH)
#define GSSE_CONTAINER_SPACE_ALGORITHMS_HH

// *** system includes
#include <iomanip>
// *** BOOST includes
// *** BOOST includes
#include <boost/xpressive/xpressive.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/less.hpp>
#include <boost/utility/enable_if.hpp>
// *** GSSE includes
#include "gsse/util/size.hpp"
#include "gsse/datastructure/domain.hpp"
#include "gsse/geometry/metric_object.hpp"
#include "gsse/algorithm.hpp"
#include "gsse/traversal.hpp"
#include "gsse/util.hpp"

#include "gsse/topology/cells.hpp"
#include "gsse/topology/boundary.hpp"
#include "gsse/topology/coboundary.hpp"

// ############################################################
//

namespace gsse {


// #############################################################################################################
//   gssev02 :: init domain part
// #############################################################################################################
//
template<typename Domain>
void init(Domain& domain, long number_cells = 10)
{
   using namespace gsse::access_specifier;   // AC, AQ, ...

   typedef typename gsse::result_of::at_dim<Domain, AC>::type SpaceTopologySegmentsT;
   typedef typename gsse::result_of::at_dim<Domain, AQ>::type SpaceQuantitySegmentsT;
   typedef typename gsse::result_of::at_dim<Domain, AP>::type FBPosT;

   typedef typename gsse::result_of::at_dim_index<SpaceTopologySegmentsT>::type  CellComplex;
   typedef typename gsse::result_of::at_dim_index<SpaceQuantitySegmentsT>::type  QuanComplex;
   typedef typename gsse::result_of::at_dim_isd  <FBPosT>::type                  CoordPosT;

   typedef typename gsse::result_of::at_dim<QuanComplex, AT_vx>::type            QuanForm_vx;
   typedef typename gsse::result_of::at_isd<QuanForm_vx, 1>::type                StorageQF_vx;
   typedef typename gsse::result_of::at_dim<QuanComplex, AT_cl>::type            QuanForm_cl;
   typedef typename gsse::result_of::at_isd<QuanForm_cl, 1>::type                StorageQF_cl;
//    dump_type<QuanDynamicForm>();
//    dump_type<StorageDynamicForm>();


   // create object instances
   //
   SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(domain);
   SpaceQuantitySegmentsT& segments_quantity = gsse::at_dim<AQ>(domain);
   FBPosT&                 geometry          = gsse::at_dim<AP>(domain);


   long size_segments = 2;
   long size_vertices = number_cells + 3;

   //######################################## prepare domain #####################################################
   //
   gsse::resize(size_segments)(segments_topology);
   gsse::resize(size_segments)(segments_quantity);

   std::string name_segments;
   for (long cnt_seg = 0; cnt_seg < size_segments; ++cnt_seg)
   {
      name_segments = "testsegment_" + boost::lexical_cast<std::string>(cnt_seg);

      gsse::at_index(gsse::at(cnt_seg)(segments_topology)) = name_segments;
      gsse::at_index(gsse::at(cnt_seg)(segments_quantity)) = name_segments;
   }



   // global metric space
   //
   for (long i =0 ; i < size_vertices; ++i)
   {
      CoordPosT coord;
      gsse::algorithm::init(coord,  gsse::algorithm::increment<double>(i) );
      gsse::at(i)(geometry) = coord;
   }



   // #############################################################################################################




   for (long cnt_seg = 0; cnt_seg < size_segments; ++cnt_seg)
   {
      // ###################################### complex:: cell (topological space) ###########################################
      //
      long size_segment_cells = number_cells;

      // [info] AT_cl -> 2D topological space (simplex indexed)
      //   depends on object_dimension
      //
      typedef typename gsse::result_of::at_dim_isd<CellComplex, AT_cl>::type CellT;  // ISD = 0
      for (long i = 0 ; i < size_segment_cells; ++i)
      {
         CellT cell;
         gsse::algorithm::init(cell,  gsse::algorithm::increment<long>(i) );

         gsse::pushback(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at(cnt_seg)(segments_topology)))) = cell;

         // cell on vertex information
         //
         for (size_t inside_i = 0; inside_i < gsse::size(cell); ++inside_i)
         {

            gsse::pushback( gsse::at_fiber( gsse::at_vx(  gsse::at(inside_i)(cell)) ( gsse::at(cnt_seg)(segments_topology)) )  ) = i;
//            gsse::pushback( gsse::at(  gsse::at(inside_i)(cell))( gsse::at_dim<AT_vx>( gsse::at_fiber(gsse::at(cnt_seg)(segments_topology)) ) ) ) = i;   // gsse::at is always safe
         }
      }



      // ###################################### complex:: quantity (physical space) ###########################################
      //
      {
         // create vertex-quantity first
         //
         std::string quan_dimension = "vertex";
         size_t nb_quantities = 1;

         for (size_t nb_q = 0; nb_q < nb_quantities; ++nb_q)
         {
            std::string quan_name = "testquanvx_" + boost::lexical_cast<std::string>(nb_q);
            size_t quan_size        = 0;

            if (quan_dimension == "vertex")
               quan_size = size_vertices;
            else   if (quan_dimension == "cell")
               quan_size = number_cells;

            for (size_t i = 0; i < quan_size; ++i)
            {
               long object_handle = i;


               if (quan_dimension == "vertex")
               {
                  StorageQF_vx quan;
                  gsse::resize(1)(quan); // build a dynamic form of size: 1

                  gsse::algorithm::init(quan,  gsse::algorithm::increment<double>(i) );
//                  gsse::algorithm::print_rt(quan);

                  gsse::at(object_handle)(gsse::at(quan_name)(gsse::at_dim<AT_vx>(  gsse::at_fiber(gsse::at(cnt_seg)(segments_quantity))))) = quan;
               }
               else if (quan_dimension == "cell")
               {
                  StorageQF_cl quan;
                  gsse::resize(1)(quan); // build a dynamic form of size: 1 (scalar)
                  gsse::algorithm::init(quan,  gsse::algorithm::increment<double>(i) );

                  gsse::at(object_handle)(gsse::at(quan_name)(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at(cnt_seg)(segments_quantity))))) = quan;
               }
            }
         } // nb quantities
      } // vertex quantities


      {
         // create cell-quantity next
         //
         std::string quan_dimension = "cell";
         size_t nb_quantities = 2;

         for (size_t nb_q = 0; nb_q < nb_quantities; ++nb_q)
         {
            std::string quan_name = "testquancl_" + boost::lexical_cast<std::string>(nb_q);
            size_t quan_size        = 0;
            if (quan_dimension == "vertex")
               quan_size = size_vertices;
            else   if (quan_dimension == "cell")
               quan_size = number_cells;


            for (size_t i = 2; i < 2+quan_size; ++i)   // just for demonstration (variable index space)
            {
               long object_handle = i;


               if (quan_dimension == "vertex")
               {
               }
               else if (quan_dimension == "cell")
               {
                  StorageQF_cl  quan;
                  gsse::algorithm::init(quan,  gsse::algorithm::increment<double>(i) );
                  gsse::resize(3)(quan); // build a dynamic form of size: 3

                  gsse::at(object_handle)(gsse::at(quan_name)(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at(cnt_seg)(segments_quantity))))) = quan;
               }
            }
         } // nb_quantities
      }  // cell quantities
   } // segment loop
}



// #############################################################################################################
//   gssev02 :: init topology part
// #############################################################################################################
//
template<typename Domain>
void init_topology(Domain& domain, size_t number_cells = 10)
{
   using namespace gsse::access_specifier;   // AC, AQ, ...

   typedef typename gsse::result_of::at_dim<Domain, AC>::type SpaceTopologySegmentsT;
   typedef typename gsse::result_of::at_dim<Domain, AQ>::type SpaceQuantitySegmentsT;
   typedef typename gsse::result_of::at_dim<Domain, AP>::type FBPosT;

   typedef typename gsse::result_of::at_dim_index<SpaceTopologySegmentsT>::type  CellComplex;
   typedef typename gsse::result_of::at_dim_index<SpaceQuantitySegmentsT>::type  QuanComplex;
   typedef typename gsse::result_of::at_dim_isd  <FBPosT>::type                  CoordPosT;

   typedef typename gsse::result_of::at_dim<QuanComplex, AT_vx>::type            QuanDynamicForm;
   typedef typename gsse::result_of::at_isd<QuanDynamicForm, 1>::type            StorageDynamicForm;
//    dump_type<QuanDynamicForm>();
//    dump_type<StorageDynamicForm>();


   // create object instances
   //
   SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(domain);
   SpaceQuantitySegmentsT& segments_quantity = gsse::at_dim<AQ>(domain);
   FBPosT&                 geometry          = gsse::at_dim<AP>(domain);


   size_t size_segments = 2;
   size_t size_vertices = number_cells + 3;

   //######################################## prepare domain #####################################################
   //
   gsse::resize(size_segments)(segments_topology);
   gsse::resize(size_segments)(segments_quantity);

   std::string name_segments;
   for (size_t cnt_seg = 0; cnt_seg < size_segments; ++cnt_seg)
   {
      name_segments = "testsegment_" + boost::lexical_cast<std::string>(cnt_seg);

      gsse::at_index(gsse::at(cnt_seg)(segments_topology)) = name_segments;
      gsse::at_index(gsse::at(cnt_seg)(segments_quantity)) = name_segments;
   }



   // global metric space
   //
   for (size_t i =0 ; i < size_vertices; ++i)
   {
      CoordPosT coord;
      gsse::algorithm::init(coord,  gsse::algorithm::increment<double>(i) );
      gsse::at(i)(geometry) = coord;
   }



   // #############################################################################################################
   // #############################################################################################################




   for (size_t cnt_seg = 0; cnt_seg < size_segments; ++cnt_seg)
   {
      // ###################################### complex:: cell (topological space) ###########################################
      //
      size_t size_segment_cells = number_cells;

      typedef typename gsse::result_of::at_dim_isd<CellComplex, AT_cl>::type CellT;  // ISD = 0
      for (size_t i = 0 ; i < size_segment_cells; ++i)
      {
         CellT cell;
         gsse::algorithm::init(cell,  gsse::algorithm::increment<long>(i) );

         gsse::pushback(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at(cnt_seg)(segments_topology)))) = cell;

         // ## cell on vertex information
         //
         for (size_t inside_i = 0; inside_i < gsse::size(cell); ++inside_i)
         {
            gsse::pushback( gsse::at_fiber( gsse::at_vx(  gsse::at(inside_i)(cell)) ( gsse::at(cnt_seg)(segments_topology)) )  ) = i;
         }
      }


      for (size_t cnt_vx = 0; cnt_vx < gsse::size( gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(cnt_seg)(segments_topology) ))); ++cnt_vx)
      {
         // detects automatically if an index space is available
         //  if there is none -> a gsse::identity object is returned
         //
         gsse::at_index (
                          gsse::at(cnt_vx)(
                                            gsse::at_dim<AT_vx>( gsse::at_fiber(gsse::at(cnt_seg)(segments_topology)) )
                                           )
                          ) = cnt_vx;
      }
   } // segment loop
}




// #############################################################################################################
//   gssev02 :: full domain reader
// #############################################################################################################
//
template<typename Geometry, typename IStream>
long read_coordinates(Geometry&    geometry,
                      IStream&     istr,
                      std::string  value_type,
                      long         dimension,
                      long         cardinality)
{
   typedef typename gsse::result_of::val <Geometry>::type    CoordPosT;
   std::string temp_input;

   // ## parse ##
   //
   istr >> temp_input;   // should be {

   for (long dc = 0; dc < cardinality; ++dc)
   {
      CoordPosT coord;
      istr >> temp_input;   // should be {
      for (long dd = 0; dd < dimension; ++dd)
      {
         istr >> coord[dd];
      }
      // std::cout << ".. coord: " << coord << std::endl;
      gsse::at(dc)(geometry) = coord;

      istr >> temp_input;   // should be }
   }

   istr >> temp_input;   // should be }

   if (temp_input == "}")
      return 0;
   else
      return -1;// error in parsing
}



template<typename CellComplex, typename IStream>
long read_cellcomplex_cl(CellComplex&  topology,
                         IStream&      istr,
                         std::string   value_type,
                         long          dimension,
                         long          cardinality,
                         std::string   object_dimension)
{
   using namespace gsse::access_specifier;   // AC, AQ, ...

   typedef typename gsse::result_of::at_dim_isd<CellComplex, AT_cl>::type CellT;  // ISD = 0
   std::string temp_input;

   // ## parse ##
   //
   istr >> temp_input;   // should be {
   for (long dc = 0; dc < cardinality; ++dc)
   {
      CellT cell;
      istr >> temp_input;   // should be {

      long temp_index; // [RH][TODO] .. for ISD = 1, include this index element
      istr >> temp_index;

      // [RH][TODO] // dimension is always the actual dimension of the set
      //
      for (size_t dd = 0; dd < cell.size(); ++dd)    // can also be calculated by dimension -> cell type
      {
         istr >> cell[dd];
      }
//      std::cout << ".. cell: " << cell << std::endl;
      gsse::pushback(gsse::at_dim<AT_cl>(  topology )) = cell;
// ISD = 1
//      gsse::at(temp_index)(gsse::at_dim<AT_cl>(  topology )) = cell;

      istr >> temp_input;   // should be }
   }
   istr >> temp_input;   // should be }

   if (temp_input == "}")
      return 0;
   else
      return -1;// error in parsing
}


template<typename CellComplex, typename IStream>
long read_cellcomplex_vx(CellComplex&  topology,
                         IStream&      istr,
                         std::string   value_type,
                         long          dimension,
                         long          cardinality,
                         std::string   object_dimension)
{
   using namespace gsse::access_specifier;   // AC, AQ, ...
   typedef typename gsse::result_of::at_fiber_val<typename gsse::result_of::at_dim_isd<CellComplex, AT_vx>::type>::type CellT;

   std::string temp_input;

   // ## parse ##
   //
   istr >> temp_input;   // should be {
   for (long dc = 0; dc < cardinality; ++dc)
   {
      std::string str_card;
      istr >> str_card;
//      std::cout << "str card: " << str_card << std::endl;

      istr >> temp_input;   // should be {
      long size_cell;
      istr >> size_cell;
      istr >> temp_input;   // should be }
//      std::cout << "cell size: " << size_cell << std::endl;

      istr >> temp_input;   // should be {
      long temp_index; // [RH][TODO] .. for ISD = 1, include this index element
      istr >> temp_index;

      // dimension -> should be -1 (means arbitary number of elements)
      //
      CellT cell;
      gsse::resize(size_cell)(cell);

      for (long dd = 0; dd < size_cell; ++dd)
      {
         istr >> cell[dd];
//         std::cout << " " << cell[dd];
      }
//       std::cout << std::endl;

      gsse::at_fiber (gsse::pushback ( gsse::at_dim<AT_vx>(topology) )) = cell;

// if ISD == 1
//      gsse::at(temp_index)(gsse::at_dim<AT_vx>(  topology )) = cell;

      istr >> temp_input;   // should be }
   }
   istr >> temp_input;   // should be }

   if (temp_input == "}")
      return 0;



   return -1;// error in parsing
}



template<typename QuantityComplex, typename IStream>
long read_quantity(QuantityComplex& quantity_complex,
                   IStream&     istr,
                   std::string  complex_dim,
                   std::string  value_type,
                   std::string  quan_name,
                   long         dimension,   // can be -1 -> arbitrary size
                   long         cardinality)
{
   using namespace gsse::access_specifier;   // AC, AQ, ...

//   dump_type<CellComplex>();
   typedef typename gsse::result_of::at_dim<QuantityComplex, AT_vx>::type    QuanDynamicForm;  // [RH][TODO]
   typedef typename gsse::result_of::at_isd<QuanDynamicForm, 1>::type        StorageDynamicForm;


   std::string temp_input;
   long size_container;

   // ## parse ##
   //
   istr >> temp_input;   // should be {
   for (long dc = 0; dc < cardinality; ++dc)
   {
      istr >> temp_input;

      if ( (temp_input == "Cardinality") && (dimension == (-1) ))
      {
         istr >> temp_input; // should be {
         istr >> size_container;
         istr >> temp_input; // should be }
      }


      // ###
      istr >> temp_input;   // should be {

      long object_handle;
      istr >> object_handle;

      StorageDynamicForm quan;
      gsse::resize( size_container ) (quan);
      for (long dd = 0; dd < size_container; ++dd)
      {
         istr >> quan[dd];
      }

      if (complex_dim == "Vertex")
      {
         gsse::at(object_handle)(gsse::at(quan_name)(gsse::at_dim<AT_vx>(  quantity_complex ))) = quan;
      }
      else if (complex_dim == "Cell")
      {
         gsse::at(object_handle)(gsse::at(quan_name)(gsse::at_dim<AT_cl>(  quantity_complex ))) = quan;
      }

      istr >> temp_input;   // should be }
   }
   istr >> temp_input;   // should be }


   if (temp_input == "}")
      return 0;
   else
      return -1;// error in parsing
}


// #############################################################################################################
//   gssev02 :: complete topological information in domain
// #############################################################################################################
//
template<typename Domain>
void create_cl_on_vx(Domain& domain)
{
   using namespace gsse::access_specifier;

   typedef typename gsse::result_of::at_dim<Domain, AC>::type SpaceTopologySegmentsT;
   typedef typename gsse::result_of::at_dim_index<SpaceTopologySegmentsT>::type  CellComplex;

   // create object instances
   //
   SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(domain);


   for (size_t cnt_seg = 0; cnt_seg < gsse::size( segments_topology ) ; ++cnt_seg)
   {
      typedef typename gsse::result_of::at_dim_isd<CellComplex, AT_cl>::type CellT;  // ISD = 0
      for (size_t i = 0 ; i < gsse::size ( gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(cnt_seg)(segments_topology))) ); ++i)
      {
         CellT cell;
         cell = gsse::at(i)(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at(cnt_seg)(segments_topology)))) ;

         // ## cell on vertex information
         //
         for (size_t inside_i = 0; inside_i < gsse::size(cell); ++inside_i)
         {
            gsse::pushback( gsse::at_fiber( gsse::at_vx(  gsse::at(inside_i)(cell)) ( gsse::at(cnt_seg)(segments_topology)) )  ) = i;
         }
      }

      for (size_t cnt_vx = 0; cnt_vx < gsse::size( gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(cnt_seg)(segments_topology) ))); ++cnt_vx)
      {
         // detects automatically if an index space is available
         //  if there is none -> a gsse::identity object is returned
         //
         gsse::at_index (
                          gsse::at(cnt_vx)(
                                            gsse::at_dim<AT_vx>( gsse::at_fiber(gsse::at(cnt_seg)(segments_topology)) )
                                           )
                          ) = cnt_vx;
      }
   } // segment loop
}



template<typename Domain>
void read(Domain& domain, std::istream& istr = std::cin)
{
   using namespace gsse::access_specifier;   // AC, AQ, ...

   // logger  (important for debugging the file hierarchy)
   //
   std::ostream& ostr_log = std::cerr;

   // data type calculation
   //
   typedef typename gsse::result_of::at_dim<Domain, AC>::type SpaceTopologySegmentsT;
   typedef typename gsse::result_of::at_dim<Domain, AQ>::type SpaceQuantitySegmentsT;
   typedef typename gsse::result_of::at_dim<Domain, AP>::type FBPosT;

   typedef typename gsse::result_of::at_dim_index<SpaceTopologySegmentsT>::type  CellComplex;
   typedef typename gsse::result_of::at_dim_index<SpaceQuantitySegmentsT>::type  QuanComplex;
   typedef typename gsse::result_of::at_dim_isd  <FBPosT>::type                  CoordPosT;

   typedef typename gsse::result_of::at_dim<QuanComplex, AT_vx>::type            QuanDynamicForm;
   typedef typename gsse::result_of::at_isd<QuanDynamicForm, 1>::type            StorageDynamicForm;


   // ===============================================================================================
   // create object instances
   //
   SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(domain);
   SpaceQuantitySegmentsT& segments_quantity = gsse::at_dim<AQ>(domain);
   FBPosT&                 geometry          = gsse::at_dim<AP>(domain);


   // construct regular expressions for GSSE file format
   //
   boost::xpressive::sregex rx_type        = boost::xpressive::sregex::compile( "Type \\{ (\\w+) \\} .*" );
   boost::xpressive::sregex rx_valuetype   = boost::xpressive::sregex::compile( ".* ValueType \\{ (\\w+) (\\w+) \\} .*" );
   boost::xpressive::sregex rx_dimension   = boost::xpressive::sregex::compile( ".* Dimension \\{ (\\w+) \\} .*" );
   boost::xpressive::sregex rx_cardinality = boost::xpressive::sregex::compile( ".* Cardinality \\{ (\\w+) \\} .*" );
   boost::xpressive::sregex rx_number      = boost::xpressive::sregex::compile( ".* Number \\{ (\\w+) \\} .*" );
   boost::xpressive::sregex rx_name        = boost::xpressive::sregex::compile( ".* Name \\{ (\\w+) \\} .*" );
   boost::xpressive::sregex rx_objectdim   = boost::xpressive::sregex::compile( ".* ObjectDimension \\{ (\\w+) \\} .*" );
   boost::xpressive::sregex rx_data        = boost::xpressive::sregex::compile( ".* Data(\\w+)" );
   boost::xpressive::smatch what;

   while (!istr.eof())
   {
      std::string input_string;
      std::getline(istr, input_string);

      std::string identifier_domain_type;
      std::string identifier_domain_valuetype;
      std::string identifier_domain_valuetype_format;  // NOT indexed is default
      long identifier_domain_dimension(0);
      long identifier_domain_cardinality(0);
      std::string identifier_domain_name;
      std::string identifier_domain_data;

      // decode string
      //
      ostr_log << "## input string: " << input_string << std::endl;

      if( boost::xpressive::regex_match( input_string, what, rx_type ) )
      {
         ostr_log << "Type: " ;
         ostr_log << what[1] << std::endl;
         identifier_domain_type = what[1];
      }

      if( boost::xpressive::regex_match( input_string, what, rx_valuetype ) )
      {
         ostr_log << "ValueType: " ;
         ostr_log << what[1];
         identifier_domain_valuetype = what[1];
         if (what.size() > 2)
         {
            ostr_log << "  ";
            ostr_log << what[2];
            identifier_domain_valuetype_format = what[2];
         }
         ostr_log << std::endl;
      }

      if( boost::xpressive::regex_match( input_string, what, rx_dimension ) )
      {
         ostr_log << "Dimension: " ;
         ostr_log << what[1] << std::endl;
         identifier_domain_dimension = boost::lexical_cast<long>( what[1] );
      }

      if( boost::xpressive::regex_match( input_string, what, rx_cardinality ) )
      {
         ostr_log << "Cardinality: ";
         ostr_log << what[1] << std::endl;
         identifier_domain_cardinality = boost::lexical_cast<long>( what[1] );
      }

      if( boost::xpressive::regex_match( input_string, what, rx_name ) )
      {
         ostr_log << "Name: " ;
         ostr_log << what[1] << std::endl;
         identifier_domain_name = what[1];
      }

      if( boost::xpressive::regex_match( input_string, what, rx_data ) )
      {
         ostr_log << "Data: " ;
         ostr_log << what[1] << std::endl;
         identifier_domain_data = what[1];
      }

      // ##########
      // ########################### Level:: domain ##########################
      //
      if (identifier_domain_type==IO::gsse_coordinates)
      {
         long rd_val = read_coordinates(geometry,
                                        istr,
                                        identifier_domain_valuetype,
                                        identifier_domain_dimension,
                                        identifier_domain_cardinality);
         if (rd_val != 0)
         {
            std::cout << "## error in read_coordinates.. " << std::endl;
//             exit(-1);
         }
      }
      else if (identifier_domain_type==IO::gsse_space)
      {
         // read
         //
         std::string identifier_space_type;
         long identifier_space_number(0);
         std::string identifier_space_name;
         std::string identifier_space_data;

         std::getline(istr, input_string);
         ostr_log << "###### space level: " << input_string << std::endl;

         for (long gsse_segment_i = 0; gsse_segment_i < identifier_domain_cardinality; ++gsse_segment_i)
         {
            ostr_log << "gsse segment i: " << gsse_segment_i << std::endl;

            if( boost::xpressive::regex_match( input_string, what, rx_type ) )
            {
               ostr_log << "Type: " ;
               ostr_log << what[1] << std::endl;
               identifier_space_type = what[1];
            }
            if( boost::xpressive::regex_match( input_string, what, rx_number ) )
            {
               ostr_log << "Number: ";
               ostr_log << what[1] << std::endl;
               identifier_space_number = boost::lexical_cast<long>( what[1] );
            }
            if( boost::xpressive::regex_match( input_string, what, rx_name ) )
            {
               ostr_log << "Name: " ;
               ostr_log << what[1] << std::endl;
               identifier_space_name = what[1];
            }
            if( boost::xpressive::regex_match( input_string, what, rx_data ) )
            {
               ostr_log << "Data: " ;
               ostr_log << what[1] << std::endl;
               identifier_space_data = what[1];
            }

            gsse::at_index(gsse::at(identifier_space_number)(segments_topology)) = identifier_space_name;
            gsse::at_index(gsse::at(identifier_space_number)(segments_quantity)) = identifier_space_name;


            // ########################### Level:: segment / complex ########################
            //
            while (1)
            {
               // [RH][TODO]
               //  check if physical space -> valuetype == type
               //
               std::string identifier_segment_type;
               std::string identifier_segment_valuetype;
               std::string identifier_segment_valuetype_format;
               long identifier_segment_dimension(0);
               long identifier_segment_cardinality(0);
               std::string identifier_segment_data;
               std::string identifier_segment_object_dimension;

               std::getline(istr, input_string);
               ostr_log << "######  segment level: " << input_string << std::endl;
               if (input_string.size() == 0)
               {
                  std::getline(istr, input_string);
                  ostr_log << "#### segment level2: " << input_string << std::endl;

                  // [info][imperative][hack] the end of the file is reached here
                  if (input_string.size() == 0)
                  {
                     break;
                  }

               }

               if( boost::xpressive::regex_match( input_string, what, rx_type ) )
               {
                  ostr_log << "Type: " ;
                  ostr_log << what[1] << std::endl;
                  identifier_segment_type = what[1];
               }

               // #####
               //  if type == GSSESegment
               //    exit this part and start again with the next segment
               // ####
               if (identifier_segment_type == IO::gsse_segment)
               {
                  ostr_log << "break here.. id: " << identifier_segment_type << std::endl;

                  // [info][imperative][hack][required state]::  input_string is already filled with correct space-level string
                  //
                  break; // break the  while(1) loop
               }

               if( boost::xpressive::regex_match( input_string, what, rx_valuetype ) )
               {
                  ostr_log << "ValueType: " ;
                  ostr_log << what[1];
                  identifier_segment_valuetype = what[1];
                  if (what.size() > 2)
                  {
                     ostr_log << "  ";
                     ostr_log << what[2];
                     identifier_segment_valuetype_format = what[2];
                  }
                  ostr_log << std::endl;
               }

               if( boost::xpressive::regex_match( input_string, what, rx_dimension ) )
               {
                  ostr_log << "Dimension: " ;
                  ostr_log << what[1] << std::endl;

                  identifier_segment_dimension = boost::lexical_cast<long>( what[1] );
               }

               if( boost::xpressive::regex_match( input_string, what, rx_cardinality ) )
               {
                  ostr_log << "Cardinality: ";
                  ostr_log << what[1] << std::endl;
                  identifier_segment_cardinality = boost::lexical_cast<long>( what[1] );
               }


               if( boost::xpressive::regex_match( input_string, what, rx_objectdim ) )
               {
                  ostr_log << "ObjectDimension: ";
                  ostr_log << what[1] << std::endl;
                  identifier_segment_object_dimension = what[1] ;
               }


               if( boost::xpressive::regex_match( input_string, what, rx_data ) )
               {
                  ostr_log << "Data: " ;
                  ostr_log << what[1] << std::endl;
                  identifier_segment_data = what[1];
               }

               // inside the segment loop
               //   name associativity requires endless loop
               //   can be:
               //      GSSESegment   -> break the loop
               //      CellComplex
               //      QuanComplex
               //



               // #####################
               //    cell complex
               // #####################
               if (identifier_segment_type==IO::gsse_cellcomplex)
               {
                  if (identifier_segment_object_dimension == IO::gsse_cell)
                  {
                     long rd_val = read_cellcomplex_cl(gsse::at_fiber((gsse::at( identifier_space_number )(segments_topology))),
                                                       istr,
                                                       identifier_segment_valuetype,
                                                       identifier_segment_dimension,
                                                       identifier_segment_cardinality,
                                                       identifier_segment_object_dimension);
                     if (rd_val != 0)
                     {
                        std::cout << "## error in read_cellcomplex_cl.. " << std::endl;
//                         exit(-1);
                     }
                  }
                  else if (identifier_segment_object_dimension == IO::gsse_vertex)
                  {
                     long rd_val = read_cellcomplex_vx(gsse::at_fiber((gsse::at( identifier_space_number )(segments_topology))),
                                                       istr,
                                                       identifier_segment_valuetype,
                                                       -1,
                                                       identifier_segment_cardinality,
                                                       identifier_segment_object_dimension);
                     if (rd_val != 0)
                     {
                        std::cout << "## error in read_cellcomplex_vx.. " << std::endl;
//                         exit(-1);
                     }
                  }

               }

               // #####################
               //    quantity complex
               // #####################
               else if (identifier_segment_type==IO::gsse_quancomplex)
               {
                  ostr_log << "######  quantity complex level .." << std::endl;

                  for (long gsse_quan_i = 0; gsse_quan_i < identifier_segment_cardinality; ++gsse_quan_i)
                  {
                     std::string identifier_qcomplex_type;
                     std::string identifier_qcomplex_valuetype;
                     std::string identifier_qcomplex_valuetype_format;
                     long identifier_qcomplex_dimension(0);
                     long identifier_qcomplex_cardinality(0);
                     std::string identifier_qcomplex_name;
                     std::string identifier_qcomplex_data;

                     std::getline(istr, input_string);
                     ostr_log << "#### quan-complex level: " << input_string << std::endl;
                     if (input_string.size() == 0)
                     {
                        std::getline(istr, input_string);
                        ostr_log << "## quan-complex level2: " << input_string << std::endl;
                     }


                     if( boost::xpressive::regex_match( input_string, what, rx_type ) )
                     {
                        ostr_log << "Type: " ;
                        ostr_log << what[1] << std::endl;
                        identifier_qcomplex_type = what[1];
                     }
                     if( boost::xpressive::regex_match( input_string, what, rx_valuetype ) )
                     {
                        ostr_log << "ValueType: " ;
                        ostr_log << what[1];
                        identifier_qcomplex_valuetype = what[1];
                        if (what.size() > 2)
                        {
                           ostr_log << "  ";
                           ostr_log << what[2];
                           identifier_qcomplex_valuetype_format = what[2];
                        }
                        ostr_log << std::endl;
                     }
                     if( boost::xpressive::regex_match( input_string, what, rx_name ) )
                     {
                        ostr_log << "Name: " ;
                        ostr_log << what[1] << std::endl;
                        identifier_qcomplex_name = what[1];
                     }

                     if( boost::xpressive::regex_match( input_string, what, rx_dimension ) )
                     {
                        ostr_log << "Dimension: " ;
                        ostr_log << what[1] << std::endl;

                        identifier_qcomplex_dimension = boost::lexical_cast<long>( what[1] );
                     }

                     if( boost::xpressive::regex_match( input_string, what, rx_cardinality ) )
                     {
                        ostr_log << "Cardinality: ";
                        ostr_log << what[1] << std::endl;
                        identifier_qcomplex_cardinality = boost::lexical_cast<long>( what[1] );
                     }

                     if( boost::xpressive::regex_match( input_string, what, rx_data ) )
                     {
                        ostr_log << "Data: " ;
                        ostr_log << what[1] << std::endl;
                        identifier_qcomplex_data = what[1];
                     }


                     if (identifier_qcomplex_type=="Quantity")
                     {
                        long rd_val = read_quantity(gsse::at_fiber((gsse::at( identifier_space_number )(segments_quantity))),
                                                    istr,
                                                    identifier_segment_valuetype.substr(0, identifier_segment_valuetype.find(" ")),
                                                    identifier_segment_valuetype,
                                                    identifier_qcomplex_name,
                                                    -1,
                                                    identifier_qcomplex_cardinality);
                        if (rd_val != 0)
                        {
                           std::cout << "## error in read_quantity.. " << std::endl;
//                            exit(-1);
                        }

                     }
                  }  // quantity loop
               }  // QuantityComplex
            }  //  while(1) loop
         }  // GSSESegment for-loop
      }  // PhysicalSpace
      else
      {
         // [RH][TODO]
         // ### are there other possibilities ????
      }

   } // read in loop
}




// #############################################################################################################
//   gssev02 :: full domain writer
// #############################################################################################################
//
template<typename Domain>
void write(Domain& domain, std::ostream& ostr = std::cout)
{
   using namespace gsse::access_specifier;   // AC, AQ, ...
   using namespace boost::phoenix;
   using namespace boost::phoenix::local_names;

   typedef gsse::property_domain<Domain>  DomainProperty;
   static const size_t DIMG = gsse::result_of::property_DIMG<DomainProperty>::value;
   static const size_t DIMT = gsse::result_of::property_DIMT<DomainProperty>::value;

   ostr << gsse::IO::type;
   ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
   ostr << gsse::IO::gsse_coordinates;
   ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

   ostr << gsse::IO::val_type;
   ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
   ostr << gsse::IO::gsse_numeric_coord;
   ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

   ostr << gsse::IO::dimension;
   ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
   ostr << DIMG;
   ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

   ostr << gsse::IO::cardinality;
   ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
   ostr << gsse::at_dim<AP>(domain).size();
   ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

   ostr << gsse::IO::data_follows;
   ostr << std::endl;


   ostr.setf( std::ios::right,std::ios::adjustfield);
   ostr << std::setprecision(12)  << std::setiosflags(std::ios::fixed);

   // ## coordinates
   //
   ostr << gsse::IO::set_open << std::endl;
   gsse::traverse<AP>()
   [
      ostr << val(gsse::IO::sep) << val(gsse::IO::set_open) << val(gsse::IO::sep)
      ,
      gsse::traverse()
      [
         ostr  << boost::phoenix::arg_names::_1  << val(gsse::IO::sep)
      ]
      ,
      ostr << val(gsse::IO::set_close) << std::endl
   ](domain);
   ostr << gsse::IO::set_close << std::endl;


   // #############################################################

   ostr << gsse::IO::type;
   ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
   ostr << gsse::IO::gsse_space;
   ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

   ostr << gsse::IO::val_type;
   ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
   ostr << gsse::IO::gsse_segment;
   ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

   ostr << gsse::IO::cardinality;
   ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
   ostr << gsse::at_dim<AC>(domain).size();
   ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

   ostr << gsse::IO::data_collect_parts;
   ostr << std::endl;

   // ########################
   // ## cell complex
   //
   for (size_t cnt_seg = 0; cnt_seg < gsse::size( gsse::at_dim<AC>(domain) ); ++cnt_seg)
   {
      ostr << gsse::IO::type;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << gsse::IO::gsse_segment;
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::number;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << cnt_seg ;
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::name;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr <<  gsse::at_index(gsse::at(cnt_seg)(gsse::at_dim<AC>(domain)));
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::data_part;
      ostr << std::endl;


      // ######################
      //  cell complex: cell
      //
      ostr << gsse::IO::type;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << gsse::IO::gsse_cellcomplex;
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::val_type;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << gsse::IO::gsse_int;
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::dimension;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << DIMT;        //just triangles (2-cell/2-simplex) (number of elements in storage set)
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::cardinality;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(cnt_seg)(gsse::at_dim<AC>(domain)))).size();
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::object_dimension;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << gsse::IO::gsse_cell;
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::object_type;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << gsse::IO::gsse_simplex;
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::storage_format;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
// [RH][wrong topological information]
      ostr << gsse::IO::gsse_vxcl;   // vertex ON cell
//      ostr << gsse::IO::gsse_clvx;   // cell ON vertex
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::gsse_indexed;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << "1";    //number of index elements
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::data_follows;
      ostr << std::endl;

      // #### cell: topological space
      //
      ostr << gsse::IO::set_open;
      ostr << std::endl;
      long cnt_cl=0;
      gsse::traverse<AT_cl>()
      [
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_open) << val(gsse::IO::sep)
         ,
         boost::ref(cnt_cl)++
         ,
         ostr << val(cnt_cl) << val(" \t")
         ,
         gsse::traverse()
         [
            ostr  << boost::phoenix::arg_names::_1 << val(gsse::IO::sep)
         ]
         ,
         ostr << val(gsse::IO::set_close) << std::endl
      ](gsse::at_fiber(gsse::at(cnt_seg)(gsse::at_dim<AC>(domain))));
      ostr << gsse::IO::set_close;
      ostr << std::endl;

      // ######################
      //  cell complex: vertex
      //
      ostr << gsse::IO::type;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << gsse::IO::gsse_cellcomplex;
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::val_type;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << gsse::IO::gsse_int;
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      // no dimensions -> means, cardinality has to be given
//       ostr << gsse::IO::dimension;
//       ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
//       ostr << 3;
//       ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::cardinality;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(cnt_seg)(gsse::at_dim<AC>(domain)))).size();
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::object_dimension;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << gsse::IO::gsse_vertex;
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::object_type;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << gsse::IO::gsse_simplex;
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::storage_format;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
// [RH][new information]
// check reader
      ostr << gsse::IO::gsse_clvx;   // cell ON vertex
//      ostr << gsse::IO::gsse_vxcl;   // vertex ON cell
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::gsse_indexed;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << "1";    //number of index elements
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::data_follows;
      ostr << std::endl;

      // #### cell: topological space
      //
      ostr << gsse::IO::set_open;
      ostr << std::endl;
      cnt_cl=0;
      gsse::traverse<AT_vx>()
      [
         ostr << val(gsse::IO::sep),
         ostr << val(gsse::IO::cardinality),
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_open)  << val(gsse::IO::sep),
         ostr << gsse::asize1,
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_close) << val(gsse::IO::sep)
         ,
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_open) << val(gsse::IO::sep),
         boost::ref(cnt_cl)++
         ,
         ostr << val(cnt_cl) << val(" \t")
         ,
         gsse::traverse()
         [
            ostr  << boost::phoenix::arg_names::_1 << val(gsse::IO::sep)
         ]
         ,
         ostr << val(gsse::IO::set_close) << std::endl
      ](gsse::at_fiber(gsse::at(cnt_seg)(gsse::at_dim<AC>(domain))));
      ostr << gsse::IO::set_close;
      ostr << std::endl;


      // ######################


      ostr << gsse::IO::type;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << gsse::IO::gsse_quancomplex;
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::val_type;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << "Vertex Indexed";
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::cardinality;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(cnt_seg)(gsse::at_dim<AQ>(domain)))).size();
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::data_part;
      ostr << std::endl;

      // #### quantity: physical space
      //
      gsse::traverse<AT_vx>()
      [
         ostr << val(gsse::IO::type),
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_open)  << val(gsse::IO::sep),
         ostr << val("Quantity"),
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_close) << val(gsse::IO::sep),

         ostr << val(gsse::IO::val_type) ,
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_open)  << val(gsse::IO::sep),
         ostr << val(gsse::IO::gsse_numeric_quan),
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_close) << val(gsse::IO::sep),

         ostr << val(gsse::IO::name),
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_open)  << val(gsse::IO::sep),
         ostr << gsse::acc0,    // ## quantity name
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_close) << val(gsse::IO::sep),

         ostr << val(gsse::IO::cardinality),
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_open)  << val(gsse::IO::sep),
         ostr << gsse::asize1,
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_close) << val(gsse::IO::sep),

         ostr << val(gsse::IO::data_follows),
         ostr << val("") << std::endl
         ,
         ostr << val(gsse::IO::set_open),
         ostr << val("") << std::endl
         ,
         gsse::traverse()
         [
            ostr << val(gsse::IO::sep),
            ostr << val(gsse::IO::cardinality),
            ostr << val(gsse::IO::sep) << val(gsse::IO::set_open)  << val(gsse::IO::sep),
            ostr << gsse::asize1,
            ostr << val(gsse::IO::sep) << val(gsse::IO::set_close) << val(gsse::IO::sep),

            ostr << val(gsse::IO::sep) << val(gsse::IO::set_open) << val(gsse::IO::sep)
            ,
            ostr << gsse::acc0  << val("\t  ")
            ,
            gsse::traverse() // for multiple values
            [
               ostr << gsse::acc << val("  ")
            ]
            ,
            ostr << val(gsse::IO::set_close) << std::endl
         ]
         ,
         ostr << val(gsse::IO::set_close),
         ostr << val("") << std::endl
      ](gsse::at_fiber(gsse::at(cnt_seg)(gsse::at_dim<AQ>(domain))));



// ################################


      ostr << gsse::IO::type;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << gsse::IO::gsse_quancomplex;
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::val_type;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << "Cell Indexed";
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::cardinality;
      ostr << gsse::IO::sep << gsse::IO::set_open << gsse::IO::sep;
      ostr << gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(cnt_seg)(gsse::at_dim<AQ>(domain)))).size();
      ostr << gsse::IO::sep << gsse::IO::set_close << gsse::IO::sep;

      ostr << gsse::IO::data_part;
      ostr << std::endl;

      // #### quantity: physical space
      //
      gsse::traverse<AT_cl>()
      [
         ostr << val(gsse::IO::type),
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_open)  << val(gsse::IO::sep),
         ostr << val("Quantity"),
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_close) << val(gsse::IO::sep),

         ostr << val(gsse::IO::val_type) ,
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_open)  << val(gsse::IO::sep),
         ostr << val(gsse::IO::gsse_numeric_quan),
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_close) << val(gsse::IO::sep),

         ostr << val(gsse::IO::name),
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_open)  << val(gsse::IO::sep),
         ostr << gsse::acc0,    // ## quantity name
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_close) << val(gsse::IO::sep),

         ostr << val(gsse::IO::cardinality),
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_open)  << val(gsse::IO::sep),
         ostr << gsse::asize1,
         ostr << val(gsse::IO::sep) << val(gsse::IO::set_close) << val(gsse::IO::sep),

         ostr << val(gsse::IO::data_follows),
         ostr << val("") << std::endl
         ,
         ostr << val(gsse::IO::set_open),
         ostr << val("") << std::endl
         ,
         gsse::traverse()
         [
            ostr << val(gsse::IO::sep),
            ostr << val(gsse::IO::cardinality),
            ostr << val(gsse::IO::sep) << val(gsse::IO::set_open)  << val(gsse::IO::sep),
            ostr << gsse::asize1,
            ostr << val(gsse::IO::sep) << val(gsse::IO::set_close) << val(gsse::IO::sep),

            ostr << val(gsse::IO::sep) << val(gsse::IO::set_open) << val(gsse::IO::sep)
            ,
            ostr << gsse::acc0 << val("\t  ")
            ,
            gsse::traverse() // for multiple values
            [
               ostr << boost::phoenix::arg_names::_1 << val("  ")
            ]
            ,
            ostr << val(gsse::IO::set_close) << std::endl
         ]
         ,
         ostr << val(gsse::IO::set_close),
         ostr << val("") << std::endl
      ](gsse::at_fiber(gsse::at(cnt_seg)(gsse::at_dim<AQ>(domain))));
   }
}



//
// ===================================================================================
//
template<typename DomainT>
void move_domain(DomainT & domain_source,
                 DomainT & domain_target)
{
   using namespace gsse::access_specifier;

   typedef gsse::property_domain<DomainT>                                                 GSSE_Domain;
   typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain>::type   SpaceTopologySegmentsT;
   typedef typename gsse::result_of::property_FBPosT<GSSE_Domain>::type                   FBPosT;

   SpaceTopologySegmentsT& segments_topology_source = gsse::at_dim<AC>(domain_source);
   FBPosT&                 geometry_source          = gsse::at_dim<AP>(domain_source);

   SpaceTopologySegmentsT& segments_topology_target = gsse::at_dim<AC>(domain_target);
   FBPosT&                 geometry_target          = gsse::at_dim<AP>(domain_target);
   //
   // RESET TARGET
   //
   reset_domain(domain_target);
   //
   // TRANSFER GEOMETRY
   //
   for(long pi = 0; pi < gsse::size(gsse::at_dim<AP>(domain_source)); ++pi)
   {
      gsse::pushback(geometry_target) = gsse::at(pi)(geometry_source);
   }
   //
   // TRANSFER TOPOLOGY
   //
   gsse::resize(gsse::size(segments_topology_source))(segments_topology_target);
   for (long si = 0; si < gsse::size(segments_topology_source); ++si)
   {
      gsse::at_index(gsse::at(si)(segments_topology_target)) = gsse::at_index(gsse::at(si)(segments_topology_source));

      for ( long ci = 0; ci < gsse::size_cl(gsse::at(si)(segments_topology_source)); ++ci)
      {
         gsse::pushback(gsse::at_dim<AT_cl>(
          gsse::at_fiber(gsse::at(si)(segments_topology_target)))) =
            gsse::at_cl(ci)(gsse::at(si)(segments_topology_source));
      }
   }
   //
   // COMPLETE TARGET TOPOLOGY
   //
   complete_topology(domain_target);
   //
   // RESET SOURCE
   //
   reset_domain(domain_source);
}
//
// ===================================================================================
//
template<typename DomainT>
void reset_domain(DomainT& domain)
{
   using namespace gsse::access_specifier;

   typedef gsse::property_domain<DomainT>                                                 GSSE_Domain;
   typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain>::type   SpaceTopologySegmentsT;

   SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(domain);
   //
   // RESET GEOMETRY
   //
   gsse::at_dim<AP>(domain).clear();
   //
   // RESET TOPOLOGY
   //
   for (long si = 0; si < gsse::size(segments_topology); ++si)
   {
      gsse::at_dim<AT_vx>(gsse::at_fiber((gsse::at(si)(segments_topology)))).clear();
      gsse::at_dim<AT_ee>(gsse::at_fiber((gsse::at(si)(segments_topology)))).clear();
      gsse::at_dim<AT_cl>(gsse::at_fiber((gsse::at(si)(segments_topology)))).clear();
   }
}
//
// ===================================================================================
//

template<typename Domain>
void create_faces(Domain& domain,
                  typename boost::enable_if<
                     typename boost::mpl::equal_to<
                        boost::mpl::int_< gsse::result_of::at_dim_index<
                           typename gsse::result_of::at_dim<Domain, gsse::access_specifier::AC>::type>::type::DIM_cc>,
                        boost::mpl::int_<3>
                     >::type
                  >::type* dummy = 0)
{

   using namespace gsse::access_specifier;

   typedef typename gsse::result_of::at_dim<Domain, AC>::type                       SpaceTopologySegmentsT;
   typedef typename gsse::result_of::at_dim_index<SpaceTopologySegmentsT>::type     CellComplex;
   typedef typename CellComplex::CellT                                              CellTopology;

   static const long DIM = CellComplex::DIM_cc;

   SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(domain);

   typedef gsse::property_data<DIM, CellTopology>                                   GSSE_Data;
   typedef typename gsse::result_of::facet_on_cell<GSSE_Data>::type                 FacetOnCell;
   typedef typename gsse::result_of::facet_on_cell<GSSE_Data>::result               FacetOnCell_result;
   typedef typename gsse::result_of::facet_on_cell<GSSE_Data>::result::value_type   FacetT;
   FacetOnCell facet_on_cell;

   typedef std::set< FacetT >             container_facets_t;

   for (size_t si = 0; si < gsse::size (segments_topology ); ++si)
   {

      container_facets_t                     container_facets;
      typename container_facets_t::iterator  ci_it;

      for ( long ci = 0; ci < gsse::size_cl( gsse::at(si) (segments_topology) ); ++ci)
      {
          FacetOnCell_result cont_f0 =
            facet_on_cell ( gsse::at(ci) (gsse::at_dim<AT_cl>( gsse::at_fiber((gsse::at(si) (segments_topology)))) ) );

         for (size_t cf_i = 0; cf_i < gsse::size(cont_f0); ++cf_i)
            container_facets.insert ( cont_f0[cf_i] );
      }

      for (ci_it = container_facets.begin();
           ci_it != container_facets.end();
           ++ci_it)
      {
         gsse::pushback ( gsse::at_dim<AT_f0>( gsse::at_fiber((gsse::at(si) (segments_topology))))) = (*ci_it);
      }
   }

}

template<typename Domain>
void create_faces(Domain& domain,
                  typename boost::enable_if<
                     typename boost::mpl::less<
                        boost::mpl::int_< gsse::result_of::at_dim_index<
                           typename gsse::result_of::at_dim<Domain, gsse::access_specifier::AC>::type>::type::DIM_cc>,
                        boost::mpl::int_<3>
                     >::type
                  >::type* dummy = 0)
{ }

//
// ===================================================================================
//
//
// ===================================================================================
//
template <typename DomainT>
void complete_topology(DomainT& domain)
{
   using namespace gsse::access_specifier;

   typedef gsse::property_domain<DomainT>                                                             GSSE_Domain;
   typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain>::type               SpaceTopologySegmentsT;
   typedef gsse::property_space<DomainT>                                                              GSSE_Space;
   typedef typename gsse::result_of::property_CellComplex<GSSE_Space>::type                           CellComplex;
   typedef typename gsse::result_of::at_dim_isd<CellComplex, gsse::access_specifier::AT_cl>::type     CellT;
   typedef typename gsse::result_of::at_dim_isd<CellComplex, gsse::access_specifier::AT_vx>::type     VertexT;

   SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(domain);

   long   segment_size = segments_topology.size();
   // --------------------------------------------
   for (long si = 0; si < segment_size; ++si)
   {
      //
      // RESET VERTEX && EDGE CONTAINER
      //
      //   brief: in case complete_topology() has been called on an previously completed domain
      //          we do not want this function to append on the previous filled container - we want
      //          to create the containers from scratch to ensure consistancy ..
      //
      if(gsse::size(gsse::at_dim<AT_vx>(gsse::at_fiber((gsse::at(si)(segments_topology))))) > 0)
         gsse::at_dim<AT_vx>(gsse::at_fiber((gsse::at(si)(segments_topology)))).clear();
      if(gsse::size(gsse::at_dim<AT_ee>(gsse::at_fiber((gsse::at(si)(segments_topology))))) > 0)
         gsse::at_dim<AT_ee>(gsse::at_fiber((gsse::at(si)(segments_topology)))).clear();
      //
      // vertex_con_t --> to impose uniqueness on the segment vertices
      //
      typedef std::map<long, bool>      vertex_con_t;
      //
      // vertex_map_t --> to bridge between the dense vertex container elements
      //                  and the sparse real vertex indices
      //
      typedef std::map<long, long>    vertex_map_t;
      vertex_con_t      vertex_con;
      vertex_map_t      vertex_map;

      long cellsize = gsse::size_cl( gsse::at(si) (segments_topology) );

      //
      // SETUP UNIQUE VERTEX CONTAINER
      //
      for (long ci = 0 ; ci < cellsize; ++ci)
      {
         CellT cell = gsse::at(ci) (gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology)))   );

         for (unsigned int voci = 0; voci < cell.size(); ++voci)
         {
            if (!vertex_con[ cell[voci] ])
               vertex_con[ cell[voci] ] = true;
         }
      }
      //
      // RESIZE VERTEX CONTAINER OF THE ACTUAL SEGMENT ACCORDING TO THE UNIQUE VERTEX CONTAINER
      //
      gsse::resize(gsse::size(vertex_con))(gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(si)(segments_topology))));
      //
      // SETUP TRANSFER UNIQUE VERTEX DATASET TO THE SEGMENT VERTEX CONTAINER
      //
      long   vindex = 0;
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
      for (long ci = 0 ; ci < cellsize; ++ci)
      {
         CellT cell = gsse::at(ci) (gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology)))   );
         for (unsigned int voci = 0; voci < cell.size(); ++voci)
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
         }
      }
   }

   //
   // CREATE EDGES
   //
   gsse::create_edges(domain);
   //
   // CREATE FACES
   //
   create_faces(domain);

   for (long si = 0; si < segment_size; ++si)
   {
   #ifdef DEBUG_TOOLBOX
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
   #endif
   }

}

// #############################################################################################################
//   gssev02 :: dump the domain informations on the stream
// #############################################################################################################
template<typename DomainT>
void domain_statistics(DomainT& domain, std::ostream& ostr = std::cout)
{
   using namespace gsse::access_specifier;

   typedef gsse::property_domain<DomainT>                                                    DomainProperty;
   typedef typename gsse::result_of::property_SpaceTopologySegmentsT<DomainProperty>::type   SpaceTopologySegments;
   typedef typename gsse::result_of::property_SpaceQuantitySegmentsT<DomainProperty>::type   SpaceQuantitySegments;
   typedef typename gsse::result_of::property_FBPosT<DomainProperty>::type                   FBPos;

   SpaceTopologySegments&  segments_topology = gsse::at_dim< AC >(domain);
   SpaceQuantitySegments&  segments_quantity = gsse::at_dim< AQ >(domain);
   FBPos&                  geometry          = gsse::at_dim< AP >(domain);

   ostr << std::endl;
   ostr << "----------------------------------------------------" << std::endl;
   ostr << "# domain statistics .. " << std::endl;
   ostr << "  geometry size:    " << gsse::size( geometry ) << std::endl;
   ostr << "  segment size:     " << gsse::size( segments_topology ) << std::endl;
   for( size_t si = 0; si < gsse::size( segments_topology ); si++ )
   {
      ostr << "    cell size:        " <<
         gsse::size( gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology))) ) << std::endl;
      ostr << "    edge size:        " <<
         gsse::size( gsse::at_dim<AT_ee>(gsse::at_fiber(gsse::at(si)(segments_topology))) ) << std::endl;
      ostr << "    vertex size:      " <<
         gsse::size( gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(si)(segments_topology))) ) << std::endl;
      ostr << "    cell quan size:   " <<
         gsse::size( gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))) ) << std::endl;
      ostr << "    edge quan size:   " <<
         gsse::size( gsse::at_dim<AT_ee>(gsse::at_fiber(gsse::at(si)(segments_quantity))) ) << std::endl;
      ostr << "    vertex quan size: " <<
         gsse::size( gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(si)(segments_quantity))) ) << std::endl;
   }
   ostr << "----------------------------------------------------" << std::endl;
   ostr << std::endl;
}


// #############################################################################################################
//   gssev02 :: inter-dimensional object creation algorithms
// #############################################################################################################
//
// template<typename Domain>
// void create_edges(Domain& domain)
// {
//    using namespace gsse::access_specifier;   // AC, AQ, ...


//    typedef typename gsse::result_of::at_dim<Domain, AC>::type                    SpaceTopologySegmentsT;
//    typedef typename gsse::result_of::at_dim_index<SpaceTopologySegmentsT>::type  CellComplex;

//    typedef typename CellComplex::CellT                                 CellTopology;
//    typedef typename gsse::result_of::at_dim<CellComplex, AT_vx>::type  ContainerCLonVX;
//    typedef typename gsse::result_of::at_dim<CellComplex, AT_cl>::type  ContainerVXonCL;

//    static const long DIM = CellComplex::DIM_cc;

// // -----------------------
//    SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(domain);


//    // boundary operator
//    //
//    //    DIM-1 .. calculate relative edge dimensions
//    //
//    typedef gsse::boundary<DIM, DIM-1 , gsse::cell_simplex> Bnd_sD_1;
//    Bnd_sD_1 bnd_sD_1;
//    typedef typename gsse::result_of::boundary<DIM, DIM-1, CellTopology>::type::value_type  EdgeTypes;


// // // ##########

//    for (long cnt_seg = 0; cnt_seg < gsse::size (segments_topology ); ++cnt_seg)
//    {
//       std::set< EdgeTypes >                    container_edges;
//       typename std::set< EdgeTypes >::iterator ci_it;

//       for ( long cnt_cell = 0; cnt_cell < gsse::size(gsse::at_dim<AT_cl>( gsse::at_fiber((gsse::at(cnt_seg) (segments_topology))))); ++cnt_cell)
//       {
//          typename gsse::result_of::boundary<DIM, DIM-1, CellTopology>::type
//             cont_result = bnd_sD_1( gsse::at(cnt_cell) (gsse::at_dim<AT_cl>( gsse::at_fiber((gsse::at(cnt_seg) (segments_topology)))) ));


//          for (long ce_i = 0; ce_i < gsse::size(cont_result); ++ce_i)
//             container_edges.insert ( cont_result[ce_i] );
//       }

//       for (ci_it = container_edges.begin();
//            ci_it != container_edges.end();
//            ++ci_it)
//       {
//          gsse::pushback (gsse::at_dim<AT_ee>( gsse::at_fiber((gsse::at(cnt_seg) (segments_topology))))) = (*ci_it);
//       }
// //       std::cout << "# all edges: " << std::endl;
// //       gsse::print_1d( gsse::at_dim<AT_ee>( gsse::at_fiber((gsse::at(cnt_seg) (segments_topology)))) );   // [TODO] for sets
//     }
// }



} //namespace gsse

#endif
