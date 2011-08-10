/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_CONFIG_HPP
#define VIENNAMESH_CONFIG_HPP

#include "viennamesh/tags.hpp"
#include "viennautils/config.hpp"
#include "viennautils/convert.hpp"
#include "viennautils/messages.hpp"

namespace viennamesh {
namespace config {
// -----------------------------------------------------------------------------
struct set : viennautils::config<viennautils::tag::xml>::type {};
// -----------------------------------------------------------------------------
static void load(viennamesh::config::set& s, std::string const& filename)
{
   s.read(filename);
}
// -----------------------------------------------------------------------------
template<typename ViennaMeshObjT>
void assign(viennamesh::config::set const& paraset, ViennaMeshObjT& obj)
{
   obj.assign(paraset);
}
// -----------------------------------------------------------------------------
namespace query {
struct adaptor_maxsize
{
   typedef double result_type;
   
   static bool 
   available(viennamesh::config::set  const& paraset, 
             std::string              const& modulekey)
   {
      std::string query_result = paraset.query
      ("/control/adaptors/adaptor[key=\""+modulekey+"\"]/paras/para[key=\"maxsize\"]/val/text()");

      if(query_result == "") return false;
      else                   return true;
   }
   
   static result_type
   eval(viennamesh::config::set  const& paraset, 
        std::string              const& modulekey)
   {
      std::string query_result = paraset.query
      ("/control/adaptors/adaptor[key=\""+modulekey+"\"]/paras/para[key=\"maxsize\"]/val/text()");

      if(query_result == "") {
         viennautils::msg::error("ViennaMesh::Adaptor::Query::MaxSize - not successful ..");
         throw;
      }
      
      return viennautils::convert<result_type>()(query_result);
   }
};
struct adaptor_minsize
{
   typedef double result_type;
   
   static bool 
   available(viennamesh::config::set  const& paraset, 
             std::string              const& modulekey)
   {
      std::string query_result = paraset.query
      ("/control/adaptors/adaptor[key=\""+modulekey+"\"]/paras/para[key=\"minsize\"]/val/text()");

      if(query_result == "") return false;
      else                   return true;
   }
   
   static result_type
   eval(viennamesh::config::set  const& paraset, 
        std::string              const& modulekey)
   {
      std::string query_result = paraset.query
      ("/control/adaptors/adaptor[key=\""+modulekey+"\"]/paras/para[key=\"minsize\"]/val/text()");

      if(query_result == "") {
         viennautils::msg::error("ViennaMesh::Adaptor::Query::MinSize - not successful ..");
         throw;
      }
      
      return viennautils::convert<result_type>()(query_result);
   }
};
struct adaptor_maxangle
{
   typedef double result_type;
   
   static bool 
   available(viennamesh::config::set  const& paraset, 
             std::string              const& modulekey)
   {
      std::string query_result = paraset.query
      ("/control/adaptors/adaptor[key=\""+modulekey+"\"]/paras/para[key=\"maxangle\"]/val/text()");

      if(query_result == "") return false;
      else                   return true;
   }
   
   static result_type
   eval(viennamesh::config::set  const& paraset, 
        std::string              const& modulekey)
   {
      std::string query_result = paraset.query
      ("/control/adaptors/adaptor[key=\""+modulekey+"\"]/paras/para[key=\"maxangle\"]/val/text()");

      if(query_result == "") {
         viennautils::msg::error("ViennaMesh::Adaptor::Query::MaxAngle - not successful ..");
         throw;
      }
      
      return viennautils::convert<result_type>()(query_result);
   }
};
struct adaptor_grading
{
   typedef double result_type;
   
   static bool 
   available(viennamesh::config::set  const& paraset, 
             std::string              const& modulekey)
   {
      std::string query_result = paraset.query
      ("/control/adaptors/adaptor[key=\""+modulekey+"\"]/paras/para[key=\"grading\"]/val/text()");

      if(query_result == "") return false;
      else                   return true;
   }   
   
   static result_type
   eval(viennamesh::config::set  const& paraset, 
        std::string              const& modulekey)
   {
      std::string query_result = paraset.query
      ("/control/adaptors/adaptor[key=\""+modulekey+"\"]/paras/para[key=\"grading\"]/val/text()");

      if(query_result == "") {
         viennautils::msg::error("ViennaMesh::Adaptor::Query::Grading - not successful ..");
         throw;
      }
      
      return viennautils::convert<result_type>()(query_result);
   }
};
} // end namespace query
// -----------------------------------------------------------------------------
} // end namespace config
} // end namespace viennamesh

#endif




