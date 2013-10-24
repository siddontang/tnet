# tnet

a tiny lib for high performance network program

# build

    mkdir -p build
    cd build
    cmake ..
    make

If you want to use tcmalloc and build release version, do below

    cmake .. -DCMAKE_USE_TCMALLOC=1 -DCMAKE_BUILD_TYPE=Release 


