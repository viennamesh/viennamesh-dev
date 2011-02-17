rm -rf build/
find . -name '*~' -exec rm -f {} \;
#rm -rf .kdev4/
#rm -f *.kdev4
rm -rf doc/doxygen/
cd tests/
./clean.sh
cd ..

cd external/vgmodeler
./clean_all.sh
cd ../..
