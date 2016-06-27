/*=============================================================================
    Copyright (c) 2001-2006 Joel de Guzman
    Copyright (c) 2005-2006 Dan Marsden

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_DETAIL_IS_MPL_SEQUENCE_15122005_2137)
#define FUSION_DETAIL_IS_MPL_SEQUENCE_15122005_2137

#include <boost/fusion/support/sequence_base.hpp>

#include <boost/mpl/is_sequence.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/int_fwd.hpp>
#include <boost/mpl/long_fwd.hpp>
#include <boost/mpl/size_t_fwd.hpp>
#include <boost/mpl/integral_c_fwd.hpp>

#include <boost/type_traits/is_base_and_derived.hpp>

#include <cstddef>

namespace boost { namespace fusion {
    
    namespace detail
    {
        template<typename T>
        struct is_mpl_sequence
            : mpl::and_<mpl::not_<is_base_and_derived<sequence_root, T> >, mpl::is_sequence<T> >
        {};

        template<int N>
        struct is_mpl_sequence<mpl::int_<N> >
            : mpl::false_
        {};

        template<int N>
        struct is_mpl_sequence<mpl::bool_<N> >
            : mpl::false_
        {};

        template<typename T, T N>
        struct is_mpl_sequence<mpl::integral_c<T, N> >
            : mpl::false_
        {};

        template<long N>
        struct is_mpl_sequence<mpl::long_<N> >
            : mpl::false_
        {};

        template<std::size_t N>
        struct is_mpl_sequence<mpl::size_t<N> >
            : mpl::false_
        {};
    }
}}

#endif
