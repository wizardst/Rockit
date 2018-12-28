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
 * module: render engine for OpenGL-ES2.0
 */

#include "RTDirector.h"      // NOLINT
#include "RTScheduler.h"     // NOLINT
#include "RTActionManager.h" // NOLINT
#include "RTScene.h"         // NOLINT
#include "rt_mem.h"          // NOLINT

RTDirector* RTDirector::mInstance = RT_NULL;

RTDirector* RTDirector::getInstance() {
    if (RT_NULL == mInstance) {
        mInstance = new RTDirector();
        mInstance->initDefaultValues();
    }
    return mInstance;
}

RTDirector::RTDirector() {
    mScheduler       = RT_NULL;
    mActionManager   = RT_NULL;
    mEventDispatcher = RT_NULL;
    mTaskScene       = RT_NULL;
    mLastTime        = 0;
    mAnimInterval    = 1.0f/30.0f;
}

RTDirector::~RTDirector() {
    this->pause();
    rt_safe_delete(mScheduler);
    rt_safe_delete(mActionManager);
    rt_safe_delete(mTaskScene);
}

void RTDirector::pause() {
    if (mPaused) {
        return;
    }

    mLastInterval = mAnimInterval;

    // when paused, don't consume CPU
    setAnimInterval(1 / 4.0);
    mPaused = RT_TRUE;
}

void RTDirector::resume() {
    if (!mPaused) {
        return;
    }

    setAnimInterval(mLastInterval);

    mPaused    = RT_FALSE;
    mDeltaTime = 0;
}

void RTDirector::replaceScene(RTScene *scene) {
    mPaused = RT_FALSE;
    rt_safe_delete(mTaskScene);
    mTaskScene = scene;
}

void RTDirector::startScene(RTScene *scene) {
    replaceScene(scene);
}

void RTDirector::drawScene() {
    calculateDeltaTime();
    if (!mPaused) {
        mScheduler->update(mDeltaTime);
        #if TO_DO_FLAG
            mEventDispatcher->dispatchEvent(_eventAfterUpdate);
        #endif
    }
    if (RT_NULL != mTaskScene) {
        mTaskScene->render();
        #if TO_DO_FLAG
        mEventDispatcher->dispatchEvent(_eventAfterVisit);
        #endif
    }
    mFrameNum++;

    // swap buffers
}

void RTDirector::setAnimInterval(RT_FLOAT interval) {
    mAnimInterval = interval;
}

void RTDirector::mainLoop() {
    if (!mPaused) {
        drawScene();
    }
}

void RTDirector::shutdown() {
    if (RT_NULL != mInstance) {
        delete mInstance;
        mInstance = RT_NULL;
    }
}

void RTDirector::calculateDeltaTime() {
    UINT64 now = RtTime::getNowTimeUs();
    mDeltaTime = (now - mLastTime)/100000.0f;
    if (mDeltaTime > 0.1f) {
        mDeltaTime = mAnimInterval;
    }
    now = mLastTime;
}

void schedulerAMUpdate(void* target, RT_FLOAT dt) {
    RTActionManager* am = reinterpret_cast<RTActionManager*>(target);
    am->update(dt);
}

void RTDirector::initDefaultValues() {
    mEventDispatcher = RT_NULL;
    mTaskScene       = RT_NULL;
    // scheduler
    mScheduler = new RTScheduler();
    // action manager
    mActionManager   = new RTActionManager();
    mScheduler->schedulePerFrame(schedulerAMUpdate,
                                 reinterpret_cast<void*>(mActionManager),
                                 mAnimInterval,  false);
}
