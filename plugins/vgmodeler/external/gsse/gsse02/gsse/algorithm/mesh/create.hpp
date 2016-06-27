/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_MESH_CREATE_HH)
#define GSSE_MESH_CREATE_HH

// *** GSSE includes
#include "gsse/util/size.hpp"
#include "gsse/algorithm.hpp"
#include "gsse/util/specifiers.hpp"   
#include "gsse/util.hpp"                     // access, resize, ..

// ####################################################
// 

namespace gsse { namespace mesh {

template<typename CC>
void fill_container(CC& container, long number_cells)
{
   typedef typename CC::value_type CellT;

   for (long cnt = 1; cnt < number_cells; ++cnt)
   {
      CellT cell;

      for (long i = 1 ; i < gsse::size(cell); ++i)
      {
         cell[i] = cnt+i-1;
      }
      cell[0] = 0;
      container.push_back(cell);
   }
}

template<typename CC>
void fill_container_from_implicit(CC& container, long number_cells)
{
   typedef typename CC::value_type CellT;

   long side_length = number_cells;

   {
      for (unsigned int v = 0; v < (side_length - 1); ++v)
      {
         int row_ofs_1 = (v + 0) * side_length;
         int row_ofs_2 = (v + 1) * side_length;
         
         for (unsigned int u = 0; u < (side_length - 1); ++u)
         {
            CellT cell1, cell2;

            // 2 triangles per grid cell
            // add an offset of 3 because of the lone triangle
            cell1 = CellT(
               row_ofs_1 + u,
               row_ofs_1 + u + 1,
               row_ofs_2 + u
               );
            cell2 = CellT(
               row_ofs_2 + u,
               row_ofs_1 + u + 1,
               row_ofs_2 + u + 1
               );
            gsse::pushback( container ) = cell1;
            gsse::pushback( container ) = cell2;

            std::cout << "cell1: " << cell1 << std::endl;
            std::cout << "cell2: " << cell2 << std::endl;
         }
      }
   }
}


template<typename CC>
void fill_container_from_implicit(CC& container, long size_row, long size_col)
{
   typedef typename CC::value_type CellT;

   {
      for (long v = 0; v < (size_row - 1); ++v)
      {
         int row_ofs_1 = (v + 0) * size_col;
         int row_ofs_2 = (v + 1) * size_col;
         
         for (long u = 0; u < (size_col - 1); ++u)
         {
            CellT cell1, cell2;

            // 2 triangles per cell
            cell1 = CellT(
               row_ofs_1 + u,
               row_ofs_1 + u + 1,
               row_ofs_2 + u
               );
            cell2 = CellT(
               row_ofs_2 + u,
               row_ofs_1 + u + 1,
               row_ofs_2 + u + 1
               );
            gsse::pushback( container ) = cell1;
            gsse::pushback( container ) = cell2;
#ifdef GSSE_DEBUG_FULLOUTPUT
//             std::cout << "cell1: " << cell1 << std::endl;
//             std::cout << "cell2: " << cell2 << std::endl;
#endif
         }
      }
   }
}
template<typename CC>
void fill_container_from_implicit_coord(CC& container, 
                                        long size_row, 
                                        long size_col,
                                        double size_x,
                                        double size_y)
{
   typedef typename CC::value_type Coord;

   double delta_x = size_x / (size_col-1);
   double delta_y = size_y / (size_row-1);
   {
      for (long v = 0; v < ( size_row ); ++v)
      {
         for (long u = 0; u < (size_col ); ++u)
         {
            Coord coord ;
            
            coord[0] = v * delta_x;
            coord[1] = u * delta_y;
            if (coord.size() > size_x)
               coord[2] = 0;

            gsse::pushback( container ) = coord;

#ifdef GSSE_DEBUG_FULLOUTPUT
//            std::cout << "coord: " << coord << std::endl;
#endif
         }
      }
   }
}



template<typename CC>
void fill_container_from_implicit_coord(CC& container, long number_cells)
{
   typedef typename CC::value_type Coord;

   long side_length = number_cells;
   double delta_x = 2. / (number_cells-1);
   double delta_y = 2. / (number_cells-1);
   {
      for (unsigned int v = 0; v < (side_length ); ++v)
      {
         for (unsigned int u = 0; u < (side_length ); ++u)
         {
            Coord coord ;
            
            coord[0] = v * delta_x;
            coord[1] = u * delta_y;
            if (coord.size() > 2)
               coord[2] = 0;

            gsse::pushback( container ) = coord;

            std::cout << "coord: " << coord << std::endl;
         }
      }
   }
}

// ##################################################
// gsse specific parts

using namespace gsse::access_specifier; 

template<typename CC, typename InputContainer>
void fill_space_from_input(CC& cell_complex, InputContainer& input_container)
{
   for (long i =0; i < gsse::size(input_container); ++i)
   {
      // [gsse][isd] = 1
      // cell on vertex information
      //
//       for (long inside_i = 0; inside_i < gsse::size(input_container[i]); ++inside_i)
//       {
//          gsse::pushback( gsse::at(input_container[i][inside_i])( gsse::at_dim<AT_vx>(cell_complex) ) ) = i;   // gsse::at is always safe
//       }
      // [gsse][isd] = 0
      gsse::pushback( gsse::at_dim<AT_cl>(cell_complex) )  = input_container[i];
   }
   
   std::cout << std::endl;
//   print_fiberbundle_fixed<AT_cl>    (cell_complex);
}


template<typename CC, typename InputContainer>
void fill_space_from_input_coord(CC& cell_complex, InputContainer& input_container)
{
   for (long i =0; i < gsse::size(input_container); ++i)
   {
      gsse::pushback( gsse::at_dim<AP_vx>(cell_complex) )  = input_container[i];
   }
   
   std::cout << std::endl;
//   print_fiberbundle_fixed<AP_vx>    (cell_complex);
}

template<typename Geometry, typename InputContainer>
void fill_space_from_input_coord_global(Geometry& geometry, InputContainer& input_container)
{
   for (long i =0; i < gsse::size(input_container); ++i)
   {
      gsse::pushback( geometry )  = input_container[i];
   }

}




} // namespace mesh
} // namespace gsse
#endif


