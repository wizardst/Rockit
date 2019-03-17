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

#ifndef SRC_RT_PLAYER_RTNDKMEDIADEF_H_
#define SRC_RT_PLAYER_RTNDKMEDIADEF_H_

#include "rt_type.h" // NOLINT

#define DEBUG_LEVEL_HIGH 1
#define DEBUG_LEVEL_LOW  0

enum RTMediaState {
    RT_STATE_ERROR              = 0,
    RT_STATE_IDLE               = 1 << 0,
    RT_STATE_INITIALIZED        = 1 << 1,
    RT_STATE_PREPARING          = 1 << 2,
    RT_STATE_PREPARED           = 1 << 3,
    RT_STATE_STARTED            = 1 << 4,
    RT_STATE_PAUSED             = 1 << 5,
    RT_STATE_STOPPED            = 1 << 6,
    RT_STATE_COMPLETE           = 1 << 7
};

enum RTMediaInfo {
    // 0xx
    RT_INFO_UNKNOWN = 1,
    // The player was started because it was used as the next player for another
    // player, which just completed playback
    RT_INFO_STARTED_AS_NEXT = 2,
    // The player just pushed the very first video frame for rendering
    RT_INFO_RENDERING_START = 3,

    // 7xx
    // The video is too complex for the decoder: it can't decode frames fast
    // enough. Possibly only the audio plays fine at this stage.
    RT_INFO_VIDEO_TRACK_LAGGING = 700,
    // pause playback internally in order to buffer more data.
    RT_INFO_BUFFERING_START = 701,
    // resume playback after filling enough buffers.
    RT_INFO_BUFFERING_END = 702,
    // Bandwidth in recent past
    RT_INFO_NETWORK_BANDWIDTH = 703,
    // buffering percent of demuxer
    RT_INFO_BUFFERING_PERCENT = 705,
    // loading percent of demuxer
    RT_INFO_LOADING_PERCENT = 706,

    // 8xx
    // Bad interleaving means that a media has been improperly interleaved or not
    // interleaved at all, e.g has all the video samples first then all the audio
    // ones. Video is playing but a lot of disk seek may be happening.
    RT_INFO_BAD_INTERLEAVING = 800,
    // The media is not seekable (e.g live stream).
    RT_INFO_NOT_SEEKABLE     = 801,
    // New media metadata is available.
    RT_INFO_METADATA_UPDATE  = 802,

    // 9xx
    RT_INFO_TIMED_TEXT_ERROR = 900,
    RT_INFO_PLAYING_START    = 901,
};

enum RTSeekType {
    RT_SEEK_NO         = 0,
    RT_SEEK_DOING,
    RT_SEEK_VIDEO_ONLY,
    RT_SEEK_DONE,
    RT_SEEK_MAX,
};

typedef struct _RTMediaUri {
    char   mUri[1024];
    char   mUserAgent[256];
    char   mVersion[32];
} RTMediaUri;


enum RTWriteDataType {
    RT_WRITEDATA_PCM = 0,
    RT_WRITEDATA_TS,
    RT_WRITEDATA_ES,
};

class RTStateUtil {
 public:
    static const char* getStateName(UINT32 player_state);
    static void  dumpStateError(UINT32 state, const char* caller);
};

enum RTLogLevel {
    RT_LOG_LEVEL_NO = 0,
    RT_LOG_LEVEL_FULL,
    RT_LOG_LEVEL_WARRING,
    RT_LOG_LEVEL_ERROR,
    RT_LOG_LEVEL_FETAL,
    RT_LOG_LEVEL_MAX,
};

struct RTMediaParams {
    UINT32 mCountPacket;
    UINT32 mCountFrame;
    UINT32 mCountDisplay;
    UINT32 mCountDeliver;
    UINT32 mDebugLevel;
};

class RTMediaDirector {
 public:
    RTMediaDirector();
    ~RTMediaDirector();
    void   snapshot();
    void   updatePacket(UINT32 delta = 1);
    void   updateFrame(UINT32 delta = 1);
    void   updateDisplay(UINT32 delta = 1);
    void   updateDelivery(UINT32 delta = 1);
    static void   setLogLevel(UINT32 level) { mLogLevel = level; }
    static UINT32 getLogLevel() { return mLogLevel; }

 private:
    RTMediaParams mParmasLast;
    RTMediaParams mParmasCurr;
    static UINT32 mLogLevel;
};

#endif  // SRC_RT_PLAYER_RTNDKMEDIADEF_H_
