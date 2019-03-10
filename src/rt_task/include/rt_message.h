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
 * author: Hery.Xu@rock-chips.com
 *   date: 20181102
 * author: martin.cheng@rock-chips.com
 *   date: 20181126
 */

#ifndef SRC_RT_TASK_INCLUDE_RT_MESSAGE_H_
#define SRC_RT_TASK_INCLUDE_RT_MESSAGE_H_

#include "rt_header.h" // NOLINT

#ifdef __cplusplus
extern "C" {
#endif

#if 0
enum RTMediaEvent {
    RT_MEDIA_NOP               = 0,  // interface test message
    RT_MEDIA_PREPARED          = 1,
    RT_MEDIA_PLAYBACK_COMPLETE = 2,
    RT_MEDIA_BUFFERING_UPDATE  = 3,
    RT_MEDIA_SEEK_COMPLETE     = 4,
    RT_MEDIA_SET_VIDEO_SIZE    = 5,
    RT_MEDIA_STARTED           = 6,
    RT_MEDIA_PAUSED            = 7,
    RT_MEDIA_STOPPED           = 8,
    RT_MEDIA_SKIPPED           = 9,
    RT_MEDIA_TIMED_TEXT        = 99,
    RT_MEDIA_ERROR             = 100,
    RT_MEDIA_INFO              = 200,
    RT_MEDIA_SUBTITLE_DATA     = 201,
};
#else
enum RTMediaEvent {
    RT_MEDIA_NOP               = 0,  // interface test message
    RT_MEDIA_PREPARED          = 1,
    RT_MEDIA_PLAYBACK_COMPLETE = 2,
    RT_MEDIA_BUFFERING_UPDATE  = 3,
    RT_MEDIA_SEEK_COMPLETE     = 4,
    RT_MEDIA_SET_VIDEO_SIZE    = 5,
    RT_MEDIA_STARTED           = 6,
    RT_MEDIA_PAUSED            = 7,
    RT_MEDIA_STOPPED           = 8,
    RT_MEDIA_SKIPPED           = 9,
    RT_MEDIA_TIMED_TEXT        = 10,
    RT_MEDIA_ERROR             = 11,
    RT_MEDIA_INFO              = 12,
    RT_MEDIA_SUBTITLE_DATA     = 13,
    RT_MEDIA_SEEK_ASYNC        = 14,
};

typedef struct _rt_media_event {
    UINT32      cmd;
    const char *name;
} rt_media_event;

static const rt_media_event mEventNames[] = {
    { RT_MEDIA_NOP,               "EVENT_NOP" },
    { RT_MEDIA_PREPARED,          "EVENT_PREPARED" },
    { RT_MEDIA_PLAYBACK_COMPLETE, "EVENT_COMPLETE" },
    { RT_MEDIA_BUFFERING_UPDATE,  "EVENT_BUFFERING_UPDATE" },
    { RT_MEDIA_SEEK_COMPLETE,     "EVENT_SEEK_COMPLETE" },
    { RT_MEDIA_SET_VIDEO_SIZE,    "EVENT_SET_VIDEO_SIZE" },
    { RT_MEDIA_STARTED,           "EVENT_STARTED" },
    { RT_MEDIA_PAUSED,            "EVENT_PAUSED" },
    { RT_MEDIA_STOPPED,           "EVENT_STOPPED" },
    { RT_MEDIA_SKIPPED,           "EVENT_SKIPPED" },
    { RT_MEDIA_TIMED_TEXT,        "EVENT_TIMED_TEXT" },
    { RT_MEDIA_ERROR,             "EVENT_ERROR" },
    { RT_MEDIA_INFO,              "EVENT_INFO" },
    { RT_MEDIA_SUBTITLE_DATA,     "SUBTITLE_DATA" },
    { RT_MEDIA_SEEK_ASYNC,        "SEEK_ASYNC" },
};
#endif

struct RTMsgHandler;
struct RTMsgLooper;

typedef int (*DoneListener)(void* looper, UINT32 what);

struct RTMessage {
    struct RTMsgData {
        INT32   mWhat;
        UINT64  mWhenUs;
        UINT32  mArgU32;
        UINT64  mArgU64;
        RT_PTR  mArgPtr;
    };

 public:
    RTMessage();
    RTMessage(UINT32 what, RT_PTR data);
    RTMessage(UINT32 what, UINT32 arg32, UINT64 arg64, struct RTMsgHandler* handler = RT_NULL);
    RTMessage(UINT32 what, RT_PTR data, struct RTMsgHandler* handler = RT_NULL);

    void           setWhat(UINT32 what);
    const UINT32   getWhat();
    void           setData(RT_PTR data);
    const RT_PTR   getData();
    void           setWhenUs(UINT64 when);
    const UINT64   getWhenUs();
    void           setTarget(struct RTMsgHandler* handler);
    struct RTMsgHandler *getTarget() {
        return mHandler;
    }
    RT_RET         post(INT64 delayUs = 0);
    RTMessage*     dup();      // performs message deep copy
    const char*    toString();

    struct RTMsgData mData;
    RT_BOOL          mSync;
    DoneListener     mDoneListener;

 private:
    friend struct    RTMsgLooper;  // deliver()

    struct RTMsgHandler* mHandler;

    RT_RET deliver();
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_RT_TASK_INCLUDE_RT_MESSAGE_H_
