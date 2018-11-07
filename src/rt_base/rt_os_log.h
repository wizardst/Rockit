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

#ifndef __RT_OS_LOG_H__
#define __RT_OS_LOG_H__

#define MAX_LINE_LEN 256

#ifdef __cplusplus
extern "C" {
#endif

void rt_os_log(const char* tag, const char* msg, va_list list);
void rt_os_err(const char* tag, const char* msg, va_list list);

#ifdef __cplusplus
}
#endif

#endif /*__RT_LOG_H__*/
