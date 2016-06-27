// switch.hpp
//
// Copyright (c) 2006-2007
// Steven Watanabe
//
// Distriuted under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost/org/LICENSE_1_0.txt)

#ifndef BOOST_SWITCH_HPP_INCLUDED
#define BOOST_SWITCH_HPP_INCLUDED

#include <stdexcept>
#include <string>
#include <boost/config.hpp>
#include <boost/throw_exception.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/preprocessor/config/limits.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/iteration/local.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/at.hpp>
#include <boost/type_traits/remove_reference.hpp>

#ifndef BOOST_SWITCH_LIMIT
    #define BOOST_SWITCH_LIMIT 50
#endif

#if BOOST_SWITCH_LIMIT > BOOST_PP_LIMIT_REPEAT
    #error BOOST_SWITCH_LIMIT exceeds Boost.Preprocessor limit
#endif
#if BOOST_SWITCH_LIMIT > BOOST_PP_LIMIT_ITERATION
    #error BOOST_SWITCH_LIMIT exceeds Boost.Preprocessor limit
#endif

namespace boost {

class bad_switch : public std::runtime_error {
public:
    template<class Int>
    explicit bad_switch(Int v) :
        std::runtime_error(
            "boost::switch_ default case called -- none provided. value is: " +
            boost::lexical_cast<std::string>(v)) {}
};

namespace switch_detail {

// Avoid the need to create all the specializations of switch_impl
// twice. Just pass this to switch_impl<N>::apply(...) when no
// default is supplied.

template<class R>
inline R forced_return(typename boost::remove_reference<R>::type* r = 0) {
    return(*r);
}

// Thanks to Stjepan Rajko for catching this.
template<>
inline void forced_return<void>(void*) {}
template<>
inline const void forced_return<const void>(const void*) {}
template<>
inline volatile void forced_return<volatile void>(volatile void*) {}
template<>
inline const volatile void forced_return<const volatile void>(const volatile void*) {}

template<class R>
struct throw_exception {
    template<class Int>
    R operator()(Int i) const {
        boost::throw_exception(bad_switch(i));
        return(switch_detail::forced_return<R>());
    }
};

// N is the number of cases not including the default
template<int N>
struct switch_impl;

// specialize for 0 separately to avoid warnings
template<>
struct switch_impl<0> {
    template<class V, class Int, class F, class Default>
    static typename F::result_type
    apply(Int i, F, Default d BOOST_APPEND_EXPLICIT_TEMPLATE_TYPE(V)) {
        return(d(i));
    }
};

#define BOOST_SWITCH_CASE(z, n, data)                   \
    case boost::mpl::at_c<data, n>::type::value: {      \
        typename boost::mpl::at_c<data, n>::type arg;   \
        return(f(arg));                                 \
    }

#define BOOST_SWITCH_IMPL(z, n, data)                                   \
    template<>                                                          \
    struct switch_impl<n> {                                             \
        template<class V, class I, class F, class D>                    \
        static typename F::result_type                                  \
        apply(I i, F f, D d BOOST_APPEND_EXPLICIT_TEMPLATE_TYPE(V)) {   \
            switch(i) {                                                 \
                BOOST_PP_REPEAT_##z(n, BOOST_SWITCH_CASE, V)            \
                default: return(d(i));                                  \
            }                                                           \
        }                                                               \
    };

#define BOOST_PP_LOCAL_LIMITS (1, BOOST_SWITCH_LIMIT)
#define BOOST_PP_LOCAL_MACRO(n) BOOST_SWITCH_IMPL(1, n, ~)
#include BOOST_PP_LOCAL_ITERATE()

#undef BOOST_SWITCH_IMPL
#undef BOOST_SWITCH_CASE

}

template<class V, class N, class F>
inline typename F::result_type
switch_(N n, F f BOOST_APPEND_EXPLICIT_TEMPLATE_TYPE(V)) {
    typedef switch_detail::switch_impl<boost::mpl::size<V>::value> impl;
    switch_detail::throw_exception<typename F::result_type> default_;
    return(impl::template apply<V>(n, f, default_));
}

template<class V, class N, class F, class D>
inline typename F::result_type
switch_(N n, F f, D d BOOST_APPEND_EXPLICIT_TEMPLATE_TYPE(V)) {
    typedef switch_detail::switch_impl<boost::mpl::size<V>::value> impl;
    return(impl::template apply<V>(n, f, d));
}

}

#endif
