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
 *   date: 20181126
 */

#ifndef SRC_RT_BASE_INCLUDE_RT_STRING_UTILS_H_
#define SRC_RT_BASE_INCLUDE_RT_STRING_UTILS_H_

#include "rt_header.h" // NOLINT

#include <stdio.h>  // NOLINT
#include <stdarg.h> // NOLINT
#include <string>   // NOLINT

#ifdef __cplusplus
extern "C" {
#endif

INT32 rt_str_vsnprintf(char* buffer, size_t max_len,
                             const char* format, va_list args);
INT32 rt_str_snprintf(char* buffer, size_t max_len,
                            const char* format, ...);
std::string rt_str_vsprintf(const char* format, va_list args);
std::string rt_str_sprintf(const char* format, ...);
const char* rt_str_to_char(const std::string buf);

#ifdef __cplusplus
}
#endif

#endif  // SRC_RT_BASE_INCLUDE_RT_STRING_UTILS_H_
