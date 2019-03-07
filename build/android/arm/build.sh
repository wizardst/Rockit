#!/bin/bash

# Clean Project
if [ "$1" = "clean" -o "$1" = "-c" ];
then
    ls | grep -v "build.sh" | xargs rm -rf
    echo "clean succeed!";
    exit 1
fi

# Define and Check NDK toolchain
ANDROID_NDK=/disk2/cmc/android_projects/android_build_env/android-ndk-r10d
#ANDROID_NDK=~/ndk10/android-ndk-r10e/

if [ -d ${ANDROID_NDK} ]
then
    echo "Found NDK: ${ANDROID_NDK}"
else
    echo "Please define [ANDROID_NDK]"
    echo -e "Please check folder exists: ${ANDROID_NDK}\r\n"
    exit 1
fi

ROCKIT_PWD=`pwd`
ROCKIT_TOP=${ROCKIT_PWD}/../../..

PLATFORM=$ANDROID_NDK/platforms/android-21/arch-arm

cmake   -DCMAKE_TOOLCHAIN_FILE=../android.toolchain.cmake                     \
        -DCMAKE_BUILD_TYPE=Debug                                              \
        -DANDROID_FORCE_ARM_BUILD=ON                                          \
        -DANDROID_NDK=${ANDROID_NDK}                                          \
        -DANDROID_SYSROOT=${PLATFORM}                                         \
        -DANDROID_ABI="armeabi-v7a with NEON"                                 \
        -DANDROID_TOOLCHAIN_NAME="arm-linux-androideabi-4.8"                  \
        -DANDROID_NATIVE_API_LEVEL=android-21                                 \
        -DHAVE_PTHREAD=ON                                                     \
        -DHAVE_MPI=OFF                                                        \
        -DHAVE_GLES=ON                                                        \
        -DHAVE_DRM=ON                                                         \
        -DOS_LINUX=ON                                                         \
        -DFFMPEG_SINGLE=ON                                                    \
        ${ROCKIT_TOP}

# ----------------------------------------------------------------------------
# usefull cmake debug flag
# ----------------------------------------------------------------------------
      #-DHAVE_DRM                                                            \
      #-DCMAKE_BUILD_TYPE=Debug                                              \
      #-DCMAKE_VERBOSE_MAKEFILE=true                                         \
      #--trace                                                               \
      #--debug-output                                                        \

# Code Style Check using cpplint
echo "Code Style Check using cpplint ..."
ROCKIT_ROOT=../../..
if [ -f ${ROCKIT_ROOT}/tools-x/cpplint/cpplint.py ]
then
    find ${ROCKIT_ROOT}/src -name *.cpp | xargs ${ROCKIT_ROOT}/tools/cpplint/cpplint.py --linelength=120 --root=${ROCKIT_ROOT}
    find ${ROCKIT_ROOT}/src -name *.h | xargs ${ROCKIT_ROOT}/tools/cpplint/cpplint.py --linelength=120 --root=${ROCKIT_ROOT}
    find ${ROCKIT_ROOT}/include -name *.h | xargs ${ROCKIT_ROOT}/tools/cpplint/cpplint.py --linelength=120 --root=${ROCKIT_ROOT}
fi
echo -e "Code Style Check using cpplint ...DONE!\r\n"

#cmake --build . --clean-first -- V=1
cmake --build .
