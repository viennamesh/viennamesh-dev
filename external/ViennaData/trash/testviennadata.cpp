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

#include "tests/testconfig.h"

#if not USE_OLD_TESTBENCH

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Master_Test_Suite_for_ViennaData"
#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>

#else

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "tests/testbench.h"


int main(int argc, char *argv[])
{
  check_data_access();

  return EXIT_SUCCESS;
}

#endif
