/*=============================================================================
    Copyright (c) 2001-2006 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_BEGIN_04052005_1132)
#define FUSION_BEGIN_04052005_1132


#include <boost/fusion/support/tag_of.hpp>

namespace boost { namespace fusion
{
    struct fusion_sequence_tag;

    namespace extension
    {
        template <typename Tag>
        struct begin_impl
        {
            template <typename Sequence>
            struct apply;
        };
    }

    namespace result_of
    {
        template <typename Sequence>
        struct begin
        {
            typedef typename
                extension::begin_impl<typename traits::tag_of<Sequence>::type>::
                template apply<Sequence>::type
            type;
        };
    }

    template <typename Sequence>
    inline typename result_of::begin<Sequence>::type
    begin(Sequence& seq)
    {
        typedef result_of::begin<Sequence> begin_meta;
        return extension::begin_impl<typename traits::tag_of<Sequence>::type>::
            template apply<Sequence>::call(seq);
    }

    template <typename Sequence>
    inline typename result_of::begin<Sequence const>::type
    begin(Sequence const& seq)
    {
        typedef result_of::begin<Sequence const> begin_meta;
        return extension::begin_impl<typename traits::tag_of<Sequence>::type>::
            template apply<Sequence const>::call(seq);
    }
}}

#endif
