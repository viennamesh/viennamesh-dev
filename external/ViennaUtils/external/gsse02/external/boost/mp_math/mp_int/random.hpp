// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/*#ifndef BOOST_MP_MATH_MP_INT_RANDOM_HPP
#define BOOST_MP_MATH_MP_INT_RANDOM_HPP

namespace boost {
namespace mp_math {

template<class A, class T>
struct mp_int;

*/

// this class is modeled after boost::uniform_int
// in fact it uses boost::uniform_int internally
template<class MpInt = mp_int<> >
struct uniform_mp_int
{
  typedef MpInt input_type;
  typedef MpInt result_type;

  // is min() and max() known at compile time?
  static const bool has_fixed_range = false;

  uniform_mp_int(const MpInt& min, const MpInt& max)
  :
    min_(min),
    max_(max),
    d_(0, MpInt::digit_max)
  {}

  result_type min() const { return min_; }
  result_type max() const { return max_; }

  void reset() { d_.reset(); }
  
  template<class Engine>
  result_type operator()(Engine& e);
  
private:

  MpInt min_, max_;
  uniform_int<typename MpInt::digit_type> d_;
};

template<class MpInt>
const bool uniform_mp_int<MpInt>::has_fixed_range;


template<class MpInt>
template<class Engine>
typename uniform_mp_int<MpInt>::result_type
uniform_mp_int<MpInt>::operator()(Engine& e)
{
  result_type tmp;
  tmp.grow_capacity(max_.size());
    
  for (typename result_type::size_type i = 0; i < max_.size(); ++i)
    tmp[i] = d_(e);
    
  tmp.set_size(max_.size());
  tmp.clamp();
    
  if (tmp > max_)
    tmp %= max_;

  return tmp;
}


template<class MpInt = mp_int<> >
struct uniform_mp_int_bits
{
  typedef MpInt input_type;
  typedef MpInt result_type;

  static const bool has_fixed_range = false;

  explicit uniform_mp_int_bits(typename MpInt::size_type bits)
  :
    d_(0, MpInt::digit_max),
    bits_(bits)
  {}

  result_type min() const;
  result_type max() const;

  typename MpInt::size_type precision() const { return bits_; }

  void reset() { d_.reset(); }
  
  template<class Engine>
  result_type operator()(Engine& e);
  
private:

  uniform_int<typename MpInt::digit_type> d_;
  typename MpInt::size_type bits_;
};

template<class MpInt>
const bool uniform_mp_int_bits<MpInt>::has_fixed_range;


template<class MpInt>
template<class Engine>
typename uniform_mp_int_bits<MpInt>::result_type
uniform_mp_int_bits<MpInt>::operator()(Engine& e)
{
  result_type tmp;

  const typename MpInt::size_type offset = bits_ / MpInt::valid_bits + 1;

  tmp.grow_capacity(offset);

  for (typename result_type::size_type i = 0; i < offset; ++i)
    tmp[i] = d_(e);
  
  tmp.set_size(offset);

  const typename MpInt::digit_type mask =
      (~typename MpInt::digit_type(0))
    << (bits_ % MpInt::valid_bits);

  tmp[offset-1] &= ~mask;
  
  tmp.set_bit(bits_-1); // make exactly bits_ bits long
  
  return tmp;
}

template<class MpInt>
typename uniform_mp_int_bits<MpInt>::result_type
uniform_mp_int_bits<MpInt>::min() const
{
  result_type tmp;
  
  const typename MpInt::size_type offset =
    bits_ / MpInt::valid_bits + 1;
  
  tmp.grow_capacity(offset);
  
  std::memset(tmp.digits(), 0,
      offset * sizeof(typename MpInt::digit_type));
  
  tmp.set_size(offset);
  tmp.set_bit(bits_ - 1);
  
  return tmp;
}

template<class MpInt>
typename uniform_mp_int_bits<MpInt>::result_type
uniform_mp_int_bits<MpInt>::max() const
{
  result_type tmp;
  const typename MpInt::size_type offset =
    bits_ / MpInt::valid_bits + 1;
  
  tmp.grow_capacity(offset);
  
  for (typename result_type::size_type i = 0; i < offset; ++i)
    tmp[i] = MpInt::digit_max;
  
  tmp.set_size(offset);

  const typename MpInt::digit_type mask =
      (~typename MpInt::digit_type(0))
    << (bits_ % MpInt::valid_bits);

  tmp[offset-1] &= ~mask;

  return tmp;
}


/*
} // namespace boost
} // namespace mp_math

#endif
*/
