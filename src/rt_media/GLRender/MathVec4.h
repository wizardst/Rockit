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
 */

#ifndef SRC_RT_MEDIA_GLRENDER_MATHVEC4_H_
#define SRC_RT_MEDIA_GLRENDER_MATHVEC4_H_

#include "rt_type.h"   // NOLINT

class MathMat4;
class MathVec4 {
 public:
    MathVec4();
    explicit MathVec4(RT_FLOAT vx, RT_FLOAT vy, RT_FLOAT vz, RT_FLOAT vw = 1.0f);
    explicit MathVec4(const MathVec4* vec);
    ~MathVec4();

 public:
    RT_FLOAT dot(const MathVec4 *vec);
    void set(RT_FLOAT vx, RT_FLOAT vy, RT_FLOAT vz, RT_FLOAT vw = 1.0f);
    void set(const MathVec4* vec);
    void get(RT_FLOAT vec[4]);
    MathVec4* normalize();
    void      cross(const MathVec4* vec1, const MathVec4* vec2);

    // @DONE: override operators
    void operator += (const MathVec4* vec);
    void operator -= (const MathVec4* vec);
    void operator * (RT_FLOAT factor);
    MathVec4* operator = (const MathVec4 *vec);
    MathVec4* operator * (const MathVec4 *vec);
    MathVec4* operator / (const MathVec4 *vec);
    MathVec4* operator + (const MathVec4 *vec);
    MathVec4* operator - (const MathVec4 *vec);

    // @TODO: Unimplemented
    RT_BOOL  operator == (const MathVec4 *vec);
    RT_BOOL  operator != (const MathVec4 *vec);

 public:
    RT_FLOAT mX, mY, mZ, mW;
};

#endif  // SRC_RT_MEDIA_GLRENDER_MATHVEC4_H_
