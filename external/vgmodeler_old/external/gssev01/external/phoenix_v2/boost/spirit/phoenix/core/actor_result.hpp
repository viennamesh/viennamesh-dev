/*=============================================================================
    Copyright (c) 2005 Hartmut Kaiser
    Copyright (c) 2005 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef PHOENIX_CORE_ACTOR_RESULT_HPP
#define PHOENIX_CORE_ACTOR_RESULT_HPP

#include <boost/spirit/phoenix/core/limits.hpp>
#include <boost/spirit/phoenix/core/basic_environment.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>

namespace boost { namespace phoenix
{

    template <
        typename Actor, 
        BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(
            PHOENIX_ACTOR_LIMIT, typename T, void_type
        )
    >
    struct actor_result
    {
        typedef basic_environment<
                BOOST_PP_ENUM_PARAMS(PHOENIX_ACTOR_LIMIT, T)
            > env_type;
        typedef typename Actor::template apply<env_type>::type type;
    };

}}

#endif
