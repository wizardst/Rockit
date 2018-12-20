#!/bin/bash

ROCKIT_ROOT=./
if [ -d .git ]
then
    ROCKIT_ROOT=./
fi

if [ -f cpplint.py ]
then
    ROCKIT_ROOT=../..
fi

CHECK_DIR=${ROCKIT_ROOT}/src
if [ -d ${ROCKIT_ROOT}/$1 ] && [ -n "$1" ]; then
    CHECK_DIR=${ROCKIT_ROOT}/$1
fi

echo "run CPPLint in ${CHECK_DIR}"

find ${CHECK_DIR} -name *.cpp | xargs ${ROCKIT_ROOT}/tools/cpplint/cpplint.py --linelength=120 --root=${ROCKIT_ROOT}
find ${CHECK_DIR} -name *.h | xargs ${ROCKIT_ROOT}/tools/cpplint/cpplint.py --linelength=120 --root=${ROCKIT_ROOT}
find ${ROCKIT_ROOT}/include -name *.h | xargs ${ROCKIT_ROOT}/tools/cpplint/cpplint.py --linelength=120 --root=${ROCKIT_ROOT}

