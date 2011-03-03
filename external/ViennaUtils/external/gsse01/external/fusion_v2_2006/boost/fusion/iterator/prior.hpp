/*=============================================================================
    Copyright (c) 2001-2006 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_PRIOR_05042005_1144)
#define FUSION_PRIOR_05042005_1144

namespace boost { namespace fusion
{
    namespace extension
    {
        template <typename Tag>
        struct prior_impl
        {
            template <typename Iterator>
            struct apply {};
        };
    }

    namespace result_of
    {
        template <typename Iterator>
        struct prior
        {
            typedef typename
                extension::prior_impl<typename Iterator::ftag>::
                    template apply<Iterator>::type
            type;
        };
    }

    template <typename Iterator>
    typename result_of::prior<Iterator>::type
    prior(Iterator const& i)
    {
        typename result_of::prior<Iterator>::type result =
            extension::prior_impl<typename Iterator::ftag>::
                template apply<Iterator>::call(i);
        return result;
    }
}}

#endif
