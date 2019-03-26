#!/bin/bash

# Define and Check NDK toolchain
# ANDROID_NDK=/disk2/cmc/android_projects/android_build_env/android-ndk-r10d
ANDROID_NDK=~/ndk10/android-ndk-r10e/

CLEAN_PROJECT=0
CPPLIT_CHECK=0

while getopts "cp" opt
do
    case $opt in
        c)
        echo "clean rockit project"
        CLEAN_PROJECT=1
        ;;
        p)
        echo "open cpplit check"
        CPPLIT_CHECK=1
        ;;
        ?)
        echo "Rockit build script Unknown Options! $opt"
        exit 1;;
    esac
done

# Clean Project
if [ $CLEAN_PROJECT -eq 1 ];
then
    ls | grep -v "build.sh" | xargs rm -rf
    echo "clean succeed!";
    exit 0
fi

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

# OS_LINUX + HAVE_NATIVE = ANDROID_NDK
cmake   -DCMAKE_TOOLCHAIN_FILE=../android.toolchain.cmake                     \
        -DCMAKE_BUILD_TYPE=Debug                                              \
        -DANDROID_FORCE_ARM_BUILD=ON                                          \
        -DANDROID_NDK=${ANDROID_NDK}                                          \
        -DANDROID_SYSROOT=${PLATFORM}                                         \
        -DANDROID_ABI="armeabi-v7a with NEON"                                 \
        -DANDROID_TOOLCHAIN_NAME="arm-linux-androideabi-4.8"                  \
        -DANDROID_NATIVE_API_LEVEL=android-21                                 \
        -DHAVE_MPI=OFF                                                        \
        -DHAVE_GLES=ON                                                        \
        -DHAVE_DRM=ON                                                         \
        -DHAVE_NDK=ON                                                         \
        -DOS_LINUX=ON                                                         \
        -DOS_ANDROID=ON                                                       \
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

if [ ${CPPLIT_CHECK} -eq 1 ]
then
    # Code Style Check using cpplint
    echo "Code Style Check using cpplint ..."
    if [ -f ${ROCKIT_ROOT}/tools/cpplint/cpplint.py ]
    then
        find ${ROCKIT_ROOT}/src -name *.cpp | xargs ${ROCKIT_ROOT}/tools/cpplint/cpplint.py --linelength=120 --root=${ROCKIT_ROOT}
        find ${ROCKIT_ROOT}/src -name *.h | xargs ${ROCKIT_ROOT}/tools/cpplint/cpplint.py --linelength=120 --root=${ROCKIT_ROOT}
        find ${ROCKIT_ROOT}/include -name *.h | xargs ${ROCKIT_ROOT}/tools/cpplint/cpplint.py --linelength=120 --root=${ROCKIT_ROOT}
    fi
    echo -e "Code Style Check using cpplint ...DONE!\r\n"
fi

#cmake --build . --clean-first -- V=1
cmake --build .
