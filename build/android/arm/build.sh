#!/bin/bash

if [ "$1" = "clear" -o "$1" = "-c" ];
then
    ls | grep -v "build.sh" | xargs rm -rf
    echo "clean succeed!";
    exit 1
fi

ANDROID_NDK=~/ndk10/android-ndk-r10e/

PLATFORM=$ANDROID_NDK/platforms/android-21/arch-arm

cmake ../../../                                                             \
      -DCMAKE_TOOLCHAIN_FILE=../android.toolchain.cmake                     \
      -DCMAKE_BUILD_TYPE=Debug                                              \
      -DANDROID_FORCE_ARM_BUILD=ON                                          \
      -DANDROID_NDK=${ANDROID_NDK}                                          \
      -DANDROID_SYSROOT=${PLATFORM}                                         \
      -DANDROID_ABI="armeabi-v7a with NEON"                                 \
      -DANDROID_TOOLCHAIN_NAME="arm-linux-androideabi-4.8"                  \
      -DANDROID_NATIVE_API_LEVEL=android-21                                 \
      -DOS_LINUX=ON


# ----------------------------------------------------------------------------
# usefull cmake debug flag
# ----------------------------------------------------------------------------
      #-DMPP_NAME="rockchip_mpp"                                             \
      #-DVPU_NAME="rockchip_vpu"                                             \
      #-DHAVE_DRM                                                            \
      #-DCMAKE_BUILD_TYPE=Debug                                              \
      #-DCMAKE_VERBOSE_MAKEFILE=true                                         \
      #--trace                                                               \
      #--debug-output                                                        \

#cmake --build . --clean-first -- V=1
cmake --build .
