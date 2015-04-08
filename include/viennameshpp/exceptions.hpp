#ifndef VIENNAMESH_CORE_EXCEPTIONS_HPP
#define VIENNAMESH_CORE_EXCEPTIONS_HPP

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

#include <exception>
#include <stdexcept>

namespace viennamesh
{

//   namespace exception
//   {
//
//     class base_exception : public std::runtime_error
//     {
//     public:
//       base_exception(std::string const & message_) : std::runtime_error(message_) {}
//       virtual ~base_exception() {}
//     };
//
//
//
//     class input_parameter_not_found : public base_exception
//     {
//     public:
//       input_parameter_not_found(std::string const & message_) : base_exception(message_) {}
//       virtual ~input_parameter_not_found() throw() {}
//     };
//
//     class input_parameter_not_of_requested_type_and_not_convertable : public base_exception
//     {
//     public:
//       input_parameter_not_of_requested_type_and_not_convertable(std::string const & message_) : base_exception(message_) {}
//       virtual ~input_parameter_not_of_requested_type_and_not_convertable() throw() {}
//     };
//
// 
//
//     class create_sizing_function : public base_exception
//     {
//     public:
//       create_sizing_function(std::string const & message_) : base_exception(message_) {}
//       virtual ~create_sizing_function() throw() {}
//     };
//   }


}

#endif
