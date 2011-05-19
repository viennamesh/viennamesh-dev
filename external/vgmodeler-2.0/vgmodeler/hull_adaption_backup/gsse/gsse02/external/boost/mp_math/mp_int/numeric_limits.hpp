// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

namespace std
{

template<class A, class T>
class numeric_limits<boost::mp_math::mp_int<A,T> >
{
public:

  static const bool is_specialized = true;
  static boost::mp_math::mp_int<A,T> min() throw() { return boost::mp_math::mp_int<A,T>(0U); }
  static boost::mp_math::mp_int<A,T> max() throw() { return boost::mp_math::mp_int<A,T>(0U); }

  static const int  digits   = 0;
  static const int  digits10 = 0;
  static const bool is_signed  = true;
  static const bool is_integer = true;
  static const bool is_exact   = true;
  static const int  radix      = 2;
  static boost::mp_math::mp_int<A,T> epsilon    () throw() { return boost::mp_math::mp_int<A,T>(0U); }
  static boost::mp_math::mp_int<A,T> round_error() throw() { return boost::mp_math::mp_int<A,T>(0U); }

  static const int  min_exponent   = 0;
  static const int  min_exponent10 = 0;
  static const int  max_exponent   = 0;
  static const int  max_exponent10 = 0;

  static const bool has_infinity      = false;
  static const bool has_quiet_NaN     = false;
  static const bool has_signaling_NaN = false;
  static const float_denorm_style has_denorm = denorm_absent;
  static const bool has_denorm_loss   = false;
  static boost::mp_math::mp_int<A,T> infinity     () throw() { return boost::mp_math::mp_int<A,T>(0U); }
  static boost::mp_math::mp_int<A,T> quiet_NaN    () throw() { return boost::mp_math::mp_int<A,T>(0U); }
  static boost::mp_math::mp_int<A,T> signaling_NaN() throw() { return boost::mp_math::mp_int<A,T>(0U); }
  static boost::mp_math::mp_int<A,T> denorm_min   () throw() { return boost::mp_math::mp_int<A,T>(0U); }

  static const bool is_iec559  = false;
  static const bool is_bounded = false;
  static const bool is_modulo  = false;

  static const bool traps = false;
  static const bool tinyness_before = false;
  static const float_round_style round_style = round_toward_zero;
};

} // namespace std

