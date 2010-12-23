/* ============================================================================
   Copyright (c) 2007-2010 Franz Stimpfl                          franz@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GSSE_UTIL_FILE_FORMATS_HPP
#define GSSE_UTIL_FILE_FORMATS_HPP

#include "gsse/util/file_formats/stl.hpp"
#include "gsse/util/file_formats/gau.hpp"
#include "gsse/util/file_formats/hin.hpp"
#include "gsse/util/file_formats/gsse.hpp"

namespace gsse {
   namespace result_of {
      
      template <typename FormatT>
      struct converter_impl { };
      
      template <>
      struct converter_impl < gsse::converter::tag_gau >
      {
         typedef gsse::converter::gau_impl type;
      };
      
      template <>
      struct converter_impl < gsse::converter::tag_gsse >
      {
         typedef gsse::converter::gsse_impl type;
      };

      template <>
      struct converter_impl < gsse::converter::tag_hin >
      {
         typedef gsse::converter::hin_impl type;
      };

      template <>
      struct converter_impl < gsse::converter::tag_hin2 >
      {
         typedef gsse::converter::hin2_impl type;
      };

      template <>
      struct converter_impl < gsse::converter::tag_stl >
      {
         typedef gsse::converter::stl_impl type;
      };
               
      template<typename FormatT>
      struct converter
      {         
         typedef typename gsse::result_of::converter_impl<FormatT>::type type;
      };

   } // end namespace: result_of

      
   // [INFO] using streams
   //
   template < typename Format, typename FullSpace >
   void reader ( FullSpace& full_space, std::istream& input = std::cin )
   { 
      typename gsse::result_of::converter<Format>::type io;
      io.read(full_space, input);      
   }

   template < typename Format, typename FullSpace >
   void writer ( FullSpace& full_space, std::ostream& output = std::cout )
   { 
      typename gsse::result_of::converter<Format>::type io;
      io.write(full_space, output);      
   }

   // [INFO] using strings
   //
   template < typename Format, typename FullSpace >
   void reader ( FullSpace& full_space, const std::string& input )
   { 
      std::ifstream file(input.c_str());
      typename gsse::result_of::converter<Format>::type io;
      io.read(full_space, file);      
   }

   template < typename Format, typename FullSpace >
   void writer ( FullSpace& full_space, const std::string& output)
   { 
      std::ofstream file(output.c_str());
      typename gsse::result_of::converter<Format>::type io;
      io.write(full_space, file); 
   }
   
} // end namespace: gsse


#endif
