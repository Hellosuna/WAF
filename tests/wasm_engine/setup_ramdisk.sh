#!/bin/bash

# 创建目录
sudo mkdir -p /mnt/ramdisk_segment_0
sudo mkdir -p /mnt/ramdisk_segment_1

# 挂载 tmpfs 到目录
sudo mount -t tmpfs -o size=64M tmpfs /mnt/ramdisk_segment_0
sudo mount -t tmpfs -o size=64M tmpfs /mnt/ramdisk_segment_1

# 验证挂载是否成功
echo "Mounted filesystems:"
mount | grep tmpfs

# 输出调试信息
echo "Host to Docker path: /mnt/ramdisk_segment_0/host_to_docker"
echo "Docker to Host path: /mnt/ramdisk_segment_0/docker_to_host"
echo "Host to Docker path: /mnt/ramdisk_segment_1/host_to_docker"
echo "Docker to Host path: /mnt/ramdisk_segment_1/docker_to_host"
