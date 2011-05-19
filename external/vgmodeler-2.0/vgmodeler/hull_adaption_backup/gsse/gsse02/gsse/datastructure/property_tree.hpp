/* ============================================================================
   Copyright (c) 2004-2010 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GSSE_PROPERTY_TREE_200802
#define GSSE_PROPERTY_TREE_200802

// ########### [RH]  maybe replaced by boost::property_tree ########
// 
// *** system includes
#include <algorithm>
#include <map>
// *** BOOST includes
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
// *** GSSE includes
#include "gsse/exception/exceptions.hpp"

// ############################################################
//

namespace gsse {  namespace property {


template<typename StorageT = std::string>
struct property_tree
{

	std::map<std::string, StorageT>& operator[](std::string quan_name) { return main_container[quan_name]; }
//	std::map<std::string, StorageT>& operator[](std::string quan_name) { return main_container[quan_name]; } // const part

private:
   std::map<std::string, std::map<std::string, StorageT> > main_container; 
};



bool is_empty(const boost::any & operand)
{
    return operand.empty();
}

bool is_int(const boost::any & operand)
{
    return operand.type() == typeid(int);
}

bool is_long(const boost::any & operand)
{
    return operand.type() == typeid(long);
}
bool is_long(const std::string& operand)
{
  bool is_truly_long = true;
  for (size_t cnt = 0; cnt < operand.size(); ++cnt)
    {

      if ((operand[cnt]<'0') || (operand[cnt]>'9'))
	{
	  is_truly_long = false;
	  break;
	}
    }
  return is_truly_long;
}

bool is_double(const boost::any & operand)
{
    return operand.type() == typeid(double);
}
bool is_double(const std::string& operand)
{
  bool is_truly_duble = true;
  bool has_dot        = false;
  for (size_t cnt = 0; cnt < operand.size(); ++cnt)
    {
      if ((operand[cnt]!='.') && ( (operand[cnt]<'0') || (operand[cnt]>'9')))
	{
	  is_truly_duble = false;
	  break;
	}
      if (operand[cnt] == '.')
	has_dot = true;
    }
  return is_truly_duble && has_dot;
}


bool is_float_l(const boost::any & operand)
{
    return operand.type() == typeid(float);
}

bool is_char_ptr(const boost::any & operand)
{
    try
    {
       boost::any_cast<const char *>(operand);
        return true;
    }
    catch(const boost::bad_any_cast &)
    {
        return false;
    }
}

bool is_string(const boost::any & operand)
{
   return boost::any_cast<std::string>(&operand);
}


// ===============================

double to_double(const boost::any & operand)
{
   if (! is_double(operand))
   {
      std::cerr << "##### operand is not convertible to DOUBLE .. " << std::endl;
      throw gsse::invalid_datatype_conversion("double");
//      exit(-1);
   }
   double val = boost::any_cast<double>(operand);
   return val;
}


float to_float(const boost::any & operand)
{
   if (! is_float_l(operand))
   {
      std::cerr << "##### operand is not convertible to FLOAT .. " << std::endl;
      throw gsse::invalid_datatype_conversion("float");
//      exit(-1);
   }
   float val = boost::any_cast<float>(operand);
   return val;
}

std::string to_string(const boost::any & operand)
{
   if (! is_string(operand))
   {
      std::cerr << "##### operand is not convertible to STRING .. " << std::endl;
      throw gsse::invalid_datatype_conversion("std::string");
//      exit(-1);
   }
   std::string val = boost::any_cast<std::string>(operand);
   return val;
}

int to_int(const boost::any & operand)
{
   if (! is_int(operand))
   {
      std::cerr << "##### operand is not convertible to INT .. " << std::endl;
      throw gsse::invalid_datatype_conversion("int");
//      exit(-1);
   }
   int val = boost::any_cast<int>(operand);
   return val;
}

long to_long(const boost::any & operand)
{
   if (! is_long(operand))
   {
      std::cerr << "##### operand is not convertible to LONG .. " << std::endl;
      throw gsse::invalid_datatype_conversion("long");
//      exit(-1);
   }
   long val = boost::any_cast<long>(operand);
   return val;
}


long to_long(const std::string& operand)
{
  long val = boost::lexical_cast<long>(operand);
  return val;
}
double to_double(const std::string& operand)
{
  double val = boost::lexical_cast<double>(operand);
  return val;
}


} // namespace property
} //namespace gsse


#endif
