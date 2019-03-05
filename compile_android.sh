#!/bin/bash

export ROCKIT_ROOT=`pwd`

FFMPEG_COMPILE_PATH=third-party/ffmpeg-dev
ROCKIT_COMPILE_PATH=build/android/arm
ROCKIT_COMPILE_SCRIPT=${ROCKIT_COMPILE_PATH}/build.sh

CPU_ARCH=arm32

while getopts "ha:cpf:r:" opt
do
    case $opt in
        h)
        echo "Usage"
        echo "  ./compile_android.sh [options] <value> ..."
        echo ""
        echo "Options:"
        echo "  -a CPU_ARCH, -a arm32, -a arm64"
        echo "                               Target CPU architecture"
        echo "  -c                           Clean all project"
        echo "  -p                           Need cpplit to do code checking"
        echo "  -f                           FFmpeg external compile options"
        echo "  -r                           Rockit external compile options"
        echo "  -o PROJECT, -o rockit, -o ffmpeg"
        echo "                               Build only one of project"
        exit 1;;
        a)
        echo "cpu arch: $OPTARG"
        CPU_ARCH=$OPTARG
        ;;
        c)
        echo "clean all project"
        FFMPEG_OPTIONS="${FFMPEG_OPTIONS} -c"
        ROCKIT_OPTIONS="${ROCKIT_OPTIONS} -c"
        echo "ffmpeg options: ${FFMPEG_OPTIONS} rockit options: ${ROCKIT_OPTIONS}"
        ;;
        p)
        echo "do cpplit check"
        ROCKIT_OPTIONS="-p ${ROCKIT_OPTIONS}"
        ;;
        f)
        echo "ffmpeg external options: $OPTARG"
        FFMPEG_OPTIONS="$OPTARG ${FFMPEG_OPTIONS}"
        ;;
        r)
        echo "rockit external options: $OPTARG"
        exit 1;;
        ?)
        echo "Unknown Options! $opt"
        exit 1;;
    esac
done

# build ffmpeg project
if [ -f ${FFMPEG_COMPILE_PATH}/build-${CPU_ARCH}.sh ]
then
    cd ${FFMPEG_COMPILE_PATH}
    ./build-${CPU_ARCH}.sh ${FFMPEG_OPTIONS}
    if [ $? -ne 0 ]
    then
        echo "ffmpeg build failed!"
        exit 1
    else
        echo "ffmpeg build succeed!"
    fi
    cd -
else
    exit 1
fi

# build rockit
if [ -f ${ROCKIT_COMPILE_SCRIPT} ]
then
    cd ${ROCKIT_COMPILE_PATH}
    ./build.sh ${ROCKIT_OPTIONS}
    if [ $? -ne 0 ]
    then
        echo "Rockit build failed!"
        exit 1
    else
        echo "Rockit build succeed!"
    fi
else
    echo 1
fi
