/*=============================================================================
    Copyright (c) 2001-2004 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef PHOENIX_OPERATOR_DETAIL_UNARY_EVAL_HPP
#define PHOENIX_OPERATOR_DETAIL_UNARY_EVAL_HPP

#include <boost/mpl/eval_if.hpp>
#include <boost/spirit/phoenix/core/compose.hpp>

#define PHOENIX_UNARY_EVAL(eval_name, op_result, expr)                          \
    struct eval_name                                                            \
    {                                                                           \
        template <typename Env, typename A0>                                    \
        struct apply                                                            \
        {                                                                       \
            typedef typename A0::template apply<Env>::type x_type;              \
                                                                                \
            typedef typename                                                    \
                mpl::eval_if<                                                   \
                    is_actor<x_type>                                            \
                  , re_curry<eval_name, x_type>                                 \
                  , op_result<x_type>                                           \
                >::type                                                         \
            type;                                                               \
        };                                                                      \
                                                                                \
        template <typename RT, typename Env, typename A0>                       \
        static RT                                                               \
        eval(Env const& env, A0& a0)                                            \
        {                                                                       \
            return expr;                                                        \
        }                                                                       \
    };

#endif


