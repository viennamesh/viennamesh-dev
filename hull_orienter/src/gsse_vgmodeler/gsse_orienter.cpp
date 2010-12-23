/* ************* :: Generic Scientific Simulation Environment :: *************
**  http://www.gsse.at                                                     **
mesh generation and adapation: 2D/3D

Copyright (c) 2003-2008 Rene Heinzl                     rene@gsse.at
Copyright (c) 2004-2008 Philipp Schwaha              philipp@gsse.at
Copyright (c) 2007-2008 Franz Stimpfl                  franz@gsse.at

Use, modification and distribution is subject to the Boost Software
License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
**  http://www.boost.org/LICENSE_1_0.txt)                                  **
*************************************************************************** */

// *** system includes
//
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unistd.h>


// ** BOOST includes
//
#include<boost/lexical_cast.hpp>
#include<boost/thread/thread.hpp>
#include<boost/thread/mutex.hpp>
#include<boost/bind.hpp>

// *** GSSE includes
//
#include "gsse_meshing_includes.hpp"
   
//#define DEBUGORIENTATION


template<typename DomainT, typename SegmentT, typename CellIterator>
bool orientation_recursive(DomainT& domain,  SegmentT& segment,  CellIterator& cit)
{
   typedef typename gsse::domain_traits<DomainT>::edge_on_cell_iterator    edge_on_cell_iterator;
   typedef typename gsse::domain_traits<DomainT>::cell_on_edge_iterator    cell_on_edge_iterator;
   typedef typename gsse::domain_traits<DomainT>::storage_type             storage_type;


   long number_of_not_oriented_cells = 0;
   for (edge_on_cell_iterator eocit(*cit); eocit.valid(); ++eocit)
   {
      for (cell_on_edge_iterator coeit(*eocit); coeit.valid(); ++coeit)
      {
         if (domain(*coeit, "orientation")(0,0) == 0.0)
            ++number_of_not_oriented_cells;
      }
   } 

#ifdef DEBUGORIENTATION
  std::cout << " number of not oriented cells: " << number_of_not_oriented_cells << std::endl;
#endif


   // stop condition for recursion
   //
   if (number_of_not_oriented_cells == 0)
      return true;


   // here we have to orient the adjacent cells to the given cell iterator cit
   //
   for (edge_on_cell_iterator eocit(*cit); eocit.valid(); ++eocit)
   {
      for (cell_on_edge_iterator coeit(*eocit); coeit.valid(); ++coeit)
      {
         // if this cell is the reference cell, skip ..
         //
         if ((*coeit) == (*cit))   continue;

         // if this cell has already been dealt with regarding orientation, skip ..
         // 
         if (domain(*coeit, "orientation")(0,0) != 0.0)     continue;

         bool is_oriented_consistently = 
            gsse::check_oriented_neighbor(   segment.retrieve_topology().get_cell( (*coeit).handle() ), 
               segment.retrieve_topology().get_cell( (*cit).handle() )  );
   
         if (is_oriented_consistently)
         {
            domain.store_quantity(*coeit, "orientation", storage_type(1,1,1.0));  // 1.0 -> oriented consistenly
         }
         else
         {
            bool wrong_oriented_neighbor = 
               gsse::check_wrong_oriented_neighbor( segment.retrieve_topology().get_cell( (*coeit).handle() ), 
                  segment.retrieve_topology().get_cell( (*cit).handle() ) ) ;
            if (wrong_oriented_neighbor)
            {
               domain.store_quantity(*coeit, "orientation", storage_type(1,1,2.0));  // 2.0 -> NOT oriented consistenly

               // try to change this error immediatly
               //
               long temp = segment.retrieve_topology().get_cell( (*coeit).handle() )[1];
               segment.retrieve_topology().get_cell((*coeit).handle())[1] = segment.retrieve_topology().get_cell((*coeit).handle())[2];
               segment.retrieve_topology().get_cell((*coeit).handle())[2] = temp;

               domain.store_quantity(*coeit, "orientation", storage_type(1,1,1.0));  // 1.0 -> oriented consistenly
            }
            else
               domain.store_quantity(*coeit, "orientation", storage_type(1,1,-1.0));  // possible error !!!!
         }
      }
   }

   // all adjacent cells are consistently oriented
   //
   // step into the next recursion level
   //
   for (edge_on_cell_iterator eocit(*cit); eocit.valid(); ++eocit)
   {
      for (cell_on_edge_iterator coeit(*eocit); coeit.valid(); ++coeit)
      {
         if ((*coeit) == (*cit))   continue;
         orientation_recursive(domain, segment, coeit);
      }
   }
}



template<typename Domain>
void check_and_repair_orientation(Domain& domain)
{
   typedef typename gsse::domain_traits<Domain>::cell_iterator              cell_iterator;
   typedef typename gsse::domain_traits<Domain>::segment_iterator           segment_iterator;
   typedef typename gsse::domain_traits<Domain>::storage_type               storage_type;
   typedef typename gsse::domain_traits<Domain>::vertex_on_cell_iterator    vertex_on_cell_iterator;

   for (segment_iterator seg_iter = domain.segment_begin(); seg_iter != domain.segment_end(); ++seg_iter)
   {
      (*seg_iter).add_cell_quantity("orientation");
       
      for (cell_iterator cit = (*seg_iter).cell_begin(); cit != (*seg_iter).cell_end(); cit++)
      {
         domain.store_quantity_direct((*cit).handle(), "orientation", storage_type(1,1,0.0));  // 0.0 -> not yet oriented 
      }
   }

//   segment_iterator seg_iter = domain.segment_begin();
//   for (seg_iter = domain.segment_begin(); seg_iter != domain.segment_end(); ++seg_iter)
//   {
//      cell_iterator cit = (*seg_iter).cell_begin(); 

//      vertex_on_cell_iterator vocit(*cit);

//      domain.store_quantity(*cit, "orientation", storage_type(1,1,1.0));  // 1.0 -> oriented consistenly
//   }

  // [FS] .. change the orientation of the second segment
  //
  //    seg_iter = domain.segment_begin();
  //    seg_iter++;
  //    cell_iterator cit = (*seg_iter).cell_begin(); 
  //    std::cout << "cell: " << (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[0] 
  //         << " " << (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[1]
  //         << " " << (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[2] << std::endl;

  //    long temp =    (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[0];
  //    (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[0] = (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[1];
  //    (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[1] = temp;

  //    std::cout << "cell: " << (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[0] 
  //         << " " << (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[1]
  //         << " " << (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[2] << std::endl;

   for (segment_iterator seg_iter = domain.segment_begin(); seg_iter != domain.segment_end(); ++seg_iter)
   {
      cell_iterator cit = (*seg_iter).cell_begin(); 
      //        domain.store_quantity(*cit, "orientation", storage_type(1,1,1.0));  // 1.0 -> oriented consistenly

      orientation_recursive(domain, *seg_iter, cit);
   }   
}

// --------------------------------------

static void printHeader()
{
  std::cout << "  " << std::endl;
  std::cout << "-------------------------------------------------------------------" << std::endl;
  std::cout << "-- ViennaMesh: VGModeler Hull Orienter 1.0,  2010                --" << std::endl;
  std::cout << "-------------------------------------------------------------------" << std::endl;
  std::cout << "--  developed by:                                                --" << std::endl;
  std::cout << "--    Franz Stimpfl, Rene Heinzl, Philipp Schwaha                --" << std::endl;
  std::cout << "--  maintained by:                                               --" << std::endl;
  std::cout << "--    Johann Cervenka, Josef Weinbub                             --" << std::endl;
  std::cout << "--  Institute for Microelectronics, TU Wien, Austria             --" << std::endl;
  std::cout << "--  http://www.iue.tuwien.ac.at                                  --" << std::endl;
  std::cout << "-------------------------------------------------------------------" << std::endl;
  std::cout << "  " << std::endl;
}


// *********************************
//
int main(int argc, char** argv)
{
   printHeader();

   if (argc < 3)
   {
      std::cout << "Usage: " <<  argv[0] << " input_file.gau32 output_file.gau32" << std::endl;
      return -1;
   } 


   std::string filename_in(argv[1]);
   std::string filename_out(argv[2]);
   std::vector<std::string> orient_changer;
   gsse::domain_32t  domain_input;

   try
   {
      domain_input.read_file(filename_in, false);


      int failure = 1;
      int max_count = 0;
      while(failure && max_count < 10)
      {
         std::cout << "## orienter run: " << max_count << "/10" << std::endl;
         failure = 0;

         check_and_repair_orientation(domain_input);

         typedef gsse::domain_traits<gsse::domain_32t>::cell_iterator        cell_iterator;
         typedef gsse::domain_traits<gsse::domain_32t>::segment_iterator     segment_iterator;
         typedef gsse::domain_traits<gsse::domain_32t>::storage_type         storage_type;

         for (segment_iterator seg_iter = domain_input.segment_begin(); seg_iter != domain_input.segment_end(); ++seg_iter)
         {       
            for (cell_iterator cit = (*seg_iter).cell_begin(); cit != (*seg_iter).cell_end(); ++cit)
            {
               storage_type value(1,1,0.0);
               domain_input.retrieve_quantity_direct((*cit).handle(), "orientation", value);
               if(value(0,0) != 1.0)
               {
//                  long temp =    (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[1];
//                  (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[1] = (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[2];
//                  (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[2] = temp;
                  failure = 1;
                  std::cout << "   orient error detected .." << std::endl;
               }
               if(failure == 1) break;
            }
            if(failure == 1) break;
         }
         max_count++;
      } // while
      if(failure == 0)
      {
         std::cout << "## Done - All orientation errors could be removed" << std::endl;
      }
      else
      {
         std::cout << "## Done - Orientation errors could not be removed" << std::endl;
      }

      domain_input.write_file(filename_out);
   }
   catch (std::exception& ex)
   { 
      std::cout << ex.what() << std::endl;
   }

   return 0;
}








