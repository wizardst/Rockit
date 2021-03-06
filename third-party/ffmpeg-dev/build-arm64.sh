#! /usr/bin/env bash
#
# build script for cross compiling ffmpeg for android-arm64
# Just configure the ${HOME_NDK}
# Android 7.0 (API level 24).
# Android 6.0 (API level 23).
# Android 5.1 (API level 22).
#
# @Todo: remove compilation warning
# @Todo: verify default opus decoder availability
# @Todo: deep detection for android level compatibility
#
# ${HOME_NDK}/sysroot/usr/include/media
# ${HOME_NDK}/platforms/android-XX/arch-arm/usr/lib/libmediandk.so
#

HOME_NDK=/home/mid_sdk/android-ndk-r16b
HOME_NDK=~/ndk16/android-ndk-r16b

FFMPEG_ROOT=`pwd`
ROCKIT_ROOT=${FFMPEG_ROOT}/../../

TARGET_PLATFORM_LEVEL=23
SYS_ROOT=${HOME_NDK}/platforms/android-${TARGET_PLATFORM_LEVEL}/arch-arm64
SYS_HEADER=${HOME_NDK}/sysroot
TOOLCHAINS=${HOME_NDK}/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64

[ -d ${HOME_NDK}   ] && echo "Found ${HOME_NDK}"   || echo "NOT Found:${HOME_NDK}"
[ -d ${SYS_ROOT}   ] && echo "Found ${SYS_ROOT}"   || echo "NOT Found:${SYS_ROOT}"
[ -d ${TOOLCHAINS} ] && echo "Found ${TOOLCHAINS}" || echo "NOT Found:${TOOLCHAINS}"

echo "build ffmpeg project."

CLEAN_PROJECT=0
MODULE_CONFIG_FILE_NAME="module_config_default.sh"

while getopts "cf:" opt
do
    case $opt in
        c)
        echo "clean ffmpeg project"
        CLEAN_PROJECT=1
        ;;
        f)
        echo "select config: module_config_$OPTARG.sh"
        MODULE_CONFIG_FILE_NAME="module_config_$OPTARG.sh"
        ;;
        ?)
        echo "Unknown Options! $opt"
        exit 1;;
    esac
done

FFMPG_GIT=https://github.com/FFmpeg/FFmpeg
if [ -d "${FFMPEG_ROOT}/FFmpeg/.git" ];
then
    git remote -v
    git branch
else
    git clone ${FFMPG_GIT}
    git checkout origin/release/4.0 -b release/4.0
fi

CPU_ARCH=armv8-a
OS_ARCH=aarch64
OS_CROSS=${OS_ARCH}-linux-android

# https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html
OPTIMIZE_CFLAGS="-march=${CPU_ARCH} -mtune=cortex-a53"
PREFIX=${FFMPEG_ROOT}/FFmpeg/android/${CPU_ARCH}

HOME_FFMPEG=${PWD}
HOME_PREBUILT=${FFMPEG_ROOT}/../prebuilt
HEADER_SSL=${HOME_PREBUILT}/headers/boringssl/include
HEADER_OPUS=${HOME_PREBUILT}/headers/libopus/include

# ffmpeg build params
if [ -f ${FFMPEG_ROOT}/config/${MODULE_CONFIG_FILE_NAME} ]
then
    export COMMON_FF_CFG_FLAGS=
    source ${FFMPEG_ROOT}/config/${MODULE_CONFIG_FILE_NAME}
else
    echo "Invail config file name: ${MODULE_CONFIG_FILE_NAME}"
    exit 1
fi

cd ${FFMPEG_ROOT}/FFmpeg/

if [ ${CLEAN_PROJECT} -eq 1 ]
then
    make clean
    exit 0
fi

echo ${COMMON_FF_CFG_FLAGS}

./configure --target-os=android \
            --prefix=${PREFIX} \
            --enable-cross-compile \
            --extra-libs="-lgcc" \
            --arch=${OS_ARCH} \
            --cc=${TOOLCHAINS}/bin/${OS_CROSS}-gcc \
            --cross-prefix=${TOOLCHAINS}/bin/${OS_CROSS}- \
            --nm=${TOOLCHAINS}/bin/${OS_CROSS}-nm \
            --sysroot=${SYS_ROOT} \
            --extra-cflags="-I${SYS_HEADER}/usr/include \
                        -I${SYS_HEADER}/usr/include/${OS_CROSS} \
                        -I${PREFIX}/include \
                        -I${HEADER_SSL} -I${HEADER_OPUS} \
                        -O3 -fpic -fasm -std=c99 \
                        -DANDROID \
                        -D__ANDROID_API__=${TARGET_PLATFORM_LEVEL} \
                        -DHAVE_BORINGSSL \
                        -DHAVE_SYS_UIO_H=1 \
                        -Dipv6mr_interface=ipv6mr_ifindex \
                        -Wno-attributes -Wno-unused-function \
                        -Wno-psabi -fno-short-enums \
                        -fno-strict-aliasing \
                        -finline-limit=300 ${OPTIMIZE_CFLAGS}" \
            --extra-ldflags="-Wl,-rpath-link=${SYS_ROOT}/usr/lib \
                        -L${SYS_ROOT}/usr/lib \
                        -L${PREFIX}/lib \
                        -L${HOME_PREBUILT}/arm64 \
                        -nostdlib -lc -lm -ldl -llog -lssl -lcrypto" \
            --enable-asm \
            --enable-neon \
            ${COMMON_FF_CFG_FLAGS}

# make clean

# update git verison
# ./version.sh

# build ffmpeg library if config is ok
if [ $? -eq 0 ]; then
    make -j8 install
fi

#Binutils supports 2 linkers, ld.gold and ld.bfd.  One of them is
#configured as the default linker, ld, which is used by GCC.  Sometimes,
#we want to use the alternate linker with GCC at run-time.  This patch
#adds -fuse-ld=bfd and -fuse-ld=gold options to GCC driver.  It changes
#collect2.c to pick either ld.bfd or ld.gold.

ls ${TOOLCHAINS}/bin/${OS_CROSS}-ar
ls $TOOLCHAINS/bin/${OS_CROSS}-ld
ls ${TOOLCHAINS}/lib/gcc/${OS_CROSS}/4.9.x/libgcc.a

${TOOLCHAINS}/bin/${OS_CROSS}-ar d libavcodec/libavcodec.a inverse.o

${TOOLCHAINS}/bin/${OS_CROSS}-ld \
          -rpath-link=${SYS_ROOT}/usr/lib \
          -L${SYS_ROOT}/usr/lib \
          -L${HOME_PREBUILT}/arm64 \
          -soname libffmpeg_58.so -shared -nostdlib  -z noexecstack -Bsymbolic \
          --whole-archive --no-undefined -o ${PREFIX}/libffmpeg_58.so \
          libavcodec/libavcodec.a libavformat/libavformat.a \
          libavutil/libavutil.a libswresample/libswresample.a \
          -lc -lm -lz -ldl -llog -lssl -lcrypto -lopus \
          --dynamic-linker=/system/bin/linker \
          ${TOOLCHAINS}/lib/gcc/${OS_CROSS}/4.9.x/libgcc.a

# copy new files to prebuilt if linux-android-ld is ok.
if [ $? -eq 0 ]; then
    cp ${PREFIX}/libffmpeg_58.so ${HOME_PREBUILT}/arm64/libffmpeg_58.so -rf
    rm ${HOME_PREBUILT}/headers/ffmpeg-4.0/include -rf
    cp ${PREFIX}/include ${HOME_PREBUILT}/headers/ffmpeg-4.0/ -rf
    ls -alh ${HOME_PREBUILT}/arm64/
fi

cd -
