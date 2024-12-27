#!/bin/bash -x

# sysctl configs
sudo sysctl kernel.nmi_watchdog=0
sudo sysctl -w kernel.watchdog=0

# Setup huge pages
sudo sh -c 'echo 8192 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages;'

# Disable turbo
sudo scripts/setup/turbo.sh disable
# Unbind the NIC from the kernel driver
sudo submodules/dpdk/usertools/dpdk-devbind.py --force -u 18:00.1
# Load uio to bypass the kernel and use the NIC. Need the module built for the kernel in use
sudo modprobe uio
sudo insmod submodules/dpdk/x86_64-native-linuxapp-gcc/build/kernel/linux/igb_uio/igb_uio.ko
sudo submodules/dpdk/usertools/dpdk-devbind.py -b igb_uio 18:00.1


sudo mkdir -p /tmpfs
mountpoint -q /tmpfs || sudo mount -t tmpfs -o size=50G,mode=1777 tmpfs /tmpfs
mkdir -p /tmpfs/experiments/
