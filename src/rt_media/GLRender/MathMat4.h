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

#ifndef SRC_RT_MEDIA_GLRENDER_MATHMAT4_H_
#define SRC_RT_MEDIA_GLRENDER_MATHMAT4_H_

#include "rt_type.h"  // NOLINT

class MathMat4 {
 public:
    MathMat4();
    explicit MathMat4(RT_FLOAT matrix[4][4]);
    explicit MathMat4(RT_FLOAT m00, RT_FLOAT m01, RT_FLOAT m02, RT_FLOAT m03,
             RT_FLOAT m10, RT_FLOAT m11, RT_FLOAT m12, RT_FLOAT m13,
             RT_FLOAT m20, RT_FLOAT m21, RT_FLOAT m22, RT_FLOAT m23,
             RT_FLOAT m30, RT_FLOAT m31, RT_FLOAT m32, RT_FLOAT m33);
    explicit MathMat4(const MathMat4* matrix);
    explicit MathMat4(RT_FLOAT vf);

    void    setAtXY(UINT8 idx_x, UINT8 idx_y, RT_FLOAT vf);
    void       null();
    void   identity();
    void  transpose();
    void    rotateX(const RT_FLOAT degree);
    void    rotateY(const RT_FLOAT degree);
    void    rotateZ(const RT_FLOAT degree);
    void      scale(const RT_FLOAT vx, const RT_FLOAT vy, const RT_FLOAT vz);
    void  translate(const RT_FLOAT vx, const RT_FLOAT vy, const RT_FLOAT vz);
    void   multiply(const RT_FLOAT matrix[4][4]);
    void   multiply(const RT_FLOAT vf);
    void        add(const RT_FLOAT matrix[4][4]);
    void        set(const RT_FLOAT matrix[4][4]);
    void as3x3Array(RT_FLOAT data[9]);

    // @TODO: Unimplemented
    void determinant();
    void     inverse();
    void    adjacent();
    void    adjacent(const MathMat4* mat);
    void       euler(const RT_FLOAT vx, const RT_FLOAT vy, const RT_FLOAT vz, const RT_FLOAT angle);

 public:
    RT_FLOAT mMat4[4][4];
};

#endif  // SRC_RT_MEDIA_GLRENDER_MATHMAT4_H_
