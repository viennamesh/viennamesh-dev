// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/mp_math/mp_int/mp_int_fwd.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/string.hpp>

namespace boost {
namespace serialization {

template<class Archive, class A, class T>
void save(Archive& ar,
          const boost::mp_math::mp_int<A,T>& x,
          const unsigned int /*version*/)
{
  const std::string s = x.template to_string<std::string>(std::ios_base::hex);
  ar & s;
}

template<class Archive, class A, class T>
void load(Archive& ar,
          boost::mp_math::mp_int<A,T>& x,
          const unsigned int /*version*/)
{
  std::string s;
  ar & s;
  x.template assign(s, std::ios_base::hex);
}

template<class Archive, class A, class T>
inline void serialize(Archive& ar,
                      boost::mp_math::mp_int<A,T>& x,
                      const unsigned int version)
{
  boost::serialization::split_free(ar, x, version);
}

} // namespace serialization
} // namespace boost

