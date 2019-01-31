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
 *   date: 20180704
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "rt_header.h" // NOLINT
#include "rt_os_log.h" // NOLINT

typedef void (*rt_log_callback)(const char*, const char*, va_list);

#ifdef __cplusplus
extern "C" {
#endif

static UINT32   rt_log_flag = 0;
static RtMutex* rt_log_lock = RT_NULL;

#define MAX_LINE_LEN 256

void rt_set_log_flag(UINT32 flag) {
    rt_log_flag = flag;
}

static void rt_log_full(rt_log_callback log_cb, const char *tag,
                        const char *fmt, const char *fname,
                        const UINT16 row, va_list args) {
    char line[MAX_LINE_LEN];
    snprintf(line, sizeof(line), "{%-18.18s:%03d} %s\r\n", fname, row, fmt);
    log_cb(tag, line, args);
}

void rt_log(const char *tag, const char *fmt, const char *fname,
                             const UINT16 row, ...) {
    if (RT_NULL == rt_log_lock) {
        rt_log_lock = new RtMutex();
    }
    RtAutoMutex autolock(rt_log_lock);
    va_list args;
    va_start(args, row);
    rt_log_full(rt_os_log, tag, fmt, fname, row, args);
    va_end(args);
}

void rt_err(const char *tag, const char *fmt, const char *fname,
                            const UINT16 row, ...) {
    if (RT_NULL == rt_log_lock) {
        rt_log_lock = new RtMutex();
    }
    RtAutoMutex autolock(rt_log_lock);
    va_list args;
    va_start(args, row);
    rt_log_full(rt_os_err, tag, fmt, fname, row, args);
    va_end(args);
}

#ifdef __cplusplus
}
#endif

