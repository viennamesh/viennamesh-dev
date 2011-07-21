/* =============================================================================
   Copyright (c) 2010, 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                ViennaUtils - The Vienna Utilities Library
                             -----------------

   authors:    Josef Weinbub                      weinbub@iue.tuwien.ac.at
               Markus Bina                        bina@iue.tuwien.ac.at

   license:    see file LICENSE in the base directory
============================================================================= */


#ifndef VIENNAUTILS_XML_PUGIXML_HPP
#define VIENNAUTILS_XML_PUGIXML_HPP

// local includes
#include "exceptions.hpp"

namespace viennautils
{

//! pugixml wrapper class
struct pugixml
{
   //! meta comuting part
   typedef boost::shared_ptr<pugi::xml_document>   xml_doc_type;

   //! default constructor
   pugixml() : indent_string("  ") 
   {
      this->xml = xml_doc_type( new pugi::xml_document() );
   }

   //! constructor
   pugixml(const std::string & filename, const std::string indent = "  ") : indent_string(indent)
   {
      this->xml = xml_doc_type( new pugi::xml_document() );
      this->read(filename);
   }

   //! default destructor
   ~pugixml() {  }

   //! read an input file
   void read(std::ifstream& stream)
   {
      pugi::xml_parse_result result = this->xml->load(stream);
   }

   //! read an input file
   void read(const std::string & filename)
   {
      pugi::xml_parse_result result = this->xml->load_file(filename.c_str());
   }

   //! write an input file
   void write(std::ofstream& ostream)
   {
      this->dump(ostream);
   }

   //! write an input file
   void write(const std::string & filename)
   {
      std::ofstream stream(filename.c_str());
      this->dump(stream);
      stream.close();
   }

   //! dump the whole tree to the stream
   void dump(std::ostream & stream = std::cout)
   {
      this->xml->save(stream, this->indent_string.c_str());
   }

   //! perform a query and return the query result
   std::string query(std::string const& xpath_query_str)
   {
      std::stringstream result_stream;
      try
      {
         pugi::xpath_node_set query_result = this->xml->select_nodes(xpath_query_str.c_str());
         for(size_t i = 0; i < query_result.size(); i++)
         {
           query_result[i].node().print(result_stream, this->indent_string.c_str());
         }
      }
      catch (const pugi::xpath_exception& e)
      {
         std::cerr << "Exception caught in XmlReader::query ->  " << e.what() << std::endl;
         throw XmlQueryException("Exception in XmlReader::query ->  " + std::string(e.what()));
      }
      std::string result = result_stream.str();
      boost::trim(result);  // remove front/trailing whitespaces
      return result;
   }

   //! perform a query and return the query result
   std::string const query(std::string const& xpath_query_str) const
   {
      std::stringstream result_stream;
      try
      {
         pugi::xpath_node_set query_result = this->xml->select_nodes(xpath_query_str.c_str());
         for(size_t i = 0; i < query_result.size(); i++)
         {
           query_result[i].node().print(result_stream, this->indent_string.c_str());
         }
      }
      catch (const pugi::xpath_exception& e)
      {
         std::cerr << "Exception caught in XmlReader::query ->  " << e.what() << std::endl;
         throw XmlQueryException("Exception in XmlReader::query ->  " + std::string(e.what()));
      }
      std::string result = result_stream.str();
      boost::trim(result);  // remove front/trailing whitespaces
      return result;
   }

   //! the master xml object
   xml_doc_type   xml;

   //! string which contains the whitespaces for identation
   std::string    indent_string;
};
} //namespace viennautils  


#endif
