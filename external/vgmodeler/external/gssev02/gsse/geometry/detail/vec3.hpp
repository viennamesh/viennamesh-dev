/* ============================================================================
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GSSE_METRICOBJECT_DETAIL_VEC_HH_ID
#define GSSE_METRICOBJECT_DETAIL_VEC_HH_ID 

// *** system includes

// *** BOOST includes
#include <boost/array.hpp>
#include <boost/operators.hpp>
#include <boost/static_assert.hpp>

#include <boost/mpl/int.hpp>
#include <boost/proto/core.hpp>
#include <boost/proto/context.hpp>

// *** GSSE includes
#include "gsse/util/common.hpp"

// ============================================================================================

namespace gsse
{
namespace mpl = boost::mpl;
namespace proto = boost::proto;
using proto::_;


// This grammar describes which Parray expressions
// are allowed; namely, int and array terminals
// plus, minus, multiplies and divides of Parray expressions.
struct ParrayGrammar
  : proto::or_<
        proto::terminal< double >
      , proto::terminal< double[3] >
      , proto::negate< ParrayGrammar >
      , proto::plus< ParrayGrammar, ParrayGrammar >
      , proto::minus< ParrayGrammar, ParrayGrammar >
      , proto::multiplies< ParrayGrammar, ParrayGrammar >
      , proto::divides< ParrayGrammar, ParrayGrammar >
    >
{};

template<typename Expr>
struct ParrayExpr;

// Tell proto that in the ParrayDomain, all
// expressions should be wrapped in ParrayExpr<> and
// must conform to the ParrayGrammar
struct ParrayDomain
  : proto::domain<proto::generator<ParrayExpr>, ParrayGrammar>
{};

// Here is an evaluation context that indexes into a Parray
// expression, and combines the result.
struct ParraySubscriptCtx
  : proto::callable_context< ParraySubscriptCtx const >
{
    typedef double result_type;

    ParraySubscriptCtx(std::ptrdiff_t i)
      : i_(i)
    {}

    // Index array terminals with our subscript. Everything
    // else will be handled by the default evaluation context.
    double operator ()(proto::tag::terminal, double const (&data)[3]) const
    {
        return data[this->i_];
    }

    std::ptrdiff_t i_;
};

// Here is an evaluation context that prints a Parray expression.
struct ParrayPrintCtx
  : proto::callable_context< ParrayPrintCtx const >
{
    typedef std::ostream &result_type;

    ParrayPrintCtx() {}

    std::ostream &operator ()(proto::tag::terminal, double i) const
    {
        return std::cout << i;
    }

    std::ostream &operator ()(proto::tag::terminal, double const (&arr)[3]) const
    {
        return std::cout << '{' << arr[0] << ", " << arr[1] << ", " << arr[2] << '}';
    }

    template<typename L, typename R>
    std::ostream &operator ()(proto::tag::plus, L const &l, R const &r) const
    {
        return std::cout << '(' << l << " + " << r << ')';
    }

    template<typename L, typename R>
    std::ostream &operator ()(proto::tag::minus, L const &l, R const &r) const
    {
        return std::cout << '(' << l << " - " << r << ')';
    }

    template<typename L, typename R>
    std::ostream &operator ()(proto::tag::multiplies, L const &l, R const &r) const
    {
        return std::cout << l << " * " << r;
    }

    template<typename L, typename R>
    std::ostream &operator ()(proto::tag::divides, L const &l, R const &r) const
    {
        return std::cout << l << " / " << r;
    }
};

// Here is the domain-specific expression wrapper, which overrides
// operator [] to evaluate the expression using the ParraySubscriptCtx.
template<typename Expr>
struct ParrayExpr
  : proto::extends<Expr, ParrayExpr<Expr>, ParrayDomain>
{
    typedef proto::extends<Expr, ParrayExpr<Expr>, ParrayDomain> base_type;

    ParrayExpr( Expr const & expr = Expr() )
      : base_type( expr )
    {}

    // Use the ParraySubscriptCtx to implement subscripting
    // of a Parray expression tree.
    double operator []( std::ptrdiff_t i ) const
    {
        ParraySubscriptCtx const ctx(i);
        return proto::eval(*this, ctx);
    }

    // Use the ParrayPrintCtx to display a Parray expression tree.
    friend std::ostream &operator <<(std::ostream &sout, ParrayExpr<Expr> const &expr)
    {
        ParrayPrintCtx const ctx;
        return proto::eval(expr, ctx);
    }
};

// Here is our Parray terminal, implemented in terms of ParrayExpr
// It is basically just an array of 3 doubles.
struct Parray
  : ParrayExpr< proto::terminal< double[3] >::type >
{
    explicit Parray( double i = 0, double j = 0, double k = 0 )
    {
        (*this)[0] = i;
        (*this)[1] = j;
        (*this)[2] = k;
    }
   // [RH]
    template< typename Expr >
    Parray (Expr const & expr)
    {
        this->assign(proto::as_expr<ParrayDomain>(expr));
    }
//     Parray (Parray const& parray)
//     {
//        (*this) = parray;
//     }



    // Here we override operator [] to give read/write access to
    // the elements of the array. (We could use the ParrayExpr
    // operator [] if we made the subscript context smarter about
    // returning non-const reference when appropriate.)
    double &operator [](std::ptrdiff_t i)
    {
        return proto::value(*this)[i];
    }

    double const &operator [](std::ptrdiff_t i) const
    {
        return proto::value(*this)[i];
    }

    // Here we define a operator = for Parray terminals that
    // takes a Parray expression.
    template< typename Expr >
    Parray &operator =(Expr const & expr)
    {
        // proto::as_expr<ParrayDomain>(expr) is the same as
        // expr unless expr is a double, in which case it
        // is made into a ParrayExpr terminal first.
        return this->assign(proto::as_expr<ParrayDomain>(expr));
    }

    template< typename Expr >
    Parray &printAssign(Expr const & expr)
    {
        *this = expr;
        std::cout << *this << " = " << expr << std::endl;
        return *this;
    }

private:
    template< typename Expr >
    Parray &assign(Expr const & expr)
    {
        // expr[i] here uses ParraySubscriptCtx under the covers.
        (*this)[0] = expr[0];
        (*this)[1] = expr[1];
        (*this)[2] = expr[2];
        return *this;
    }
};


}
#endif
