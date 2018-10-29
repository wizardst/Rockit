/*
 * Copyright 2018 The Rockit Open Source Project
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
 * author: martin.cheng
 *   date: 2018/07/19
 */

#include "rt_time.h"

#ifdef OS_LINUX
#include <sys/time.h>
#include <time.h>

void RtTime::getDateTime(DateTime* dt)
{
    if (dt)
    {
        time_t m_time;
        time(&m_time);
        struct tm* tstruct;
        tstruct = localtime(&m_time);

        dt->fYear       = RtToU16(tstruct->tm_year);
        dt->fMonth      = RtToU8(tstruct->tm_mon + 1);
        dt->fDayOfWeek  = RtToU8(tstruct->tm_wday);
        dt->fDay        = RtToU8(tstruct->tm_mday);
        dt->fHour       = RtToU8(tstruct->tm_hour);
        dt->fMinute     = RtToU8(tstruct->tm_min);
        dt->fSecond     = RtToU8(tstruct->tm_sec);
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

#endif
