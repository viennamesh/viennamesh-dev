/*=============================================================================
    Copyright (c) 2005 Hartmut Kaiser
    Copyright (c) 2005 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef PHOENIX_SEQUENCE_GET_HPP
#define PHOENIX_SEQUENCE_GET_HPP

#include <boost/spirit/fusion/sequence/at.hpp>
#include <boost/spirit/fusion/sequence/get.hpp>
#include <boost/spirit/fusion/sequence/tuple_element.hpp>

namespace boost { namespace phoenix
{
    template <int N>
    struct get_eval
    {
        template <typename Env, typename Tuple>
        struct apply
        {
            typedef typename fusion::tuple_element<N, Tuple>::type type;
        };

        template <typename RT, typename Env, typename Tuple>
        static RT
        eval(Env const& env, Tuple const& t)
        {
            return get<N>(t);
        }
    };

    template <int N, typename Tuple>
    inline actor<typename as_composite<get_eval<N>, Tuple>::type>
    get(Tuple const& tup) const    
    {
        return compose<get_eval<N> >(tup);
    }

}}

#endif
