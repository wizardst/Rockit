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
 *   date: 20181218
 * module: render engine for OpenGL-ES2.0
 */

#include "RTCamera.h"   // NOLINT

#include <math.h>       // NOLINT
#include "MathMat4.h"   // NOLINT
#include "rt_mem.h"     // NOLINT
#include "rt_debug.h"   // NOLINT

#define CAMERA_NEAR_MIN 0.001f
#define PI 3.1415f

RTCamera::RTCamera() {
    mPerspective = new MathMat4();
    mPerspective->identity();
    mModelView = new MathMat4();
    mModelView->identity();
}

RTCamera::~RTCamera() {
    rt_safe_delete(mPerspective);
    rt_safe_delete(mModelView);
}

// perspective matrix for projection transformation
void RTCamera::setFrustum(RT_FLOAT left,  RT_FLOAT right, RT_FLOAT bottom, RT_FLOAT top,
                    RT_FLOAT nearZ, RT_FLOAT farZ) {
    RT_ASSERT(RT_NULL != mPerspective);

    RT_FLOAT deltaX = right - left;
    RT_FLOAT deltaY = top - bottom;
    RT_FLOAT deltaZ = farZ - nearZ;
    RT_FLOAT frust[4][4];

    if ( (nearZ <= 0.0f) || (farZ <= 0.0f) ||
         (deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f) )
         return;

    frust[0][0] = 2.0f * nearZ / deltaX;
    frust[0][1] = frust[0][2] = frust[0][3] = 0.0f;

    frust[1][1] = 2.0f * nearZ / deltaY;
    frust[1][0] = frust[1][2] = frust[1][3] = 0.0f;

    frust[2][0] = (right + left) / deltaX;
    frust[2][1] = (top + bottom) / deltaY;
    frust[2][2] = -(nearZ + farZ) / deltaZ;
    frust[2][3] = -1.0f;

    frust[3][2] = -2.0f * nearZ * farZ / deltaZ;
    frust[3][0] = frust[3][1] = frust[3][3] = 0.0f;

    if (RT_NULL != mPerspective) {
        mPerspective->identity();
        mPerspective->multiply(frust);
    }
}

void RTCamera::setOrtho(RT_FLOAT left,  RT_FLOAT right, RT_FLOAT bottom, RT_FLOAT top,
                    RT_FLOAT nearZ, RT_FLOAT farZ) {
    RT_FLOAT deltaX = right - left;
    RT_FLOAT deltaY = top - bottom;
    RT_FLOAT deltaZ = farZ - nearZ;
    RT_FLOAT ortho[4][4];

    if ( (deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f) )
        return;

    // make Matrix4x4 identity
    rt_memset(&ortho, 0x0, sizeof(ortho));
    ortho[0][0] = ortho[1][1] = ortho[2][2] = ortho[3][3] = 1.0f;

    // set Ortho to Matrix4x4
    ortho[0][0] = 2.0f / deltaX;
    ortho[3][0] = -(right + left) / deltaX;
    ortho[1][1] = 2.0f / deltaY;
    ortho[3][1] = -(top + bottom) / deltaY;
    ortho[2][2] = -2.0f / deltaZ;
    ortho[3][2] = -(nearZ + farZ) / deltaZ;

    if (RT_NULL != mPerspective) {
        mPerspective->identity();
        mPerspective->multiply(ortho);
    }
}

void RTCamera::setPerspective(RT_FLOAT viewAngle, RT_FLOAT aspect, RT_FLOAT nearZ, RT_FLOAT farZ) {
    RT_FLOAT frustumW, frustumH;

    frustumH = tanf(viewAngle / 360.0f * PI) * nearZ;
    frustumW = frustumH * aspect;

    setFrustum(-frustumW, frustumW, -frustumH, frustumH, nearZ, farZ);
}

void RTCamera::setModelScale(RT_FLOAT sx, RT_FLOAT sy, RT_FLOAT sz) {
    mModelView->identity();
    RT_FLOAT (*matrix)[4] = (mModelView->mMat4);
    matrix[0][0] *= sx;
    matrix[0][1] *= sx;
    matrix[0][2] *= sx;
    matrix[0][3] *= sx;

    matrix[1][0] *= sy;
    matrix[1][1] *= sy;
    matrix[1][2] *= sy;
    matrix[1][3] *= sy;

    matrix[2][0] *= sz;
    matrix[2][1] *= sz;
    matrix[2][2] *= sz;
    matrix[2][3] *= sz;
}

void RTCamera::setModelRotate(RT_FLOAT rx, RT_FLOAT ry, RT_FLOAT rz, RT_FLOAT angle) {
    RT_FLOAT sinAngle, cosAngle;
    RT_FLOAT mag = sqrtf(rx * rx + ry * ry + rz * rz);

    sinAngle = sinf(angle * PI / 180.0f);
    cosAngle = cosf(angle * PI / 180.0f);
    if ( mag > 0.0f ) {
        RT_FLOAT xx, yy, zz, xy, yz, zx, xs, ys, zs;
        RT_FLOAT oneMinusCos;
        RT_FLOAT matrix[4][4];

        rx /= mag; ry /= mag; rz /= mag;

        xx = rx * rx; yy = ry * ry; zz = rz * rz;
        xy = rx * ry; yz = ry * rz; zx = rz * rx;
        xs = rx * sinAngle; ys = ry * sinAngle; zs = rz * sinAngle;
        oneMinusCos = 1.0f - cosAngle;

        matrix[0][0] = (oneMinusCos * xx) + cosAngle;
        matrix[0][1] = (oneMinusCos * xy) - zs;
        matrix[0][2] = (oneMinusCos * zx) + ys;
        matrix[0][3] = 0.0F;

        matrix[1][0] = (oneMinusCos * xy) + zs;
        matrix[1][1] = (oneMinusCos * yy) + cosAngle;
        matrix[1][2] = (oneMinusCos * yz) - xs;
        matrix[1][3] = 0.0F;

        matrix[2][0] = (oneMinusCos * zx) - ys;
        matrix[2][1] = (oneMinusCos * yz) + xs;
        matrix[2][2] = (oneMinusCos * zz) + cosAngle;
        matrix[2][3] = 0.0F;

        matrix[3][0] = 0.0F;
        matrix[3][1] = 0.0F;
        matrix[3][2] = 0.0F;
        matrix[3][3] = 1.0F;

        if (RT_NULL != mModelView) {
            mModelView->identity();
            mModelView->multiply(matrix);
        }
    }
}

void RTCamera::setModelTranslate(RT_FLOAT tx, RT_FLOAT ty, RT_FLOAT tz) {
    RT_FLOAT (*matrix)[4] = (mModelView->mMat4);
    matrix[3][0] += (matrix[0][0] * tx + matrix[1][0] * ty + matrix[2][0] * tz);
    matrix[3][1] += (matrix[0][1] * tx + matrix[1][1] * ty + matrix[2][1] * tz);
    matrix[3][2] += (matrix[0][2] * tx + matrix[1][2] * ty + matrix[2][2] * tz);
    matrix[3][3] += (matrix[0][3] * tx + matrix[1][3] * ty + matrix[2][3] * tz);
}

void RTCamera::getShaderMatric(RT_FLOAT matrix[4][4]) {
    RT_FLOAT (*src_mat_a)[4] = (mModelView->mMat4);
    RT_FLOAT (*src_mat_b)[4] = (mPerspective->mMat4);
    for (UINT32 idx = 0; idx < 4; idx++) {
        matrix[idx][0] = (src_mat_a[idx][0] * src_mat_b[0][0]) +
                         (src_mat_a[idx][1] * src_mat_b[1][0]) +
                         (src_mat_a[idx][2] * src_mat_b[2][0]) +
                         (src_mat_a[idx][3] * src_mat_b[3][0]);

        matrix[idx][1] = (src_mat_a[idx][0] * src_mat_b[0][1]) +
                         (src_mat_a[idx][1] * src_mat_b[1][1]) +
                         (src_mat_a[idx][2] * src_mat_b[2][1]) +
                         (src_mat_a[idx][3] * src_mat_b[3][1]);

        matrix[idx][2] = (src_mat_a[idx][0] * src_mat_b[0][2]) +
                         (src_mat_a[idx][1] * src_mat_b[1][2]) +
                         (src_mat_a[idx][2] * src_mat_b[2][2]) +
                         (src_mat_a[idx][3] * src_mat_b[3][2]);

        matrix[idx][3] = (src_mat_a[idx][0] * src_mat_b[0][3]) +
                         (src_mat_a[idx][1] * src_mat_b[1][3]) +
                         (src_mat_a[idx][2] * src_mat_b[2][3]) +
                         (src_mat_a[idx][3] * src_mat_b[3][3]);
    }
}
