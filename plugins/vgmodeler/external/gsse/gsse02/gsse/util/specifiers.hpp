/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_UTIL_SPECIFIER_HH)
#define GSSE_UTIL_SPECIFIER_HH

// *** system includes
// *** BOOST includes
// *** GSSE includes

// ==========================
namespace gsse{ 

struct key_associative;
struct transfer_key;

namespace access_specifier {

   struct GSSE_accessor            {    typedef key_associative  access_mechanism;  };
   struct GSSE_accessor_transfer   {    typedef transfer_key     access_mechanism;  };

   static const long DIM_0 = 0;
   static const long DIM_1 = 1;
   static const long DIM_2 = 2;
   static const long DIM_3 = 3;
   static const long DIM_4 = 4;

   // short cuts
   //
   // global accessors
   struct AP:GSSE_accessor{};     // accessor position
   struct AC:GSSE_accessor{};     // accessor cell complex
   struct AQ:GSSE_accessor{};     // accessor quan complex
   struct AMQ:GSSE_accessor{};     // accessor matrix quan complex [RH][new][200908]
   struct ASQ:GSSE_accessor{};     // accessor segment quantity [RH][new][200908]
   struct ADQ:GSSE_accessor{};     // accessor domain quantity [JW][new][201009]

   // opengl 
//   struct AP2:GSSE_accessor{};     // accessor quan complex
//   struct AM:GSSE_accessor{};     // accessor quan complex
//   struct ATM:GSSE_accessor{};     // accessor quan complex
//    struct APN:GSSE_accessor{};     // accessor quan complex
//    struct APC:GSSE_accessor{};     // accessor quan complex
//   struct APV:GSSE_accessor{};     // accessor quan complex
   // --

   struct AP_0:GSSE_accessor{};     // accessor position
   struct AT_0:GSSE_accessor{};     // accessor topology
   struct AT_1:GSSE_accessor{};     // accessor topology
   struct AT_2:GSSE_accessor{};     // accessor topology
   struct AT_3:GSSE_accessor{};     // accessor topology

   // names
   //
   struct AP_vx:GSSE_accessor{};     // accessor position
   struct AT_vx:GSSE_accessor{};     // accessor topology
   struct AT_ee:GSSE_accessor{};     // accessor topology
   struct AT_cl:GSSE_accessor{};     // accessor topology

   struct AT_ft:GSSE_accessor{};     // accessor topology
   struct AT_f0:GSSE_accessor{};     // accessor topology   // equivalence: AT_ft <-> AT_f0
   struct AT_f1:GSSE_accessor{};     // accessor topology
   struct AT_f2:GSSE_accessor{};     // accessor topology
   struct AT_f3:GSSE_accessor{};     // accessor topology
   struct AT_f4:GSSE_accessor{};     // accessor topology

   // ====
   // not used right now
   //
   struct AC_1:GSSE_accessor{};      // accessor co-domain  -> represents a scalar
   struct AC_2:GSSE_accessor{};      // accessor co-domain
   struct AC_3:GSSE_accessor{};      // accessor co-domain
   struct AC_4:GSSE_accessor{};      // accessor co-domain
   struct AC_5:GSSE_accessor{};      // accessor co-domain
   struct AC_6:GSSE_accessor{};      // accessor co-domain
   struct AC_7:GSSE_accessor{};      // accessor co-domain
   struct AC_8:GSSE_accessor{};      // accessor co-domain

   struct AF_0:GSSE_accessor{};      // accessor 0-form
   struct AF_1:GSSE_accessor{};      // accessor 1-form
   struct AF_2:GSSE_accessor{};      // accessor 2-form
   struct AF_3:GSSE_accessor{};      // accessor 3-form
   struct AF_4:GSSE_accessor{};      // accessor 4-form
   struct AF_5:GSSE_accessor{};      // accessor 5-form
   struct AF_6:GSSE_accessor{};      // accessor 6-form
   struct AF_7:GSSE_accessor{};      // accessor 7-form
   //
   // not used right now
   // ====


   // new gsse transfer mechanism
   //
   struct TP_vx: GSSE_accessor_transfer  {   typedef AP_vx   type;  };
   struct TT_vx: GSSE_accessor_transfer  {   typedef AT_vx   type;  };
   struct TT_cl: GSSE_accessor_transfer  {   typedef AT_cl   type;  };

//    struct AC_cc : GSSE_accessor {};  // cell complex
//    struct AC_qc : GSSE_accessor {};  // quan complex
}


   using namespace access_specifier;



// ###########################################
} // namespace gsse
#endif
