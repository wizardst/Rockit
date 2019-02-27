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

#ifndef SRC_RT_BASE_RT_OS_MEM_H_
#define SRC_RT_BASE_RT_OS_MEM_H_

#include "rt_type.h" // NOLINT

#define MEM_ALIGN               32

int  rt_os_malloc(void **memptr, size_t alignment, size_t size);
int  rt_os_realloc(void *src, void **dst, size_t alignment, size_t size);
void rt_os_free(void *ptr);

#endif  // SRC_RT_BASE_RT_OS_MEM_H_

