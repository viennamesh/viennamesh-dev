#ifndef VIENNAGRID_IO_VIENNASAP_COMMON_GUARD
#define VIENNAGRID_IO_VIENNASAP_COMMON_GUARD


#include <list>
#include <string>
#include <deque>
#include <map>
#include <boost/smart_ptr.hpp>

#include "viennamesh/utils/string_tools.hpp"

namespace viennagrid
{
    
    namespace io
    {
        
        namespace viennasap_common
        {
            struct token_t : public boost::enable_shared_from_this<token_t>
            {
            //     std::string name;
                std::string content;
                
                typedef std::deque< std::pair<std::string, boost::shared_ptr<token_t> > > children_container;
//                 typedef std::pair<children_container::iterator, children_container::iterator> children_container_range;
                children_container children;
                
                typedef std::deque< boost::shared_ptr<token_t> > unnamed_children_container;
                unnamed_children_container unnamed_children;
                
                
                
                void add_child( std::string const & name, boost::shared_ptr<token_t> const & child )
                {
                    children.push_back( std::make_pair(name, child) );
                }
                
                void add_child( boost::shared_ptr<token_t> const & child )
                {
                    unnamed_children.push_back( child );
                }
                
                
                
                template<typename iterator_type>
                boost::shared_ptr<token_t> get_child( iterator_type begin, iterator_type const & end )
                {
                    if (begin == end) return shared_from_this();
                    
                    for (children_container::iterator it = children.begin(); it != children.end(); ++it)
                        if (it->first == *begin)
                            return it->second->get_child( ++begin, end );
                    
//                     std::map< std::string, boost::shared_ptr<token_t> >::iterator it = children.find(*begin);
//                     if (it != children.end())
//                         return it->second->get_child( ++begin, end );
                    
                    return boost::shared_ptr<token_t>();
                }
                
                
                boost::shared_ptr<token_t> get_child( std::string const & path )
                {
                    std::list<std::string> splitted_path = stringtools::split_string(path, "/");
                    return get_child( splitted_path.begin(), splitted_path.end() );
                }
            };

            std::pair<std::string::size_type, std::string::size_type> next_token_position( std::string const & str, std::string::size_type pos, char openening_char = '{', char closing_char = '}'  )
            {
                std::string::size_type start_pos = str.find( openening_char, pos );
                if (start_pos == std::string::npos)
                    return std::make_pair(std::string::npos, std::string::npos);
                
                unsigned int open_count = 1;
                pos = start_pos + 1;
                
                while (open_count != 0)
                {    
                    std::string::size_type open_position = str.find( openening_char, pos );
                    std::string::size_type closing_position = str.find( closing_char, pos );
                    
                    if (closing_position == std::string::npos)
                        return std::make_pair(std::string::npos, std::string::npos);
                    
                    if (open_position < closing_position)
                    {
                        ++open_count;
                        pos = open_position + 1;
                    }
                    else
                    {
                        --open_count;
                        pos = closing_position + 1;
                    }
                }
                
                return std::make_pair(start_pos, pos);
            }

            boost::shared_ptr<token_t> split_into_tokens( std::string const & str, std::string::size_type start_pos, std::string::size_type end_pos, char openening_char = '{', char closing_char = '}' )
            {
                std::set<std::string> valid_token_names;
                valid_token_names.insert( "desc" );
                valid_token_names.insert( "lengthunit" );
                valid_token_names.insert( "mask" );
                valid_token_names.insert( "rectangle" );
                valid_token_names.insert( "polygon" );
                valid_token_names.insert( "contact" );
                valid_token_names.insert( "material" );
                valid_token_names.insert( "layerstructure" );
                valid_token_names.insert( "origin" );
                valid_token_names.insert( "plane" );
                valid_token_names.insert( "layer" );
                
                valid_token_names.insert( "conductivity" );
                valid_token_names.insert( "thermal_conductivity" );
                valid_token_names.insert( "permittivity" );
                valid_token_names.insert( "thermal_conductivity" );
                
                valid_token_names.insert( "node" );
                valid_token_names.insert( "scalingfactor" );
                valid_token_names.insert( "objdesc" );
                valid_token_names.insert( "struct" );
                valid_token_names.insert( "numobj" );
                valid_token_names.insert( "element" );
                valid_token_names.insert( "nodeattribute" );
                valid_token_names.insert( "name" );
                
                boost::shared_ptr<token_t> root = boost::make_shared<token_t>();
                
                std::string::size_type pos = start_pos;
                while (pos <= end_pos)
                {
                    
                    std::pair<std::string::size_type, std::string::size_type> token_pos = next_token_position(str, pos, openening_char, closing_char);
                    if ((token_pos.first == std::string::npos) || (token_pos.second >= end_pos))
                        break;
                    
            //         std::cout << " FOUND OPEN-CLOSE: " << token_pos.first << " " << token_pos.second << std::endl;
                    
                    boost::shared_ptr<token_t> child = split_into_tokens( str, token_pos.first+1, token_pos.second, openening_char, closing_char );
                    
                    std::string::size_type name_start_pos = token_pos.first-1;
                    while ( (str[name_start_pos] == ' ') || (str[name_start_pos] == '\n') || (str[name_start_pos] == '\t') )
                        --name_start_pos;
                    
                    while ( (str[name_start_pos] != ' ') && (str[name_start_pos] != '\n') && (str[name_start_pos] != '\t') )
                    {
                        --name_start_pos;
                        if (name_start_pos <= pos)
                            break;
                    }
                    
                    std::string child_name = stringtools::remove_begin_end_chars(str.substr( name_start_pos, token_pos.first-name_start_pos ));
//                     std::cout << "    " << child_name << std::endl;
                    if ( valid_token_names.find(child_name) != valid_token_names.end() )
                    {
                        root->add_child( child_name, child );
//                         [child_name] = child;
                    }
                    else
                        root->add_child( child );
//                         root->unnamed_children.push_back(child);
            //         push_back(child);
            //         std::cout << "   CHILD NAME = " << child->name << std::endl;
                    
                    pos = token_pos.second+1;
                }
                
                
            //     std::cout << str.substr( start_pos, end_pos ) << std::endl;
                root->content = stringtools::remove_begin_end_chars(str.substr( start_pos, end_pos-start_pos-1 ));
            //     std::cout << "CONTENT = " << root->content << std::endl;
                
                return root;
            }
        }
        
    }
    
    
}


#endif