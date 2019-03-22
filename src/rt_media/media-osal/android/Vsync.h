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
 * author: princejay.dai@rock-chips.com
 *   date: 2019/03/12
 */

#ifndef SRC_RT_MEDIA_MEDIA_OSAL_ANDROID_VSYNC_H_
#define SRC_RT_MEDIA_MEDIA_OSAL_ANDROID_VSYNC_H_

#include "IMediaSync.h"

typedef void* (*CallBackFun)(void *);

struct VSyncContext;
class Vsync : public IMediaSync {
 public:
    Vsync();
    ~Vsync();
    void schedule();
    static int receiver(int fd, int events, void* data);
    int FrameAlign(int64_t count, float rate);
    void computeFrameAbandonNum(float hdmirate);
    void cancel();
 private:
     struct VSyncContext* mContext;
};

#endif  // SRC_RT_MEDIA_MEDIA_OSAL_ANDROID_VSYNC_H_
