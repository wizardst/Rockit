/*
 * Copyright 2019 Rockchip Electronics Co. LTD
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
 *   date: 2019/03/21
 * module: RTVideoSink
 */

#ifndef SRC_RT_MEDIA_MEDIA_OSAL_IMEDIASYNC_H_
#define SRC_RT_MEDIA_MEDIA_OSAL_IMEDIASYNC_H_

#include <inttypes.h>

typedef void (*RTSyncCallback)(void* target, uint64_t time);

class IMediaSync {
 public:
    virtual ~IMediaSync() {}
    virtual void schedule() = 0;
    virtual void cancel()  = 0;

    /** set target and callback*/
    void    setTarget(void* target, RTSyncCallback callback);

    /** get interval in us */
    uint64_t getInterval() const { return mInterval; }

    /** set interval in us */
    void setInterval(uint64_t interval) { mInterval = interval; }

 protected:
    RTSyncCallback mCallback;
    void*          mTarget;
    uint64_t       mInterval;
    uint64_t       mLastSchedule;
};

class RTTimerSync : public IMediaSync {
 public:
    RTTimerSync();
    ~RTTimerSync();
    void schedule();
    void cancel();
};

#endif  // SRC_RT_MEDIA_MEDIA_OSAL_IMEDIASYNC_H_
