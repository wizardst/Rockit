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
 * @ANGLE is an open source project launched by Google.
 * @ANGLE maps the OpenGL ES 2.0 API to the DirectX 9 API(Window/Linux/MAC).
 */

#include <stdlib.h>    // NOLINT
#include <stdio.h>     // NOLINT
#include <GLES2/gl2.h> // NOLINT

#include "rt_header.h"       // NOLINT
#include "rt_string_utils.h" // NOLINT

#include "include/RTGLView.h"           // NOLINT
#include "GLRender/RTSpriteVideo.h" // NOLINT
#include "GLRender/RTDirector.h"    // NOLINT
#include "GLRender/RTScene.h"       // NOLINT
#include "GLRender/RTSprite.h"      // NOLINT

RTGLView::RTGLView() {
    mEglCtx = rt_malloc(EGLWindowCtx);
}

RTGLView::~RTGLView() {
    rt_safe_free(mEglCtx);
}

void RTGLView::initialize(EGLNativeWindowType wndID, INT32 width, INT32 height) {
    if (RT_NULL != mEglCtx) {
        mEglCtx->mWnd    = wndID;
        mEglCtx->mWidth  = width;
        mEglCtx->mHeight = height;
        egl_window_initialize(mEglCtx);
    }
}

void RTGLView::destroy() {
    if (RT_NULL != mEglCtx) {
        egl_window_destroy(mEglCtx);
    }
}

void RTGLView::swapBuffer() {
    // swap front and back Surface
    if (RT_NULL != mEglCtx) {
        eglSwapBuffers(mEglCtx->mDisplay, mEglCtx->mSurface);
    }
}

void RTGLView::toString(char* buffer) {
    rt_str_snprintf(buffer, MAX_NAME_LEN, "%s", "RTObject/RTGLView");  // NOLINT
}

void RTGLView::summary(char* buffer) {
    toString(buffer);
}

// @private function
void opengles_20_init(EGLWindowCtx* egl_ctx) {
    EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,

        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_NONE
    };
    EGLint format(0);
    EGLint numConfigs(0);
    EGLint major;
    EGLint minor;

    //! 1
    egl_ctx->mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    // egl_ctx->mDisplay = eglGetDisplay(GetDC(egl_ctx->mWnd));
    eglInitialize(egl_ctx->mDisplay, &major, &minor);
    RT_LOGE("eglGetDisplay display=%p", egl_ctx->mDisplay);

    //! 2
    eglChooseConfig(egl_ctx->mDisplay, attribs, &egl_ctx->mConfig, 1, &numConfigs);
    eglGetConfigAttrib(egl_ctx->mDisplay,
                       egl_ctx->mConfig,
                       EGL_NATIVE_VISUAL_ID, &format);

    //! 3
    egl_ctx->mSurface = eglCreateWindowSurface(egl_ctx->mDisplay,
                                               egl_ctx->mConfig,
                                               egl_ctx->mWnd, NULL);
    RT_LOGE("eglCreateWindowSurface surface=%p", egl_ctx->mSurface);

    //! 4
    EGLint attr[]     = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
    egl_ctx->mContext = eglCreateContext(egl_ctx->mDisplay,
                                         egl_ctx->mConfig, 0, attr);
    RT_LOGE("eglCreateContext context=%p", egl_ctx->mContext);

    //! 5
    if (eglMakeCurrent(egl_ctx->mDisplay, egl_ctx->mSurface,
                       egl_ctx->mSurface, egl_ctx->mContext) == EGL_FALSE) {
        RT_LOGE("Fail to eglMakeCurrent(display=%p, surface=%p)",
                         egl_ctx->mDisplay, egl_ctx->mSurface);
        return;
    }

    eglQuerySurface(egl_ctx->mDisplay, egl_ctx->mSurface, EGL_WIDTH,  &egl_ctx->mWidth);
    eglQuerySurface(egl_ctx->mDisplay, egl_ctx->mSurface, EGL_HEIGHT, &egl_ctx->mHeight);
    RT_LOGE("eglQuerySurface width=%d, height=%d",
            egl_ctx->mWidth, egl_ctx->mHeight);
}

// @private function
void opengles_20_destroy(EGLWindowCtx* egl_ctx) {
    if (egl_ctx->mSurface != EGL_NO_SURFACE) {
        RT_ASSERT(egl_ctx->mDisplay != EGL_NO_DISPLAY);
        eglDestroySurface(egl_ctx->mDisplay, egl_ctx->mSurface);
        egl_ctx->mSurface = EGL_NO_SURFACE;
        RT_LOGE("@DONE eglDestroySurface");
    }

    if (egl_ctx->mContext != EGL_NO_CONTEXT) {
        RT_ASSERT(egl_ctx->mDisplay != EGL_NO_DISPLAY);
        eglDestroyContext(egl_ctx->mDisplay, egl_ctx->mContext);
        egl_ctx->mContext = EGL_NO_CONTEXT;
        RT_LOGE("@DONE eglDestroyContext");
    }

    if (egl_ctx->mDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(egl_ctx->mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglTerminate(egl_ctx->mDisplay);
        egl_ctx->mDisplay = EGL_NO_DISPLAY;
        RT_LOGE("@DONE eglTerminate");
    }
}

// interface for managing EGL-Window
void egl_window_initialize(EGLWindowCtx* egl_ctx) {
    egl_ctx->mDisplay = EGL_NO_DISPLAY;
    egl_ctx->mSurface = EGL_NO_SURFACE;
    egl_ctx->mContext = EGL_NO_CONTEXT;

    // create os-window and OpenGLES-2.0 environment
    opengles_20_init(egl_ctx);
}

void egl_window_destroy(EGLWindowCtx* egl_ctx) {
    opengles_20_destroy(egl_ctx);
}
