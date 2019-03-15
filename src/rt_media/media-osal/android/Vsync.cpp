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

#include "Vsync.h"
#include <gui/DisplayEventReceiver.h>
#include <utils/Looper.h>
#include <sys/resource.h>
#include "android/looper.h"
#include <sys/ioctl.h>
#include <utils/Log.h>
#include "utils/CallStack.h"
#include <sys/prctl.h>

int64_t Vsync::phase = 0;  // only for test

int64_t Vsync::mLastVsyncTime = 0;  // only for test

int64_t GetNowUs() {
    return systemTime(SYSTEM_TIME_MONOTONIC) / 1000ll;
}

Vsync::Vsync(CallBackFun callback) {
    mCallBack = callback;
    mVsyncState = true ;
    phase = 0;
    mLastVsyncTime = 0;
    mVsyncIntervalTimeUs = 0;
    vsync_rate = 0;
    ALOGE("....Vsync() constructer %p ,%p...", callback, mCallBack);
}

void Vsync::computeFrameAbandonNum(float hdmirate) { //  TODO: temp use,to be added later
    ALOGE("Vsync::computeFrameAbandonNum enter...");
    if (hdmirate < 61.5 && hdmirate > 58) {
        vsync_rate = 60;
        } else if (hdmirate <= 51 && hdmirate >= 49) {
            vsync_rate = 50;
        } else if (hdmirate <= 31 && hdmirate >= 29) {
            vsync_rate = 30;

        } else if (hdmirate < 24.5 && hdmirate >= 23.5) {
            vsync_rate = 24;
        } else if (hdmirate <= 26 && hdmirate > 24.5) {
            vsync_rate = 25;
        }
}


int Vsync::FrameAlign(int64_t count, float rate) {
    ALOGE("Vsync::FrameAlign enter....");
    computeFrameAbandonNum(rate);
    if (mCallBack == NULL) {
        ALOGE("Vsync::FrameAlign mCallBack is NULL, return...");
        return -1;
    }
    mCallBack(NULL);
    return 0;
}





int Vsync::receiver(int fd, int events, void* data) {
    // ALOGE("Vsync::receiver enter ..%p",data);
    Vsync* mvsync = (Vsync*)data;
    if (mvsync == NULL) {
        ALOGE("Vsync::receiver NULL, return...");
        return NULL;
    }
    DisplayEventReceiver* q = &(mvsync->myDisplayEvent);
    if (q == NULL) {
        ALOGE("DisplayEventReceiver* q is NULL, return...");
        return NULL;
    }

    ssize_t n;
    DisplayEventReceiver::Event buffer[1];
    static nsecs_t oldTimeStamp = 0;
    while ((n = q->getEvents(buffer, 1)) > 0) {
        for (int i=0 ; i<n ; i++) {
            if (buffer[i].header.type == DisplayEventReceiver::DISPLAY_EVENT_VSYNC) {
                ALOGE("==============event vsync: count=%d\t", buffer[i].vsync.count);
            }
            if (mLastVsyncTime/*mvsync->mVsyncState*/) { ALOGE("Vsync::receiver framealign enter...");
                float t = float(buffer[i].header.timestamp - mLastVsyncTime) / s2ns(1);
                if (mvsync) {
                    phase = GetNowUs() - buffer[i].header.timestamp / 1000;
                    if ((float)(phase/1000.00f) < t*1000) {
                        mvsync->FrameAlign(buffer[i].vsync.count, 1.0/t);
                        mvsync->mVsyncIntervalTimeUs = (int64_t)(t*1000*1000);
                    }
                else
                    ALOGE("timeout [this=%p]%f ms (%f Hz), buffer[%d].count = %d phase %lld\n", mvsync, t*1000, 1.0/t, i, buffer[i].vsync.count, phase);
                }
            }
            else {
                usleep(5*1000);
            }
            mLastVsyncTime = buffer[i].header.timestamp;
            ALOGE("Vsync::receiver mLastVsyncTime is %lld now...", mLastVsyncTime);
        }
    }    ¡i
    if (n<0) {
        printf("error reading events (%s)\n", strerror(-n));
    }
    return 1;
}





void* Vsync::onSyncEvent(void* ptr_node) {
    if (!myDisplayEvent.getFd()) {
        ALOGD("Vsync::onSyncEvent getFd error...");
        return NULL;
    }
    prctl(PR_SET_NAME, (unsigned long)"Vsync", 0, 0, 0);
    sp<Looper> loop = new Looper(false);
    loop->addFd(myDisplayEvent.getFd(), 0, ALOOPER_EVENT_INPUT, Vsync::receiver, this);
    myDisplayEvent.setVsyncRate(1);
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
    }while (mVsyncState);

    if (loop.get()) {
        loop->removeFd(myDisplayEvent.getFd());
        loop.clear();
        loop = NULL;
    }
    ALOGE("Vsync onSyncEvent() start...");
    return 0;
}




Vsync::~Vsync() {
    mVsyncState = false;
    ALOGE("Vsync() destructer...");
}


