// Copyright 2005 Daniel Wallin. 
// Copyright 2005 Joel de Guzman.
//
// Use, modification and distribution is subject to the Boost Software 
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Modeled after range_ex, Copyright 2004 Eric Niebler

#ifndef PHOENIX_ALGORITHM_ITERATION_HPP
#define PHOENIX_ALGORITHM_ITERATION_HPP

#include <algorithm>
#include <numeric>

#include <boost/spirit/phoenix/stl/algorithm/detail/begin.hpp>
#include <boost/spirit/phoenix/stl/algorithm/detail/end.hpp>

#include <boost/spirit/phoenix/function/function.hpp>

namespace boost { namespace phoenix {
namespace impl
{
    struct for_each
    {
        template<class R, class F>
        struct apply
        {
            typedef F type;
        };

        template<class R, class F>
        F operator()(R& r, F fn) const
        {        
            return std::for_each(detail::begin_(r), detail::end_(r), fn);
        }
    };

    struct accumulate
    {
        template<class R, class I, class C = void>
        struct apply
        {
            typedef I type;
        };

        template<class R, class I>
        typename apply<R,I>::type
        operator()(R& r, I i) const
        {
            return std::accumulate(detail::begin_(r), detail::end_(r), i);
        }

        template<class R, class I, class C>
        typename apply<R,I,C>::type
        operator()(R& r, I i, C c) const
        {
            return std::accumulate(detail::begin_(r), detail::end_(r), i, c);
        }
    };
}

function<impl::for_each> const for_each = impl::for_each();
function<impl::accumulate> const accumulate = impl::accumulate();

}}

#endif
