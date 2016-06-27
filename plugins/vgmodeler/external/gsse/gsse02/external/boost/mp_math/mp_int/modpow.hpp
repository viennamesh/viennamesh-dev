// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_MP_INT_MODPOW_HPP
#define BOOST_MP_MATH_MP_INT_MODPOW_HPP

#include <boost/mp_math/mp_int/modpow_ctx.hpp>
#include <boost/mp_math/mp_int/mp_int_fwd.hpp>
#include <boost/mp_math/mp_int/detail/modpow.hpp>
#include <boost/mp_math/mp_int/detail/modular_reduction.hpp>

namespace boost {
namespace mp_math {

// z = base^exp % mod
template<class A, class T>
mp_int<A,T> modpow(const mp_int<A,T>& base,
                   const mp_int<A,T>& exp,
                   const mp_int<A,T>& mod,
                   modpow_ctx<A,T>*   ctx = 0)
{
  if (mod.is_negative())
    throw std::domain_error("modpow: modulus must be positive");

  typedef modpow_ctx<A,T> ctx_t;
  
  ctx_t tmp_ctx;
  
  if (!ctx)
  {
    tmp_ctx.detect_modulus_type(mod);
    tmp_ctx.precalculate(mod);
    ctx = &tmp_ctx;
  }
  else
  {
    if (!ctx->precalculated)
    {
      ctx->detect_modulus_type(mod);
      ctx->precalculate(mod);
    }
  }

  if (exp.is_negative())
    return modpow(modinv(base, mod), abs(exp), mod, ctx);

  switch (ctx->modulus_type)
  {
    case ctx_t::mod_restricted_dr:
      return detail::montgomery_modpow(base, exp, mod, 1, ctx->rho);
    case ctx_t::mod_unrestricted_dr:
      return detail::montgomery_modpow(base, exp, mod, 2, ctx->rho);
    case ctx_t::mod_unrestricted_dr_slow:
      return detail::barret_modpow    (base, exp, mod, 1, ctx->mu);
    case ctx_t::mod_odd:
      return detail::montgomery_modpow(base, exp, mod, 0, ctx->rho);
    case ctx_t::mod_generic:
    default:
      return detail::barret_modpow    (base, exp, mod, 0, ctx->mu);
  }
}


} // namespace mp_math
} // namespace boost

#endif

