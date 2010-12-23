/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at
     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_PYTHON_DOMAIN
#define GSSE_PYTHON_DOMAIN

// *** system includes
//
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <sstream>
#include <functional>

// *** BOOST includes
//
#include <boost/python.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/at.hpp>

// *** GSSE includes
//
#include "gsse/domain.hpp"
#include "gsse/gtl/selector.hpp"

#include "testing/creater/create_domain.hpp"

namespace gsse {
namespace python {

template<typename ItemT>
std::string item_string(const ItemT& i)
{
   std::stringstream s;
   s << i;
   return s.str();
}

template <typename DomainT>
struct python_domain_glue
{   
   typedef DomainT domain_type;
   
   typedef typename gsse::domain_traits<DomainT>::vertex_type  vertex_type;
   typedef typename gsse::domain_traits<DomainT>::edge_type    edge_type;
   typedef typename gsse::domain_traits<DomainT>::facet_type   facet_type;
   typedef typename gsse::domain_traits<DomainT>::cell_type    cell_type;
   typedef typename gsse::domain_traits<DomainT>::segment_type segment_type;
   typedef typename gsse::domain_traits<DomainT>::point_t      point_type;

   typedef typename gsse::domain_traits<DomainT>::storage_type storage_type;

   typedef typename gsse::domain_traits<DomainT>::segment_iterator segment_iterator;
   typedef typename gsse::domain_traits<DomainT>::cell_iterator    cell_iterator;
   typedef typename gsse::domain_traits<DomainT>::facet_iterator   facet_iterator;
   typedef typename gsse::domain_traits<DomainT>::edge_iterator    edge_iterator;
   typedef typename gsse::domain_traits<DomainT>::vertex_iterator  vertex_iterator;

   
   // input / output operations
   //
   static void read_domain(DomainT& target_domain, const std::string& filename)
   {
      std::cout << "reading: " << filename << std::endl;
      target_domain.read_file(filename);
   }

   static void write_domain(DomainT& domain, const std::string& filename)
   {
      std::cout << "writing: " << filename << std::endl;
      domain.write_file(filename);
   }

   static void write_dx(DomainT& domain, const std::string& filename, const std::string& quan_name)
   {
      std::cout << "writing quantity " << quan_name << " to " << filename << std::endl;
      domain.write_dx_file(filename, quan_name);
   }

   // domain quantities
   //
   static void store_domain_quan(DomainT& domain, const std::string& quan_name, storage_type& val)
   //static void store_domain_quan(DomainT& domain, const std::string& quan_name, double d)
   {
      domain.store_domain_quantity(quan_name, val); // typename gsse::domain_traits<DomainT>::storage_type(1, 1, d));
   }

   static double retrieve_domain_quan(DomainT& domain, const std::string& quan_name)
   {
      typename gsse::domain_traits<DomainT>::storage_type quan;
      domain.retrieve_domain_quantity(quan_name, quan);
      return quan(0,0);
   }

   // manipulations of quantities stored on elements 
   //
   template<typename ElementT>
   //void store_element_quan(DomainT& domain, ElementT& e, std::string quan_name, typename gsse::domain_traits<DomainT>::storage_type& val)
   //static void store_element_quan(DomainT& domain, segment_type& segment, const ElementT& e, const std::string& quan_name, double v)
   //static void store_element_quan(DomainT& domain, const ElementT& e, const std::string& quan_name, double v)
   static void store_element_quan(DomainT& domain, segment_type& segment, const ElementT& e, const std::string& quan_name, storage_type& val)
   {
      segment.add_vertex_quantity(quan_name);
      // typename gsse::domain_traits<DomainT>::storage_type val(1,1,v);
      domain.store_quantity(e, quan_name, val);
   }

   template<typename ElementT>
   static double retrieve_element_quan(DomainT& domain, ElementT& e, std::string& quan_name)
   {
      typename gsse::domain_traits<DomainT>::storage_type quan;
      domain.retrieve_quantity(e,quan_name, quan);
      return quan(0,0);
   }
   
   template<typename ElementT>
   //   static double& retrieve_element_quan2(DomainT& domain, const ElementT& e, const std::string& quan_name)
   static storage_type& retrieve_element_quan2(DomainT& domain, const ElementT& e, const std::string& quan_name)
   {
      return domain(e, quan_name); //(0,0);
   }

   template<typename ElementT>
   static storage_type& assign_data(DomainT& domain, boost::python::tuple& key, storage_type& s)
   {
      // now see this! we get a tuple from python so we need to forcibly extract some info [PS]
      // 
      (boost::python::extract<segment_type&>(key[2])()).add_vertex_quantity(boost::python::extract<std::string>(key[1])());
      domain.store_quantity(boost::python::extract<ElementT&>(key[0])(), 
                            boost::python::extract<std::string>(key[1])(),
                            s);
      
      return domain(boost::python::extract<ElementT&>(key[0])(), boost::python::extract<std::string>(key[1])()); 
   }


   static void assign_data2(DomainT& domain, boost::python::tuple& key, storage_type& s)
   {
      // now see this! we get a tuple from python so we need to forcibly extract some info [PS]
      // 
//      (boost::python::extract<segment_type&>(key[2])()).add_vertex_quantity(boost::python::extract<std::string>(key[1])());
//      domain.store_quantity(boost::python::extract<vertex_type&>(key[0])(), 
//                            boost::python::extract<std::string>(key[1])(),
//                            s);
      (boost::python::extract<segment_type&>(key[0])()).add_vertex_quantity(boost::python::extract<std::string>(key[2])());
      domain.store_quantity(boost::python::extract<vertex_type&>(key[1])(), 
                            boost::python::extract<std::string>(key[2])(),
                            s);
   }


   // new domain quantity access is implemented here
   // perhaps move the domain_element to the iterator structs?
   //
   template<typename ElementT>
   struct domain_element
   {
      DomainT& domain;
      ElementT& element;
      domain_element(DomainT& domain, ElementT& element) : domain(domain), element(element) {} ;
   };

   // this is used to couple the element and the domain
   //
   template<typename ElementT>
   static domain_element<ElementT> get_element_quan(DomainT& domain, ElementT& element)
   {
      return domain_element<ElementT>(domain,element);
   }
   
   // the final query is made against the coupled object above 
   //
   template<typename ElementT>
   static storage_type& get_quantity_from_element(domain_element<ElementT> de, std::string name)
   {
      return de.domain(de.element, name);
   }

   static segment_type& get_segment_by_name(DomainT& domain, const std::string& name)
   {
      segment_iterator end(domain.segment_end());
      for (segment_iterator it(domain.segment_begin()); it != end; ++it)
         if ((*it).get_segment_name() == name)
            return (*it);

      // if we get to this there is no segment of the specified name
      // so we raise an exception and are done 
      // really using the *end result will segfault [PS]
      //
      std::string error("no segment named: " + name);
      PyErr_SetString(PyExc_KeyError, error.c_str()) ; 
      boost::python::throw_error_already_set();

      return (*end);
   }


   // maybe one day this will work ...
   //
   // template<typename ElementT>
   // struct quan_bind
   // {
   //    DomainT&     domain;
   //    ElementT     element;
   //    std::string  name;
   // 
   //    quan_bind(DomainT& domain, ElementT element, const char* name) : domain(domain), element(element), name(name) 
   //    {
   //       std::cout << "registering quantity " << name << std::endl;
   //       segment_iterator end(domain.segment_end());
   //       for (segment_iterator seg_it(domain.segment_begin()); seg_it!=end; seg_it++)
   //       {
   //          (*seg_it).add_vertex_quantity(name);
   //       }
   //    }
   //    
   //    storage_type& operator[](ElementT& element)
   //    {
   //       return domain(element, name);
   //    }
   // };
   // 
   //    template<typename ElementT>
   //    static void quan_bind_lvalue(quan_bind<ElementT>& qb, ElementT& element, storage_type& data)
   //    //static void quan_bind_lvalue(quan_bind<vertex_type>& qb, vertex_type& element, storage_type& data)
   //    {
   //       qb.domain.store_quantity(element, qb.name, data);
   //    }
   //
   //   template<typename ElementT>
   //   quan_bind<ElementT> quan_bind_gen(DomainT& domain, ElementT& element, const char* name)
   //   {
   //      return quan_bind<ElementT>(domain, element, name);
   //   }

   struct quan_bind_vertex
   {
      DomainT&     domain;
      vertex_type  element;
      std::string  name;

      quan_bind_vertex(DomainT& domain, vertex_type element, const char* name) : domain(domain), element(element), name(name) 
      {
         std::cout << "registering vertex quantity " << name << std::endl;
         segment_iterator end(domain.segment_end());
         for (segment_iterator seg_it(domain.segment_begin()); seg_it!=end; seg_it++)
         {
            (*seg_it).add_vertex_quantity(name);
         }
      }
      
      storage_type& operator[](vertex_type& element)
      {
         return domain(element, name);
      }
   };

   static quan_bind_vertex quan_bind_gen_vertex(DomainT& domain, vertex_type& element, const char* name)
   {
      return quan_bind_vertex(domain, element, name);
   }

   struct quan_bind_edge
   {
      DomainT&   domain;
      edge_type  element;
      std::string  name;

      quan_bind_edge(DomainT& domain, edge_type element, const char* name) : domain(domain), name(name) 
      {
         std::cout << "registering edge quantity " << name << std::endl;
         segment_iterator end(domain.segment_end());
         // typename boost::mpl::at<lookup, vertex_type>::type bbb(domain);
         for (segment_iterator seg_it(domain.segment_begin()); seg_it!=end; seg_it++)
         {
            (*seg_it).add_edge_quantity(name);
            // domain.init_edge_quantity(*seg_it, name, storage_type(1, 1, 0));
         }
      }
      
      storage_type& operator[](edge_type& element)
      {
         return domain(element, name);
      }
   };

   static quan_bind_edge quan_bind_gen_edge(DomainT& domain, edge_type& element, const char* name)
   {
      return quan_bind_edge(domain, element, name);
   }

   struct quan_bind_facet
   {
      DomainT&     domain;
      facet_type  element;
      std::string  name;

      quan_bind_facet(DomainT& domain, facet_type element, const char* name) : domain(domain), element(element), name(name) 
      {
         std::cout << "registering facet quantity " << name << std::endl;
         segment_iterator end(domain.segment_end());
         for (segment_iterator seg_it(domain.segment_begin()); seg_it!=end; seg_it++)
         {
            (*seg_it).add_facet_quantity(name);
         }
      }
      
      storage_type& operator[](facet_type& element)
      {
         return domain(element, name);
      }
   };

   static quan_bind_facet quan_bind_gen_facet(DomainT& domain, facet_type& element, const char* name)
   {
      return quan_bind_facet(domain, element, name);
   }

   struct quan_bind_cell
   {
      DomainT&     domain;
      cell_type  element;
      std::string  name;

      quan_bind_cell(DomainT& domain, cell_type element, const char* name) : domain(domain), element(element), name(name) 
      {
         std::cout << "registering cell quantity " << name << std::endl;
         segment_iterator end(domain.segment_end());
         for (segment_iterator seg_it(domain.segment_begin()); seg_it!=end; seg_it++)
         {
            (*seg_it).add_cell_quantity(name);
         }
      }
      
      storage_type& operator[](cell_type& element)
      {
         return domain(element, name);
      }
   };

   static quan_bind_cell quan_bind_gen_cell(DomainT& domain, cell_type& element, const char* name)
   {
      return quan_bind_cell(domain, element, name);
   }


   template<typename AccessT, typename ElementT>
   static void quan_bind_lvalue(AccessT& qb, ElementT& element, storage_type& data)
   //static void quan_bind_lvalue(quan_bind<vertex_type>& qb, vertex_type& element, storage_type& data)
   {
      qb.domain.store_quantity(element, qb.name, data);
   }

   template<typename AccessT, typename ElementT>
   static void quan_bind_lvalue_double(AccessT& qb, ElementT& element, double data)
   //static void quan_bind_lvalue(quan_bind<vertex_type>& qb, vertex_type& element, storage_type& data)
   {
      qb.domain.store_quantity(element, qb.name, storage_type(1,1,data));
   }


   // iterations
   //
   // do not change the name of the iterator typedefs or boost::python will complain (alot)
   // as it is used by internal mpl code
   // 
   // fulfil interface requirements (begin & end)
   // 
   
   struct iterate_segments
   {
      typedef segment_iterator iterator;
      
      DomainT& domain;
//      segment_type& segment;
      iterate_segments(DomainT& domain) : domain(domain) {}
//      iterate_segments(DomainT& domain, segment_type& segment) : domain(domain), segment(segment) {}
      
      iterator begin() { return domain.segment_begin();}
      iterator end() { return domain.segment_end(); }
   };

   struct iterate_cells_domain
   {
      typedef cell_iterator iterator;
      
      DomainT& domain;
      iterate_cells_domain(DomainT& domain) : domain(domain) {}
      
      iterator begin() { return domain.cell_begin();}
      iterator end() { return domain.cell_end(); }
   };


   struct storage_wrapper
   {
      storage_type internal;
      storage_wrapper(double in) : internal(1,1,in) {};
      operator storage_type() { return internal; };
   };

   static std::string quan_dump(const storage_type& q)
   {
      std::stringstream s;
      s << q ; //<< "data type" ;
      return s.str(); 
   }

//   static storage_type construct_quan(storage_type s, double d)
//   {
//      return storage_type(1,1,d);
//   }

   static storage_type construct_quan(double d)
   {
      return storage_type(1,1,d);
   }

   template<typename T>
   static storage_type quan_gen(T in)
   {
      return storage_type(1,1,in);
   }

   struct py_quan
   {
      storage_type& stored;
      operator storage_type&() { return stored; } ;
      operator double() { return stored(0,0); } ;
      py_quan(storage_type& t) : stored(t) {} ;
   };


   struct coordinates
   {
      point_type point;
      coordinates(DomainT& domain, const vertex_type& v) : point(domain.get_point(v)) {}
      const double& operator[](long index) { return point[index] ;}
   };
   
   // static void set_operation(coordinates& c, long index, double value) { c[index] = value; }

   // this function can be used to overload __get_attr__!! 
   // a string will not be matched by the type system, so use the char*
   //
   template<typename T>
   static void get_attr(T& t, char* s) 
   {
      std::cout << "__get_attr__ called " << s << std::endl;
   }

   // [RH] ############### can be removed from here ..
   //
   //
// [RH] replacement of iterator_sub_elements by gsse::traversal facility
//
// -->    gsse::TraversalObject   <->  typhoon::ElementT
// -->    gsse::TraversalSubType  <->  typhoon::IterationT
// 


//    typedef typename gsse::init_functor_switch
//    < 
//       TraversalObject,                    // TraversalObject  : e.g. domain
//       TraversalSubType                    // TraversalSubType : e.g. segment --> segment_on_domain 
//       >::type iterate_sub_elements;


   // this can stand alone (no dependence on domain)
   // so it is not part of the big bad struct [PS]   --> [RH] it is actually depending on topology properties .. now with domain_traits<>
   //
//    template<typename ElementT, typename IterationT> 
//    struct iterate_sub_elements
//    {
//       //
//       // [RH] .. use GSSEs internal mechanisms
//       //
//       typedef typename gsse::get_traversal_object<typename gsse::domain_traits<DomainT>::topology_t, IterationT, ElementT >::type  iterator;

//       // typedef typename boost::mpl::at<traversal_repository, boost::mpl::pair<ElementT, IterationT> >::type iterator;
//       // typedef typename boost::mpl::at<typename ElementT::traversal_repository, 
//       //                                 boost::mpl::pair<ElementT, IterationT> >::type iterator;

//       ElementT base;
//       iterate_sub_elements(ElementT& base) : base(base) {}
      
//       iterator begin() { return iterator(base); } 
      
//       // this is kind of a hack!   // ---> [RH] traversal_selector.hpp
//       // is there a better way?
//       //
//       iterator end() { return --(iterator(base));} 
//    };


   // multiplication for the storage_type (as long as it lags a cast operator)
   //
   static storage_type double_multiply_a(storage_type& st, double factor) { return st * factor;}
   static storage_type double_multiply_b(double factor, storage_type& st) { return st * factor;}

   // the purpose of these functions is to be overloaded at runtime in python 
   //
   static coordinates       fetch_coordinates(domain_type& domain, const vertex_type& v) { return (coordinates(domain, v));}
   static iterate_segments  fetch_segments(domain_type& domain)   { return (iterate_segments(domain)); }


// [RH] now with these two functions   ### final implementation ##
//
   template<typename ElementT, typename IterationT>
   static typename intrinsic_traversal<ElementT, IterationT>::type fetch_elements(ElementT& element)     { return typename intrinsic_traversal<ElementT, IterationT>::type(element);}

   template<typename ElementT, typename IterationT>
   static deduced_traversal<ElementT, IterationT>   fetch_sub_elements(ElementT& element)                { return deduced_traversal<ElementT, IterationT>(element);}


   // gsse::build_structured_domain(domain, ticks);


   // export all the goodies defined above to python with a single function call
   // this avoids nasty preprocessor macros
   //
   static void glue_it(const char* char_suffix)
   {
      // important NOTE: all classes should carry the suffix otherwise it WILL mess up overloads
      // so expose functions with constant name only!
      //
      const std::string suffix(char_suffix);

      // a few tags only exposed to keep python from complaining too much
      // can not be constructed from python (no_init)
      //
//      boost::python::class_<std::pair<domain_type, vertex_type> > (("vertex_tag_" + suffix).c_str(), boost::python::no_init);
//      boost::python::class_<std::pair<domain_type, edge_type> > (("edge_tag_" + suffix).c_str(), boost::python::no_init);
//      boost::python::class_<std::pair<domain_type, facet_type> > (("facet_tag_" + suffix).c_str(), boost::python::no_init);
//      boost::python::class_<std::pair<domain_type, cell_type> > (("cell_tag_" + suffix).c_str(), boost::python::no_init);

      boost::python::class_<domain_type> (("domain_" + suffix).c_str())
//         .def_readonly("vertex", std::pair<domain_type, vertex_type>())
//         .def_readonly("edge",   std::pair<domain_type, edge_type>())
//         .def_readonly("facet",  std::pair<domain_type, facet_type>())
//         .def_readonly("cell",   std::pair<domain_type, cell_type>())
         .def_readonly("vertex", vertex_type())
         .def_readonly("edge",   edge_type())
         .def_readonly("facet",  facet_type())
         .def_readonly("cell",   cell_type())
         .def("__getitem__", get_element_quan<vertex_type>) 
         .def("__getitem__", get_element_quan<cell_type>)   
         .def("__getitem__", get_element_quan<edge_type>)   
         .def("__getitem__", get_element_quan<facet_type>)
         .def("__setitem__", assign_data2);

      boost::python::class_<segment_type> (("segment_" + suffix).c_str());
      boost::python::class_<cell_type>    (("cell_" + suffix).c_str())
         .def("__str__", gsse::python::item_string<cell_type>);
      boost::python::class_<facet_type>   (("facet_" + suffix).c_str())
         .def("__str__", gsse::python::item_string<facet_type>);
      boost::python::class_<edge_type>    (("edge_" + suffix).c_str())
         .def("__str__", gsse::python::item_string<edge_type>);
      boost::python::class_<vertex_type>  (("vertex_" + suffix).c_str())
         .def("__str__", gsse::python::item_string<vertex_type>);
      

      // encapsulates a gsse::point_t and has access to the coordinates
      //
      boost::python::class_<coordinates> (("coordinates" + suffix).c_str(), boost::python::init<domain_type&, vertex_type&>())
         .def("__getitem__", &coordinates::operator[], boost::python::return_value_policy<boost::python::copy_const_reference>());
         // .def("__setitem__", &set_operation);
      
      boost::python::class_<quan_bind_vertex>(("quan_v" + suffix).c_str(), 
                                                      boost::python::init<domain_type&, vertex_type, const char*>() ) 
         .def("__getitem__", &quan_bind_vertex::operator[], boost::python::return_value_policy<boost::python::copy_non_const_reference>())
         .def("__setitem__", quan_bind_lvalue<quan_bind_vertex, vertex_type> )
         .def("__setitem__", quan_bind_lvalue_double<quan_bind_vertex, vertex_type> )
      ;
   
      boost::python::class_<quan_bind_edge>(("quan_e" + suffix).c_str(), 
                                            boost::python::init<domain_type&, edge_type, const char*>() ) 
         .def("__getitem__", &quan_bind_edge::operator[], boost::python::return_value_policy<boost::python::copy_non_const_reference>())
         .def("__setitem__", quan_bind_lvalue<quan_bind_edge, edge_type> )
         .def("__setitem__", quan_bind_lvalue_double<quan_bind_edge, edge_type> )
         ;
   
      boost::python::class_<quan_bind_facet>(("quan_f" + suffix).c_str(), 
                                             boost::python::init<domain_type&, facet_type, const char*>() ) 
         .def("__getitem__", &quan_bind_facet::operator[], boost::python::return_value_policy<boost::python::copy_non_const_reference>())
         .def("__setitem__", quan_bind_lvalue<quan_bind_facet, facet_type> )
         .def("__setitem__", quan_bind_lvalue_double<quan_bind_facet, facet_type> )
      ;
   
      boost::python::class_<quan_bind_cell>(("quan_c" + suffix).c_str(), 
                                            boost::python::init<domain_type&, cell_type, const char*>() ) 
         .def("__getitem__", &quan_bind_cell::operator[], boost::python::return_value_policy<boost::python::copy_non_const_reference>())
         .def("__setitem__", quan_bind_lvalue<quan_bind_cell, cell_type> )
         .def("__setitem__", quan_bind_lvalue_double<quan_bind_cell, cell_type> );
      ;

      // iteration capabilities
      //
      boost::python::class_<iterate_segments >(("segments_" + suffix).c_str(), boost::python::init<domain_type&>()) 
         .def("__iter__", boost::python::iterator<iterate_segments>() );
      // *** intrinsic traversal
      //
      boost::python::class_< typename intrinsic_traversal<segment_type, vertex_type>::type>(("vertices_" + suffix).c_str(), boost::python::init<segment_type&>()) 
         .def("__iter__", boost::python::iterator< typename intrinsic_traversal<segment_type, vertex_type>::type> () );
      boost::python::class_< typename intrinsic_traversal<segment_type, facet_type>::type>(("facets_" + suffix).c_str(), boost::python::init<segment_type&>()) 
         .def("__iter__", boost::python::iterator<typename intrinsic_traversal<segment_type, facet_type>::type> () );
      boost::python::class_< typename intrinsic_traversal<segment_type, edge_type>::type>(("edges_" + suffix).c_str(), boost::python::init<segment_type&>()) 
         .def("__iter__", boost::python::iterator<typename intrinsic_traversal<segment_type, edge_type>::type> () );
      boost::python::class_< typename intrinsic_traversal<segment_type, cell_type>::type>(("cells_" + suffix).c_str(), boost::python::init<segment_type&>()) 
         .def("__iter__", boost::python::iterator<typename intrinsic_traversal<segment_type, cell_type>::type> () );

      // *** deduced traversal
      //
      boost::python::class_<deduced_traversal<cell_type, vertex_type> >("voc", boost::python::init<cell_type&>()) 
         .def("__iter__", boost::python::iterator<deduced_traversal<cell_type, vertex_type> > () );
      boost::python::class_<deduced_traversal<cell_type, edge_type> >("eoc", boost::python::init<cell_type&>()) 
         .def("__iter__", boost::python::iterator<deduced_traversal<cell_type, edge_type> > () );
      boost::python::class_<deduced_traversal<cell_type, facet_type> >("foc", boost::python::init<cell_type&>()) 
         .def("__iter__", boost::python::iterator<deduced_traversal<cell_type, facet_type> > () );

      boost::python::class_<deduced_traversal<facet_type, vertex_type> >("vof", boost::python::init<facet_type&>()) 
         .def("__iter__", boost::python::iterator<deduced_traversal<facet_type, vertex_type> > () );
      boost::python::class_<deduced_traversal<facet_type, edge_type> >("eof", boost::python::init<facet_type&>()) 
         .def("__iter__", boost::python::iterator<deduced_traversal<facet_type, edge_type> > () );
      boost::python::class_<deduced_traversal<facet_type, cell_type> >("cof", boost::python::init<facet_type&>()) 
         .def("__iter__", boost::python::iterator<deduced_traversal<facet_type, cell_type> > () );

      boost::python::class_<deduced_traversal<edge_type, vertex_type> >("voe", boost::python::init<edge_type&>()) 
         .def("__iter__", boost::python::iterator<deduced_traversal<edge_type, vertex_type> > () );
      boost::python::class_<deduced_traversal<edge_type, facet_type> >("foe", boost::python::init<edge_type&>()) 
         .def("__iter__", boost::python::iterator<deduced_traversal<edge_type, facet_type> > () );
      boost::python::class_<deduced_traversal<edge_type, cell_type> >("coe", boost::python::init<edge_type&>()) 
         .def("__iter__", boost::python::iterator<deduced_traversal<edge_type, cell_type> > () );

      boost::python::class_<deduced_traversal<vertex_type, edge_type> >("eov", boost::python::init<vertex_type&>()) 
         .def("__iter__", boost::python::iterator<deduced_traversal<vertex_type, edge_type> > () );
      boost::python::class_<deduced_traversal<vertex_type, facet_type> >("fov", boost::python::init<vertex_type&>()) 
         .def("__iter__", boost::python::iterator<deduced_traversal<vertex_type, facet_type> > () );
      boost::python::class_<deduced_traversal<vertex_type, cell_type> >("cov", boost::python::init<vertex_type&>()) 
         .def("__iter__", boost::python::iterator<deduced_traversal<vertex_type, cell_type> > () );

      // expose gsse quantity type
      //
      boost::python::class_<storage_type>(("quan_" + suffix).c_str())
         .def("__str__", quan_dump)
         .def("__init__", construct_quan)
         .def(boost::python::self + boost::python::self)
         .def(boost::python::self - boost::python::self)
         .def(boost::python::self += boost::python::self)
         .def(boost::python::self == boost::python::self)
         .def(boost::python::self != boost::python::self)
         .def(boost::python::self * boost::python::self)
         .def(boost::python::self -= boost::python::self)
         .def("__mul__",  double_multiply_a)
         .def("__rmul__", double_multiply_b)
         ;

      boost::python::def("quan_gen", quan_gen<double>, "generate a quantity");


      // quantity storage / retrieval
      //
      boost::python::def("store_vertex_quan", store_element_quan<vertex_type>, "store a vertex quantity"); 
      boost::python::def("retrieve_vertex_quan", retrieve_element_quan2<vertex_type>, 
                         boost::python::return_value_policy<boost::python::copy_non_const_reference>(), "retrieve a vertex quantity"); 
      boost::python::def("store_edge_quan", store_element_quan<edge_type>, "store an edge quantity"); 
      boost::python::def("retrieve_edge_quan", retrieve_element_quan2<edge_type>, 
                         boost::python::return_value_policy<boost::python::copy_non_const_reference>(), "retrieve an edge quantity"); 
      boost::python::def("store_facet_quan", store_element_quan<facet_type>, "store a facet quantity"); 
      boost::python::def("retrieve_facet_quan", retrieve_element_quan2<facet_type>, 
                         boost::python::return_value_policy<boost::python::copy_non_const_reference>(), "retrieve a facet quantity"); 
      boost::python::def("store_cell_quan", store_element_quan<cell_type>, "store a cell quantity"); 
      boost::python::def("retrieve_cell_quan", retrieve_element_quan2<cell_type>, 
                         boost::python::return_value_policy<boost::python::copy_non_const_reference>(), "retrieve a cell quantity"); 
 
      boost::python::def("store_domain_quan", store_domain_quan, "store a domain quantity");
      boost::python::def("retrieve_domain_quan", retrieve_domain_quan, "retrieve a domain quantity");

      // quantity accessors
      //
      boost::python::def("quan", quan_bind_gen_vertex);
      boost::python::def("quan", quan_bind_gen_edge);
      boost::python::def("quan", quan_bind_gen_facet);
      boost::python::def("quan", quan_bind_gen_cell);


      // segments can be referenced by name
      //
      boost::python::def("segment", get_segment_by_name, boost::python::return_value_policy<boost::python::copy_non_const_reference>());

      // these returns iteration objects (interface is equivalent to be a python list)
      //
      boost::python::def("coordinates", fetch_coordinates); 
      boost::python::def("segments", fetch_segments);
      boost::python::def("segmentsb", fetch_segments);
      boost::python::def("vertices", fetch_elements<segment_type, vertex_type>);
      boost::python::def("edges",    fetch_elements<segment_type, edge_type>);
      boost::python::def("facets",   fetch_elements<segment_type, facet_type>);
      boost::python::def("cells",    fetch_elements<segment_type, cell_type>);

      boost::python::def("cells",    fetch_sub_elements<facet_type,  cell_type>);
      boost::python::def("cells",    fetch_sub_elements<edge_type,   cell_type>);
      boost::python::def("cells",    fetch_sub_elements<vertex_type, cell_type>);

      boost::python::def("facets",   fetch_sub_elements<cell_type,   facet_type>);
      boost::python::def("facets",   fetch_sub_elements<edge_type,   facet_type>);
      boost::python::def("facets",   fetch_sub_elements<vertex_type, facet_type>);

      boost::python::def("edges",    fetch_sub_elements<cell_type,   edge_type>);
      boost::python::def("edges",    fetch_sub_elements<facet_type,  edge_type>);
      boost::python::def("edges",    fetch_sub_elements<vertex_type, edge_type>);

      boost::python::def("vertices", fetch_sub_elements<cell_type,  vertex_type>);
      boost::python::def("vertices", fetch_sub_elements<facet_type, vertex_type>);
      boost::python::def("vertices", fetch_sub_elements<edge_type,  vertex_type>);


      // make the domain readble / writable
      //
      boost::python::def("read_domain", read_domain, "read an unstructured domain from file");
      boost::python::def("write_domain", write_domain, "write an unstructured domain to file");
      boost::python::def("write_dx", write_dx, "write domain and quantity to dx file");

   }

};

// [PS] this has to be seperate from the rest (at least for the moment)
// so far only a 1D structured domain can be constructed from python 
// attempting to include it, make the compiler very unhappy 
//
template<typename DomainT>
void build_structured_domain(DomainT& domain, long ticks, double start, double stop)
{
   typedef typename gsse::domain_traits<DomainT>::topology_t        topology_t;
   typedef typename gsse::domain_traits<DomainT>::segment_t         segment_t;
   typedef typename DomainT::pointlist_type                         pointlist_type;
   
   static const long dim = DomainT::dimension;
   
   std::vector<std::vector<double> > dimticks(dim);
   std::vector<int> dim_v;
   
   double delta = (stop - start) / ( ticks - 1.0);

   std::cout << "delta: " << delta << std::endl;
   for(int i=0; i<static_cast<int>(dim); ++i)
   {
      dim_v.push_back(ticks);
      for (int j=0; j < ticks; ++j)
         dimticks[i].push_back(start + j * delta); 
   }
   
   (*(domain.segment_begin())).retrieve_topology() = topology_t(dim_v.begin(), dim_v.end());
   
   domain.retrieve_pointlist() = pointlist_type(dimticks);
}

template<typename DomainT>
void build_structured_domain_2(DomainT& domain, const boost::python::list& ticks, const boost::python::list& start, const boost::python::list& end)
{
   typedef typename gsse::domain_traits<DomainT>::topology_t        topology_t;
   typedef typename gsse::domain_traits<DomainT>::segment_t         segment_t;
   typedef typename DomainT::pointlist_type                         pointlist_type;
   
   static const long dim = DomainT::dimension;

   long tick_count = boost::python::len(ticks);
   long start_count = boost::python::len(start);
   long end_count = boost::python::len(end);

   if ((tick_count != start_count) || (start_count != end_count) || (end_count != dim))
   {
      std::stringstream error;
      error << "dimension and data inconsistent -- requirement: " << dim << " == " << tick_count << " == " << start_count << " == " << end_count ;
      PyErr_SetString(PyExc_ValueError, error.str().c_str()) ; 
      boost::python::throw_error_already_set();
   }
   
   std::vector<std::vector<double> > dimticks(dim);
   std::vector<int> dim_v;

   for (long i(0); i < tick_count; ++i)
   {
      long local_ticks = (boost::python::extract<long>((ticks)[i]));
      double local_start = (boost::python::extract<double>((start)[i]));
      double local_end = (boost::python::extract<double>((end)[i]));

      double local_delta = (local_end - local_start) / (local_ticks - 1.0);

      // for(int i=0; i<static_cast<int>(dim); ++i)
      // {
      dim_v.push_back(local_ticks);
      for (long j=0; j < local_ticks; ++j)
         dimticks[i].push_back(local_start + j * local_delta); 
      // }
   }

   (*(domain.segment_begin())).retrieve_topology() = topology_t(dim_v.begin(), dim_v.end());
   
   domain.retrieve_pointlist() = pointlist_type(dimticks);
}


}} // close namespace gsse::python

#endif //  GSSE_PYTHON_DOMAIN
