/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at     
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at                     

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#if !defined(GSSE_UTIL_LOGGER_HH)
#define GSSE_UTIL_LOGGER_HH   
   
// *** system includes
// *** BOOST includes
// *** GSSE includes

namespace gsse {
namespace logger {

struct red     {};
struct green   {};
struct yellow  {};
struct blue    {};
struct magenta {};
struct cyan    {};

// [RH] added
#define GSSELOG_IMPORTANT   //[TODO?] can be transferred to Makefile
#define GSSELOG_WARNING
#define GSSELOG_ERROR
#define GSSELOG_INFO
#define GSSELOG_NOTICE
struct important {};
struct warning {};
struct error {};
struct info{};
struct notice{};

// for colors: http://en.wikipedia.org/wiki/ANSI_escape_code


template< typename Level >
struct record_pump { };

//
// RED  
//
template< >
struct record_pump < gsse::logger::red >
{
   template< typename T >
   record_pump<gsse::logger::red> const& 
   operator<< (T const& value) const
   {
      std::cout << "\033[1;31m" << value << "\033[0m";
      return *this;
   }
};
template< >
struct record_pump < gsse::logger::error >
{
   template< typename T >
   record_pump<gsse::logger::error> const& 
   operator<< (T const& value) const
   {
#if defined(GSSELOG_ERROR)
      std::cout << "\033[1;31m" << value << "\033[0m";
#endif
      return *this;
   }
};


//
// GREEN  
//
template< >
struct record_pump < gsse::logger::green >
{
   template< typename T >
   record_pump<gsse::logger::green> const& 
   operator<< (T const& value) const
   {
      std::cout << "\033[1;32m" << value << "\033[0m";
      return *this;
   }
};
template< >
struct record_pump < gsse::logger::info >
{
   template< typename T >
   record_pump<gsse::logger::info> const& 
   operator<< (T const& value) const
   {

#if defined(GSSELOG_INFO)
     std::cout << "\033[1;32m" << value << "\033[0m";
#endif
     return *this;
   }
};

//
// YELLOW  
//
template< >
struct record_pump < gsse::logger::yellow >
{
   template< typename T >
   record_pump<gsse::logger::yellow> const& 
   operator<< (T const& value) const
   {
      std::cout << "\033[1;33m" << value << "\033[0m";
      return *this;
   }
};
template< >
struct record_pump < gsse::logger::important >
{
   template< typename T >
   record_pump<gsse::logger::important> const& 
   operator<< (T const& value) const
   {
#if defined(GSSELOG_IMPORTANT)
      std::cout << "\033[1;33m" << value << "\033[0m";
#endif
      return *this;
   }
};


//
// BLUE  
//
template< >
struct record_pump < gsse::logger::blue >
{
   template< typename T >
   record_pump<gsse::logger::blue> const& 
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
struct record_pump < gsse::logger::magenta >
{
   template< typename T >
   record_pump<gsse::logger::magenta> const& 
   operator<< (T const& value) const
   {
      std::cout << "\033[1;35m" << value << "\033[0m";
      return *this;
   }
};
template< >
struct record_pump < gsse::logger::warning >
{
   template< typename T >
   record_pump<gsse::logger::warning> const& 
   operator<< (T const& value) const
   {
#if defined(GSSELOG_WARNING)
      std::cout << "\033[1;35m" << value << "\033[0m";
#endif
      return *this;
   }
};
//
// CYAN  
//
template< >
struct record_pump < gsse::logger::cyan >
{
   template< typename T >
   record_pump<gsse::logger::cyan> const& 
   operator<< (T const& value) const
   {
      std::cout << "\033[1;36m" << value << "\033[0m";
      return *this;
   }
};
template< >
struct record_pump < gsse::logger::notice >
{
   template< typename T >
   record_pump<gsse::logger::notice> const& 
   operator<< (T const& value) const
   {
#if defined(GSSELOG_NOTICE)
      std::cout << "\033[1;36m" << value << "\033[0m";
#endif
      return *this;
   }
};


template< typename Level >
gsse::logger::record_pump< Level > 
make_stream()
{
    return gsse::logger::record_pump< Level >();
}

#define GSSELOGGER(lvl)\
   gsse::logger::record_pump< lvl >()



} // end namespace log
} // end namesapce gsse

#endif 



