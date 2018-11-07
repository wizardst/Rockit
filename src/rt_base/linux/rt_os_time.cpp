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

#ifdef OS_LINUX
#include <sys/time.h>
#include <time.h>
#include <unistd.h> 

#include "rt_time.h"

void RtTime::getDateTime(DateTime* dt)
{
    if (dt)
    {
        time_t m_time;
        time(&m_time);
        struct tm* tstruct;
        tstruct = localtime(&m_time);

        dt->mYear       = RtToU16(tstruct->tm_year);
        dt->mMonth      = RtToU8(tstruct->tm_mon + 1);
        dt->mDayOfWeek  = RtToU8(tstruct->tm_wday);
        dt->mDay        = RtToU8(tstruct->tm_mday);
        dt->mHour       = RtToU8(tstruct->tm_hour);
        dt->mMinute     = RtToU8(tstruct->tm_min);
        dt->mSecond     = RtToU8(tstruct->tm_sec);
    }
}

UINT64 RtTime::getNowTimeMs()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (UINT64) (tv.tv_sec * 1000 + tv.tv_usec / 1000 );  /* milliseconds */
}

UINT64 RtTime::getNowTimeUs() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (UINT64) (tv.tv_sec * 1000000 + tv.tv_usec); /* microseconds */
}

void RtTime::sleepMs(UINT64 time) {
    usleep(time*1000);
}
void RtTime::sleepUs(UINT64 time) {
    usleep(time);
}

#endif
