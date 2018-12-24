/*
 * Copyright 2018 Rockchip Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * author: martin.cheng@rock-chips.com
 *   date: 20181221
 * module: render engine for OpenGL-ES2.0
 */

#include "MathMat4.h"   // NOLINT
#include "rt_mem.h"     // NOLINT
#include <math.h>       // NOLINT

MathMat4::MathMat4() {
    this->identity();
}

MathMat4::MathMat4(RT_FLOAT matrix[4][4]) {
    set(matrix);
}

MathMat4::MathMat4(RT_FLOAT m00, RT_FLOAT m01, RT_FLOAT m02, RT_FLOAT m03,
         RT_FLOAT m10, RT_FLOAT m11, RT_FLOAT m12, RT_FLOAT m13,
         RT_FLOAT m20, RT_FLOAT m21, RT_FLOAT m22, RT_FLOAT m23,
         RT_FLOAT m30, RT_FLOAT m31, RT_FLOAT m32, RT_FLOAT m33) {
    mMat4[0][0] = m00; mMat4[0][1] = m01; mMat4[0][2] = m02; mMat4[0][3] = m03;
    mMat4[1][0] = m10; mMat4[1][1] = m11; mMat4[1][2] = m12; mMat4[1][3] = m13;
    mMat4[2][0] = m20; mMat4[2][1] = m21; mMat4[2][2] = m22; mMat4[2][3] = m23;
    mMat4[3][0] = m30; mMat4[3][1] = m31; mMat4[3][2] = m32; mMat4[3][3] = m33;
}

MathMat4::MathMat4(const MathMat4* matrix) {
    set(matrix->mMat4);
}

MathMat4::MathMat4(RT_FLOAT vf) {
    rt_memset(&mMat4, 0x0, sizeof(mMat4));
    mMat4[0][0] = mMat4[1][1] = mMat4[2][2] = vf;
    mMat4[3][3] = 1.0f;
}

void MathMat4::setAtXY(UINT8 idx_x, UINT8 idx_y, RT_FLOAT vf) {
    if ((idx_x < 4) && (idx_y < 4)) {
        mMat4[idx_x][idx_y] = vf;
    }
}

void MathMat4::null() {
    rt_memset(&mMat4, 0x0, sizeof(mMat4));
}

void MathMat4::identity() {
    rt_memset(&mMat4, 0x0, sizeof(mMat4));
    mMat4[0][0] = mMat4[1][1] = mMat4[2][2] = mMat4[3][3] = 1.0f;
}

void MathMat4::transpose() {
    RT_FLOAT temp[4][4] = {0.0f};
    rt_memcpy(temp, this->mMat4, sizeof(temp));
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            this->mMat4[row][col] = temp[col][row];
        }
    }
}

RT_FLOAT utils_radians(const RT_FLOAT degree) {
    return 3.1415f * degree / 180.0f;
}

void MathMat4::rotateX(const RT_FLOAT degree) {
    RT_FLOAT beta = utils_radians(degree);
    RT_FLOAT sr   = sinf(beta);
    RT_FLOAT cr   = cosf(beta);
    this->mMat4[1][1] =  cr;
    this->mMat4[2][1] = -sr;
    this->mMat4[1][2] =  sr;
    this->mMat4[2][2] =  cr;
}

void MathMat4::rotateY(const RT_FLOAT degree) {
    RT_FLOAT beta = utils_radians(degree);
    RT_FLOAT sr   = sinf(beta);
    RT_FLOAT cr   = cosf(beta);
    this->mMat4[0][0] =  cr;
    this->mMat4[2][0] =  sr;
    this->mMat4[0][2] = -sr;
    this->mMat4[2][2] =  cr;
}

void MathMat4::rotateZ(const RT_FLOAT degree) {
    RT_FLOAT beta = utils_radians(degree);
    RT_FLOAT sr   = sinf(beta);
    RT_FLOAT cr   = cosf(beta);
    this->mMat4[0][0] =  cr;
    this->mMat4[1][0] = -sr;
    this->mMat4[0][1] =  sr;
    this->mMat4[1][1] =  cr;
}

void MathMat4::scale(const RT_FLOAT vx, const RT_FLOAT vy, const RT_FLOAT vz) {
    this->mMat4[0][0] = vx;
    this->mMat4[1][1] = vy;
    this->mMat4[2][2] = vz;
}

void MathMat4::translate(const RT_FLOAT vx, const RT_FLOAT vy, const RT_FLOAT vz) {
    this->mMat4[3][0] = vx;
    this->mMat4[3][1] = vy;
    this->mMat4[3][2] = vz;
}

void MathMat4::multiply(const RT_FLOAT matrix[4][4]) {
    RT_FLOAT temp[4][4] = {0.0f};
    rt_memcpy(&temp, this->mMat4, sizeof(temp));
    for (UINT32 idx = 0; idx < 4; idx++) {
        mMat4[idx][0] = (temp[idx][0] * matrix[0][0]) +
                        (temp[idx][1] * matrix[1][0]) +
                        (temp[idx][2] * matrix[2][0]) +
                        (temp[idx][3] * matrix[3][0]);

        mMat4[idx][1] = (temp[idx][0] * matrix[0][1]) +
                        (temp[idx][1] * matrix[1][1]) +
                        (temp[idx][2] * matrix[2][1]) +
                        (temp[idx][3] * matrix[3][1]);

        mMat4[idx][2] = (temp[idx][0] * matrix[0][2]) +
                        (temp[idx][1] * matrix[1][2]) +
                        (temp[idx][2] * matrix[2][2]) +
                        (temp[idx][3] * matrix[3][2]);

        mMat4[idx][3] = (temp[idx][0] * matrix[0][3]) +
                        (temp[idx][1] * matrix[1][3]) +
                        (temp[idx][2] * matrix[2][3]) +
                        (temp[idx][3] * matrix[3][3]);
    }
}

void MathMat4::multiply(const RT_FLOAT vf) {
    for (UINT32 row = 0; row < 4; row++) {
        for (UINT32 col = 0; col < 4; col++) {
            this->mMat4[row][col] *= vf;
        }
    }
}

void MathMat4::add(const RT_FLOAT matrix[4][4]) {
    for (UINT32 row = 0; row < 4; row++) {
        for (UINT32 col = 0; col < 4; col++) {
            this->mMat4[row][col] *= matrix[row][col];
        }
    }
}

void MathMat4::set(const RT_FLOAT matrix[4][4]) {
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            this->mMat4[row][col] = matrix[row][col];
        }
    }
}

void MathMat4::as3x3Array(RT_FLOAT data[9]) {
    data[0] = mMat4[0][0]; data[1] = mMat4[1][0]; data[2] = mMat4[2][0];
    data[3] = mMat4[0][1]; data[4] = mMat4[1][1]; data[5] = mMat4[2][1];
    data[6] = mMat4[0][2]; data[7] = mMat4[1][2]; data[8] = mMat4[2][2];
}

void MathMat4::determinant() {
    // @TODO: Unimplemented
}

void MathMat4::inverse() {
    // @TODO: Unimplemented
}

void MathMat4::adjacent() {
    // @TODO: Unimplemented
}

void MathMat4::adjacent(const MathMat4* mat) {
    // @TODO: Unimplemented
}

void MathMat4::euler(const RT_FLOAT vx, const RT_FLOAT vy, const RT_FLOAT vz, const RT_FLOAT angle) {
    // @TODO: Unimplemented
}
