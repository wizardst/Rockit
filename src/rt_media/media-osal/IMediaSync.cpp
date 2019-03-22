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
 * module: IMediaSync&RTTimerSync
 */

#include "rt_time.h"     // NOLINT
#include "IMediaSync.h"  // NOLINT



void IMediaSync::setTarget(void* target, RTSyncCallback callback) {
    mCallback = callback;
    mTarget   = target;
    mLastSchedule = RtTime::getNowTimeUs();
}

RTTimerSync::RTTimerSync() {
    mCallback = NULL;
    mTarget   = NULL;
    mInterval     = 0;
    mLastSchedule = 0;
    mLastSchedule = RtTime::getNowTimeUs();
}

RTTimerSync::~RTTimerSync() {
    cancel();
}

void RTTimerSync::schedule() {
    uint64_t now = RtTime::getNowTimeUs();
    if ((now - mLastSchedule) > mInterval) {
        if ((NULL != mCallback) && (NULL != mTarget)) {
            mCallback(mTarget, now);
        }
        mLastSchedule = now;
    }
}

void RTTimerSync::cancel() {
    mCallback = NULL;
    mTarget   = NULL;
}
