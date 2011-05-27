/*=============================================================================
    Copyright (c) 2001-2006 Joel de Guzman
    Copyright (c) 2006 Dan Marsden

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_EQUAL_TO_IMPL_20060128_1423)
#define FUSION_EQUAL_TO_IMPL_20060128_1423

#include <boost/fusion/sequence/comparison/equal_to.hpp>

namespace boost { namespace fusion {

    struct zip_view_iterator_tag;

    namespace extension
    {
        template<typename Tag>
        struct equal_to_impl;

        template<>
        struct equal_to_impl<zip_view_iterator_tag>
        {
            template<typename It1, typename It2>
            struct apply
                : result_of::equal_to<typename It1::iterators, typename It2::iterators>
            {};
        };
    }
}}

#endif
