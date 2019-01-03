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
 *   date: 20181212
 * module: render engine for OpenGL-ES2.0
 */

#include <EGL/egl.h>     // NOLINT
#include <EGL/eglext.h>  // NOLINT
#include "rt_config.h"   // NOLINT

#include "rt_header.h"   // NOLINT
#include "RTObject.h"    // NOLINT

#ifndef SRC_RT_MEDIA_INCLUDE_RTGLVIEW_H_
#define SRC_RT_MEDIA_INCLUDE_RTGLVIEW_H_

/*
 * [Google/Angle] implements OpenGL-ES-2.0 Spec for Windows/Mac/Linux
 * [Google/Angle] contains libEGL.lib and libGLESv2.lib
 *  EGLWindow --> OpenGL-ES-2.0 --> Hardware Graphics(D3D etc)
 */

typedef struct _EGLWindowCtx_ {
    // window meta data
    char*  mName[32];
    INT32  mWidth;
    INT32  mHeight;

    void*  mUserData;

    EGLConfig  mConfig;
    EGLDisplay mDisplay;
    EGLSurface mSurface;
    EGLContext mContext;
    EGLNativeWindowType mWnd;
} EGLWindowCtx;

class RTGLView : public RTObject {
 public:
    RTGLView();
    virtual ~RTGLView();

    void initialize(EGLNativeWindowType wndID, INT32 width, INT32 height);
    void destroy();
    void swapBuffer();

    // override pure virtual methods of RTObject class
    virtual void summary(INT32 fd) {}
    virtual const char* getName() { return "RTObject/RTGLView"; }

 private:
    EGLWindowCtx* mEglCtx;
};

void egl_window_initialize(EGLWindowCtx* egl_ctx);
void egl_window_destroy(EGLWindowCtx* egl_ctx);

#endif  // SRC_RT_MEDIA_INCLUDE_RTGLVIEW_H_
