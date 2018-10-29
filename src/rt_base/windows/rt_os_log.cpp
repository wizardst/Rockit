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

#ifdef OS_WINDOWS
#include <stdio.h>
#include <stdarg.h>

#include "../rt_os_log.h"

void rt_os_log(const char* tag, const char* msg, va_list list)
{
    char line[MAX_LINE_LEN] = {0};
    _snprintf(line, sizeof(line), "%16s: %s", tag, msg);
    vfprintf(stdout, line, list);
}

void rt_os_err(const char* tag, const char* msg, va_list list)
{
    char line[MAX_LINE_LEN] = {0};
    _snprintf(line, sizeof(line), "%16s: %s", tag, msg);
    vfprintf(stderr, line, list);
}

#endif
