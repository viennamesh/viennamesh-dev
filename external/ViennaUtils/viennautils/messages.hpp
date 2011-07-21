/* =============================================================================
   Copyright (c) 2010, 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                ViennaUtils - The Vienna Utilities Library
                             -----------------

   authors:    Josef Weinbub                      weinbub@iue.tuwien.ac.at               
               Rene Heinzl                        

   license:    see file LICENSE in the base directory
============================================================================= */
   
#ifndef VIENNAUTILS_MESSAGES_HPP
#define VIENNAUTILS_MESSAGES_HPP
   
#include "viennautils/logger.hpp"   
   
namespace viennautils {
namespace msg {

void error(std::string const& str)
{
   VIENNALOGGER( viennautils::logger::red ) << str << "\n";
}

void warning(std::string const& str)
{
   VIENNALOGGER( viennautils::logger::yellow ) << str << "\n";
}

void info(std::string const& str)
{
   VIENNALOGGER( viennautils::logger::green ) << str << "\n";
}

} // end namespace msg
} // end namespace viennamos

#endif 



