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
 * Author: martin.cheng@rock-chips.com
 *   Date: 2019/03/10
 *   Task: build player streamline with node bus.
 */

#include <string.h>           // NOLINT
#include "rt_header.h"        // NOLINT
#include "RTNDKNodePlayer.h"  // NOLINT
#include "RTNDKMediaDef.h"    // NOLINT

UINT32 RTMediaDirector::mLogLevel = RT_LOG_LEVEL_FULL;

typedef struct _rt_media_state {
    UINT32      state;
    const char *name;
} rt_media_state;

#define RT_STATE_MAX  9
static const rt_media_state gStateNames[] = {
    { RT_STATE_ERROR,           "STATE_ERROR" },
    { RT_STATE_IDLE,            "STATE_IDLE" },
    { RT_STATE_INITIALIZED,     "STATE_INITIALIZED" },
    { RT_STATE_PREPARING,       "STATE_PREPARING" },
    { RT_STATE_PREPARED,        "STATE_PREPARED" },
    { RT_STATE_STARTED,         "STATE_STARTED" },
    { RT_STATE_PAUSED,          "STATE_PAUSED" },
    { RT_STATE_STOPPED,         "STATE_STOPPED" },
    { RT_STATE_COMPLETE,        "STATE_COMPLETE" },
};

const char* RTMediaUtil::getStateName(UINT32 state) {
    for (UINT32 idx = 0; idx < RT_STATE_MAX; idx++) {
        if (gStateNames[idx].state == state) {
            return gStateNames[idx].name;
        }
    }
    return "STATE_UNKOWN";
}

void RTMediaUtil::dumpStateError(UINT32 state, const char* caller) {
    switch (RTMediaDirector::getLogLevel()) {
      case RT_LOG_LEVEL_WARRING:
      case RT_LOG_LEVEL_ERROR:
      case RT_LOG_LEVEL_FETAL:
        RT_LOGE("fail to %s, invalid state(%d)", caller, state);
        break;
      case RT_LOG_LEVEL_FULL:
        RT_LOGE("fail to %s, invalid state(%s)", caller, getStateName(state));
        break;
      default:
        break;
    }
}

RTProtocolType RTMediaUtil::getMediaProtocol(const char* uri) {
    RTProtocolType protocol = RT_PROTOCOL_NONE;
    if ((RT_NULL == uri) || (strlen(uri) < 4)) {
        return protocol;
    }

    if (!strncasecmp("http://", uri, 7) || !strncasecmp("https://", uri, 8)) {
        RT_LOGD("uri is with https or http protocol");
        protocol = RT_PROTOCOL_HTTP;
    } else if (!strncasecmp("rtsp://", uri, 7) || !strncasecmp("rtmp://", uri, 7)) {
        RT_LOGD("uri is with rtsp or rtmp protocol");
        protocol = RT_PROTOCOL_RTSP;
    } else if (!strncasecmp("/data/smb/", uri, 10)) {
        RT_LOGD("uri is with /data/smb protocol");
        protocol = RT_PROTOCOL_SAMBA;
    } else if (!strncasecmp("/data/nfs/", uri, 10)) {
        RT_LOGD("uri is with /data/nfs protocol (signed as samba)");
        protocol = RT_PROTOCOL_SAMBA;
    } else if (strstr(uri, "m3u8")) {
        RT_LOGD("uri is with m3u8 protocol");
        protocol = RT_PROTOCOL_HLS;
    } else if (!strncasecmp("rtp:", uri, 4)) {
        RT_LOGD("uri is with rtp protocol");
        protocol = RT_PROTOCOL_RTP;
    } else if (!strncasecmp("udp:", uri, 4)) {
        RT_LOGD("uri is with udp protocol");
        protocol = RT_PROTOCOL_UDP;
    } else if (!strncasecmp("mms://", uri, 6)) {
        RT_LOGD("uri is with mms protocol");
        protocol = RT_PROTOCOL_MMS;
    } else if (!strncasecmp("mmsh://", uri, 7)) {
        RT_LOGD("uri is with mmsh protocol");
        protocol = RT_PROTOCOL_MMSH;
    } else if (!strncasecmp("mmst://", uri, 7)) {
        RT_LOGD("uri is with mmst protocol");
        protocol = RT_PROTOCOL_MMST;
    } else if (strstr(uri, "app_tts-cache")) {
        RT_LOGD("uri is with tts protocol");
        protocol = RT_PROTOCOL_TTS;
    }
    return protocol;
}

RTMediaDirector::RTMediaDirector() {
    rt_memset(&mParmasLast, 0, sizeof(RTMediaParams));
    rt_memset(&mParmasCurr, 0, sizeof(RTMediaParams));
}

RTMediaDirector::~RTMediaDirector() {
    rt_memset(&mParmasLast, 0, sizeof(RTMediaParams));
    rt_memset(&mParmasCurr, 0, sizeof(RTMediaParams));
}

void RTMediaDirector::snapshot() {
    if (mParmasCurr.mCountPacket == mParmasLast.mCountPacket) {
        RT_LOGE("fail, no media packet");
    }
    if (mParmasCurr.mCountFrame == mParmasLast.mCountFrame) {
        RT_LOGE("fail, no media frame");
    }
    if (mParmasCurr.mCountDisplay == mParmasLast.mCountDisplay) {
        RT_LOGE("fail, no frame displayed");
    }
    if (mParmasCurr.mCountDeliver == mParmasLast.mCountDeliver) {
        RT_LOGE("fail, no data delivered");
    }
    rt_memcpy(&mParmasLast, &mParmasCurr, sizeof(RTMediaParams));
}

void RTMediaDirector::updatePacket(UINT32 delta /* = 1*/) {
    mParmasCurr.mCountPacket += 1;
}

void RTMediaDirector::updateFrame(UINT32 delta /* = 1*/) {
    mParmasCurr.mCountFrame += 1;
}

void RTMediaDirector::updateDisplay(UINT32 delta /* = 1*/) {
    mParmasCurr.mCountDisplay += 1;
}

void RTMediaDirector::updateDelivery(UINT32 delta /* = 1*/) {
    mParmasCurr.mCountDeliver += 1;
}
