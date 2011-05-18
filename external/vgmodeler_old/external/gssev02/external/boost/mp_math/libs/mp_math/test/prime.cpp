// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/bind.hpp>
#include <boost/test/unit_test.hpp>
#include "prerequisite.hpp"


template<class MpInt>
struct fixture
{
  std::vector<MpInt> primes;
  std::vector<MpInt> composites;
  std::vector<MpInt> carmichaels;

  fixture();
};

template<class MpInt>
fixture<MpInt>::fixture()
{
  // from http://primes.utm.edu/
  primes.push_back("2424833");
  primes.push_back("5915587277");
  primes.push_back("48112959837082048697");
  primes.push_back("671998030559713968361666935769");
  primes.push_back("2425967623052370772757633156976982469681");
  primes.push_back("22953686867719691230002707821868552601124472329079");
  primes.push_back("31353958997402666638501031970734176101289470405573395248411"
                   "3");
  primes.push_back("46695238499321305088763925547134075213191172396379432249800"
                   "15676156491");
  primes.push_back("18532395500947174450709383384936679868383424444311405679463"
                   "280782405796233163977");
  primes.push_back("28275548353370728705475218432112134576686148069744870344385"
                   "7012153264407439766013042402571");
  primes.push_back("20747222467734852078216952221076085874809964747211172927529"
                   "92589912196684750549658310084416732550077");
  primes.push_back("35201546659608842026088328007565866231962578784643756647773"
                   "109869245232364730066609837018108561065242031153677");
  primes.push_back("49949091806585030192119760356408111278062369027342098434296"
                   "86905940646121085912172293044610060051708652944665271663688"
                   "51");
  primes.push_back("54522121518442644531155217703627128153004567880738702606371"
                   "72006414987479914150831821202259862091373741738511579629040"
                   "732909194883");
  primes.push_back("11116154175595527770308362725647119128773382144932530406518"
                   "51655768643786825585565208246254887946100346011818823101363"
                   "6706338524913578946637");
  primes.push_back("65669205018189751363824155419918192392295592176092883676630"
                   "41617905539892282237934618347035068727470717051679959727072"
                   "53940099469869516422893633357693");
  primes.push_back("51665668390920744584663348665715976941144605703879863575380"
                   "48450432901440804868689337999823161841839689242893622491638"
                   "917313351308387294478994745350551549126803");
  primes.push_back("27218343798190233889779431662111815254174074647423987223301"
                   "82099632472829888864333146379225493741435309651830476334589"
                   "6749125106775048493507719412795029690510090142402163");
  primes.push_back("27834442010031021673804244175247010833232390861821952548705"
                   "37102073847719982865971352904187195267023851783658963402109"
                   "57041552257518693488098683242070746473230980005141419410511"
                   "409");
  primes.push_back("50774619156173716518115936073898485346139106651602235699024"
                   "04065418076795944656510340577164207672426574638345121648036"
                   "69334456133986869450012704680852639785739151463455742734197"
                   "2391976756821");
  primes.push_back("58021664585639791181184025950440248398226136069516938232493"
                   "68750582247183653682429882273371034225069773999682593823264"
                   "19406708576245141031259861340509976971601273015479957884681"
                   "37887651823707102007839");
  primes.push_back("7455602825647884208337395736200454918783366342657");

  // composites with small factors
  composites.push_back("2530121");
  
  // composites with large factors
  // from http://web.mit.edu/kenta/www/three/prime/composites.html.gz
  /*composites.push_back("241999944999997");
  composites.push_back("9247999997483999999981");
  composites.push_back("9247999999996395999999999973");
  composites.push_back("2738000000000000184999999999998407");
  composites.push_back("172979999999999999711699999999999999867");
  composites.push_back("3920000000000000000000000000000071399999999999999999999"
                       "999999994969");
  composites.push_back("6479999999999999999999999999999999999995877999999999999"
                       "9999999999999999999999947559");
  composites.push_back("1458000000000000000000000000000000000000000000000000000"
                       "0000000006749999999999999999999999999999999999999999999"
                       "99999999999999998367");
  composites.push_back("1095200000000000000000000000000000000000000000000000000"
                       "0000000000000000000000000000000003329999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "99999999999433");
  composites.push_back("1999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999772999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999960081");
  composites.push_back("7937999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999960"
                       "7509999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999222"
                       "63");
  composites.push_back("6727999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999996154599999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "999999999999999999999999999999999999999999447193");
  composites.push_back("4231999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999977321999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "999999999999999999999999999999999921103");
  composites.push_back("1729799999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999383409999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "9999999999999999999999999999999999999999999999999999999"
                       "999999610793");*/

  // from http://de.wikibooks.org/wiki/Pseudoprimzahlen:_Tabelle_Carmichael-Zahlen
  carmichaels.push_back("294409");
  carmichaels.push_back("825265"); // 5*7*17*19*73
  carmichaels.push_back("1152271");
  carmichaels.push_back("23382529");
  carmichaels.push_back("62756641");
  carmichaels.push_back("114910489");
  carmichaels.push_back("1407548341");
  carmichaels.push_back("11346205609");
  carmichaels.push_back("173032371289");
  carmichaels.push_back("2199733160881");
  carmichaels.push_back("84154807001953");
  carmichaels.push_back("973694665856161");
  carmichaels.push_back("9746347772161"); // 7*11*13*17*19*31*37*41*641
}


// primality tests
BOOST_AUTO_TEST_CASE_TEMPLATE(prime_is_divisible1, mp_int_type, mp_int_types)
{
  using namespace boost::mp_math;

  fixture<mp_int_type> f;
  typedef typename std::vector<mp_int_type>::const_iterator iter;

  for (iter i = f.primes.begin(); i != f.primes.end(); ++i)
    BOOST_CHECK_EQUAL(is_prime(*i, primality_division_test()), true);
  
  for (iter i = f.composites.begin(); i != f.composites.end(); ++i)
    BOOST_CHECK_EQUAL(is_prime(*i, primality_division_test()), false);
}


BOOST_AUTO_TEST_CASE_TEMPLATE(prime_fermat_test1, mp_int_type, mp_int_types)
{
  using namespace boost;

  mp_math::primality_fermat_test<
    mp_math::uniform_mp_int<mp_int_type>
  > fermat_test(1);

  mt19937 rng;

  fixture<mp_int_type> f;
  typedef typename std::vector<mp_int_type>::const_iterator iter;

  for (iter i = f.primes.begin(); i != f.primes.end(); ++i)
    BOOST_CHECK_EQUAL(boost::mp_math::is_prime(*i, bind(fermat_test, rng, _1)), true);
  
  for (iter i = f.composites.begin(); i != f.composites.end(); ++i)
    BOOST_CHECK_EQUAL(boost::mp_math::is_prime(*i, bind(fermat_test, rng, _1)), false);
}


BOOST_AUTO_TEST_CASE_TEMPLATE(prime_miller_rabin_test1, mp_int_type, mp_int_types)
{
  using namespace boost;

  mp_math::primality_miller_rabin_test<
    mp_math::uniform_mp_int<mp_int_type>
  > mr_test;

  mt19937 rng;

  fixture<mp_int_type> f;
  typedef typename std::vector<mp_int_type>::const_iterator iter;
  for (iter i = f.primes.begin(); i != f.primes.end(); ++i)
    BOOST_CHECK_EQUAL(mp_math::is_prime(*i, bind(mr_test, rng, _1)), true);
  
  for (iter i = f.composites.begin(); i != f.composites.end(); ++i)
    BOOST_CHECK_EQUAL(mp_math::is_prime(*i, bind(mr_test, rng, _1)), false);
  
  for (iter i = f.carmichaels.begin(); i != f.carmichaels.end(); ++i)
    BOOST_CHECK_EQUAL(mp_math::is_prime(*i, bind(mr_test, rng, _1)), false);
}


// prime generation
template<class Engine, class Distribution>
struct tester
{
  boost::mp_math::primality_division_test                   test1;
  boost::mp_math::primality_miller_rabin_test<Distribution> test2;
  Engine rng;

  explicit tester(const Engine& e) : rng(e) {}

  template<class A, class T>
  bool operator()(const boost::mp_math::mp_int<A,T>& p)
  {
    return test1(p) && test2(rng, p);
  }
};

BOOST_AUTO_TEST_CASE_TEMPLATE(generate_safe_prime_128bits, mp_int_type, mp_int_types)
{
  typedef tester<boost::mt19937, boost::mp_math::uniform_mp_int<mp_int_type> > tester_type;
  typedef boost::mp_math::uniform_mp_int_bits<mp_int_type> distribution_type;
  
  boost::mt19937 rng;

  boost::mp_math::safe_prime_generator<tester_type, distribution_type>
    generator(128U, tester_type(rng));

  const mp_int_type safe_prime = generator(rng);

  BOOST_CHECK_EQUAL(safe_prime.precision(), 128U);
}
