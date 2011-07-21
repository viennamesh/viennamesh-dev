/* =============================================================================
   Copyright (c) 2010, 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                ViennaUtils - The Vienna Utilities Library
                             -----------------

   authors:    Josef Weinbub                      weinbub@iue.tuwien.ac.at

   license:    see file LICENSE in the base directory
============================================================================= */


#include "viennautils/logger.hpp"

int main(int argc, char * argv[])
{

   VIENNALOGGER( viennautils::logger::red )        << "some red text" << "\n";
   VIENNALOGGER( viennautils::logger::green )      << "some green text" << "\n";
   VIENNALOGGER( viennautils::logger::yellow )     << "some yellow text" << "\n";
   VIENNALOGGER( viennautils::logger::blue )       << "some blue text" << "\n";
   VIENNALOGGER( viennautils::logger::magenta )    << "some magenta text" << "\n";
   VIENNALOGGER( viennautils::logger::cyan )       << "some cyan text" << "\n";

   return 0;
}



