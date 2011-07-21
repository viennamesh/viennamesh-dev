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
   
#ifndef VIENNAUTILS_LOGGER_HPP
#define VIENNAUTILS_LOGGER_HPP
   
#include <iostream>

namespace viennautils {
namespace logger {

struct red     {};
struct green   {};
struct yellow  {};
struct blue    {};
struct magenta {};
struct cyan    {};

// for colors: http://en.wikipedia.org/wiki/ANSI_escape_code


template< typename Level >
struct record_pump { };

//
// RED  
//
template< >
struct record_pump < viennautils::logger::red >
{
   template< typename T >
   record_pump<viennautils::logger::red> const& 
   operator<< (T const& value) const
   {
      std::cout << "\033[1;31m" << value << "\033[0m";
      return *this;
   }
};

//
// GREEN  
//
template< >
struct record_pump < viennautils::logger::green >
{
   template< typename T >
   record_pump<viennautils::logger::green> const& 
   operator<< (T const& value) const
   {
      std::cout << "\033[1;32m" << value << "\033[0m";
      return *this;
   }
};

//
// YELLOW  
//
template< >
struct record_pump < viennautils::logger::yellow >
{
   template< typename T >
   record_pump<viennautils::logger::yellow> const& 
   operator<< (T const& value) const
   {
      std::cout << "\033[1;33m" << value << "\033[0m";
      return *this;
   }
};

//
// BLUE  
//
template< >
struct record_pump < viennautils::logger::blue >
{
   template< typename T >
   record_pump<viennautils::logger::blue> const& 
   operator<< (T const& value) const
   {
      std::cout << "\033[1;34m" << value << "\033[0m";
      return *this;
   }
};

//
// MAGENTA  
//
template< >
struct record_pump < viennautils::logger::magenta >
{
   template< typename T >
   record_pump<viennautils::logger::magenta> const& 
   operator<< (T const& value) const
   {
      std::cout << "\033[1;35m" << value << "\033[0m";
      return *this;
   }
};

//
// CYAN  
//
template< >
struct record_pump < viennautils::logger::cyan >
{
   template< typename T >
   record_pump<viennautils::logger::cyan> const& 
   operator<< (T const& value) const
   {
      std::cout << "\033[1;36m" << value << "\033[0m";
      return *this;
   }
};

template< typename Level >
viennautils::logger::record_pump< Level > 
make_stream()
{
    return viennautils::logger::record_pump< Level >();
}

#define VIENNALOGGER(lvl)\
   viennautils::logger::record_pump< lvl >()



} // end namespace logger
} // end namespace viennamos

#endif 



