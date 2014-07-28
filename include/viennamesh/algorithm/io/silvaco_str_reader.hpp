#ifndef VIENNAGRID_SILVACO_STR_READER_HPP
#define VIENNAGRID_SILVACO_STR_READER_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */


#include <fstream>
#include <iostream>
#include <assert.h>

// #include "viennagrid/forwards.hpp"
#include "viennagrid/io/tetgen_poly_reader.hpp"

#include "viennagrid/mesh/mesh.hpp"

/** @file viennagrid/io/tetgen_poly_reader.hpp
    @brief Provides a reader for Tetgen .poly files. See http://wias-berlin.de/software/tetgen/fformats.poly.html
*/



namespace viennagrid
{
  namespace io
  {
    /** @brief Reader for Tetgen .poly files.
      *
      * See http://wias-berlin.de/software/tetgen/fformats.poly.html for a description of the file format
      */
    struct silvaco_str_reader
    {
      /** @brief The functor interface triggering the read operation. Segmentations are not supported in this version.
       *
       * @param mesh_obj      The mesh where the file content is written to
       * @param filename      Name of the file
       */
      template <typename MeshT>
      void operator()(MeshT & mesh, std::string const & filename) const
      {
        typedef typename viennagrid::result_of::point<MeshT>::type           PointType;

        static const std::size_t point_dim = viennagrid::result_of::static_size<PointType>::value;

        typedef typename result_of::vertex<MeshT>::type         VertexType;
        typedef typename VertexType::id_type VertexIDType;

        std::ifstream reader(filename.c_str());

        #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
        std::cout << "* silvaco_str_reader::operator(): Reading file " << filename << std::endl;
        #endif

        if (!reader)
        {
          throw cannot_open_file_exception("* ViennaGrid: silvaco_str_reader::operator(): File " + filename + ": Cannot open file!");
        }

        if (!reader.good())
        {
          throw bad_file_format_exception("* ViennaGrid: silvaco_str_reader::operator(): File " + filename + ": File is empty.");
        }



        std::string tmp;
        std::string local_tmp;
        std::string type;
        std::istringstream current_line;

        long vertex_count;
        long triangle_count;

        while (true)
        {
          if (!get_valid_line(reader, tmp, '#'))
            throw bad_file_format_exception("* ViennaGrid: silvaco_str_reader::operator(): File " + filename + ": EOF encountered when reading information");

          if (tmp.find("k 3") == 0)
          {
            current_line.str(tmp); current_line.clear();
            current_line >> local_tmp >> local_tmp >> vertex_count >> local_tmp >> triangle_count;
            break;
          }
        }

        typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;

        std::map<long, VertexHandleType> vertex_handles;

        for (long i = 0; i < vertex_count; ++i)
        {
          if (!get_valid_line(reader, tmp, '#'))
            throw bad_file_format_exception("* ViennaGrid: silvaco_str_reader::operator(): File " + filename + ": EOF encountered when reading information");

          PointType point;

          long id;

          current_line.str(tmp); current_line.clear();
          current_line >> type >> id;

          if (type != "c")
            throw bad_file_format_exception("* ViennaGrid: silvaco_str_reader::operator(): File " + filename + ": expected point, got \"" + tmp + "\"");

          for (std::size_t j = 0; j != point_dim; ++j)
            current_line >> point[j];

          vertex_handles[id] = viennagrid::make_vertex(mesh, point);
        }

        for (long i = 0; i < triangle_count; ++i)
        {
          if (!get_valid_line(reader, tmp, '#'))
            throw bad_file_format_exception("* ViennaGrid: silvaco_str_reader::operator(): File " + filename + ": EOF encountered when reading information");

          long id;

          long vertex_id0;
          long vertex_id1;
          long vertex_id2;

          current_line.str(tmp); current_line.clear();
          current_line >> type >> id >> local_tmp >> vertex_id0 >> vertex_id1 >> vertex_id2;

          if (type != "t")
            throw bad_file_format_exception("* ViennaGrid: silvaco_str_reader::operator(): File " + filename + ": expected triangle, got \"" + tmp + "\"");


          viennagrid::make_triangle(mesh, vertex_handles[vertex_id0],
                                          vertex_handles[vertex_id1],
                                          vertex_handles[vertex_id2]);
        }
      } //operator()

    }; //class tetgen_poly_reader

  } //namespace io
} //namespace viennagrid

#endif
