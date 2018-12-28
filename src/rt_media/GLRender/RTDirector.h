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

#ifndef SRC_RT_MEDIA_GLRENDER_RTDIRECTOR_H_
#define SRC_RT_MEDIA_GLRENDER_RTDIRECTOR_H_

#include "rt_type.h"  // NOLINT

class RTSprite;
class RTActionManager;
class RTCamera;
class RTEventDispatcher;
class RTScheduler;
class RTScene;

class RTDirector {
 public:
    static RTDirector* getInstance();
    static void shutdown();
    void pause();
    void resume();
    void restart();
    void replaceScene(RTScene *scene);
    void startScene(RTScene *scene);
    void drawScene();
    void setAnimInterval(RT_FLOAT interval);
    void mainLoop();

    /** calculates delta time since last time it was called */
    void calculateDeltaTime();

    RTActionManager*   getActionManager() { return mActionManager; }
    RTEventDispatcher* getEventDispatcher() { return mEventDispatcher; }
    RTScheduler* getScheduler() { return mScheduler; }

 private:
    RTDirector();
    ~RTDirector();
    static RTDirector*  mInstance;

 protected:
    RTScheduler       *mScheduler;
    RTActionManager   *mActionManager;
    RTEventDispatcher *mEventDispatcher;
    RTScene           *mTaskScene;
    void initDefaultValues();

 private:
    /* delta time since last tick to main loop */
    RT_FLOAT mDeltaTime;
    RT_FLOAT mFrameRate;
    UINT32   mScreenWidth, mScreenHeight;
    RT_BOOL  mPaused;
    RT_FLOAT mAnimInterval, mLastInterval;
    UINT32   mFrameNum;
    UINT64   mLastTime;
};

#endif  // SRC_RT_MEDIA_GLRENDER_RTDIRECTOR_H_
