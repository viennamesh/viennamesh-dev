//
//  Copyright (c) 2005, 2006 João Abecasis
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#if !BOOST_PP_IS_ITERATING

# if !defined(BOOST_FUSION_SEQUENCE_UTILITY_UNPACK_ARGS_HPP_INCLUDED)
# define BOOST_FUSION_SEQUENCE_UTILITY_UNPACK_ARGS_HPP_INCLUDED

#   include <boost/preprocessor/repetition/enum.hpp>
#   include <boost/preprocessor/iteration/iterate.hpp>
#   include <boost/utility/result_of.hpp>
#   include <boost/mpl/int.hpp>
#   include <boost/fusion/sequence/intrinsic/at.hpp>
#   include <boost/fusion/sequence/intrinsic/size.hpp>
#   include <boost/fusion/sequence/utility/limits.hpp>

namespace boost { namespace fusion
{

    namespace detail
    {

        template <
            class F,
            class Sequence,
            class Size = mpl::int_<result_of::size<Sequence>::value>
        >
        struct unpack_args_impl;

        template <class F, class Sequence>
        struct unpack_args_impl<F, Sequence, mpl::int_<0> >
        {
            typedef typename boost::result_of<F()>::type type;

            static type call(F & f, Sequence &)
            {
                return f();
            }
        };

#   define BOOST_FUSION_arg_type(z, n, _)                                       \
      typename result_of::at<Sequence, mpl::int_<n> >::type
#   define BOOST_FUSION_arg(z, n, _) fusion::at<mpl::int_<n> >(seq)
#   define BOOST_FUSION_n BOOST_PP_ITERATION()

#   define BOOST_PP_ITERATION_PARAMS_1 (3, (1, FUSION_MAX_UNPACK_ARG_SIZE,      \
      <boost/fusion/sequence/utility/unpack_args.hpp>))

#   include BOOST_PP_ITERATE()

#   undef BOOST_FUSION_arg_type
#   undef BOOST_FUSION_arg
#   undef BOOST_FUSION_n

#   undef BOOST_PP_ITERATION_PARAMS_1

    } // namespace detail

    namespace result_of
    {

        template <class F, class Sequence>
        struct unpack_args
            : detail::unpack_args_impl<F, Sequence>
        {
        };

    } // namespace result_of

    template <class F, class Sequence>
    inline typename result_of::unpack_args<F, Sequence>::type
    unpack_args(F & f, Sequence & seq)
    {
        return result_of::unpack_args<F, Sequence>::call(f, seq);
    }

    template <class F, class Sequence>
    inline typename result_of::unpack_args<F const, Sequence>::type
    unpack_args(F const & f, Sequence & seq)
    {
        return result_of::unpack_args<F const, Sequence>::call(f, seq);
    }

    template <class F, class Sequence>
    inline typename result_of::unpack_args<F, Sequence const>::type
    unpack_args(F & f, Sequence const & seq)
    {
        return result_of::unpack_args<F, Sequence const>::call(f, seq);
    }

    template <class F, class Sequence>
    inline typename result_of::unpack_args<F const, Sequence const>::type
    unpack_args(F const & f, Sequence const & seq)
    {
        return result_of::unpack_args<F const, Sequence const>::call(f, seq);
    }

}} // namespace boost::fusion

# endif // include guard

#else // BOOST_PP_IS_ITERATING

    template <class F, class Sequence>
    struct unpack_args_impl<F, Sequence, mpl::int_<BOOST_FUSION_n> >
    {
        typedef typename boost::result_of<
            F( BOOST_PP_ENUM(BOOST_FUSION_n, BOOST_FUSION_arg_type, ~) )
        >::type type;

        static type call(F & f, Sequence & seq)
        {
            return f( BOOST_PP_ENUM(BOOST_FUSION_n, BOOST_FUSION_arg, ~) );
        }
    };

#endif // BOOST_PP_IS_ITERATING
