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
 *   date: 2019/03/21
 * module: RTMediaSync
 */

#include <inttypes.h>      // NOLINT

#include "RTMediaSync.h"   // NOLINT
#include "IMediaSync.h"    // NOLINT
#include "rt_mem.h"        // NOLINT
#include "android/Vsync.h"

RTMediaSync::RTMediaSync() {
#if OS_ANDROID
    mSync = new Vsync();
#else
    mSync = new RTTimerSync();
#endif
}

RTMediaSync::~RTMediaSync() {
    rt_safe_delete(mSync);
}
