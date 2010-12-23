#!/bin/bash

# remove waf build folder
#
rm -rf build/

# remove compiled python code
#
rm -f waftools/*.pyc

# clean hidden waf folder 
#
rm -rf .waf-*/
rm -f .lock-wafbuild

# remove all temporaries
#
find . -name '*~' -exec rm -f {} \;
