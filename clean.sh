rm -rf build/
find . -name '*~' -exec rm -f {} \;
#rm -rf .kdev4/
#rm -f *.kdev4
cd tests/
./clean.sh
cd ..
