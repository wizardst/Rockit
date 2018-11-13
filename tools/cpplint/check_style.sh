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

find ${ROCKIT_ROOT} -name *.cpp | xargs ${ROCKIT_ROOT}/tools/cpplint/cpplint.py --linelength=120 --root=${ROCKIT_ROOT}
find ${ROCKIT_ROOT} -name *.h | xargs ${ROCKIT_ROOT}/tools/cpplint/cpplint.py --linelength=120 --root=${ROCKIT_ROOT}

