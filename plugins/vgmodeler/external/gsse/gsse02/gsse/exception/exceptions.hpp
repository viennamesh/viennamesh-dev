/* ============================================================================
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_EXCEPTIONS_HH)
#define GSSE_EXCEPTIONS_HH

// *** system includes
#include <iostream>
#include <fstream>
#include <cmath>

// *** BOOST includes
#include <boost/lexical_cast.hpp>


namespace gsse
{


///////////////////////////////////////////////////////////////////////////
/// @brief GSSE exception
///
/// Base functionality for gsse exceptions
///
///////////////////////////////////////////////////////////////////////////

struct gsse_exception : public std::exception {
  virtual ~gsse_exception() throw() {}
  virtual const char* what() const throw() = 0;
};


///
/// @brief is thrown, if conversion to a given datatype is not possible
///
struct  invalid_datatype_conversion: public gsse_exception {
  std::string which_dt;
  mutable std::string statement;

  invalid_datatype_conversion(const std::string& wq) :
     which_dt(wq), statement("") {}

  virtual ~invalid_datatype_conversion() throw() {}
  const char* what() const throw() {
    if(statement.empty())
      statement =
        std::string("Invalid datatype conversion. Datatype ("+which_dt+") cannot be converted. \n");
    return statement.c_str();
  }
};


///
/// @brief is thrown, if a quantity name is not valid
///
struct  invalid_quantity_name: public gsse_exception {
  std::string which_quan;
  mutable std::string statement;

  invalid_quantity_name(const std::string& wq) :
     which_quan(wq), statement("") {}

  virtual ~invalid_quantity_name() throw() {}
  const char* what() const throw() {
    if(statement.empty())
      statement =
        std::string("Invalid quantity name. This name ("+which_quan+") was not added before. \n");
    return statement.c_str();
  }
};



///
/// @brief is thrown, if a key is not valid
///
struct  invalid_key: public gsse_exception {
  std::string which_key;
  mutable std::string statement;

   template<typename Key>
   invalid_key(const Key& key) :statement("") 
   {
      which_key = (boost::lexical_cast<std::string>(key));
   }
   virtual ~invalid_key() throw() {}

   const char* what() const throw() {
      if(statement.empty())
         statement =
            std::string("Invalid key. This key ("+which_key+") was not added before. \n");
      return statement.c_str();
   }
};



///
/// @brief is thrown, if an object index is not valid
///
struct  invalid_object_index: public gsse_exception {
  std::string wq;
  mutable std::string statement;

  template<typename ObjectIndexType>
  invalid_object_index(const ObjectIndexType& wq) 
  {
    //wq(boost::lexical_cast<std::string>(wq));
  }

  virtual ~invalid_object_index() throw() {}
  const char* what() const throw() {
    if(statement.empty())
      statement =
        std::string("Invalid object index. The index ("+wq+") is not valid. \n");
    return statement.c_str();
  }
};

///
/// @brief is thrown, if an object index is not valid
///
struct  invalid_index: public gsse_exception {
  std::string         error;
  mutable std::string statement;

  template<typename Index>
  invalid_index(const Index& index) 
  {
     error = (boost::lexical_cast<std::string>(index));
  }

  virtual ~invalid_index() throw() {}
  const char* what() const throw() {
    if(statement.empty())
      statement =
        std::string("Invalid index. The index ("+error+") is not valid. \n");
    return statement.c_str();
  }
};



///
/// @brief is thrown, if an object index is not valid
///
struct  invalid_program_path: public gsse_exception {
  mutable std::string statement;

  invalid_program_path() {}

  virtual ~invalid_program_path() throw() {}
  const char* what() const throw() {
    if(statement.empty())
      statement =
        std::string("Invalid program path.. This should not happen \n");
    return statement.c_str();
  }
};


///
/// @brief is thrown, if the dimensionality is wrong
///
struct  dimension_problem: public gsse_exception {
  std::string dim_prob;
  mutable std::string statement;

   dimension_problem() {}
  dimension_problem(const std::string& str) :
     dim_prob(str) {}


  virtual ~dimension_problem() throw() {}
  const char* what() const throw() {
    if(statement.empty())
      statement =
        std::string("Dimension problem: " + dim_prob);
    return statement.c_str();
  }
};

///
/// @brief is thrown, if a file is not found
///
struct  file_not_found: public gsse_exception {
  std::string dim_prob;
  mutable std::string statement;

  file_not_found () {}
  file_not_found (const std::string& str) :
     dim_prob(str) {}


  virtual ~file_not_found() throw() {}
  const char* what() const throw() {
    if(statement.empty())
      statement =
        std::string("file not found: " + dim_prob);
    return statement.c_str();
  }
};



///
/// @brief is thrown, if a search within a container was not successfull
///
struct  search_not_successfull: public gsse_exception {
  std::string where_except;
  mutable std::string statement;

  search_not_successfull(const std::string& wq) :
    where_except(wq) {}

  virtual ~search_not_successfull() throw() {}
  const char* what() const throw() {
    if(statement.empty())
      statement =
        std::string("A search within a container was not successfull. The search is within: ("+where_except+")  \n");
    return statement.c_str();
  }
};




///
/// @brief is thrown, if a mesher/triangle error occurs
///
struct  mesher_triangle_error: public gsse_exception {
  std::string where_except;
  mutable std::string statement;

  mesher_triangle_error(const std::string& wq) :
    where_except(wq) {}

  virtual ~mesher_triangle_error() throw() {}
  const char* what() const throw() {
    if(statement.empty())
      statement =
        std::string("GSSE::meshing:: triangle error with ("+where_except+")  \n");
    return statement.c_str();
  }
};


///
/// @brief is thrown, if a numerical calculation is invalid / determinant to small
///
struct  numerical_calculation_error: public gsse_exception {
  std::string where_except;
  mutable std::string statement;

  numerical_calculation_error(const std::string& wq) :
    where_except(wq) {}

  virtual ~numerical_calculation_error() throw() {}
  const char* what() const throw() {
    if(statement.empty())
      statement =
        std::string("GSSE::numerical:: calculation error with ("+where_except+")  \n");
    return statement.c_str();
  }
};


}   // namespace gsse 

#endif
