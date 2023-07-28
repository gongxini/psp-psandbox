#!/bin/bash -x

sudo apt install -y libmysqlcppconn-dev 
# Setup synthetic work library
make -j20 -C submodules/fake_work libfake

# Setup RocksDB
make -j 20 -C submodules/rocksdb static_lib
make -j 20 -C submodules/rocksdb shared_lib

# Setup Perséphone
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DDPDK_MELLANOX_SUPPORT=OFF ..
make -j 20 -C build

sudo mkdir -p /tmpfs
mountpoint -q /tmpfs || sudo mount -t tmpfs -o size=50G,mode=1777 tmpfs /tmpfs
mkdir -p /tmpfs/experiments/
