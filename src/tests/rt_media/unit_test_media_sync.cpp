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
 */

#include "RTMediaSync.h"
#include "rt_header.h"          // NOLINT
#include "rt_time.h"            // NOLINT
#include "rt_thread.h"          // NOLINT

void FakeDisplay(void* target, uint64_t time) {
    uint64_t second = time/1000000;
    second = second%3600;
    uint64_t millisec = (time%1000000)/1000;
    RT_LOGE("call, FakeDisplay(%p, %llu:%llums)", target, second, millisec);
    RtTime::sleepUs(RtTime::randInt()%2000);
}

#define MAX_TEST_COUNT 10000
#define MAX_INTERVAL 50000
static int schedule_count = 0;
void *display_update_loop(void* target) {
    RTMediaSync *msync = reinterpret_cast<RTMediaSync *>(target);
    while (schedule_count < MAX_TEST_COUNT) {
        msync->getSync()->schedule();
        schedule_count++;
        RtTime::sleepUs(MAX_INTERVAL/2);
    }
    return RT_NULL;
}

RT_RET unit_test_media_sync(INT32 index, INT32 total_index) {
    RTMediaSync *msync = new RTMediaSync();
    if (RT_NULL != msync->getSync()) {
        msync->getSync()->setInterval(500*1000);
        msync->getSync()->setTarget(msync, FakeDisplay);
    }
    RtThread*  display = new RtThread(display_update_loop, msync);
    if (RT_NULL != display) {
        display->setName("mediasync");
        display->start();
    }
    RtTime::sleepUs(MAX_INTERVAL);
    rt_safe_delete(display);
    rt_safe_delete(msync);
    return RT_OK;
}
