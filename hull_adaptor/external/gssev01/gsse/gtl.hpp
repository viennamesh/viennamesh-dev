/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_GTL_MAIN_INCLUDE_20080324
#define GSSE_GTL_MAIN_INCLUDE_20080324


// *** system includes
//
#include <algorithm>
#include <functional>

// *** BOOST includes
//
// [RH] adaptation to boost >= 1.37
#include <boost/lambda/lambda.hpp>
#include <boost/spirit/home/phoenix.hpp>   

// old boost < 1.37
// #include <boost/spirit/phoenix/operator.hpp>    // V2
// #include <boost/spirit/phoenix/scope.hpp>
// #include <boost/spirit/phoenix/statement.hpp>
// #include <boost/spirit/phoenix/scope/local_variable.hpp>
// #include <boost/spirit/phoenix/bind/bind_member_function.hpp>


// *** GSSE includes
// 
#include "gsse/gtl/for_each.hpp"   // [RH][TODO] .. to be deleted
#include "gsse/gtl/traverse.hpp"
#include "gsse/gtl/orient.hpp"
#include "gsse/gtl/quan_access.hpp"
//#include "gsse/lambda/quan_access_linearized.hpp"   // [RH][TODO] .. to be deleted
#include "gsse/debug_meta.hpp"   // [RH][TODO][temporary]




namespace gsse
{

struct insert_impl
{
    template <typename C, typename T>
    struct result
    {
        typedef void type;
    };

    template <typename C, typename T>
    void operator()(C& c, T& x) const
    {
       c.insert(0,x);
    }
};

boost::phoenix::function<insert_impl> const insert = insert_impl();


struct register_vquan_impl
{
    template <typename C, typename T>
    struct result
    {
        typedef void type;
    };

    template <typename C, typename T>
    void operator()(C& c, T& x) const
    {
       c.add_vertex_quantity(x);
    }
};

struct register_equan_impl
{
    template <typename C, typename T>
    struct result
    {
        typedef void type;
    };

    template <typename C, typename T>
    void operator()(C& c, T& x) const
    {
       c.add_edge_quantity(x);
    }
};

struct register_fquan_impl
{
    template <typename C, typename T>
    struct result
    {
        typedef void type;
    };

    template <typename C, typename T>
    void operator()(C& c, T& x) const
    {
//        dump_type<C>();
//        std::cout << "phoenix func: .. add facet quantity. :"<<x << std::endl;
       c.add_facet_quantity(x);
    }
};

struct register_cquan_impl
{
    template <typename C, typename T>
    struct result
    {
        typedef void type;
    };

    template <typename C, typename T>
    void operator()(C& c, T& x) const
    {
       c.add_cell_quantity(x);
    }
};


// [RH][TODO] .. rewrite this with dimension accessors
//
//   gtl_dim<X>(register_quan_impl)() 
//
boost::phoenix::function<register_vquan_impl> const register_vquan = register_vquan_impl();
boost::phoenix::function<register_equan_impl> const register_equan = register_equan_impl();
boost::phoenix::function<register_fquan_impl> const register_fquan = register_fquan_impl();
boost::phoenix::function<register_cquan_impl> const register_cquan = register_cquan_impl();


}
#endif
