#!/bin/bash

# remove waf build folder
#
rm -rf build/

# clean hidden waf folder 
#
rm -rf .waf-*/
rm -f .lock-wafbuild

# remove all temporaries
#
find . -name '*~' -exec rm -f {} \;
