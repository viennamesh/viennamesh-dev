/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                             rupp@iue.tuwien.ac.at
   authors:    Markus Bina (boost.test interface)    bina@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaData base directory
======================================================================= */

#include "testconfig.h"

#if not USE_OLD_TESTBENCH

#include "testbench.h"
#include "viennadata/interface.hpp"
#include "viennadata/vlog.hpp"
#include <iostream>
#include <stdexcept>

#include <boost/test/auto_unit_test.hpp>

BOOST_AUTO_TEST_SUITE(testbench);

BOOST_AUTO_TEST_CASE( full_test_of_viennadata )
{
    StandardClass std_obj1;
    StandardClass std_obj2;

    ClassWithID    id_obj1(23);
    ClassWithID    id_obj2(42);

    viennadata::vlog << std::endl;
    viennadata::vlog << "Reserve data ..." << std::endl;

    BOOST_REQUIRE_NO_THROW(( viennadata::reserve<char,     double     >(123)(StandardClass()) ));
    BOOST_REQUIRE_NO_THROW(( viennadata::reserve<char,     std::string>(123)(StandardClass()) ));
    BOOST_REQUIRE_NO_THROW((viennadata::reserve<SomeKey,  double     >(123)(StandardClass())   ));
    BOOST_REQUIRE_NO_THROW((viennadata::reserve<SomeKey,  std::string>(123)(StandardClass())));
    BOOST_REQUIRE_NO_THROW((viennadata::reserve<QuickKey, double     >(123)(StandardClass())));
    BOOST_REQUIRE_NO_THROW((viennadata::reserve<QuickKey, std::string>(123)(StandardClass())));

    BOOST_REQUIRE_NO_THROW((viennadata::reserve<char,     double     >(123)(ClassWithID(1))));
    BOOST_REQUIRE_NO_THROW((viennadata::reserve<char,     std::string>(123)(ClassWithID(2))));
    BOOST_REQUIRE_NO_THROW((viennadata::reserve<SomeKey,  double     >(123)(ClassWithID(3))));
    BOOST_REQUIRE_NO_THROW((viennadata::reserve<SomeKey,  std::string>(123)(ClassWithID(4))));
    BOOST_REQUIRE_NO_THROW((viennadata::reserve<QuickKey, double     >(123)(ClassWithID(5))));
    BOOST_REQUIRE_NO_THROW((viennadata::reserve<QuickKey, std::string>(123)(ClassWithID(6))));

    viennadata::vlog << "Write data ..." << std::endl;
    viennadata::access<char,double>('c')(std_obj1) = 23.45;
    viennadata::access<char,std::string>('c')(std_obj1) = "Hello";
    viennadata::access<SomeKey,double>(SomeKey(2))(std_obj1) = 23.456;
    viennadata::access<SomeKey,std::string>(SomeKey(2))(std_obj1) = "World";
    viennadata::access<QuickKey,double>(QuickKey(2))(std_obj1) = 23.4567;
    viennadata::access<QuickKey,std::string>(QuickKey(2))(std_obj1) = "!";

    viennadata::access<char,double>('c')(std_obj2) = 30;
    viennadata::access<char,std::string>('c')(std_obj2) = "This";
    viennadata::access<SomeKey,double>(SomeKey(2))(std_obj2) = 300;
    viennadata::access<SomeKey,std::string>(SomeKey(2))(std_obj2) = "is";
    viennadata::access<QuickKey,double>(QuickKey(2))(std_obj2) = 3000;
    viennadata::access<QuickKey,std::string>(QuickKey(2))(std_obj2) = "Sparta!";

    viennadata::access<char,double>('c')(id_obj1) = 1.2;
    viennadata::access<char,double>('d')(id_obj1) = 8.1;
    viennadata::access<char,std::string>('c')(id_obj1) = "foo";
    viennadata::access<SomeKey,double>(SomeKey(2))(id_obj1) = 3.4;
    viennadata::access<SomeKey,std::string>(SomeKey(2))(id_obj1) = "or";
    viennadata::access<QuickKey,double>(QuickKey(2))(id_obj1) = 5.6;
    viennadata::access<QuickKey,std::string>(QuickKey(2))(id_obj1) = "bar";

    viennadata::access<char,double>('c')(id_obj2) = 9.8;
    viennadata::access<char,double>('d')(id_obj2) = 1.1;
    viennadata::access<char,std::string>('c')(id_obj2) = "all";
    viennadata::access<SomeKey,double>(SomeKey(2))(id_obj2) = 7.6;
    viennadata::access<SomeKey,std::string>(SomeKey(2))(id_obj2) = "your";
    viennadata::access<QuickKey,double>(QuickKey(2))(id_obj2) = 5.4;
    viennadata::access<QuickKey,std::string>(QuickKey(2))(id_obj2) = "base";

    viennadata::vlog << "Try to modify a little bit of data ..." << std::endl;
    BOOST_REQUIRE_NO_THROW((viennadata::erase<char,double>('c')(std_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::erase<SomeKey,double>(SomeKey(2))(std_obj2)));
    BOOST_REQUIRE_NO_THROW((viennadata::erase<QuickKey,double>(QuickKey(2))(id_obj2)));

    viennadata::vlog << "Try to read data ..." << std::endl;
    BOOST_CHECK_EQUAL((viennadata::access<char,double>('c')(std_obj1)), 0 );
    BOOST_CHECK_EQUAL((viennadata::access<char,std::string>('c')(std_obj1)), "Hello");
    BOOST_CHECK_EQUAL((viennadata::access<SomeKey,double>(SomeKey(2))(std_obj1)), 23.456);
    BOOST_CHECK_EQUAL((viennadata::access<SomeKey,std::string>(SomeKey(2))(std_obj1)), "World");
    BOOST_CHECK_EQUAL((viennadata::access<QuickKey,double>(QuickKey(2))(std_obj1)), 23.4567);
    BOOST_CHECK_EQUAL((viennadata::access<QuickKey,std::string>(QuickKey(2))(std_obj1)), "!");

    BOOST_CHECK_EQUAL((viennadata::access<char,double>('c')(std_obj2)), 30);
    BOOST_CHECK_EQUAL((viennadata::access<char,std::string>('c')(std_obj2)), "This");
    BOOST_CHECK_EQUAL((viennadata::access<SomeKey,double>(SomeKey(2))(std_obj2)), 0);
    BOOST_CHECK_EQUAL((viennadata::access<SomeKey,std::string>(SomeKey(2))(std_obj2)), "is");
    BOOST_CHECK_EQUAL((viennadata::access<QuickKey,double>(QuickKey(2))(std_obj2)), 3000);
    BOOST_CHECK_EQUAL((viennadata::access<QuickKey,std::string>(QuickKey(2))(std_obj2)), "Sparta!");

    BOOST_CHECK_EQUAL((viennadata::access<char,double>('c')(id_obj1)), 1.2);
    BOOST_CHECK_EQUAL((viennadata::access<char,std::string>('c')(id_obj1)), "foo");
    BOOST_CHECK_EQUAL((viennadata::access<SomeKey,double>(SomeKey(2))(id_obj1)), 3.4);
    BOOST_CHECK_EQUAL((viennadata::access<SomeKey,std::string>(SomeKey(2))(id_obj1)), "or");
    BOOST_CHECK_EQUAL((viennadata::access<QuickKey,double>(QuickKey(2))(id_obj1)), 5.6);
    BOOST_CHECK_EQUAL((viennadata::access<QuickKey,std::string>(QuickKey(2))(id_obj1)), "bar");

    BOOST_CHECK_EQUAL((viennadata::access<char,double>('c')(id_obj2)), 9.8);
    BOOST_CHECK_EQUAL((viennadata::access<char,std::string>('c')(id_obj2)), "all");
    BOOST_CHECK_EQUAL((viennadata::access<SomeKey,double>(SomeKey(2))(id_obj2)), 7.6);
    BOOST_CHECK_EQUAL((viennadata::access<SomeKey,std::string>(SomeKey(2))(id_obj2)), "your");
    BOOST_CHECK_EQUAL((viennadata::access<QuickKey,double>(QuickKey(2))(id_obj2)), 0);
    BOOST_CHECK_EQUAL((viennadata::access<QuickKey,std::string>()(id_obj2)), "base");

    BOOST_CHECK_EQUAL((*viennadata::find<char, double>('c')(id_obj1)), 1.2);
    BOOST_CHECK_EQUAL((*viennadata::find<char, double>('c')(id_obj2)), 9.8);
    BOOST_CHECK_EQUAL((*viennadata::find<char, double>('d')(id_obj1)), 8.1);
    BOOST_CHECK_EQUAL((*viennadata::find<char, double>('d')(id_obj2)), 1.1);

    BOOST_REQUIRE_NO_THROW((viennadata::move<char, double>()(id_obj2, std_obj1)));

    BOOST_CHECK_EQUAL((viennadata::access<char,double>('c')(std_obj1)), 9.8);
    BOOST_CHECK_EQUAL((viennadata::access<char,double>('d')(std_obj1)), 1.1);

    BOOST_CHECK_EQUAL((*viennadata::find<char, double>('c')(std_obj1)), 9.8);
    BOOST_CHECK((viennadata::find<char, double>('c')(id_obj2)) == 0);
    BOOST_CHECK_EQUAL((*viennadata::find<char, double>('d')(std_obj1)), 1.1);
    BOOST_CHECK((viennadata::find<char, double>('d')(id_obj2)) == 0);
    BOOST_CHECK((viennadata::find<char, double>('d')(id_obj2)) == 0);
    BOOST_CHECK((viennadata::find<char, long>('c')(id_obj2)) == 0);
    BOOST_CHECK_EQUAL((*viennadata::find<char, std::string>('c')(id_obj2)), "all");

    viennadata::vlog << "Running a little bit of compilation checks ..." << std::endl;

    BOOST_REQUIRE_NO_THROW((viennadata::move<char, double>()(id_obj2, std_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::move<char, std::string>()(id_obj2, std_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::move<SomeKey, double>()(id_obj2, std_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::move<SomeKey, std::string>()(id_obj2, std_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::move<QuickKey, double>()(id_obj2, std_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::move<QuickKey, std::string>()(id_obj2, std_obj1)));

    BOOST_REQUIRE_NO_THROW((viennadata::move<char, double>('c')(id_obj2, std_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::move<char, std::string>('c')(id_obj2, std_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::move<SomeKey, double>(SomeKey(2))(id_obj2, std_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::move<SomeKey, std::string>(SomeKey(2))(id_obj2, std_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::move<QuickKey, double>(QuickKey(2))(id_obj2, std_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::move<QuickKey, std::string>()(id_obj2, std_obj1)));

    BOOST_REQUIRE_NO_THROW((viennadata::copy<char, double>()(id_obj2, id_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::copy<char, std::string>()(id_obj2, id_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::copy<SomeKey, double>()(id_obj2, id_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::copy<SomeKey, std::string>()(id_obj2, id_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::copy<QuickKey, double>()(id_obj2, id_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::copy<QuickKey, std::string>()(id_obj2, id_obj1)));

    BOOST_REQUIRE_NO_THROW((viennadata::copy<char, double>('c')(id_obj2, id_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::copy<char, std::string>('c')(id_obj2, id_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::copy<SomeKey, double>(SomeKey(2))(id_obj2, id_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::copy<SomeKey, std::string>(SomeKey(2))(id_obj2, id_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::copy<QuickKey, double>(QuickKey(2))(id_obj2, id_obj1)));
    BOOST_REQUIRE_NO_THROW((viennadata::copy<QuickKey, std::string>(QuickKey(2))(id_obj2, id_obj1)));


    viennadata::vlog << "Try to erase all quantities stored using QuickKey ..." << std::endl;
    BOOST_REQUIRE_NO_THROW((viennadata::erase<QuickKey, viennadata::all>()(id_obj2)));

}


BOOST_AUTO_TEST_SUITE_END();

#endif
