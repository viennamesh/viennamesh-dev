/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_MC_PARTICLE_INCLUDE_HH)
#define GSSE_MC_PARTICLE_INCLUDE_HH

#include "gsse/geometry/metric_object.hpp"

// ####################################

namespace gsse {
namespace spec{
namespace miss {
   // 6D coordinate space
   struct geom;
   struct phase;

   struct weight;
   struct vel_gr;
   struct energy;
} // namespace miss
}// namespace spec

namespace monte_carlo {


namespace detail {
   static const long DIM_GEO = 2;
   typedef double NumericT;
   typedef gsse::metric_object<NumericT, DIM_GEO>   CoordinateT; 

  
   typedef boost::fusion::map<
      boost::fusion::pair<gsse::spec::miss::geom,   CoordinateT>
    , boost::fusion::pair<gsse::spec::miss::phase,  CoordinateT>

    , boost::fusion::pair<gsse::spec::miss::weight, NumericT>
    , boost::fusion::pair<gsse::spec::miss::vel_gr, CoordinateT>
    , boost::fusion::pair<gsse::spec::miss::energy, NumericT>
   >  ParticleT;
} // namespace detail

using detail::ParticleT;
using detail::CoordinateT;


// ## numerical integrator
// velvet scheme

template<typename NumericT, typename ParamT, typename CoordT>
void calc_update(NumericT       delta_t, 
                 const ParamT&  param_E,
                 CoordT&        coord,
                 CoordT&        coord_prev)
{

//   CoordT   param_E = CoordT(0.1, 0.8);
   NumericT param_q = 1.0;

   NumericT dt2        = delta_t;
   CoordT   param_acc  = param_E * param_q;

   CoordT  coord_new = coord;
   coord_new  += (coord - coord_prev) + (param_acc * dt2);

   coord_prev  = coord;
   coord       = coord_new;
}

template<typename ContainerCoeff, typename CoortT, typename ParamT>
void calc_field(const ContainerCoeff& coeff,
                const CoortT&         coord,
                ParamT&               param_E)
{

//ax + by + cz + d = 0
//  coeff[0] = a
//  coeff[1] = b
//  coeff[2] = c
// z = (-d -ax -by) / c
//
   param_E[2] = (-1 - coeff[0] * coord[0] - coeff[1]*coord[1]) / coeff[2];
   std::cout << "## param e: " << param_E << std::endl;
}

} // namespace monte_carlo
} // namespace gsse

#endif
