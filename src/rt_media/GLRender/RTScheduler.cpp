/*
 * Copyright 2018 Rockchip Electronics Co-> LTD
 *
 * Licensed under the Apache License, Version 2->0 (the "License");
 * you may not use this file except in compliance with the License->
 * You may obtain a copy of the License at
 *
 *      http://www->apache->org/licenses/LICENSE-2->0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied->
 * See the License for the specific language governing permissions and
 * limitations under the License->
 *
 * author: martin->cheng@rock-chips->com
 *   date: 20181219
 * module: render engine for OpenGL-ES2.0
 */

#include "RTScheduler.h"     // NOLINT
#include "rt_string_utils.h" // NOLINT

RTTimer::RTTimer() {
    mScheduler  = RT_NULL;
    mElapsed    = -1.0f;
    mDelay      = 0.0f;
    mInterval   = 0.0f;
    mExecutedTimes = mTargetTimes = 0;
}

void RTTimer::setupTimer(RT_FLOAT interval, UINT32 repeat, RT_FLOAT delay) {
    setInterval(interval);
    mDelay = delay;
    mTargetTimes = repeat;
}

void RTTimer::update(RT_FLOAT dt) {
    if (mElapsed < 0.0f) {
        mElapsed       = 0.0f;
        mExecutedTimes = 0;
    }
    mElapsed += dt;
    if (mDelay > 0.0f) {
        if (mElapsed >= mDelay) {
            this->trigger();
            mExecutedTimes++;
            mElapsed = mDelay = 0.0f;
        }
    } else {
        if (mElapsed >= mInterval) {
            this->trigger();
            mElapsed = 0.0f;
            mExecutedTimes += 1;
        }
    }

    if (mTargetTimes == RT_REPEAT_FOREVER) {
        mExecutedTimes = 0;
    }
    if (mExecutedTimes > mTargetTimes) {
        this->cancel();
    }
}

/*
 * RenderSelector Implementation
 */
RenderSelector::RenderSelector() {
    mScheduler     = RT_NULL;
    mSchedulerFunc = RT_NULL;
    mTarget        = RT_NULL;
}

RenderSelector::~RenderSelector() {
    // @TODO(martin)
}

void RenderSelector::setTarget(RTScheduler* scheduler, SchedulerFunc callback, void* target) {
    mScheduler     = scheduler;
    mSchedulerFunc = callback;
    mTarget        = target;
}

void RenderSelector::trigger() {
    if (mSchedulerFunc) {
        mSchedulerFunc(mTarget, mElapsed);
    }
}

void RenderSelector::cancel() {
    if ((RT_NULL != mScheduler)&&(RT_NULL != mTarget)) {
        mScheduler->unschedule(mTarget);
    }
}

void RenderSelector::toString(char* buffer) {
    rt_str_snprintf(buffer, MAX_NAME_LEN, "%s", "RTTimer/RenderSelector"); // NOLINT
}

void RenderSelector::summary(char* buffer) {
    this->toString(buffer);
}

/*
 * RTScheduler Implementation
 */
RTScheduler::RTScheduler() {
    mSelectors = array_list_create();
}

RTScheduler::~RTScheduler() {
    array_list_destroy(mSelectors);
    mSelectors = RT_NULL;
}

void RTScheduler::update(RT_FLOAT dt) {
    for (UINT32 idx = 0; idx < array_list_get_size(mSelectors); idx++) {
        void* _selector = array_list_get_data(mSelectors, idx);
        if (RT_NULL != _selector) {
            RenderSelector* _render = reinterpret_cast<RenderSelector*>(_selector);
            _render->update(dt);
        }
    }
}

void RTScheduler::schedule(SchedulerFunc callback, void *target, RT_FLOAT interval, RT_BOOL paused) {
    schedule(callback, target, interval, 0, 0.0f, paused);
}

void RTScheduler::schedule(SchedulerFunc callback, void *target, RT_FLOAT interval,
                        UINT32 repeat, RT_FLOAT delay, RT_BOOL paused) {
    RenderSelector* selector = new RenderSelector();
    if (RT_NULL != selector) {
        selector->setupTimer(interval, repeat, delay);
        selector->setTarget(this, callback, target);
        array_list_add(mSelectors, reinterpret_cast<void*>(selector));
    }
}

void RTScheduler::schedulePerFrame(SchedulerFunc callback, void *target,
                                   RT_FLOAT interval, RT_BOOL paused) {
    UINT32 repeat = RT_REPEAT_FOREVER;
    schedule(callback, target, interval, repeat, 0.0f, paused);
}

void RTScheduler::unschedule(void *target) {
    for (UINT32 idx = 0; idx < array_list_get_size(mSelectors); idx++) {
        void* _selector = array_list_get_data(mSelectors, idx);
        if (RT_NULL != _selector) {
            array_list_remove(mSelectors, _selector);
            RenderSelector* _render = reinterpret_cast<RenderSelector*>(_selector);
            rt_safe_delete(_render);
        }
    }
}

void RTScheduler::toString(char* buffer) {
    rt_str_snprintf(buffer, MAX_NAME_LEN, "%s", "RTTimer/RTScheduler");  // NOLINT
}

void RTScheduler::summary(char* buffer) {
    this->toString(buffer);
}
