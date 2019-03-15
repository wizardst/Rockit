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
#define LOG_TAG "RTMediaSync"
#include "RTMediaSync.h" // NOLINT
#define USE_VSYNC_FLAG 1


static void * releaseRender() {   // Todo: common interface used for vsync or timer release
    /*	#if 0
    if(mRTNodeSinkAWindow != NULL)
    rt_safe_delete(mRTNodeSinkAWindow);
    #endif
    */
    ALOGE("releaseRender() quit...");
    return NULL;
}

void* RTMediaSync::onSyncEvent(void* ptr_node) {
    ALOGE("RTMediaSync::onSyncEvent enter...");
    if (mVsync != NULL)
        mVsync->onSyncEvent(ptr_node);
    else
        ALOGE("mVsync is NUll and onSyncEvent quit...");
    return 0;
}



RTMediaSync::RTMediaSync(CallBackFun callRender) {
    ALOGE("RTMediaSync() constructer now...");
    if (USE_VSYNC_FLAG) {
        mVsync = new Vsync(callRender);
    }
    else { //  TODO: add Timer callback here later
        ALOGE("choose another method for avsync...");
    }
}

RTMediaSync::~RTMediaSync() {
    if (mVsync) {
        delete(mVsync);
        mVsync = NULL;
    }
    releaseRender();
    ALOGE("RTMediaSync() destructer...");
}



