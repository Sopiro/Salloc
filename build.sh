rm -rf build
mkdir build
cd build
cmake ..
cmake --build .
./bin/unit_test