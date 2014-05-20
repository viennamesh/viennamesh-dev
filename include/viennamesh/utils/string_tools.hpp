#ifndef VIENNAMESH_STRING_TOOLS_HPP
#define VIENNAMESH_STRING_TOOLS_HPP

/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include <string>
#include <list>

namespace stringtools
{
  struct boolalpha_bool;

  namespace detail
  {
    template<typename DestinationT>
    struct lexical_cast_impl
    {
    public:
      typedef DestinationT result_type;

      template<typename SourceT>
      lexical_cast_impl(SourceT const & src)
      {
        ss << src;
      }

      operator result_type()
      {
        result_type tmp;
        ss >> tmp;
        return tmp;
      }

    private:
      std::stringstream ss;
    };

    template<>
    struct lexical_cast_impl<std::string>
    {
    public:
      typedef std::string result_type;

      template<typename SourceT>
      lexical_cast_impl(SourceT const & src)
      {
        ss << src;
      }

      lexical_cast_impl(bool src)
      {
        ss << std::boolalpha << src;
      }

      operator result_type()
      {
        return ss.str();
      }

    private:
      std::stringstream ss;
    };

    // from http://stackoverflow.com/questions/4452136/how-do-i-use-boostlexical-cast-and-stdboolalpha-i-e-boostlexical-cast-b
//     struct locale_bool
//     {
  //     bool data;
  //     locale_bool() {}
  //     locale_bool( bool data ) : data(data) {}
  //     operator bool() const { return data; }
  //     friend std::ostream & operator << ( std::ostream &out, locale_bool b )
  //     {
  //       out << std::boolalpha << b.data;
  //       return out;
  //     }
  //     friend std::istream & operator >> ( std::istream &in, locale_bool &b )
  //     {
  //       in >> std::boolalpha >> b.data;
  //       return in;
  //     }
//     };


    template<>
    struct lexical_cast_impl<boolalpha_bool>
    {
    public:
      typedef bool result_type;

      template<typename SourceT>
      lexical_cast_impl(SourceT const & src)
      {
        ss << std::boolalpha << src;
      }

      operator result_type()
      {
        result_type tmp;
        ss >> std::boolalpha >> tmp;
        return tmp;
      }

    private:
      std::stringstream ss;
    };
  }


  template<typename DestinationT, typename SourceT>
  typename detail::lexical_cast_impl<DestinationT>::result_type lexical_cast( SourceT const & src )
  {
    return detail::lexical_cast_impl<DestinationT>(src);
  }



  inline std::list<std::string> split_string( std::string const & str, std::string const & delimiter )
  {
    std::list<std::string> tokens;

    std::string::size_type pos = 0;
    while (pos < str.size())
    {
      std::string::size_type new_pos = str.find(delimiter, pos);
      if (new_pos == std::string::npos)
      {
          tokens.push_back( str.substr(pos, str.size()-pos) );
          return tokens;
      }

      tokens.push_back( str.substr(pos, new_pos-pos) );
      pos = new_pos+delimiter.size();
    }

    if (pos == str.size())
      return tokens;

    std::cout << "something went wrong..." << std::endl;

    return std::list<std::string>();
  }

  inline std::string remove_begin_end_chars( std::string const & str )
  {
    std::string::size_type start = 0;
    for (; start != str.size(); ++start)
    {
      char const & cur = str[start];
      if ((cur != ' ') && (cur != '\n') && (cur != '\t'))
          break;
    }

    std::string::size_type end = str.size()-1;
    for (; end != std::size_t(-1); --end)
    {
      char const & cur = str[end];
      if ((cur != ' ') && (cur != '\n') && (cur != '\t'))
          break;
    }

    return str.substr( start, end-start+1 );
  }



  inline void erase_all( std::string & str, std::string const & to_erase )
  {
    std::string::size_type pos;
    while ( (pos = str.find(to_erase)) != std::string::npos)
      str.erase(pos, to_erase.size());
  }

  template<typename NumericT>
  std::vector<NumericT> vector_from_string( std::string str )
  {
    erase_all( str, "(" );
    erase_all( str, ")" );

    std::vector<NumericT> result;

    std::list<std::string> elements = split_string(str, ",");
    for (std::list<std::string>::const_iterator eit = elements.begin(); eit != elements.end(); ++eit)
      result.push_back( lexical_cast<NumericT>(*eit) );

    return result;
  }



  template<typename stream_type>
  inline std::string read_stream( stream_type & stream )
  {
    std::string line;
    std::string tmp;

    while ( std::getline( stream, line ) )
      tmp += line + "\n";

    return tmp;
  }



  inline std::string extract_filename( std::string const & path )
  {
    std::string path_delimiters = "\\/";

    std::size_t pos = path.find_last_of(path_delimiters);
    if (pos == std::string::npos)
      return path;
    return path.substr(pos+1, std::string::npos);
  }

  inline std::string extract_path( std::string const & path )
  {
    std::string path_delimiters = "\\/";

    std::size_t pos = path.find_last_of(path_delimiters);
    if (pos == std::string::npos)
      return "";
    return path.substr(0, pos+1);
  }

}

#endif
