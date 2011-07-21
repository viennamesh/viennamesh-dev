/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at
     Copyright (c) 2005-2006 Michael Spevak

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_IOMETHODS_DOMAIN_HH_ID
#define GSSE_IOMETHODS_DOMAIN_HH_ID

// *** system includes
//
#include <vector>
#include <map>


// *** extern dependencies
//
#include <libxml++/libxml++.h>


namespace gsse
{
namespace detail_domain
{


void read_node(const xmlpp::Node* node, std::vector<std::string>& node_vector, int level)
{
  const xmlpp::ContentNode* nodeContent = dynamic_cast<const xmlpp::ContentNode*>(node);
  const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(node);
  const xmlpp::CommentNode* nodeComment = dynamic_cast<const xmlpp::CommentNode*>(node);

  if(nodeText && nodeText->is_white_space()) //Let's ignore the indenting - you don't always want to do this.
     return;
    
  //std::cout << "read node.. output.. level:" << level << std::endl;


  std::string nodename (node->get_name());

  if(!nodeText && !nodeComment && !nodename.empty()) //Let's not say "name: text".
  {
     //std::cout << "Node name = " << node->get_name() << std::endl;
    node_vector.push_back(node->get_name());
    //std::cout << "Node name = " << nodename << std::endl;
  }
  else if(nodeText) //Let's say when it's text. - e.g. let's say what that white space is.
  {
     //std::cout << "Text Node" << std::endl;
  }




  //Treat the various node types differently: 
  if(nodeText)
  {
     //std::cout << "text = \"" << nodeText->get_content() << "\"" << std::endl;
    node_vector.push_back(nodeText->get_content() );
  }
  else if(nodeComment)
  {
     //std::cout << "comment = " << nodeComment->get_content() << std::endl;
  }
  else if(nodeContent)
  {
     //std::cout << "content = " << nodeContent->get_content() << std::endl;
  }
  else if(const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(node))
  {
    //A normal Element node:

    //line() works only for ElementNodes.
    //std::cout << "     line = " << node->get_line() << std::endl;

    //Print attributes:
    const xmlpp::Element::AttributeList& attributes = nodeElement->get_attributes();
    for(xmlpp::Element::AttributeList::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter)
    {
      const xmlpp::Attribute* attribute = *iter;
      //std::cout << "  Attribute " << attribute->get_name() << " = " << attribute->get_value() << std::endl;
      //node_vector.push_back(attribute->get_name());
      node_vector.push_back(attribute->get_value());
    }

    const xmlpp::Attribute* attribute = nodeElement->get_attribute("title");
    if(attribute)
    {
       //std::cout << "title found: =" << attribute->get_value() << std::endl;
      
    }
  }


  
  if(!nodeContent)
  {
    //Recurse through child nodes:
    xmlpp::Node::NodeList list = node->get_children();
    for(xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
    {
       read_node(*iter, node_vector, level+1 ); //recursive
    }
  }
}


int convert_vector_2_map(std::vector<std::string>& node_vector, 
                         std::vector<std::map<std::string, std::string> >& token_vector,
                         std::string start_information, int number_of_elements)
{
    std::map<std::string, std::string> token_map;

//     for (unsigned int i = 0; i < node_vector.size(); ++i)
//     {
//        std::cout << node_vector[i] <<std::endl;
//     }
//     std::cout << std::endl;


   for (unsigned int i = 0; i < node_vector.size(); ++i)
   {
      // start processing with the right start_token
      //
      //std::cout << "## startinformation: " << start_information << std::endl;
      //std::cout << "## node_vector: " << node_vector[i] << std::endl;

      if (node_vector[i] == start_information  &&  
          start_information !="segmentinformation"  && 
          start_information !="domaininformation")  
      {
         token_map["mainname"]=node_vector[i+1];
         
         unsigned int temp_upper = i+2 + number_of_elements;
         for (unsigned int go_on = i+2; go_on < temp_upper; go_on+=2, i+=2)
         {
            //std::cout << "## node_vector: " << node_vector[go_on] << std::endl;

            token_map[node_vector[go_on]]=node_vector[go_on+1];

            // fileinput or inline information
            if (node_vector[go_on] == "inputsource")
            {
               token_map["fileinformation"]=node_vector[go_on+2];
               i+=2;
            }
         }
         token_vector.push_back(token_map);
      }
      else if (node_vector[i] == start_information  && 
               (start_information =="segmentinformation"  ||  start_information =="domaininformation"))  
      {
         token_map["mainname"]=node_vector[i+1];
         int go_on=i+2;;
         while (node_vector[go_on] == "toponame"  || 
                node_vector[go_on] == "quanname" )
         {
            //std::cout << "1:: " << node_vector[go_on] << "   2:" << node_vector[go_on+1] << std::endl;
            token_map[node_vector[go_on]]=node_vector[go_on+1];

            go_on+=2;
            i+=2;
         }
         token_vector.push_back(token_map);

      }

   }
   

   return 0;
}


int convert_vector_2_vector(std::vector<std::string>& node_vector, 
                            std::vector<std::vector<std::pair<std::string, std::string> > >& token_vector,
                            std::string start_information)
{
//     for (unsigned int i = 0; i < node_vector.size(); ++i)
//     {
//        std::cout << node_vector[i] <<std::endl;
//     }
//     std::cout << std::endl;

   std::vector<std::pair<std::string, std::string> > temp_token_vector;

   for (unsigned int i = 0; i < node_vector.size(); ++i)
   {
      // start processing with the right start_token
      //
      //std::cout << "## startinformation: " << start_information << std::endl;
      //std::cout << "## node_vector: " << node_vector[i] << std::endl;

      if (node_vector[i] == start_information  && start_information=="segmentinformation")  
      {
         temp_token_vector.push_back(std::pair<std::string, std::string>("mainname", node_vector[i+1]));

         int go_on=i+2;
         while (node_vector[go_on] == "toponame"  || 
                node_vector[go_on] == "quanname" )
         {
            //std::cout << "1:: " << node_vector[go_on] << "   2:" << node_vector[go_on+1] << std::endl;
            temp_token_vector.push_back(std::pair<std::string, std::string>(node_vector[go_on], node_vector[go_on+1]));

            go_on+=2;
            i+=2;
         }
         token_vector.push_back(temp_token_vector);
      }

   }
   

   return 0;
}


int process_map(std::vector<std::map<std::string, std::string> >& node_vector)
{
   for (unsigned int i = 0; i < node_vector.size(); ++i)
   {
      std::map<std::string, std::string>::iterator mit;
      for (mit = node_vector[i].begin(); mit != node_vector[i].end(); ++mit)
      {
         std::cout << (*mit).first << "  ::: " << (*mit).second <<std::endl;
      }
      std::cout << std::endl;
   }

   return 0;
}

int process_vector(std::vector<std::vector<std::pair<std::string, std::string> > >& node_vector)
{
   for (unsigned int i = 0; i < node_vector.size(); ++i)
   {
      for (unsigned int j = 0; j < node_vector[i].size(); ++j)
      {
      
         std::cout << node_vector[i][j].first << "  ::: " << node_vector[i][j].second <<std::endl;
      }
      std::cout << std::endl;
   }

   return 0;
}


}
}

#endif
