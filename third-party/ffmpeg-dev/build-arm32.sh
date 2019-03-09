#!/bin/sh

#
# build script for cross compiling ffmpeg for android-arm32
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

TARGET_PLATFORM_LEVEL=23
SYS_ROOT=${HOME_NDK}/platforms/android-${TARGET_PLATFORM_LEVEL}/arch-arm
SYS_HEADER=${HOME_NDK}/sysroot
TOOLCHAINS=${HOME_NDK}/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64

[ -d ${HOME_NDK}   ] && echo "Found ${HOME_NDK}"   || echo "NOT Found:${HOME_NDK}"
[ -d ${SYS_ROOT}   ] && echo "Found ${SYS_ROOT}"   || echo "NOT Found:${SYS_ROOT}"
[ -d ${TOOLCHAINS} ] && echo "Found ${TOOLCHAINS}" || echo "NOT Found:${TOOLCHAINS}"

FFMPG_GIT=https://github.com/FFmpeg/FFmpeg
if [ -d ".git" ];then
    git remote -v
    git branch
else
    git clone ${FFMPG_GIT}
    git checkout origin/release/4.0 -b release/4.0
fi

CPU_ARCH=armv7-a
OS_ARCH=arm
OS_CROSS=${OS_ARCH}-linux-androideabi

# https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html
OPTIMIZE_CFLAGS="-march=${CPU_ARCH} -mtune=cortex-a9"
PREFIX=./android/${CPU_ARCH}

HOME_FFMPEG=${PWD}
HOME_PREBUILT=${HOME_FFMPEG}/../../prebuilt
HEADER_SSL=${HOME_PREBUILT}/headers/boringssl/include
HEADER_OPUS=${HOME_PREBUILT}/headers/libopus/include

echo "${0} -f [ac3]  -- enable AC3/DTS"
if [ "${1}" = "-f" ] && [ "${2}" = "ac3" ];then
FLAG_AC3_DTS=
echo "${0} FLAG_AC3_DTS=@AC3/@DTS"
else
FLAG_AC3_DTS="--disable-decoder=dca     --disable-decoder=ac3 \
              --disable-decoder=eac3    --disable-decoder=atrac3 \
              --disable-decoder=atrac3p --disable-decoder=truehd \
              --disable-decoder=vp6     --disable-decoder=vp6f \
              --disable-decoder=vp6a"
echo "${0} FLAG_AC3_DTS=@NULL"
fi

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
            --disable-shared \
            --disable-debug \
            --enable-static \
            --disable-pthreads \
            --extra-ldflags="-Wl,-rpath-link=${SYS_ROOT}/usr/lib \
                        -L${SYS_ROOT}/usr/lib \
                        -L${PREFIX}/lib \
                        -L${HOME_PREBUILT}/arm32 \
                        -nostdlib -lc -lm -ldl -llog -lssl -lcrypto" \
            --enable-gpl \
            --enable-nonfree \
            --enable-parsers \
            --disable-encoders \
            --enable-decoders \
            --disable-muxers \
            --enable-demuxers \
            --disable-swscale \
            --disable-swscale-alpha \
            --disable-ffmpeg \
            --disable-ffplay \
            --disable-ffprobe \
            --enable-network  \
            --disable-indevs \
            --disable-bsfs \
            --disable-filters \
            --disable-avfilter \
            --enable-openssl \
            --enable-protocols \
            --disable-libopus \
            --enable-asm \
            --enable-neon \
            ${FLAG_AC3_DTS}

# make clean

# update git verison
# ./version.sh

make -j8 install

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
          -L${HOME_PREBUILT}/arm32 \
          -soname libffmpeg_58.so -shared -nostdlib  -z noexecstack -Bsymbolic \
          --whole-archive --no-undefined -o ${PREFIX}/libffmpeg_58.so \
          libavcodec/libavcodec.a libavformat/libavformat.a \
          libavutil/libavutil.a libswresample/libswresample.a \
          -lc -lm -lz -ldl -llog -lssl -lcrypto -lopus \
          --dynamic-linker=/system/bin/linker \
          ${TOOLCHAINS}/lib/gcc/${OS_CROSS}/4.9.x/armv7-a/libgcc.a

# copy new files to prebuilt if linux-android-ld is ok.
if [ $? -eq 0 ]; then
    cp ${PREFIX}/libffmpeg_58.so ${HOME_PREBUILT}/arm32/libffmpeg_58.so -rf
    rm ${HOME_PREBUILT}/headers/ffmpeg-4.0/include -rf
    cp ${PREFIX}/include ${HOME_PREBUILT}/headers/ffmpeg-4.0/ -rf
    ls -alh ${HOME_PREBUILT}/arm32/
fi
