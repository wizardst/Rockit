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

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RTSync"

#include <gui/DisplayEventReceiver.h>    // NOLINT
#include <sys/resource.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <utils/Log.h>
#include <utils/Looper.h>
#include <utils/threads.h>
#include <utils/Vector.h>
#include <semaphore.h>
#include <inttypes.h>

using namespace android;   // NOLINT

#include "Vsync.h"                       // NOLINT
#include "utils/CallStack.h"             // NOLINT
#include "android/looper.h"              // NOLINT


struct VSyncContext {
    bool mVsyncState;
    DisplayEventReceiver *myDisplayEvent;
    int64_t mLastVsyncTime;
    int64_t phase;
    int64_t mVsyncIntervalTimeUs;
    int vsync_rate;
};


int64_t GetNowUs() {
    return systemTime(SYSTEM_TIME_MONOTONIC) / 1000ll;
}

Vsync::Vsync() {
    mContext = (struct VSyncContext*)malloc(sizeof(struct VSyncContext));
    mContext->mVsyncState = true;
    mContext->phase = 0;
    mContext->myDisplayEvent = new DisplayEventReceiver;
    mContext->mLastVsyncTime = 0;
    mContext->mVsyncIntervalTimeUs = 0;
    mContext->vsync_rate = 0;
    mContext->mLastVsyncTime = 0;
    mContext->phase = 0;
    ALOGE("....Vsync() constructer...");
}

void Vsync::computeFrameAbandonNum(float hdmirate) {
    // TODO(princejay.dai): temp use,to be added later
    ALOGE("Vsync::computeFrameAbandonNum enter...");
    if (hdmirate < 61.5 && hdmirate > 58) {
        mContext->vsync_rate = 60;
        } else if (hdmirate <= 51 && hdmirate >= 49) {
            mContext->vsync_rate = 50;
        } else if (hdmirate <= 31 && hdmirate >= 29) {
            mContext->vsync_rate = 30;

        } else if (hdmirate < 24.5 && hdmirate >= 23.5) {
            mContext->vsync_rate = 24;
        } else if (hdmirate <= 26 && hdmirate > 24.5) {
            mContext->vsync_rate = 25;
        }
}


int Vsync::FrameAlign(int64_t count, float rate) {
    int64_t now = GetNowUs();
    ALOGE("Vsync::FrameAlign enter....");
    computeFrameAbandonNum(rate);
    mCallback(mTarget, now);
    return 0;
}

int Vsync::receiver(int fd, int events, void* data) {
    // ALOGE("Vsync::receiver enter ..%p",data);
    (void)fd;
    (void)events;
    Vsync* mvsync = reinterpret_cast<Vsync*>(data);
    if (mvsync == NULL) {
        ALOGE("Vsync::receiver NULL, return...");
        return NULL;
    }
    ssize_t n;
    DisplayEventReceiver::Event buffer[1];
    static nsecs_t oldTimeStamp = 0;
    while ((n = mvsync->mContext->myDisplayEvent->getEvents(buffer, 1)) > 0) {
        for (int i = 0; i < n; i++) {
            if (buffer[i].header.type == DisplayEventReceiver::DISPLAY_EVENT_VSYNC) {
                ALOGE("==============event vsync: count=%d\t", buffer[i].vsync.count);
            }
            if (mvsync->mContext->mLastVsyncTime/*mvsync->mVsyncState*/) {
                ALOGE("Vsync::receiver framealign enter...");
                float t = (float)(buffer[i].header.timestamp - mvsync->mContext->mLastVsyncTime) / s2ns(1);  // NOLINT
                if (mvsync) {
                    mvsync->mContext->phase = GetNowUs() - buffer[i].header.timestamp / 1000;
                    if ((float)(mvsync->mContext->phase/1000.00f) < t*1000) {  // NOLINT
                        mvsync->FrameAlign(buffer[i].vsync.count, 1.0/t);
                        mvsync->mContext->mVsyncIntervalTimeUs = (int64_t)(t*1000*1000);
                    } else {
                        ALOGE("timeout [this=%p]%f ms (%f Hz), buffer[%d].count = %d phase %lld\n", \
                               mvsync, t*1000, 1.0/t, i, buffer[i].vsync.count, mContext->phase);
                    }
                }
            } else {
                usleep(5*1000);
            }
            mvsync->mContext->mLastVsyncTime = buffer[i].header.timestamp;
            ALOGE("Vsync::receiver mLastVsyncTime is %lld now...", mvsync->mContext->mLastVsyncTime);
        }
    }
    if (n < 0) {
        printf("error reading events (%s)\n", strerror(-n));
    }
    return 1;
}

void Vsync::schedule() {
    if (!mContext->myDisplayEvent->getFd()) {
        ALOGD("Vsync::onSyncEvent getFd error...");
        return NULL;
    }
    prctl(PR_SET_NAME, (unsigned long)"Vsync", 0, 0, 0);  // NOLINT
    sp<Looper> loop = new Looper(false);
    loop->addFd(mContext->myDisplayEvent->getFd(), 0, ALOOPER_EVENT_INPUT, Vsync::receiver, this);
    mContext->myDisplayEvent->setVsyncRate(1);
    setpriority(PRIO_PROCESS, 0, ANDROID_PRIORITY_URGENT_DISPLAY);
    do {
        int32_t ret = loop->pollOnce(100);
        switch (ret) {
         case  ALOOPER_POLL_WAKE:
            printf("ALOOPER_POLL_WAKE\n");
            break;
         case  ALOOPER_POLL_CALLBACK:
            printf("ALOOPER_POLL_CALLBACK\n");
            break;
         case  ALOOPER_POLL_TIMEOUT:
            printf("ALOOPER_POLL_TIMEOUT\n");
            break;
         case  ALOOPER_POLL_ERROR:
            printf("ALOOPER_POLL_TIMEOUT\n");
            break;
         default:
            printf("ugh? poll returned %d\n", ret);
            break;
        }
    } while (mContext->mVsyncState);
    if (loop.get()) {
        loop->removeFd(mContext->myDisplayEvent->getFd());
        loop.clear();
        loop = NULL;
    }
    ALOGE("Vsync onSyncEvent() start...");
    return;
}

void Vsync::cancel() {
    if (NULL != mContext) {
        free(mContext);
        mContext = NULL;
    }
    mCallback = NULL;
    mTarget   = NULL;
}

Vsync::~Vsync() {
    mTarget   = NULL;
    mCallback = NULL;
    mContext->mVsyncState = false;
    if (mContext->myDisplayEvent != NULL) {
        delete(mContext->myDisplayEvent);
        mContext->myDisplayEvent = NULL;
    }
    ALOGE("Vsync() destructer...");
}


