/*=============================================================================
    Copyright (c) 2001-2006 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_NEXT_05042005_1101)
#define FUSION_NEXT_05042005_1101

namespace boost { namespace fusion
{
    namespace extension
    {
        template <typename Tag>
        struct next_impl
        {
            template <typename Iterator>
            struct apply {};
        };
    }

    namespace result_of
    {
        template <typename Iterator>
        struct next
        {
            typedef typename
                extension::next_impl<typename Iterator::ftag>::
                    template apply<Iterator>::type
            type;
        };
    }

    template <typename Iterator>
    typename result_of::next<Iterator>::type
    next(Iterator const& i)
    {
        typename result_of::next<Iterator>::type result =
            extension::next_impl<typename Iterator::ftag>::
                template apply<Iterator>::call(i);
        return result;
    }
}}

#endif
