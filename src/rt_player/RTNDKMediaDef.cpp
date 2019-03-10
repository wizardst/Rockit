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

#include "rt_type.h"          // NOLINT
#include "RTNDKNodePlayer.h"  // NOLINT
#include "RTNDKMediaDef.h"    // NOLINT

UINT32 RTStateUtil::mLevel = DEBUG_LEVEL_HIGH;  // or DEBUG_LEVEL_LOW

typedef struct _rt_media_state {
    UINT32      state;
    const char *name;
} rt_media_state;

#define RT_STATE_MAX  9
static const rt_media_state gStateNames[] = {
    { RT_STATE_STATE_ERROR,     "STATE_ERROR" },
    { RT_STATE_IDLE,            "STATE_IDLE" },
    { RT_STATE_INITIALIZED,     "STATE_INITIALIZED" },
    { RT_STATE_PREPARING,       "STATE_PREPARING" },
    { RT_STATE_PREPARED,        "STATE_PREPARED" },
    { RT_STATE_STARTED,         "STATE_STARTED" },
    { RT_STATE_PAUSED,          "STATE_PAUSED" },
    { RT_STATE_STOPPED,         "STATE_STOPPED" },
    { RT_STATE_COMPLETE,        "STATE_COMPLETE" },
};

const char* RTStateUtil::getStateName(UINT32 state) {
    for (UINT32 idx = 0; idx < RT_STATE_MAX; idx++) {
        if (gStateNames[idx].state == state) {
            return gStateNames[idx].name;
        }
    }
    return "STATE_UNKOWN";
}

void RTStateUtil::dumpStateError(UINT32 state, const char* caller) {
    switch (mLevel) {
      case DEBUG_LEVEL_LOW:
        RT_LOGE("fail to %s, invalid state(%d)", caller, state);
        break;
      case DEBUG_LEVEL_HIGH:
        RT_LOGE("fail to %s, invalid state(%s)", caller, getStateName(state));
        break;
      default:
        break;
    }
}

void RTStateUtil::setDebugLevel(UINT32 level) {
    mLevel = DEBUG_LEVEL_LOW;
    if (level >= DEBUG_LEVEL_HIGH) {
        mLevel = DEBUG_LEVEL_HIGH;
    }
}
