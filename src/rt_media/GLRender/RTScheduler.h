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
 *   date: 20181221
 */

#ifndef SRC_RT_MEDIA_GLRENDER_RTSCHEDULER_H_
#define SRC_RT_MEDIA_GLRENDER_RTSCHEDULER_H_

#include "RTObject.h"      // NOLINT
#include "rt_type.h"       // NOLINT
#include "rt_array_list.h" // NOLINT

#define RT_UINT_MAX 0xffffffff
#define RT_REPEAT_FOREVER (UINT_MAX -1)

typedef void (*SchedulerFunc)(void*, RT_FLOAT);

class RTScheduler;
class RTTimer : public RTObject {
 protected:
    RTTimer();

 public:
    /** get interval in seconds */
    RT_FLOAT getInterval() const { return mInterval; }
    /** set interval in seconds */
    void setInterval(RT_FLOAT interval) { mInterval = interval; }

    void setupTimer(RT_FLOAT interval, UINT32 repeat, RT_FLOAT delay);

    virtual void trigger() = 0;
    virtual void cancel() = 0;

    /** triggers the timer */
    void update(RT_FLOAT dt);

 protected:
    RTScheduler*  mScheduler;

    RT_FLOAT mElapsed;
    RT_FLOAT mDelay;
    RT_FLOAT mInterval;
    UINT32   mExecutedTimes;
    UINT32   mTargetTimes;
};

class RenderSelector : public RTTimer {
 public:
    RenderSelector();
    ~RenderSelector();
    void setTarget(RTScheduler* scheduler, SchedulerFunc callback, void* target);

    virtual void trigger();
    virtual void cancel();
    virtual void toString(char* buffer);
    virtual void summary(char* buffer);
 protected:
    SchedulerFunc mSchedulerFunc;
    void* mTarget;
};

class RTScheduler : public RTObject {
 public:
    RTScheduler();
    ~RTScheduler();
 public:
    void update(RT_FLOAT dt);
    void schedule(SchedulerFunc callback, void *target, RT_FLOAT interval, RT_BOOL paused);
    void schedule(SchedulerFunc callback, void *target, RT_FLOAT interval,
                        UINT32 repeat, RT_FLOAT delay, RT_BOOL paused);
    void schedulePerFrame(SchedulerFunc callback, void *target, RT_FLOAT interval, RT_BOOL paused);
    // void schedule(SEL_SCHEDULE selector, void *target, RT_FLOAT interval, RT_BOOL paused);
    void unschedule(void *target);

    // override @RTObject methods
    void toString(char* buffer);
    void summary(char* buffer);
 private:
    RtArrayList* mSelectors;
};

#endif  // SRC_RT_MEDIA_GLRENDER_RTSCHEDULER_H_
