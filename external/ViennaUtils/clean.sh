rm -rf build/
rm -rf doc/doxygen/
find . -name '*~' -exec rm -f {} \;
#find -name "\.svn" -exec rm -rf {} \;
#find -name "\CMakeFiles" -exec rm -rf {} \;
#cd ..
cd tests/
./clean.sh
cd ..
