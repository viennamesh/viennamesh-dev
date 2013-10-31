#ifndef VIENNAMESH_STRING_TOOLS_HPP
#define VIENNAMESH_STRING_TOOLS_HPP

#include <string>
#include <list>

namespace stringtools
{

  std::list<std::string> split_string( std::string const & str, std::string const & delimiter )
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

    std::cout << "something went wrong..." << std::endl;

    return std::list<std::string>();
  }

  std::string remove_begin_end_chars( std::string const & str )
  {
    std::string::size_type start = 0;
    for (; start != str.size(); ++start)
    {
      char const & cur = str[start];
      if ((cur != ' ') && (cur != '\n') && (cur != '\t'))
          break;
    }

    std::string::size_type end = str.size()-1;
    for (; end != -1; --end)
    {
      char const & cur = str[end];
      if ((cur != ' ') && (cur != '\n') && (cur != '\t'))
          break;
    }

    return str.substr( start, end-start+1 );
  }


  template<typename stream_type>
  std::string read_stream( stream_type & stream )
  {
    std::string line;
    std::string tmp;

    while ( std::getline( stream, line ) )
      tmp += line + "\n";

    return tmp;
  }

}

#endif
