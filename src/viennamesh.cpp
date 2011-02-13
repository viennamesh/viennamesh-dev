/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at
               Karl Rupp                                rupp@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#include <iostream>

#include "viennamesh/mesh.hpp"
#include "viennamesh/interfaces.hpp"            // all meshers

#include "viennagrid/domain.hpp"
#include "viennautils/io.hpp"

#include "viennagrid/io/vtk_writer.hpp"


#include "viennagrid/domain.hpp"

template<typename DomainT>
void statistics(DomainT& domain, std::ostream& ostr = std::cout)
{
   
    
}

int main(int argc, char * argv[])
{
   if(argc != 4)
   {
      std::cerr << "## Error::Parameter - usage: " << argv[0] << " inputfile.{gts} outputfile.{vtu} dimension" << std::endl;
      std::cerr << "## shutting down .." << std::endl;
      return -1;
   }
   
   std::string inputfile(argv[1]);
   std::string outputfile(argv[2]);
   int dim(atoi(argv[3]));
   
   std::string::size_type pos = inputfile.rfind(".")+1;
   std::string input_extension = inputfile.substr(pos, inputfile.size());
   pos = outputfile.rfind(".")+1;   
   std::string output_extension = outputfile.substr(pos, outputfile.size());
   
   if(input_extension == "gts")
   {
      if(dim == 2)
      {
         typedef viennagrid::domain<viennagrid::config::line_2d>     domain_in_type;
         domain_in_type domain_in;
         std::cout << "# viennamesh::reading domain .. " << std::endl;
         viennautils::io::gts_reader   gtsread;
         gtsread(domain_in, inputfile, true);
         
         statistics(domain_in);
         
         typedef viennagrid::domain<viennagrid::config::triangular_2d> domain_out_type;
         domain_out_type domain_out;

         std::cout << "# viennamesh::generating mesh .. " << std::endl;
          typedef viennamesh::mesh_generator<double> mesh_generator_type;
          mesh_generator_type mesher;
          mesher(domain_in);

         std::cout << "# viennamesh::building output domain .. " << std::endl;         
         viennamesh::transfer(mesher, domain_out);
         
         if(output_extension == "vtu")
         {
            std::cout << "# viennamesh::writing vtu file .. " << std::endl;
            viennagrid::io::Vtk_writer<domain_out_type> my_vtk_writer;
            my_vtk_writer.writeDomain(domain_out, outputfile);
         }
         else
         {
            std::cerr << "## Error: output fileformat not supported: " << output_extension << std::endl;
            std::cerr << "## shutting down .." << std::endl;     
            return -1;         
         }         
         
         // idea: provide different mesh generators, with different capabilities.
         // use our fold::property approach to get a subset of generators for a given property list.
         // provide different tools to classify/orient/improve quality of a mesh (extract the vgmodeler tools) 
         //
         // acc<dim>(property_map) = 2;
         // acc<method>(property_map) = constrained_delaunay;
         // acc<size>(property_map) = ....
         // a heterogenous mesher kernel environment canbe supported, the fold approach automatically 
         // computes the mesher kernels which fits the requested task
         //
         // typedef typename viennamesh::mesh_generator<property_map>::type  mesh_generator_type;
         // mesh_generator_type mesh_generator;
         // mesh_generator(domain_in);   // a functor, the same mesher can be applied on different input domains
         
         // typedef domain_generator<mesh_generator>  domain_generator_type;
         // domain_generator_type domain_generator(mesh_generator);
         // domain_generator(domain_out);

      }
      else
      {
         std::cerr << "## Error: dimension not supported: " << dim << std::endl;
         std::cerr << "## shutting down .." << std::endl;     
         return -1;         
      }
   }
   else
   {
      std::cerr << "## Error: input fileformat not supported: " << input_extension << std::endl;
      std::cerr << "## shutting down .." << std::endl;     
      return -1;
   }
   
   return 0;
}



