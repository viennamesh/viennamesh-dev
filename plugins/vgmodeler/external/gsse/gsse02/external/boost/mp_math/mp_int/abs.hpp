// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

template<class A, class T>
mp_int<A,T> abs(const mp_int<A,T>& x)
{
  mp_int<A,T> tmp(x);
  tmp.set_sign(1);
  return tmp;
}
