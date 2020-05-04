if [[ ! -d debug ]]; then
    mkdir -p debug
fi

cd debug
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS_DEBUG="-g -O0" ..
make -j
cd ..
