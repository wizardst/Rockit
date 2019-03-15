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
 *   date: 2019/02/26
 * module: RTVideoSink
 */

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cutils/log.h>
#include <binder/IPCThreadState.h>

#include <gui/IGraphicBufferProducer.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/ISurfaceComposer.h>

#include "RTVideoSink.h"
#include "android/RTAWindowSink.h"

#define LOG_TAG "DisplaySink"

#define DEBUG_FLAG   0x1
#define RT_LOGD_IF(condition, format, ...) if (condition > 0) ALOGD(format, ##__VA_ARGS__)
#define RT_LOGE_IF(condition, format, ...) if (condition > 0) ALOGE(format, ##__VA_ARGS__)

using namespace android;   // NOLINT

void testcase_display_sink(int dst_w, int dst_h) {
    int             maxBufCount = 20;
    RTVideoSink*    pARender    = NULL;

    sp<IGraphicBufferProducer> pBufferProducer;
    sp<SurfaceComposerClient>  pComposerClient;
    sp<SurfaceControl>         pSurfaceControl;
    sp<Surface>                pSurface;

    pComposerClient = new SurfaceComposerClient;
    pComposerClient->initCheck();

    pSurfaceControl = pComposerClient->createSurface(String8("DisplaySink"),
                                             dst_w, dst_h, PIXEL_FORMAT_RGBA_8888, 0);
    SurfaceComposerClient::openGlobalTransaction();
    pSurfaceControl->setLayer(0x20000000);
    pSurfaceControl->setPosition(0, 0);
    pSurfaceControl->setSize(dst_w, dst_h);
    pSurfaceControl->show();
    SurfaceComposerClient::closeGlobalTransaction();
    pSurface = pSurfaceControl->getSurface();

    RT_LOGD_IF(DEBUG_FLAG, "\r\n =============================================");
    if (pSurface == NULL) {
        ALOGE("Fail to create native Surface");
        return ;
    }
    pBufferProducer = pSurface->getIGraphicBufferProducer();
    RT_LOGD_IF(DEBUG_FLAG, "%24s: pSurfaceControl = %p", __FUNCTION__, pSurfaceControl.get());
    RT_LOGD_IF(DEBUG_FLAG, "%24s: surface         = %p dimens(%dx%d)",
                              __FUNCTION__, pSurface.get(), dst_w, dst_h);
    RT_LOGD_IF(DEBUG_FLAG, "%24s: pBufferProducer = %p", __FUNCTION__, pBufferProducer.get());

    pARender   = new RTAWindowSink();
    RT_LOGD_IF(DEBUG_FLAG, "%24s: pARender        = %p", __FUNCTION__, pARender);
    if (NULL != pARender) {
        RTDisplaySetting renderSetting;
        memset(&renderSetting, 0, sizeof(RTDisplaySetting));
        renderSetting.mFrameWidth   = 1920;
        renderSetting.mFrameHeight  = 1080;
        renderSetting.mMaxBufCount  = maxBufCount;
        renderSetting.mHandler      = pBufferProducer.get();
        if (-1 != pARender->initRender(&renderSetting)) {
            pARender->allocRenderBuffers();
            pARender->renderBlank();
            pARender->releaseRender();
        }
        delete pARender;
        pARender        = NULL;
        pBufferProducer = NULL;
    }
    RT_LOGD_IF(DEBUG_FLAG, "=============================================");
}

int main(int , const char * argv[]) {
    // init runtime
    android::ProcessState::self()->startThreadPool();

    // display sink testcase
    for (int idx = 0; idx < 1; idx++) {
        testcase_display_sink(1920, 1080);
        usleep(50*1000);
    }

    // release runtime
    // IPCThreadState::self()->shutdown();
    // IPCThreadState::self()->joinThreadPool();
    IPCThreadState::self()->stopProcess(true);
    IPCThreadState::self()->joinThreadPool();
    return 0;
}
