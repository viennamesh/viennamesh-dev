//
//  Copyright (c) 2006 João Abecasis
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/fusion/sequence/utility/unpack_args.hpp>
#include <boost/fusion/sequence/container/vector/vector.hpp>
#include <boost/fusion/sequence/intrinsic/at.hpp>
#include <boost/mpl/int.hpp>

#include <boost/detail/lightweight_test.hpp>

struct object {};

struct functor
{
    typedef int result_type;

    int get()                                   { return 287; }
    int get() const                             { return 139; }

    int operator()()                            { return get(); }
    int operator()() const                      { return get(); }

    int operator()(int i)                       { return get() + i; }
    int operator()(int i) const                 { return get() + i; }

    int operator()(object &, int i)             { return get() - i; }
    int operator()(object &, int i) const       { return get() - i; }
    int operator()(object const &, int i)       { return get() + i; }
    int operator()(object const &, int i) const { return get() + i; }
};

template <class T>
inline T const & const_(T const & t)
{
    return t;
}

int main()
{
    typedef boost::fusion::vector<> vector0;
    typedef boost::fusion::vector<int> vector1;
    typedef boost::fusion::vector<object, int> vector2;

    using boost::fusion::unpack_args;
    using boost::fusion::at;

    functor f;
    object o;

    vector0 v0;
    vector1 v1(23);
    vector2 v2(o, 17);

    // Preconditions for test
    BOOST_TEST(23 == at<boost::mpl::int_<0> >(v1));
    BOOST_TEST(23 == at<boost::mpl::int_<0> >(const_(v1)));
    BOOST_TEST(17 == at<boost::mpl::int_<1> >(v2));
    BOOST_TEST(17 == at<boost::mpl::int_<1> >(const_(v2)));

    BOOST_TEST(f.get()              != const_(f).get());

    BOOST_TEST(f.get()              ==              f           ()              );
    BOOST_TEST(const_(f).get()      ==              const_(f)   ()              );

    BOOST_TEST(23 + f.get()         ==              f           (23)            );
    BOOST_TEST(23 + const_(f).get() ==              const_(f)   (23)            );

    BOOST_TEST(f.get() - 17         ==              f           (o, 17)         );
    BOOST_TEST(const_(f).get() - 17 ==              const_(f)   (o, 17)         );
    BOOST_TEST(f.get() + 17         ==              f           (const_(o), 17) );
    BOOST_TEST(const_(f).get() + 17 ==              const_(f)   (const_(o), 17) );

    // === Actual Tests for unpack_args === //
    BOOST_TEST(f.get()              == unpack_args( f           , v0        ));
    BOOST_TEST(const_(f).get()      == unpack_args( const_(f)   , v0        ));
    BOOST_TEST(f.get()              == unpack_args( f           , const_(v0)));
    BOOST_TEST(const_(f).get()      == unpack_args( const_(f)   , const_(v0)));

    BOOST_TEST(23 + f.get()         == unpack_args( f           , v1        ));
    BOOST_TEST(23 + const_(f).get() == unpack_args( const_(f)   , v1        ));
    BOOST_TEST(23 + f.get()         == unpack_args( f           , const_(v1)));
    BOOST_TEST(23 + const_(f).get() == unpack_args( const_(f)   , const_(v1)));

    BOOST_TEST(f.get() - 17         == unpack_args( f           , v2        ));
    BOOST_TEST(const_(f).get() - 17 == unpack_args( const_(f)   , v2        ));
    BOOST_TEST(f.get() + 17         == unpack_args( f           , const_(v2)));
    BOOST_TEST(const_(f).get() + 17 == unpack_args( const_(f)   , const_(v2)));
}
