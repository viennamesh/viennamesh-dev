/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at
     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_BOOST_PYTHON
#define GSSE_BOOST_PYTHON

// *** system includes
//
#include <string>
#include <vector>
#include <map>
#include <iostream>

// *** BOOST includes
//
#include <boost/python.hpp>




namespace gsse {

struct python_persistency
{
   python_persistency() 
   {
      // changed to skip installation of signal handlers [PS]
      //
      Py_InitializeEx(0);
      // Py_Initialize();
      //
      char string_main[]="__main__";
      boost::python::object main_module((boost::python::handle<>(boost::python::borrowed(PyImport_AddModule(string_main)))));
      main_namespace = main_module.attr("__dict__");
   };

   python_persistency(std::string file_name) 
   {
      // changed to skip installation of signal handlers [PS]
      //
      Py_InitializeEx(0);
      // Py_Initialize();
      //
      char string_main[]="__main__";
      boost::python::object main_module((boost::python::handle<>(boost::python::borrowed(PyImport_AddModule(string_main)))));
      main_namespace = main_module.attr("__dict__");
      FILE* file = fopen(file_name.c_str(),"r");
      if (file == 0)
      {
         std::cerr << "no file of name: " << file_name << std::endl;
         exit(-1);
      }
      PyRun_SimpleFile(file, file_name.c_str());
   };

   ~python_persistency() 
   {
      std::cerr << "party's over, the snake just left" << std::endl;
      Py_Finalize();
   };

//   template<typename ret_type> ret_type operator[](std::string query)
//   {
//      try
//      {
//         return boost::python::extract<ret_type>(main_namespace[query.c_str()]);
//      }
//      catch(boost::python::error_already_set)
//      {
//         std::cerr << "it is the sad duty of this humble message to inform you that:" << std::endl;
//         PyErr_Print();
//      }
//   };

   double operator[](std::string query)
   {
      try
      {
         return boost::python::extract<double>(main_namespace[query.c_str()]);
      }
      catch(boost::python::error_already_set)
      {
         std::cerr << "it is the sad duty of this humble message to inform you that:" << std::endl;
         PyErr_Print();
      }
      // stop the compiler from bitching ...
      return 0;
   };

//   std::string operator[](std::string query)
//   {
//      try
//      {
//         return boost::python::extract<std::string>(main_namespace[query.c_str()]);
//      }
//      catch(boost::python::error_already_set)
//      {
//         std::cerr << "it is the sad duty of this humble message to inform you that:" << std::endl;
//         PyErr_Print();
//      }
//      // stop the compiler from bitching ...
//      return 0;
//   };
   
   void file(std::string file_name)
   {
      FILE* file = fopen(file_name.c_str(),"r");
      if (file == 0)
      {
         std::cerr << "no file of name: " << file_name << std::endl;
         exit(-1);
      }
      PyRun_SimpleFile(file, file_name.c_str());      
   };

   void interactive()
   {
      PyRun_AnyFile(stdin, "<stdin>");
   };

   void inject()
   {

   };

private:
   boost::python::object main_namespace;
};

}  // namespace gsse
#endif
