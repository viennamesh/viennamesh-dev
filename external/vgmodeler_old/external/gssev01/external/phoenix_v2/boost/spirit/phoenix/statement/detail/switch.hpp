/*=============================================================================
    Copyright (c) 2001-2004 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef PHOENIX_STATEMENT_DETAIL_SWITCH_HPP
#define PHOENIX_STATEMENT_DETAIL_SWITCH_HPP

#include <boost/spirit/phoenix/core/nothing.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/generate.hpp>
#include <boost/spirit/fusion/algorithm/push_front.hpp>
#include <boost/spirit/fusion/algorithm/push_back.hpp>
#include <boost/spirit/fusion/sequence/begin.hpp>
#include <boost/spirit/fusion/sequence/size.hpp>
#include <boost/spirit/fusion/iterator/value_of.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/if.hpp>

namespace boost { namespace phoenix
{

    template <typename Actor, typename K, K Value>
    struct switch_case;

    template <typename Actor>
    struct default_case;

    namespace detail
    {
        template <typename T>
        struct is_default_case : mpl::bool_<T::is_default> {};

        template <typename A0, typename A1>
        struct compose_case_a
        {
            // here, A0 and A1 are both switch cases
            typedef typename
                mpl::if_<
                    is_default_case<A1>
                  , fusion::tuple<actor<A1>, actor<A0> >
                  , fusion::tuple<actor<A0>, actor<A1> >
                >::type
            type;

            static type
            apply(A0 const& _0, A1 const& _1, mpl::false_)
            {
                return type(_0, _1);
            }

            static type
            apply(A0 const& _0, A1 const& _1, mpl::true_)
            {
                return type(_1, _0);
            }

            static type
            apply(A0 const& _0, A1 const& _1)
            {
                return apply(_0, _1, is_default_case<A1>());
            }
        };

        template <typename Seq, typename Case>
        struct compose_case_b
        {
            typedef typename fusion::meta::generate<
                typename mpl::eval_if<
                    is_default_case<Case>
                  , fusion::meta::push_front<Seq const, actor<Case> >
                  , fusion::meta::push_back<Seq const, actor<Case> >
	        >::type>::type
            type;

            static type
            apply(Seq const& seq, Case const& case_, mpl::false_)
            {
                return fusion::generate(
		    fusion::push_back(seq, actor<Case>(case_)));
            }

            static type
            apply(Seq const& seq, Case const& case_, mpl::true_)
            {
                return fusion::generate(
		    fusion::push_front(seq, actor<Case>(case_)));
            }

            static type
            apply(Seq const& seq, Case const& case_)
            {
                return apply(seq, case_, is_default_case<Case>());
            }
        };

        template <typename Cases>
        struct ensure_default
        {
            typedef
                is_default_case<
                    typename fusion::meta::value_of<
                        typename fusion::meta::begin<Cases>::type
                    >::type
                >
            is_default_case;

            typedef typename
                mpl::eval_if<
                    is_default_case
                  , mpl::identity<Cases>
                  , fusion::meta::push_front<
                        Cases const, actor<default_case<null_actor> > >
                >::type
            type;

            static type
            apply(Cases const& cases, mpl::false_)
            {
                actor<default_case<null_actor> > default_
                    = default_case<null_actor>(nothing);
                return fusion::push_front(cases, default_);
            }

            static type
            apply(Cases const& cases, mpl::true_)
            {
                return cases;
            }

            static type
            apply(Cases const& cases)
            {
                return apply(cases, is_default_case());
            }
        };

        template <typename Cond, typename Cases>
        struct switch_composite
        {
            BOOST_STATIC_ASSERT(fusion::is_sequence<Cases>::value);
            typedef ensure_default<Cases> ensure_default;

            typedef typename
                fusion::meta::generate<
                    typename fusion::meta::push_front<
                        typename ensure_default::type, Cond>::type
                    >::type
            tuple_type;

            typedef
                composite<
                    detail::switch_eval<fusion::meta::size<tuple_type>::value-2>
                  , tuple_type>
            type;

            static type
            apply(Cond const& cond, Cases const& cases)
            {
                return fusion::generate(
                    fusion::push_front(ensure_default::apply(cases), cond));
            }
        };

        template <typename Cond, typename Cases>
        struct switch_composite_actor
        {
            typedef actor<typename switch_composite<Cond, Cases>::type> type;
        };
    }
}}

#endif
