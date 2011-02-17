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


#include "viennautils/io.hpp"
#include "viennautils/dumptype.hpp"

#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/domain.hpp"

#include "viennamesh/generator.hpp"
#include "viennamesh/wrapper/gtsio.hpp"
#include "viennamesh/transfer/viennagrid.hpp"

template<typename DomainT>
void statistics(DomainT& domain, std::ostream& ostr = std::cout)
{
   typedef typename DomainT::config_type                                         domain_config_type;
   typedef typename domain_config_type::cell_tag                                 cell_tag_type;   
   
   static const int DIMG = domain_config_type::dimension_tag::value;
   static const int DIMT = cell_tag_type::topology_level;
   
   typedef viennagrid::segment_t<domain_config_type>                                         segment_type;
   typedef typename viennagrid::result_of::ncell_container<segment_type, DIMT>::type         cell_complex_type;
   typedef typename viennagrid::result_of::iterator<cell_complex_type>::type                 cell_iterator_type;   
   typedef typename viennagrid::result_of::ncell_type<domain_config_type, DIMT>::type        cell_type;
   typedef typename viennagrid::result_of::ncell_container<segment_type, 0>::type            vertex_complex_type;
//    typedef typename viennagrid::result_of::iterator<vertex_complex_type>::type               vertex_iterator_type;   // DOES NOT WORK ATM
   typedef typename viennagrid::result_of::ncell_type<domain_config_type, 0>::type           vertex_type;   
   typedef typename viennagrid::result_of::ncell_container<cell_type, 0>::type               vertex_on_cell_container_type;
   typedef typename viennagrid::result_of::iterator<vertex_on_cell_container_type>::type     vertex_on_cell_iterator_type;   
   
   size_t segment_size = domain.segment_container()->size();
   std::cout << "## Domain Statistics ##" << std::endl;
   std::cout << "Topology Dimension: " << DIMT << std::endl;
   std::cout << "Geometry Dimension: " << DIMG << std::endl;
   std::cout << "   segment size: " << segment_size << std::endl;
   
   for(size_t si = 0; si < segment_size; si++)
   {  
      std::cout << "   segment: " << si << std::endl;
      //std::cout << "     cell size: " << domain.segment(si).size<DIMT>() << std::endl;
      cell_complex_type cell_complex = viennagrid::ncells<DIMT>(domain.segment(si));
      std::cout << "      cells:" << std::endl;
      size_t cid = 0;
      for(cell_iterator_type cit = cell_complex.begin(); cit != cell_complex.end(); cit++)
      {
         std::cout << "         id: " << cid++ << " - vertices: ";
         vertex_on_cell_container_type vertex_on_cell_cont = viennagrid::ncells<0>(*cit);
         for(vertex_on_cell_iterator_type vocit = vertex_on_cell_cont.begin();
             vocit != vertex_on_cell_cont.end(); vocit++)
         {
            std::cout << vocit->getID() << " ";
         }         
         std::cout << std::endl;
      }
  /*     
      vertex_complex_type vertex_complex = viennagrid::ncells<0>(domain.segment(si));
     std::cout << "      vertices:" << std::endl;
      size_t cid = 0;
      for(vertex_iterator_type vit = vertex_complex.begin(); vit != vertex_complex.end(); vit++)
      {
         std::cout << "         id: " << cid++ << " - vertex: " << vit->getID() << std::endl;
      }     */ 
   }
//   std::cout << "Cells in Segment 0: "    << domain.segment(0).size<CellTag::topology_level>() << std::endl;
//   std::cout << "Cells in Segment 1: "    << domain.segment(1).size<CellTag::topology_level>() << std::endl;   
   
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
         GTSIO::DeviceFileReader* pReader = new GTSIO::DeviceFileReader();
         GTSIO::GTSDevice*        pDevice = new GTSIO::GTSDevice();
         try {
            pReader->loadDeviceFromFile(inputfile.c_str(), pDevice);
         } 
         catch (GTSIO::GtsioException ex) {
            std::cerr << ex.toString() << std::endl << std::endl;
         }
         catch (std::exception stdEx) {
            std::cerr << stdEx.what() << std::endl << std::endl;
         }         
         
         typedef viennamesh::wrapper<viennamesh::tag::gtsio3, GTSIO::GTSDevice*>     gts_wrapper_type;;
         gts_wrapper_type data_in(pDevice);
         

         typedef viennamesh::result_of::mesh_generator<viennamesh::tag::triangle, gts_wrapper_type>::type   mesh_generator_type;
         //mesh_generator_type mesher(data_in);         
         
/*         typedef viennagrid::domain<viennagrid::config::line_2d>     domain_in_type;
         typedef domain_in_type::config_type                         domain_config_type;
         domain_in_type domain_in;
         std::cout << "# viennamesh::reading domain .. " << std::endl;
         viennautils::io::gts_reader   gtsread;
         gtsread(domain_in, inputfile, false);*/
         
         
/*         std::cout << "## viennamesh::generating mesh .. " << std::endl;
         // ------------------------------------------------------------------------------------------
         typedef viennagrid::domain<viennagrid::config::triangular_2d> domain_out_type;
         domain_out_type domain_out;

         using namespace viennamesh;
         typedef boost::fusion::result_of::make_map<
            tag::cell_type, tag::algorithm,            tag::criteria,            tag::dim_topo, tag::dim_geom, 
            tag::simplex,   tag::incremental_delaunay, tag::conforming_delaunay, tag::two,      tag::two  >::type  mesher_properties;          
         
         typedef viennamesh::result_of::generate_mesh_kernel<mesher_properties>::type        mesh_kernel;
            
         typedef viennamesh::wrapper<viennamesh::tag::viennagrid, domain_in_type>            data_input_type;
         data_input_type data_in(domain_in);
         
         typedef viennamesh::result_of::mesh_generator<mesh_kernel, data_input_type>::type   mesh_generator_type;
          mesh_generator_type mesher(data_in);
 
          mesher( boost::fusion::make_map<tag::criteria, tag::size>(tag::conforming_delaunay(), 1.0) );         

         typedef viennamesh::transfer<viennamesh::tag::viennagrid>      transfer_type;
         transfer_type  transfer;
         transfer(mesher, domain_out);
         // ------------------------------------------------------------------------------------------         
         if(output_extension == "vtk")
         {
            std::cout << "# viennamesh::writing vtk files .. " << std::endl;
            viennagrid::io::Vtk_writer<domain_out_type> my_vtk_writer;
            my_vtk_writer.writeDomain(domain_out, outputfile);
         }
         else
         {
            std::cerr << "## Error: output fileformat not supported: " << output_extension << std::endl;
            std::cerr << "## shutting down .." << std::endl;     
            return -1;         
         }         */
      }
      if(dim == 3)
      {
         
      }
      else
      {
         std::cerr << "## Error: dimension not supported: " << dim << std::endl;
         std::cerr << "## shutting down .." << std::endl;     
         return -1;         
      }
   }
//    if(input_extension == "gts")
//    {
//       if(dim == 2)
//       {
//       }
//    }
   else
   {
      std::cerr << "## Error: input fileformat not supported: " << input_extension << std::endl;
      std::cerr << "## shutting down .." << std::endl;     
      return -1;
   }
   
   return 0;
}



