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
 *   date: 2019/02/18
 * module: RTAWindowSink
 */

#ifndef SRC_RT_MEDIA_MEDIA_OSAL_ANDROID_RTAWINDOWSINK_H_
#define SRC_RT_MEDIA_MEDIA_OSAL_ANDROID_RTAWINDOWSINK_H_

class RTAWindowSink : public RTVideoSink {
 public:
    RTAWindowSink();
    ~RTAWindowSink();

 public:
    int32_t initRender(RTDisplaySetting* setting);
    int32_t allocRenderBuffers();
    int32_t releaseRender();
    int32_t setBufferListener(RTBufferListener* listener);
    int32_t renderFrame(RTBufferInfo* pInfo, int64_t timestamp);
    int32_t renderBlank();

 private:
    int32_t renderHWFrame(RTBufferInfo* pInfo, int64_t timestamp);
    int32_t renderSWFrame(RTBufferInfo* pInfo, int64_t timestamp);
    int32_t dequeBuffer(RTBufferInfo* pInfo);
    int32_t queueBuffer(RTBufferInfo* pInfo);
    int32_t renderHWBlank();
    int32_t renderSWBlank();
};

void* createNativeSurface(RTDisplaySetting* setting);

#endif  // SRC_RT_MEDIA_MEDIA_OSAL_ANDROID_RTAWINDOWSINK_H_
