if [[ ! -d debug ]]; then
    mkdir -p debug
fi

cd debug
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j
cd ..
