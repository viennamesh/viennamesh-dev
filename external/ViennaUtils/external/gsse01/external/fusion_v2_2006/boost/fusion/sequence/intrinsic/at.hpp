/*=============================================================================
    Copyright (c) 2001-2006 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_AT_05042005_0722)
#define FUSION_AT_05042005_0722

#include <boost/mpl/int.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/fusion/support/tag_of.hpp>
#include <boost/fusion/support/detail/access.hpp>

namespace boost { namespace fusion
{
    namespace extension
    {
        template <typename Tag>
        struct at_impl
        {
            template <typename Sequence, typename N>
            struct apply;
        };
    }

    namespace result_of
    {
        template <typename Sequence, int N>
        struct at_c
        {
            typedef typename
                extension::at_impl<typename traits::tag_of<Sequence>::type>::
                    template apply<Sequence, mpl::int_<N> >::type
            type;
        };

        template <typename Sequence, typename N>
        struct at 
        {
            typedef typename
                extension::at_impl<typename traits::tag_of<Sequence>::type>::
                    template apply<Sequence, N>::type
            type;
        };
    }

    template <int N, typename Sequence>
    inline typename 
        lazy_disable_if<
            is_const<Sequence>
          , result_of::at_c<Sequence, N>
        >::type
    at_c(Sequence& seq)
    {
        typedef result_of::at_c<Sequence, N> at_meta;
        return extension::at_impl<typename traits::tag_of<Sequence>::type>::
            template apply<Sequence, mpl::int_<N> >::call(seq);
    }

    template <int N, typename Sequence>
    inline typename result_of::at_c<Sequence const, N>::type
    at_c(Sequence const& seq)
    {
        typedef result_of::at_c<Sequence const, N> at_meta;
        return extension::at_impl<typename traits::tag_of<Sequence>::type>::
            template apply<Sequence const, mpl::int_<N> >::call(seq);
    }

    template <typename N, typename Sequence>
    inline typename 
        lazy_disable_if<
            is_const<Sequence>
          , result_of::at<Sequence, N>
        >::type
    at(Sequence& seq)
    {
        typedef result_of::at<Sequence, N> at_meta;
        return extension::at_impl<typename traits::tag_of<Sequence>::type>::
            template apply<Sequence, N>::call(seq);
    }

    template <typename N, typename Sequence>
    inline typename result_of::at<Sequence const, N>::type
    at(Sequence const& seq)
    {
        typedef result_of::at<Sequence const, N> at_meta;
        return extension::at_impl<typename traits::tag_of<Sequence>::type>::
            template apply<Sequence const, N>::call(seq);
    }
}}

#endif

