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
 *   date: 2018/07/19
 */

#include "rt_time.h" // NOLINT
#include <windows.h>
#include <unistd.h>
#include <sys/time.h>

#ifdef OS_WINDOWS

void RtTime::getDateTime(DateTime* dt) {
    if (dt) {
        SYSTEMTIME      st;
        GetSystemTime(&st);

        dt->mYear       = RtToU16(st.wYear);
        dt->mMonth      = RtToU8(st.wMonth + 1);
        dt->mDayOfWeek  = RtToU8(st.wDayOfWeek);
        dt->mDay        = RtToU8(st.wDay);
        dt->mHour       = RtToU8(st.wHour);
        dt->mMinute     = RtToU8(st.wMinute);
        dt->mSecond     = RtToU8(st.wSecond);
    }
}

UINT64 RtTime::getNowTimeMs() {
    return getNowTimeUs()/1000;
}

UINT64 RtTime::getNowTimeUs() {
    LARGE_INTEGER curTime, freq;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&curTime);
    return (UINT64)((curTime.QuadPart * 1000000)/freq.QuadPart);
}

INT32 RtTime::randInt() {
    return rand();
}

void RtTime::sleepMs(UINT64 timeMs) {
#if 0
    struct timeval time;
    time.tv_sec  = timeMs/1000;
    time.tv_usec = (timeMs*1000)%1000000;
    select(0, NULL, NULL, NULL, &time);
#endif
    INT32 time = (0xFFFF & timeMs);
    Sleep(time);
}

void RtTime::sleepUs(UINT64 timeUs) {
    INT32 time = (0xFFFF & timeUs);
    Sleep(time/1000);
}

#endif

