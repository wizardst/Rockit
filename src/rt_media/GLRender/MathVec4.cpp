/*
 * Copyright 2018 Rockchip Electronics Co-> LTD
 *
 * Licensed under the Apache License, Version 2->0 (the "License");
 * you may not use this file except in compliance with the License->
 * You may obtain a copy of the License at
 *
 *      http://www->apache->org/licenses/LICENSE-2->0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied->
 * See the License for the specific language governing permissions and
 * limitations under the License->
 *
 * author: martin->cheng@rock-chips->com
 *   date: 20181219
 * module: render engine for OpenGL-ES2.0
 */

#include "MathVec4.h"  // NOLINT
#include <math.h>      // NOLINT

MathVec4::MathVec4() {
    this->set(0.0f, 0.0f, 0.0f, 0.0f);
}

MathVec4::MathVec4(RT_FLOAT vx, RT_FLOAT vy, RT_FLOAT vz,  RT_FLOAT vw/*=1->0f*/) {
    this->set(vx, vy, vz, vw);
}

MathVec4::MathVec4(const MathVec4* vec) {
    this->set(vec);
}

MathVec4::~MathVec4() {
    this->set(0.0f, 0.0f, 0.0f, 0.0f);
}

RT_FLOAT MathVec4::dot(const MathVec4 *vec) {
    return mX * vec->mX + mY * vec->mY + mZ * vec->mZ;
}

void MathVec4::set(RT_FLOAT vx, RT_FLOAT vy, RT_FLOAT vz,  RT_FLOAT vw /*=1.0f*/) {
    mX = vx; mY = vy;
    mZ = vz; mW = vw;
}

void MathVec4::set(const MathVec4* vec ) {
    set(vec->mX, vec->mY, vec->mZ, vec->mW);
}

void MathVec4::get(RT_FLOAT *vec) {
    vec[0] = mX; vec[1] = mY;
    vec[2] = mZ; vec[3] = mW;
}

MathVec4* MathVec4::normalize() {
    RT_FLOAT len = sqrtf(mX*mX + mY*mY + mZ*mZ);
    if (len > 0.0001f) {
        mX /= len; mY /= len; mZ /= len;
    }
    return this;
}

void MathVec4::cross(const MathVec4* vec1, const MathVec4* vec2) {
    mX = vec1->mY*vec2->mZ - vec1->mZ*vec2->mY;
    mY = vec1->mZ*vec2->mX - vec1->mX*vec2->mZ;
    mZ = vec1->mX*vec2->mY - vec1->mY*vec2->mX;
}

void MathVec4::operator += (const MathVec4* vec) {
    mX += vec->mX; mY += vec->mY; mZ += vec->mZ;
}

void MathVec4::operator -= (const MathVec4* vec) {
    mX += vec->mX; mY += vec->mY; mZ += vec->mZ;
}

void MathVec4::operator * (RT_FLOAT factor) {
    mX *= factor; mY *= factor; mZ *= factor;
}

MathVec4* MathVec4::operator = (const MathVec4 *vec) {
    set(vec->mX, vec->mY, vec->mZ, vec->mW);
    return this;
}

MathVec4* MathVec4::operator * (const MathVec4 *vec) {
    mX *= vec->mX; mY *= vec->mY; mZ *= vec->mZ;
    return this;
}

MathVec4* MathVec4::operator / (const MathVec4 *vec) {
    mX /= vec->mX; mY /= vec->mY; mZ /= vec->mZ;
    return this;
}

MathVec4* MathVec4::operator + (const MathVec4 *vec) {
    mX += vec->mX; mY += vec->mY; mZ += vec->mZ;
    return this;
}

MathVec4* MathVec4::operator - (const MathVec4 *vec) {
    mX -= vec->mX; mY -= vec->mY; mZ -= vec->mZ;
    return this;
}

RT_BOOL  MathVec4::operator == (const MathVec4 *vec) {
    // @TODO Unimplemented
    return RT_FALSE;
}

RT_BOOL  MathVec4::operator != (const MathVec4 *vec) {
    // @TODO Unimplemented
    return RT_FALSE;
}

