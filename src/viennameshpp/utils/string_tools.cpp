// #include "viennamesh/utils/string_tools.hpp"
//
// namespace stringtools
// {
//   std::list<std::string> split_string( std::string const & str, std::string const & delimiter )
//   {
//     std::list<std::string> tokens;
//
//     std::string::size_type pos = 0;
//     while (pos < str.size())
//     {
//       std::string::size_type new_pos = str.find(delimiter, pos);
//       if (new_pos == std::string::npos)
//       {
//         tokens.push_back( str.substr(pos, str.size()-pos) );
//         return tokens;
//       }
//
//       tokens.push_back( str.substr(pos, new_pos-pos) );
//       pos = new_pos+delimiter.size();
//     }
//
//     if (pos == str.size())
//       return tokens;
//
//     std::cout << "something went wrong..." << std::endl;
//
//     return std::list<std::string>();
//   }
//
//
//   std::list<std::string> split_string_brackets( std::string const & str, std::string const & delimiter )
//   {
//     std::list<std::string> tokens;
//
//     std::vector<int> bracket_count(str.size());
//
//     std::string::size_type pos = 0;
//     int bc = 0;
//     for (pos = 0; pos != str.size(); ++pos)
//     {
//       if (str[pos] == '(')
//         ++bc;
//
//       bracket_count[pos] = bc;
//
//       if (str[pos] == ')')
//         --bc;
//     }
//
// //     std::cout << str << std::endl;
// //     for (pos = 0; pos != str.size(); ++pos)
// //       std::cout << bracket_count[pos];
// //     std::cout << std::endl;
//
//     pos = 0;
//     std::string::size_type old_pos = 0;
//     while (pos < str.size())
//     {
//       std::string::size_type new_pos = str.find(delimiter, pos);
//
// //       std::cout << "Found on " << new_pos << std::endl;
//
//       if (new_pos == std::string::npos)
//       {
//         tokens.push_back( str.substr(old_pos, str.size()-old_pos) );
// //         std::cout << "Added " << tokens.back() << std::endl;
//         return tokens;
//       }
//
//       if (bracket_count[new_pos] != 0)
//       {
// //         std::cout << "bracket count not 0 -> skipping" << std::endl;
//         pos = new_pos+delimiter.size();
//         continue;
//       }
//
//
//       tokens.push_back( str.substr(old_pos, new_pos-old_pos) );
// //       std::cout << "Added " << tokens.back() << std::endl;
//       pos = new_pos+delimiter.size();
//       old_pos = pos;
//     }
//
//     if (pos == str.size())
//       return tokens;
//
//     std::cout << "something went wrong..." << std::endl;
//
//     return std::list<std::string>();
//   }
//
//
//
//
//   std::string remove_begin_end_chars( std::string const & str )
//   {
//     std::string::size_type start = 0;
//     for (; start != str.size(); ++start)
//     {
//       char const & cur = str[start];
//       if ((cur != ' ') && (cur != '\n') && (cur != '\t'))
//           break;
//     }
//
//     std::string::size_type end = str.size()-1;
//     for (; end != std::size_t(-1); --end)
//     {
//       char const & cur = str[end];
//       if ((cur != ' ') && (cur != '\n') && (cur != '\t'))
//           break;
//     }
//
//     return str.substr( start, end-start+1 );
//   }
//
//
//
//   void erase_all( std::string & str, std::string const & to_erase )
//   {
//     std::string::size_type pos;
//     while ( (pos = str.find(to_erase)) != std::string::npos)
//       str.erase(pos, to_erase.size());
//   }
//
//
//
//
//   std::string extract_filename( std::string const & path )
//   {
//     std::string path_delimiters = "\\/";
//
//     std::size_t pos = path.find_last_of(path_delimiters);
//     if (pos == std::string::npos)
//       return path;
//     return path.substr(pos+1, std::string::npos);
//   }
//
//   std::string extract_path( std::string const & path )
//   {
//     std::string path_delimiters = "\\/";
//
//     std::size_t pos = path.find_last_of(path_delimiters);
//     if (pos == std::string::npos)
//       return "";
//     return path.substr(0, pos+1);
//   }
// }
