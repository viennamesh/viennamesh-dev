/* ============================================================================
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        

   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_ALGORITHM_IO_HH)
#define GSSE_ALGORITHM_IO_HH

// *** system includes
#include <iostream>
#include <algorithm>
// *** BOOST includes
#include <boost/fusion/include/for_each.hpp>
// *** GSSE includes
#include "gsse/util/debug_meta.hpp"
#include "gsse/traversal.hpp"


// ############################################################
//
namespace gsse { namespace IO {
namespace 
{
   std::string set_open ("{");
   std::string set_close("}");
}
}
}

namespace gsse { namespace algorithm {

struct saver
{
   saver(std::ostream& ostr):ostr(ostr) {}
   
   template <typename Pair>
   void operator()(Pair const& data) const
   {
     ostr << " " << gsse::type_to_string< typename Pair::first_type > ();
     //      gsse::dump_type_str< typename Pair::first_type> (ostr);
     ostr << " " << gsse::IO::set_open << " " << data.second  << " " << gsse::IO::set_close << " ";
     ostr << std::endl;
   }
protected:
   std::ostream& ostr;
};

template <typename Container>
void store(Container const& container, std::ostream& ostr= std::cout)
{
   boost::fusion::for_each(container, saver(ostr));
   ostr << std::endl;
}

template <typename Container>
void store_complex(Container const& container, std::ostream& ostr= std::cout)
{
  ostr << Container::DIM << " "  << gsse::type_to_string<typename Container::CellTopology> () ;
  //   gsse::dump_type_str<typename Container::CellTopology>(ostr);
   ostr << std::endl;
   store(container, ostr);

}



struct loader
{
   loader(std::istream& istr):istr(istr) {}
    
   template <typename Pair>
   void operator()(Pair& data)   const
   {
      std::string tempstr;
      istr >> tempstr; // should be Dual to :: gsse::dump_type_str< typename Pair::first_type> (ostr);
      istr >> tempstr;  
      if (tempstr != gsse::IO::set_open)
      {
         std::cout << "## error at loader ## " << std::endl;
         std::cout << "temstr; " << tempstr << std::endl;
      }
      istr >> data.second;
      istr >> tempstr;  
      if (tempstr != gsse::IO::set_close)
      {
         std::cout << "## error at loader ## " << std::endl;
      }
   }
protected:   
   std::istream& istr;
};

template <typename Stuff>
void load(Stuff& stuff, std::istream& istr= std::cin)
{
   boost::fusion::for_each(stuff, loader(istr));
}


template <typename Container>
void load_complex(Container& container, std::istream& istr= std::cin)
{
   long dim;
   istr >> dim;
   std::string tempstr;
   istr >> tempstr;

   // check if dim / celltype(tempstr) is correct for container

   load(container, istr);
}


// #################################################
//  already usable 
//
struct printer
{
   printer(std::ostream& ostr):ostr(ostr) {}
   
   template <typename Data>
   void operator()(Data const& data) const
   {
      ostr << data << " .. " ;
   }
protected:
   std::ostream& ostr;
};

// template <typename Container>
// void print(Container const& container, std::ostream& ostr= std::cout)
// {
//    boost::fusion::for_each(container, printer(ostr));
//    ostr << std::endl;
// }



// ##############################################################################


template<typename Container1D>
void print_1d_no(Container1D const& container, std::ostream& ostr = std::cout)
{
   gsse::traverse() 
   [
      ostr 
      << boost::phoenix::val(" ") 
      << boost::phoenix::val(gsse::IO::set_open) 
      << boost::phoenix::val(" ") 
      << boost::phoenix::arg_names::_1
      << boost::phoenix::val(" ") 
      << boost::phoenix::val(gsse::IO::set_close) 
      << boost::phoenix::val(" ") 
   ](container);
}


template<typename Container1D>
void print_1d(Container1D const& container, std::ostream& ostr = std::cout)
{
   gsse::traverse() 
   [
      ostr << boost::phoenix::val(" ") << boost::phoenix::arg_names::_1  << std::endl
   ](container);
   ostr << std::endl;
}

template<typename Container2D>
void print_2d(Container2D const& container, std::ostream& ostr = std::cout)
{
   gsse::traverse() 
   [
      gsse::traverse() 
      [
         ostr << boost::phoenix::val(" ") << boost::phoenix::arg_names::_1 
      ]
      ,
      ostr << boost::phoenix::val("") << std::endl
   ](container);
   ostr << std::endl;
}


template<typename Container1D>
void print(Container1D const& container, std::ostream& ostr = std::cout)
{
   print_1d(container, ostr);
}




}  // namespace algorithm


using gsse::algorithm::print;
using gsse::algorithm::print_1d;
using gsse::algorithm::print_1d_no;
using gsse::algorithm::print_2d;

}  // namespace gsse


namespace std {

   template<typename T1, typename T2>
   std::ostream& operator<<(std::ostream& ostr, std::pair<T1, T2> pair)
   {
      ostr << pair.first << " " << pair.second;
      return ostr;
   }

   template<typename T1, size_t DIM>
   std::ostream& operator<<(std::ostream& ostr, boost::array<T1, DIM> array)
   {
      for (size_t i = 0; i < gsse::size(array) -1; ++i)
      {
         ostr << array[i] << " " ;
      }
      ostr << array[ array.size()-1 ];
      return ostr;
   }

   template<typename T1, size_t DIM>
   std::ostream& operator<<(std::ostream& ostr, gsse::array<T1, DIM> array)
   {
      for (size_t i = 0; i < gsse::size(array)-1; ++i)
      {
         ostr << array[i] << " " ;
      }
      ostr << array[ array.size()-1 ];
      return ostr;
   }

   // ### input

   template<typename T1, size_t DIM>
   std::istream& operator>>(std::istream& istr, boost::array<T1, DIM>& array)
   {
      for (size_t i = 0; i < gsse::size(array); ++i)
      {
         istr >> array[i] ;
      }
      return istr;
   }
   template<typename T1, size_t DIM>
   std::istream& operator>>(std::istream& istr, gsse::array<T1, DIM>& array)
   {
      for (size_t i = 0; i < gsse::size(array); ++i)
      {
         istr >> array[i] ;
      }
      return istr;
   }


// ========================
   template<typename T1>
   std::ostream& operator<<(std::ostream& ostr, std::vector<T1> const& vec)
   {
      ostr << " " << gsse::size(vec) << "  ";
      gsse::print_1d_no(vec, ostr);
      return ostr;
   }

   template<typename T1>
   std::istream& operator>>(std::istream& istr, std::vector<T1>& vec)
   {
      size_t size;
      istr >> size;
//      vec.resize(size);

      std::string tempstr;
      for (size_t icnt = 0; icnt < size; ++icnt)
      {
         istr >> tempstr;  
         if (tempstr != gsse::IO::set_open)
         {
            std::cout << "## error at loader ## vec in" << std::endl;
            std::cout << "temstr; " << tempstr << std::endl;
         }

         T1 val;
         istr >> val;
         vec.push_back(val);

         istr >> tempstr;  
         if (tempstr != gsse::IO::set_close)
         {
            std::cout << "## error at loader ## vec in close" << std::endl;
            std::cout << "temstr; " << tempstr << std::endl;
         }

      }
      return istr;
   }


   template<typename T1, typename T2>
   std::ostream& operator<<(std::ostream& ostr, std::map<T1, T2> const& stdmap)
   {
      ostr << " " << gsse::size(stdmap) << "  ";

      typename std::map<T1, T2>::const_iterator map_it;
      for (map_it = stdmap.begin();
           map_it != stdmap.end();
           ++map_it)
      {

         ostr << " " << gsse::IO::set_open << " ";
         ostr << (*map_it).first ;
         ostr << " " << gsse::IO::set_close << " ";

         ostr << " " << gsse::IO::set_open << " ";
         ostr << (*map_it).second ;
         ostr << " " << gsse::IO::set_close << " ";

      }

      return ostr;

   }


   template<typename T1, typename T2>
   std::istream& operator>>(std::istream& istr, std::map<T1, T2>& stdmap)
   {
//      gsse::dump_type<std::map<T1,T2> >();
      size_t size ;
      istr >> size;

      for (size_t icnt = 0; icnt < size; ++icnt)
      {
         T1 key;
         T2 value;

         std::string tempstr;

         istr >> tempstr;
         if (tempstr != gsse::IO::set_open)
         {
            std::cout << "## error at loader ## std::map level 1" << std::endl;
            std::cout << "tempstr: " << tempstr << std::endl;
         }
         istr >> key;

         istr >> tempstr;  
         if (tempstr != gsse::IO::set_close)
         {
            std::cout << "## error at loader ## vec in close" << std::endl;
            std::cout << "temstr; " << tempstr << std::endl;
         }
         

         istr >> tempstr;
         if (tempstr != gsse::IO::set_open)
         {
            std::cout << "## error at loader ## std::map level 1" << std::endl;
            std::cout << "tempstr: " << tempstr << std::endl;
         }
         istr >> value;

         istr >> tempstr;  
         if (tempstr != gsse::IO::set_close)
         {
            std::cout << "## error at loader ## vec in close" << std::endl;
            std::cout << "temstr; " << tempstr << std::endl;
         }

         stdmap[key] = value;

      }


      return istr;
   }


}



#endif
