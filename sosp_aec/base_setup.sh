#!/bin/bash -x

# Setup synthetic work library
#make -C ${AE_DIR}/Persephone/submodules/fake_work libfake

# Setup RocksDB
#make -j -C ${AE_DIR}/Persephone/submodules/rocksdb static_lib
pushd $(dirname $0)/../submodules/rocksdb
git checkout v5.15.10
CFLAGS="\
-Wno-deprecated-copy \
-Wno-pessimizing-move \
-Wno-error \
-Wno-redundant-move" \
make -j$(nproc) static_lib
popd

# Setup Perséphone
mkdir build
pushd build
cmake -DCMAKE_BUILD_TYPE=Release -DDPDK_MELLANOX_SUPPORT=ON ../
make -j$(NPROC)
popd

# Setup Shinjuku
#cd ${AE_DIR}/Persephone/submodules/shinjuku
#${AE_DIR}/Persephone/submodules/shinjuku/deps/fetch-deps.sh
#sudo rmmod pcidma
#sudo rmmod dune
#sudo make -sj -C deps/dune
#make -sj -C deps/pcidma
#make -sj -C deps/dpdk config T=x86_64-native-linuxapp-gcc
#cd ${AE_DIR}/Persephone/submodules/shinjuku/deps/dpdk
#git apply ${AE_DIR}/Persephone/submodules/shinjuku/deps/dpdk_i40e.patch
#git apply ${AE_DIR}/Persephone/submodules/shinjuku/deps/dpdk_mk.patch
#cd ${AE_DIR}/Persephone/submodules/shinjuku
#make -sj -C deps/dpdk
#cd ${AE_DIR}/Persephone/submodules/shinjuku/deps/rocksdb
#git apply ${AE_DIR}/Persephone/submodules/shinjuku/deps/rocksdb.patch
#cd ${AE_DIR}/Persephone/submodules/shinjuku/
#make -sj -C deps/rocksdb static_lib
#make -sj -C deps/opnew
#make -j
## Setup the RocksDB database creation utility
#make -C db create_db

sudo mkdir -p /tmpfs
mountpoint -q /tmpfs || sudo mount -t tmpfs -o size=50G,mode=1777 tmpfs /tmpfs
mkdir -p /tmpfs/experiments/
