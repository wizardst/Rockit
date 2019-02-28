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

#ifndef SRC_RT_MEDIA_INCLUDE_RTGLAPP_H_
#define SRC_RT_MEDIA_INCLUDE_RTGLAPP_H_

#include "rt_header.h"   // NOLINT
#include "RTObject.h"    // NOLINT
#include "../GLRender/RTScheduler.h" // NOLINT

class RTGLView;
class RTGLVideoScene;
class RTGLApp : public RTObject {
 public:
    RTGLApp();
    virtual ~RTGLApp();

    void createWindow(const char* title, INT32 width, INT32 height);
    void runtime();
    void eventLoop();
    void setVideoScheduler(SchedulerFunc callback, void* target);
    void updateFrame(unsigned char* frame, INT32 width, INT32 height);

    // override pure virtual methods of RTObject class
    virtual void summary(INT32 fd) {}
    virtual const char* getName() { return "RTObject/RTGLApp"; }

 private:
    RTGLView*        mGLView;
    RTGLVideoScene*  mScene;
    void*            mWndPtr;
};

void unit_test_win32_gles_app();

#endif  // SRC_RT_MEDIA_INCLUDE_RTGLAPP_H_
