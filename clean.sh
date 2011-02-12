rm -rf build/
find . -name '*~' -exec rm -f {} \;
cd tests/
./clean.sh
cd ..
