/* =======================================================================

                     ---------------------------------------
                     ViennaMesh - The Vienna Meshing Library
                     ---------------------------------------
                            
   maintainers:    
               Josef Weinbub                      weinbub@iue.tuwien.ac.at
               Johann Cervenka                   cervenka@iue.tuwien.ac.at

   developers:
               Franz Stimpfl, Rene Heinzl, Philipp Schwaha

   license:    
               LGPL, see file LICENSE in the ViennaMesh base directory

   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at

   ======================================================================= */

// *** system includes
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

// *** BOOST includes
#include <boost/array.hpp>

// *** GSSE includes
#include "gsse/datastructure/fiberbundle.hpp"
#include "gsse/util/specifiers.hpp"   
#include "gsse/traversal.hpp"
#include "gsse/util.hpp"


// ############################################################
//
namespace IO_tokens
{
   std::string open_sequence ("{");
   std::string close_sequence("}");
   std::string set_open ("[");
   std::string set_close("]");
   std::string delimiter_space("\t");
}

namespace DFISE_tokens
{
   std::string name_0cell ("Vertices");
   std::string name_1cell ("Edges");
   std::string name_2cell ("Faces");
   std::string name_3cell ("Elements");
   std::string name_element("10");
   std::map<long, std::string> name_container;
}

namespace HIN_tokens
{
   std::string name_0cell    ("Vertices");
   std::string name_2cell    ("Polygons");
   std::string name_2cell_ll ("Polygon");
   std::string name_3cell    ("Polyhedra");
   std::string name_3cell_ll ("Polyhedron");
   std::string name_material ("Material");

   std::map<long, std::string> name_container;
}

using namespace boost::phoenix;
using namespace boost::phoenix::arg_names;
using namespace IO_tokens;
//namespace fus = boost::fusion;

// ############################################################


template<typename DIM_local, typename FiberBundle>
void read_in_positions(FiberBundle& fiber_bundle, std::istream& filein, std::string cell_name)
{
//   std::cout << "  .. read in positions.. cell name: " << cell_name << std::endl;

   typedef typename gsse::result_of::at_dim<FiberBundle, DIM_local>::type::value_type FB_pos;
   std::string tempinput;
   long        cell_size;

   filein >> tempinput;
   while (tempinput != cell_name) 
   {
      filein >> tempinput;
   }
   filein >> tempinput;   // (
   filein >> cell_size;
   filein >> tempinput;   // )
   
   filein >> tempinput;   // {
   gsse::resize(cell_size)(gsse::at_dim<DIM_local>(fiber_bundle));
//   std::cout << "     .. cell size: " << cell_size << std::endl;


   for (long i =0; i < cell_size ; ++i)
   {
      FB_pos positions;   // coordinates (fb context)
      for (size_t sc = 0; sc<positions.size(); ++sc)
      {
         positions[sc] = 0;
      }
      filein >> positions[0];
      filein >> positions[1];
      filein >> positions[2];

      gsse::at(i)(gsse::at_dim<DIM_local>(fiber_bundle)) = positions;  
   }
}

template<typename DIM_local, typename FiberBundle>
void read_in_connectivity(FiberBundle& fiber_bundle, std::istream& filein, std::string cell_name)
{
//   std::cout << "  .. read in connectivity.. cell_name: "<< cell_name  << std::endl;

   std::string tempinput;
   long        cell_size;

   // navigate to current read-in token given by cell_name
   //
   filein >> tempinput;
   while (tempinput != cell_name) 
   {
      filein >> tempinput;
   }
   filein >> tempinput;   // (
   filein >> cell_size;
   filein >> tempinput;   // )
   
   filein >> tempinput;   // {
   gsse::resize(cell_size)(gsse::at_dim<DIM_local>(fiber_bundle));

//   std::cout << "     .. cell size: " << cell_size << std::endl;
   
   for (long i =0; i < cell_size ; ++i)
   {
      long number_elements;

      if (cell_name == DFISE_tokens::name_1cell)
      {
         number_elements = 2;
      }
      else if (cell_name == DFISE_tokens::name_2cell)
      {
         filein >> number_elements;
      }
      else if (cell_name == DFISE_tokens::name_3cell)
      {
         std::string DFISE_element_code;
         filein >> DFISE_element_code;   // should be 10 ..
         if (DFISE_element_code != DFISE_tokens::name_element)
         {
            std::cout << "##### error.. elements are only supported for faces for now  !!! .. exit ! ### " << std::endl;
            exit(-1);
         }
         filein >> number_elements;
      }

//      std::cout << "    .. number of elements: "<< number_elements << std::endl;

      for (long i2 = 0; i2 < number_elements; ++i2)
      {
         long cell_index;
         filein >> cell_index;


         if (cell_name == DFISE_tokens::name_3cell)
         {
            // [INFO]
            // [DFISE] file format specific part
            //   facet index < 0 -> -facet_index - 1
            //
            // [HIN]
            //   requires element index starting from 1
            //
            // here we use both calculations together
            //   < 0: should be incremented, but from DFISE has to be decremented
            //   > 0: has to be incremented 
            //
            if (cell_index >= 0)
            {
               cell_index += 1;
            }
         }

         gsse::at(i2)(gsse::at(0)(gsse::at(i)(gsse::at_dim<DIM_local>(fiber_bundle))))= cell_index;
      }
   }
}



// #################################################################################################
// ### HIN output
//
template<typename DIM_local, typename FiberBundle>
void output_0cell_pos(FiberBundle& fiber_bundle, std::ostream& output=std::cout)
{
   output << HIN_tokens::name_container[0] << set_open  << gsse::at_dim<DIM_local>(fiber_bundle).size() << set_close;
   output << open_sequence << std::endl;

   gsse::traverse() 
   [ 
      gsse::traverse() 
      [ 
//         output << gsse::access(arg1),
         output << _1,
         output << val(delimiter_space)
      ]
      ,
      output << val("") << std::endl
   ](  gsse::at_dim<DIM_local>(fiber_bundle) );

   output << close_sequence << std::endl;

}


template<typename DIM_local, typename FiberBundle>
void output_2cell(FiberBundle& fiber_bundle, std::ostream& output=std::cout)
{
//   gsse::at_dim<DIM_local>(fiber_bundle)

   output << HIN_tokens::name_container[2] << set_open <<   gsse::at_dim<DIM_local>(fiber_bundle).size() << set_close;
   output<< open_sequence << std::endl;

   long cnt         = 0;
   long section_cnt = 0;
   gsse::traverse() 
   [ 
      output << val(HIN_tokens::name_2cell_ll) 
      ,
      ref(section_cnt) = 0,
      gsse::traverse() // more polygon information, the polygon fiber
      [ 
         if_(ref(section_cnt) == 1)
         [
            ref(cnt) = 0,
            gsse::traverse() 
            [ 
               ++ref(cnt)
            ]
            ,
            output << val(set_open),
            output << ref(cnt),
            output << val(set_close),
            output << val(open_sequence),
            output << val("") << std::endl,
            gsse::traverse() 
            [ 
//               output << gsse::access(arg1),
               output << _1,
               output << val(" ")
            ]
            ,
            output << val("") << std::endl,
            output << val(close_sequence) << std::endl
         ]
         ,
         ref(section_cnt) ++
         
      ]        
   ](  gsse::at_dim<DIM_local>(fiber_bundle) );
   output << close_sequence << std::endl;
}


template<typename DIM_local, typename FiberBundle>
void output_3cell(FiberBundle& fiber_bundle, std::ostream& output=std::cout)
{
   output << HIN_tokens::name_container[3]<< set_open <<  gsse::at_dim<DIM_local>(fiber_bundle).size() << set_close;
   output<< open_sequence << std::endl;
   
   long cnt = 0;
   gsse::traverse() 
   [ 
      output << val(HIN_tokens::name_3cell_ll)   // HIN extra information
      ,
      gsse::traverse() // more segments information, the segments fiber
      [ 
         ref(cnt) = 0,
         gsse::traverse() 
         [ 
            ++ref(cnt)
         ]
         ,
         output << val(set_open),
         output << ref(cnt),
         output << val(set_close),
         output << val(open_sequence),
         output << val("") << std::endl,
         gsse::traverse() 
         [ 
//            if_( gsse::access(arg1) > 0)  // HIN - DFISE conversion (DFISE uses different default orientation than HIN
            if_( _1 > 0)  // HIN - DFISE conversion (DFISE uses different default orientation than HIN
            [
//               output  << val("-") << gsse::access(arg1) << val(" ")
               output  << val("-") << _1 << val(" ")
            ]
            .else_
            [
//               output  << gsse::access(arg1) << val(" ")
               output  << _1 << val(" ")
            ]
         ]
         ,
         output << val("") << std::endl,
         output << val(close_sequence) << std::endl
      ]        
   ](  gsse::at_dim<DIM_local>(fiber_bundle) );
   output << close_sequence << std::endl;


   // ## extrahin .. material section
   output << HIN_tokens::name_material << set_open <<   gsse::at_dim<DIM_local>(fiber_bundle).size() << set_close;
   output<< open_sequence << std::endl;
   for (size_t i = 0; i < gsse::at_dim<DIM_local>(fiber_bundle).size(); ++i)
   {
      output << i << " ";
   }
   output << std::endl << close_sequence << std::endl;

}

// ############################################################


template<typename FBCollection>
void reader(FBCollection& fb_collection, std::string filename)
{
//   std::cout << "### read in structure from file: " << filename << std::endl;

   std::ifstream filein(filename.c_str());

   read_in_positions<gsse::access_specifier::AP_0>   (fb_collection, filein, DFISE_tokens::name_container[0]);

   read_in_connectivity<gsse::access_specifier::AT_1>(fb_collection, filein, DFISE_tokens::name_container[1]);
   read_in_connectivity<gsse::access_specifier::AT_2>(fb_collection, filein, DFISE_tokens::name_container[2]);
   read_in_connectivity<gsse::access_specifier::AT_3>(fb_collection, filein, DFISE_tokens::name_container[3]);
}



template<typename FBCollection>
void writer(FBCollection& fb_collection, std::string filename)
{
//   std::cout << "### write structure to file: " << filename << std::endl;

   std::ofstream fileout(filename.c_str());

   if (filename == "")   // default points to std::cout
   {
      output_0cell_pos<gsse::access_specifier::AP_0>(fb_collection);
      output_2cell<gsse::access_specifier::AT_2>    (fb_collection);
      output_3cell<gsse::access_specifier::AT_3>    (fb_collection);
   }
   else
   {
      output_0cell_pos<gsse::access_specifier::AP_0>(fb_collection, fileout);
      output_2cell<gsse::access_specifier::AT_2>    (fb_collection, fileout);
      output_3cell<gsse::access_specifier::AT_3>    (fb_collection, fileout);
   }

   fileout.close();
}



// ############################################################


// [INFO]  edge-2-facet index transformation
//  DFISE: facet -> edge  (where: edge -> vertex)
//  -> [HIN] requires facet-vertex indices
//
template<typename FiberBundle1,
         typename FiberBundle2>
void calculate_2cell_from_1cell(FiberBundle1& fiberbundle_ee, FiberBundle2& fiberbundle_ft)
{
//   std::cout << "### calculate 2cell from 1cell " << std::endl;

   long fb_section_ee    = 0;
   long fb_section_facet = 1;

   for (size_t cnt_fb_in = 0; cnt_fb_in < fiberbundle_ft.size(); ++cnt_fb_in)
   {
      for (size_t cnt_faces = 0;  cnt_faces < fiberbundle_ft[cnt_fb_in][fb_section_ee].size(); ++cnt_faces)
      {
         long index_facet = fiberbundle_ft[cnt_fb_in][fb_section_ee][cnt_faces];

         long sign_access = 1;
         if (index_facet < 0)   // [info][index transformation] from DFISE file format indices
         {
            sign_access  =  0;
            index_facet  = -1*index_facet -1;
         }
         long index_edge = fiberbundle_ee[index_facet][fb_section_ee] [ sign_access ];

         gsse::resize(2)(fiberbundle_ft[cnt_fb_in]);
         gsse::pushback(fiberbundle_ft[cnt_fb_in][fb_section_facet]) = index_edge;
      }
   }
}


// #############################################################################################################################
// #### fiberbundle data structure for synopsis/dfise file format
//
template<long DIM>
void converter(std::string filename_in, std::string filename_out)
{
   typedef double NumericT;
   typedef long   IndexBS;
   typedef long   IndexCell;
   static const long ISD = 1;
   typedef gsse::array<NumericT, DIM> PosNumeric;

   typedef typename gsse::fiberbundle::create::template apply<PosNumeric>::type                                                                    FB_0CellPos;
   typedef typename gsse::fiberbundle::create::template apply<IndexCell, std::vector, ISD, IndexBS, std::vector, DIM, gsse::cell_arbitrary>::type  FB_NCell;

   typedef gsse::fiberbundle::collection<
      DIM, gsse::cell_nothing
      , boost::fusion::pair<gsse::access_specifier::AP_0, FB_0CellPos>
      , boost::fusion::pair<gsse::access_specifier::AT_1, FB_NCell>
      , boost::fusion::pair<gsse::access_specifier::AT_2, FB_NCell>
      , boost::fusion::pair<gsse::access_specifier::AT_3, FB_NCell>
      > CC;

   // ###############

   DFISE_tokens::name_container[0] = DFISE_tokens::name_0cell;
   DFISE_tokens::name_container[1] = DFISE_tokens::name_1cell;
   DFISE_tokens::name_container[2] = DFISE_tokens::name_2cell;
   DFISE_tokens::name_container[3] = DFISE_tokens::name_3cell;

   HIN_tokens::name_container  [0] = HIN_tokens::name_0cell;
   HIN_tokens::name_container  [2] = HIN_tokens::name_2cell;
   HIN_tokens::name_container  [3] = HIN_tokens::name_3cell;

   {
      CC container;
   
      reader(container, filename_in);
   
      calculate_2cell_from_1cell (gsse::at_dim<gsse::access_specifier::AT_1>(container),
                                  gsse::at_dim<gsse::access_specifier::AT_2>(container));
   
      writer(container, filename_out);
   }
}



template<unsigned long DIM>
void run_all(std::string filename_in, std::string filename_out)
{
   converter<DIM>(filename_in, filename_out);
}



// int main(int argc, char** argv)
// {
//    if (argc < 2)
//    {
//       std::cout << "use: ./converter filename_in [filename_out] " << std::endl;
//       exit(-1);
//    }

//    std::string filename_in  = std::string(argv[1]);
//    std::string filename_out = "";

//    if (argc >= 3)
//    {
//       filename_out = std::string(argv[2]);
//    }

//    run_all<3>(filename_in, filename_out);      // synposis/dfise file format -> 3D , but only the boundary is given for now
   
//    return 0;

// }
