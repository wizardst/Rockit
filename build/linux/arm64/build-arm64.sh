#!/bin/bash
# Run this from within a bash shell

set +e

# Clean Project
if [ "$1" = "clean" -o "$1" = "-c" -o "$1" = "-clean" ];
then
    ls | grep -v "build-arm64.sh" | xargs rm -rf
    echo "clean succeed!";
    exit 1
fi

ROCKIT_PWD=`pwd`
ROCKIT_TOP=${ROCKIT_PWD}/../../..

# toolchain detection
check_cmd(){
    "$@" >> /dev/null 2>&1
}

check_system_arm_linux_gcc(){
    check_cmd aarch64-linux-gnu-gcc -v
}

check_system_arm_linux_gcc
if [ $? -eq 127 ];then
    ROCKIT_TOOLCHAIN=/usr/bin
    export PATH=$PATH:${ROCKIT_TOOLCHAIN}
fi

# generate Makefile
cmake   -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_TOOLCHAIN_FILE=../linux.cross.cmake              \
        -DHAVE_PTHREAD=ON                                         \
        -DHAVE_MPI=OFF                                           \
        -DHAVE_GLES=OFF                                          \
        -DHAVE_DRM=OFF                                           \
        -DOS_LINUX=ON                                            \
        -DLINUX_TOOLCHAIN:STRING="aarch64-rockchip-linux-gnu"    \
        -DLINUX_ABI:STRING="arm64-v8a"                           \
        ${ROCKIT_TOP}

make -j1
