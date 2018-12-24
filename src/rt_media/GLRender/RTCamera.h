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

 /*
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   glFrustum (-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
   glMatrixMode (GL_MODELVIEW);

   glClear (GL_COLOR_BUFFER_BIT);
   glColor3f (1.0, 1.0, 1.0);
   glLoadIdentity ();

   gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
   glScalef (1.0, 2.0, 1.0);
   glutWireCube (1.0);
   glFlush ();
 */

#ifndef SRC_RT_MEDIA_GLRENDER_RTCAMERA_H_
#define SRC_RT_MEDIA_GLRENDER_RTCAMERA_H_

#include "rt_type.h"  // NOLINT

class MathVec4;
class MathMat4;
class RTCamera {
 public:
    RTCamera();
    ~RTCamera();

 public:
    // perspective matrix for projection transformation
    void setFrustum(RT_FLOAT left,  RT_FLOAT right, RT_FLOAT bottom, RT_FLOAT top,
                    RT_FLOAT nearZ, RT_FLOAT farZ);
    void setOrtho(RT_FLOAT left,  RT_FLOAT right, RT_FLOAT bottom, RT_FLOAT top,
                    RT_FLOAT nearZ, RT_FLOAT farZ);
    void setPerspective(RT_FLOAT viewAngle, RT_FLOAT aspect, RT_FLOAT near, RT_FLOAT far);

    // model matrix for view transformation
    void     setModelScale(RT_FLOAT sx, RT_FLOAT sy, RT_FLOAT sz);
    void    setModelRotate(RT_FLOAT vx, RT_FLOAT vy, RT_FLOAT vz, RT_FLOAT angle);
    void setModelTranslate(RT_FLOAT vx, RT_FLOAT vy, RT_FLOAT vz);
    void   getShaderMatric(RT_FLOAT matrix[4][4]);

 private:
    MathMat4* mPerspective;
    MathMat4* mModelView;
    RT_FLOAT mWidth, mHeight;
    RT_FLOAT mViewAngle, mAspect, mNear, mFar;
};

#endif  // SRC_RT_MEDIA_GLRENDER_RTCAMERA_H_
