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
#include <stdio.h>
#include <stdarg.h>
#include "../rt_os_log.h"
#ifdef OS_ANDROID
#include <android/log.h>
#endif
#define LINE_SZ 1024

void rt_os_log(const char* tag, const char* msg, va_list list) {
    char line[LINE_SZ] = {0};
    snprintf(line, sizeof(line), "%-16.16s: %s", tag, msg);
    vfprintf(stdout, line, list);
#ifdef OS_ANDROID
    __android_log_vprint(ANDROID_LOG_INFO, tag, msg, list);

#endif
}

void rt_os_err(const char* tag, const char* msg, va_list list) {
    char line[LINE_SZ] = {0};
    snprintf(line, sizeof(line), "%-16.16s: %s", tag, msg);
    vfprintf(stderr, line, list);
#ifdef OS_ANDROID
    __android_log_vprint(ANDROID_LOG_ERROR, tag, msg, list);
#endif
}

#endif
