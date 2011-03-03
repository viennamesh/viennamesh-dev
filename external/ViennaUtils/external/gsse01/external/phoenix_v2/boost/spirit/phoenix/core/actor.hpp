/*=============================================================================
    Copyright (c) 2001-2004 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef PHOENIX_CORE_ACTOR_HPP
#define PHOENIX_CORE_ACTOR_HPP

#include <boost/spirit/phoenix/core/limits.hpp>
#include <boost/spirit/phoenix/core/basic_environment.hpp>
#include <boost/mpl/min.hpp>
#include <boost/mpl/identity.hpp>

namespace boost { namespace phoenix
{
    namespace detail
    {
        //  Forward declarations. These will come in when we get to the
        //  operator module, yet, the actor's assignment operator and index
        //  operator are required to be members.

        template <typename T0, typename T1>
        struct make_assign_composite;

        template <typename T0, typename T1>
        struct make_index_composite;

        template <typename BaseT0, typename BaseT1>
        struct comma_result;
        
        // error no arguments supplied
        struct error_expecting_arguments
        {
            template <typename T>
            error_expecting_arguments(T const&) {}
        };            
    }

    typedef fusion::void_t void_type;

    template <typename Actor, typename Env>
    struct apply_actor
    {
        typedef typename Actor::template apply<Env>::type type;
    };
    
    template <typename Eval>
    struct actor : Eval
    {
        typedef actor<Eval> self_type;
        typedef Eval eval_type;

        actor()
            : Eval() {}

        actor(Eval const& base)
            : Eval(base) {}

        template <typename T0>
        explicit actor(T0 const& _0)
            : Eval(_0) {}

        template <typename T0, typename T1>
        actor(T0 const& _0, T1 const& _1)
            : Eval(_0, _1) {}

        typename 
            mpl::eval_if<
                typename Eval::no_nullary // avoid calling apply_actor when this is true
              , mpl::identity<detail::error_expecting_arguments>
              , apply_actor<eval_type, basic_environment<> >
            >::type
        operator()() const
        {
            return eval_type::eval(basic_environment<>());
        }

        template <typename T0>
        typename apply_actor<eval_type, basic_environment<T0> >::type
        operator()(T0& _0) const
        {
            return eval_type::eval(basic_environment<T0>(_0));
        }

        template <typename T0, typename T1>
        typename apply_actor<eval_type, basic_environment<T0, T1> >::type
        operator()(T0& _0, T1& _1) const
        {
            return eval_type::eval(basic_environment<T0, T1>(_0, _1));
        }
        
        template <typename T1>
        typename detail::make_assign_composite<self_type, T1>::type
        operator=(T1 const& a1) const;

        template <typename T1>
        typename detail::make_index_composite<self_type, T1>::type
        operator[](T1 const& a1) const;

        //  Bring in the rest of the constructors and function call operators 
        #include <boost/spirit/phoenix/core/detail/actor.hpp>
    };

    //  Forward declaration: The intent to overload the comma must be
    //  stated early on to avoid the subtle problem that arises when
    //  the header file where the comma operator overload is defined, 
    //  is not included by the client and the client attempts to use 
    //  the comma anyway.

    namespace detail
    {
        template <typename BaseT0, typename BaseT1>
        struct comma_result;
    }

    template <typename BaseT0, typename BaseT1>
    typename detail::comma_result<BaseT0, BaseT1>::type
    operator,(actor<BaseT0> const& a0, actor<BaseT1> const& a1);
}}

#endif
