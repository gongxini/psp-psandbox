#!/bin/bash -x

# Setup synthetic work library
make -C submodules/fake_work libfake

# Setup RocksDB
make -C submodules/rocksdb static_lib

# Setup Perséphone
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DDPDK_MELLANOX_SUPPORT=OFF ..
make -j 4 -C build

sudo mkdir -p /tmpfs
mountpoint -q /tmpfs || sudo mount -t tmpfs -o size=50G,mode=1777 tmpfs /tmpfs
mkdir -p /tmpfs/experiments/
