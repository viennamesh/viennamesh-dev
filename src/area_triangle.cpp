/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

// *** system includes
#include <iostream>

// *** viennamesh includes
#include "viennamesh/common.hpp"
#include "viennamesh/wrapper.hpp"
#include "viennamesh/generation.hpp"
#include "viennamesh/adaptation.hpp"
#include "viennamesh/classification.hpp"
#include "viennamesh/io.hpp"

//
// generate 2d meshes
//
template<typename WrappedDatastructureT>
void process_2d(WrappedDatastructureT& data, std::string const& outputfile)
{
   typedef typename viennamesh::result_of::mesh_generator<viennamesh::tag::triangle>::type        mesh_generator_type;
   mesh_generator_type  mesher;

   typedef typename mesh_generator_type::result_type       result_type;   

   result_type result = mesher(data);

   // write paraview/vtk output
   //
   viennamesh::io::domainwriter(result, outputfile);
}

int main(int argc, char *argv[])
{
   if(argc != 4)
   {
      std::cerr << "## Parameter Error - usage: " << argv[0] << " inputfile.{hin,bnd,gau32,inp,sgf} outputfile checks[0|1]" << std::endl;
      std::cerr << "## shutting down .." << std::endl;
      return -1;
   }
   
  
   std::string inputfile(argv[1]);
   std::string outputfile(argv[2]);
   int checks(atoi(argv[3]));
   std::cout << "## " << argv[0] << " processing file: " << inputfile << std::endl;
   
   std::string input_extension  = viennautils::file_extension(inputfile);

   //
   // process different file types
   //
   if(input_extension == "bnd")
   {
      // the reader object
      //
      viennautils::io::bnd_reader my_bnd_reader;
      my_bnd_reader(inputfile); 

      // the reader datastructure is wrapped to offer a specific interface
      //
      typedef viennamesh::wrapper<viennamesh::tag::bnd, viennautils::io::bnd_reader>      bnd_wrapper_type;
      bnd_wrapper_type                    wrapped_data(my_bnd_reader);      

      // mesh this data
      //
      if(my_bnd_reader.dim_geom() == 2)
         process_2d(wrapped_data, outputfile);
      else
      {
         std::cerr << "## only 2d BND meshes supported .. " << std::endl;
         exit(-1);
      }


   }
   else
   {
      std::cerr << "## input file format not supported: " << input_extension << std::endl;
      std::cerr << "## shutting down .. " << std::endl;
      return -1;   
   }

   return 0;
}



