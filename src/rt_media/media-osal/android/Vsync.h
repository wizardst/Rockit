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
#include <utils/Log.h>
#include <utils/threads.h>
#include <utils/Vector.h>

#include <gui/DisplayEventReceiver.h>
#include <semaphore.h>
#include <inttypes.h>

typedef void* (*CallBackFun)(void *);

using namespace android;   // NOLINT

class Vsync : public RefBase {
 public:
    explicit Vsync(CallBackFun callback);  // init callback function here
    void* onSyncEvent(void* ptr_node);
    CallBackFun mCallBack;
    static int receiver(int fd, int events, void* data);
    int FrameAlign(int64_t count, float rate);
    void computeFrameAbandonNum(float hdmirate);
    bool mVsyncState;
    DisplayEventReceiver myDisplayEvent;
    static int64_t mLastVsyncTime;
    static int64_t phase;
    int64_t mVsyncIntervalTimeUs;
    int vsync_rate;
    ~Vsync();

 private:
    // sp<Looper> mLooper;
};

#endif  // SRC_RT_MEDIA_MEDIA_OSAL_ANDROID_VSYNC_H_
