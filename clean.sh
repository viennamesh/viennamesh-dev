rm -rf build/
rm -rf bin/
rm -rf lib/
find . -name '*~' -exec rm -f {} \;
#rm -rf .kdev4/
#rm -f *.kdev4

cd tests/
./clean.sh
cd ..

#cd external/vgmodeler
#./clean_all.sh
#cd ../..
